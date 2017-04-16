#include "config.h"
#include <gem.h>
#include <time.h>
#include "debug.h"

/* 1024 entries, means at least 8 KB, plus 8 bytes per string,
 * plus the lengths of strings
 */
#define TH_BITS 10
#define TH_SIZE (1 << TH_BITS)
#define TH_MASK (TH_SIZE - 1)
#define TH_BMASK ((1 << (16 - TH_BITS)) - 1)


/*
 * set this to one to first hash addresses in RAM before
 * computing actual string hashes. (experimental)
 */
#define USE_RAM_HASH 0

#if USE_RAM_HASH
static char *ram_hash[(TH_SIZE + 10) * 2];
#endif

/* initialisation */

void nls_init(nls_domain *domain)
{
	domain->hash = NULL;
	gettext_init(domain);
}


void gettext_init(nls_domain *domain)
{
	(void) domain;
#if USE_RAM_HASH
	memset(ram_hash, 0, sizeof(ram_hash));
#endif
}


unsigned int nls_hash(const char *t)
{
	const unsigned char *u = (const unsigned char *) t;
	unsigned short a, b;

	a = 0;
	while (*u)
	{
		a = (a << 1) | ((a >> 15) & 1);
		a += *u++;
	}
	b = (a >> TH_BITS) & TH_BMASK;
	a &= TH_MASK;
	a ^= b;
	return a;
}


char *dgettext(const nls_domain *domain, const char *key)
{
	unsigned int hash;
	const char *const *chain;
	const char *cmp;
	
	/* check for empty string - often used in RSC - must return original address */
	if (domain == NULL || key == NULL || *key == '\0' || domain->hash == NULL)
		return (char *)NO_CONST(key);
	hash = nls_hash(key);
	if ((chain = domain->hash[hash]) != NULL)
	{
		while ((cmp = *chain++) != NULL)
		{
			if (strcmp(cmp, key) == 0)
			{
				/* strings are equal, return next string */
				key = *chain;
				break;
			}
			/* the strings differ, next */
			chain++;
		}
	}
	/* not in hash, return original string */
#if USE_RAM_HASH
	store_ram_hash();
#else
	return (char *)NO_CONST(key);
#endif
}
