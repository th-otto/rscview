#define _GNU_SOURCE

#include "config.h"
#include <ro_mem.h>
#include <time.h>
#include "debug.h"

#undef min
#define	min(a, b)	((a) < (b) ? (a) : (b))
#undef max
#define	max(a, b)	((a) > (b) ? (a) : (b))

/*** ---------------------------------------------------------------------- ***/

#ifndef HAVE_GLIB
char *g_strdup(const char *str)
{
	char *p;
	size_t len;
	
	if (str == NULL)
		return NULL;
	len = strlen(str);
	if ((p = g_new(char, len + 1)) != NULL)
	{
		strcpy(p, str);
	}
	return p;
}

/*** ---------------------------------------------------------------------- ***/

char *g_strndup(const char *str, size_t len)
{
	char *dst;
	
	if (str == NULL)
		return NULL;
	if (len == STR0TERM)
		len = strlen(str);
	dst = g_new(char, len + 1);
	if (dst == NULL)
		return NULL;
	memcpy(dst, str, sizeof(char) * len);
	dst[len] = '\0';
	return dst;
}

/*** ---------------------------------------------------------------------- ***/

char *g_strconcat(const char *first, ...)
{
	va_list args;
	size_t len;
	const char *str;
	char *ret, *ptr;
	
	if (first == NULL)
		return NULL;
	len = strlen(first) + 1;
	va_start(args, first);
	for (;;)
	{
		str = va_arg(args, const char *);
		if (str == NULL)
			break;
		len += strlen(str);
	}
	va_end(args);
	ret = g_new(char, len);
	if (ret == NULL)
		return NULL;
	strcpy(ret, first);
	ptr = ret + strlen(ret);
	va_start(args, first);
	for (;;)
	{
		str = va_arg(args, const char *);
		if (str == NULL)
			break;
		strcpy(ptr, str);
		ptr += strlen(ptr);
	}
	va_end(args);
	return ret;
}

/*** ---------------------------------------------------------------------- ***/

char *g_strdup_vprintf(const char *format, va_list args)
{
	char *res;
	int len;
	size_t initsize;
	
/* Pure-C lacks vsnprintf() */
#if defined(__PUREC__) && defined(_PUREC_SOURCE)
	initsize = 1000000ul;
	res = g_new(char, initsize);
	while (res == NULL && initsize > 128)
	{
		initsize >>= 1;
		res = g_new(char, initsize);
	}
	if (res == NULL)
	{
		return NULL;
	}

	len = vsprintf(res, format, args);
	if (len >= initsize)
	{
		unreachable();
	}
	res = g_renew(char, res, (len + 1));
#else
	va_list args2;

	initsize = 1024;
	res = g_new(char, initsize);
	if (res == NULL)
	{
		return NULL;
	}
	G_VA_COPY(args2, args);

	len = vsnprintf(res, initsize, format, args);
	if ((size_t)len >= initsize)
	{
		initsize = len + 1;
		res = g_renew(char, res, initsize);
		if (res != NULL)
		{
			len = vsnprintf(res, initsize, format, args2);
			if ((size_t)len >= initsize)
			{
				unreachable();
			}
		}
	}
	va_end(args2);
#endif
	
	return res;
}

/*** ---------------------------------------------------------------------- ***/

char *g_strdup_printf(const char *format, ...)
{
	va_list args;
	char *res;
	
	va_start(args, format);
	res = g_strdup_vprintf(format, args);
	va_end(args);
	return res;
}

/*** ---------------------------------------------------------------------- ***/

/*
 * Removes trailing whitespace from a string.
 */
char *g_strchomp(char *str)
{
	char *end;
	
	if (str == NULL)
		return NULL;
	end = str + strlen(str) - 1;
	while (end > str && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n'))
		--end;
	*++end = '\0';
	return str;
}

/*** ---------------------------------------------------------------------- ***/

/*
 * Removes leading whitespace from a string, by moving the rest of the characters forward.
 */
char *g_strchug(char *str)
{
	char *src;
	
	if (str == NULL)
		return NULL;
	src = str;
	while (*src == ' ' || *src == '\t')
		src++;
	if (str != src)
		memmove(str, src, strlen(src) + 1);
	return str;
}

/*** ---------------------------------------------------------------------- ***/

void g_strfreev(char **str_array)
{
	if (str_array)
	{
		int i;

		for (i = 0; str_array[i] != NULL; i++)
			g_free(str_array[i]);

		g_free(str_array);
	}
}

/*** ---------------------------------------------------------------------- ***/

unsigned int g_strv_length(char **str_array)
{
	int i = 0;
	if (str_array)
	{
		for (; str_array[i] != NULL; i++)
			;
	}
	return i;
}

/*** ---------------------------------------------------------------------- ***/

char **g_strsplit(const char *string, const char *delimiter, int max_tokens)
{
	char **str_array;
	const char *s;
	unsigned int n;
	const char *remainder;
	size_t delimiter_len;
	int tokens;
	
	if (string == NULL)
		return NULL;
	if (delimiter == NULL)
		return NULL;
	delimiter_len = strlen(delimiter);
	if (delimiter_len == 0)
		return NULL;

	if (max_tokens < 1)
		max_tokens = INT_MAX;

	remainder = string;
	n = 0;
	s = strstr(remainder, delimiter);
	tokens = max_tokens;
	if (s)
	{
		while (--tokens && s)
		{
			n++;
			remainder = s + delimiter_len;
			s = strstr(remainder, delimiter);
		}
	}
	if (*string)
	{
		n++;
	}

	str_array = g_new(char *, n + 1);
	if (str_array == NULL)
		return NULL;
	
	remainder = string;
	n = 0;
	s = strstr(remainder, delimiter);
	tokens = max_tokens;
	if (s)
	{
		while (--tokens && s)
		{
			size_t len;

			len = (const char *)s - remainder;
			str_array[n] = g_strndup(remainder, len);
			n++;
			remainder = s + delimiter_len;
			s = strstr(remainder, delimiter);
		}
	}
	if (*string)
	{
		str_array[n] = g_strdup(remainder);
		n++;
	}

	str_array[n] = NULL;

	return str_array;
}

/*** ---------------------------------------------------------------------- ***/

char *g_stpcpy(char *dest, const char *src)
{
	if (dest == NULL)
		return NULL;
	if (src == NULL)
		return NULL;
#ifdef HAVE_STPCPY
	return stpcpy(dest, src);
#else
	do
		*dest++ = *src;
	while (*src++ != '\0');

	return dest - 1;
#endif
}

/*** ---------------------------------------------------------------------- ***/

char *g_strjoinv(const char *separator, char **str_array)
{
	char *string;
	char *ptr;

	if (str_array == NULL)
		return NULL;

	if (separator == NULL)
		separator = "";

	if (*str_array)
	{
		int i;
		size_t len;
		size_t separator_len;

		separator_len = strlen(separator);
		/* First part, getting length */
		len = 1 + strlen(str_array[0]);
		for (i = 1; str_array[i] != NULL; i++)
			len += strlen(str_array[i]);
		len += separator_len * (i - 1);

		/* Second part, building string */
		string = g_new(char, len);
		ptr = g_stpcpy(string, *str_array);
		for (i = 1; str_array[i] != NULL; i++)
		{
			ptr = g_stpcpy(ptr, separator);
			ptr = g_stpcpy(ptr, str_array[i]);
		}
	} else
	{
		string = g_strdup("");
	}
	
	return string;
}

/*** ---------------------------------------------------------------------- ***/

#define ISSPACE(c)              ((c) == ' ' || (c) == '\f' || (c) == '\n' || \
                                 (c) == '\r' || (c) == '\t' || (c) == '\v')
#define ISUPPER(c)              ((c) >= 'A' && (c) <= 'Z')
#define ISLOWER(c)              ((c) >= 'a' && (c) <= 'z')
#define ISALPHA(c)              (ISUPPER (c) || ISLOWER (c))
#define TOUPPER(c)              (ISLOWER (c) ? (c) - 'a' + 'A' : (c))
#define TOLOWER(c)              (ISUPPER (c) ? (c) - 'A' + 'a' : (c))

int g_ascii_strcasecmp(const char *s1, const char *s2)
{
	int c1, c2;

	while (*s1 && *s2)
    {
		c1 = (int)(unsigned char) TOLOWER (*s1);
		c2 = (int)(unsigned char) TOLOWER (*s2);
		if (c1 != c2)
			return c1 - c2;
		s1++; s2++;
	}

	return (((int)(unsigned char) *s1) - ((int)(unsigned char) *s2));
}

/*** ---------------------------------------------------------------------- ***/

/**
 * g_ascii_strdown:
 * @str: a string
 * @len: length of @str in bytes, or -1 if @str is nul-terminated
 *
 * Converts all upper case ASCII letters to lower case ASCII letters.
 *
 * Returns: a newly-allocated string, with all the upper case
 *     characters in @str converted to lower case, with semantics that
 *     exactly match g_ascii_tolower(). (Note that this is unlike the
 *     old g_strdown(), which modified the string in place.)
 */
char *g_ascii_strdown(const char *str, gssize len)
{
	char *result, *s;

	if (str == NULL)
		return NULL;

	if (len < 0)
		len = strlen(str);

	result = g_strndup(str, len);
	for (s = result; *s; s++)
		*s = TOLOWER(*s);

	return result;
}

/*** ---------------------------------------------------------------------- ***/

int g_ascii_strncasecmp(const char *s1, const char *s2, size_t n)
{
	int c1, c2;
	
	while (n && *s1 && *s2)
	{
		n -= 1;
		c1 = (int)(unsigned char) TOLOWER (*s1);
		c2 = (int)(unsigned char) TOLOWER (*s2);
		if (c1 != c2)
			return c1 - c2;
		s1++; s2++;
	}
	
	if (n)
		return (((int) (unsigned char) *s1) - ((int) (unsigned char) *s2));
	return 0;
}

/*** ---------------------------------------------------------------------- ***/

GSList *g_slist_remove(GSList *list, gconstpointer data)
{
	GSList *l, **last;
	
	for (last = &list; (l = *last) != NULL; last = &(*last)->next)
	{
		if (l->data == data)
		{
			*last = l->next;
			g_slist_free_1(l);
			break;
		}
	}
	return list;
}

/*** ---------------------------------------------------------------------- ***/

GSList *g_slist_prepend(GSList *list, gpointer data)
{
	GSList *l;
	
	l = g_new(GSList, 1);
	l->data = data;
	l->next = list;
	return l;
}

/*** ---------------------------------------------------------------------- ***/

GSList *g_slist_append(GSList *list, gpointer data)
{
	GSList *l, **last;
	
	for (last = &list; *last != NULL; last = &(*last)->next)
		;
	l = g_new(GSList, 1);
	l->data = data;
	l->next = NULL;
	*last = l;
	return list;
}

/*** ---------------------------------------------------------------------- ***/

static inline GSList *_g_slist_remove_link(GSList *list, GSList *link)
{
	GSList *tmp;
	GSList *prev;

	prev = NULL;
	tmp = list;

	while (tmp)
	{
		if (tmp == link)
		{
			if (prev)
				prev->next = tmp->next;
			if (list == tmp)
				list = list->next;

			tmp->next = NULL;
			break;
		}

		prev = tmp;
		tmp = tmp->next;
	}

	return list;
}

GSList *g_slist_remove_link(GSList *list, GSList *link_)
{
	return _g_slist_remove_link(list, link_);
}

/*** ---------------------------------------------------------------------- ***/

void g_slist_free_full(GSList *list, void (*freefunc)(void *))
{
	GSList *l, *next;
	
	for (l = list; l; l = next)
	{
		next = l->next;
		if (freefunc)
 			freefunc(l->data);
		g_slist_free_1(l);
	}
}

/*** ---------------------------------------------------------------------- ***/

void g_slist_free(GSList *list)
{
	g_slist_free_full(list, 0);
}

/*** ---------------------------------------------------------------------- ***/

GSList *g_slist_delete_link(GSList *list, GSList *link_)
{
	list = _g_slist_remove_link(list, link_);
	g_slist_free_1(link_);

	return list;
}

/*** ---------------------------------------------------------------------- ***/

int g_ascii_xdigit_value(char c)
{
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= '0' && c <= '9')
		return c - '0';
	return -1;
}

/*** ---------------------------------------------------------------------- ***/

#define MY_MAXSIZE ((gsize)-1)

static inline gsize nearest_power(gsize base, gsize num)
{
	if (num > MY_MAXSIZE / 2)
	{
		return MY_MAXSIZE;
	} else
	{
		gsize n = base;

		while (n < num)
			n <<= 1;

		return n;
	}
}

static void g_string_maybe_expand(GString *string, gsize len)
{
	if (string->len + len >= string->allocated_len)
	{
		string->allocated_len = nearest_power(1, string->len + len + 1);
		string->str = g_renew(char, string->str, string->allocated_len);
	}
}

/*** ---------------------------------------------------------------------- ***/

GString *g_string_insert_c(GString *string, gssize pos, char c)
{
	if (string == NULL)
		return NULL;

	g_string_maybe_expand(string, 1);

	if (pos < 0)
		pos = string->len;
	else if ((gsize)pos > string->len)
		return string;

	/* If not just an append, move the old stuff */
	if ((gsize)pos < string->len)
		memmove(string->str + pos + 1, string->str + pos, string->len - pos);

	string->str[pos] = c;
	string->len += 1;
	string->str[string->len] = 0;

	return string;
}

/*** ---------------------------------------------------------------------- ***/

GString *g_string_append_c(GString *string, char c)
{
	if (string == NULL)
		return NULL;

	return g_string_insert_c(string, -1, c);
}

/*** ---------------------------------------------------------------------- ***/

GString *g_string_sized_new(gsize dfl_size)
{
	GString *string = g_new(GString, 1);

	string->allocated_len = 0;
	string->len = 0;
	string->str = NULL;

	g_string_maybe_expand(string, max(dfl_size, 2));
	string->str[0] = 0;

	return string;
}

/*** ---------------------------------------------------------------------- ***/

GString *g_string_new(const char *init)
{
	GString *string;

	if (init == NULL || *init == '\0')
		string = g_string_sized_new(2);
	else
	{
		gsize len;

		len = strlen(init);
		string = g_string_sized_new(len + 2);

		g_string_append_len(string, init, len);
	}

	return string;
}

/*** ---------------------------------------------------------------------- ***/

GString *g_string_insert_len(GString *string, gssize pos, const char *val, gssize len)
{
	if (string == NULL)
		return NULL;
	if (len != 0 && val == NULL)
		return string;

	if (len == 0)
		return string;

	if (len < 0)
		len = strlen(val);

	if (pos < 0)
		pos = string->len;
	else if ((gsize)pos > string->len)
		return string;

	/* Check whether val represents a substring of string.
	 * This test probably violates chapter and verse of the C standards,
	 * since ">=" and "<=" are only valid when val really is a substring.
	 * In practice, it will work on modern archs.
	 */
	if (val >= string->str && val <= string->str + string->len)
	{
		gsize offset = val - string->str;
		gsize precount = 0;

		g_string_maybe_expand(string, len);
		val = string->str + offset;
		/* At this point, val is valid again.  */

		/* Open up space where we are going to insert.  */
		if ((gsize)pos < string->len)
			memmove(string->str + pos + len, string->str + pos, string->len - pos);

		/* Move the source part before the gap, if any.  */
		if (offset < (gsize)pos)
		{
			precount = min((gsize)len, (gsize)pos - offset);
			memcpy(string->str + pos, val, precount);
		}

		/* Move the source part after the gap, if any.  */
		if ((gsize)len > precount)
			memcpy(string->str + pos + precount, val + /* Already moved: */ precount + /* Space opened up: */ len,
				   len - precount);
	} else
	{
		g_string_maybe_expand(string, len);

		/* If we aren't appending at the end, move a hunk
		 * of the old string to the end, opening up space
		 */
		if ((gsize)pos < string->len)
			memmove(string->str + pos + len, string->str + pos, string->len - pos);

		/* insert the new string */
		if (len == 1)
			string->str[pos] = *val;
		else
			memcpy(string->str + pos, val, len);
	}

	string->len += len;

	string->str[string->len] = 0;

	return string;
}

/*** ---------------------------------------------------------------------- ***/

char *g_string_free(GString *string, gboolean free_segment)
{
	char *segment;

	if (string == NULL)
		return NULL;

	if (free_segment)
	{
		g_free(string->str);
		segment = NULL;
	} else
	{
		segment = string->str;
	}
	
	g_free(string);

	return segment;
}

/*** ---------------------------------------------------------------------- ***/

GString *g_string_append_len(GString *string, const char *val, gssize len)
{
	return g_string_insert_len(string, -1, val, len);
}

/*** ---------------------------------------------------------------------- ***/

GString *g_string_append(GString *string, const char *val)
{
	return g_string_insert_len(string, -1, val, -1);
}

/*** ---------------------------------------------------------------------- ***/

void g_string_append_vprintf(GString *string, const char *format, va_list args)
{
	char *buf;
	gssize len;

	if (string == NULL)
		return;
	if (format == NULL)
		return;

	buf = g_strdup_vprintf(format, args);
	if (buf == NULL)
		return;
	len = strlen(buf);
	
	if (len > 0)
	{
		g_string_maybe_expand(string, len);
		memcpy(string->str + string->len, buf, len + 1);
		string->len += len;
		g_free(buf);
	}
}

/*** ---------------------------------------------------------------------- ***/

void g_string_append_printf(GString *string, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	g_string_append_vprintf(string, format, args);
	va_end(args);
}

/*** ---------------------------------------------------------------------- ***/

GString *g_string_truncate(GString *string, gsize len)
{
	if (string == NULL)
		return NULL;

	string->len = min(len, string->len);
	string->str[string->len] = 0;

	return string;
}

/*** ---------------------------------------------------------------------- ***/

GString *g_string_set_size(GString *string, gsize len)
{
	if (string == NULL)
		return NULL;

	if (len >= string->allocated_len)
		g_string_maybe_expand(string, len - string->len);

	string->len = len;
	string->str[len] = 0;

	return string;
}

/*** ---------------------------------------------------------------------- ***/

char *g_build_filename(const char *first, ...)
{
	va_list args;
	size_t len;
	const char *str;
	char *ret, *ptr;
	
	if (first == NULL)
		return NULL;
	len = strlen(first) + 1;
	va_start(args, first);
	for (;;)
	{
		str = va_arg(args, const char *);
		if (str == NULL)
			break;
		while (G_IS_DIR_SEPARATOR(*str))
			str++;
		len += strlen(str) + 1;
	}
	va_end(args);
	ret = g_new(char, len);
	if (ret == NULL)
		return NULL;
	strcpy(ret, first);
	ptr = ret + strlen(ret);
	while (ptr > ret && G_IS_DIR_SEPARATOR(ptr[-1]))
		*--ptr = '\0';
	va_start(args, first);
	for (;;)
	{
		str = va_arg(args, const char *);
		if (str == NULL)
			break;
		while (G_IS_DIR_SEPARATOR(*str))
			str++;
		if (*str == '\0')
			continue;
		ptr = ret + strlen(ret);
		*ptr++ = G_DIR_SEPARATOR;
		strcpy(ptr, str);
		ptr += strlen(ptr);
		while (ptr > ret && G_IS_DIR_SEPARATOR(ptr[-1]))
			*--ptr = '\0';
	}
	va_end(args);
	convslash(ret);
	return ret;
}

#endif /* HAVE_GLIB */

/*** ---------------------------------------------------------------------- ***/

void convslash(char *str)
{
	char *p = str;
	if (p != NULL)
	{
		while (*p)
		{
			if (G_IS_DIR_SEPARATOR(*p))
				*p = G_DIR_SEPARATOR;
			p++;
		}
	}
}

/*** ---------------------------------------------------------------------- ***/

#define CONTINUATION_CHAR                           \
  if ((*(const unsigned char *)p & 0xc0) != 0x80) /* 10xxxxxx */ \
    { goto error; }                                    \
  val <<= 6;                                        \
  val |= (*(const unsigned char *)p) & 0x3f

#define UNICODE_VALID(Char)                   \
    ((Char) < 0x110000UL &&                     \
     (((Char) & 0xFFFFF800UL) != 0xD800UL) &&     \
     ((Char) < 0xFDD0UL || (Char) > 0xFDEFUL) &&  \
     ((Char) & 0xFFFEUL) != 0xFFFEUL)

const char *g_utf8_getchar(const char *p, unsigned int *ch)
{
	const char *last;

	if (*(const unsigned char *) p < 0x80)
	{
		*ch = *(const unsigned char *) p;
		return p + 1;
	}
	last = p;
	if ((*(const unsigned char *) p & 0xe0) == 0xc0)	/* 110xxxxx */
	{
		if ((*(const unsigned char *) p & 0x1e) == 0)
			goto error;
		*ch = (*(const unsigned char *) p & 0x1f) << 6;
		p++;
		if ((*(const unsigned char *) p & 0xc0) != 0x80)	/* 10xxxxxx */
			goto error;
		*ch |= (*(const unsigned char *) p) & 0x3f;
	} else
	{
		unsigned int val = 0;
		unsigned int min = 0;
		
		if ((*(const unsigned char *) p & 0xf0) == 0xe0)	/* 1110xxxx */
		{
			min = (1 << 11);
			val = *(const unsigned char *) p & 0x0f;
			goto TWO_REMAINING;
		} else if ((*(const unsigned char *) p & 0xf8) == 0xf0)	/* 11110xxx */
		{
			min = (1 << 16);
			val = *(const unsigned char *) p & 0x07;
		} else
		{
			goto error;
		}
		
		p++;
		CONTINUATION_CHAR;
	  TWO_REMAINING:
		p++;
		CONTINUATION_CHAR;
		p++;
		CONTINUATION_CHAR;

		if (val < min)
			goto error;

		if (!UNICODE_VALID(val))
			goto error;
		*ch = val;
	}

	return p + 1;

  error:
    *ch = 0xffff;
	return last + 1;
}

