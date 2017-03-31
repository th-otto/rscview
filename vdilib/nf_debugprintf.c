#include "config.h"
#include <portab.h>
#include <stdarg.h>
#include <errno.h>
#include "nls.h"
#include "debug.h"

/*** ---------------------------------------------------------------------- ***/

#if !defined(OS_ATARI) && defined(SPEC_DEBUG)
void nf_debugprintf(const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

#else

extern int _I_dont_care_that_ISO_C_forbids_an_empty_source_file_;

#endif
