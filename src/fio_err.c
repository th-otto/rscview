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

void err_fcreate(const char *filename)
{
	error(_("can't create %s: %s"), filename, strerror(errno));
}

/*** ---------------------------------------------------------------------- ***/

void err_fopen(const char *filename)
{
	error(_("can't open %s: %s"), filename, strerror(errno));
}

/*** ---------------------------------------------------------------------- ***/

void err_fread(const char *filename)
{
	error(_("reading %s"), filename);
}

/*** ---------------------------------------------------------------------- ***/

void err_fwrite(const char *filename)
{
	error(_("writing %s"), filename);
}

/*** ---------------------------------------------------------------------- ***/

void err_rename(const char *oldname, const char *newname)
{
	error(_("can't rename %s to %s"), oldname, newname);
}

/*** ---------------------------------------------------------------------- ***/

void err_nota_rsc(const char *filename)
{
	error(_("not a resource file: %s"), filename);
}

/*** ---------------------------------------------------------------------- ***/

void warn_damaged(const char *filename, const char *where)
{
	warn("problems in %s while scanning %s", filename, where);
}

/*** ---------------------------------------------------------------------- ***/

void warn_cicons(void)
{
	warn("I couldn't find any color icons although\nthe flag is set the header");
}

/*** ---------------------------------------------------------------------- ***/

void warn_crc_mismatch(const char *filename)
{
	warn("%s: CRC does not match resource file", filename);
}

/*** ---------------------------------------------------------------------- ***/

void warn_crc_string_mismatch(const char *filename)
{
	warn("%s: embedded string CRC does not match resource file", filename);
}
