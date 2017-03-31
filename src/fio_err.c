#include "config.h"
#include <portab.h>
#include <gem.h>
#include <stdarg.h>
#include <errno.h>
#include "fileio.h"
#include "nls.h"
#include "debug.h"


/*** ---------------------------------------------------------------------- ***/

static void error(const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	fprintf(stderr, "%s: error: ", program_name);
	vfprintf(stderr, msg, args);
	va_end(args);
	fprintf(stderr, "\n");
}

/*** ---------------------------------------------------------------------- ***/

static void warn(const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	fprintf(stderr, "%s: warning: ", program_name);
	vfprintf(stderr, msg, args);
	va_end(args);
	fprintf(stderr, "\n");
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_fcreate(const char *filename)
{
	error(_("can't create %s: %s"), filename, strerror(errno));
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_fopen(const char *filename)
{
	error(_("can't open %s: %s"), filename, strerror(errno));
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_fread(const char *filename)
{
	error(_("reading %s"), filename);
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_fwrite(const char *filename)
{
	error(_("writing %s"), filename);
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_rename(const char *oldname, const char *newname)
{
	error(_("can't rename %s to %s"), oldname, newname);
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_nota_rsc(const char *filename)
{
	error(_("not a resource file: %s"), filename);
}

/*** ---------------------------------------------------------------------- ***/

_VOID warn_damaged(CONST _UBYTE *filename, CONST _UBYTE *where)
{
	warn("problems in %s while scanning %s", filename, where);
}

/*** ---------------------------------------------------------------------- ***/

_VOID warn_cicons(_VOID)
{
	warn("I couldn't find any color icons although\nthe flag is set the header");
}

/*** ---------------------------------------------------------------------- ***/

_VOID warn_crc_mismatch(CONST _UBYTE *filename)
{
	warn("%s: CRC does not match resource file", filename);
}

/*** ---------------------------------------------------------------------- ***/

_VOID warn_crc_string_mismatch(CONST _UBYTE *filename)
{
	warn("%s: embedded string CRC does not match resource file", filename);
}
