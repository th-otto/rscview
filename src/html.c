#include "config.h"
#include <gem.h>
#include <time.h>
#include <assert.h>
#include "debug.h"
#include "html.h"

/* ------------------------------------------------------------------------- */

gboolean uri_has_scheme(const char *uri)
{
	gboolean colon = FALSE;
	
	if (uri == NULL)
		return FALSE;
	while (*uri)
	{
		if (*uri == ':')
			colon = TRUE;
		else if (*uri == '/')
			return colon;
		uri++;
	}
	return colon;
}

/* ------------------------------------------------------------------------- */

void html_out_response_header(FILE *out, unsigned long len, gboolean xml)
{
	fprintf(out, "Content-Type: %s;charset=UTF-8\015\012", xml ? "application/xhtml+xml" : "text/html");
	fprintf(out, "Content-Length: %lu\015\012", len);
	fprintf(out, "\015\012");
}

/* ------------------------------------------------------------------------- */

void html_out_header(RSCFILE *file, rsc_opts *opts, GString *out, const char *title, _WORD treeindex, gboolean for_error)
{
	(void) file;
	(void) opts;
	(void) out;
	(void) title;
	(void) treeindex;
	(void) for_error;
}

/* ------------------------------------------------------------------------- */

void html_out_trailer(RSCFILE *file, rsc_opts *opts, GString *out, _WORD treeindex, gboolean for_error)
{
	(void) file;
	(void) opts;
	(void) out;
	(void) treeindex;
	(void) for_error;
}
