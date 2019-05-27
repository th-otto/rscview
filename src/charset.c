#include "config.h"
#include <gem.h>
#include "ro_mem.h"
#include "nls.h"
#include "debug.h"

#include "cp_atari.h"
#include "cp_iso1.h"
#include "cp_st_l2.h"
#include "cp_st_l9.h"
#include "cp_st_gr.h"
#include "cp_st_ru.h"

#define nls_put_unichar(p, wc) \
	if (wc < 0x80) \
	{ \
		*p++ = wc; \
	} else if (wc < 0x800) \
	{ \
		p[1] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[0] = wc | 0xc0; \
		p += 2; \
	} else if (wc < 0x10000UL) \
	{ \
		p[2] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[1] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[0] = wc | 0xe0; \
		p += 3; \
	} else if (wc < 0x200000UL) \
	{ \
		p[3] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[2] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[1] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[0] = wc | 0xf0; \
		p += 4; \
	} else if (wc < 0x4000000UL) \
	{ \
		p[4] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[3] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[2] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[1] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[0] = wc | 0xf8; \
		p += 5; \
	} else \
	{ \
		p[5] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[4] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[3] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[2] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[1] = (wc & 0x3f) | 0x80; \
		wc >>= 6; \
		p[0] = wc | 0xfc; \
		p += 6; \
	}

static struct {
	nls_wchar_t uni;
	unsigned char ascii;
} const replacements[] = {
	{ 0x00a0, ' ' },	/* NO-BREAK SPACE */
	{ 0x00a6, '|' },	/* BROKEN BAR */
	{ 0x00ad, '-' },	/* SOFT HYPHEN */
	{ 0x00c1, 'A' },	/* A' LATIN CAPITAL LETTER A WITH ACUTE */
	{ 0x00c2, 'A' },	/* A^ LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
	{ 0x00c8, 'E' },	/* E` LATIN CAPITAL LETTER E WITH GRAVE */
	{ 0x00ca, 'E' },	/* E^ LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
	{ 0x00cb, 'E' },	/* E" LATIN CAPITAL LETTER E WITH DIAERESIS */
	{ 0x00cc, 'I' },	/* I` LATIN CAPITAL LETTER I WITH GRAVE */
	{ 0x00cd, 'I' },	/* I' LATIN CAPITAL LETTER I WITH ACUTE */
	{ 0x00ce, 'I' },	/* I^ LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
	{ 0x00cf, 'I' },	/* I" LATIN CAPITAL LETTER I WITH DIAERESIS */
	{ 0x00d2, 'O' },	/* O` LATIN CAPITAL LETTER O WITH GRAVE */
	{ 0x00d3, 'O' },	/* O' LATIN CAPITAL LETTER O WITH ACUTE */
	{ 0x00d4, 'O' },	/* O^ LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
	{ 0x00d7, 'x' },	/* MULTIPLICATION SIGN */
	{ 0x00d9, 'U' },	/* U` LATIN CAPITAL LETTER U WITH GRAVE */
	{ 0x00da, 'U' },	/* U' LATIN CAPITAL LETTER U WITH ACUTE */
	{ 0x00db, 'U' },	/* U^ LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
	{ 0x00dd, 'Y' },	/* Y' LATIN CAPITAL LETTER Y WITH ACUTE */
	{ 0x00fd, 'y' },	/* y' LATIN SMALL LETTER Y WITH ACUTE */
	{ 0x0000, '?' }
};

/*** ---------------------------------------------------------------------- ***/

static const nls_wchar_t *get_cset(int charset)
{
	switch (charset)
	{
	case CHARSET_ST:
		return atari_to_unicode;
	case CHARSET_L1:
		return latin1_to_unicode;
	case CHARSET_L9:
		return atari_latin9_to_unicode;
	case CHARSET_L2:
		return atari_latin2_to_unicode;
	case CHARSET_GR:
		return atari_greek_to_unicode;
	case CHARSET_RU:
		return atari_russian_to_unicode;
	default:
		/*
		 * this should not happen at all
		 */
		break;
	}
	return NULL;
}

/*** ---------------------------------------------------------------------- ***/

char *nls_conv_to_utf8(int charset, const void *src, size_t len, int quote_nl)
{
	char *dst;
	const nls_wchar_t *cset;
	size_t i;
	
	if (src == NULL)
		return NULL;
	if (len == STR0TERM)
		len = strlen((const char *) src);
	cset = get_cset(charset);
	if (cset != NULL)
	{
		const unsigned char *ptr;
		char *p;
		unsigned int wc;
		
		dst = p = g_new(char, len * UTF8_CHARMAX + 1);
		if (dst != NULL)
		{
			ptr = (const unsigned char *)src;
			for (i = 0; i < len; i++)
			{
				unsigned char c = *ptr++;
				if (quote_nl && c == '\n')
				{
					*p++ = '\\';
					*p++ = 'n';
				} else
				{
					wc = cset[c];
					nls_put_unichar(p, wc);
				}
			}
			*p++ = '\0';
			dst = g_renew(char, dst, p - dst);
		}
	} else
	{
		dst = g_strndup((const char *)src, len);
	}
	return dst;
}

/*** ---------------------------------------------------------------------- ***/

void latin1_to_atarist(char *s)
{
	nls_wchar_t c, newc;
	int warned = 0;
	int i;
	
	while ((c = (unsigned char) (*s) & 0xFF) != 0)
	{
		newc = latin1_to_unicode[c];
		newc = (*utf16_to_atari[newc >> 8])[newc & 0xff];
		if (newc == 0xff && atari_to_unicode[0xff] != newc)
		{
			newc = latin1_to_unicode[c];
			for (i = 0; replacements[i].uni != 0 && replacements[i].uni != newc; i++)
				;
			newc = replacements[i].ascii;
			if (!warned)
			{
				char *utf8 = nls_conv_to_utf8(CHARSET_L1, s, STR0TERM, FALSE);
				errout("warning: untranslatable character $%02x in %s, using '%c' instead\n", c, utf8, newc);
				g_free(utf8);
				warned = 1;
			}
		}
		*s = newc;
		s++;
	}
}
