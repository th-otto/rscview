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


#if 0 /* currently unused */
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
#endif

static unsigned short const atari_to_unicode[][256] = {
	{
	/* 0: atarist encoding */
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
	int x;
	int w;
	const char *keylabel;
} const layout_table[] = {
	{ 0x3b,    0,  81, "F1" },
	{ 0x3c,   81,  81, "F2" },
	{ 0x3d,  162,  81, "F3" },
	{ 0x3d,  243,  81, "F4" },
	{ 0x3d,  324,  81, "F5" },
	{ 0x3d,  405,  81, "F6" },
	{ 0x3d,  486,  81, "F7" },
	{ 0x3d,  567,  81, "F8" },
	{ 0x3d,  648,  81, "F9" },
	{ 0x3d,  729,  81, "F10" },
	{ 0x00,    0,   0, NULL },

	{ 0x01,    0,   0, "Esc" },
	{ 0x02,   54,   0, NULL },
	{ 0x03,  108,   0, NULL },
	{ 0x04,  162,   0, NULL },
	{ 0x05,  216,   0, NULL },
	{ 0x06,  270,   0, NULL },
	{ 0x07,  324,   0, NULL },
	{ 0x08,  378,   0, NULL },
	{ 0x09,  432,   0, NULL },
	{ 0x0a,  486,   0, NULL },
	{ 0x0b,  540,   0, NULL },
	{ 0x0c,  594,   0, NULL },
	{ 0x0d,  648,   0, NULL },
	{ 0x29,  702,   0, NULL },
	{ 0x0e,  756,  81, "Backspace" },

	{ 0x62,  857,  81, "Help" },
	{ 0x61,  938,  81, "Undo" },

	{ 0x63, 1039,   0, NULL },
	{ 0x64, 1093,   0, NULL },
	{ 0x65, 1147,   0, NULL },
	{ 0x66, 1201,   0, NULL },

	{ 0x00,    0,   0, NULL },

	{ 0x0f,    0,  81, "Tab" },
	{ 0x10,   81,   0, NULL },
	{ 0x11,  135,   0, NULL },
	{ 0x12,  189,   0, NULL },
	{ 0x13,  243,   0, NULL },
	{ 0x14,  297,   0, NULL },
	{ 0x15,  351,   0, NULL },
	{ 0x16,  405,   0, NULL },
	{ 0x17,  459,   0, NULL },
	{ 0x18,  513,   0, NULL },
	{ 0x19,  567,   0, NULL },
	{ 0x1a,  621,   0, NULL },
	{ 0x1b,  675,   0, NULL },
	{ 0x1c,  729,   0, "Return" },
	{ 0x53,  783,   0, "Delete" },

	{ 0x52,  857,   0, "Insert" },
	{ 0x48,  911,   0, "&#x2191;" }, /* Cursor up */
	{ 0x47,  965,   0, "Clr<br />Home" },

	{ 0x67, 1039,   0, NULL },
	{ 0x68, 1093,   0, NULL },
	{ 0x69, 1147,   0, NULL },
	{ 0x4a, 1202,   0, NULL },

	{ 0x00,    0,   0, NULL },

	{ 0x1d,    0, 108, "Control" },
	{ 0x1e,  108,   0, NULL },
	{ 0x1f,  162,   0, NULL },
	{ 0x20,  216,   0, NULL },
	{ 0x21,  270,   0, NULL },
	{ 0x22,  324,   0, NULL },
	{ 0x23,  378,   0, NULL },
	{ 0x24,  432,   0, NULL },
	{ 0x25,  486,   0, NULL },
	{ 0x26,  540,   0, NULL },
	{ 0x27,  594,   0, NULL },
	{ 0x28,  648,   0, NULL },
	{ 0x2b,  783,   0, NULL },
	
	{ 0x4b,  857,   0, "&#x2190;" }, /* Cursor left */
	{ 0x50,  911,   0, "&#x2193;" }, /* Cursor down */
	{ 0x4d,  965,   0, "&#x2192;" }, /* Cursor right */

	{ 0x6a, 1039,   0, NULL },
	{ 0x6b, 1093,   0, NULL },
	{ 0x6c, 1147,   0, NULL },
	{ 0x4e, 1202,   0, NULL },

	{ 0x00,    0,   0, NULL },

	{ 0x2a,    0,  66, "Shift" },
	{ 0x60,   66,   0, NULL },
	{ 0x2c,  120,   0, NULL },
	{ 0x2d,  174,   0, NULL },
	{ 0x2e,  228,   0, NULL },
	{ 0x2f,  282,   0, NULL },
	{ 0x30,  336,   0, NULL },
	{ 0x31,  390,   0, NULL },
	{ 0x32,  444,   0, NULL },
	{ 0x33,  498,   0, NULL },
	{ 0x34,  552,   0, NULL },
	{ 0x35,  606,   0, NULL },
	{ 0x36,  660, 108, "Shift" },

	{ 0x6d, 1039,   0, NULL },
	{ 0x6e, 1093,   0, NULL },
	{ 0x6f, 1147,   0, NULL },
	{ 0x72, 1202,   0, "Enter" },

	{ 0x00,    0,   0, NULL },

	{ 0x38,   81,  81, "Alternate" },
	{ 0x39,  162, 460, "Space" },
	{ 0x3a,  622,  81, "CapsLock" },

	{ 0x70, 1039, 108, NULL },
	{ 0x71, 1147,   0, NULL },

	{ 0x00,    0,   0, NULL },
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static struct {
	unsigned short code;
	const char *name;
} const unicode_table[] = {
	{ 0x0001, "START OF HEADING" },
	{ 0x0002, "START OF TEXT" },
	{ 0x0003, "END OF TEXT" },
	{ 0x0004, "END OF TRANSMISSION" },
	{ 0x0005, "ENQUIRY" },
	{ 0x0006, "ACKNOWLEDGE" },
	{ 0x0007, "BELL" },
	{ 0x0008, "BACKSPACE" },
	{ 0x0009, "CHARACTER TABULATION" },
	{ 0x000a, "LINE FEED (LF)" },
	{ 0x000b, "LINE TABULATION" },
	{ 0x000c, "FORM FEED (FF)" },
	{ 0x000d, "CARRIAGE RETURN (CR)" },
	{ 0x000e, "SHIFT OUT" },
	{ 0x000f, "SHIFT IN" },
	{ 0x0010, "DATA LINK ESCAPE" },
	{ 0x0011, "DEVICE CONTROL ONE" },
	{ 0x0012, "DEVICE CONTROL TWO" },
	{ 0x0013, "DEVICE CONTROL THREE" },
	{ 0x0014, "DEVICE CONTROL FOUR" },
	{ 0x0015, "NEGATIVE ACKNOWLEDGE" },
	{ 0x0016, "SYNCHRONOUS IDLE" },
	{ 0x0017, "END OF TRANSMISSION BLOCK" },
	{ 0x0018, "CANCEL" },
	{ 0x0019, "END OF MEDIUM" },
	{ 0x001a, "SUBSTITUTE" },
	{ 0x001b, "ESCAPE" },
	{ 0x001c, "INFORMATION SEPARATOR FOUR" },
	{ 0x001d, "INFORMATION SEPARATOR THREE" },
	{ 0x001e, "INFORMATION SEPARATOR TWO" },
	{ 0x001f, "INFORMATION SEPARATOR ONE" },
	{ 0x0020, "SPACE" },
	{ 0x0021, "EXCLAMATION MARK" },
	{ 0x0022, "QUOTATION MARK" },
	{ 0x0023, "NUMBER SIGN" },
	{ 0x0024, "DOLLAR SIGN" },
	{ 0x0025, "PERCENT SIGN" },
	{ 0x0026, "AMPERSAND" },
	{ 0x0027, "APOSTROPHE" },
	{ 0x0028, "LEFT PARENTHESIS" },
	{ 0x0029, "RIGHT PARENTHESIS" },
	{ 0x002a, "ASTERISK" },
	{ 0x002b, "PLUS SIGN" },
	{ 0x002c, "COMMA" },
	{ 0x002d, "HYPHEN-MINUS" },
	{ 0x002e, "FULL STOP" },
	{ 0x002f, "SOLIDUS" },
	{ 0x0030, "DIGIT ZERO" },
	{ 0x0031, "DIGIT ONE" },
	{ 0x0032, "DIGIT TWO" },
	{ 0x0033, "DIGIT THREE" },
	{ 0x0034, "DIGIT FOUR" },
	{ 0x0035, "DIGIT FIVE" },
	{ 0x0036, "DIGIT SIX" },
	{ 0x0037, "DIGIT SEVEN" },
	{ 0x0038, "DIGIT EIGHT" },
	{ 0x0039, "DIGIT NINE" },
	{ 0x003a, "COLON" },
	{ 0x003b, "SEMICOLON" },
	{ 0x003c, "LESS-THAN SIGN" },
	{ 0x003d, "EQUALS SIGN" },
	{ 0x003e, "GREATER-THAN SIGN" },
	{ 0x003f, "QUESTION MARK" },
	{ 0x0040, "COMMERCIAL AT" },
	{ 0x0041, "LATIN CAPITAL LETTER A" },
	{ 0x0042, "LATIN CAPITAL LETTER B" },
	{ 0x0043, "LATIN CAPITAL LETTER C" },
	{ 0x0044, "LATIN CAPITAL LETTER D" },
	{ 0x0045, "LATIN CAPITAL LETTER E" },
	{ 0x0046, "LATIN CAPITAL LETTER F" },
	{ 0x0047, "LATIN CAPITAL LETTER G" },
	{ 0x0048, "LATIN CAPITAL LETTER H" },
	{ 0x0049, "LATIN CAPITAL LETTER I" },
	{ 0x004a, "LATIN CAPITAL LETTER J" },
	{ 0x004b, "LATIN CAPITAL LETTER K" },
	{ 0x004c, "LATIN CAPITAL LETTER L" },
	{ 0x004d, "LATIN CAPITAL LETTER M" },
	{ 0x004e, "LATIN CAPITAL LETTER N" },
	{ 0x004f, "LATIN CAPITAL LETTER O" },
	{ 0x0050, "LATIN CAPITAL LETTER P" },
	{ 0x0051, "LATIN CAPITAL LETTER Q" },
	{ 0x0052, "LATIN CAPITAL LETTER R" },
	{ 0x0053, "LATIN CAPITAL LETTER S" },
	{ 0x0054, "LATIN CAPITAL LETTER T" },
	{ 0x0055, "LATIN CAPITAL LETTER U" },
	{ 0x0056, "LATIN CAPITAL LETTER V" },
	{ 0x0057, "LATIN CAPITAL LETTER W" },
	{ 0x0058, "LATIN CAPITAL LETTER X" },
	{ 0x0059, "LATIN CAPITAL LETTER Y" },
	{ 0x005a, "LATIN CAPITAL LETTER Z" },
	{ 0x005b, "LEFT SQUARE BRACKET" },
	{ 0x005c, "REVERSE SOLIDUS" },
	{ 0x005d, "RIGHT SQUARE BRACKET" },
	{ 0x005e, "CIRCUMFLEX ACCENT" },
	{ 0x005f, "LOW LINE" },
	{ 0x0060, "GRAVE ACCENT" },
	{ 0x0061, "LATIN SMALL LETTER A" },
	{ 0x0062, "LATIN SMALL LETTER B" },
	{ 0x0063, "LATIN SMALL LETTER C" },
	{ 0x0064, "LATIN SMALL LETTER D" },
	{ 0x0065, "LATIN SMALL LETTER E" },
	{ 0x0066, "LATIN SMALL LETTER F" },
	{ 0x0067, "LATIN SMALL LETTER G" },
	{ 0x0068, "LATIN SMALL LETTER H" },
	{ 0x0069, "LATIN SMALL LETTER I" },
	{ 0x006a, "LATIN SMALL LETTER J" },
	{ 0x006b, "LATIN SMALL LETTER K" },
	{ 0x006c, "LATIN SMALL LETTER L" },
	{ 0x006d, "LATIN SMALL LETTER M" },
	{ 0x006e, "LATIN SMALL LETTER N" },
	{ 0x006f, "LATIN SMALL LETTER O" },
	{ 0x0070, "LATIN SMALL LETTER P" },
	{ 0x0071, "LATIN SMALL LETTER Q" },
	{ 0x0072, "LATIN SMALL LETTER R" },
	{ 0x0073, "LATIN SMALL LETTER S" },
	{ 0x0074, "LATIN SMALL LETTER T" },
	{ 0x0075, "LATIN SMALL LETTER U" },
	{ 0x0076, "LATIN SMALL LETTER V" },
	{ 0x0077, "LATIN SMALL LETTER W" },
	{ 0x0078, "LATIN SMALL LETTER X" },
	{ 0x0079, "LATIN SMALL LETTER Y" },
	{ 0x007a, "LATIN SMALL LETTER Z" },
	{ 0x007b, "LEFT CURLY BRACKET" },
	{ 0x007c, "VERTICAL LINE" },
	{ 0x007d, "RIGHT CURLY BRACKET" },
	{ 0x007e, "TILDE" },
	{ 0x007f, "DELETE" },
	{ 0x0081, "<undefined>" },
	{ 0x0083, "<undefined>" },
	{ 0x0088, "<undefined>" },
	{ 0x0090, "<undefined>" },
	{ 0x0098, "<undefined>" },
	{ 0x00a0, "NO-BREAK SPACE" },
	{ 0x00a1, "INVERTED EXCLAMATION MARK" },
	{ 0x00a2, "CENT SIGN" },
	{ 0x00a3, "POUND SIGN" },
	{ 0x00a4, "CURRENCY SIGN" },
	{ 0x00a5, "YEN SIGN" },
	{ 0x00a6, "BROKEN BAR" },
	{ 0x00a7, "SECTION SIGN" },
	{ 0x00a8, "DIAERESIS" },
	{ 0x00a9, "COPYRIGHT SIGN" },
	{ 0x00aa, "FEMININE ORDINAL INDICATOR" },
	{ 0x00ab, "LEFT-POINTING DOUBLE ANGLE QUOTATION MARK" },
	{ 0x00ac, "NOT SIGN" },
	{ 0x00ad, "SOFT HYPHEN" },
	{ 0x00ae, "REGISTERED SIGN" },
	{ 0x00af, "MACRON" },
	{ 0x00b0, "DEGREE SIGN" },
	{ 0x00b1, "PLUS-MINUS SIGN" },
	{ 0x00b2, "SUPERSCRIPT TWO" },
	{ 0x00b3, "SUPERSCRIPT THREE" },
	{ 0x00b4, "ACUTE ACCENT" },
	{ 0x00b5, "MICRO SIGN" },
	{ 0x00b6, "PILCROW SIGN" },
	{ 0x00b7, "MIDDLE DOT" },
	{ 0x00b8, "CEDILLA" },
	{ 0x00b9, "SUPERSCRIPT ONE" },
	{ 0x00ba, "MASCULINE ORDINAL INDICATOR" },
	{ 0x00bb, "RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK" },
	{ 0x00bc, "VULGAR FRACTION ONE QUARTER" },
	{ 0x00bd, "VULGAR FRACTION ONE HALF" },
	{ 0x00be, "VULGAR FRACTION THREE QUARTERS" },
	{ 0x00bf, "INVERTED QUESTION MARK" },
	{ 0x00c0, "LATIN CAPITAL LETTER A WITH GRAVE" },
	{ 0x00c1, "LATIN CAPITAL LETTER A WITH ACUTE" },
	{ 0x00c2, "LATIN CAPITAL LETTER A WITH CIRCUMFLEX" },
	{ 0x00c3, "LATIN CAPITAL LETTER A WITH TILDE" },
	{ 0x00c4, "LATIN CAPITAL LETTER A WITH DIAERESIS" },
	{ 0x00c5, "LATIN CAPITAL LETTER A WITH RING ABOVE" },
	{ 0x00c6, "LATIN CAPITAL LETTER AE" },
	{ 0x00c7, "LATIN CAPITAL LETTER C WITH CEDILLA" },
	{ 0x00c8, "LATIN CAPITAL LETTER E WITH GRAVE" },
	{ 0x00c9, "LATIN CAPITAL LETTER E WITH ACUTE" },
	{ 0x00ca, "LATIN CAPITAL LETTER E WITH CIRCUMFLEX" },
	{ 0x00cb, "LATIN CAPITAL LETTER E WITH DIAERESIS" },
	{ 0x00cc, "LATIN CAPITAL LETTER I WITH GRAVE" },
	{ 0x00cd, "LATIN CAPITAL LETTER I WITH ACUTE" },
	{ 0x00ce, "LATIN CAPITAL LETTER I WITH CIRCUMFLEX" },
	{ 0x00cf, "LATIN CAPITAL LETTER I WITH DIAERESIS" },
	{ 0x00d0, "LATIN CAPITAL LETTER ETH" },
	{ 0x00d1, "LATIN CAPITAL LETTER N WITH TILDE" },
	{ 0x00d2, "LATIN CAPITAL LETTER O WITH GRAVE" },
	{ 0x00d3, "LATIN CAPITAL LETTER O WITH ACUTE" },
	{ 0x00d4, "LATIN CAPITAL LETTER O WITH CIRCUMFLEX" },
	{ 0x00d5, "LATIN CAPITAL LETTER O WITH TILDE" },
	{ 0x00d6, "LATIN CAPITAL LETTER O WITH DIAERESIS" },
	{ 0x00d7, "MULTIPLICATION SIGN" },
	{ 0x00d8, "LATIN CAPITAL LETTER O WITH STROKE" },
	{ 0x00d9, "LATIN CAPITAL LETTER U WITH GRAVE" },
	{ 0x00da, "LATIN CAPITAL LETTER U WITH ACUTE" },
	{ 0x00db, "LATIN CAPITAL LETTER U WITH CIRCUMFLEX" },
	{ 0x00dc, "LATIN CAPITAL LETTER U WITH DIAERESIS" },
	{ 0x00dd, "LATIN CAPITAL LETTER Y WITH ACUTE" },
	{ 0x00de, "LATIN CAPITAL LETTER THORN" },
	{ 0x00df, "LATIN SMALL LETTER SHARP S" },
	{ 0x00e0, "LATIN SMALL LETTER A WITH GRAVE" },
	{ 0x00e1, "LATIN SMALL LETTER A WITH ACUTE" },
	{ 0x00e2, "LATIN SMALL LETTER A WITH CIRCUMFLEX" },
	{ 0x00e3, "LATIN SMALL LETTER A WITH TILDE" },
	{ 0x00e4, "LATIN SMALL LETTER A WITH DIAERESIS" },
	{ 0x00e5, "LATIN SMALL LETTER A WITH RING ABOVE" },
	{ 0x00e6, "LATIN SMALL LETTER AE" },
	{ 0x00e7, "LATIN SMALL LETTER C WITH CEDILLA" },
	{ 0x00e8, "LATIN SMALL LETTER E WITH GRAVE" },
	{ 0x00e9, "LATIN SMALL LETTER E WITH ACUTE" },
	{ 0x00ea, "LATIN SMALL LETTER E WITH CIRCUMFLEX" },
	{ 0x00eb, "LATIN SMALL LETTER E WITH DIAERESIS" },
	{ 0x00ec, "LATIN SMALL LETTER I WITH GRAVE" },
	{ 0x00ed, "LATIN SMALL LETTER I WITH ACUTE" },
	{ 0x00ee, "LATIN SMALL LETTER I WITH CIRCUMFLEX" },
	{ 0x00ef, "LATIN SMALL LETTER I WITH DIAERESIS" },
	{ 0x00f0, "LATIN SMALL LETTER ETH" },
	{ 0x00f1, "LATIN SMALL LETTER N WITH TILDE" },
	{ 0x00f2, "LATIN SMALL LETTER O WITH GRAVE" },
	{ 0x00f3, "LATIN SMALL LETTER O WITH ACUTE" },
	{ 0x00f4, "LATIN SMALL LETTER O WITH CIRCUMFLEX" },
	{ 0x00f5, "LATIN SMALL LETTER O WITH TILDE" },
	{ 0x00f6, "LATIN SMALL LETTER O WITH DIAERESIS" },
	{ 0x00f7, "DIVISION SIGN" },
	{ 0x00f8, "LATIN SMALL LETTER O WITH STROKE" },
	{ 0x00f9, "LATIN SMALL LETTER U WITH GRAVE" },
	{ 0x00fa, "LATIN SMALL LETTER U WITH ACUTE" },
	{ 0x00fb, "LATIN SMALL LETTER U WITH CIRCUMFLEX" },
	{ 0x00fc, "LATIN SMALL LETTER U WITH DIAERESIS" },
	{ 0x00fd, "LATIN SMALL LETTER Y WITH ACUTE" },
	{ 0x00fe, "LATIN SMALL LETTER THORN" },
	{ 0x00ff, "LATIN SMALL LETTER Y WITH DIAERESIS" },
	{ 0x0102, "LATIN CAPITAL LETTER A WITH BREVE" },
	{ 0x0103, "LATIN SMALL LETTER A WITH BREVE" },
	{ 0x0104, "LATIN CAPITAL LETTER A WITH OGONEK" },
	{ 0x0105, "LATIN SMALL LETTER A WITH OGONEK" },
	{ 0x0106, "LATIN CAPITAL LETTER C WITH ACUTE" },
	{ 0x0107, "LATIN SMALL LETTER C WITH ACUTE" },
	{ 0x010c, "LATIN CAPITAL LETTER C WITH CARON" },
	{ 0x010d, "LATIN SMALL LETTER C WITH CARON" },
	{ 0x010e, "LATIN CAPITAL LETTER D WITH CARON" },
	{ 0x010f, "LATIN SMALL LETTER D WITH CARON" },
	{ 0x0110, "LATIN CAPITAL LETTER D WITH STROKE" },
	{ 0x0111, "LATIN SMALL LETTER D WITH STROKE" },
	{ 0x0118, "LATIN CAPITAL LETTER E WITH OGONEK" },
	{ 0x0119, "LATIN SMALL LETTER E WITH OGONEK" },
	{ 0x011a, "LATIN CAPITAL LETTER E WITH CARON" },
	{ 0x011b, "LATIN SMALL LETTER E WITH CARON" },
	{ 0x0131, "LATIN SMALL LETTER DOTLESS I" },
	{ 0x0132, "LATIN CAPITAL LIGATURE IJ" },
	{ 0x0133, "LATIN SMALL LIGATURE IJ" },
	{ 0x0139, "LATIN CAPITAL LETTER L WITH ACUTE" },
	{ 0x013a, "LATIN SMALL LETTER L WITH ACUTE" },
	{ 0x013d, "LATIN CAPITAL LETTER L WITH CARON" },
	{ 0x013e, "LATIN SMALL LETTER L WITH CARON" },
	{ 0x0141, "LATIN CAPITAL LETTER L WITH STROKE" },
	{ 0x0142, "LATIN SMALL LETTER L WITH STROKE" },
	{ 0x0143, "LATIN CAPITAL LETTER N WITH ACUTE" },
	{ 0x0144, "LATIN SMALL LETTER N WITH ACUTE" },
	{ 0x0147, "LATIN CAPITAL LETTER N WITH CARON" },
	{ 0x0148, "LATIN SMALL LETTER N WITH CARON" },
	{ 0x0150, "LATIN CAPITAL LETTER O WITH DOUBLE ACUTE" },
	{ 0x0151, "LATIN SMALL LETTER O WITH DOUBLE ACUTE" },
	{ 0x0152, "LATIN CAPITAL LIGATURE OE" },
	{ 0x0153, "LATIN SMALL LIGATURE OE" },
	{ 0x0154, "LATIN CAPITAL LETTER R WITH ACUTE" },
	{ 0x0155, "LATIN SMALL LETTER R WITH ACUTE" },
	{ 0x0158, "LATIN CAPITAL LETTER R WITH CARON" },
	{ 0x0159, "LATIN SMALL LETTER R WITH CARON" },
	{ 0x015a, "LATIN CAPITAL LETTER S WITH ACUTE" },
	{ 0x015b, "LATIN SMALL LETTER S WITH ACUTE" },
	{ 0x015e, "LATIN CAPITAL LETTER S WITH CEDILLA" },
	{ 0x015f, "LATIN SMALL LETTER S WITH CEDILLA" },
	{ 0x0160, "LATIN CAPITAL LETTER S WITH CARON" },
	{ 0x0161, "LATIN SMALL LETTER S WITH CARON" },
	{ 0x0162, "LATIN CAPITAL LETTER T WITH CEDILLA" },
	{ 0x0163, "LATIN SMALL LETTER T WITH CEDILLA" },
	{ 0x0164, "LATIN CAPITAL LETTER T WITH CARON" },
	{ 0x0165, "LATIN SMALL LETTER T WITH CARON" },
	{ 0x016e, "LATIN CAPITAL LETTER U WITH RING ABOVE" },
	{ 0x016f, "LATIN SMALL LETTER U WITH RING ABOVE" },
	{ 0x0170, "LATIN CAPITAL LETTER U WITH DOUBLE ACUTE" },
	{ 0x0171, "LATIN SMALL LETTER U WITH DOUBLE ACUTE" },
	{ 0x0179, "LATIN CAPITAL LETTER Z WITH ACUTE" },
	{ 0x017a, "LATIN SMALL LETTER Z WITH ACUTE" },
	{ 0x017b, "LATIN CAPITAL LETTER Z WITH DOT ABOVE" },
	{ 0x017c, "LATIN SMALL LETTER Z WITH DOT ABOVE" },
	{ 0x017d, "LATIN CAPITAL LETTER Z WITH CARON" },
	{ 0x017e, "LATIN SMALL LETTER Z WITH CARON" },
	{ 0x018f, "LATIN CAPITAL LETTER SCHWA" },
	{ 0x0192, "LATIN SMALL LETTER F WITH HOOK" },
	{ 0x02c7, "CARON" },
	{ 0x02d8, "BREVE" },
	{ 0x02d9, "DOT ABOVE" },
	{ 0x02db, "OGONEK" },
	{ 0x02dd, "DOUBLE ACUTE ACCENT" },
	{ 0x0391, "GREEK CAPITAL LETTER ALPHA" },
	{ 0x0392, "GREEK CAPITAL LETTER BETA" },
	{ 0x0393, "GREEK CAPITAL LETTER GAMMA" },
	{ 0x0394, "GREEK CAPITAL LETTER DELTA" },
	{ 0x0395, "GREEK CAPITAL LETTER EPSILON" },
	{ 0x0396, "GREEK CAPITAL LETTER ZETA" },
	{ 0x0397, "GREEK CAPITAL LETTER ETA" },
	{ 0x0398, "GREEK CAPITAL LETTER THETA" },
	{ 0x0399, "GREEK CAPITAL LETTER IOTA" },
	{ 0x039a, "GREEK CAPITAL LETTER KAPPA" },
	{ 0x039b, "GREEK CAPITAL LETTER LAMDA" },
	{ 0x039c, "GREEK CAPITAL LETTER MU" },
	{ 0x039d, "GREEK CAPITAL LETTER NU" },
	{ 0x039e, "GREEK CAPITAL LETTER XI" },
	{ 0x039f, "GREEK CAPITAL LETTER OMICRON" },
	{ 0x03a0, "GREEK CAPITAL LETTER PI" },
	{ 0x03a1, "GREEK CAPITAL LETTER RHO" },
	{ 0x03a3, "GREEK CAPITAL LETTER SIGMA" },
	{ 0x03a4, "GREEK CAPITAL LETTER TAU" },
	{ 0x03a5, "GREEK CAPITAL LETTER UPSILON" },
	{ 0x03a6, "GREEK CAPITAL LETTER PHI" },
	{ 0x03a7, "GREEK CAPITAL LETTER CHI" },
	{ 0x03a8, "GREEK CAPITAL LETTER PSI" },
	{ 0x03a9, "GREEK CAPITAL LETTER OMEGA" },
	{ 0x03aa, "GREEK CAPITAL LETTER IOTA WITH DIALYTIKA" },
	{ 0x03ab, "GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA" },
	{ 0x03ac, "GREEK SMALL LETTER ALPHA WITH TONOS" },
	{ 0x03ad, "GREEK SMALL LETTER EPSILON WITH TONOS" },
	{ 0x03ae, "GREEK SMALL LETTER ETA WITH TONOS" },
	{ 0x03af, "GREEK SMALL LETTER IOTA WITH TONOS" },
	{ 0x03b0, "GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS" },
	{ 0x03b1, "GREEK SMALL LETTER ALPHA" },
	{ 0x03b2, "GREEK SMALL LETTER BETA" },
	{ 0x03b3, "GREEK SMALL LETTER GAMMA" },
	{ 0x03b4, "GREEK SMALL LETTER DELTA" },
	{ 0x03b5, "GREEK SMALL LETTER EPSILON" },
	{ 0x03b6, "GREEK SMALL LETTER ZETA" },
	{ 0x03b7, "GREEK SMALL LETTER ETA" },
	{ 0x03b8, "GREEK SMALL LETTER THETA" },
	{ 0x03b9, "GREEK SMALL LETTER IOTA" },
	{ 0x03ba, "GREEK SMALL LETTER KAPPA" },
	{ 0x03bb, "GREEK SMALL LETTER LAMDA" },
	{ 0x03bc, "GREEK SMALL LETTER MU" },
	{ 0x03bd, "GREEK SMALL LETTER NU" },
	{ 0x03be, "GREEK SMALL LETTER XI" },
	{ 0x03bf, "GREEK SMALL LETTER OMICRON" },
	{ 0x03c0, "GREEK SMALL LETTER PI" },
	{ 0x03c1, "GREEK SMALL LETTER RHO" },
	{ 0x03c2, "GREEK SMALL LETTER FINAL SIGMA" },
	{ 0x03c3, "GREEK SMALL LETTER SIGMA" },
	{ 0x03c4, "GREEK SMALL LETTER TAU" },
	{ 0x03c5, "GREEK SMALL LETTER UPSILON" },
	{ 0x03c6, "GREEK SMALL LETTER PHI" },
	{ 0x03c7, "GREEK SMALL LETTER CHI" },
	{ 0x03c8, "GREEK SMALL LETTER PSI" },
	{ 0x03c9, "GREEK SMALL LETTER OMEGA" },
	{ 0x03ca, "GREEK SMALL LETTER IOTA WITH DIALYTIKA" },
	{ 0x03cb, "GREEK SMALL LETTER UPSILON WITH DIALYTIKA" },
	{ 0x03cc, "GREEK SMALL LETTER OMICRON WITH TONOS" },
	{ 0x03cd, "GREEK SMALL LETTER UPSILON WITH TONOS" },
	{ 0x03ce, "GREEK SMALL LETTER OMEGA WITH TONOS" },
	{ 0x05d0, "HEBREW LETTER ALEF" },
	{ 0x05d1, "HEBREW LETTER BET" },
	{ 0x05d2, "HEBREW LETTER GIMEL" },
	{ 0x05d3, "HEBREW LETTER DALET" },
	{ 0x05d4, "HEBREW LETTER HE" },
	{ 0x05d5, "HEBREW LETTER VAV" },
	{ 0x05d6, "HEBREW LETTER ZAYIN" },
	{ 0x05d7, "HEBREW LETTER HET" },
	{ 0x05d8, "HEBREW LETTER TET" },
	{ 0x05d9, "HEBREW LETTER YOD" },
	{ 0x05da, "HEBREW LETTER FINAL KAF" },
	{ 0x05db, "HEBREW LETTER KAF" },
	{ 0x05dc, "HEBREW LETTER LAMED" },
	{ 0x05dd, "HEBREW LETTER FINAL MEM" },
	{ 0x05de, "HEBREW LETTER MEM" },
	{ 0x05df, "HEBREW LETTER FINAL NUN" },
	{ 0x05e0, "HEBREW LETTER NUN" },
	{ 0x05e1, "HEBREW LETTER SAMEKH" },
	{ 0x05e2, "HEBREW LETTER AYIN" },
	{ 0x05e3, "HEBREW LETTER FINAL PE" },
	{ 0x05e4, "HEBREW LETTER PE" },
	{ 0x05e5, "HEBREW LETTER FINAL TSADI" },
	{ 0x05e6, "HEBREW LETTER TSADI" },
	{ 0x05e7, "HEBREW LETTER QOF" },
	{ 0x05e8, "HEBREW LETTER RESH" },
	{ 0x05e9, "HEBREW LETTER SHIN" },
	{ 0x05ea, "HEBREW LETTER TAV" },
	{ 0x2013, "EN DASH" },
	{ 0x2014, "EM DASH" },
	{ 0x2018, "LEFT SINGLE QUOTATION MARK" },
	{ 0x2019, "RIGHT SINGLE QUOTATION MARK" },
	{ 0x201a, "SINGLE LOW-9 QUOTATION MARK" },
	{ 0x201c, "LEFT DOUBLE QUOTATION MARK" },
	{ 0x201d, "RIGHT DOUBLE QUOTATION MARK" },
	{ 0x201e, "DOUBLE LOW-9 QUOTATION MARK" },
	{ 0x2020, "DAGGER" },
	{ 0x2021, "DOUBLE DAGGER" },
	{ 0x2022, "BULLET" },
	{ 0x2026, "HORIZONTAL ELLIPSIS" },
	{ 0x2030, "PER MILLE SIGN" },
	{ 0x2039, "SINGLE LEFT-POINTING ANGLE QUOTATION MARK" },
	{ 0x203a, "SINGLE RIGHT-POINTING ANGLE QUOTATION MARK" },
	{ 0x207f, "SUPERSCRIPT LATIN SMALL LETTER N" },
	{ 0x20ac, "EURO SIGN" },
	{ 0x2122, "TRADE MARK SIGN" },
	{ 0x21e6, "LEFTWARDS WHITE ARROW" },
	{ 0x21e7, "UPWARDS WHITE ARROW" },
	{ 0x21e8, "RIGHTWARDS WHITE ARROW" },
	{ 0x21e9, "DOWNWARDS WHITE ARROW" },
	{ 0x2206, "INCREMENT" },
	{ 0x2208, "ELEMENT OF" },
	{ 0x2219, "BULLET OPERATOR" },
	{ 0x221a, "SQUARE ROOT" },
	{ 0x221e, "INFINITY" },
	{ 0x2227, "LOGICAL AND" },
	{ 0x2229, "INTERSECTION" },
	{ 0x222e, "CONTOUR INTEGRAL" },
	{ 0x2248, "ALMOST EQUAL TO" },
	{ 0x2261, "IDENTICAL TO" },
	{ 0x2264, "LESS-THAN OR EQUAL TO" },
	{ 0x2265, "GREATER-THAN OR EQUAL TO" },
	{ 0x2310, "REVERSED NOT SIGN" },
	{ 0x2320, "TOP HALF INTEGRAL" },
	{ 0x2321, "BOTTOM HALF INTEGRAL" },
	{ 0x237e, "BELL SYMBOL" },
};

static struct {
	unsigned short code;
	unsigned short display_code;
} const unicode_display[] = {
	{ 0x0000, 0x2400 },
	{ 0x0001, 0x2401 },
	{ 0x0002, 0x2402 },
	{ 0x0003, 0x2403 },
	{ 0x0004, 0x2404 },
	{ 0x0005, 0x2405 },
	{ 0x0006, 0x2406 },
	{ 0x0007, 0x2407 },
	{ 0x0008, 0x2408 },
	{ 0x0009, 0x2409 },
	{ 0x000a, 0x240a },
	{ 0x000b, 0x240b },
	{ 0x000c, 0x240c },
	{ 0x000d, 0x240d },
	{ 0x000e, 0x240e },
	{ 0x000f, 0x240f },
	{ 0x0010, 0x2410 },
	{ 0x0011, 0x2411 },
	{ 0x0012, 0x2412 },
	{ 0x0013, 0x2413 },
	{ 0x0014, 0x2414 },
	{ 0x0015, 0x2415 },
	{ 0x0016, 0x2416 },
	{ 0x0017, 0x2417 },
	{ 0x0018, 0x2418 },
	{ 0x0019, 0x2419 },
	{ 0x001a, 0x241a },
	{ 0x001b, 0x241b },
	{ 0x001c, 0x241c },
	{ 0x001d, 0x241d },
	{ 0x001e, 0x241e },
	{ 0x001f, 0x241f },
	{ 0x007f, 0x2421 },
};

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


static int gen_tooltip(FILE *out, const char *prefix, unsigned char code, int prev)
{
	unsigned short unicode = atari_to_unicode[codeset][code];
	unsigned int i;
	unsigned short display_code;
	
	if (prev)
		fprintf(out, "&#x0a;");
	display_code = unicode;
	for (i = 0; i < ARRAY_SIZE(unicode_display); i++)
		if (unicode_display[i].code == unicode)
		{
			display_code = unicode_display[i].display_code;
			break;
		}
	fprintf(out, "%s&#x%04x; 0x%02X U+%04x ", prefix, display_code, code, unicode);
	for (i = 0; i < ARRAY_SIZE(unicode_table); i++)
	{
		if (unicode == unicode_table[i].code)
		{
			fputs(unicode_table[i].name, out);
			break;
		}
	}
	if (i >= ARRAY_SIZE(unicode_table))
		fputs("<unknown>", out);
	return 1;
}


static void gen_tooltips(FILE *out, unsigned char scancode)
{
	if (keytab[TAB_UNSHIFT][scancode] != 0)
	{
		int prev;

		fprintf(out, " title=\"");
		prev = 0;
		prev = gen_tooltip(out, "", keytab[TAB_UNSHIFT][scancode], prev);
		if (keytab[TAB_SHIFT][scancode] != 0)
			prev = gen_tooltip(out, "Shift: ", keytab[TAB_SHIFT][scancode], prev );
		if (keytab[TAB_ALT][scancode] != 0)
			prev = gen_tooltip(out, "Alt: ", keytab[TAB_ALT][scancode], prev);
		if (keytab[TAB_SHALT][scancode] != 0)
			prev = gen_tooltip(out, "AltShift: ", keytab[TAB_SHALT][scancode], prev);
		fprintf(out, "\"");
	}
}


static int is_deadkey(unsigned char c, int deadkeys_format)
{
	int i;
	unsigned char deadchars[256 + 1];
	int n_deadchars;

	switch (deadkeys_format)
	{
	case FORMAT_MINT:
		n_deadchars = 0;
		deadchars[0] = 0;
		for (i = 0; i < tabsize[TAB_DEADKEYS] && deadkeys[i] != 0; i += 3)
		{
			if (strchr((char *) deadchars, deadkeys[i]) == NULL)
			{
				deadchars[n_deadchars++] = deadkeys[i];
				deadchars[n_deadchars] = 0;
			}
		}
		for (i = 0; i < n_deadchars; i++)
			if (c == deadchars[i])
				return i;
		break;
	case FORMAT_MAGIC:
		for (i = 0; i < tabsize[TAB_DEADKEYS] && deadkeys[i] != 0; i++)
			if (c == deadkeys[i])
				return i;
		break;
	}
	return -1;
}


static void write_html_src(FILE *out, int deadkeys_format)
{
	unsigned int i;
	int row = 0;
	int start_row = 1;
	
#define INT_SPACE(n) ((n) < 10 ? 2 : (n) < 100 ? 1 : 0), ""

fputs("\
<div class=\"keyboard\">\n\
<div class=\"keyboard-bg\">\n\
\n\
", out);

	for (i = 0; i < ARRAY_SIZE(layout_table); i++)
	{
		const char *keylabel;
		const char *keyclass;
		unsigned char scancode = layout_table[i].scancode;
		int w;
		int deadkey;

		if (start_row)
		{
			++row;
			fprintf(out, "<div class=\"kr%d\">\n", row);
			start_row = 0;
		}
		
		if (scancode == 0)
		{
			fprintf(out, "</div>\n\n");
			start_row = 1;
			continue;
		}
		/*
		 * skip unmapped keys, but keep keys like Shift/Control etc.
		 */
		if (keytab[TAB_UNSHIFT][scancode] == 0 && layout_table[i].keylabel == NULL)
			continue;
		
		w = layout_table[i].w;
		if (w == 0)
			w = 54;
		keyclass = "kl_center";
		keylabel = layout_table[i].keylabel;
		if (keylabel == NULL)
		{
			keylabel = get_keylabel(keytab[TAB_UNSHIFT][scancode]);
			keyclass = "kl_bottomleft";
		}
		/*
		 * if the key right to the left Shift does not exist, make
		 * the shift key larger
		 */
		if (scancode == 0x2a && keytab[TAB_UNSHIFT][0x60] == 0)
			w += 54;

		deadkey = is_deadkey(keytab[TAB_UNSHIFT][scancode], deadkeys_format);

		fprintf(out, "<div class=\"key%s\">\n", scancode >= 0x3b && scancode <= 0x3d ? " skew" : "");

		if (scancode == 0x1c)
		{
			/* return key is special */
			fprintf(out, "  <div class=\"kb\" style=\"left: 729px; width:54px; height:108px\"></div>\n");
			fprintf(out, "  <div class=\"kb\" style=\"left: 702px; width:81px; margin-top: 54px\"></div>\n");
			fprintf(out, "  <div class=\"kb\" style=\"left: 730px; width:52px; height:100px; margin-top: 1px; margin-right: 2px; border-style: none;\"></div>\n");
			fprintf(out, "  <div class=\"kt\" style=\"left: 735px; width:42px; height:96px; border-style: none !important; margin-top: 1px;\"></div>\n");
			fprintf(out, "  <div class=\"kt\" style=\"left: 708px; width:69px; height:42px; border-style: none !important; margin-top: 55px;\"");
			gen_tooltips(out, scancode);
			fprintf(out, "></div>\n");
			fprintf(out, "      <div class=\"kls\" style=\"left: 708px; width: 69px; height: 100px\">\n");
			fprintf(out, "        <div class=\"kl ts4\" style=\"width:%dpx; margin-top: 55px;\">\n", w);
			fprintf(out, "          <div class=\"%s\">%s%s%s</div>\n", keyclass, deadkey >= 0 ? "<span class=\"dead\">" : "", keylabel, deadkey >= 0 ? "</span>" : "");
			fprintf(out, "        </div>\n");
			fprintf(out, "        <div class=\"kl scancode\">\n");
			fprintf(out, "          <div title=\"%02X\">%02X</div>\n", scancode, scancode);
			fprintf(out, "        </div>\n");
			fprintf(out, "      </div>\n");
		} else
		{
			fprintf(out, "  <div class=\"kb\" style=\"left: %dpx; width:%dpx;%s\"", layout_table[i].x, w, scancode == 0x72 ? " height:108px;" : "");
			gen_tooltips(out, scancode);
			fprintf(out, ">\n");
			
			fprintf(out, "    <div class=\"kt\" style=\"width:\%dpx;%s\">\n", w - 12, scancode == 0x72 ? " height:96px;" : "");
			fprintf(out, "      <div class=\"kls\" style=\"width:\%dpx;%s\">\n", w - 12, scancode == 0x72 ? " height:96px;" : "");
			fprintf(out, "        <div class=\"kl %s\" style=\"width:%dpx\">\n", scancode == 0x0e ? "ts3" : "ts4", w);
			fprintf(out, "          <div class=\"%s\">%s%s%s</div>\n", keyclass, deadkey >= 0 ? "<span class=\"dead\">" : "", keylabel, deadkey >= 0 ? "</span>" : "");
			fprintf(out, "        </div>\n");
			if (layout_table[i].keylabel == NULL)
			{
				if (keytab[TAB_SHIFT][scancode] != 0 &&
					keytab[TAB_SHIFT][scancode] != keytab[TAB_UNSHIFT][scancode])
				{
					keylabel = get_keylabel(keytab[TAB_SHIFT][scancode]);
					deadkey = is_deadkey(keytab[TAB_SHIFT][scancode], deadkeys_format);
					fprintf(out, "        <div class=\"kl ts4\" style=\"width:%dpx\">\n", w);
					fprintf(out, "          <div class=\"kl_top_left\">%s%s%s</div>\n", deadkey >= 0 ? "<span class=\"dead\">" : "", keylabel, deadkey >= 0 ? "</span>" : "");
					fprintf(out, "        </div>\n");
				}
				if (keytab[TAB_SHALT][scancode] != 0 && keytab[TAB_ALT][scancode] != keytab[TAB_SHALT][scancode])
				{
					keylabel = get_keylabel(keytab[TAB_SHALT][scancode]);
					deadkey = is_deadkey(keytab[TAB_SHALT][scancode], deadkeys_format);
					fprintf(out, "        <div class=\"kl ts4\" style=\"width:%dpx\">\n", w);
					fprintf(out, "          <div class=\"kl_topright\">%s%s%s</div>\n", deadkey >= 0 ? "<span class=\"dead\">" : "", keylabel, deadkey >= 0 ? "</span>" : "");
					fprintf(out, "        </div>\n");
				}
				if (keytab[TAB_ALT][scancode] != 0)
				{
					keylabel = get_keylabel(keytab[TAB_ALT][scancode]);
					deadkey = is_deadkey(keytab[TAB_ALT][scancode], deadkeys_format);
					fprintf(out, "        <div class=\"kl ts4\" style=\"width:%dpx\">\n", w);
					fprintf(out, "          <div class=\"kl_bottomright\">%s%s%s</div>\n", deadkey >= 0 ? "<span class=\"dead\">" : "", keylabel, deadkey >= 0 ? "</span>" : "");
					fprintf(out, "        </div>\n");
				}
			}
			fprintf(out, "        <div class=\"kl scancode\">\n");
			fprintf(out, "          <div title=\"%02X\">%02X</div>\n", scancode, scancode);
			fprintf(out, "        </div>\n");
			fprintf(out, "      </div>\n");
			fprintf(out, "    </div>\n");
			fprintf(out, "  </div>\n");
		}

		fprintf(out, "</div>\n");
	}

	fprintf(out, "</div>\n");
	fprintf(out, "</div>\n");
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
	int is_mint;
	int mint_expected;
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
	for (tab = 0; tab <= N_KEYTBL; tab++)
		tabsize[tab] = 0;
	lineno = 0;
	tab = -1;
	w = 0;
	is_mint = 0;
	mint_expected = 0;

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
				if (is_mint == 1)
				{
					/* check the AKP code */
					switch ((unsigned int) num)
					{
					case 15: /* Czech */
					case 16: /* Hungarian */
					case 17: /* Polish */
						codeset = 1;
						break;
					}
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

	write_html_src(out, deadkeys_format);

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
