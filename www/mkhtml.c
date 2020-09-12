/*
 * This file belongs to FreeMiNT. It's not in the original MiNT 1.12
 * distribution. See the file CHANGES for a detailed log of changes.
 *
 *
 * Copyright 2003 Konrad M. Kokoszkiewicz <draco@atari.org>
 * All rights reserved.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * begin:	2003-02-25
 * last change:	2000-02-25
 *
 * Author:	Konrad M. Kokoszkiewicz <draco@atari.org>
 *          Thorsten Otto <admin@tho-otto.de>
 *
 * Please send suggestions, patches or bug reports to me or
 * the MiNT mailing list.
 *
 */

/* Make a keyboard translation table out of the given source file.
 *
 * The source file must consist of text lines. A text line must either
 * begin with a semicolon (;), or contain one of two directives:
 *
 * dc.b	- begins a sequence of bytes
 * dc.w - begins a sequence of words
 *
 * The data may be given as hex numbers (in either asm or C syntax),
 * dec numbers or ASCII characters. A hex number begins with $ or 0x,
 * (e.g. $2735 or 0x2735), an ASCII character is quoted (e.g. 'a'),
 * and a dec number has no prefix (e.g. 1 or 48736 is fine).
 *
 * If a number exceeds the desired limit, e.g. when you do
 *
 * dc.b 100000
 *
 * Only the lowest eight (or sixteen in case of dc.w) of such a
 * number will be taken into account, and a warning message is
 * printed.
 *
 * The data may be separated with commas or semicolons.
 *
 * Examples:
 *
 * ; This is a comment
 *
 *	dc.b 'a',$00,0x55,76
 *	dc.w $2334,4,0x12,'d'
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>


#define TAB_UNSHIFT   0
#define TAB_SHIFT     1
#define TAB_CAPS      2
#define TAB_ALTGR     3
#define TAB_SHALTGR   4
#define TAB_CAPSALTGR 5
#define TAB_ALT       6
#define TAB_SHALT     7
#define TAB_CAPSALT   8
#define N_KEYTBL      9
#define TAB_DEADKEYS  N_KEYTBL

#define MAX_SCANCODE 128
static unsigned char keytab[N_KEYTBL][MAX_SCANCODE];

#define MAX_DEADKEYS 2048
static unsigned char deadkeys[MAX_DEADKEYS];
static int tabsize[N_KEYTBL + 1];
static int copyfrom[N_KEYTBL];
static const char *progname;
static int codeset;

static const char *const labels[N_KEYTBL + 1] = {
	"tab_unshift:",
	"tab_shift:",
	"tab_caps:",
	"tab_altgr:",
	"tab_shaltgr:",
	"tab_capsaltgr:",
	"tab_alt:",
	"tab_shalt:",
	"tab_capsalt:",
	"tab_dead:"
};

static unsigned char const possible_dead_keys[] = {
	0x5e,  /* U+005E Circumflex accent */
	0x60,  /* U+0060 Grave accent */
	0x7e,  /* U+007E Tilde */
	0xf8,  /* U+00B0 Degree sign (latin-1) */
	0xb7,  /* U+02C7 Caron (latin-2) */
	0xa2,  /* U+02D8 Breve (latin-2) */
	0xb0,  /* U+00B0 Degree sign (latin-2) */
	0xb2,  /* U+02DB Ogonek (latin-2) */
	0xff,  /* U+02D9 Dot above (latin-2) */
	0xba,  /* U+00B4 Acute accent (latin-1) */
	0xb4,  /* U+00B4 Acute accent (latin-2) */
	0xbd,  /* U+02DD Double acute accent (latin-2) */
	0xb9,  /* U+00A8 Diaresis (latin-1) */
	0xa8,  /* U+00A8 Diaresis (latin-2) */
	0xb8,  /* U+00b8 Cedilla (latin-2) */
	0xb7,  /* U+00b7 Middle dot (latin-1) */
	       /* U+201a Single low-9 quotation mark */
	0
};

#define FORMAT_NONE  0
#define FORMAT_MAGIC 1
#define FORMAT_MINT  2

/* Own getdelim(). The `n' buffer size must definitely be bigger than 0!
 */
static int mktbl_getdelim(char **lineptr, size_t *n, FILE *stream)
{
	int ch;
	char *buf = *lineptr;
	size_t len = 0;

	while ((ch = fgetc(stream)) != EOF)
	{
		if ((len + 1) >= *n)
		{
			buf = realloc(buf, len + 256L);
			assert(buf);
			*n += 256L;
			*lineptr = buf;
		}

		if (ch == 0x0a)
			break;
		if (ch == 0x0d)
		{
			ch = fgetc(stream);
			if (ch != 0x0a && ch != EOF)
				ungetc(ch, stream);
			break;
		}
		buf[len++] = (char) ch;
	}

	buf[len] = 0;

	/* A problem here: returning -1 on EOF may cause data loss
	 * if there is no terminator character at the end of the file
	 * (in this case all the previously read characters of the
	 * line are discarded). At the other hand returning 0 at
	 * first EOF and -1 at the other creates an additional false
	 * empty line, if there *was* terminator character at the
	 * end of the file. So the check must be more extensive
	 * to behave correctly.
	 */
	if (ch == EOF)
	{
		if (len == 0)					/* Nothing read before EOF in this line */
			return -1;
		/* Pretend success otherwise */
	}

	return 0;
}

static unsigned char const atari_toupper[][256] = {
	{
	/* 0: atarist encoding */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,	/* 0x00-0x07 */
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,	/* 0x08-0x0f */
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	/* 0x10-0x17 */
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,	/* 0x18-0x1f */
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,	/* 0x20-0x27 */
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,	/* 0x28-0x2f */
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,	/* 0x30-0x37 */
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,	/* 0x38-0x3f */
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,	/* 0x40-0x47 */
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,	/* 0x48-0x4f */
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,	/* 0x50-0x57 */
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,	/* 0x58-0x5f */
	0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,	/* 0x60-0x67 */
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,	/* 0x68-0x6f */
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,	/* 0x70-0x77 */
	0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,	/* 0x78-0x7f */

	0x80, 0x9a, 0x90, 0x83, 0x8e, 0xb6, 0x8f, 0x80,	/* 0x80-0x87 */
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,	/* 0x88-0x8f */
	0x90, 0x92, 0x92, 0x93, 0x99, 0x95, 0x96, 0x97,	/* 0x90-0x97 */
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,	/* 0x98-0x9f */
	0xa0, 0xa1, 0xa2, 0xa3, 0xa5, 0xa5, 0xa6, 0xa7,	/* 0xa0-0xa7 */
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,	/* 0xa8-0xaf */
	0xb7, 0xb8, 0xb2, 0xb2, 0xb5, 0xb5, 0xb6, 0xb7,	/* 0xb0-0xb7 */
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,	/* 0xb8-0xbf */
	0xc1, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,	/* 0xc0-0xc7 */
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,	/* 0xc8-0xcf */
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,	/* 0xd0-0xd7 */
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,	/* 0xd8-0xdf */
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe4, 0xe6, 0xe7,	/* 0xe0-0xe7 */
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xe8, 0xee, 0xef,	/* 0xe8-0xef */
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,	/* 0xf0-0xf7 */
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff	/* 0xf8-0xff */
	}, {
	/* 1: iso-8859-2 / CP 1250 */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,	/* 0x00-0x07 */
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,	/* 0x08-0x0f */
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	/* 0x10-0x17 */
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,	/* 0x18-0x1f */
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,	/* 0x20-0x27 */
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,	/* 0x28-0x2f */
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,	/* 0x30-0x37 */
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,	/* 0x38-0x3f */
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,	/* 0x40-0x47 */
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,	/* 0x48-0x4f */
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,	/* 0x50-0x57 */
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,	/* 0x58-0x5f */
	0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,	/* 0x60-0x67 */
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,	/* 0x68-0x6f */
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,	/* 0x70-0x77 */
	0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,	/* 0x78-0x7f */

	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,	/* 0x80-0x87 */
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,	/* 0x88-0x8f */
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,	/* 0x90-0x97 */
	0x98, 0x99, 0x8a, 0x9b, 0x8c, 0x8d, 0x8e, 0x8f,	/* 0x98-0x9f */
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,	/* 0xa0-0xa7 */
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,	/* 0xa8-0xaf */
	0xb0, 0xb1, 0xb2, 0xa3, 0xb4, 0xb5, 0xb6, 0xb7,	/* 0xb0-0xb7 */
	0xb8, 0xa5, 0xaa, 0xbb, 0xbc, 0xbd, 0xbc, 0xaf,	/* 0xb8-0xbf */
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,	/* 0xc0-0xc7 */
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,	/* 0xc8-0xcf */
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,	/* 0xd0-0xd7 */
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,	/* 0xd8-0xdf */
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,	/* 0xe0-0xe7 */
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,	/* 0xe8-0xef */
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xf7,	/* 0xf0-0xf7 */
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xff	/* 0xf8-0xff */
	}
};

static unsigned short const atari_to_unicode[][256] = {
	{
	/* 0: atarist encoding */
	/* 00 */	0x0000, 0x21e7, 0x21e9, 0x21e8, 0x21e6, 0x2610, 0x2611, 0x2612,
	/* 08 */	0x2713, 0x0009, 0x237e, 0x266a, 0x240c, 0x240d, 0x240e, 0x240f,
	/* 10 */	0x24ea, 0x2460, 0x2461, 0x2462, 0x2463, 0x2464, 0x2465, 0x2466,
	/* 18 */	0x2467, 0x2468, 0x018f, 0x241b, 0x241c, 0x241d, 0x241e, 0x241f,
	/* 20 */	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
	/* 28 */	0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
	/* 30 */	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
	/* 38 */	0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
	/* 40 */	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
	/* 48 */	0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
	/* 50 */	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
	/* 58 */	0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
	/* 60 */	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
	/* 68 */	0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
	/* 70 */	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	/* 78 */	0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x2206,
	/* 80 */	0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7,
	/* 88 */	0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
	/* 90 */	0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9,
	/* 98 */	0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x00df, 0x0192,
	/* a0 */	0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba,
	/* a8 */	0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
	/* b0 */	0x00e3, 0x00f5, 0x00d8, 0x00f8, 0x0153, 0x0152, 0x00c0, 0x00c3,
	/* b8 */	0x00d5, 0x00a8, 0x00b4, 0x2020, 0x00b6, 0x00a9, 0x00ae, 0x2122,
	/* c0 */	0x0133, 0x0132, 0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5,
	/* c8 */	0x05d6, 0x05d7, 0x05d8, 0x05d9, 0x05db, 0x05dc, 0x05de, 0x05e0,
	/* d0 */	0x05e1, 0x05e2, 0x05e4, 0x05e6, 0x05e7, 0x05e8, 0x05e9, 0x05ea,
	/* d8 */	0x05df, 0x05da, 0x05dd, 0x05e3, 0x05e5, 0x00a7, 0x2227, 0x221e,
	/* e0 */	0x03b1, 0x03b2, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4,
	/* e8 */	0x03a6, 0x0398, 0x03a9, 0x03b4, 0x222e, 0x03c6, 0x2208, 0x2229,
	/* f0 */	0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248,
	/* f8 */	0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x00b3, 0x00af
	}, {
	/* 1: iso-8859-2 / CP 1250 */
	/* 00 */	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
	/* 08 */	0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
	/* 10 */	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
	/* 18 */	0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
	/* 20 */	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
	/* 28 */	0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
	/* 30 */	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
	/* 38 */	0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
	/* 40 */	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
	/* 48 */	0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
	/* 50 */	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
	/* 58 */	0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
	/* 60 */	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
	/* 68 */	0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
	/* 70 */	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	/* 78 */	0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
	/* 80 */	0x20ac, 0x0081, 0x201a, 0x0083, 0x201e, 0x2026, 0x2020, 0x2021,
	/* 88 */	0x0088, 0x2030, 0x0160, 0x2039, 0x015a, 0x0164, 0x017d, 0x0179,
	/* 90 */	0x0090, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
	/* 98 */	0x0098, 0x2122, 0x0161, 0x203a, 0x015b, 0x0165, 0x017e, 0x017a,
	/* a0 */	0x00a0, 0x02c7, 0x02d8, 0x0141, 0x00a4, 0x0104, 0x00a6, 0x00a7,
	/* a8 */	0x00a8, 0x00a9, 0x015e, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x017b,
	/* b0 */	0x00b0, 0x00b1, 0x02db, 0x0142, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
	/* b8 */	0x00b8, 0x0105, 0x015f, 0x00bb, 0x013d, 0x02dd, 0x013e, 0x017c,
	/* c0 */	0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7,
	/* c8 */	0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
	/* d0 */	0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7,
	/* d8 */	0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
	/* e0 */	0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7,
	/* e8 */	0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
	/* f0 */	0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7,
	/* f8 */	0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9
	}
};


static struct {
	unsigned char scancode;
	int w;
	int h;
	int x;
	int xkey;
	int xscan;
	int y;
	const char *keylabel;
} const svgtable[] = {
	{ 0x01, 37, 40,  10,  12,  15,  60, NULL },
	{ 0x02, 37, 40,  47,  49,  52,  60, NULL },
	{ 0x03, 37, 40,  84,  86,  89,  60, NULL },
	{ 0x04, 37, 40, 121, 123, 126,  60, NULL },
	{ 0x05, 37, 40, 158, 160, 163,  60, NULL },
	{ 0x06, 37, 40, 195, 197, 200,  60, NULL },
	{ 0x07, 37, 40, 232, 234, 237,  60, NULL },
	{ 0x08, 37, 40, 269, 271, 274,  60, NULL },
	{ 0x09, 37, 40, 306, 308, 311,  60, NULL },
	{ 0x0a, 37, 40, 343, 345, 348,  60, NULL },
	{ 0x0b, 37, 40, 380, 382, 385,  60, NULL },
	{ 0x0c, 37, 40, 417, 419, 422,  60, NULL },
	{ 0x0d, 37, 40, 454, 456, 459,  60, NULL },
	{ 0x29, 37, 40, 491, 493, 496,  60, NULL },
	{ 0x0e, 58, 40, 528, 530, 533,  60, NULL },
	{ 0x00,  0,  0,   0,   0,   0,   0, NULL },

	{ 0x0f, 55, 40,  10,  12,  24, 100, NULL },
	{ 0x10, 37, 40,  65,  67,  70, 100, NULL },
	{ 0x11, 37, 40, 102, 104, 107, 100, NULL },
	{ 0x12, 37, 40, 139, 141, 144, 100, NULL },
	{ 0x13, 37, 40, 176, 178, 181, 100, NULL },
	{ 0x14, 37, 40, 213, 215, 218, 100, NULL },
	{ 0x15, 37, 40, 250, 252, 255, 100, NULL },
	{ 0x16, 37, 40, 287, 289, 292, 100, NULL },
	{ 0x17, 37, 40, 324, 326, 329, 100, NULL },
	{ 0x18, 37, 40, 361, 363, 366, 100, NULL },
	{ 0x19, 37, 40, 398, 400, 403, 100, NULL },
	{ 0x1a, 37, 40, 435, 437, 440, 100, NULL },
	{ 0x1b, 37, 40, 472, 474, 477, 100, NULL },
	{ 0x1c,  0,  0, 509, 493, 514, 100, "Return" },
	{ 0x53, 40, 40, 546, 548, 551, 100, "Delete" },
	{ 0x00,  0,  0,   0,   0,   0,   0, NULL },

	{ 0x1d, 74, 40,  10,  12,  34, 140, "Control" },
	{ 0x1e, 37, 40,  84,  86,  89, 140, NULL },
	{ 0x1f, 37, 40, 121, 123, 126, 140, NULL },
	{ 0x20, 37, 40, 158, 160, 163, 140, NULL },
	{ 0x21, 37, 40, 195, 197, 200, 140, NULL },
	{ 0x22, 37, 40, 232, 234, 237, 140, NULL },
	{ 0x23, 37, 40, 269, 271, 274, 140, NULL },
	{ 0x24, 37, 40, 306, 308, 311, 140, NULL },
	{ 0x25, 37, 40, 343, 345, 348, 140, NULL },
	{ 0x26, 37, 40, 380, 382, 385, 140, NULL },
	{ 0x27, 37, 40, 417, 419, 422, 140, NULL },
	{ 0x28, 37, 40, 454, 456, 459, 140, NULL },
	{ 0x2b, 40, 40, 546, 548, 551, 140, NULL },
	{ 0x00,  0,  0,   0,   0,   0,   0, NULL },
	
	{ 0x2a, 45, 40,  10,  12,  15, 180, "Shift" },
	{ 0x60, 37, 40,  55,  57,  60, 180, NULL },
	{ 0x2c, 37, 40,  92,  94,  97, 180, NULL },
	{ 0x2d, 37, 40, 129, 131, 134, 180, NULL },
	{ 0x2e, 37, 40, 166, 168, 171, 180, NULL },
	{ 0x2f, 37, 40, 203, 205, 208, 180, NULL },
	{ 0x30, 37, 40, 240, 242, 245, 180, NULL },
	{ 0x31, 37, 40, 277, 279, 282, 180, NULL },
	{ 0x32, 37, 40, 314, 316, 319, 180, NULL },
	{ 0x33, 37, 40, 351, 353, 356, 180, NULL },
	{ 0x34, 37, 40, 388, 390, 393, 180, NULL },
	{ 0x35, 37, 40, 425, 427, 430, 180, NULL },
	{ 0x36, 74, 40, 462, 464, 485, 180, "Shift" }, /* x001d0 */
	{ 0x00,  0,  0,   0,   0,   0,   0, NULL },

	{ 0x38, 55, 40,  55,  57,  69, 220, "Alternate" },
	{ 0x39,333, 40, 110, 112, 124, 220, "Space" },
	{ 0x3a, 55, 40, 443, 445, 457, 220, "CapsLock" },
	{ 0x00,  0,  0,   0,   0,   0,   0, NULL },

	{ 0x62, 55, 40, 606, 608, 621,  60, "Help" },
	{ 0x61, 56, 40, 661, 663, 675,  60, "Undo" },
	{ 0x52, 37, 40, 606, 608, 611, 100, "Insert" },
	{ 0x48, 37, 40, 643, 645, 648, 100, "&#x2191;" }, /* Cursor up */
	{ 0x47, 37, 40, 680, 682, 685, 100, "Clr" },
	{ 0x4b, 37, 40, 606, 608, 611, 140, "&#x2190;" }, /* Cursor left */
	{ 0x50, 37, 40, 643, 645, 648, 140, "&#x2193;" }, /* Cursor down */
	{ 0x4d, 37, 40, 680, 682, 685, 140, "&#x2192;" }, /* Cursor right */
	{ 0x00,  0,  0,   0,   0,   0,   0, NULL },

	{ 0x63, 37, 40, 737, 739, 742,  60, NULL },
	{ 0x64, 37, 40, 774, 776, 779,  60, NULL },
	{ 0x65, 37, 40, 811, 813, 816,  60, NULL },
	{ 0x66, 37, 40, 848, 850, 853,  60, NULL },
	{ 0x67, 37, 40, 737, 739, 742, 100, NULL },
	{ 0x68, 37, 40, 774, 776, 779, 100, NULL },
	{ 0x69, 37, 40, 811, 813, 816, 100, NULL },
	{ 0x4a, 37, 40, 848, 850, 853, 100, NULL },
	{ 0x6a, 37, 40, 737, 739, 742, 140, NULL },
	{ 0x6b, 37, 40, 774, 776, 779, 140, NULL },
	{ 0x6c, 37, 40, 811, 813, 816, 140, NULL },
	{ 0x4e, 37, 40, 848, 850, 853, 140, NULL },
	{ 0x6d, 37, 40, 737, 739, 742, 180, NULL },
	{ 0x6e, 37, 40, 774, 776, 779, 180, NULL },
	{ 0x6f, 37, 40, 811, 813, 816, 180, NULL },
	{ 0x72, 37, 80, 848, 850, 853, 180, "Enter" },
	{ 0x70, 74, 40, 737, 739, 742, 220, NULL },
	{ 0x71, 37, 40, 811, 813, 816, 220, NULL },
};

#define KEY_LABEL_YOFFSET1  11
#define KEY_LABEL_YOFFSET2  36
#define KEY_LABEL_XOFFSET   20
#define SCAN_LABEL_OFFSET   25

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static const char *get_keylabel(unsigned char c)
{
	static char labelbuf[20];
	
	switch (c)
	{
		case 0x08: return "Backspace";
		case 0x09: return "Tab";
		case 0x1b: return "Esc";
		case 0x22: return "&#x22;";
		case 0x27: return "&#x27;";
		case 0x60: return "&#x60;";
		case 0x7f: return "Delete";
		case '<': return "&lt;";
		case '>': return "&gt;";
		case '&': return "&amp;";
		case '\\': return "&#x5c;";
		default:
		if (c < 0x20 || c >= 0x80)
		{
			sprintf(labelbuf, "&#x%04x;", atari_to_unicode[codeset][c]);
			return labelbuf;
		}
	}
	labelbuf[0] = c;
	labelbuf[1] = 0;
	return labelbuf;
}


static void write_html_src(FILE *out)
{
	unsigned int i;

#define INT_SPACE(n) ((n) < 10 ? 2 : (n) < 100 ? 1 : 0), ""

fputs("\
<svg\n\
   xmlns:svg=\"http://www.w3.org/2000/svg\"\n\
   xmlns=\"http://www.w3.org/2000/svg\"\n\
   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n\
   width=\"900\"\n\
   height=\"270\">\n\
  <g>\n\
    <path class=\"rect\" d=\"M 20,10 L 75,10 L 65,40 L 10,40 L 20,10\"/>\n\
    <text class=\"keylabel\" x=\"55\" y=\"22\">F1</text>\n\
    <text class=\"scanlabel\" x=\"28\" y=\"30\">0x3b</text>\n\
    <path class=\"rect\" d=\"M 75,10 L 130,10 L 120,40 L 65,40 L 75,10\"/>\n\
    <text class=\"keylabel\" x=\"110\" y=\"22\">F2</text>\n\
    <text class=\"scanlabel\" x=\"83\" y=\"30\">0x3c</text>\n\
    <path class=\"rect\" d=\"M 130,10 L 185,10 L 175,40 L 120,40 L 130,10\"/>\n\
    <text class=\"keylabel\" x=\"165\" y=\"22\">F3</text>\n\
    <text class=\"scanlabel\" x=\"138\" y=\"30\">0x3d</text>\n\
    <path class=\"rect\" d=\"M 185,10 L 240,10 L 230,40 L 175,40 L 185,10\"/>\n\
    <text class=\"keylabel\" x=\"220\" y=\"22\">F4</text>\n\
    <text class=\"scanlabel\" x=\"193\" y=\"30\">0x3e</text>\n\
    <path class=\"rect\" d=\"M 240,10 L 295,10 L 285,40 L 230,40 L 240,10\"/>\n\
    <text class=\"keylabel\" x=\"275\" y=\"22\">F5</text>\n\
    <text class=\"scanlabel\" x=\"248\" y=\"30\">0x3f</text>\n\
    <path class=\"rect\" d=\"M 295,10 L 350,10 L 340,40 L 285,40 L 295,10\"/>\n\
    <text class=\"keylabel\" x=\"330\" y=\"22\">F6</text>\n\
    <text class=\"scanlabel\" x=\"303\" y=\"30\">0x40</text>\n\
    <path class=\"rect\" d=\"M 350,10 L 405,10 L 395,40 L 340,40 L 350,10\"/>\n\
    <text class=\"keylabel\" x=\"385\" y=\"22\">F7</text>\n\
    <text class=\"scanlabel\" x=\"358\" y=\"30\">0x41</text>\n\
    <path class=\"rect\" d=\"M 405,10 L 460,10 L 450,40 L 395,40 L 405,10\"/>\n\
    <text class=\"keylabel\" x=\"440\" y=\"22\">F8</text>\n\
    <text class=\"scanlabel\" x=\"413\" y=\"30\">0x42</text>\n\
    <path class=\"rect\" d=\"M 460,10 L 515,10 L 505,40 L 450,40 L 460,10\"/>\n\
    <text class=\"keylabel\" x=\"495\" y=\"22\">F9</text>\n\
    <text class=\"scanlabel\" x=\"468\" y=\"30\">0x43</text>\n\
    <path class=\"rect\" d=\"M 515,10 L 570,10 L 560,40 L 505,40 L 515,10\"/>\n\
    <text class=\"keylabel\" x=\"545\" y=\"22\">F10</text>\n\
    <text class=\"scanlabel\" x=\"523\" y=\"30\">0x44</text>\n\
\n\
", out);

	for (i = 0; i < ARRAY_SIZE(svgtable); i++)
	{
		const char *keylabel;
		unsigned char scancode = svgtable[i].scancode;
		int ykey = svgtable[i].y + KEY_LABEL_YOFFSET1;
		int wkey = svgtable[i].w;
		
		if (scancode == 0)
		{
			fprintf(out, "\n");
			continue;
		}
		/*
		 * skip unmapped keys, but keep keys like Shift/Control etc.
		 */
		if (keytab[TAB_UNSHIFT][scancode] == 0 && svgtable[i].keylabel == NULL)
			continue;
		
		keylabel = svgtable[i].keylabel;
		if (keylabel == NULL)
			keylabel = get_keylabel(keytab[TAB_SHIFT][scancode]);
		/*
		 * if the key right to the left Shift does not exist, make
		 * the shift key larger
		 */
		if (scancode == 0x2a && keytab[TAB_UNSHIFT][0x60] == 0)
			wkey += 37;
		
		fprintf(out, "    <g><title>Hello</title>\n");
		if (scancode == 0x1c)
		{
			/* return key is special */
			fprintf(out, "    <path class=\"rect\" d=\"M %d,%d L %d,%d L %d,%d L %d,%d L %d,%d L %d,%d L %d,%d\"/>\n",
				svgtable[i].x, svgtable[i].y,
				svgtable[i].x + 37, svgtable[i].y,
				svgtable[i].x + 37, svgtable[i].y + 80,
				svgtable[i].x - 18, svgtable[i].y + 80,
				svgtable[i].x - 18, svgtable[i].y + 40,
				svgtable[i].x, svgtable[i].y + 40,
				svgtable[i].x, svgtable[i].y);
				ykey += 40;
		} else
		{
			fprintf(out, "    <rect class=\"rect\" width=\"%d\" height=\"%d\" x=\"%d\"%-*s y=\"%d\"></rect>\n",
				wkey,
				svgtable[i].h,
				svgtable[i].x,
				INT_SPACE(svgtable[i].x),
				svgtable[i].y);
		}
		fprintf(out, "    <text class=\"");
		if (scancode == 0x0e) /* make text for "Backspace" a bit smaller */
			fprintf(out, "keylabelsmall\"");
		else
			fprintf(out, "keylabel\"     ");
		fprintf(out, "                            x=\"%d\"%-*s y=\"%d\">%s</text>\n",
			svgtable[i].xkey,
			INT_SPACE(svgtable[i].xkey),
			ykey,
			keylabel);
		if (scancode == 0x47)
		{
			/* ClrHome needs a 2nd text */
			fprintf(out, "    <text class=\"keylabel\"     ");
			fprintf(out, "                            x=\"%d\"%-*s y=\"%d\">%s</text>\n",
				svgtable[i].xkey,
				INT_SPACE(svgtable[i].xkey),
				ykey + 25,
				"Home");
		}
		if (keytab[TAB_SHIFT][scancode] != 0 &&
			keytab[TAB_UNSHIFT][scancode] != 0 &&
			keytab[TAB_SHIFT][scancode] != atari_toupper[codeset][keytab[TAB_UNSHIFT][scancode]])
		{
			keylabel = get_keylabel(keytab[TAB_UNSHIFT][scancode]);
			fprintf(out, "    <text class=\"keylabel\"                                 x=\"%d\"%-*s y=\"%d\">%s</text>\n",
				svgtable[i].xkey,
				INT_SPACE(svgtable[i].xkey),
				svgtable[i].y + KEY_LABEL_YOFFSET2,
				keylabel);
		}
		if (keytab[TAB_SHALT][scancode] != 0 && keytab[TAB_ALT][scancode] != keytab[TAB_SHALT][scancode])
		{
			keylabel = get_keylabel(keytab[TAB_SHALT][scancode]);
			fprintf(out, "    <text class=\"keylabel\"                                 x=\"%d\"%-*s y=\"%d\">%s</text>\n",
				svgtable[i].xkey + KEY_LABEL_XOFFSET,
				INT_SPACE(svgtable[i].xkey + KEY_LABEL_XOFFSET),
				svgtable[i].y + KEY_LABEL_YOFFSET1,
				keylabel);
		}
		if (keytab[TAB_ALT][scancode] != 0)
		{
			keylabel = get_keylabel(keytab[TAB_ALT][scancode]);
			fprintf(out, "    <text class=\"keylabel\"                                 x=\"%d\"%-*s y=\"%d\">%s</text>\n",
				svgtable[i].xkey + KEY_LABEL_XOFFSET,
				INT_SPACE(svgtable[i].xkey + KEY_LABEL_XOFFSET),
				svgtable[i].y + KEY_LABEL_YOFFSET2,
				keylabel);
		}
		fprintf(out, "    <text class=\"scanlabel\"                                x=\"%d\"%-*s y=\"%d\">0x%02x</text>\n",
			svgtable[i].xscan,
			INT_SPACE(svgtable[i].xscan),
			svgtable[i].y + SCAN_LABEL_OFFSET,
			scancode);
		fprintf(out, "    </g>\n");
	}
}


static void usage(void)
{
	printf("Usage: %s src-file [html-file]\n", progname);
}


int main(int argc, char **argv)
{
	char *line;
	char *ln;
	char *outname = NULL;
	char *o;
	int r;
	long lineno;
	long num;
	int scancode = 0;
	long flen;
	short w;
	size_t buf;
	FILE *infile;
	FILE *out = 0;
	int tab, newtab;
	int loop;
	const char *filename;
	int is_mint = 0;
	int mint_expected = 0;
	int deadkeys_format;

	/*
	 * very minimalistic option processing
	 */
	argc--;
	progname = *argv++;
	while (argc > 0 && argv[0][0] == '-')
	{
		if (strcmp(argv[0], "--help") == 0)
		{
			usage();
			return 0;
		} else if (strcmp(argv[0], "--") == 0)
		{
			argc--;
			argv++;
			break;
		} else
		{
			fprintf(stderr, "%s: unknown option %s\n", progname, argv[0]);
			return 1;
		}
	}

	if (argc == 0)
	{
		usage();
		return 1;
	} else if (argc == 1)
	{
		filename = argv[0];
	} else if (argc == 2)
	{
		filename = argv[0];
		outname = argv[1];
	} else
	{
		fprintf(stderr, "%s: too many arguments\n", progname);
		return 1;
	}

	buf = 1024;
	line = malloc(buf);					/* should be plenty */
	assert(line);

	if (strcmp(filename, "-") == 0)
		infile = stdin;
	else
		infile = fopen(filename, "r");
	if (infile == NULL)
	{
		fprintf(stderr, "%s: %s\n", filename, strerror(errno));
		return 3;
	}
	/*
	 * hack for now; should be an option
	 */
	codeset = 0;
	if (strncmp(filename, "czech", 5) == 0 ||
		strncmp(filename, "polish", 6) == 0 ||
		strncmp(filename, "hunga", 5) == 0)
		codeset = 1;

	for (tab = 0; tab < N_KEYTBL; tab++)
		copyfrom[tab] = -1;
	lineno = 0;
	tab = -1;
	w = 0;
	
	for (;;)
	{
		lineno++;

		r = mktbl_getdelim(&line, &buf, infile);
		if (r < 0)
			break;
		ln = line;
		while (*ln == ' ' || *ln == '\t')
			ln++;
		if (ln[0] == ';' || ln[0] == '\0')
			continue;
		newtab = -1;
		if (strncmp(ln, "dc.b", 4) == 0)
		{
			w = 0;
		} else if (strncmp(ln, "dc.w", 4) == 0)
		{
			w = 1;
		} else if (strcmp(ln, labels[TAB_SHIFT]) == 0)
		{
			newtab = TAB_SHIFT;
		} else if (strcmp(ln, labels[TAB_UNSHIFT]) == 0)
		{
			newtab = TAB_UNSHIFT;
		} else if (strcmp(ln, labels[TAB_CAPS]) == 0)
		{
			newtab = TAB_CAPS;
		} else if (strcmp(ln, labels[TAB_ALTGR]) == 0)
		{
			newtab = TAB_ALTGR;
		} else if (strcmp(ln, labels[TAB_SHALTGR]) == 0)
		{
			newtab = TAB_SHALTGR;
		} else if (strcmp(ln, labels[TAB_CAPSALTGR]) == 0)
		{
			newtab = TAB_CAPSALTGR;
		} else if (strcmp(ln, labels[TAB_ALT]) == 0)
		{
			newtab = TAB_ALT;
		} else if (strcmp(ln, labels[TAB_ALT]) == 0)
		{
			newtab = TAB_ALT;
		} else if (strcmp(ln, labels[TAB_SHALT]) == 0)
		{
			newtab = TAB_SHALT;
		} else if (strcmp(ln, labels[TAB_CAPSALT]) == 0)
		{
			newtab = TAB_CAPSALT;
		} else if (strcmp(ln, labels[TAB_DEADKEYS]) == 0)
		{
			newtab = TAB_DEADKEYS;
		} else if (strncmp(ln, "IFNE", 4) == 0)
		{
			continue;
		} else if (strncmp(ln, "ENDC", 4) == 0)
		{
			continue;
		} else
		{
			fprintf(stderr, "%s:%ld: warning: unknown statement: %s\n", filename, lineno, line);
			continue;
		}
		
		if (newtab >= 0)
		{
			if (tab >= 0 && tab < N_KEYTBL && tabsize[tab] == 0)
				copyfrom[tab] = newtab;
			tab = newtab;
			continue;
		}
		
		ln += 4;
		while (*ln == ' ' || *ln == '\t')
			ln++;

		while (*ln)
		{
			if (ln[0] == ';')
				break;
			
			if (ln[0] == '\'')
			{
				if (ln[2] != '\'')
				{
					fprintf(stderr, "%s:%ld: error: unmatched quotes: %s\n", filename, lineno, line);
					r = 5;
					goto error;
				}
				num = (unsigned char)ln[1];
				ln += 3;
			} else if (ln[0] == '$')
			{
				if (!isxdigit(ln[1]))
				{
					fprintf(stderr, "%s:%ld: error: '%c' is not a hex number: %s\n", filename, lineno, ln[1], line);
					r = 6;
					goto error;
				}

				ln++;
				num = strtoul(ln, &ln, 16);
			} else if (ln[0] == '0' && ln[1] == 'x')
			{
				if (!isxdigit(ln[3]))
				{
					fprintf(stderr, "%s:%ld: error: '%c' is not a hex number: %s\n", filename, lineno, ln[3], line);
					r = 7;
					goto error;
				}

				ln += 2;
				num = strtoul(ln, &ln, 16);
			} else if (isdigit(ln[0]))
			{
				num = strtoul(ln, &ln, 10);
			} else if (strncmp(ln, "XXX", 3) == 0)
			{
				num = 0;
				ln += 3;
			} else if (strncmp(ln, "YYY", 3) == 0)
			{
				num = 0;
				ln += 3;
			} else if (strncmp(ln, "U2B", 3) == 0)
			{
				num = 0x7e;
				ln += 3;
			} else if (strncmp(ln, "S2B", 3) == 0)
			{
				num = 0x7c;
				ln += 3;
			} else if (strncmp(ln, "S29", 3) == 0)
			{
				num = 0x5e;
				ln += 3;
			} else
			{
				fprintf(stderr, "%s:%ld: error: unexpected '%c': %s\n", filename, lineno, ln[0], line);
				r = 8;
				goto error;
			}

			if (w)
			{
				/*
				 * dc.w only used for MiNT to specify magics at filestart
				 */
				if (num > 65535L)
				{
					fprintf(stderr, "%s:%ld: error: number %ld is too big\n", filename, lineno, num);
					r = 1;
					goto error;
				}
				if (tab >= 0)
				{
					fprintf(stderr, "%s:%ld: error: too many magics: %s\n", filename, lineno, line);
					r = 1;
					goto error;
				}
				if (is_mint == 0)
				{
					switch ((unsigned int) num)
					{
					case 0x2771:
						mint_expected = 1;
						fprintf(stderr, "%s:%ld: warning: old format without AKP code: %s\n", filename, lineno, line);
						break;
					case 0x2772:
						mint_expected = 2;
						break;
					case 0x2773:
						mint_expected = 4;
						break;
					default:
						fprintf(stderr, "%s:%ld: error: invalid magic: %s\n", filename, lineno, line);
						r = 1;
						goto error;
					}
				}
				if (is_mint == mint_expected)
				{
					/*
					 * MinT keyboard tables start with 1, 2 or 4 words
					 */
					fprintf(stderr, "%s:%ld: error: too many magics: %s\n", filename, lineno, line);
					r = 1;
					goto error;
				}
				is_mint++;
				if (is_mint == mint_expected)
					tab = TAB_UNSHIFT;
			} else
			{
				if (num > 255)
					fprintf(stderr, "%s:%ld: warning: number %ld is too big\n", filename, lineno, num);
				if (tab < 0)
				{
					fprintf(stderr, "%s:%ld: error: no table: %s\n", filename, lineno, line);
					r = 1;
					goto error;
				}
				if (tab == TAB_DEADKEYS)
				{
					if (tabsize[tab] >= MAX_DEADKEYS)
					{
						fprintf(stderr, "%s:%ld: error: too many dead keys: %s\n", filename, lineno, line);
						r = 1;
						goto error;
					}
					deadkeys[tabsize[tab]] = num;
					tabsize[tab]++;
				} else if (tab > TAB_DEADKEYS)
				{
					fprintf(stderr, "%s:%ld: error: too many tables: %s\n", filename, lineno, line);
					r = 1;
					goto error;
				} else
				{
					if (tabsize[tab] >= MAX_SCANCODE)
					{
						if (is_mint && tab <= TAB_CAPS)
						{
							/* no keywords in mint tables; just switch to next one */
							tab++;
							if (tab == TAB_ALTGR)
								tab = TAB_ALT;
						} else if (!is_mint || tab <= TAB_CAPS)
						{
							fprintf(stderr, "%s:%ld: error: too many keys: %s\n", filename, lineno, line);
							r = 1;
							goto error;
						}
					}
					if (is_mint && tab >= TAB_ALTGR)
					{
						/*
						 * in MiNT sources, all alt-tables are a scancode/value pair
						 */
						if (scancode == 0)
						{
							/*
							 * first value: scancode
							 */
							scancode = (unsigned char)num;
							if (num == 0)
							{
								/*
								 * end of table; automatically switch to next
								 */
								switch (tab)
								{
								case TAB_ALT:
									tab = TAB_SHALT;
									break;
								case TAB_SHALT:
									tab = TAB_CAPSALT;
									break;
								case TAB_CAPSALT:
									tab = TAB_ALTGR;
				 					break;
								case TAB_ALTGR:
									tab = TAB_DEADKEYS;
									break;
								case TAB_DEADKEYS:
									tab = TAB_DEADKEYS + 1;
									break;
								default:
									assert(0);
									break;
								}
							} else if (num <= 0 || num >= MAX_SCANCODE)
							{
								fprintf(stderr, "%s:%ld: error: illegal scancode $%02x: %s\n", filename, lineno, (int)num, line);
								r = 1;
								goto error;
							}
						} else
						{
							/*
							 * 2nd value: ascii code
							 */
							if (num == 0)
							{
								fprintf(stderr, "%s:%ld: error: illegal ascii code $%02x: %s\n", filename, lineno, (int)num, line);
								r = 1;
								goto error;
							}
							if (scancode >= tabsize[tab])
								tabsize[tab] = scancode + 1;
							if (keytab[tab][scancode] != 0)
							{
								fprintf(stderr, "%s:%ld: warning: duplicate scancode $%02x: %s\n", filename, lineno, (int)scancode, line);
							}
							keytab[tab][scancode] = num;
							scancode = 0;
						}
					} else
					{
						keytab[tab][tabsize[tab]] = num;
						tabsize[tab]++;
					}
				}
			}

			while (*ln == ' ' || *ln == '\t')
				ln++;
			if (ln[0] == ',')
			{
				ln++;
				while (*ln == ' ' || *ln == '\t')
					ln++;
			}
		}
	}

	for (loop = 0; loop < N_KEYTBL; loop++)
	{
		for (tab = 0; tab < N_KEYTBL; tab++)
		{
			if (copyfrom[tab] >= 0 && tabsize[copyfrom[tab]] > 0)
			{
				memcpy(keytab[tab], keytab[copyfrom[tab]], MAX_SCANCODE);
				tabsize[tab] = tabsize[copyfrom[tab]];
			}
		}
	}
	
	/*
	 * the altgr table is missing from a lot of sources
	 */
	if (is_mint && tabsize[TAB_ALTGR] == 0)
	{
		keytab[TAB_ALTGR][0] = 0;
		tabsize[TAB_ALTGR] = 1;
	}
	
	for (tab = 0; tab < N_KEYTBL; tab++)
	{
		if (tab <= TAB_CAPS || is_mint == 0)
		{
			if (tabsize[tab] == 0)
				fprintf(stderr, "%s: warning: missing table for %s\n", filename, labels[tab]);
			else if (tab <= TAB_CAPS && tabsize[tab] != MAX_SCANCODE)
				fprintf(stderr, "%s: warning: incomplete table for %s\n", filename, labels[tab]);
		}
	}

	deadkeys_format = FORMAT_NONE;
	if (tabsize[TAB_DEADKEYS] > 0)
	{
		deadkeys_format = FORMAT_MAGIC;
		/*
		 * Magic uses a list of only the used deadkeys first
		 */
		for (loop = 0; loop < tabsize[TAB_DEADKEYS] && deadkeys[loop] != 0; loop++)
		{
			if (strchr((const char *)possible_dead_keys, deadkeys[loop]) == NULL)
			{
				deadkeys_format = FORMAT_MINT;
				break;
			}
		}
		if (deadkeys[tabsize[TAB_DEADKEYS] - 1] != 0)
		{
			fprintf(stderr, "%s: warning: missing terminating zero in dead keys\n", filename);
			deadkeys[tabsize[TAB_DEADKEYS]] = 0;
			tabsize[TAB_DEADKEYS]++;
		}
	}
	(void) deadkeys_format;

	if (outname == NULL && infile != stdin)
	{
		const char *ext = ".html";

		flen = strlen(filename);
		outname = malloc(flen + 6);
		if (!outname)
		{
			fprintf(stderr, "Out of RAM\n");
			return 1;
		}
		strcpy(outname, filename);
		o = strrchr(outname, '.');
		if (o == NULL)
			strcat(outname, ext);
		else
			strcpy(o, ext);

		printf("%s: output to %s\n", filename, outname);
	}

	if (outname == NULL || strcmp(outname, "-") == 0)
		out = stdout;
	else
		out = fopen(outname, "wb");
	if (out == NULL)
	{
		fprintf(stderr, "%s: %s\n", outname, strerror(errno));
		return 4;
	}

	r = 0;

	write_html_src(out);

error:
	if (infile != stdin)
		fclose(infile);

	if (out)
	{
		fflush(out);
		if (out != stdout)
			fclose(out);
	}

	if (r && outname && out != stdout)
		unlink(outname);

	return r;
}
