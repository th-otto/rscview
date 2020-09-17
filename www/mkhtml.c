/*
 * This file belongs to FreeMiNT. It's not in the original MiNT 1.12
 * distribution. See the file CHANGES for a detailed log of changes.
 *
 *
 * Copyright 2020 Thorsten Otto
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
 * Author:	Thorsten Otto <admin@tho-otto.de>
 *
 * Please send suggestions, patches or bug reports to me or
 * the MiNT mailing list.
 *
 */

/*
 * Uses the MiNT keyboard table parser, and gernates HTML
 * code with the layout
 */

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "mktbl.h"
#include "unitable.h"

#ifndef FALSE
#  define FALSE 0
#  define TRUE  1
#endif

static struct {
	unsigned char scancode;
	int x;
	int w;
	const char *keylabel;
} const layout_table[] = {
	{ 0x3b,    0,  81, "F1" },
	{ 0x3c,   81,  81, "F2" },
	{ 0x3d,  162,  81, "F3" },
	{ 0x3e,  243,  81, "F4" },
	{ 0x3f,  324,  81, "F5" },
	{ 0x40,  405,  81, "F6" },
	{ 0x41,  486,  81, "F7" },
	{ 0x42,  567,  81, "F8" },
	{ 0x43,  648,  81, "F9" },
	{ 0x44,  729,  81, "F10" },
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

/******************************************************************************/
/* -------------------------------------------------------------------------- */
/******************************************************************************/

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
			sprintf(labelbuf, "&#x%04x;", atari_to_unicode[keytab_codeset][c]);
			return labelbuf;
		}
	}
	labelbuf[0] = c;
	labelbuf[1] = 0;
	return labelbuf;
}


static int gen_tooltip(FILE *out, const char *prefix, unsigned char code, int prev)
{
	unsigned short unicode = atari_to_unicode[keytab_codeset][code];
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
	fputs(unicode_name(unicode), out);
	return TRUE;
}


static void gen_tooltips(FILE *out, unsigned char scancode)
{
	if (keytab[TAB_UNSHIFT][scancode] != 0)
	{
		int prev;

		fprintf(out, " title=\"");
		prev = FALSE;
		prev = gen_tooltip(out, "", keytab[TAB_UNSHIFT][scancode], prev);
		if (keytab[TAB_SHIFT][scancode] != 0)
			prev = gen_tooltip(out, "Shift: ", keytab[TAB_SHIFT][scancode], prev );
		if (keytab[TAB_ALT][scancode] != 0)
			prev = gen_tooltip(out, "Alt: ", keytab[TAB_ALT][scancode], prev);
		if (keytab[TAB_ALTSHIFT][scancode] != 0)
			prev = gen_tooltip(out, "AltShift: ", keytab[TAB_ALTSHIFT][scancode], prev);
		fprintf(out, "\"");
	}
}


static void write_html_src(FILE *out)
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

		deadkey = is_deadkey(keytab[TAB_UNSHIFT][scancode]);

		fprintf(out, "<div class=\"key%s\">\n", scancode >= 0x3b && scancode <= 0x44 ? " skew" : "");

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
					deadkey = is_deadkey(keytab[TAB_SHIFT][scancode]);
					fprintf(out, "        <div class=\"kl ts4\" style=\"width:%dpx\">\n", w);
					fprintf(out, "          <div class=\"kl_top_left\">%s%s%s</div>\n", deadkey >= 0 ? "<span class=\"dead\">" : "", keylabel, deadkey >= 0 ? "</span>" : "");
					fprintf(out, "        </div>\n");
				}
				if (keytab[TAB_ALTSHIFT][scancode] != 0 && keytab[TAB_ALT][scancode] != keytab[TAB_ALTSHIFT][scancode])
				{
					keylabel = get_keylabel(keytab[TAB_ALTSHIFT][scancode]);
					deadkey = is_deadkey(keytab[TAB_ALTSHIFT][scancode]);
					fprintf(out, "        <div class=\"kl ts4\" style=\"width:%dpx\">\n", w);
					fprintf(out, "          <div class=\"kl_topright\">%s%s%s</div>\n", deadkey >= 0 ? "<span class=\"dead\">" : "", keylabel, deadkey >= 0 ? "</span>" : "");
					fprintf(out, "        </div>\n");
				}
				if (keytab[TAB_ALT][scancode] != 0)
				{
					keylabel = get_keylabel(keytab[TAB_ALT][scancode]);
					deadkey = is_deadkey(keytab[TAB_ALT][scancode]);
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


static char const progname[] = "mkhtml";


static void usage(FILE *fp)
{
	fprintf(fp, "Usage: %s src-file [html-file]\n", progname);
}


int main(int argc, char **argv)
{
	char *outname = NULL;
	FILE *infile;
	FILE *out;
	const char *filename;
	int r;

	/*
	 * very minimalistic option processing
	 */
	argc--;
	argv++;
	while (argc > 0 && argv[0][0] == '-')
	{
		if (argv[0][1] == '\0')
		{
			break;
		} else if (strcmp(argv[0], "--help") == 0)
		{
			usage(stdout);
			return EXIT_SUCCESS;
#ifdef WITH_CHECKS
		} else if (strcmp(argv[0], "--check") == 0)
		{
			return unitable_selfcheck() ? EXIT_SUCCESS : EXIT_FAILURE;
#endif
		} else if (strcmp(argv[0], "--") == 0)
		{
			argc--;
			argv++;
			break;
		} else
		{
			fprintf(stderr, "%s: unknown option %s\n", progname, argv[0]);
			return EXIT_FAILURE;
		}
	}

	if (argc == 0)
	{
		usage(stderr);
		return EXIT_FAILURE;
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
		return EXIT_FAILURE;
	}

	if (strcmp(filename, "-") == 0)
	{
		infile = stdin;
		filename = "<stdin>";
	} else
	{
		infile = fopen(filename, "rb");
	}
	if (infile == NULL)
	{
		fprintf(stderr, "%s: %s\n", filename, strerror(errno));
		return EXIT_FAILURE;
	}

	r = mktbl_parse(infile, filename);
	if (infile != stdin)
		fclose(infile);

	if (r == FALSE)
	{
		return EXIT_FAILURE;
	}

	if (outname == NULL && infile != stdin)
	{
		const char *ext = ".html";
		char *o;
		long flen;

		flen = strlen(filename);
		outname = malloc(flen + 6);
		if (outname == NULL)
		{
			fprintf(stderr, "%s\n", strerror(errno));
			return EXIT_FAILURE;
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
		return EXIT_FAILURE;
	}

	write_html_src(out);

	fflush(out);
	if (out != stdout)
		fclose(out);

	return EXIT_SUCCESS;
}
