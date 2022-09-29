/* fntdump
 * This utility converts GEM Bitmap fonts (.FNT) to C sourcecode
 *
 * Written by Thorsten Otto (May, 2017)
 */
/*
 * do not used config values for target when cross-compiling
 */
#undef HAVE_CONFIG_H

#include <portab.h>
#include <s_endian.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <getopt.h>
#include "fonthdr.h"


static char const program_name[] = "fntdump";
static gboolean info = FALSE;
static gboolean verbose = FALSE;
static gboolean quiet = FALSE;
static int override_id = 0;


#ifndef EXIT_FAILURE
#  define EXIT_FAILURE 1
#  define EXIT_SUCCESS 0
#endif

typedef int8_t B;
typedef uint8_t UB;
typedef int16_t W;
typedef uint16_t UW;
typedef int32_t L;
typedef uint32_t UL;

static INLINE B *TO_B(void *s) { return (B *)s; }
static INLINE UB *TO_UB(void *s) { return (UB *)s; }
static INLINE W *TO_W(void *s) { return (W *)s; }
static INLINE UW *TO_UW(void *s) { return (UW *)s; }
static INLINE L *TO_L(void *s) { return (L *)s; }
static INLINE UL *TO_UL(void *s) { return (UL *)s; }

/* Load/Store primitives (without address checking) */
#define LOAD_B(_s) (*(TO_B(_s)))
#define LOAD_UB(_s) (*(TO_UB(_s)))
#define LOAD_W(_s) (*(TO_W(_s)))
#define LOAD_UW(_s) (*(TO_UW(_s)))
#define LOAD_L(_s) (*(TO_L(_s)))
#define LOAD_UL(_s) (*(TO_UL(_s)))

#define STORE_B(_d,_v) *(TO_B(_d)) = _v
#define STORE_UB(_d,_v)	*(TO_UB(_d)) = _v
#define STORE_W(_d,_v) *(TO_W(_d)) = _v
#define STORE_UW(_d,_v) *(TO_UW(_d)) = _v
#define STORE_L(_d,_v) *(TO_L(_d)) = _v
#define STORE_UL(_d,_v) *(TO_UL(_d)) = _v

#define LM_B LOAD_B
#define LM_UB LOAD_UB
#define SM_B STORE_B
#define SM_UB STORE_UB

#define SWAP_W(s) STORE_UW(s, ((UW)bswap_16(LOAD_UW(s))))
#define SWAP_L(s) STORE_UL(s, ((UL)bswap_32(LOAD_UL(s))))

static UB *m;


static void swap_gemfnt_header(UB *h, unsigned int l)
{
	UB *u;
	
	if (l < 84)
		return;
	SWAP_W(h + 0); /* font_id */
	SWAP_W(h + 2); /* point */
	/* skip name */
	for (u = h + 36; u < h + 68; u += 2) /* first_ade .. flags */
	{
		SWAP_W(u);
	}
	SWAP_L(h + 68); /* hor_table */
	SWAP_L(h + 72); /* off_table */
	SWAP_L(h + 76); /* dat_table */
	SWAP_W(h + 80); /* form_width */
	SWAP_W(h + 82); /* form_height */
}


/*
 * There are apparantly several fonts that have the Motorola flag set
 * but are stored in little-endian format.
 */
static gboolean check_gemfnt_header(UB *h, unsigned int l)
{
	UW firstc, lastc, points;
	UW form_width, form_height;
	UW cellwidth;
	UL dat_offset;
	UL off_table;
	
	if (l < 84)
		return FALSE;
	firstc = LOAD_UW(h + 36);
	lastc = LOAD_UW(h + 38);
	points = LOAD_UW(h + 2);
	if (lastc == 256)
	{
		lastc = 255;
	}
	if (firstc >= 0x2000 || lastc >= 0xff00 || firstc > lastc)
		return FALSE;
	if (points >= 0x300)
		return FALSE;
	if (LOAD_UL(h + 68) >= l)
		return FALSE;
	off_table = LOAD_UL(h + 72);
	if (off_table < 84 || (off_table + (lastc - firstc + 1) * 2) > l)
		return FALSE;
	dat_offset = LOAD_UL(h + 76);
	if (dat_offset < 84 || dat_offset >= l)
		return FALSE;
	cellwidth = LOAD_UW(h + 52);
	if (cellwidth == 0)
		return FALSE;
	form_width = LOAD_UW(h + 80);
	form_height = LOAD_UW(h + 82);
	if ((dat_offset + form_width * form_height) > l)
		return FALSE;
	STORE_UW(h + 38, lastc);
	return TRUE;
}


static void chomp(char *dst, const char *src, size_t maxlen)
{
	size_t len;
	
	strncpy(dst, src, maxlen);
	dst[maxlen - 1] = '\0';
	len = strlen(dst);
	while (len > 0 && dst[len - 1] == ' ')
		dst[--len] = '\0';
	while (len > 0 && dst[0] == ' ')
	{
		memmove(dst, dst + 1, len);
		len--;
	}
}


static void fnttoc(UB *b, int l, FILE *out, const char *filename, const char *name)
{
	UB *hor_table;
	UB *off_table;
	UB *dat_table;
	int i, count;
	UB *h = b;
	int numoffs;
	UW flags;
	UW mono;
	UW form_width, form_height;
	UW firstc, lastc;
	W top;
	W max_cell_width;
	char facename_buf[VDI_FONTNAMESIZE + 1];
	
#define HOST_BIG (HOST_BYTE_ORDER == BYTE_ORDER_BIG_ENDIAN)
#define FONT_BIG ((LOAD_UW(h + 66) & 0x04) != 0)

	if (!check_gemfnt_header(h, l))
	{
		swap_gemfnt_header(h, l);
		if (!check_gemfnt_header(h, l))
		{
			swap_gemfnt_header(h, l);
			fprintf(stderr, "%s: %s: invalid font header\n", program_name, filename);
			if (!info)
				exit(EXIT_FAILURE);
		} else
		{
			if (HOST_BIG == FONT_BIG)
			{
				if (!quiet)
					fprintf(stderr, "%s: warning: %s: wrong endian flag in header\n", program_name, filename);
				if (HOST_BIG)
					STORE_W(h + 66, LOAD_UW(h + 66) & ~0x0004);
				else
					STORE_W(h + 66, LOAD_UW(h + 66) | 0x0004);
			}
		}
	} else
	{
		if (HOST_BIG != FONT_BIG)
		{
			if (!quiet)
				fprintf(stderr, "%s: warning: %s: wrong endian flag in header\n", program_name, filename);
			if (HOST_BIG)
				STORE_W(h + 66, LOAD_UW(h + 66) | 0x0004);
			else
				STORE_W(h + 66, LOAD_UW(h + 66) & ~0x0004);
		}
	}
	
	firstc = LOAD_UW(h + 36);
	lastc = LOAD_UW(h + 38);

	flags = LOAD_UW(h + 66);
	form_width = LOAD_UW(h + 80);
	form_height = LOAD_UW(h + 82);
	
	top = LOAD_W(h + 40);
	max_cell_width = LOAD_W(h + 52);
	
	chomp(facename_buf, (const char *)h + 4, VDI_FONTNAMESIZE);
	
	hor_table = h + LOAD_UL(h + 68);
	off_table = h + LOAD_UL(h + 72);
	dat_table = h + LOAD_UL(h + 76);

	numoffs = lastc - firstc + 1;

	if ((flags & FONTF_HORTABLE) && hor_table != h && hor_table != off_table && (off_table - hor_table) >= (numoffs * 2))
	{
	} else
	{
		flags &= ~FONTF_HORTABLE;
		hor_table = NULL;
	}
	
	if (info)
	{
		fprintf(out, "Filename: %s\n", filename);
		fprintf(out, "Name: %s\n", facename_buf);
		fprintf(out, "Id: %d\n", LOAD_UW(h + 0));
		fprintf(out, "Size: %dpt\n", LOAD_UW(h + 2));
		fprintf(out, "First ade: %d\n", firstc);
		fprintf(out, "Last ade: %d\n", lastc);
		fprintf(out, "Top: %d\n", top);
		fprintf(out, "Ascent: %d\n", LOAD_W(h + 42));
		fprintf(out, "Half: %d\n", LOAD_W(h + 44));
		fprintf(out, "Descent: %d\n", LOAD_W(h + 46));
		fprintf(out, "Bottom: %d\n", LOAD_W(h + 48));
		fprintf(out, "Max charwidth: %d\n", LOAD_UW(h + 50));
		fprintf(out, "Max cellwidth: %d\n", max_cell_width);
		fprintf(out, "Left offset: %d\n", LOAD_W(h + 54));
		fprintf(out, "Right offset: %d\n", LOAD_W(h + 56));
		fprintf(out, "Thicken: %d\n", LOAD_UW(h + 58));
		fprintf(out, "Underline size: %d\n", LOAD_UW(h + 60));
		fprintf(out, "Lighten: $%x\n", LOAD_UW(h + 62));
		fprintf(out, "Skew: $%x\n", LOAD_UW(h + 64));
		fprintf(out, "Flags: $%x (%s%s%s-endian %s%s)\n", flags,
			flags & FONTF_SYSTEM ? "system " : "",
			flags & FONTF_HORTABLE ? "offsets " : "",
			flags & FONTF_BIGENDIAN ? "big" : "little",
			flags & FONTF_MONOSPACED ? "monospaced" : "proportional",
			flags & FONTF_EXTENDED ? " extended" : "");
		if (flags & FONTF_HORTABLE)
			fprintf(out, "Horizontal table: %u\n", LOAD_UL(h + 68));
		else
			fprintf(out, "Horizontal table: none\n");
		fprintf(out, "Offset table: %u\n", LOAD_UL(h + 72));
		fprintf(out, "Data: %u\n", LOAD_UL(h + 76));
		fprintf(out, "Form width: %d\n", form_width);
		fprintf(out, "Form height: %d\n", form_height);
		return;
	}
	
	if (HOST_BIG != FONT_BIG)
	{
		UB *u;
		
		for (u = off_table; u <= off_table + numoffs * 2; u += 2)
		{
			SWAP_W(u);
		}
	}
	
	if ((flags & FONTF_HORTABLE) && hor_table != h && hor_table != off_table && (off_table - hor_table) >= (numoffs * 2))
	{
		UB *u;
		
		if (HOST_BIG != FONT_BIG)
		{
			for (u = hor_table; u < hor_table + numoffs * 2; u += 2)
			{
				SWAP_W(u);
			}
		}
	}
	
	mono = TRUE;
	{
		int o, w, firstw;
		
		o = LOAD_UW(off_table + 2 * 0);
		firstw = LOAD_UW(off_table + 2 * 0 + 2) - o;
		for (i = 0; i < numoffs; i++)
		{
			o = LOAD_UW(off_table + 2 * i);
			w = LOAD_UW(off_table + 2 * i + 2) - o;
			if (w != 0 && w != firstw)
			{
				mono = FALSE;
				if (flags & FONTF_MONOSPACED)
				{
					if (!quiet)
						fprintf(stderr, "%s: warning: %s: font says it is monospaced, but isn't\n", program_name, filename);
					flags &= ~FONTF_MONOSPACED;
					STORE_UW(h + 66, flags);
				}
				break;
			}
		}
		if (mono && !(flags & FONTF_MONOSPACED))
		{
			if (!quiet)
				fprintf(stderr, "%s: warning: %s: font does not say it is monospaced, but is\n", program_name, filename);
			flags |= FONTF_MONOSPACED;
			STORE_UW(h + 66, flags);
		}
	}
	
	if (verbose)
		fprintf(stderr, "Writing font %s (chars %d..%d)\n", facename_buf, firstc, lastc);
	fprintf(out, "#include \"config.h\"\n");
	fprintf(out, "#include <portab.h>\n");
	fprintf(out, "#include <fonthdr.h>\n");
	fprintf(out, "\n");
	fprintf(out, "static uint16_t const %s_off_table[] = {\n", name);
	for (i = 0; i <= numoffs; )
	{
		int o = LOAD_UW(off_table + 2 * i);
		
		if ((i % 8) == 0)
			fprintf(out, "\t");
		
		fprintf(out, "0x%04x", o);
		i++;
		if (i <= numoffs)
		{
			putc(',', out);
			if ((i % 8) == 0)
				putc('\n', out);
			else
				putc(' ', out);
		}
	}
	putc('\n', out);
	fprintf(out, "};\n");
	fprintf(out, "\n");

	if (flags & FONTF_HORTABLE)
	{
		fprintf(out, "\n");
		fprintf(out, "static uint16_t const %s_hor_table[] = {\n", name);
		for (i = 0; i < numoffs; )
		{
			int o = LOAD_UW(hor_table + 2 * i);
			
			if ((i % 8) == 0)
				fprintf(out, "\t");
			
			fprintf(out, "0x%04x", o);
			i++;
			if (i < numoffs)
			{
				putc(',', out);
				if ((i % 8) == 0)
					putc('\n', out);
				else
					putc(' ', out);
			}
		}
		putc('\n', out);
		fprintf(out, "};\n");
		fprintf(out, "\n");
	}

	fprintf(out, "\n");
	fprintf(out, "static uint8_t const %s_dat_table[] = {\n", name);
	count = form_width * form_height;
	for (i = 0; i < count; )
	{
		int o = LOAD_UB(dat_table + i);
		
		if ((i % 16) == 0)
			fprintf(out, "\t");
		
		fprintf(out, "0x%02x", o);
		i++;
		if (i < count)
		{
			putc(',', out);
			if ((i % 16) == 0)
				putc('\n', out);
			else
				putc(' ', out);
		}
	}
	putc('\n', out);
	fprintf(out, "};\n");
	fprintf(out, "\n");

	fprintf(out, "\n");
	fprintf(out, "static FONT_HDR const %s = {\n", name);
	fprintf(out, "\t%d,\t\t/* font_id */\n", override_id ? override_id : LOAD_UW(h + 0));
	fprintf(out, "\t%d,\t\t/* point */\n", LOAD_UW(h + 2));
	fprintf(out, "\t\"%s\",\t\t/* name */\n", facename_buf);
	fprintf(out, "\t%u,\t\t/* first_ade */\n", firstc);
	fprintf(out, "\t%u,\t/* last_ade */\n", lastc);
	fprintf(out, "\t%d,\t\t/* top */\n", top);
	fprintf(out, "\t%d,\t\t/* ascent */\n", LOAD_W(h + 42));
	fprintf(out, "\t%d,\t\t/* half */\n", LOAD_W(h + 44));
	fprintf(out, "\t%d,\t\t/* descent */\n", LOAD_W(h + 46));
	fprintf(out, "\t%d,\t\t/* bottom */\n", LOAD_W(h + 48));
	fprintf(out, "\t%d,\t\t/* max_char_width */\n", LOAD_W(h + 50));
	fprintf(out, "\t%d,\t\t/* max_cell_width */\n", max_cell_width);
	fprintf(out, "\t%d,\t\t/* left_offset */\n", LOAD_W(h + 54));
	fprintf(out, "\t%d,\t\t/* right_offset */\n", LOAD_W(h + 56));
	fprintf(out, "\t%d,\t\t/* thicken */\n", LOAD_W(h + 58));
	fprintf(out, "\t%d,\t\t/* ul_size */\n", LOAD_W(h + 60));
	fprintf(out, "\t0x%04x,\t/* lighten */\n", LOAD_UW(h + 62));
	fprintf(out, "\t0x%04x,\t/* skew */\n", LOAD_UW(h + 64));
	fprintf(out, "\t0x%04x,\t/* flags */\n", flags);
	if (flags & FONTF_HORTABLE)
		fprintf(out, "\t%s_hor_table", name);
	else
		fprintf(out, "\tNULL");
	fprintf(out, ",\t/* hor_table */\n");
	fprintf(out, "\t%s_off_table,\t/* off_table */\n", name);
	fprintf(out, "\t%s_dat_table,\t/* dat_table */\n", name);
	fprintf(out, "\t%d,\t/* form_width */\n", form_width);
	fprintf(out, "\t%d,\t\t/* form_height */\n", form_height);
	fprintf(out, "\tNULL\n");
	fprintf(out, "};\n");
}



static struct option const long_options[] = {
	{ "info", no_argument, NULL, 'i' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "quiet", no_argument, NULL, 'q' },
	{ "name", required_argument, NULL, 'n' },
	{ "id", required_argument, NULL, 'I' },
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'V' },
	{ NULL, no_argument, NULL, 0 }
};


static void usage(FILE *fp)
{
	UNUSED(fp);
}


static void print_version(void)
{
}


int main(int argc, char **argv)
{
	FILE *in, *out;
	int l;
	const char *filename = NULL;
	const char *name = "fnt_st";
	int c;
	
	while ((c = getopt_long(argc, argv, "tiI:vqn:hV", long_options, NULL)) != EOF)
	{
		switch (c)
		{
		case 'i':
			info = TRUE;
			break;
		case 'I':
			override_id = (int)strtol(optarg, NULL, 0);
			break;
		case 'v':
			verbose = TRUE;
			break;
		case 'q':
			quiet = TRUE;
			break;
		case 'n':
			name = optarg;
			break;
		case 'h':
			usage(stdout);
			exit(EXIT_SUCCESS);
			break;
		case 'V':
			print_version();
			exit(EXIT_SUCCESS);
			break;
		default:
			exit(EXIT_FAILURE);
			break;
		}
	}

	switch (argc - optind)
	{
	case 1:
		break;
	case 0:
		fprintf(stderr, "%s: no files specified\n", program_name);
		exit(EXIT_FAILURE);
		break;
	default:
		if (!info)
		{
			fprintf(stderr, "%s: too many files specified\n", program_name);
			exit(EXIT_FAILURE);
		}
		break;
	}

	while (optind < argc)
	{
		filename = argv[optind];
		in = fopen(filename, "rb");
		if (in == NULL)
		{
			fprintf(stderr, "%s: ", program_name);
			perror(filename);
			exit(EXIT_FAILURE);
		}
		out = stdout;
		fseek(in, 0, SEEK_END);
		l = ftell(in);
		fseek(in, 0, SEEK_SET);
		m = (UB *)malloc(l);
		l = fread(m, 1, l, in);
	
		fnttoc(m, l, out, filename, name);
		fclose(in);
		free(m);
		optind++;
		if (info && optind < argc)
			fputc('\n', out);
	}
		
	return EXIT_SUCCESS;
}
