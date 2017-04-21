#define HTML_DOCTYPE_OLD              0           /* HTML 3.2 */
#define HTML_DOCTYPE_STRICT           1           /* HTML 4.01 */
#define HTML_DOCTYPE_TRANS            2           /* HTML 4.01 Transitional */
#define HTML_DOCTYPE_FRAME            3           /* HTML 4.01 Frameset */
#define HTML_DOCTYPE_XSTRICT          4           /* XHTML 1.0 Strict */
#define HTML_DOCTYPE_XTRANS           5           /* XHTML 1.0 Transitional */
#define HTML_DOCTYPE_XFRAME           6           /* XHTML 1.0 Frameset */
#define HTML_DOCTYPE_HTML5            7           /* HTML 5 */
 
#define QUOTE_CONVSLASH  0x0001
#define QUOTE_SPACE      0x0002
#define QUOTE_URI        0x0004
#define QUOTE_JS         0x0008
#define QUOTE_ALLOWUTF8  0x0010
#define QUOTE_LABEL      0x0020
#define QUOTE_UNICODE    0x0040
#define QUOTE_NOLTR      0x0080

typedef struct _rsc_opts {
	FILE *errorfile;
	gboolean cgi_cached;
	gboolean use_xhtml;
	char *lang;
	const char *charset;
} rsc_opts;

char *html_quote_name(const char *name, unsigned int flags);
gboolean uri_has_scheme(const char *uri);
void html_out_response_header(FILE *out, unsigned long len, gboolean xml);
