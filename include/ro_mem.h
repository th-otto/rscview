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

#ifndef HAVE_GLIB
char *g_strdup(const char *);
char *g_strndup(const char *, size_t n);
#endif

char *g_strdup0(const char *str);
char *g_strvdup(const char *str);

EXTERN_C_END

#endif /* __RO_MEM_H__ */
