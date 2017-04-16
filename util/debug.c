#include "config.h"
#include <portab.h>
#include <stdarg.h>
#include <errno.h>
#include "nls.h"
#include "debug.h"

static void (*error_handler)(void *data, const char *format, va_list args);
static void *error_data;

/*** ---------------------------------------------------------------------- ***/

void erroutv(const char *format, va_list args)
{
	if (error_handler)
	{
		error_handler(error_data, format, args);
	} else
	{
#if defined(OS_ATARI)
		nf_debugvprintf(format, args);
#else
		vfprintf(stderr, format, args);
#endif
	}
}

void errout(const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	erroutv(format, args);
	va_end(args);
}

/*** ---------------------------------------------------------------------- ***/

void set_errout_handler(void (*handler)(void *data, const char *format, va_list args), void *data)
{
	error_handler = handler;
	error_data = data;
}
