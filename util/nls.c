#include "config.h"
#include <gem.h>
#include <time.h>
#include "debug.h"
#include "aesutils.h"

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

/*** ---------------------------------------------------------------------- ***/

/*
 *  trims trailing spaces from string
 */
static void trim_spaces(char *string)
{
	char *p;

	for (p = string + strlen(string) - 1; p >= string; p--)
		if (*p != ' ')
			break;
	p[1] = '\0';
}

/*** ---------------------------------------------------------------------- ***/

void xlate_obj_array(nls_domain *domain, OBJECT *obj_array, _LONG nobs, _BOOL trim_strings)
{
	OBJECT *obj;
	
	for (obj = obj_array; --nobs >= 0; obj++)
	{
		_WORD type = obj->ob_type & OBTYPEMASK;
		switch (type)
		{
		case G_TEXT:
		case G_FTEXT:
			obj->ob_spec.tedinfo->te_ptext = dgettext(domain, obj->ob_spec.tedinfo->te_ptext);
			break;
		case G_BOXTEXT:
		case G_FBOXTEXT:
			obj->ob_spec.tedinfo->te_ptmplt = dgettext(domain, obj->ob_spec.tedinfo->te_ptmplt);
			break;
		case G_STRING:
		case G_BUTTON:
		case G_TITLE:
			if (type == G_STRING && trim_strings)
				trim_spaces(obj->ob_spec.free_string);
			obj->ob_spec.free_string = dgettext(domain, obj->ob_spec.free_string);
			break;
		default:
			break;
		}
	}
}
