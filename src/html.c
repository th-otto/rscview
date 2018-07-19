#include "config.h"
#include <gem.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include "fileio.h"
#include "debug.h"
#include "html.h"
#include "htmljs.h"

const char *cgi_scriptname = "rscview.cgi";
char *html_referer_url;

static int html_doctype = HTML_DOCTYPE_XSTRICT;
static const char *html_closer = " />";
static const char *html_name_attr = "id";

static char const rscview_css_name[] = "_rscview.css";
static char const rscview_js_name[] = "_rscview.js";
static char const html_nav_load_href[] = "index.php";

static gboolean const force_overwrite = TRUE;
static gboolean css_written = FALSE;
static gboolean js_written = FALSE;

#define COLOR_BG         "#ffffff"
#define COLOR_TEXT       "#000000"
#define COLOR_LINK       "#0000ff"
#define COLOR_POPUP      "#00ff00"
#define COLOR_ERROR      "#ff0000"
#define COLOR_GHOSTED    "#808080"

#define LINE_WIDTH 80
#define TEXT_XOFFSET 8

/*
 * style names used
 */
static char const html_toolbar_style[] = "rscview_nav_toolbar";
static char const html_nav_img_style[] = "rscview_nav_img";
static char const html_node_style[] = "rscview_node";
static char const html_pnode_style[] = "rscview_pnode";
static char const html_dropdown_pnode_style[] = "rscview_dropdown_pnode";
static char const html_dropdown_info_style[] = "rscview_dropdown_info";
static char const html_error_note_style[] = "rscview_error_note";
static char const html_dropdown_style[] = "rscview_dropdown";

static char const html_nav_back_png[] = "images/iback.png";
static char const html_nav_prev_png[] = "images/iprev.png";
static char const html_nav_next_png[] = "images/inext.png";
static char const html_nav_load_png[] = "images/iload.png";
#if 0 /* not used yet */
static char const html_image_style[] = "rscview_image";
static char const html_nav_toc_png[] = "images/itoc.png";
static char const html_nav_xref_png[] = "images/ixref.png";
static char const html_nav_index_png[] = "images/iindex.png";
static char const html_nav_help_png[] = "images/ihelp.png";
#endif
static char const html_nav_info_png[] = "images/iinfo.png";
static char const html_rsc_info_id[] = "rsc_info";
static char const html_nav_dimensions[] = " width=\"32\" height=\"21\"";


/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

void write_strout(GString *s, FILE *outfp)
{
	fwrite(s->str, 1, s->len, outfp);
}

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

void html_init(rsc_opts *opts)
{
	(void) opts;
	html_closer = html_doctype >= HTML_DOCTYPE_XSTRICT ? " />" : ">";
	html_name_attr = html_doctype >= HTML_DOCTYPE_XSTRICT ? "id" : "name";
}

/* ------------------------------------------------------------------------- */

void html_out_response_header(FILE *out, unsigned long len, gboolean xml)
{
	fprintf(out, "Content-Type: %s;charset=UTF-8\015\012", xml ? "application/xhtml+xml" : "text/html");
	fprintf(out, "Content-Length: %lu\015\012", len);
	fprintf(out, "\015\012");
}

/* ------------------------------------------------------------------------- */

/*
 * Disallow certain characters that might clash with
 * the filesystem or uri escape sequences, and also any non-ascii characters.
 * For simplicity, this is done in-place.
 */
static void html_convert_filename(char *filename)
{
	char *p = filename;
	unsigned char c;
	
	while ((c = *p) != '\0')
	{
		if (c == ' ' ||
			c == ':' ||
			c == '%' ||
			c == '?' ||
			c == '*' ||
			c == '/' ||
			c == '&' ||
			c == '<' ||
			c == '>' ||
			c == '"' ||
			c == '\'' ||
			c == '\\' ||
			c >= 0x7f ||
			c < 0x20)
		{
			c = '_';
		} else if (c >= 'A' && c <= 'Z')
		{
			/* make it lowercase. should eventually be configurable */
			c = c - 'A' + 'a';
		}
		*p++ = c;
	}
}

/* ------------------------------------------------------------------------- */

static char *html_cgi_params(rsc_opts *opts)
{
	return g_strconcat(
		opts->hidemenu ? "&amp;hidemenu=1" : "",
		opts->cgi_cached ? "&amp;cached=1" : "",
		NULL);
}
	
/* ------------------------------------------------------------------------- */

static char *html_filename_for_tree(RSCFILE *file, rsc_opts *opts, _WORD treeindex, gboolean quote)
{
	char *filename;
	RSCTREE *tree;
	
	tree = rsc_tree_index(file, treeindex, RT_ANY);
	{
		char *name = g_ascii_strdown(tree->rt_name, STR0TERM);
		if (strcmp(name, "index") == 0)
		{
			g_free(name);
			name = g_strdup_printf("index_%d", treeindex);
		}
		filename = g_strconcat(name, RSC_EXT_HTML, NULL);
		g_free(name);
	}
	
	html_convert_filename(filename);
	
	if (quote)
	{
		char *p;
		
		if (opts->for_cgi)
		{
			char *params = html_cgi_params(opts);
			char *tmp = html_quote_name(html_referer_url, QUOTE_URI);
			p = g_strdup_printf("%s?url=%s%s&amp;index=%u", cgi_scriptname, tmp, params, treeindex);
			g_free(tmp);
			g_free(params);
		} else
		{
			p = html_quote_name(filename, QUOTE_UNICODE|QUOTE_NOLTR);
		}
		g_free(filename);
		filename = p;
	}
	return filename;	
}

/* ------------------------------------------------------------------------- */

static void html_out_entities(GString *out)
{
	g_string_append(out, " [\n");
	g_string_append(out, "<!ENTITY uparrow \"&#8679;\">          <!-- 0x01 U+21E7 -->\n");
	g_string_append(out, "<!ENTITY downarrow \"&#8681;\">        <!-- 0x02 U+21E9 -->\n");
	g_string_append(out, "<!ENTITY rightarrow \"&#8680;\">       <!-- 0x03 U+21E8 -->\n");
	g_string_append(out, "<!ENTITY leftarrow \"&#8678;\">        <!-- 0x04 U+21E6 -->\n");
	g_string_append(out, "<!ENTITY ballotbox \"&#9744;\">        <!-- 0x05 U+2610 -->\n");
	g_string_append(out, "<!ENTITY ballotboxcheck \"&#9745;\">   <!-- 0x06 U+2611 -->\n");
	g_string_append(out, "<!ENTITY ballotboxx \"&#9746;\">       <!-- 0x07 U+2612 -->\n");
	g_string_append(out, "<!ENTITY checkmark \"&#10003;\">       <!-- 0x08 U+2713 -->\n");
	g_string_append(out, "<!ENTITY watch \"&#8986;\">            <!-- 0x09 U+231A -->\n");
	g_string_append(out, "<!ENTITY bell \"&#9086;\">             <!-- 0x0a U+237E -->\n");
	g_string_append(out, "<!ENTITY eightnote \"&#9834;\">        <!-- 0x0b U+266a -->\n");
	g_string_append(out, "<!ENTITY mountain \"&#9968;\">         <!-- 0x0e U+26f0 -->\n");
	g_string_append(out, "<!ENTITY umbrella \"&#9969;\">         <!-- 0x0f U+26f1 -->\n");
	g_string_append(out, "<!ENTITY circledzero \"&#9450;\">      <!-- 0x10 U+24ea -->\n");
	g_string_append(out, "<!ENTITY circledone \"&#9312;\">       <!-- 0x11 U+2460 -->\n");
	g_string_append(out, "<!ENTITY circledtwo \"&#9313;\">       <!-- 0x12 U+2461 -->\n");
	g_string_append(out, "<!ENTITY circledthree \"&#9314;\">     <!-- 0x13 U+2462 -->\n");
	g_string_append(out, "<!ENTITY circledfour \"&#9315;\">      <!-- 0x14 U+2463 -->\n");
	g_string_append(out, "<!ENTITY circledfive \"&#9316;\">      <!-- 0x15 U+2464 -->\n");
	g_string_append(out, "<!ENTITY circledsix \"&#9317;\">       <!-- 0x16 U+2465 -->\n");
	g_string_append(out, "<!ENTITY circledseven \"&#9318;\">     <!-- 0x17 U+2466 -->\n");
	g_string_append(out, "<!ENTITY circledeight \"&#9319;\">     <!-- 0x18 U+2467 -->\n");
	g_string_append(out, "<!ENTITY circlednine \"&#9320;\">      <!-- 0x19 U+2468 -->\n");
	g_string_append(out, "<!ENTITY capitalschwa \"&#399;\">      <!-- 0x1a U+018f -->\n");
	g_string_append(out, "<!ENTITY fountain \"&#9970;\">         <!-- 0x1c U+26f2 -->\n");
	g_string_append(out, "<!ENTITY flaginhole \"&#9971;\">       <!-- 0x1d U+26f3 -->\n");
	g_string_append(out, "<!ENTITY ferry \"&#9972;\">            <!-- 0x1e U+26f4 -->\n");
	g_string_append(out, "<!ENTITY sailboat \"&#9973;\">         <!-- 0x1f U+26f5 -->\n");
	g_string_append(out, "<!ENTITY increment \"&#8710;\">        <!-- 0x7f U+2206 -->\n");

	g_string_append(out, "<!ENTITY nul \"&#9216;\">              <!-- 0x00 U+2400 -->\n");
	g_string_append(out, "<!ENTITY soh \"&#9217;\">              <!-- 0x01 U+2401 -->\n");
	g_string_append(out, "<!ENTITY stx \"&#9218;\">              <!-- 0x02 U+2402 -->\n");
	g_string_append(out, "<!ENTITY etx \"&#9219;\">              <!-- 0x03 U+2403 -->\n");
	g_string_append(out, "<!ENTITY eot \"&#9220;\">              <!-- 0x04 U+2404 -->\n");
	g_string_append(out, "<!ENTITY enq \"&#9221;\">              <!-- 0x05 U+2405 -->\n");
	g_string_append(out, "<!ENTITY ack \"&#9222;\">              <!-- 0x06 U+2406 -->\n");
	g_string_append(out, "<!ENTITY bel \"&#9223;\">              <!-- 0x07 U+2407 -->\n");
	g_string_append(out, "<!ENTITY bs  \"&#9224;\">              <!-- 0x08 U+2408 -->\n");
	g_string_append(out, "<!ENTITY ht  \"&#9225;\">              <!-- 0x09 U+2409 -->\n");
	g_string_append(out, "<!ENTITY lf  \"&#9226;\">              <!-- 0x0a U+240a -->\n");
	g_string_append(out, "<!ENTITY vt  \"&#9227;\">              <!-- 0x0b U+240b -->\n");
	g_string_append(out, "<!ENTITY ff  \"&#9228;\">              <!-- 0x0c U+240c -->\n");
	g_string_append(out, "<!ENTITY cr  \"&#9229;\">              <!-- 0x0d U+240d -->\n");
	g_string_append(out, "<!ENTITY so  \"&#9230;\">              <!-- 0x0e U+240e -->\n");
	g_string_append(out, "<!ENTITY si  \"&#9231;\">              <!-- 0x0f U+240f -->\n");
	g_string_append(out, "<!ENTITY dle \"&#9232;\">              <!-- 0x10 U+2410 -->\n");
	g_string_append(out, "<!ENTITY dc1 \"&#9233;\">              <!-- 0x11 U+2411 -->\n");
	g_string_append(out, "<!ENTITY dc2 \"&#9234;\">              <!-- 0x12 U+2412 -->\n");
	g_string_append(out, "<!ENTITY dc3 \"&#9235;\">              <!-- 0x13 U+2413 -->\n");
	g_string_append(out, "<!ENTITY dc4 \"&#9236;\">              <!-- 0x14 U+2414 -->\n");
	g_string_append(out, "<!ENTITY nak \"&#9237;\">              <!-- 0x15 U+2415 -->\n");
	g_string_append(out, "<!ENTITY syn \"&#9238;\">              <!-- 0x16 U+2416 -->\n");
	g_string_append(out, "<!ENTITY etb \"&#9239;\">              <!-- 0x17 U+2417 -->\n");
	g_string_append(out, "<!ENTITY can \"&#9240;\">              <!-- 0x18 U+2418 -->\n");
	g_string_append(out, "<!ENTITY em  \"&#9241;\">              <!-- 0x19 U+2419 -->\n");
	g_string_append(out, "<!ENTITY sub \"&#9242;\">              <!-- 0x1a U+241a -->\n");
	g_string_append(out, "<!ENTITY esc \"&#9243;\">              <!-- 0x1b U+241b -->\n");
	g_string_append(out, "<!ENTITY fs  \"&#9244;\">              <!-- 0x1c U+241c -->\n");
	g_string_append(out, "<!ENTITY gs  \"&#9245;\">              <!-- 0x1d U+241d -->\n");
	g_string_append(out, "<!ENTITY rs  \"&#9246;\">              <!-- 0x1e U+241e -->\n");
	g_string_append(out, "<!ENTITY us  \"&#9247;\">              <!-- 0x1f U+241f -->\n");
	g_string_append(out, "<!ENTITY del \"&#9249;\">              <!-- 0x7f U+2421 -->\n");

	g_string_append(out, "<!ENTITY nbsp \"&#32;\">\n");
	g_string_append(out, "]");
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

char *html_quote_name(const char *name, unsigned int flags)
{
	char *str, *ret;
	size_t len;
	static char const hex[] = "0123456789ABCDEF";
	
	if (name == NULL)
		return NULL;
	len = strlen(name);
	str = ret = g_new(char, len * 20 + 1);
	if (str != NULL)
	{
		if (*name != '\0' && (flags & QUOTE_LABEL) && *name != '_' && !g_ascii_isalpha(*name))
			*str++ = '_';
		while (*name)
		{
			unsigned char c = *name++;
#define STR(s) strcpy(str, s), str += sizeof(s) - 1
			switch (c)
			{
			case '\\':
				if (flags & QUOTE_URI)
				{
					STR("%2F");
				} else if (flags & QUOTE_CONVSLASH)
				{
					*str++ = '/';
				} else
				{
					*str++ = '\\';
				}
				break;
			case ' ':
				if (flags & QUOTE_URI)
				{
					STR("%20");
				} else if (flags & QUOTE_SPACE)
				{
					STR("&nbsp;");
				} else if (flags & QUOTE_LABEL)
				{
					*str++ = '_';
				} else
				{
					*str++ = ' ';
				}
				break;
			case '"':
				if (flags & QUOTE_JS)
				{
					STR("\\&quot;");
				} else if (flags & QUOTE_URI)
				{
					STR("%22");
				} else
				{
					STR("&quot;");
				}
				break;
			case '&':
				if (flags & QUOTE_URI)
				{
					STR("%26");
				} else
				{
					STR("&amp;");
				}
				break;
			case '\'':
				if (flags & QUOTE_URI)
				{
					STR("%27");
				} else
				{
					STR("&apos;");
				}
				break;
			case '<':
				if (flags & QUOTE_URI)
				{
					STR("%3C");
				} else
				{
					STR("&lt;");
				}
				break;
			case '>':
				if (flags & QUOTE_URI)
				{
					STR("%3E");
				} else
				{
					STR("&gt;");
				}
				break;
			case '-':
			case '.':
			case '_':
			case '~':
				*str++ = c;
				break;
			case 0x01:
				if (flags & QUOTE_URI)
				{
					STR("%01");
				} else
				{
					STR("&soh;");
				}
				break;
			case 0x02:
				if (flags & QUOTE_URI)
				{
					STR("%02");
				} else
				{
					STR("&stx;");
				}
				break;
			case 0x03:
				if (flags & QUOTE_URI)
				{
					STR("%03");
				} else
				{
					STR("&etx;");
				}
				break;
			case 0x04:
				if (flags & QUOTE_URI)
				{
					STR("%04");
				} else
				{
					STR("&eot;");
				}
				break;
			case 0x05:
				if (flags & QUOTE_URI)
				{
					STR("%05");
				} else
				{
					STR("&enq;");
				}
				break;
			case 0x06:
				if (flags & QUOTE_URI)
				{
					STR("%06");
				} else
				{
					STR("&ack;");
				}
				break;
			case 0x07:
				if (flags & QUOTE_URI)
				{
					STR("%07");
				} else
				{
					STR("&bel;");
				}
				break;
			case 0x08:
				if (flags & QUOTE_URI)
				{
					STR("%08");
				} else
				{
					STR("&bs;");
				}
				break;
			case 0x09:
				if (flags & QUOTE_URI)
				{
					STR("%09");
				} else
				{
					STR("&ht;");
				}
				break;
			case 0x0a:
				if (flags & QUOTE_URI)
				{
					STR("%0A");
				} else
				{
					STR("&lf;");
				}
				break;
			case 0x0b:
				if (flags & QUOTE_URI)
				{
					STR("%0B");
				} else
				{
					STR("&vt;");
				}
				break;
			case 0x0c:
				if (flags & QUOTE_URI)
				{
					STR("%0C");
				} else
				{
					STR("&ff;");
				}
				break;
			case 0x0d:
				if (flags & QUOTE_URI)
				{
					STR("%0D");
				} else
				{
					STR("&cr;");
				}
				break;
			case 0x0e:
				if (flags & QUOTE_URI)
				{
					STR("%0E");
				} else
				{
					STR("&so;");
				}
				break;
			case 0x0f:
				if (flags & QUOTE_URI)
				{
					STR("%0F");
				} else
				{
					STR("&si;");
				}
				break;
			case 0x10:
				if (flags & QUOTE_URI)
				{
					STR("%10");
				} else
				{
					STR("&dle;");
				}
				break;
			case 0x11:
				if (flags & QUOTE_URI)
				{
					STR("%11");
				} else
				{
					STR("&dc1;");
				}
				break;
			case 0x12:
				if (flags & QUOTE_URI)
				{
					STR("%12");
				} else
				{
					STR("&dc2;");
				}
				break;
			case 0x13:
				if (flags & QUOTE_URI)
				{
					STR("%13");
				} else
				{
					STR("&dc3;");
				}
				break;
			case 0x14:
				if (flags & QUOTE_URI)
				{
					STR("%14");
				} else
				{
					STR("&dc4;");
				}
				break;
			case 0x15:
				if (flags & QUOTE_URI)
				{
					STR("%15");
				} else
				{
					STR("&nak;");
				}
				break;
			case 0x16:
				if (flags & QUOTE_URI)
				{
					STR("%16");
				} else
				{
					STR("&syn;");
				}
				break;
			case 0x17:
				if (flags & QUOTE_URI)
				{
					STR("%17");
				} else
				{
					STR("&etb;");
				}
				break;
			case 0x18:
				if (flags & QUOTE_URI)
				{
					STR("%18");
				} else
				{
					STR("&can;");
				}
				break;
			case 0x19:
				if (flags & QUOTE_URI)
				{
					STR("%19");
				} else
				{
					STR("&em;");
				}
				break;
			case 0x1a:
				if (flags & QUOTE_URI)
				{
					STR("%1A");
				} else
				{
					STR("&sub;");
				}
				break;
			case 0x1b:
				if (flags & QUOTE_URI)
				{
					STR("%1B");
				} else
				{
					STR("&esc;");
				}
				break;
			case 0x1c:
				if (flags & QUOTE_URI)
				{
					STR("%1C");
				} else
				{
					STR("&fs;");
				}
				break;
			case 0x1D:
				if (flags & QUOTE_URI)
				{
					STR("%1D");
				} else
				{
					STR("&gs;");
				}
				break;
			case 0x1E:
				if (flags & QUOTE_URI)
				{
					STR("%1E");
				} else
				{
					STR("&rs;");
				}
				break;
			case 0x1F:
				if (flags & QUOTE_URI)
				{
					STR("%1F");
				} else
				{
					STR("&us;");
				}
				break;
			default:
				if (c >= 0x80 && (flags & QUOTE_ALLOWUTF8))
				{
					*str++ = c;
				} else if (g_ascii_isalnum(c))
				{
					*str++ = c;
				} else if (flags & QUOTE_URI)
				{
					*str++ = '%';
					*str++ = hex[c >> 4];
					*str++ = hex[c & 0x0f];
				} else if (c >= 0x80 && (flags & QUOTE_UNICODE))
				{
					unsigned int wc;
					--name;
					name = g_utf8_getchar(name, &wc);
					/*
					 * neccessary for hebrew characters to prevent switching to rtl
					 */
					if (wc >= 0x590 && wc <= 0x5ff && !(flags & QUOTE_NOLTR))
						str += sprintf(str, "<span dir=\"ltr\">&#x%lx;</span>", (unsigned long) wc);
					else
						str += sprintf(str, "&#x%lx;", (unsigned long) wc);
				} else
				{
					*str++ = c;
				}
				break;
			}
#undef STR
		}
		*str++ = '\0';
		ret = g_renew(char, ret, str - ret);
	}
	return ret;
}

/* ------------------------------------------------------------------------- */

static char *html_quote_treename(RSCFILE *file, _WORD treeindex, unsigned int flags)
{
	RSCTREE *tree;
	char *p;
	
	tree = rsc_tree_index(file, treeindex, RT_ANY);
	p = html_quote_name(tree->rt_name, flags);
	return p;
}

/* ------------------------------------------------------------------------- */

static void html_out_globals(RSCFILE *file, rsc_opts *opts, GString *out)
{
	(void) file;
	(void) opts;
	(void) out;
}

/* ------------------------------------------------------------------------- */

static gboolean html_out_stylesheet(rsc_opts *opts, GString *outstr, gboolean do_inline)
{
	GString *out;
	FILE *outfp = NULL;
	
	if (do_inline)
	{
		out = outstr;
		g_string_append(out, "<style type=\"text/css\">\n");
	} else
	{
		char *fname;
		int exists;
		struct stat st;
		
		if (html_doctype >= HTML_DOCTYPE_XSTRICT)
			g_string_append_printf(outstr, "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\"%s\n", rscview_css_name, html_closer);
		else
			g_string_append_printf(outstr, "<style type=\"text/css\">@import url(\"%s\");</style>\n", rscview_css_name);
		if (opts->for_cgi || css_written)
			return TRUE;
		fname = g_build_filename(opts->output_dir, rscview_css_name, NULL);
		exists = stat(fname, &st) == 0;
		if (exists && !force_overwrite)
		{
			g_free(fname);
			return TRUE;
		}
		outfp = fopen(fname, "wb");
		if (outfp == NULL)
		{
			fprintf(opts->errorfile, "%s: %s: %s\n", program_name, fname, strerror(errno));
			g_free(fname);
			return FALSE;
		}
		if (opts->verbose >= 2 && opts->outfile != stdout)
			fprintf(stdout, _("writing %s\n"), rscview_css_name);
		css_written = TRUE;
		g_free(fname);
		out = g_string_sized_new(5000);
		g_string_append_printf(out, "/* This file was automatically generated by %s version %s */\n\n", program_name, program_version);
	}
	
	g_string_append(out, "body {\n");
	g_string_append_printf(out, "  background-color: %s;\n", COLOR_BG);
	g_string_append_printf(out, "  color: %s;\n", COLOR_TEXT);
	g_string_append(out, "}\n");
	
	g_string_append(out, "/* style used to display links */\n");
	g_string_append(out, "a:link, a:visited {\n");
	g_string_append_printf(out, "  color: %s;\n", COLOR_LINK);
	g_string_append(out, "  text-decoration: underline;\n");
	g_string_append(out, "}\n");
	
	g_string_append(out, "ul {\n");
	g_string_append(out, "  list-style-type: none;\n");
	g_string_append(out, "  margin: 0;\n");
	g_string_append(out, "  padding: 0;\n");
	g_string_append(out, "}\n");
	
	g_string_append(out, "/* style used for the navigation toolbar */\n");
	g_string_append_printf(out, ".%s {\n", html_toolbar_style);
	g_string_append(out, "  position:fixed;\n");
	g_string_append(out, "  top:0;\n");
	g_string_append(out, "  width:100%;\n");
	g_string_append(out, "  height:28px;\n");
	g_string_append(out, "  overflow:hidden;\n");
	g_string_append(out, "  z-index:3;\n");
	g_string_append(out, "  margin:0;\n");
	g_string_append(out, "  padding:0;\n");
	g_string_append_printf(out, "  background-color: %s;\n", COLOR_BG);
	g_string_append_printf(out, "  color: %s;\n", COLOR_TEXT);
	g_string_append(out, "}\n");
	
	g_string_append(out, "/* style used for images in the navigation toolbar */\n");
	g_string_append_printf(out, ".%s {\n", html_nav_img_style);
	g_string_append(out, "  display:block;\n");
	g_string_append(out, "  border:0;\n");
	g_string_append(out, "  width:40px;\n");
	g_string_append(out, "}\n");
	
	g_string_append(out, "/* style used for disabled images in the navigation toolbar */\n");
	g_string_append_printf(out, ".%s_disabled {\n", html_nav_img_style);
	g_string_append(out, "  display:block;\n");
	g_string_append(out, "  border:0;\n");
	g_string_append(out, "  width:40px;\n");
	g_string_append(out, "  opacity: 0.4;\n");
	g_string_append(out, "  /* filter: alpha(opacity=40); For IE8 and earlier */\n");
	g_string_append(out, "}\n");
	
	g_string_append(out, "li {\n");
	g_string_append(out, "  float: left;\n");
	g_string_append(out, "}\n");
	
	g_string_append(out, "/* style used for the body of regular nodes */\n");
	g_string_append_printf(out, ".%s {\n", html_node_style);
	g_string_append(out, "  z-index:2;\n");
	g_string_append(out, "}\n");
	
	g_string_append(out, "/* style used for the body of popup nodes */\n");
	g_string_append_printf(out, ".%s {\n", html_pnode_style);
	g_string_append_printf(out, "  background-color: %s;\n", COLOR_BG);
	g_string_append_printf(out, "  color: %s;\n", COLOR_TEXT);
	g_string_append(out, "  z-index:3;\n");
	g_string_append(out, "  box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);\n");
	g_string_append(out, "}\n");
	
	g_string_append(out, "/* style used for the body of popup nodes */\n");
	g_string_append_printf(out, ".%s {\n", html_dropdown_pnode_style);
	g_string_append_printf(out, "  background-color: %s;\n", COLOR_BG);
	g_string_append_printf(out, "  color: %s;\n", COLOR_TEXT);
	g_string_append(out, "  z-index:4;\n");
	g_string_append(out, "  box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);\n");
	g_string_append(out, "  border: solid 1px;\n");
	g_string_append(out, "  display:none;\n");
	g_string_append(out, "  position:relative;\n");
	g_string_append(out, "  padding: 3px 6px;\n");
	g_string_append(out, "}\n");
	
	g_string_append(out, "/* style used for the body of the file info */\n");
	g_string_append_printf(out, ".%s {\n", html_dropdown_info_style);
	g_string_append_printf(out, "  background-color: %s;\n", COLOR_BG);
	g_string_append_printf(out, "  color: %s;\n", COLOR_TEXT);
	g_string_append(out, "  z-index:5;\n");
	g_string_append(out, "  box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);\n");
	g_string_append(out, "  border: solid 1px;\n");
	g_string_append(out, "  display:none;\n");
	g_string_append(out, "  position:fixed;\n");
	g_string_append(out, "  top: 34px;\n");
	g_string_append(out, "  padding: 3px 6px;\n");
	g_string_append(out, "}\n");

	g_string_append(out, "/* style used for error messages */\n");
	g_string_append_printf(out, ".%s {\n", html_error_note_style);
	g_string_append(out, "}\n");
	
	g_string_append(out, "/* style used for the outer element of popups */\n");
	g_string_append_printf(out, ".%s {\n", html_dropdown_style);
	g_string_append(out, "  position:relative;\n");
	g_string_append(out, "  display:inline;\n");
	g_string_append(out, "}\n");
	
	g_string_append_printf(out, ".%s:hover .%s {\n", html_dropdown_style, html_dropdown_pnode_style);
	g_string_append(out, "  display:block;\n");
	g_string_append(out, "}\n");
	
	if (do_inline)
	{
		g_string_append(out, "</style>\n");
	}
	
	if (outfp)
	{
		write_strout(out, outfp);
		g_string_free(out, TRUE);
		fclose(outfp);
	}
	
	return TRUE;
}

/* ------------------------------------------------------------------------- */

static gboolean html_out_javascript(rsc_opts *opts, GString *outstr, gboolean do_inline)
{
	GString *out;
	FILE *outfp = NULL;
	const char *charset = "UTF-8";
	
	if (do_inline)
	{
		out = outstr;
		g_string_append(out, "<script type=\"text/javascript\">\n");
		if (html_doctype >= HTML_DOCTYPE_XSTRICT)
			g_string_append(out, "//<![CDATA[\n");
	} else
	{
		char *fname;
		int exists;
		struct stat st;
		
		g_string_append_printf(outstr, "<script type=\"text/javascript\" src=\"%s\" charset=\"%s\"></script>\n", rscview_js_name, charset);
		if (opts->for_cgi || js_written)
			return TRUE;
		fname = g_build_filename(opts->output_dir, rscview_js_name, NULL);
		exists = stat(fname, &st) == 0;
		if (exists && !force_overwrite)
		{
			g_free(fname);
			return TRUE;
		}
		outfp = fopen(fname, "wb");
		if (outfp == NULL)
		{
			fprintf(opts->errorfile, "%s: %s: %s\n", program_name, fname, strerror(errno));
			g_free(fname);
			return FALSE;
		}
		if (opts->verbose >= 2 && opts->outfile != stdout)
			fprintf(stdout, _("writing %s\n"), rscview_js_name);
		js_written = TRUE;
		g_free(fname);
		out = g_string_sized_new(sizeof(html_javascript_code) + 200);
		g_string_append_printf(out, "/* This file was automatically generated by %s version %s */\n\n", program_name, program_version);
	}
	
	g_string_append(out, html_javascript_code);

	if (do_inline)
	{
		if (html_doctype >= HTML_DOCTYPE_XSTRICT)
			g_string_append(out, "//]]>\n");
		g_string_append(out, "</script>\n");
	}
	
	if (outfp)
	{
		write_strout(out, outfp);
		g_string_free(out, TRUE);
		fclose(outfp);
	}
		
	return TRUE;
}
	
/* ------------------------------------------------------------------------- */

static void html_out_nav_toolbar(RSCFILE *file, rsc_opts *opts, GString *out, _WORD treeindex)
{
	char *str;
	char *title;
	const char *alt;
	const char *disabled;
	const char *void_href = "javascript:void(0);";
	int xpos = 0;
	const int button_w = 40;
	
	g_string_append_printf(out, "<div class=\"%s\">\n", html_toolbar_style);
	
	g_string_append(out, "<form action=\"rscview.cgi\" method=\"get\">\n");
	g_string_append(out, "<fieldset style=\"border:0;margin-left:0;margin-right:0;padding-top:0;padding-bottom:0;padding-left:0;padding-right:0;\">\n");
	g_string_append(out, "<legend></legend>\n");
	g_string_append(out, "<ul>\n");
	alt = _("Back");
	g_string_append_printf(out,
		"<li style=\"position:absolute;left:%dpx;\">"
		"<a href=\"javascript: window.history.go(-1)\" class=\"%s\"><img src=\"%s\" alt=\"&nbsp;%s&nbsp;\" title=\"%s\"%s%s</a>"
		"</li>\n",
		xpos,
		html_nav_img_style, html_nav_back_png, alt, alt, html_nav_dimensions, html_closer);
	xpos += button_w;
	
	if (treeindex >= 1)
	{
		str = html_filename_for_tree(file, opts, treeindex - 1, TRUE);
		title = html_quote_treename(file, treeindex - 1, QUOTE_UNICODE|QUOTE_NOLTR);
		disabled = "";
	} else
	{
		title = g_strdup(_("Previous page"));
		str = g_strdup(void_href);
		disabled = "_disabled";
	}
	g_string_append_printf(out,
		"<li style=\"position:absolute;left:%dpx;\">"
		"<a href=\"%s\" class=\"%s%s\" accesskey=\"p\" rel=\"prev\"><img src=\"%s\" alt=\"&nbsp;%s&nbsp;\" title=\"%s\"%s%s</a>"
		"</li>\n",
		xpos,
		str, html_nav_img_style, disabled, html_nav_prev_png, title, title, html_nav_dimensions, html_closer);
	g_free(title);
	g_free(str);
	xpos += button_w;
	
	if (treeindex < (file->rsc_ntrees - 1))
	{
		str = html_filename_for_tree(file, opts, treeindex + 1, TRUE);
		title = html_quote_treename(file, treeindex + 1, QUOTE_UNICODE|QUOTE_NOLTR);
		disabled = "";
	} else
	{
		title = g_strdup(_("Next page"));
		str = g_strdup(void_href);
		disabled = "_disabled";
	}
	g_string_append_printf(out,
		"<li style=\"position:absolute;left:%dpx;\">"
		"<a href=\"%s\" class=\"%s%s\" accesskey=\"n\" rel=\"next\"><img src=\"%s\" alt=\"&nbsp;%s&nbsp;\" title=\"%s\"%s%s</a>"
		"</li>\n",
		xpos,
		str, html_nav_img_style, disabled, html_nav_next_png, title, title, html_nav_dimensions, html_closer);
	g_free(title);
	g_free(str);
	xpos += button_w;

	alt = _("Show info about file");
	str = g_strdup(void_href);
	disabled = "";
	g_string_append_printf(out,
		"<li style=\"position:absolute;left:%dpx;\">"
		"<a href=\"%s\" class=\"%s%s\" onclick=\"showInfo();\" accesskey=\"i\" rel=\"copyright\"><img src=\"%s\" alt=\"&nbsp;%s&nbsp;\" title=\"%s\"%s%s</a>"
		"</li>\n",
		xpos,
		str, html_nav_img_style, disabled, html_nav_info_png, alt, alt, html_nav_dimensions, html_closer);
	g_free(str);
	xpos += button_w + 20;

	if (opts->for_cgi)
	{
		char *str;
		
		alt = _("View a new file");
		disabled = "";
		g_string_append_printf(out,
			"<li style=\"position:absolute;left:%dpx;\">"
			"<a href=\"%s\" class=\"%s%s\" accesskey=\"o\"><img src=\"%s\" alt=\"&nbsp;%s&nbsp;\" title=\"%s\"%s%s</a>"
			"</li>\n",
			xpos,
			html_nav_load_href, html_nav_img_style, disabled, html_nav_load_png, alt, alt, html_nav_dimensions, html_closer);
		xpos += button_w;
	
		g_string_append_printf(out, "<li style=\"position:absolute;left:%dpx;\">\n", xpos);
		str = html_quote_name(html_referer_url, QUOTE_UNICODE|QUOTE_NOLTR);
		g_string_append_printf(out, "<input type=\"hidden\" name=\"url\" value=\"%s\"%s\n", str, html_closer);
		g_free(str);
		g_string_append_printf(out, "<input accesskey=\"s\" type=\"text\" id=\"searchfield\" name=\"q\" size=\"10\" value=\"\"%s\n", html_closer);
		g_string_append_printf(out, "<script type=\"text/javascript\">document.getElementById('searchfield').placeholder = '%s';</script>\n", _("Search"));
		g_string_append(out, "</li>\n");
	}
	g_string_append(out, "</ul>\n");
	g_string_append(out, "</fieldset>\n");
	g_string_append(out, "</form>\n");

	g_string_append(out, "</div>\n");
}

/* ------------------------------------------------------------------------- */

void html_out_header(RSCFILE *file, rsc_opts *opts, GString *out, const char *title, _WORD treeindex, gboolean for_error)
{
	const char *charset = "UTF-8";
	char *str;
	const char *doctype;
	
	{
	char *html_extra;
	char *html_lang;
	
	html_lang = g_strdup("en");
	
	switch (html_doctype)
	{
	case HTML_DOCTYPE_OLD:
		doctype = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\"";
		html_extra = g_strdup_printf(" lang=\"%s\"", html_lang);
		break;
	case HTML_DOCTYPE_TRANS:
		doctype = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n"
		          "        \"http://www.w3.org/TR/html4/loose.dtd\"";
		html_extra = g_strdup_printf(" lang=\"%s\"", html_lang);
		break;
	
	case HTML_DOCTYPE_XSTRICT:
		if (opts->to_xml)
			g_string_append_printf(out, "<?xml version=\"1.0\" encoding=\"%s\"?>\n", charset);
		doctype = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\"\n"
		          "          \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\"";
		html_extra = g_strdup_printf(" xml:lang=\"%s\" lang=\"%s\" xmlns=\"http://www.w3.org/1999/xhtml\"", html_lang, html_lang);
		break;
	case HTML_DOCTYPE_STRICT:
	default:
		doctype = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n"
		          "          \"http://www.w3.org/TR/html4/strict.dtd\"";
		html_extra = g_strdup_printf(" lang=\"%s\"", html_lang);
		break;
	case HTML_DOCTYPE_HTML5:
		doctype = "<!DOCTYPE html";
		html_extra = g_strdup_printf(" xml:lang=\"%s\" lang=\"%s\"", html_lang, html_lang);
		break;
	case HTML_DOCTYPE_FRAME:
	case HTML_DOCTYPE_XFRAME:
		abort();
		break;
	}

	if (doctype)
		g_string_append(out, doctype);
	(void) html_out_entities;
	if (doctype)
		g_string_append(out, ">\n");
	g_string_append(out, "<html");
	g_string_append(out, html_extra);
	g_string_append(out, ">\n");
	g_free(html_extra);
	g_free(html_lang);
	}
	
	g_string_append_printf(out, "<!-- This file was automatically generated by %s version %s -->\n", program_name, program_version);
	g_string_append_printf(out, "<!-- %s -->\n", RSCVIEW_COPYRIGHT);
	if (file != NULL && treeindex == 0)
		html_out_globals(file, opts, out);
	if (treeindex >= 0)
		g_string_append_printf(out, _("<!-- Tree #%d -->\n"), treeindex);
	g_string_append(out, "<head>\n");
	if (html_doctype >= HTML_DOCTYPE_HTML5)
		g_string_append_printf(out, "<meta charset=\"%s\"%s\n", charset, html_closer);
	else
		g_string_append_printf(out, "<meta http-equiv=\"content-type\" content=\"text/html;charset=%s\"%s\n", charset, html_closer);
	g_string_append_printf(out, "<meta name=\"GENERATOR\" content=\"%s %s\"%s\n", program_name, program_version, html_closer);
	if (title)
		g_string_append_printf(out, "<title>%s</title>\n", title);

	if (treeindex >= 0)
	{
		if (treeindex > 0)
		{
			str = html_filename_for_tree(file, opts, 0, TRUE);
			g_string_append_printf(out, "<link href=\"%s\" rel=\"%s\"%s\n", str, html_doctype >= HTML_DOCTYPE_XSTRICT ? "start" : "first", html_closer);
			g_free(str);
		}
		
		if (treeindex > 0)
		{
			str = html_filename_for_tree(file, opts, treeindex - 1, TRUE);
			g_string_append_printf(out, "<link href=\"%s\" rel=\"prev\"%s\n", str, html_closer);
			g_free(str);
		}
		
		if (treeindex < (file->rsc_ntrees - 1))
		{
			str = html_filename_for_tree(file, opts, treeindex + 1, TRUE);
			g_string_append_printf(out, "<link href=\"%s\" rel=\"next\"%s\n", str, html_closer);
			g_free(str);
		}

		if (treeindex < (file->rsc_ntrees - 1))
		{
			str = html_filename_for_tree(file, opts, file->rsc_ntrees - 1, TRUE);
			g_string_append_printf(out, "<link href=\"%s\" rel=\"%s\"%s\n", str, html_doctype >= HTML_DOCTYPE_XSTRICT ? "end" : "last", html_closer);
			g_free(str);
		}
		
		str = g_strdup("javascript: showInfo();");
		g_string_append_printf(out, "<link href=\"%s\" rel=\"copyright\"%s\n", str, html_closer);
		g_free(str);
	}
	
	html_out_stylesheet(opts, out, FALSE);
	html_out_javascript(opts, out, FALSE);
	
	if (html_doctype >= HTML_DOCTYPE_HTML5)
	{
		g_string_append(out, "<!--[if lt IE 9]>\n");
		g_string_append(out, "<script src=\"http://html5shiv.googlecode.com/svn/trunk/html5.js\" type=\"text/javascript\"></script>\n");
		g_string_append(out, "<![endif]-->\n");
	}

	g_string_append(out, "</head>\n");
	g_string_append(out, "<body>\n");
	if (file)
		g_string_append_printf(out, "<div style=\"width:%dex;position:absolute;left:%dpx;\">\n", LINE_WIDTH, TEXT_XOFFSET);

	if (for_error)
	{
		g_string_append_printf(out, "<div class=\"%s\">\n", html_error_note_style);
		g_string_append(out, "<p>\n");
	} else if (treeindex >= 0)
	{
		if (opts->hidemenu)
		{
			g_string_append_printf(out, "<div class=\"%s\">\n", html_node_style);
		} else
		{
			html_out_nav_toolbar(file, opts, out, treeindex);
			g_string_append_printf(out, "<div class=\"%s\" style=\"position:absolute; top:32px;\">\n", html_node_style);
		}
		g_string_append(out, "<pre style=\"margin-top:0;\">");
	} else
	{
		g_string_append_printf(out, "<div class=\"%s\">\n", html_pnode_style);
		g_string_append(out, "<pre>");
	}

	if (file)
	{
		/*
		 * this element is displayed for "About"
		 */
		g_string_append_printf(out, "<span class=\"%s\">", html_dropdown_style);
		g_string_append_printf(out, "<span class=\"%s\" id=\"%s_content\">", html_dropdown_info_style, html_rsc_info_id);
		g_string_append_printf(out, "</span></span>\n");
	}
}

/* ------------------------------------------------------------------------- */

void html_out_trailer(RSCFILE *file, rsc_opts *opts, GString *out, _WORD treeindex, gboolean for_error)
{
	/* RSCTREE *tree = file ? rsc_tree_index(file, treeindex, RT_ANY) : NULL; */

	UNUSED(opts);
	UNUSED(treeindex);
	if (for_error)
	{
		g_string_append(out, "</p>\n");
		g_string_append(out, "</div>\n");
	} else
	{
		g_string_append(out, "</pre>\n");
		g_string_append(out, "</div>\n");
		if (file != NULL)
			g_string_append(out, "</div>\n");
	}
#if 0
	/*
	 * hack to remove the "]>" at the start that slips
	 * in from the entity definitions if the file
	 * was not loaded as xml
	 */
	g_string_append(out, "<script type=\"text/javascript\">\n");
	g_string_append(out, "var a = document.body.firstChild;\n");
	g_string_append(out, "if (a.nodeType == 3) document.body.removeChild(a);\n");
	g_string_append(out, "</script>\n");
#endif
	
	g_string_append(out, "</body>\n");
	g_string_append(out, "</html>\n");
}
