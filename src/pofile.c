#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include "nls.h"
#include "pofile.h"
#include "debug.h"


struct lang_info
{
	char *name;
	int fontcharset;
	int in_use;
};

typedef struct da
{
	int size;
	void **buf;
	int len;
} da;

static da *languages;


#define PO_DIR "../po/"

#define ARRAY_SIZE(array) ((int)(sizeof(array)/sizeof(array[0])))


/*
 * errors
 */

__attribute__((format(printf, 1, 2)))
static void warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	errout("Warning: ");
	erroutv(fmt, ap);
	errout("\n");
	va_end(ap);
}

__attribute__((format(printf, 1, 2)))
static void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	errout("Fatal: ");
	erroutv(fmt, ap);
	errout("\n");
	va_end(ap);
	exit(EXIT_FAILURE);
}

__attribute__((format(printf, 1, 2)))
static void error(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	errout("Error: ");
	erroutv(fmt, ap);
	errout("\n");
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
		fatal("%s", strerror(errno));
	return a;
}

static void *xrealloc(void *b, size_t s)
{
	void *a = realloc(b, s);

	if (a == NULL)
		fatal("%s", strerror(errno));
	return a;
}


static void g_free(void *s)
{
	if (s)
	{
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
 * da - dynamic array
 */
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
	/* the font id needed to display translated entries */
	int to_charset;
} oh;


static oh *o_new(void)
{
	oh *o = g_new(oh, 1);

	o->h = h_new();
	o->d = da_new();
	o->to_charset = -1;
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
 * read a simple textfile,
 * ignoring lines that start with a #,
 * and empty lines
 */
typedef void (*parse_oipl_callback)(da *d, char *str, int arg);

__attribute__((__warn_unused_result__))
static gboolean parse_oipl_file(const char *fname, da *d, parse_oipl_callback func, int arg, int mustexist)
{
	int c;
	IFILE *f;

	f = ifopen(fname);
	if (f == NULL)
	{
		if (mustexist)
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
			func(d, s_detach(s), arg);
		}
	}
	ifclose(f);
	return TRUE;
}


/* decomposes a string <lang><white><encoding>, adding
 * a lang_info item from it to the array
 */
static void parse_linguas_item(da *d, char *s, int in_use)
{
	struct lang_info *info;
	char *p = s;
	int id;
	
	while (is_letter(*p) || *p == '_')
		p++; 
	if (p > s && is_white(*p))
	{
		*p++ = '\0';
		while (is_white(*p))
			p++;
		if ((id = po_get_charset_id(p)) >= 0)
		{
			info = g_new(struct lang_info, 1);
			info->fontcharset = id;
			info->name = s;
			info->in_use = in_use;
			da_add(d, info);
			return;
		}
	}
	warn("LINGUAS: bad lang/charset specification \"%s\"", s);
	g_free(s);
}


/*
 * determine "underscore length"
 *
 * the "underscore length" for a string is the length of the first substring
 * that starts with an underscore and contains only underscores and periods.
 * this is designed to catch simple typos that translators might make when
 * translating tedinfo strings.
 */
static inline int underscore_length(const char *s)
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
static gboolean parse_po_file(nls_domain *domain, const char *fname, oh *o, gboolean ignore_ae)
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
#if 0 /* check now done in xlate_file */
			if (e->msgstr && strlen(e->msgstr))		/* really translating */
			{
				if (underscore_length(e->msgid.key) != underscore_length(e->msgstr))
					warn("%s: underscores appear invalid for translation of '%s'", fname, e->msgid.key);
			}
#endif
			if (e->msgid.key && *e->msgid.key != '\0' && e->msgstr && *e->msgstr != '\0')
			{
				size_t lp = strlen(e->msgid.key);
				size_t ln = strlen(e->msgstr);
				if ((e->msgid.key[lp - 1] == '\n' && e->msgstr[ln - 1] != '\n') ||
					(e->msgid.key[lp - 1] != '\n' && e->msgstr[ln - 1] == '\n'))
				{
					char *from = nls_conv_to_utf8(CHARSET_ST, e->msgid.key, lp, TRUE);
					char *utf8 = nls_conv_to_utf8(domain->fontset, e->msgstr, ln, TRUE);
					warn("entries do not both end with '\\n' in translation of '%s' to '%s'",
						from, utf8);
					g_free(utf8);
					g_free(from);
				}
			}
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
 * charsets
 */

struct charset_alias
{
	const char *name;
	int id;
};

static struct charset_alias const charsets[] = {
 	{ "ISO-8859-1", CHARSET_L1 },
 	{ "latin1", CHARSET_L1 },
	{ "ISO-8859-2", CHARSET_L2 },
	{ "latin2", CHARSET_L2 },
	{ "ISO-8859-15", CHARSET_L9 },
	{ "latin9", CHARSET_L9 },
	{ "russian-atarist", CHARSET_RU },
	{ "cp737", CHARSET_GR },
	{ "atarist", CHARSET_ST }
};


/* resolve any known alias */
int po_get_charset_id(const char *name)
{
	int i;
	int n = ARRAY_SIZE(charsets);

	for (i = 0; i < n; i++)
	{
		if (strcmp(charsets[i].name, name) == 0)
		{
			return charsets[i].id;
		}
	}
	warn("unknown charset name %s", name);
	errout("known charsets are:\n");
	for (i = 0; i < n; i++)
	{
		errout("  %s\n", charsets[i].name);
	}
	return -1;
}

/*
 * opposite of po_get_charset_id.
 * Only used for error reporting.
 */
static const char *get_charset_name(int id)
{
	int i;
	int n = ARRAY_SIZE(charsets);

	for (i = 0; i < n; i++)
	{
		if (charsets[i].id == id)
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
static void converter_noop(char *s)
{
	UNUSED(s);
}

typedef void (*converter_t) (char *);


struct converter_info
{
	int from;
	int to;
	converter_t func;
};

/*
 * note: only po-files encoded in latin1 get recoded,
 * this currently affects de, fi, fr, and it.
 * All others use a font that matches the encoding
 * of the input files.
 */
static struct converter_info const converters[] = {
	{ CHARSET_L1, CHARSET_ST, latin1_to_atarist },
};


static const struct lang_info *get_language_info(const char *lang)
{
	int i;
	int n = da_len(languages);

	for (i = 0; i < n; i++)
	{
		const struct lang_info *info = (const struct lang_info *)da_nth(languages, i);
		if (strcmp(info->name, lang) == 0)
		{
			return info;
		}
	}
	return NULL;
}


static int get_language_charset(const char *lang)
{
	const struct lang_info *info = get_language_info(lang);
	int i, n;
	
	if (info != NULL)
		return info->fontcharset;
	warn("unknown language %s.", lang);
	errout("known languages are:\n");
	n = da_len(languages);
	for (i = 0; i < n; i++)
	{
		info = (const struct lang_info *)da_nth(languages, i);
		errout("  %s\n", info->name);
	}
	return -1;
}


static converter_t get_converter(const char *from, int to_id)
{
	int i;
	int from_id;
	int n = ARRAY_SIZE(converters);

	from_id = po_get_charset_id(from);
	
	if (from_id == to_id)
	{
		return converter_noop;
	}

	for (i = 0; i < n; i++)
	{
		if (converters[i].from == from_id && converters[i].to == to_id)
		{
			return converters[i].func;
		}
	}
	warn("unknown charset conversion %s..%s.", from, get_charset_name(to_id));
	errout("known conversions are:\n");
	for (i = 0; i < n; i++)
	{
		errout("  %s..%s\n", get_charset_name(converters[i].from), get_charset_name(converters[i].to));
	}
	return converter_noop;
}


void po_init(const char *po_dir, int mustexist)
{
	str *s;
	da *d;
	int i;
	char *fname;
	
	if (languages != NULL)
		return;
	if (po_dir == NULL)
		po_dir = PO_DIR;
	s = s_new();
	s_addstr(s, po_dir);
	i = strlen(po_dir);
	if (i > 0 && po_dir[i - 1] != '/')
		s_addstr(s, "/");
	s_addstr(s, "LINGUAS");
	fname = s_detach(s);
	d = da_new();
	if (!parse_oipl_file(fname, d, parse_linguas_item, TRUE, mustexist))
	{
		/*
		 * construct a list of languages as fallback
		 */
		parse_linguas_item(d, g_strdup("en atarist"), FALSE);
		parse_linguas_item(d, g_strdup("de atarist"), FALSE);
		parse_linguas_item(d, g_strdup("fr atarist"), FALSE);
		parse_linguas_item(d, g_strdup("fi atarist"), FALSE);
		parse_linguas_item(d, g_strdup("it atarist"), FALSE);
		parse_linguas_item(d, g_strdup("nn atarist"), FALSE);
		parse_linguas_item(d, g_strdup("sv atarist"), FALSE);
		parse_linguas_item(d, g_strdup("cs latin2"), FALSE);
		parse_linguas_item(d, g_strdup("es latin9"), FALSE);
		parse_linguas_item(d, g_strdup("ru russian-atarist"), FALSE);
		parse_linguas_item(d, g_strdup("gr cp737"), FALSE); /* deprecated */
		parse_linguas_item(d, g_strdup("el cp737"), FALSE);
	}
	languages = d;
	g_free(fname);
}


void po_exit(void)
{
	int i, n;
	da *d;
	
	d = languages;
	if (d == NULL)
		return;
	n = da_len(d);
	for (i = 0; i < n; i++)
	{
		struct lang_info *info = (struct lang_info *)da_nth(d, i);
		g_free(info->name);
		g_free(info);
	}
	da_free(d);
	languages = NULL;
}


/*
 * load po file
 */
__attribute__((__warn_unused_result__))
static oh *po_load(nls_domain *domain, const char *po_dir)
{
	oh *o;
	poe *e;
	FILE *f = NULL;
	int i, n;
	int numtransl = 0;					/* number of translated entries */
	int numuntransl = 0;				/* number of untranslated entries */
	gboolean retval = FALSE;
	char *from_charset = NULL;
	char *fname;
	str *s;
	converter_t converter;
	
	if (po_dir == NULL)
		po_dir = PO_DIR;
	
	s = s_new();
	s_addstr(s, po_dir);
	i = strlen(po_dir);
	if (i > 0 && po_dir[i - 1] != '/')
		s_addstr(s, "/");
	s_addstr(s, domain->lang);
	s_addstr(s, ".po");
	fname = s_detach(s);
	
	o = o_new();
	if (parse_po_file(domain, fname, o, FALSE) == FALSE)
		goto errout;

	/* get the source charset from the po file */
	{
		ae_t a = { NULL, NULL, NULL, NULL };
		poe *e = o_find(o, "");

		if (e == NULL || !parse_ae(e->msgstr, &a))
		{
			warn("%s: bad administrative entry", fname);
			from_charset = g_strdup("ISO-8859-1");
		} else
		{
			from_charset = g_strdup(a.charset);
		}
		free_pot_ae(&a);
		o->to_charset = get_language_charset(domain->lang);
		domain->fontset = o->to_charset;
	}
	converter = get_converter(from_charset, o->to_charset);
	
	n = o_len(o);
	for (i = 0; i < n; i++)
	{
		e = o_nth(o, i);
		if (e->kind == KIND_COMM)
		{
			/* comment, ignore */
		} else if (e->msgid.key[0] == 0)
		{
			/* the old admin entry - do nothing */
		} else
		{
			if (e->msgstr && strcmp("", e->msgstr) != 0)
			{
				numtransl++;
				/* recode the translation */
				converter(e->msgstr);
			} else
			{
				numuntransl++;
				/* if there is no translation, we must recode the original string */
				converter(e->msgid.key);
			}
		}
	}

	/* print stats */
	KINFO(("translated %d, untranslated %d\n", numtransl, numuntransl));
	retval = TRUE;
	
errout:
	if (f)
		fclose(f);
	
	if (!retval)
	{
		o_free(o, TRUE);
		o = NULL;
	}
	
	g_free(from_charset);
	g_free(fname);
	
	return o;
}


#define TH_BITS 10
#define TH_SIZE (1 << TH_BITS)

gboolean po_create_hash(const char *lang, nls_domain *domain, const char *po_dir)
{
	oh *o;
	da *th[TH_SIZE];
	int i, n;
	char ***hash;
	
	domain->lang = lang;
	o = po_load(domain, po_dir);
	if (o == NULL)
		return FALSE;
	
	/* clear target hash */
	for (i = 0; i < TH_SIZE; i++)
	{
		th[i] = NULL;
	}
	
	n = o_len(o);
	for (i = 0; i < n; i++)
	{
		poe *e = o_nth(o, i);
		
		if (e->kind == KIND_NORM && e->msgid.key[0] != '\0' && e->msgstr && strcmp("", e->msgstr) != 0)
		{
			unsigned int a = nls_hash(e->msgid.key);
			if (th[a] == NULL)
			{
				th[a] = da_new();
			}
			da_add(th[a], e->msgid.key);
			da_add(th[a], e->msgstr);
		}
	}
	
	/* create the nls hash table */
	hash = g_new0(char **, TH_SIZE);
	for (i = 0; i < TH_SIZE; i++)
	{
		if (th[i])
		{
			int ii, nn;
			char **t;
			
			nn = da_len(th[i]);
			t = g_new(char *, nn + 1);
			hash[i] = t;
			for (ii = 0; ii < nn; ii += 2)
			{
				t[ii] = g_strdup((char *)da_nth(th[i], ii));
				t[ii + 1] = g_strdup((char *)da_nth(th[i], ii + 1));
			}			
			da_free(th[i]);
			/* terminate the list of translatable string */
			t[nn] = NULL;
		}
	}
	
	domain->hash = (const char *const *const *)hash;
	
	o_free(o, TRUE);
	
	return TRUE;
}


void po_delete_hash(nls_domain *domain)
{
	char ***hash = (char ***)NO_CONST(domain->hash);
	char **t;
	int i, ii;
	
	if (hash == NULL)
		return;
	for (i = 0; i < TH_SIZE; i++)
	{
		t = hash[i];
		if (t != NULL)
		{
			for (ii = 0; t[ii] != NULL; ii++)
				g_free(t[ii]);
			g_free(t);
		}
	}
	g_free(hash);
	domain->hash = NULL;
}
