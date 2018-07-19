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

/* UTF-8 of \u00a9 */
#define S_COPYRIGHT_SIGN "\xC2\xA9"

#define RSCVIEW_COPYRIGHT "Copyright " S_COPYRIGHT_SIGN " 1991-" RELEASE_YEAR " by Thorsten Otto"

#define RSC_EXT_HTML ".html"
#define RSC_EXT_PNG  ".png"

typedef struct _rsc_opts {
	FILE *errorfile;
	gboolean for_cgi;
	gboolean cgi_cached;
	gboolean use_xhtml;
	gboolean to_xml;
	gboolean hidemenu;
	char *lang;
	const char *charset;
	char *output_dir;			/* local filename */
	char *ref_output_dir;		/* filename to use in html references */
	char *po_dir;
	int verbose;
	FILE *outfile;
	_BOOL gen_imagemap;
	gboolean use_timestamps;
} rsc_opts;

extern const char *cgi_scriptname;
extern char *html_referer_url;

void html_init(rsc_opts *opts);

void write_strout(GString *s, FILE *outfp);
char *html_quote_name(const char *name, unsigned int flags);
gboolean uri_has_scheme(const char *uri);
void html_out_response_header(FILE *out, unsigned long len, gboolean xml);
void html_out_header(RSCFILE *file, rsc_opts *opts, GString *out, const char *title, _WORD treeindex, gboolean for_error);
void html_out_trailer(RSCFILE *file, rsc_opts *opts, GString *out, _WORD treeindex, gboolean for_error);
