#include "config.h"
#include <portab.h>
#include <gem.h>
#include <stdarg.h>
#include "fileio.h"

/*** ---------------------------------------------------------------------- ***/

static _VOID error(const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	fprintf(stderr, "%s: error: ", program_name);
	vfprintf(stderr, msg, args);
	va_end(args);
	fprintf(stderr, "\n");
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_fcreate(const _UBYTE *filename)
{
	error("can't create %s", filename);
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_fopen(const _UBYTE *filename)
{
	error("can't open %s", filename);
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_fread(const _UBYTE *filename)
{
	error("reading %s", filename);
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_fwrite(const _UBYTE *filename)
{
	error("writing %s", filename);
}

/*** ---------------------------------------------------------------------- ***/

_VOID err_rename(const _UBYTE *oldname, const _UBYTE *newname)
{
	error("can't rename %s to %s", oldname, newname);
}
