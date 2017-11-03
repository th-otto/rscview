/*
 * nls.h - Definitions for Native Language Support
 *
 * Copyright (C) 2001 The Emutos Development Team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef NLS_H
#define NLS_H

#define CONF_WITH_NLS 1

/* the gettext-like macros */

#define N_(a) a

/* Indexes of font sets inside font_sets[] */
#define CHARSET_ST 1
#define CHARSET_L9 2
#define CHARSET_L2 3
#define CHARSET_GR 4
#define CHARSET_RU 5
#define CHARSET_L1 6

typedef struct _nls_domain {
	const char *domain;
	const char *lang;
	int fontset;
	const char *const *const *hash;
} nls_domain;

char *nls_dgettext(const nls_domain *domain, const char *key) __attribute__((__format_arg__(2)));

/* initialisation */

void nls_init(nls_domain *domain);
void nls_gettext_init(nls_domain *domain);
unsigned int nls_hash(const char *t);

#if CONF_WITH_NLS

extern nls_domain nls_default_domain;

# define _(a) nls_gettext(a)
# define nls_gettext(key) nls_dgettext(&nls_default_domain, key)

/* functions to query the lang database and to set the lang */

#else

/* Disable NLS / gettext completely */

# define _(a) (a)
# define nls_gettext(a) (a)

#endif

typedef unsigned short nls_wchar_t;
#define UTF8_CHARMAX 6

char *nls_conv_to_utf8(int charset, const void *src, size_t len, int quote_nl);
void latin1_to_atarist(char *s);

#endif /* NLS_H */
