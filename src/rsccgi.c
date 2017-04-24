#include "config.h"
#include <gem.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <sys/time.h>
#include <utime.h>
#include "portvdi.h"
#include "nls.h"
#include "fileio.h"
#include "rsc.h"
#include "ws.h"
#include "debug.h"
#include "pofile.h"
#include "../vdi/writepng.h"
#include "ro_mem.h"
#include "cgic.h"
#include "html.h"


#define ALLOWED_PROTOS ( \
	CURLPROTO_FTP | \
	CURLPROTO_FTPS | \
	CURLPROTO_HTTP | \
	CURLPROTO_HTTPS | \
	CURLPROTO_SCP | \
	CURLPROTO_SFTP | \
	CURLPROTO_TFTP)

char const program_name[] = "rscview.cgi";
char const program_version[] = RSCVIEW_VERSION;
char const program_date[] = RSCVIEW_DATE;

nls_domain nls_default_domain = { "rscview", NULL, CHARSET_ST, NULL };


/*
 * gui variables
 */
static _WORD gl_hchar;
static _WORD gl_wchar;
static _WORD gl_wbox;
static _WORD gl_hbox;							/* system sizes */
static GRECT desk;
static _WORD phys_handle;						/* physical workstation handle */
static _WORD vdi_handle;						/* virtual screen handle */
static WS ws;

/*
 * program options
 */
static _BOOL verbose = FALSE;
static char const cgi_cachedir[] = "cache";

struct curl_parms {
	const char *filename;
	FILE *fp;
	rsc_opts *opts;
};



/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

void GetTextSize(_WORD *wchar, _WORD *hchar)
{
	*wchar = gl_wchar;
	*hchar = gl_hchar;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static void open_screen(void)
{
	int i;
	_WORD pxy[8];
	_WORD workin[11];

	vdi_handle = phys_handle;
	for (i = 0; i < 10; i++)
		workin[i] = 1;
	workin[10] = 2;
	v_opnvwk(workin, &vdi_handle, &ws.ws_xres);
	vsf_interior(vdi_handle, FIS_SOLID);
	vsf_perimeter(vdi_handle, FALSE);
	vswr_mode(vdi_handle, MD_REPLACE);
	vsf_color(vdi_handle, G_GREEN);
	
	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = ws.ws_xres;
	pxy[3] = ws.ws_yres;
	vr_recfl(vdi_handle, pxy);

	vsf_color(vdi_handle, WHITE);
}

/* ------------------------------------------------------------------------- */

static void close_screen(void)
{
	v_clsvwk(vdi_handle);
}

/* ------------------------------------------------------------------------- */

static void clear_screen(char *title)
{
	static char empty[1] = { 0 };
	static TEDINFO tedinfo = {
		NULL, empty, empty, IBM, 1, TE_CNTR, 0x1100, 0x0, 1, 2,1
	};
	static OBJECT desktop[] = {
		{ NIL, 1, 2, G_BOX, OF_NONE, OS_NORMAL, { OBSPEC_MAKE(0, 0, G_BLACK, G_BLACK, TRUE, IP_SOLID, G_GREEN) }, 0, 0, 0, 0 },
		{ 2, NIL, NIL, G_BOX, OF_NONE, OS_NORMAL, { OBSPEC_MAKE(0, -1, G_BLACK, G_BLACK, FALSE, IP_HOLLOW, G_WHITE) }, 0, 0, 0, 0 },
		{ 0, NIL, NIL, G_TEXT, OF_LASTOB, OS_NORMAL, { (_LONG_PTR)&tedinfo }, 0, 0, 0, 0 },
	};
	int i;
	
	for (i = 0; i < 3; i++)
		desktop[i].ob_width = desk.g_x + desk.g_w;
	desktop[ROOT].ob_height = desk.g_y + desk.g_h;
	desktop[1].ob_height = gl_hchar + 2;
	desktop[2].ob_height = gl_hchar + 3;
	tedinfo.te_ptext = title;
	
	objc_draw(desktop, ROOT, MAX_DEPTH, 0, 0, desk.g_x + desk.g_w, desk.g_y + desk.g_h);
}

/* ------------------------------------------------------------------------- */

static _WORD write_png(RSCTREE *tree, rsc_opts *opts, _WORD x, _WORD y, _WORD w, _WORD h)
{
	_WORD pxy[4];
	char *basename;
	char namebuf[PATH_MAX];
	char *filename;
	_WORD err;
	
	if (verbose)
		printf("%s %ld %s: %dx%d\n", rtype_name(tree->rt_type), tree->rt_index, tree->rt_name, w, h);
	pxy[0] = x;
	pxy[1] = y;
	pxy[2] = x + w - 1;
	pxy[3] = y + h - 1;
	vs_clip(vdi_handle, 1, pxy);
	basename = g_ascii_strdown(tree->rt_name, STR0TERM);
	if (tree->rt_file->rsc_nls_domain.lang)
		sprintf(namebuf, "%03ld_%s_%s.png", tree->rt_index, tree->rt_file->rsc_nls_domain.lang, basename);
	else
		sprintf(namebuf, "%03ld_%s.png", tree->rt_index, basename);
	filename = g_build_filename(opts->output_dir, namebuf, NULL);
	err = v_write_png(vdi_handle, filename);
	if (err != 0)
	{
		KINFO(("write_png: %s: %s\n", filename, strerror(err)));
	}
	g_free(filename);
	g_free(basename);
	return err;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static _BOOL draw_dialog(RSCTREE *tree, rsc_opts *opts)
{
	OBJECT *ob;
	_WORD x, y, w, h;
	_WORD err;
	
	ob = tree->rt_objects.dial.di_tree;
	if (ob == NULL)
		return FALSE;
	form_center(ob, &x, &y, &w, &h);

	wind_update(BEG_UPDATE);
	form_dial(FMD_START, x, y, w, h, x, y, w, h);
	
	clear_screen(tree->rt_name);
	
	objc_draw(ob, ROOT, MAX_DEPTH, x, y, w, h);
	
	err = write_png(tree, opts, x, y, w, h);

	form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
	wind_update(END_UPDATE);

	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_menu(RSCTREE *tree, rsc_opts *opts)
{
	OBJECT *ob;
	_WORD thebar;
	_WORD theactive;
	_WORD themenus;
	_WORD title, menubox;
	_WORD x, y, w, h;
	_WORD err;
	_WORD maxx, maxy;
	
	ob = tree->rt_objects.menu.mn_tree;
	if (ob == NULL)
		return FALSE;
	/*
	 * set the width of the root object, the titlebar, and the background box
	 * to screen width
	 */
	ob[ROOT].ob_width = ob[menu_the_bar(ob)].ob_width = ob[menu_the_menus(ob)].ob_width = desk.g_x + desk.g_w;
	/*
	 * adjust the height of the root object and the
	 * background box to fill up the screen
	 */
	ob[ROOT].ob_height = desk.g_y + desk.g_h;
	ob[menu_the_menus(ob)].ob_height = ob[ROOT].ob_height - ob[menu_the_menus(ob)].ob_y;

	objc_offset(ob, ROOT, &x, &y);
	w = ob[ROOT].ob_width;
	h = ob[ROOT].ob_height;

	wind_update(BEG_UPDATE);
	
	/*
	 * draw the menu titles
	 */
	clear_screen(tree->rt_name);
	menu_bar(ob, TRUE);

	/*
	 * reposition the submenu boxes so that they don't overlap
	 */
	thebar = menu_the_bar(ob);
	if (thebar == NIL)
		return FALSE;
	themenus = ob[thebar].ob_next;
	if (themenus == thebar)
		return FALSE;
	theactive = ob[thebar].ob_head;
	if (theactive == NIL)
		return FALSE;
	title = ob[theactive].ob_head;
	if (title == NIL)
		return FALSE;
	menubox = ob[themenus].ob_head;
	if (menubox == NIL)
		return FALSE;
	x = ob[menubox].ob_x;
	do
	{
		ob[menubox].ob_x = x;
		/* ob[title].ob_x = x + gl_wchar; */
		x += ob[menubox].ob_width + 1;
		title = ob[title].ob_next;
		menubox = ob[menubox].ob_next;
	} while (title != theactive && menubox != themenus);
	
	/*
	 * draw the boxes
	 */
	maxx = maxy = 0;
	menubox = ob[themenus].ob_head;
	do
	{
		_WORD mx, my, mw, mh;
		
		objc_offset(ob, menubox, &mx, &my);
		mw = ob[menubox].ob_width;
		mh = ob[menubox].ob_height;
		mx -= 1;
		my -= 1;
		mw += 2;
		mh += 2;
		objc_draw(ob, menubox, MAX_DEPTH, mx, my, mw, mh);
		menubox = ob[menubox].ob_next;
		mx = mx + mw;
		my = my + mh;
		if (mx > maxx)
			maxx = mx;
		if (my > maxy)
			maxy = my;
	} while (menubox != themenus);
	
	err = write_png(tree, opts, 0, 0, maxx, maxy);

	menu_bar(ob, FALSE);
	form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
	wind_update(END_UPDATE);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_string(RSCTREE *tree, rsc_opts *opts)
{
	/* can't do much here */
	UNUSED(opts);
	if (verbose)
		printf("%s %ld %s\n", rtype_name(tree->rt_type), tree->rt_index, tree->rt_name);
	return TRUE;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_alert(RSCTREE *tree, rsc_opts *opts)
{
	const char *str;
	_WORD err;
	_WORD wo[57];
	_WORD x, y, w, h;
	
	str = tree->rt_objects.alert.al_str;	
	if (str == NULL)
		return FALSE;
	str = dgettext(&tree->rt_file->rsc_nls_domain, str);
	
	clear_screen(tree->rt_name);
	/*
	 * call our special version that only displays the dialog,
	 * and does not restore the screen background.
	 */
	form_alert_ex(1, str, 1 | (tree->rt_file->rsc_emutos != EMUTOS_NONE ? 2 : 0));
	/*
	 * get clipping rect
	 */
	vq_extnd(phys_handle, 1, wo);
	x = wo[45];
	y = wo[46];
	w = wo[47] - x + 1;
	h = wo[48] - y + 1;
	
	err = write_png(tree, opts, x, y, w, h);

	form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_all_trees(RSCFILE *file, rsc_opts *opts)
{
	RSCTREE *tree;
	_BOOL ret = TRUE;
	
	FOR_ALL_RSC(file, tree)
	{
		switch (tree->rt_type)
		{
		case RT_DIALOG:
		case RT_FREE:
		case RT_UNKNOWN:
			ret &= draw_dialog(tree, opts);
			break;
		case RT_MENU:
			ret &= draw_menu(tree, opts);
			break;
		case RT_FRSTR:
			ret &= draw_string(tree, opts);
			break;
		case RT_ALERT:
			ret &= draw_alert(tree, opts);
			break;
		case RT_FRIMG:
		case RT_MOUSE:
			break;
		case RT_BUBBLEMORE:
		case RT_BUBBLEUSER:
			break;
		}
	}
	return ret;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static gboolean display_tree(const char *filename, rsc_opts *opts, GString *out, _WORD treeindex)
{
	RSCFILE *file;
	const char *po_dir = NULL;
	gboolean retval = FALSE;
	
	(void) treeindex;

	file = load_all(filename, opts->lang, XRSC_SAFETY_CHECKS, po_dir);
	if (file == NULL)
	{
		html_out_header(NULL, opts, out, _("404 Not Found"), -1, TRUE);
		g_string_append_printf(out, "%s: %s\n", rsc_basename(filename), strerror(errno));
		html_out_trailer(NULL, opts, out, -1, TRUE);
		return FALSE;
	}

	appl_init();
	
	menu_register(-1, program_name);
	phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
	wind_get(DESK, WF_WORKXYWH, &desk.g_x, &desk.g_y, &desk.g_w, &desk.g_h);

	if (opts->charset)
	{
		int cset = po_get_charset_id(opts->charset);
		if (cset >= 0)
			file->rsc_nls_domain.fontset = cset;
	}

	open_screen();
	vst_font(vdi_handle, file->rsc_nls_domain.fontset);
	vst_font(phys_handle, file->rsc_nls_domain.fontset);
	
	retval = draw_all_trees(file, opts);
	
	vst_font(phys_handle, 1);
	close_screen();

	html_out_header(file, opts, out, rsc_basename(filename), treeindex, FALSE);
	html_out_trailer(file, opts, out, treeindex, FALSE);

	rsc_file_delete(file, FALSE);
	xrsrc_free(file);

	appl_exit();
		
	return retval;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static size_t mycurl_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	struct curl_parms *parms = (struct curl_parms *) userdata;
	
	if (size == 0 || nmemb == 0)
		return 0;
	if (parms->fp == NULL)
	{
		parms->fp = fopen(parms->filename, "wb");
		if (parms->fp == NULL)
			fprintf(parms->opts->errorfile, "%s: %s\n", parms->filename, strerror(errno));
	}
	if (parms->fp == NULL)
		return 0;
	return fwrite(ptr, size, nmemb, parms->fp);
}

/* ------------------------------------------------------------------------- */

static int mycurl_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userdata)
{
	struct curl_parms *parms = (struct curl_parms *) userdata;

	UNUSED(handle);
	switch (type)
	{
	case CURLINFO_TEXT:
		fprintf(parms->opts->errorfile, "== Info: %s", data);
		if (size == 0 || data[size - 1] != '\n')
			fputc('\n', parms->opts->errorfile);
		break;
	case CURLINFO_HEADER_OUT:
		fprintf(parms->opts->errorfile, "=> Send header %ld\n", (long)size);
		fwrite(data, 1, size, parms->opts->errorfile);
		break;
	case CURLINFO_DATA_OUT:
		fprintf(parms->opts->errorfile, "=> Send data %ld\n", (long)size);
		break;
	case CURLINFO_SSL_DATA_OUT:
		fprintf(parms->opts->errorfile, "=> Send SSL data %ld\n", (long)size);
		break;
	case CURLINFO_HEADER_IN:
		fprintf(parms->opts->errorfile, "<= Recv header %ld\n", (long)size);
		fwrite(data, 1, size, parms->opts->errorfile);
		break;
	case CURLINFO_DATA_IN:
		fprintf(parms->opts->errorfile, "<= Recv data %ld\n", (long)size);
		break;
	case CURLINFO_SSL_DATA_IN:
		fprintf(parms->opts->errorfile, "<= Recv SSL data %ld\n", (long)size);
		break;
	case CURLINFO_END:
	default:
		break;
 	}
	return 0;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static void stdout_handler(void *data, const char *fmt, va_list args)
{
	vfprintf((FILE *)data, fmt, args);
}

/* ------------------------------------------------------------------------- */

static const char *currdate(void)
{
	struct tm *tm;
	static char buf[40];
	time_t t;
	t = time(NULL);
	tm = localtime(&t);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
	return buf;
}

/* ------------------------------------------------------------------------- */

static char *curl_download(CURL *curl, rsc_opts *opts, GString *body, const char *filename)
{
	char *local_filename;
	struct stat st;
	long unmet;
	long respcode;
	CURLcode curlcode;
	double size;
	char err[CURL_ERROR_SIZE];
	char *content_type;
	struct curl_parms parms;
	
	curl_easy_setopt(curl, CURLOPT_URL, filename);
	curl_easy_setopt(curl, CURLOPT_REFERER, filename);
	local_filename = g_build_filename(parms.opts->output_dir, rsc_basename(filename), NULL);
	parms.filename = local_filename;
	parms.fp = NULL;
	parms.opts = opts;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mycurl_write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &parms);
	curl_easy_setopt(curl, CURLOPT_STDERR, opts->errorfile);
	curl_easy_setopt(curl, CURLOPT_PROTOCOLS, ALLOWED_PROTOS);
	curl_easy_setopt(curl, CURLOPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, (long)1);
	curl_easy_setopt(curl, CURLOPT_FILETIME, (long)1);
	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, mycurl_trace);
	curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &parms);
	*err = 0;
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, err);
	
	/* set this to 1 to activate debug code above */
	curl_easy_setopt(curl, CURLOPT_VERBOSE, (long)0);

	if (stat(local_filename, &st) == 0)
	{
		curlcode = curl_easy_setopt(curl, CURLOPT_TIMECONDITION, (long)CURL_TIMECOND_IFMODSINCE);
		curlcode = curl_easy_setopt(curl, CURLOPT_TIMEVALUE, (long)st.st_mtime);
	}
	
	/*
	 * TODO: reject attempts to connect to local addresses
	 */
	curlcode = curl_easy_perform(curl);
	
	respcode = 0;
	unmet = -1;
	size = 0;
	content_type = NULL;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respcode);
	curl_easy_getinfo(curl, CURLINFO_CONDITION_UNMET, &unmet);
	curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &size);
	curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
	fprintf(opts->errorfile, "%s: GET from %s, url=%s, curl=%d, resp=%ld, size=%ld\n", currdate(), fixnull(cgiRemoteHost), filename, curlcode, respcode, (long)size);
	
	if (parms.fp)
	{
		fclose(parms.fp);
		parms.fp = NULL;
	}
	
	if (curlcode != CURLE_OK)
	{
		html_out_header(NULL, opts, body, err, -1, TRUE);
		g_string_append_printf(body, "%s:\n%s", _("Download error"), err);
		html_out_trailer(NULL, opts, body, -1, TRUE);
		unlink(local_filename);
		g_free(local_filename);
		local_filename = NULL;
	} else if ((respcode != 200 && respcode != 304) ||
		(respcode == 200 && (content_type == NULL || strcmp(content_type, "text/plain") != 0)))
	{
		/* most likely the downloaded data will contain the error page */
		parms.fp = fopen(local_filename, "rb");
		if (parms.fp != NULL)
		{
			size_t nread;
			
			while ((nread = fread(err, 1, sizeof(err), parms.fp)) > 0)
				g_string_append_len(body, err, nread);
			fclose(parms.fp);
		}
		unlink(local_filename);
		g_free(local_filename);
		local_filename = NULL;
	} else
	{
		long ft = -1;
		if (curl_easy_getinfo(curl, CURLINFO_FILETIME, &ft) == CURLE_OK && ft != -1)
		{
			struct utimbuf ut;
			ut.actime = ut.modtime = ft;
			utime(local_filename, &ut);
		}
	}
	
	return local_filename;
}

/* ------------------------------------------------------------------------- */

int main(void)
{
	int exit_status = EXIT_SUCCESS;
	FILE *out = stdout;
	GString *body;
	CURL *curl = NULL;
	char *val;
	rsc_opts _opts;
	rsc_opts *opts = &_opts;
	_WORD treeindex = -1;
	
	opts->cgi_cached = FALSE;
	opts->use_xhtml = FALSE;
	opts->to_xml = FALSE;
	opts->charset = NULL;
	opts->hidemenu = FALSE;
	opts->for_cgi = TRUE;
	opts->verbose = 0;
	opts->outfile = out;
	opts->errorfile = fopen("rscview.log", "a");
	if (opts->errorfile == NULL)
		opts->errorfile = stderr;
	else
		dup2(fileno(opts->errorfile), 2);
	set_errout_handler(stdout_handler, opts->errorfile);

	body = g_string_new(NULL);
	cgiInit(body);

	{
		opts->output_dir = g_build_filename(cgi_cachedir, cgiRemoteAddr, NULL);

		if (mkdir(cgi_cachedir, 0750) < 0 && errno != EEXIST)
			fprintf(opts->errorfile, "%s: %s\n", cgi_cachedir, strerror(errno));
		if (mkdir(opts->output_dir, 0750) < 0 && errno != EEXIST)
			fprintf(opts->errorfile, "%s: %s\n", opts->output_dir, strerror(errno));
	}
	
	html_init(opts);

	if (cgiAccept && strstr(cgiAccept, "application/xhtml+xml") != NULL)
		opts->use_xhtml = TRUE;

	if (cgiScriptName)
		cgi_scriptname = cgiScriptName;
	
	opts->lang = cgiFormString("lang");
	if ((val = cgiFormString("cached")) != NULL)
	{
		opts->cgi_cached = (int)strtol(val, NULL, 10) != 0;
		g_free(val);
	}
	if ((val = cgiFormString("index")) != NULL)
	{
		treeindex = (int)strtoul(val, NULL, 10);
		g_free(val);
	} else
	{
		treeindex = 0;
	}
	
	if (g_ascii_strcasecmp(cgiRequestMethod, "GET") == 0)
	{
		char *url = cgiFormString("url");
		char *filename = g_strdup(url);
		char *scheme = empty(filename) ? g_strdup("undefined") : uri_has_scheme(filename) ? g_strndup(filename, strchr(filename, ':') - filename) : g_strdup("file");
		
		if (filename && filename[0] == '/')
		{
			html_referer_url = filename;
			filename = g_strconcat(cgiDocumentRoot, filename, NULL);
		} else if (empty(rsc_basename(filename)) || (!opts->cgi_cached && g_ascii_strcasecmp(scheme, "file") == 0))
		{
			/*
			 * disallow file URIs, they would resolve to local files on the WEB server
			 */
			html_out_header(NULL, opts, body, _("403 Forbidden"), -1, TRUE);
			g_string_append_printf(body,
				_("Sorry, this type of\n"
				  "<a href=\"http://www.w3.org/Addressing/\">URL</a>\n"
				  "<a href=\"http://www.iana.org/assignments/uri-schemes.html\">scheme</a>\n"
				  "(<q>%s</q>) is not\n"
				  "supported by this service. Please check that you entered the URL correctly.\n"),
				scheme);
			html_out_trailer(NULL, opts, body, -1, TRUE);
			g_free(filename);
			filename = NULL;
		} else
		{
			if (!opts->cgi_cached &&
				(curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK ||
				(curl = curl_easy_init()) == NULL))
			{
				html_out_header(NULL, opts, body, _("500 Internal Server Error"), -1, TRUE);
				g_string_append(body, _("could not initialize curl\n"));
				html_out_trailer(NULL, opts, body, -1, TRUE);
				exit_status = EXIT_FAILURE;
			} else
			{
				char *local_filename;
				
				if (opts->cgi_cached)
				{
					html_referer_url = g_strdup(rsc_basename(filename));
					local_filename = g_build_filename(opts->output_dir, html_referer_url, NULL);
					g_free(filename);
					filename = local_filename;
				} else
				{
					html_referer_url = g_strdup(filename);
					local_filename = curl_download(curl, opts, body, filename);
					g_free(filename);
					filename = local_filename;
				}
			}
		}
		if (filename && exit_status == EXIT_SUCCESS)
		{
			if (display_tree(filename, opts, body, treeindex) == FALSE)
			{
				exit_status = EXIT_FAILURE;
			}
			g_free(filename);
		}
		g_free(scheme);
		g_free(html_referer_url);
		html_referer_url = NULL;

		g_free(url);
	} else if (g_ascii_strcasecmp(cgiRequestMethod, "POST") == 0 ||
		g_ascii_strcasecmp(cgiRequestMethod, "BOTH") == 0)
	{
		char *filename;
		int len;
		
		g_string_truncate(body, 0);
		filename = cgiFormFileName("file", &len);
		if (filename == NULL || len == 0)
		{
			const char *scheme = "undefined";
			html_out_header(NULL, opts, body, _("403 Forbidden"), -1, TRUE);
			g_string_append_printf(body,
				_("Sorry, this type of\n"
				  "<a href=\"http://www.w3.org/Addressing/\">URL</a>\n"
				  "<a href=\"http://www.iana.org/assignments/uri-schemes.html\">scheme</a>\n"
				  "(<q>%s</q>) is not\n"
				  "supported by this service. Please check that you entered the URL correctly.\n"),
				scheme);
			html_out_trailer(NULL, opts, body, -1, TRUE);
		} else
		{
			FILE *fp;
			char *local_filename;
			const char *data;
			
			if (*filename == '\0')
			{
				g_free(filename);
#if defined(HAVE_MKSTEMPS)
				{
				int fd;
				filename = g_strdup("tmpfile.XXXXXX.hyp");
				local_filename = g_build_filename(opts->output_dir, rsc_basename(filename), NULL);
				fd = mkstemps(local_filename, 4);
				if (fd > 0)
					close(fd);
				}
#elif defined(HAVE_MKSTEMP)
				{
				int fd;
				filename = g_strdup("tmpfile.hyp.XXXXXX");
				local_filename = g_build_filename(opts->output_dir, rsc_basename(filename), NULL);
				fd = mkstemp(local_filename);
				if (fd > 0)
					close(fd);
				}
#else
				filename = g_strdup("tmpfile.hyp.XXXXXX");
				local_filename = g_build_filename(opts->output_dir, rsc_basename(filename), NULL);
				mktemp(local_filename);
#endif
			} else
			{
				local_filename = g_build_filename(opts->output_dir, rsc_basename(filename), NULL);
			}

			fprintf(opts->errorfile, "%s: POST from %s, file=%s, size=%d\n", currdate(), fixnull(cgiRemoteHost), rsc_basename(filename), len);

			fp = fopen(local_filename, "wb");
			if (fp == NULL)
			{
				const char *err = strerror(errno);
				fprintf(opts->errorfile, "%s: %s\n", local_filename, err);
				html_out_header(NULL, opts, body, _("404 Not Found"), -1, TRUE);
				g_string_append_printf(body, "%s: %s\n", rsc_basename(filename), err);
				html_out_trailer(NULL, opts, body, -1, TRUE);
				exit_status = EXIT_FAILURE;
			} else
			{
				data = cgiFormFileData("file", &len);
				fwrite(data, 1, len, fp);
				fclose(fp);
				opts->cgi_cached = TRUE;
				html_referer_url = g_strdup(filename);
				if (display_tree(local_filename, opts, body, treeindex) == FALSE)
				{
					exit_status = EXIT_FAILURE;
				}
			}
			g_free(local_filename);
		}
		g_free(filename);
		g_free(html_referer_url);
		html_referer_url = NULL;
	}
	
	html_out_response_header(out, body->len, opts->use_xhtml);
	cgiExit();
	write_strout(body, out);
	g_string_free(body, TRUE);
	
	g_free(opts->lang);
	if (opts->errorfile && opts->errorfile != stderr)
	{
		fclose(opts->errorfile);
	}
	g_free(opts->output_dir);
	
	if (curl)
	{
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}
	
	return exit_status;
}
