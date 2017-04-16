/*****************************************************************************
 * DEBUG.H
 *****************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdarg.h>

#ifndef __PORTAB_H__
#  include <portab.h>
#endif

EXTERN_C_BEG

void set_errout_handler(void (*handler)(void *data, const char *format, va_list args), void *data);

void errout(const char *format, ...) __attribute__((format(printf, 1, 2)));
void erroutv(const char *format, va_list args) __attribute__((format(printf, 1, 0)));

#define KINFO(args) errout args

#ifdef ENABLE_KDEBUG
#define KDEBUG(args) KINFO(args)
#else
#define KDEBUG(args)
#endif

EXTERN_C_END

#endif /* __DEBUG_H__ */
