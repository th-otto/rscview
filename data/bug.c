/*
 * bug.c - Basic Unencumbering Gettext, a minimal gettext-like tool
 *         (any better name is welcome)
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Bugs and limitations:
 * - double quotes within simple quotes are not handled
 * - free comments in po files are put to the end when updating
 * - only _() and N_() macros are handled
 * - no fuzzy or printf-format parameters
 * - some weird messages
 * - trigraphs are not handled (this is a feature actually !)
 */

/*
 * Structure of this file
 * (in this order, as each function calls functions above):
 * - library support (errors, memory, ...)
 * - basic data structures (string, dynamic array, hash, ...)
 * - po-file data structure
 * - po-file administrative data entry
 * - input file with line counting and ability to go backwards
 * - low level lexical parsing for sh-like and C syntax.
 * - high level parsers
 * - charset conversion
 * - po file internal operations
 * - the three tool commands (xgettext, update, make)
 * - main()
 */

/*
 * TODO list
 * - better algorithm for merging po files (keep the order of entries)
 * - parse_po_file is a mess
 * - use #~ for old entries
 * - split this into proper files
 * - finish translate command
 * - LINGUAS, POTFILES.in to be optionally specified on the command line
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

#define VERSION "0.2e"

#define ALERT_TEXT_WARNINGS 1			/* 1 => generate warning msgs */
#define MAX_LINE_COUNT      5			/* validation values */
#define MAX_LINE_LENGTH     32
#define MAX_BUTTON_LENGTH   10
#define AC_LINE_COUNT       -1			/* error codes from alert_check() */
#define AC_LINE_LENGTH      -2
#define AC_BUTTON_LENGTH    -3

#define TOOLNAME "bug"
#define DOCNAME  "doc/nls.txt"
#define LANGS_C  "util/langs.c"

#define PO_DIR "../po/"

#define UNUSED(x) (void)(x)				/* Unused variable */
#define ARRAY_SIZE(array) ((int)(sizeof(array)/sizeof(array[0])))

#ifndef __GNUC__
#  define __attribute__(x)
#endif

/*
 * typedefs
 */

typedef int gboolean;

#define FALSE 0
#define TRUE 1


/*
 * errors
 */

__attribute__((format(printf, 1, 2)))
static void warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "Warning: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

__attribute__((format(printf, 1, 2)))
static void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "Fatal: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(EXIT_FAILURE);
}

__attribute__((format(printf, 1, 2)))
static void error(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

/*
 * memory
 */

#define g_malloc(n) xmalloc(n)
#define g_calloc(n, s) xcalloc((size_t)(n), (size_t)(s))
#define g_malloc0(n) xmalloc((size_t)(n))
#define g_realloc(ptr, s) xrealloc(ptr, s)

#define g_new(t, n) ((t *)g_malloc((size_t)(n) * sizeof(t)))
#define g_new0(t, n) ((t *)g_malloc0((size_t)(n) * sizeof(t)))
#define g_renew(t, p, n) ((t *)g_realloc(p, (size_t)(n) * sizeof(t)))
#define g_strdup(s) xstrdup(s)


static void *xmalloc(size_t s)
{
	void *a = calloc(1, s);

	if (a == NULL)
		fatal("memory");
	return a;
}

static void *xrealloc(void *b, size_t s)
{
	void *a = realloc(b, s);

	if (a == NULL)
		fatal("memory");
	return a;
}


static void g_free(void *s)
{
	if (s)
	{
		*((char *)s) = 0xff;
		free(s);
	}
}


/*
 * xstrdup
 */

static char *xstrdup(const char *s)
{
	int len;
	char *a;

	if (s == NULL)
		return NULL;
	len = strlen(s);
	a = g_new(char, len + 1);
	strcpy(a, s);
	return a;
}

/*
 * now - current date in gettext format (stolen from gettext)
 */

#define TM_YEAR_ORIGIN 1900

/* Yield A - B, measured in seconds.  */
static long difftm(const struct tm *a, const struct tm *b)
{
	int ay = a->tm_year + (TM_YEAR_ORIGIN - 1);
	int by = b->tm_year + (TM_YEAR_ORIGIN - 1);

	/* Some compilers cannot handle this as a single return statement.  */
	long days = (
					/* difference in day of year  */
					a->tm_yday - b->tm_yday
					/* + intervening leap days  */
					+ ((ay >> 2) - (by >> 2)) - (ay / 100 - by / 100) + ((ay / 100 >> 2) - (by / 100 >> 2))
					/* + difference in years * 365  */
					+ (long) (ay - by) * 365l);

	return 60l * (60l * (24l * days + (a->tm_hour - b->tm_hour)) + (a->tm_min - b->tm_min)) + (a->tm_sec - b->tm_sec);
}


static char *now(void)
{
	time_t now;
	struct tm local_time;
	char tz_sign;
	long tz_min;
	char buf[80];

	time(&now);
	local_time = *localtime(&now);
	tz_sign = '+';
	tz_min = difftm(&local_time, gmtime(&now)) / 60;
	if (tz_min < 0)
	{
		tz_min = -tz_min;
		tz_sign = '-';
	}
	sprintf(buf, "%d-%02d-%02d %02d:%02d%c%02ld%02ld",
			local_time.tm_year + TM_YEAR_ORIGIN, local_time.tm_mon + 1,
			local_time.tm_mday, local_time.tm_hour, local_time.tm_min, tz_sign, tz_min / 60, tz_min % 60);
	return g_strdup(buf);
}

/*
 * da - dynamic array
 */

typedef struct da
{
	int size;
	void **buf;
	int len;
} da;

#define DA_SIZE 1000

static void da_grow(da *d)
{
	if (d->size == 0)
	{
		d->size = DA_SIZE;
		d->buf = g_new(void *, d->size);
	} else
	{
		d->size *= 4;
		d->buf = g_renew(void *, d->buf, d->size);
	}
}

static da *da_new(void)
{
	da *d = g_new(da, 1);

	d->size = 0;
	d->len = 0;
	return d;
}

static void da_free(da *d)
{
	if (d)
	{
		g_free(d->buf);
		g_free(d);
	}
}

static int da_len(da *d)
{
	return d->len;
}

static void *da_nth(da *d, int n)
{
	return d->buf[n];
}

static void da_add(da *d, void *elem)
{
	if (d->len >= d->size)
	{
		da_grow(d);
	}
	d->buf[d->len++] = elem;
}

/*
 * str - string
 */

typedef struct str
{
	int size;
	int len;
	char *buf;
} str;

#define STR_SIZE 100

static str *s_new(void)
{
	str *s = g_new(str, 1);

	s->size = 0;
	s->len = 0;
	s->buf = NULL;
	return s;
}

static void s_grow(str *s)
{
	if (s->size == 0)
	{
		s->size = STR_SIZE;
		s->buf = g_new(char, s->size);
	} else
	{
		s->size *= 4;
		s->buf = g_renew(char, s->buf, s->size);
	}
}

static void s_free(str *s)
{
	if (s)
	{
		g_free(s->buf);
		g_free(s);
	}
}

static void s_addch(str *s, char c)
{
	if (s->len >= s->size)
	{
		s_grow(s);
	}
	s->buf[s->len++] = c;
}

static void s_addstr(str *s, const char *t)
{
	while (*t)
	{
		s_addch(s, *t++);
	}
}

/* add a trailing 0 if needed and release excess mem */
static char *s_close(str *s)
{
	if (s->size == 0)
	{
		if (s->buf == NULL)
		{
			s->buf = g_new(char, 1);
			s->buf[0] = 0;
		}
		return s->buf;
	}
	s->buf = g_renew(char, s->buf, s->len + 1);
	s->buf[s->len] = 0;
	return s->buf;
}

static char *s_detach(str *s)
{
	char *t = s_close(s);

	g_free(s);
	return t;
}

/*
 * hi - hash item. This is intended to be aggregated by effective
 * hash item structures (a way to implement inheritance in C)
 */

typedef struct hash_item
{
	char *key;
} hi;

/*
 * hash - a hash will contain hash-items sorted by their hash
 * value.
 */

#define HASH_SIZ 10000

typedef struct hash
{
	da *d[HASH_SIZ];
} hash;

static hash *h_new(void)
{
	hash *h = g_new(hash, 1);

	return h;
}

/* a dumb one */
static unsigned int compute_hash(const char *t)
{
	unsigned int m = 0;

	while (*t)
	{
		m += *t++;
		m <<= 1;
	}
	return m;
}

static void *h_find(hash *h, const char *key)
{
	unsigned int m = compute_hash(key) % HASH_SIZ;
	da *d;
	int i, n;
	hi *k;

	d = h->d[m];
	if (d != NULL)
	{
		n = da_len(d);
		for (i = 0; i < n; i++)
		{
			k = (hi *)da_nth(d, i);
			if (strcmp(key, k->key) == 0)
			{
				return k;
			}
		}
	}
	return NULL;
}

static void h_insert(hash *h, void *k)
{
	unsigned int m = compute_hash(((hi *) k)->key) % HASH_SIZ;
	da *d;

	d = h->d[m];
	if (d == NULL)
	{
		d = da_new();
		h->d[m] = d;
	}
	da_add(d, k);
}


static void h_free(hash *h)
{
	int i;
	
	if (h)
	{
		for (i = 0; i < HASH_SIZ; i++)
			da_free(h->d[i]);
		g_free(h);
	}
}


/*
 * poe - po-entries
 * the po structure is an ordered-hash of po-entries,
 * the po-entry being a sub-type of hash-item.
 */

#define KIND_NORM 0
#define KIND_COMM 1
#define KIND_OLD 2

typedef struct poe
{
	hi msgid;							/* the key (super-type) */
	int kind;							/* kind of entry */
	char *comment;						/* free user comments */
	da *refs;							/* the references to locations in code */
	char *refstr;						/* a char * representation of the references */
	char *msgstr;						/* the translation */
} poe;

/*
 * oh - ordered hash
 */

typedef struct oh
{
	hash *h;
	da *d;
} oh;


static oh *o_new(void)
{
	oh *o = g_new(oh, 1);

	o->h = h_new();
	o->d = da_new();
	return o;
}

static void poe_free(poe *e)
{
	if (e)
	{
		g_free(e->msgid.key);
		g_free(e->comment);
		g_free(e->refstr);
		g_free(e->msgstr);
		da_free(e->refs);
		g_free(e);
	}
}

static void o_free(oh *o, gboolean freeentries)
{
	int i, len;
	
	if (o)
	{
		h_free(o->h);
		
		if (o->d)
		{
			if (freeentries)
			{
				len = da_len(o->d);
				for (i = 0; i < len; i++)
				{
					poe_free((poe *)da_nth(o->d, i));
				}
			}
			da_free(o->d);
		}
		g_free(o);
	}
}

static poe *o_find(oh *o, const char *t)
{
	return (poe *)h_find(o->h, t);
}

static void o_insert(oh *o, poe *k)
{
	da_add(o->d, k);
	h_insert(o->h, k);
}

static void o_add(oh *o, poe *k)
{
	da_add(o->d, k);
}

static int o_len(oh *o)
{
	return da_len(o->d);
}

static poe *o_nth(oh *o, int n)
{
	return (poe *)da_nth(o->d, n);
}

/*
 * ref - reference to locations in source files
 */

typedef struct ref
{
	const char *fname;
	int lineno;
} ref;

static ref *ref_new(const char *fname, int lineno)
{
	ref *r = g_new(ref, 1);

	r->fname = fname;
	r->lineno = lineno;
	return r;
}


static void ref_free(ref *r)
{
	if (r)
	{
		g_free(r);
	}
}

static poe *poe_new(char *t)
{
	poe *e = g_new(poe, 1);

	e->msgid.key = t;
	e->kind = KIND_NORM;
	e->comment = NULL;
	e->refs = NULL;
	e->msgstr = NULL;
	e->refstr = NULL;
	return e;
}


/*
 * gettext administrative entry, an entry with msgid empty, and
 * msgstr being specially formatted (example in doc/nls.txt)
 */

typedef struct
{
	char *lasttrans;
	char *langteam;
	char *charset;
	char *other;
} ae_t;


static void free_pot_ae(ae_t *a)
{
	g_free(a->lasttrans);
	a->lasttrans = NULL;
	g_free(a->langteam);
	a->langteam = NULL;
	g_free(a->charset);
	a->charset = NULL;
	g_free(a->other);
	a->other = NULL;
}

static void fill_pot_ae(ae_t *a)
{
	free_pot_ae(a);
	a->lasttrans = g_strdup("FULL NAME <EMAIL@ADDRESS>");
	a->langteam = g_strdup("LANGUAGE");
	a->charset = g_strdup("CHARSET");
	a->other = g_strdup("");
}

static char *ae_to_string(ae_t *a)
{
	str *s = s_new();

	s_addstr(s, "Last-Translator: ");
	s_addstr(s, a->lasttrans);
	s_addstr(s, "\nLanguage-Team: ");
	s_addstr(s, a->langteam);
	s_addstr(s, "\nMIME-Version: 1.0\nContent-Type: text/plain; charset=");
	s_addstr(s, a->charset);
	s_addstr(s, "\nContent-Transfer-Encoding: 8bit\n");
	s_addstr(s, a->other);
	return s_detach(s);
}

static gboolean ae_check_line(char **cc, const char *start, char **end)
{
	char *c, *t;
	int n = strlen(start);
	int m;

	c = *cc;
	if (c == NULL || strncmp(c, start, n) != 0)
	{
		warn("Expecting \"%s\" in administrative entry", start);
		return FALSE;
	}
	t = strchr(c + n, '\n');
	if (t == NULL)
	{
		warn("Fields in administrative entry must end with \\n");
		return FALSE;
	}
	*cc = t + 1;
	m = t - (c + n);
	t = g_new(char, m + 1);
	memcpy(t, c + n, m);
	t[m] = 0;
	g_free(*end);
	*end = t;
	return TRUE;
}

static gboolean parse_ae(char *msgstr, ae_t *a)
{
	char *c = msgstr;
	char *tmp;

	tmp = NULL;
	if (!ae_check_line(&c, "Last-Translator: ", &a->lasttrans))
		goto fail;
	if (!ae_check_line(&c, "Language-Team: ", &a->langteam))
		goto fail;
	if (!ae_check_line(&c, "MIME-Version: ", &tmp))
		goto fail;
	if (strcmp(tmp, "1.0") != 0)
	{
		warn("MIME version must be 1.0");
		goto fail;
	}
	if (!ae_check_line(&c, "Content-Type: text/plain; charset=", &a->charset))
		goto fail;
	if (!ae_check_line(&c, "Content-Transfer-Encoding: ", &tmp))
		goto fail;
	if (strcmp(tmp, "8bit") != 0)
	{
		warn("Content-Transfer-Encoding must be 8bit");
		goto fail;
	}
	g_free(a->other);
	a->other = g_strdup(c);
	g_free(tmp);
	return TRUE;
  fail:
	warn("Error in administrative entry");
	g_free(tmp);
	return FALSE;
}


/*
 * input files
 */

#define BACKSIZ 10
#define READSIZ 512

typedef struct ifile
{
	int lineno;
	char *fname;
	FILE *fh;
	unsigned char buf[BACKSIZ + READSIZ];
	int size;
	int index;
	int ateof;
} IFILE;

static void irefill(IFILE *f)
{
	if (f->size > BACKSIZ)
	{
		memmove(f->buf, f->buf + f->size - BACKSIZ, BACKSIZ);
		f->size = BACKSIZ;
		f->index = f->size;
	}
	f->size += fread(f->buf + f->size, 1, READSIZ, f->fh);
}


static void ifclose(IFILE *f)
{
	if (f)
	{
		if (f->fh)
			fclose(f->fh);
		g_free(f->fname);
		g_free(f);
	}
}

static IFILE *ifopen(const char *fname)
{
	IFILE *f = g_new(IFILE, 1);

	f->fname = g_strdup(fname);
	f->fh = fopen(fname, "rb");
	if (f->fh == NULL)
	{
		ifclose(f);
		return NULL;
	}
	f->size = 0;
	f->index = 0;
	f->ateof = 0;
	f->lineno = 1;
	return f;
}

static void iback(IFILE *f)
{
	if (f->index == 0)
	{
		fatal("too far backward");
	} else
	{
		if (f->buf[f->index] == 012)
		{
			f->lineno--;
		}
		f->index--;
	}
}

static void ibackn(IFILE *f, int n)
{
	f->index -= n;
	if (f->index < 0)
	{
		fatal("too far backward");
	}
}

static int igetc(IFILE *f)
{
	if (f->index >= f->size)
	{
		irefill(f);
		if (f->index >= f->size)
		{
			f->ateof = 1;
			return EOF;
		}
	}
	return f->buf[f->index++];
}

/* returns the next logical char, in sh syntax */
static int inextsh(IFILE *f)
{
	int ret;

	ret = igetc(f);
	if (ret == 015)
	{
		ret = igetc(f);
		if (ret == 012)
		{
			f->lineno++;
			return '\n';
		} else
		{
			iback(f);
			return 015;
		}
	} else if (ret == 012)
	{
		f->lineno++;
		return '\n';
	} else
	{
		return ret;
	}
}


/* returns the next logical char, in C syntax */
static int inextc(IFILE *f)
{
	int ret;

  again:
	ret = igetc(f);
	/* look ahead if backslash new-line */
	if (ret == '\\')
	{
		ret = igetc(f);
		if (ret == 015)
		{
			ret = igetc(f);
			if (ret == 012)
			{
				f->lineno++;
				goto again;
			} else
			{
				ibackn(f, 2);
				return '\\';
			}
		} else if (ret == 012)
		{
			f->lineno++;
			goto again;
		} else
		{
			iback(f);
			return '\\';
		}
	} else if (ret == 015)
	{
		ret = igetc(f);
		if (ret == 012)
		{
			f->lineno++;
			return '\n';
		} else
		{
			iback(f);
			return 015;
		}
	} else if (ret == 012)
	{
		f->lineno++;
		return '\n';
	} else
	{
		return ret;
	}
}

#define is_white(c)  (((c)==' ')||((c)=='\t')||((c)=='\f'))
#define is_letter(c) ((((c)>='a')&&((c)<='z'))||(((c)>='A')&&((c)<='Z')))
#define is_digit(c)  (((c)>='0')&&((c)<='9'))
#define is_octal(c)  (((c)>='0')&&((c)<='7'))
#define is_hexdig(c) ((((c)>='a')&&((c)<='f'))||(((c)>='A')&&((c)<='F')))
#define is_hex(c)    (is_digit(c)||is_hexdig(c))

/*
 * functions swallowing lexical tokens. return 1 if
 * the token was the one tested for, return 0 otherwise.
 */

static int try_eof(IFILE *f)
{
	int c = igetc(f);

	if (c == EOF)
	{
		return 1;
	} else
	{
		iback(f);
		return 0;
	}
}

static int try_c_comment(IFILE *f)
{
	int c;

	c = inextc(f);
	if (c == '/')
	{
		c = inextc(f);
		if (c == '/')
		{
			do
			{
				c = inextc(f);
			} while (c != EOF && c != '\n');
			return 1;
		} else if (c == '*')
		{
			int state = 0;

			do
			{
				c = inextc(f);
				if (c == '*')
				{
					state = 1;
				} else if (c == '/')
				{
					if (state == 1)
						return 1;
					else
						state = 0;
				} else
				{
					state = 0;
				}
			} while (c != EOF);
			if (c == EOF)
			{
				warn("EOF reached inside comment");
				return 1;
			}
		}
	}
	iback(f);
	return 0;
}

static int try_white(IFILE *f)
{
	int c;

	c = inextc(f);
	if (is_white(c) || c == '\n')
	{
		do
		{
			c = inextc(f);
		} while (is_white(c) || (c == '\n'));
		if (c == EOF)
			return 1;
		iback(f);
		return 1;
	} else
	{
		iback(f);
		return 0;
	}
}

static int try_c_white(IFILE *f)
{
	if (try_eof(f))
	{
		return 0;
	}
	if (try_c_comment(f) || try_white(f))
	{
		while (!try_eof(f) && (try_c_comment(f) || try_white(f)))
			;
		return 1;
	} else
	{
		return 0;
	}
}


/* only one "..." string will be appended to string s */
static int get_c_string(IFILE *f, str *s)
{
	int c;

	c = inextc(f);
	if (c != '"')
	{
		iback(f);
		return 0;
	}
	for (;;)
	{
		c = inextc(f);
		if (c == EOF)
		{
			warn("EOF reached inside string");
			return 0;
		} else if (c == '\\')
		{
			c = inextc(f);
			if (c == EOF)
			{
				warn("EOF reached inside string");
				return 0;
			} else if (is_octal(c))
			{
				int i;
				int a = c - '0';

				c = inextc(f);
				for (i = 0; i < 3 && is_octal(c); i++)
				{
					a <<= 3;
					a += (c - '0');
					c = inextc(f);
				};
				s_addch(s, a);
				iback(f);
			} else if (c == 'x')
			{
				int a = 0;

				c = inextc(f);
				while (is_hex(c))
				{
					a <<= 4;
					if (c <= '9')
					{
						a += (c - '0');
					} else if (c <= 'F')
					{
						a += (c - 'A' + 10);
					} else
					{
						a += (c - 'a' + 10);
					}
					c = inextc(f);
				}
				s_addch(s, a);
				iback(f);
			} else
			{
				switch (c)
				{
				case 'a':
					c = '\a';
					break;
				case 'b':
					c = '\b';
					break;
				case 'v':
					c = '\v';
					break;
				case 'e':
					c = 033;
					break;				/* GNU C extension: \e for escape */
				case 'f':
					c = '\f';
					break;
				case 'r':
					c = '\r';
					break;
				case 't':
					c = '\t';
					break;
				case 'n':
					c = '\n';
					break;
				default:
					break;
				}
				s_addch(s, c);
			}
		} else if (c == '\"')
		{
			return 1;
		} else
		{
			s_addch(s, c);
		}
	}
}


/*
 * parse c files
 * put strings surrounded by _("...") or N_("...") into the ordered-hash
 *
 * state means :
 * 0 outside names, 1 after 'N', 2 after '_', 3 after '(', 4 in a name
 * when anything meaningful has been parsed, the corresponding structure of
 * the action structure is called.
 */

typedef struct parse_c_action
{
	void (*gstring) (void *self, str *s, const char *fname, int lineno);
	void (*string) (void *self, str *s);
	void (*other) (void *self, int c);
} parse_c_action;

static void pca_xgettext_gstring(void *self, str *s, const char *fname, int lineno)
{
	oh *o = (oh *) self;
	poe *e;
	ref *r;
	char *t;

	t = s_detach(s);

	/* add the string into the hash */
	e = o_find(o, t);
	if (e)
	{
		/* the string already exists */
		g_free(t);
	} else
	{
		e = poe_new(t);
		o_insert(o, e);
	}
	r = ref_new(fname, lineno);
	if (e->refs == NULL)
	{
		e->refs = da_new();
	}
	da_add(e->refs, r);
}

static void pca_xgettext_string(void *self, str *s)
{
	UNUSED(self);
	s_free(s);
}

static void pca_xgettext_other(void *self, int c)
{
	UNUSED(self);
	UNUSED(c);
}

static parse_c_action const pca_xgettext = {
   pca_xgettext_gstring,
   pca_xgettext_string,
   pca_xgettext_other
};

static int print_canon(FILE *, const char *, const char *, gboolean);

/* pcati - Parse C Action Translate Info */
typedef struct pcati
{
	FILE *f;
	void (*conv) (char *);
	oh *o;
} pcati;


static void pca_translate_gstring(void *self, str *s, const char *fname, int lineno)
{
	pcati *p = (pcati *) self;
	char *t;
	poe *e;

	UNUSED(fname);
	UNUSED(lineno);

	t = s_detach(s);
	e = o_find(p->o, t);
	if (e)
	{									/* if there is a translation, get it instead */
		if (e->msgstr && e->msgstr[0])
		{								/* if the translation isn't empty, use it instead */
			g_free(t);
			t = g_strdup(e->msgstr);
		}
	}
	p->conv(t);							/* convert the string, be it a translation or the original */
	print_canon(p->f, t, NULL, TRUE);
	g_free(t);
}

static void pca_translate_string(void *self, str *s)
{
	pcati *p = (pcati *) self;
	char *t;

	t = s_detach(s);
	print_canon(p->f, t, NULL, TRUE);
	g_free(t);
}

static void pca_translate_other(void *self, int c)
{
	pcati *p = (pcati *) self;

	fputc(c, p->f);
}

static parse_c_action const pca_translate = {
	pca_translate_gstring,
	pca_translate_string,
	pca_translate_other
};

/*
 * parse C code
 *
 * the state machine appears to have the following states
 * (info supplied by Eero Tamminen):
 *  state   meaning
 *    0     valid place to start token
 *    1     within token starting with 'N'
 *    2     within 'N_' or '_' token
 *    3     valid token ended by '(', can now parse string
 *    4     within invalid token
 */
static gboolean parse_c_file(char *fname, const parse_c_action *pca, void *self)
{
	int c;
	int state;
	str *s;
	int lineno;
	IFILE *f = ifopen(fname);
	
	if (f == NULL)
	{
		error("could not open file '%s': %s", fname, strerror(errno));
		return FALSE;
	}

	/* TODO - merge parse_c_comment into this, rewrite the parser */

	state = 0;
	for (;;)
	{
		c = inextc(f);
		if (c == EOF)
		{
			break;
		} else if (c == '/')
		{
			c = inextc(f);
			if (c == '/')
			{
				pca->other(self, '\n');
			}
			ibackn(f, 2);
			c = '/';
			state = 0;
			if (!try_c_comment(f))
			{
				pca->other(self, c);
			} else
			{
				pca->other(self, ' ');
			}
		} else if (c == '\"')
		{
			if (state == 3)
			{
				/* this is a new gettext string */
				s = s_new();
				lineno = f->lineno;
				/* accumulate all consecutive strings (separated by spaces) */
				do
				{
					iback(f);
					get_c_string(f, s);
					try_c_white(f);
					c = inextc(f);
				} while (c == '\"');
				if (c != ')')
				{
					char *t = s_detach(s);

					warn("_(\"...\" with no closing )");
					warn("the string is %s", t);
					g_free(t);
					state = 0;
					continue;
				}
				/* handle the string */
				pca->gstring(self, s, fname, lineno);
				pca->other(self, ')');
			} else
			{
				iback(f);
				s = s_new();
				get_c_string(f, s);
				pca->string(self, s);
			}
		} else
		{
			if (c == '(')
			{
				if (state == 2)
				{
					state = 3;
				} else
				{
					state = 0;
				}
			} else if (c == '_')
			{
				if (state < 2)
				{
					state = 2;
				} else
				{
					state = 4;
				}
			} else if (c == 'N')
			{
				if (state == 0)
				{
					state = 1;
				} else
				{
					state = 4;
				}
			} else if (is_white(c))
			{
				if (state == 1 || state == 4)
				{
					state = 0;
				}
			} else if (is_letter(c) || is_digit(c))
			{
				state = 4;
			} else
			{
				state = 0;
			}
			pca->other(self, c);
		}
	}
	ifclose(f);
	
	return TRUE;
}


/*
 * determine "underscore length"
 *
 * the "underscore length" for a string is the length of the first substring
 * that starts with an underscore and contains only underscores and periods.
 * this is designed to catch simple typos that translators might make when
 * translating tedinfo strings.
 */
static int underscore_length(const char *s)
{
	int len = 0;

	for (; *s; s++)
		if (*s == '_')
			len++;

	return len;
}


/*
 * parse po files
 */

__attribute__((__warn_unused_result__))
static gboolean parse_po_file(const char *fname, oh *o, gboolean ignore_ae)
{
	int c;
	IFILE *f;
	poe *e;
	str *s,	*userstr, *refstr, *otherstr, *msgid, *msgstr;
	gboolean retval = FALSE;
	
	f = ifopen(fname);
	if (f == NULL)
	{
		/* TODO: UGLY HACK !!! */
		if (strcmp(PO_DIR "messages.pot", fname) == 0)
		{
			error("could not open %s (run 'bug xgettext' to generate it)", fname);
		} else
		{
			error("could not open %s", fname);
		}
		goto errout;
	}
	for (;;)
	{
		c = inextsh(f);
		/* skip any blank line before next entry */
		while (c == ' ' || c == '\t')
		{
			while (c == ' ' || c == '\t')
			{
				c = inextsh(f);
			}
			if (c != EOF && c != '\n')
			{
				warn("syntax error in %s line %d", fname, f->lineno);
				while (c != EOF && c != '\n')
				{
					c = inextsh(f);
				}
			}
			c = inextsh(f);
		}
		if (c == EOF)
		{
			break;
		}

		/* start an entry */
		userstr = NULL;
		refstr = NULL;
		otherstr = NULL;
		msgid = 0;
		msgstr = NULL;
		while (c == '#')
		{
			c = inextsh(f);
			switch (c)
			{
			case '\n':
			case ' ':					/* user comment */
				if (!userstr)
					userstr = s_new();
				s = userstr;
				break;
			case ':':					/* ref comment */
				if (!refstr)
					refstr = s_new();
				s = refstr;
				break;
			default:					/* other comment */
				if (!otherstr)
					otherstr = s_new();
				s = otherstr;
				break;
			}
			/* accumulate this comment line to the string */
			s_addch(s, '#');
			if (c == EOF)
			{
				s_addch(s, '\n');
				break;
			}
			s_addch(s, c);
			if (c != '\n')
			{
				while (c != EOF && c != '\n')
				{
					c = inextsh(f);
					s_addch(s, c);
				}
				if (c == EOF)
				{
					s_addch(s, '\n');
				}
			}
			c = inextsh(f);
		}
		if (c == ' ' || c == '\t' || c == '\n' || c == EOF)
		{
			/* the previous entry is a pure comment */
			if (userstr)
			{
				if (otherstr)
				{
					s_addstr(userstr, s_close(otherstr));
					s_free(otherstr);
				}
			} else if (otherstr)
			{
				userstr = otherstr;
			} else
			{
				if (refstr)
				{
					s_free(refstr);
					warn("stray ref ignored in %s:%d", fname, f->lineno);
				}
				/* we will reach here when an entry is followed by more than one
				 * empty line, at each additional empty line.
				 */
				continue;
			}
			e = poe_new(g_strdup(""));
			e->comment = s_detach(userstr);
			e->kind = KIND_COMM;
			o_add(o, e);
			continue;
		}
		if (c != 'm')
			goto err;
		c = inextsh(f);
		if (c != 's')
			goto err;
		c = inextsh(f);
		if (c != 'g')
			goto err;
		c = inextsh(f);
		if (c != 'i')
			goto err;
		c = inextsh(f);
		if (c != 'd')
			goto err;
		c = inextsh(f);
		if (c != ' ' && c != '\t')
			goto err;
		while (c == ' ' || c == '\t')
		{
			c = inextsh(f);
		}
		if (c != '\"')
			goto err;
		s = msgid = s_new();
		/* accumulate all consecutive strings (separated by spaces) */
		do
		{
			iback(f);
			get_c_string(f, s);
			c = inextsh(f);
			while (c == ' ' || c == '\t')
			{
				c = inextsh(f);
			}
			if (c == EOF)
				goto err;
			if (c != '\n')
				goto err;
			c = inextsh(f);
		} while (c == '\"');
		if (c != 'm')
			goto err;
		c = inextsh(f);
		if (c != 's')
			goto err;
		c = inextsh(f);
		if (c != 'g')
			goto err;
		c = inextsh(f);
		if (c != 's')
			goto err;
		c = inextsh(f);
		if (c != 't')
			goto err;
		c = inextsh(f);
		if (c != 'r')
			goto err;
		c = inextsh(f);
		if (c != ' ' && c != '\t')
			goto err;
		while (c == ' ' || c == '\t')
		{
			c = inextsh(f);
		}
		if (c != '\"')
			goto err;
		s = msgstr = s_new();
		/* accumulate all consecutive strings (separated by spaces) */
		do
		{
			iback(f);
			get_c_string(f, s);
			c = inextsh(f);
			while (c == ' ' || c == '\t')
			{
				c = inextsh(f);
			}
			if (c == EOF)
				break;
			if (c != '\n')
				goto err;
			c = inextsh(f);
		} while (c == '\"');
		if (c != '\n' && c != EOF)
			goto err;
		/* put the comment in userstr */
		if (userstr)
		{
			if (otherstr)
			{
				s_addstr(userstr, s_close(otherstr));
				s_free(otherstr);
				otherstr = 0;
			}
		} else if (otherstr)
		{
			userstr = otherstr;
			otherstr = 0;
		}
		/* now we have the complete entry */
		e = o_find(o, s_close(msgid));
		if (e)
		{
			warn("double entry %s", s_close(msgid));
			s_free(msgid);
			s_free(msgstr);
		} else if (ignore_ae && msgid->buf[0] == '\0')
		{
			/* ignore administrative entry */
			s_free(msgid);
			s_free(msgstr);
		} else
		{
			e = poe_new(s_detach(msgid));
			e->msgstr = s_detach(msgstr);
			if (e->msgstr && strlen(e->msgstr))		/* really translating */
				if (underscore_length(e->msgid.key) != underscore_length(e->msgstr))
					warn("%s: underscores appear invalid for translation of '%s'", fname, e->msgid.key);
			if (refstr)
			{
				e->refstr = s_detach(refstr);
				refstr = NULL;
			}
			if (userstr)
			{
				e->comment = s_detach(userstr);
				userstr = NULL;
			}
			o_insert(o, e);
		}
		/* free temp strings */
		s_free(refstr);
		s_free(otherstr);
		s_free(userstr);
		continue;
	  err:
		warn("syntax error at %s:%d (c = '%c')", fname, f->lineno, c);
		while (c != '\n' && c != EOF)
		{
			c = inextsh(f);
		}
	}
	retval = TRUE;

errout:
	if (f)
		ifclose(f);
	return retval;
}



/*
 * read a simple textfile (one item per line),
 * ignoring lines that start with a #,
 * and empty lines
 */
static void free_oipl_file(da *d)
{
	int i, len;
	
	if (d)
	{
		len = da_len(d);
		for (i = 0; i < len; i++)
			g_free(da_nth(d, i));
		da_free(d);
	}
}


__attribute__((__warn_unused_result__))
static gboolean parse_oipl_file(const char *fname, da *d)
{
	int c;
	IFILE *f;

	f = ifopen(fname);
	if (f == NULL)
	{
		error("could not open %s: %s", fname, strerror(errno));
		return FALSE;
	}
	for (;;)
	{
		c = inextsh(f);
		if (c == EOF)
		{
			break;
		} else if (c == '#')
		{
			while (c != EOF && c != '\n')
			{
				c = inextsh(f);
			}
		} else if (c == ' ' || c == '\t')
		{
			while (c == ' ' || c == '\t')
			{
				c = inextsh(f);
			}
			if (c != EOF && c != '\n')
			{
				warn("syntax error in %s line %d", fname, f->lineno);
				while (c != EOF && c != '\n')
				{
					c = inextsh(f);
				}
			}
		} else if (c == '\n')
		{
			continue;
		} else
		{
			str *s = s_new();

			while (c != EOF && c != '\n')
			{
				if (c != '\r')
					s_addch(s, c);
				c = inextsh(f);
			}
			da_add(d, s_detach(s));
		}
	}
	ifclose(f);
	return TRUE;
}


#if ALERT_TEXT_WARNINGS
/*
 * Check given alert line and button text and set return code
 * if they're too long or there are too many lines of
 * text.  For buttons the lines parameter is zero.
 */
static int alert_check(const char *start, const char *end, int lines)
{
	int len = end - start - 1;

	if (lines > MAX_LINE_COUNT)
		return AC_LINE_COUNT;

	if (lines)
	{
		/* dialog text */
		if (len > MAX_LINE_LENGTH)
			return AC_LINE_LENGTH;
	} else
	{
		/* dialog button */
		if (len > MAX_BUTTON_LENGTH)
			return AC_BUTTON_LENGTH;
	}

	return 0;
}

/*
 * Print warning message corresponding to code
 */
static void print_alert_warning(int code, char *lang, char *key)
{
	char msg[100];

	switch (code)
	{
	case AC_LINE_COUNT:
		sprintf(msg, "has more than %d lines", MAX_LINE_COUNT);
		break;
	case AC_LINE_LENGTH:
		sprintf(msg, "line has more than %d characters", MAX_LINE_LENGTH);
		break;
	case AC_BUTTON_LENGTH:
		sprintf(msg, "button has more than %d characters", MAX_BUTTON_LENGTH);
		break;
	default:
		sprintf(msg, "has error code %d", code);
		break;
	}

	printf("lang %s: translated dialog %s (see %s in %s)\n", lang, msg, key, LANGS_C);
}
#else
static int alert_check(const char *start, const char *end, int lines)
{
	UNUSED(start);
	UNUSED(end);
	UNUSED(lines);

	return 0;
}

static void print_alert_warning(int code, char *lang, char *key)
{
	UNUSED(code);
	UNUSED(lang);
	UNUSED(key);
}
#endif

/*
 * print string in canonical format
 *
 * NOTE: the 'canonical' format is modified for handling of
 * the GEM Alert string specifications: if the string begins with
 * [n][, where n is a digit, then the string will be cut after
 * this initial [n][ and after every |.
 *
 * returns error code from alert_check() (if any)
 */

#define CANON_GEM_ALERT 1

static int print_canon(FILE *f, const char *t, const char *prefix, gboolean encode_extended_ascii)
{
	unsigned int a;
	int translate = 0;
	int rc = 0;

#if CANON_GEM_ALERT
	int gem_alert = 0;
	int gem_button = 0;
	int alert_lines = 0;
	int err;

	const char *line_start = NULL;
#endif /* CANON_GEM_ALERT */

	if (t == NULL)
		t = "";
	
	/*
	 * we need a special translate mode indicator
	 * so we can generate backslashes in the output
	 */
	if (!prefix)
	{									/* being called for translation */
		translate = 1;
		prefix = "";
	}

	if (strchr(t, '\n'))
	{
		if (translate)
			fprintf(f, "\"\"\\\n");		/* insert backslash before newline */
		else
			fprintf(f, "\"\"\n%s", prefix);
	}
#if CANON_GEM_ALERT
	if (t[0] == '[' && t[1] >= '0' && t[1] <= '9' && t[2] == ']' && t[3] == '[')
	{
		fprintf(f, "\"[%c][\"\n%s", t[1], prefix);
		t += 4;
		line_start = t;
		gem_alert = 1;
	}
#endif /* CANON_GEM_ALERT */

	fprintf(f, "\"");
	while (*t)
	{
		switch (*t)
		{
		case '\n':
			if (t[1])
			{
				if (translate)
					fprintf(f, "\\n\"\\\n\"");	/* insert backslash before newline */
				else
					fprintf(f, "\\n\"\n%s\"", prefix);
			} else
			{
				fprintf(f, "\\n");
			}
			break;
		case '\r':
			fprintf(f, "\\r");
			break;
		case '\t':
			fprintf(f, "\\t");
			break;
		case '\"':
		case '\\':
			fprintf(f, "\\%c", *t);
			break;
#if CANON_GEM_ALERT
		case '|':
			if (gem_alert)
			{
				alert_lines += 1;
				if ((err = alert_check(line_start, t, alert_lines)) < 0)
					rc = err;
				line_start = t + 1;
				fprintf(f, "%c\"\n%s\"", *t, prefix);
				break;
			} else if (gem_button)
			{
				if ((err = alert_check(line_start, t, 0)) < 0)
					rc = err;
				line_start = t + 1;
			}
			/* fallthrough */
		case ']':
			if (gem_alert)
			{
				gem_alert = 0;
				if ((err = alert_check(line_start, t, alert_lines + 1)) < 0)
					rc = err;
				if (t[1] == '[')
				{
					line_start = t + 2;
					gem_button = 1;
				}
			} else if (gem_button && *t != '|')
			{
				gem_button = 0;
				if ((err = alert_check(line_start, t, 0)) < 0)
					rc = err;
			}
#endif /* CANON_GEM_ALERT */
			/* fallthrough */
		default:
			a = ((unsigned char) (*t)) & 0xFF;
			if (a < ' ' || a == 0x7f || (encode_extended_ascii && a >= 0x80))
			{
				/* control character */
				fprintf(f, "\\%03o", a);
			} else
			{
				fprintf(f, "%c", *t);
			}
		}
		t++;
	}
	fprintf(f, "\"");

	return rc;
}

/*
 * pretty print refs.
 */

static char *refs_to_str(da *refs)
{
	int pos, len;
	int i, n;
	str *s;
	ref *r;
	char line[12];

	s = s_new();
	s_addstr(s, "#:");
	pos = 2;
	n = da_len(refs);
	for (i = 0; i < n; i++)
	{
		r = (ref *)da_nth(refs, i);
		sprintf(line, ":%d", r->lineno);
		len = strlen(line) + strlen(r->fname);
		if (pos + len > 78)
		{
			s_addstr(s, "\n#:");
			pos = 2;
		}
		pos += len + 1;
		s_addch(s, ' ');
		s_addstr(s, r->fname);
		s_addstr(s, line);
		ref_free(r);
	}
	s_addch(s, '\n');
	return s_detach(s);
}

/*
 * convert_refs : transform da * refs into char * refstr in all entries
 * of the po file contained in an oh.
 */

static void po_convert_refs(oh *o)
{
	int i, n;
	poe *e;

	n = o_len(o);
	for (i = 0; i < n; i++)
	{
		e = o_nth(o, i);
		if (e->refs)
		{
			e->refstr = refs_to_str(e->refs);
			da_free(e->refs);
			e->refs = NULL;
		}
	}
}


/*
 * print po file
 */
static void print_po_file(FILE *f, oh *o)
{
	int i, n;
	const char *prefix;

	n = o_len(o);
	for (i = 0; i < n; i++)
	{
		poe *e = o_nth(o, i);

		if (e->comment)
			fputs(e->comment, f);
		if (e->kind == KIND_COMM)
		{
			fputs("\n", f);
			continue;
		} else if (e->kind == KIND_OLD)
		{
			prefix = "#~";
		} else
		{
			if (e->refstr)
				fputs(e->refstr, f);
			prefix = "";
		}
		fprintf(f, "%smsgid ", prefix);
		print_canon(f, e->msgid.key, prefix, TRUE);
		fprintf(f, "\n%smsgstr ", prefix);
		print_canon(f, e->msgstr, prefix, FALSE);
		fputs("\n\n", f);
	}
}

/*
 * update po file against messages.pot
 */

__attribute__((__warn_unused_result__))
static gboolean update(const char *fname)
{
	oh *o1, *o2 = NULL, *o = NULL;
	poe *e1, *e2, *e;
	str *s;
	char *bfname;
	FILE *f = NULL;
	int i1, i2, n1, n2;
	int numtransl = 0;					/* number of translated entries */
	int numold = 0;						/* number of old entries */
	int numuntransl = 0;				/* number of untranslated entries */
	gboolean retval = FALSE;
	
	/* get the reference first, before renaming the file */
	o1 = o_new();
	if (parse_po_file(PO_DIR "messages.pot", o1, FALSE) == FALSE)
		goto errout;

	/* rename the po file (backup) */
	s = s_new();
	s_addstr(s, fname);
	s_addstr(s, ".bak");
	bfname = s_detach(s);

	if (rename(fname, bfname))
	{
		warn("cannot rename file '%s' to '%s', cancelled", fname, bfname);
		goto errout;
	}

	/* parse the po file */
	o2 = o_new();
	if (parse_po_file(bfname, o2, FALSE) == FALSE)
		goto errout;

	/* scan o1 and o2, merging the two */
	n1 = o_len(o1);
	n2 = o_len(o2);
	o = o_new();
	/* first, put an updated admin entry */
	{
		ae_t a1 = { NULL, NULL, NULL, NULL };
		ae_t a2 = { NULL, NULL, NULL, NULL };

		e1 = o_find(o1, "");
		e2 = o_find(o2, "");
		if (e1 == NULL || !parse_ae(e1->msgstr, &a1))
		{
			fill_pot_ae(&a1);
		}
		if (e2 == NULL || !parse_ae(e2->msgstr, &a2))
		{
			warn("bad administrative entry, getting back that of the template");
			free_pot_ae(&a2);
			a2 = a1;
			a1.lasttrans = a1.langteam = a1.charset = a1.other = NULL;
		}
		e = poe_new(g_strdup(""));
		if (e2 != NULL)
		{
			e->comment = g_strdup(e2->comment);	/* keep the initial comment */
		}
		e->kind = KIND_NORM;
		e->msgstr = ae_to_string(&a2);
		o_insert(o, e);
		free_pot_ae(&a2);
		free_pot_ae(&a1);
	}

	/* TODO, better algorithm to keep the order of entries... */

	/* first, update entries in the po file */
	for (i2 = 0; i2 < n2; i2++)
	{
		e2 = o_nth(o2, i2);
		if (e2->kind == KIND_COMM)
		{
			o_add(o, e2);
		} else if (e2->msgid.key[0] == '\0')
		{
			/* the old admin entry - do nothing */
		} else
		{
			e = o_find(o1, e2->msgid.key);
			if (e)
			{
				e2->kind = KIND_NORM;
				e2->refstr = g_strdup(e->refstr);
				e2->refs = 0;
				o_add(o, e2);
				if (e2->msgstr && strcmp("", e2->msgstr) != 0)
				{
					numtransl++;
				} else
				{
					numuntransl++;
				}
			} else
			{
				e2->kind = KIND_OLD;
				o_add(o, e2);
				numold++;
			}
		}
	}

	/* then, add new entries from the template */
	for (i1 = 0; i1 < n1; i1++)
	{
		e1 = o_nth(o1, i1);
		if (e1->kind == KIND_NORM)
		{
			if (e1->msgid.key[0] != '\0')
			{
				e = o_find(o2, e1->msgid.key);
				if (!e)
				{
					o_add(o, e1);
					numuntransl++;
				}
			}
		}
	}

	/* print stats */
	printf("translated %d, untranslated %d, obsolete %d\n", numtransl, numuntransl, numold);

	/* dump o into the new file */
	f = fopen(fname, "w");
	if (f == NULL)
	{
		error("could not open %s: %s", fname, strerror(errno));
	} else
	{
		print_po_file(f, o);
		retval = TRUE;
	}

errout:
	if (f)
		fclose(f);
	
	o_free(o, FALSE);
	o_free(o1, TRUE);
	o_free(o2, TRUE);
	
	return retval;
}


/*
 * xgettext : parse POTFILES.in, and generate messages.pot
 */

static gboolean xgettext(void)
{
	da *d;
	oh *o = NULL;
	int i, n;
	FILE *f = NULL;
	const char *fname;
	ae_t a = { NULL, NULL, NULL, NULL };
	poe *e = NULL;
	gboolean retval = FALSE;
	
	d = da_new();
	if (parse_oipl_file(PO_DIR "POTFILES.in", d) == FALSE)
		goto errout;

	o = o_new();

	/* create the administrative entry */
	fill_pot_ae(&a);
	e = poe_new(g_strdup(""));
	e->kind = KIND_NORM;
	e->msgstr = ae_to_string(&a);
	free_pot_ae(&a);
	e->comment = g_strdup("# SOME DESCRIPTIVE TITLE.\n\
# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n");
	o_add(o, e);

	n = da_len(d);
	retval = TRUE;
	for (i = 0; i < n; i++)
	{
		retval &= parse_c_file((char *)da_nth(d, i), &pca_xgettext, o);
	}

	po_convert_refs(o);

	fname = PO_DIR "messages.pot";
	f = fopen(fname, "w");
	if (f == NULL)
	{
		error("couldn't create %s: %s", fname, strerror(errno));
		retval = FALSE;
		goto errout;
	}
	print_po_file(f, o);

errout:
	if (f)
		fclose(f);
	free_pot_ae(&a);
	free_oipl_file(d);
	return retval;
}

/*
 * charset conversions
 */


/*
 * charsets
 */

struct charset_alias
{
	const char *name;
	const char *alias;
};

static struct charset_alias const charsets[] = {
 	{ "latin1", "ISO-8859-1" },
	{ "latin2", "ISO-8859-2" },
	{ "latin9", "ISO-8859-15" },
	{ "atarist", "atarist" },
};

/* resolve any known alias */
static const char *get_canon_cset_name(const char *name)
{
	int i;
	int n = ARRAY_SIZE(charsets);

	for (i = 0; i < n; i++)
	{
		if (strcmp(charsets[i].alias, name) == 0 ||
			strcmp(charsets[i].name, name) == 0)
		{
			return charsets[i].name;
		}
	}
	return "unknown";
}

/*
 * charset conversion
 */

/*
 * iso_to_atari : convert in situ iso latin 1 to atari ST encoding
 */

static const unsigned char i2a[] = {
	0x00, '?',	/* EURO SIGN */
	0x00, '?',	/* UNDEFINED */
	0x00, '?',	/* SINGLE LOW-9 QUOTATION MARK */
	0x00, '?',	/* LATIN SMALL LETTER F WITH HOOK */
	0x00, '?',	/* DOUBLE LOW-9 QUOTATION MARK */
	0x00, '?',	/* HORIZONTAL ELLIPSIS */
	0x00, '?',	/* DAGGER */
	0x00, '?',	/* DOUBLE DAGGER */
	0x00, '?',	/* MODIFIER LETTER CIRCUMFLEX ACCENT */
	0x00, '?',	/* PER MILLE SIGN */
	0x00, '?',	/* LATIN CAPITAL LETTER S WITH CARON */
	0x00, '?',	/* SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
	0x00, '?',	/* LATIN CAPITAL LIGATURE OE */
	0x00, '?',	/* UNDEFINED */
	0x00, '?',	/* LATIN CAPITAL LETTER Z WITH CARON */
	0x00, '?',	/* UNDEFINED */
	0x00, '?',	/* UNDEFINED */
	0x00, '?',	/* LEFT SINGLE QUOTATION MARK */
	0x00, '?',	/* RIGHT SINGLE QUOTATION MARK */
	0x00, '?',	/* LEFT DOUBLE QUOTATION MARK */
	0x00, '?',	/* RIGHT DOUBLE QUOTATION MARK */
	0x00, '?',	/* BULLET */
	0x00, '?',	/* EN DASH */
	0x00, '?',	/* EM DASH */
	0x00, '?',	/* SMALL TILDE */
	0x00, '?',	/* TRADE MARK SIGN */
	0x00, '?',	/* LATIN SMALL LETTER S WITH CARON */
	0x00, '?',	/* SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
	0x00, '?',	/* LATIN SMALL LIGATURE OE */
	0x00, '?',	/* UNDEFINED */
	0x00, '?',	/* LATIN SMALL LETTER Z WITH CARON */
	0x00, '?',	/* LATIN CAPITAL LETTER Y WITH DIAERESIS */
	0x00, ' ',	/*    NO-BREAK SPACE */
	0xad, '?',	/* !` INVERTED EXCLAMATION MARK */
	0x9b, '?',	/* \cent CENT SIGN */
	0x9c, '?',	/* \pound POUND SIGN */
	0x00, '?',	/* CURRENCY SIGN */
	0x9d, '?',	/* \yen YEN SIGN */
	0x00, '|',	/* BROKEN BAR */
	0xdd, '?',	/* PARAGRAPH SIGN, SECTION SIGN */
	0xb9, '?',	/* DIAERESIS */
	0xbd, '?',	/* COPYRIGHT SIGN */
	0xa6, '?',	/* a_ FEMININE ORDINAL INDICATOR */
	0xae, '?',	/* `` LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
	0xaa, '?',	/* \neg NOT SIGN */
	0x00, '-',	/* SOFT HYPHEN */
	0xbe, '?',	/* REGISTERED SIGN */
	0xff, '?',	/* MACRON */
	0xf8, '?',	/* \deg DEGREE SIGN */
	0xf1, '?',	/* +- PLUS-MINUS SIGN */
	0xfd, '?',	/* ^2 SUPERSCRIPT TWO */
	0xfe, '?',	/* ^3 SUPERSCRIPT THREE */
	0xba, '?',	/* ACUTE ACCENT */
	0xe6, '?',	/* MICRO SIGN */
	0xbc, '?',	/* PILCROW SIGN */
	0xfa, '?',	/* \cdot MIDDLE DOT */
	0x00, '?',	/* CEDILLA */
	0x00, '?',	/* ^1 SUPERSCRIPT ONE */
	0xa7, '?',	/* o_ MASCULINE ORDINAL INDICATOR */
	0xaf, '?',	/* '' RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
	0xac, '?',	/* 1/4 VULGAR FRACTION ONE QUARTER */
	0xab, '?',	/* 1/2 VULGAR FRACTION ONE HALF */
	0x00, '?',	/* NOT SIGN */
	0xa8, '?',	/* ?' INVERTED QUESTION MARK */
	0xb6, '?',	/* A` LATIN CAPITAL LETTER A WITH GRAVE */
	0x00, 'A',	/* A' LATIN CAPITAL LETTER A WITH ACUTE */
	0x00, 'A',	/* A^ LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
	0xB7, '?',	/* A~ LATIN CAPITAL LETTER A WITH TILDE */
	0x8e, '?',	/* A" LATIN CAPITAL LETTER A WITH DIAERESIS */
	0x8f, '?',	/* AA LATIN CAPITAL LETTER A WITH RING ABOVE */
	0x92, '?',	/* AE LATIN CAPITAL LETTER AE */
	0x80, '?',	/* C, LATIN CAPITAL LETTER C WITH CEDILLA */
	0x00, 'E',	/* E` LATIN CAPITAL LETTER E WITH GRAVE */
	0x90, '?',	/* E' LATIN CAPITAL LETTER E WITH ACUTE */
	0x00, 'E',	/* E^ LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
	0x00, 'E',	/* E" LATIN CAPITAL LETTER E WITH DIAERESIS */
	0x00, 'I',	/* I` LATIN CAPITAL LETTER I WITH GRAVE */
	0x00, 'I',	/* I' LATIN CAPITAL LETTER I WITH ACUTE */
	0x00, 'I',	/* I^ LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
	0x00, 'I',	/* I" LATIN CAPITAL LETTER I WITH DIAERESIS */
	0x00, '?',	/* LATIN CAPITAL LETTER ETH */
	0xa5, '?',	/* N~ LATIN CAPITAL LETTER N WITH TILDE */
	0x00, 'O',	/* O` LATIN CAPITAL LETTER O WITH GRAVE */
	0x00, 'O',	/* O' LATIN CAPITAL LETTER O WITH ACUTE */
	0x00, 'O',	/* O^ LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
	0xb8, '?',	/* O~ LATIN CAPITAL LETTER O WITH TILDE */
	0x99, '?',	/* O" LATIN CAPITAL LETTER O WITH DIAERESIS */
	0x00, 'x',	/* MULTIPLICATION SIGN */
	0xb2, '?',	/* O/ LATIN CAPITAL LETTER O WITH STROKE */
	0x00, 'U',	/* U` LATIN CAPITAL LETTER U WITH GRAVE */
	0x00, 'U',	/* U' LATIN CAPITAL LETTER U WITH ACUTE */
	0x00, 'U',	/* U^ LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
	0x9a, '?',	/* U" LATIN CAPITAL LETTER U WITH DIAERESIS */
	0x00, 'Y',	/* Y` LATIN CAPITAL LETTER Y WITH ACUTE */
	0x00, '?',	/* LATIN CAPITAL LETTER THORN */
	0x9e, '?',	/* \ss LATIN SMALL LETTER SHARP S */
	0x85, '?',	/* a` LATIN SMALL LETTER A WITH GRAVE */
	0xa0, '?',	/* a' LATIN SMALL LETTER A WITH ACUTE */
	0x83, '?',	/* a^ LATIN SMALL LETTER A WITH CIRCUMFLEX */
	0xb0, '?',	/* a~ LATIN SMALL LETTER A WITH TILDE */
	0x84, '?',	/* a" LATIN SMALL LETTER A WITH DIAERESIS */
	0x86, '?',	/* aa LATIN SMALL LETTER A WITH RING ABOVE */
	0x91, '?',	/* ae LATIN SMALL LETTER AE */
	0x87, '?',	/* c, LATIN SMALL LETTER C WITH CEDILLA */
	0x8a, '?',	/* e` LATIN SMALL LETTER E WITH GRAVE */
	0x82, '?',	/* e' LATIN SMALL LETTER E WITH ACUTE */
	0x88, '?',	/* e^ LATIN SMALL LETTER E WITH CIRCUMFLEX */
	0x89, '?',	/* e^ LATIN SMALL LETTER E WITH CIRCUMFLEX */
	0x8d, '?',	/* i` LATIN SMALL LETTER I WITH GRAVE */
	0xa1, '?',	/* i' LATIN SMALL LETTER I WITH ACUTE */
	0x8c, '?',	/* i^ LATIN SMALL LETTER I WITH CIRCUMFLEX */
	0x8b, '?',	/* i" LATIN SMALL LETTER I WITH DIAERESIS */
	0x00, '?',	/* LATIN SMALL LETTER ETH  */
	0xa4, '?',	/* n~ LATIN SMALL LETTER N WITH TILDE */
	0x95, '?',	/* o` LATIN SMALL LETTER O WITH GRAVE */
	0xa2, '?',	/* o' LATIN SMALL LETTER O WITH ACUTE */
	0x93, '?',	/* o^ LATIN SMALL LETTER O WITH CIRCUMFLEX */
	0xb1, '?',	/* o~ LATIN SMALL LETTER O WITH TILDE */
	0x94, '?',	/* o" LATIN SMALL LETTER O WITH DIAERESIS */
	0xf6, '?',	/* \div DIVISION SIGN */
	0xb3, '?',	/* o/ LATIN SMALL LETTER O WITH STROKE */
	0x97, '?',	/* u` LATIN SMALL LETTER U WITH GRAVE */
	0xa3, '?',	/* u' LATIN SMALL LETTER U WITH ACUTE */
	0x96, '?',	/* u^ LATIN SMALL LETTER U WITH CIRCUMFLEX */
	0x81, '?',	/* u" LATIN SMALL LETTER U WITH DIAERESIS */
	0x00, 'y',	/* Y' LATIN SMALL LETTER Y WITH ACUTE */
	0x00, '?',	/* LATIN SMALL LETTER THORN */
	0x98, '?'	/* y" LATIN SMALL LETTER Y WITH DIAERESIS */
};

static void latin1_to_atarist(char *s)
{
	unsigned int c, newc;
	int warned = 0;

	while ((c = (unsigned char) (*s) & 0xFF) != 0)
	{
		if (c >= 0x80)
		{
			newc = i2a[(c - 0x80) << 1];
			if (newc == 0)
			{
				newc = i2a[((c - 0x80) << 1) + 1];
				if (!warned)
				{
					warn("untranslatable character $%02x in %s, using '%c' instead", c, s, newc);
					warned = 1;
				}
			}
			*s = newc;
		}
		s++;
	}
}


static void converter_noop(char *s)
{
	UNUSED(s);
}

typedef void (*converter_t) (char *);


struct converter_info
{
	const char *from;
	const char *to;
	converter_t func;
};

static struct converter_info const converters[] = {
	{ "latin1", "atarist", latin1_to_atarist },
};

static converter_t get_converter(const char *from, const char *to)
{
	int i;

	int n = ARRAY_SIZE(converters);

	if (strcmp(from, to) == 0)
	{
		return converter_noop;
	}

	for (i = 0; i < n; i++)
	{
		if (strcmp(from, converters[i].from) == 0 && strcmp(to, converters[i].to) == 0)
		{
			return converters[i].func;
		}
	}
	warn("unknown charset conversion %s..%s.", from, to);
	fprintf(stderr, "known conversions are:\n");
	for (i = 0; i < n; i++)
	{
		fprintf(stderr, "  %s..%s\n", converters[i].from, converters[i].to);
	}
	return converter_noop;
}


/*
 * thash - target hash.
 * Needless to say, any change here must be checked against the
 * code using this hash in the run-time routine
 */

/* 1024 entries, means at least 8 KB, plus 8 bytes per string,
 * plus the lengths of strings
 */
#define TH_BITS 10
#define TH_SIZE (1 << TH_BITS)
#define TH_MASK (TH_SIZE - 1)
#define TH_BMASK ((1 << (16 - TH_BITS)) - 1)

static unsigned int compute_th_value(const char *t)
{
	const unsigned char *u = (const unsigned char *) t;
	unsigned int a, b;

	a = 0;
	while (*u)
	{
		b = (a >> 15) & 1;
		a <<= 1;
		a |= b;
		a += *u++;
	}
	b = (a >> TH_BITS) & TH_BMASK;
	a &= TH_MASK;
	a ^= b;
	return a;
}

/*
 * make a big langs.c file from all supplied lang names.
 */


/* decomposes a string <lang><white><encoding>, returning
 * 0 if s is badly formatted.
 * needs a 3-byte buffer in lang
 */
#define LANG_LEN 3
static gboolean parse_linguas_item(const char *s, char *lang, const char **charset)
{
	if (*s < 'a' || *s >= 'z')
		return FALSE;
	*lang++ = *s++;
	if (*s < 'a' || *s >= 'z')
		return FALSE;
	*lang++ = *s++;
	*lang = 0;
	if (*s != ' ' && *s != '\t')
		return FALSE;
	while (*s == ' ' || *s == '\t')
		s++;
	*charset = get_canon_cset_name(s);
	return TRUE;
}


__attribute__((__warn_unused_result__))
static gboolean make(void)
{
	da *d;
	da *th[TH_SIZE];
	oh *o = NULL;
	oh *oref = NULL;
	int i, n, j, m;
	FILE *f = NULL;
	poe *eref;
	char tmp[20];
	char *t;
	char lang[LANG_LEN];
	da *langs = NULL;
	const char *from_charset, *to_charset;
	converter_t converter;
	int numref = 0;						/* number of entries in the reference */
	int numtransl;						/* number of translated entries */
	gboolean retval = FALSE;
	
	langs = da_new();

	d = da_new();
	if (parse_oipl_file(PO_DIR "LINGUAS", d) == FALSE)
		goto errout;

	oref = o_new();
	if (parse_po_file(PO_DIR "messages.pot", oref, TRUE) == FALSE)
		goto errout;

	f = fopen(LANGS_C, "w");
	if (f == NULL)
	{
		error("cannot open " LANGS_C ": %s", strerror(errno));
		goto errout;
	}

	fprintf(f, "\
/*\n\
 * " LANGS_C " - tables for all languages\n\
 *\n\
 * This file was generated by " TOOLNAME " version " VERSION " on %s\n\
 * Do not change this file!\n\
 *\n\
 * For more info, refer to file " DOCNAME "\n\
 */\n\n", now());

	fprintf(f, "#include \"config.h\"\n");
	fprintf(f, "#include \"i18nconf.h\"\n\n");
	fprintf(f, "#if CONF_WITH_NLS\n\n");
	fprintf(f, "#include \"langs.h\"\n\n");

	/* generate the default strings table, and store the
	 * name of the key string in msgstr
	 */
	fprintf(f, "/*\n * The keys for hash tables below.\n */\n\n");

	m = o_len(oref);
	for (j = 0; j < m; j++)
	{
		eref = o_nth(oref, j);
		if (eref->kind == KIND_NORM)
		{
			sprintf(tmp, "nls_key_%d", j);
			eref->msgstr = g_strdup(tmp);
			fprintf(f, "static const char %s [] = ", tmp);
			print_canon(f, eref->msgid.key, "  ", TRUE);
			fprintf(f, ";\n");
			numref++;
		}
	}
	fprintf(f, "\n\n");

	/* for each language, generate a hash table, pointing
	 * back to the keys output above
	 */
	n = da_len(d);
	for (i = 0; i < n; i++)
	{
		/* clear target hash */
		for (j = 0; j < TH_SIZE; j++)
		{
			th[j] = NULL;
		}

		/* obtain destination charset from LINGUAS */
		t = (char *)da_nth(d, i);
		if (!parse_linguas_item(t, lang, &to_charset))
		{
			warn(PO_DIR "LINGUAS: bad lang/charset specification \"%s\"", t);
			continue;
		}

		/* read translations */
		o = o_new();
		sprintf(tmp, PO_DIR "%s.po", lang);
		if (parse_po_file(tmp, o, FALSE) == FALSE)
			goto errout;

		/* get the source charset from the po file */
		{
			ae_t a = { NULL, NULL, NULL, NULL };
			poe *e = o_find(o, "");

			if (e == NULL || !parse_ae(e->msgstr, &a))
			{
				warn("%s: bad administrative entry", tmp);
				free_pot_ae(&a);
				continue;
			}
			from_charset = get_canon_cset_name(a.charset);
			free_pot_ae(&a);
		}
		da_add(langs, g_strdup(lang));

		converter = get_converter(from_charset, to_charset);

		/* compare o to oref */
		numtransl = 0;
		m = o_len(o);
		for (j = 0; j < m; j++)
		{
			poe *e = o_nth(o, j);

			if (e->kind == KIND_NORM && e->msgstr && strcmp("", e->msgstr) != 0)
			{
				eref = o_find(oref, e->msgid.key);
				if (eref)
				{
					unsigned int a = compute_th_value(e->msgid.key);

					if (th[a] == NULL)
					{
						th[a] = da_new();
					}
					da_add(th[a], eref->msgstr);
					/* translate into destination encoding */
					converter(e->msgstr);
					da_add(th[a], e->msgstr);
					numtransl++;
				}
			}
		}

		/* print stats if some entries are missing */
		if (numtransl < numref)
		{
			printf("lang %s: %d untranslated entr%s\n",
				   lang, numref - numtransl, (numref - numtransl == 1) ? "y" : "ies");
		}

		/* dump the hash table */
		fprintf(f, "/*\n * hash table for lang %s.\n */\n\n", lang);
		for (j = 0; j < TH_SIZE; j++)
		{
			if (th[j] != NULL)
			{
				int ii, nn, rc;

				fprintf(f, "static const char * const msg_%s_hash_%d[] = {\n", lang, j);
				nn = da_len(th[j]);
				for (ii = 0; ii < nn; ii += 2)
				{
					fprintf(f, "  %s, ", (char *) da_nth(th[j], ii));
					rc = print_canon(f, (char *)da_nth(th[j], ii + 1), "    ", TRUE);
					if (rc < 0)
						print_alert_warning(rc, lang, (char *)da_nth(th[j], ii));
					fprintf(f, ",\n");
				}
				fprintf(f, "  0\n};\n\n");
			}
		}
		fprintf(f, "static const char * const * const msg_%s[] = {\n", lang);
		for (j = 0; j < TH_SIZE; j++)
		{
			if (th[j])
			{
				fprintf(f, "  msg_%s_hash_%d,\n", lang, j);
				da_free(th[j]);
			} else
			{
				fprintf(f, "  0,\n");
			}
		}
		fprintf(f, "  0\n};\n\n");

		/* free this po */
		o_free(o, TRUE);
		o = NULL;
	}

	/* print a lang table */
	fprintf(f, "/*\n * the table of available langs.\n */\n\n");
	n = da_len(langs);
	for (i = 0; i < n; i++)
	{
		t = (char *)da_nth(langs, i);
		fprintf(f, "\
static const struct lang_info lang_%s = { \"%s\", msg_%s };\n", t, t, t);
	}
	fprintf(f, "\n");
	fprintf(f, "const struct lang_info * const langs[] = {\n");
	for (i = 0; i < n; i++)
	{
		t = (char *)da_nth(langs, i);
		fprintf(f, "  &lang_%s, \n", t);
	}
	fprintf(f, "  0,\n};\n\n#endif /* CONF_WITH_NLS */\n");
	retval = TRUE;

errout:
	free_oipl_file(langs);
	if (f)
		fclose(f);
	free_oipl_file(d);
	o_free(o, TRUE);
	o_free(oref, TRUE);
	return retval;
}

/*
 * translate
 */

__attribute__((__warn_unused_result__))
static gboolean translate(char *lang, char *from)
{
	pcati p = { 0, 0, 0 };
	char *to = NULL;
	char po[10];
	const char *from_charset, *to_charset;
	gboolean retval = FALSE;
	
	{									/* build destination filename */
		const char *dot = strrchr(from, '.');
		int len = strlen(from);
		
		if (dot == NULL || (strcmp(dot, ".c") != 0 && strcmp(dot, ".rsh") != 0))
		{
			warn("I only translate .c files");
			goto errout;
		}
		to = g_new(char, len + 3);
		strcpy(to, from);
		strcpy(to + (dot - from), ".tr.c");
	}
	p.f = fopen(to, "w");
	if (p.f == NULL)
	{
		warn("cannot create %s: %s", to, strerror(errno));
		goto errout;
	}
	g_free(to);
	to = NULL;

	to_charset = NULL;
	{									/* obtain destination charset from LINGUAS */
		da *d = da_new();
		int i, n;

		if (parse_oipl_file(PO_DIR "LINGUAS", d) == FALSE)
		{
			free_oipl_file(d);
			goto errout;
		}

		n = da_len(d);
		for (i = 0; i < n; i++)
		{
			char *t = (char *)da_nth(d, i);
			char l[LANG_LEN];

			if (!parse_linguas_item(t, l, &to_charset))
			{
				warn(PO_DIR "LINGUAS: bad lang/charset specification \"%s\"", t);
			} else if (strcmp(lang, l) == 0)
			{
				break;
			}
		}
		free_oipl_file(d);
	}
	if (to_charset == NULL)
	{
		warn("cannot find destination charset.");
		to_charset = "unknown";
	}

	/* read all translations */
	p.o = o_new();
	sprintf(po, PO_DIR "%s.po", lang);
	if (parse_po_file(po, p.o, FALSE) == FALSE)
		goto errout;

	{									/* get the source charset from the po file */
		ae_t a = { NULL, NULL, NULL, NULL };
		poe *e = o_find(p.o, "");

		if (e == NULL || !parse_ae(e->msgstr, &a))
		{
			warn("%s: bad administrative entry", po);
			free_pot_ae(&a);
			goto errout;
		}
		from_charset = get_canon_cset_name(a.charset);
		free_pot_ae(&a);
	}

	p.conv = get_converter(from_charset, to_charset);

	retval = parse_c_file(from, &pca_translate, &p);

errout:
	o_free(p.o, TRUE);
	if (p.f)
		fclose(p.f);
	g_free(to);
	return retval;
}



static void usage(FILE *fp)
{
	fprintf(fp, "\
Usage: " TOOLNAME " command\n");

	fprintf(fp, "\
Commands are:\n\
  xgettext       scans source files listed in POTFILES.in \n\
                 and (re)creates messages.pot\n\
  update xx.po   compares xx.po to the current messages.pot \n\
                 and creates a new xx.po (new entries added, old \n\
                 entries commented out)\n\
  translate xx from.c\n\
                 translates from.c into from.tr.c for language xx.\n\
  make           takes all languages listed in file LINGUAS \n\
                 and creates the C file(s) for the project\n");

	fprintf(fp, "\
\n\
Note: " TOOLNAME " is a very limited gettext clone, with some compatibility \n\
with the original gettext. To have more control of your po files, \n\
please use the original gettext utilities. You will still need this \n\
tool to create the C file(s) at the end, though.\n");
}

/*
 * main
 */

int main(int argc, char **argv)
{
	gboolean ret;
	
	if (argc < 2)
	{
		usage(stderr);
		ret = FALSE;
	} else if (strcmp(argv[1], "xgettext") == 0)
	{
		if (argc != 2)
		{
			usage(stderr);
			return EXIT_FAILURE;
		}
		ret = xgettext();
	} else if (strcmp(argv[1], "update") == 0)
	{
		if (argc != 3)
		{
			usage(stderr);
			return EXIT_FAILURE;
		}
		ret = update(argv[2]);
	} else if (strcmp(argv[1], "make") == 0)
	{
		if (argc != 2)
		{
			usage(stderr);
			return EXIT_FAILURE;
		}
		ret = make();
	} else if (strcmp(argv[1], "translate") == 0)
	{
		if (argc != 4)
		{
			usage(stderr);
			return EXIT_FAILURE;
		}
		ret = translate(argv[2], argv[3]);
	} else if (strcmp(argv[1], "--version") == 0)
	{
		printf("version " VERSION "\n");
		ret = TRUE;
	} else if (strcmp(argv[1], "--help") == 0)
	{
		usage(stdout);
		ret = TRUE;
	} else
	{
		usage(stderr);
		ret = FALSE;
	}
	return ret ? EXIT_SUCCESS : EXIT_FAILURE;
}
