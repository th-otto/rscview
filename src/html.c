#include "config.h"
#include <ro_mem.h>
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
