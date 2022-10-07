#include "config.h"
#include <gem.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include "fileio.h"
#include "debug.h"
#include "html.h"

const char *cgi_scriptname = "rscview.cgi";
char *html_referer_url;

static int html_doctype = HTML_DOCTYPE_XSTRICT;
static const char *html_closer = " />";
static const char *html_name_attr = "id";

static char const rscview_css_name[] = "_rscview.css";
static char const rscview_js_name[] = "_rscview.js";
static char const html_nav_load_href[] = "index.php";

#define COLOR_BG         "#ffffff"
#define COLOR_TEXT       "#0000ff"
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
static char const html_error_note_style[] = "rscview_error_note";

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
		opts->aes_3d ? "&amp;aes3d=1" : "",
		NULL);
}
	
/* ------------------------------------------------------------------------- */

static char *html_filename_for_tree(RSCFILE *file, rsc_opts *opts, _WORD treeindex, gboolean quote)
{
	char *filename;
	RSCTREE *tree;
	
	tree = rsc_tree_index(file, treeindex, RT_ANY);
	if (tree == NULL)
		return g_strdup("null");

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

static gboolean html_out_stylesheet(rsc_opts *opts, GString *outstr)
{
	UNUSED(opts);
	if (html_doctype >= HTML_DOCTYPE_XSTRICT)
		g_string_append_printf(outstr, "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\"%s\n", rscview_css_name, html_closer);
	else
		g_string_append_printf(outstr, "<style type=\"text/css\">@import url(\"%s\");</style>\n", rscview_css_name);
	return TRUE;
}

/* ------------------------------------------------------------------------- */

static gboolean html_out_javascript(rsc_opts *opts, GString *outstr)
{
	const char *charset = "UTF-8";
	
	UNUSED(opts);
	g_string_append_printf(outstr, "<script type=\"text/javascript\" src=\"%s\" charset=\"%s\"></script>\n", rscview_js_name, charset);
	
	return TRUE;
}
	
/* ------------------------------------------------------------------------- */

void html_out_nav_toolbar(RSCFILE *file, rsc_opts *opts, GString *out, _WORD treeindex)
{
	char *str;
	char *title;
	const char *alt;
	const char *disabled;
	static char const void_href[] = "javascript:void(0);";
	int xpos = 0;
	const int button_w = 40;
	
	g_string_append_printf(out, "<div class=\"%s\">\n", html_toolbar_style);
	
	g_string_append(out, "<form action=\"rscview.cgi\" method=\"get\">\n");
	g_string_append(out, "<fieldset style=\"border:0;margin-left:0;margin-right:0;padding-top:0;padding-bottom:0;padding-left:0;padding-right:0;\">\n");
	g_string_append(out, "<legend></legend>\n");
	g_string_append(out, "<nav><ul>\n");
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
	disabled = "";
	g_string_append_printf(out,
		"<li style=\"position:absolute;left:%dpx;\">"
		"<ul><li class=\"rsctitle\">"
		"<label class=\"rsctitlestring\" for=\"fileinfo\">"
		"<img class=\"%s%s\" src=\"%s\" alt=\"&nbsp;%s&nbsp;\" title=\"%s\"%s%s</a>"
		"</label>"
		"<input class=\"rsctitleinput\" type=\"checkbox\" id=\"fileinfo\"/>"
		"<ul class=\"rscmenubox\">"
		"<li class=\"rscmenuentry\"><a class=\"fileinfo\">Info goes here</a></li>"
		"</ul></li></ul></li>\n",
		xpos, html_nav_img_style, disabled, html_nav_info_png, alt, alt, html_nav_dimensions, html_closer);
	xpos += button_w + 20;

	if (opts->for_cgi)
	{
		alt = _("View a new file");
		disabled = "";
		g_string_append_printf(out,
			"<li style=\"position:absolute;left:%dpx;\">"
			"<a href=\"%s\" class=\"%s%s\" accesskey=\"o\"><img src=\"%s\" alt=\"&nbsp;%s&nbsp;\" title=\"%s\"%s%s</a>"
			"</li>\n",
			xpos,
			html_nav_load_href, html_nav_img_style, disabled, html_nav_load_png, alt, alt, html_nav_dimensions, html_closer);
		xpos += button_w;
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
	if (doctype)
		g_string_append(out, ">\n");
	g_string_append(out, "<html");
	g_string_append(out, html_extra);
	g_string_append(out, ">\n");
	g_free(html_extra);
	g_free(html_lang);
	}
	
	if (file != NULL && treeindex <= 0)
		html_out_globals(file, opts, out);
	g_string_append(out, "<head>\n");
	g_string_append_printf(out, "<!-- This file was automatically generated by %s version %s -->\n", program_name, program_version);
	g_string_append_printf(out, "<!-- %s -->\n", RSCVIEW_COPYRIGHT);
	if (treeindex >= 0)
		g_string_append_printf(out, _("<!-- Tree #%d -->\n"), treeindex);
	if (html_doctype >= HTML_DOCTYPE_HTML5)
		g_string_append_printf(out, "<meta charset=\"%s\"%s\n", charset, html_closer);
	else
		g_string_append_printf(out, "<meta http-equiv=\"content-type\" content=\"text/html;charset=%s\"%s\n", charset, html_closer);
	g_string_append_printf(out, "<meta name=\"GENERATOR\" content=\"%s %s\"%s\n", program_name, program_version, html_closer);
	if (title)
		g_string_append_printf(out, "<title>%s</title>\n", title);

	if (treeindex >= 0)
	{
		/*
		 * emit a navivation bar
		 */
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
		
#if 0
		str = g_strdup("javascript: showInfo();");
		g_string_append_printf(out, "<link href=\"%s\" rel=\"copyright\"%s\n", str, html_closer);
		g_free(str);
#endif
	}
	
	html_out_stylesheet(opts, out);
	html_out_javascript(opts, out);
	
	if (html_doctype >= HTML_DOCTYPE_HTML5)
	{
		g_string_append(out, "<!--[if lt IE 9]>\n");
		g_string_append(out, "<script src=\"http://html5shiv.googlecode.com/svn/trunk/html5.js\" type=\"text/javascript\"></script>\n");
		g_string_append(out, "<![endif]-->\n");
	}

	g_string_append(out, "</head>\n");
	g_string_append(out, "<body>\n");
#if 0
	if (file)
		g_string_append_printf(out, "<div style=\"width:%dex;position:absolute;left:%dpx;\">\n", LINE_WIDTH, TEXT_XOFFSET);
#endif

	if (for_error)
	{
		g_string_append_printf(out, "<div class=\"%s\">\n", html_error_note_style);
		g_string_append(out, "<p>\n");
	} else if (treeindex >= 0)
	{
		if (treeindex >= 0)
		{
			html_out_nav_toolbar(file, opts, out, treeindex);
			g_string_append_printf(out, "<div class=\"%s\" style=\"position:absolute; top:32px;\">\n", html_node_style);
		}
#if 0
		g_string_append(out, "<pre style=\"margin-top:0;\">");
#endif
	} else
	{
		g_string_append_printf(out, "<div class=\"%s\">\n", html_pnode_style);
#if 0
		g_string_append(out, "<pre>");
#endif
	}
}

/* ------------------------------------------------------------------------- */

void html_out_trailer(RSCFILE *file, rsc_opts *opts, GString *out, _WORD treeindex, gboolean for_error)
{
	/* RSCTREE *tree = file ? rsc_tree_index(file, treeindex, RT_ANY) : NULL; */

	UNUSED(file);
	UNUSED(opts);
	UNUSED(treeindex);
	if (for_error)
	{
		g_string_append(out, "</p>\n");
		g_string_append(out, "</div>\n");
	} else
	{
#if 0
		g_string_append(out, "</pre>\n");
#endif
		if (treeindex >= 0)
			g_string_append(out, "</div>\n");
#if 0
		if (file != NULL)
			g_string_append(out, "</div>\n");
#endif
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
