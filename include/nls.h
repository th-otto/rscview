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

typedef struct _nls_domain {
	const char *domain;
	const char *const *const *hash;
} nls_domain;

char *dgettext(const nls_domain *domain, const char *key);

/* initialisation */

void nls_init(nls_domain *domain);
void gettext_init(nls_domain *domain);
unsigned int nls_hash(const char *t);

#if CONF_WITH_NLS

extern nls_domain nls_default_domain;

# define _(a) gettext(a)
# define gettext(key) dgettext(&nls_default_domain, key)

/* functions to query the lang database and to set the lang */

#else

/* Disable NLS / gettext completely */

# define _(a) (a)
# define gettext(a) (a)

#endif

#endif /* NLS_H */
