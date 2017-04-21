/*****************************************************************************
 * RO_MEM.H
 *****************************************************************************/

#ifndef __RO_MEM_H__
#define __RO_MEM_H__

#ifndef __PORTAB_H__
#  include <portab.h>
#endif

#ifdef HAVE_GTK
#  define HAVE_GLIB
#  include <glib.h>
#endif

#ifdef HAVE_GLIB
#include <glib.h>
#else

#include <stdint.h>
#include <stdarg.h>

#define GLIB_CHECK_VERSION(a, b, c) 0

#if __GNUC_PREREQ(3, 4)
#define G_GNUC_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define G_GNUC_WARN_UNUSED_RESULT
#endif

#if defined(__WIN32__)
#ifndef G_OS_WIN32
#define G_OS_WIN32 1
#endif
#ifndef G_PLATFORM_WIN32
#define G_PLATFORM_WIN32 1
#endif
#elif defined(__TOS__)
#define G_OS_TOS 1
#else
#ifndef G_OS_UNIX
#define G_OS_UNIX 1
#endif
#endif

#undef G_IS_DIR_SEPARATOR
#undef G_SEARCHPATH_SEPARATOR
#undef G_DIR_SEPARATOR
#undef G_DOSTYLE_PATHNAMES

#if defined(G_OS_TOS)
#  define G_SEARCHPATH_SEPARATOR ';'
#  define G_DIR_SEPARATOR '\\'
#  define G_DOSTYLE_PATHNAMES 1
#endif
#if defined(G_OS_WIN32)
#  define G_SEARCHPATH_SEPARATOR ';'
#  define G_DOSTYLE_PATHNAMES 1
#endif
#ifndef G_SEARCHPATH_SEPARATOR
#  define G_SEARCHPATH_SEPARATOR ':'
#endif
#ifndef G_DIR_SEPARATOR
#  define G_DIR_SEPARATOR '/'
#endif

#define G_IS_DIR_SEPARATOR(c) ((c) == '/' || (c) == '\\')

#if defined(__TOS__) || defined(__WIN32__)
#  define G_DOSTYLE_PATHNAMES 1
#endif

/* Define G_VA_COPY() to do the right thing for copying va_list variables.
 * config.h may have already defined G_VA_COPY as va_copy or __va_copy.
 */
#if !defined (G_VA_COPY)
#  if defined (__GNUC__)
#    define G_VA_COPY(ap1, ap2)	va_copy(ap1, ap2)
#  elif defined (G_VA_COPY_AS_ARRAY)
#    define G_VA_COPY(ap1, ap2)	  memmove ((ap1), (ap2), sizeof (va_list))
#  else /* va_list is a pointer */
#    define G_VA_COPY(ap1, ap2)	  ((ap1) = (ap2))
#  endif /* va_list is a pointer */
#endif /* !G_VA_COPY */

#define g_malloc(n) malloc(n)
#define g_calloc(n, s) calloc((size_t)(n), (size_t)(s))
#define g_malloc0(n) calloc((size_t)(n), 1)
#define g_realloc(ptr, s) realloc(ptr, s)
#define g_free(t) free(t)

#define g_new(t, n) ((t *)g_malloc((size_t)(n) * sizeof(t)))
#define g_new0(t, n) ((t *)g_malloc0((size_t)(n) * sizeof(t)))
#define g_renew(t, p, n) ((t *)g_realloc(p, (size_t)(n) * sizeof(t)))

#endif /* HAVE_GLIB */


EXTERN_C_BEG

typedef void (*ERR_MEM_FUNC)(size_t size, void *parm);
ERR_MEM_FUNC set_err_memory(ERR_MEM_FUNC p_err, void *parm);
void err_memory(size_t size);

#define STR0TERM  ((size_t)-1)
#define unreachable() abort()

#ifndef HAVE_GLIB

typedef void *gpointer;
typedef const void *gconstpointer;

#undef g_ascii_isspace
#define g_ascii_isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n' || (c) == '\f' || (c) == '\v')
#undef g_ascii_isdigit
#define g_ascii_isdigit(c) ((c) >= '0' && (c) <= '9')
#undef g_ascii_isxdigit
#define g_ascii_isxdigit(c) (g_ascii_isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#undef g_ascii_isalpha
#define g_ascii_isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#undef g_ascii_isalnum
#define g_ascii_isalnum(c) ((c) == '_' || g_ascii_isalpha((c)) || g_ascii_isdigit((c)))
#undef g_ascii_toupper
#define g_ascii_toupper(c) toupper(c)

char *g_strdup_printf(const char *format, ...) __attribute__((format(printf, 1, 2)));
char *g_strdup_vprintf(const char *format, va_list args);

char *g_strdup(const char *);
char *g_strndup(const char *, size_t n);
int g_ascii_strcasecmp(const char *s1, const char *s2);
int g_ascii_strncasecmp(const char *s1, const char *s2, size_t n);
char *g_strconcat(const char *, ...);

char *g_strchomp(char *str);
char *g_strchug(char *str);
void g_strfreev(char **str_array);
unsigned int g_strv_length(char **str_array);
char **g_strsplit(const char *string, const char *delimiter, int max_tokens);
char *g_strjoinv(const char *separator, char **str_array);
char *g_stpcpy(char *dest, const char *src);

char *g_build_filename(const char *, ...);

int g_ascii_xdigit_value(char c);

typedef struct _gslist GSList;
struct _gslist {
	GSList *next;
	void *data;
};

GSList *g_slist_remove(GSList *list, gconstpointer data) G_GNUC_WARN_UNUSED_RESULT;
GSList *g_slist_prepend(GSList  *list, gpointer  data) G_GNUC_WARN_UNUSED_RESULT;
GSList *g_slist_append(GSList *list, gpointer data);
#define g_slist_free_1(l) g_free(l)
void g_slist_free_full(GSList *list, void (*freefunc)(void *));
void g_slist_free(GSList *list);
GSList *g_slist_delete_link(GSList *list, GSList *link_);
GSList *g_slist_remove_link(GSList *list, GSList *link_);

struct _GString
{
  char  *str;
  gsize len;
  gsize allocated_len;
};
typedef struct _GString GString;
GString *g_string_insert_c(GString *string, gssize pos, char c);
GString *g_string_append_c(GString *string, char c);
GString *g_string_sized_new(gsize dfl_size);
GString *g_string_new(const char *init);
GString *g_string_insert_len(GString *string, gssize pos, const char *val, gssize len);
GString *g_string_append_len(GString * string, const char *val, gssize len);
GString *g_string_append(GString *string, const char *val);
char *g_string_free(GString *string, gboolean free_segment);
void g_string_append_vprintf(GString *string, const char *format, va_list args) __attribute__((format(printf, 2, 0)));
void g_string_append_printf(GString *string, const char *format, ...) __attribute__((format(printf, 2, 3)));
GString *g_string_truncate(GString *string, gsize len);
GString *g_string_set_size(GString *string, gsize len);

#endif

char *g_strdup0(const char *str);
char *g_strvdup(const char *str);

void convslash(char *str);

#define empty(str) ((str) == NULL || *(str) == '\0')
#define fixnull(str) ((str) != NULL ? (str) : "")
#define printnull(str) ((str) != NULL ? (const char *)(str) : "(nil)")

EXTERN_C_END

#endif /* __RO_MEM_H__ */
