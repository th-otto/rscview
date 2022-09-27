#include "config.h"
#include <gem.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#if defined(__CYGWIN__) && !defined(_WIN32)
/* workaround bug in cygwin curl header */
#define SOCKET int
#endif
#include <curl/curl.h>
#include <sys/time.h>
#include <utime.h>
#include "portvdi.h"
#include "nls.h"
#include "fileio.h"
#include "rsc.h"
#include "rsc_lang.h"
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
static _WORD xworkout[57];

/*
 * program options
 */
static _BOOL verbose = FALSE;
static _BOOL aes_3d = FALSE;
static char const cgi_cachedir[] = "cache";

struct curl_parms {
	const char *filename;
	FILE *fp;
	rsc_opts *opts;
};



/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static void open_screen(void)
{
	int i;
	_WORD pxy[8];
	_WORD workin[11];
	_WORD dummy;

	vdi_handle = phys_handle;
	for (i = 0; i < 10; i++)
		workin[i] = 1;
	workin[10] = 2;
	v_opnvwk(workin, &vdi_handle, &ws.ws_xres);
	vq_extnd(vdi_handle, 1, xworkout);
	vsf_interior(vdi_handle, FIS_SOLID);
	vsf_perimeter(vdi_handle, FALSE);
	vswr_mode(vdi_handle, MD_REPLACE);
	vsf_color(vdi_handle, G_GREEN);
	
	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = ws.ws_xres;
	pxy[3] = ws.ws_yres;
	vr_recfl(vdi_handle, pxy);

	vsf_color(vdi_handle, G_WHITE);
	
	if (aes_3d)
	{
		objc_sysvar(SV_SET, LK3DIND, FALSE, TRUE, &dummy, &dummy);
		objc_sysvar(SV_SET, LK3DACT, TRUE, FALSE, &dummy, &dummy);
		objc_sysvar(SV_SET, INDBUTCOL, G_LWHITE, 0, &dummy, &dummy);
		objc_sysvar(SV_SET, ACTBUTCOL, G_LWHITE, 0, &dummy, &dummy);
		objc_sysvar(SV_SET, BACKGRCOL, G_LWHITE, 0, &dummy, &dummy);
	} else
	{
		objc_sysvar(SV_SET, LK3DIND, FALSE, FALSE, &dummy, &dummy);
		objc_sysvar(SV_SET, LK3DACT, FALSE, FALSE, &dummy, &dummy);
		objc_sysvar(SV_SET, INDBUTCOL, G_WHITE, 0, &dummy, &dummy);
		objc_sysvar(SV_SET, ACTBUTCOL, G_WHITE, 0, &dummy, &dummy);
		objc_sysvar(SV_SET, BACKGRCOL, G_WHITE, 0, &dummy, &dummy);
	}
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

static void generate_imagemap(RSCTREE *tree, rsc_opts *opts, GString *out)
{
	OBJECT *obj;
	_WORD j;
	GRECT gr;
	_WORD dx, dy;
	
	UNUSED(opts);
	g_string_append_printf(out, "<map name=\"%s\">\n", tree->rt_name);
	if (tree->rt_type == RT_MENU)
	{
		obj = tree->rt_objects.menu.mn_tree;
		objc_offset(obj, ROOT, &gr.g_x, &gr.g_y);
		gr.g_w = obj[ROOT].ob_width;
		gr.g_h = obj[ROOT].ob_height;
		dx = dy = 0;
	} else
	{
		obj = tree->rt_objects.dial.di_tree;
		form_center_grect(obj, &gr);
		objc_offset(obj, ROOT, &dx, &dy);
		dx = dx - gr.g_x;
		dy = dy - gr.g_y;
	}
	
	/*
	 * if area definitions overlap, the first one will be used,
	 * so we have to output them in reverse order
	 */
	for (j = 0; !(obj[j].ob_flags & OF_LASTOB); j++)
	{
	}
	
	while (j >= 0)
	{
		const char *name;
		_WORD x, y, w, h;
		_WORD type;
		char sbuf[128];
		
		objc_offset(obj, j, &x, &y);
		x -= gr.g_x;
		y -= gr.g_y;
		w = obj[j].ob_width;
		h = obj[j].ob_height;
		type = obj[j].ob_type & OBTYPEMASK;
		/* if object #1 is a dialog title, recalc the width */
		if (tree->rt_file->rsc_emutos != EMUTOS_NONE && j == 1 && type == G_STRING && obj[j].ob_y == gl_hchar)
			w = strlen(obj[j].ob_spec.free_string) * gl_wchar;
		g_string_append_printf(out, "<area shape=\"rect\" coords=\"%d,%d,%d,%d\" title=\"Object #%d",
			x, y,
			x + w - 1, y + h - 1,
			j);
		name = ob_name(tree->rt_file, tree, j);
		if (name)
		{
			g_string_append_printf(out, "&#10;%s", name);
		}
		x = x - dx;
		y = y - dy;
		g_string_append_printf(out, "&#10;type = %s", type_name(type));
		if ((obj[j].ob_type & ~OBTYPEMASK) != 0)
			g_string_append_printf(out, "&#10;exttype = %d", (obj[j].ob_type >> 8) & 0xff);
		g_string_append_printf(out, "&#10;x = %d", x / gl_wchar);
		if (x % gl_wchar != 0)
			g_string_append_printf(out, " + %d", x % gl_wchar);
		g_string_append_printf(out, "&#10;y = %d", y / gl_hchar);
		if (y % gl_hchar != 0)
			g_string_append_printf(out, " + %d", y % gl_wchar);
		g_string_append_printf(out, "&#10;w = %d", w / gl_wchar);
		if (w % gl_wchar != 0)
			g_string_append_printf(out, " + %d", w % gl_wchar);
		g_string_append_printf(out, "&#10;h = %d", h / gl_hchar);
		if (h % gl_hchar != 0)
			g_string_append_printf(out, " + %d", h % gl_wchar);
		g_string_append_printf(out, "&#10;flags = %s", flags_name(sbuf, obj[j].ob_flags, tree->rt_file->rsc_emutos));
		g_string_append_printf(out, "&#10;state = %s", state_name(sbuf, obj[j].ob_state));
		
		g_string_append_printf(out, "\" />\n");
		j--;
	}
	g_string_append_printf(out, "</map>\n");
}

/* ------------------------------------------------------------------------- */

static _WORD write_image(RSCTREE *tree, rsc_opts *opts, _WORD x, _WORD y, _WORD w, _WORD h, GString *out, _BOOL write_imagemap)
{
	_WORD pxy[4];
	char *basename;
	char *namebuf;
	char *filename;
	_WORD err;
	
	if (verbose)
		printf("%s %ld %s: %dx%d\n", rtype_name(tree->rt_type), tree->rt_number, tree->rt_name, w, h);
	pxy[0] = x;
	pxy[1] = y;
	pxy[2] = x + w - 1;
	pxy[3] = y + h - 1;
	vs_clip(vdi_handle, 1, pxy);
	if (opts->use_timestamps)
	{
		time_t t = time(NULL);
		struct tm *tp = gmtime(&t);
		char *tmp = g_ascii_strdown(tree->rt_name, STR0TERM);;
		basename = g_strdup_printf("%s_%04d%02d%02d%02d%02d%02d%s",
			tmp,
			tp->tm_year + 1900,
			tp->tm_mon + 1,
			tp->tm_mday,
			tp->tm_hour,
			tp->tm_min,
			tp->tm_sec,
			aes_3d ? "_3d" : "");
		g_free(tmp);
	} else
	{
		char *tmp = g_ascii_strdown(tree->rt_name, STR0TERM);
		basename = g_strdup_printf("%s%s", tmp, aes_3d ? "_3d" : "");
		g_free(tmp);
	}
	if (tree->rt_file->rsc_nls_domain.lang)
		namebuf = g_strdup_printf("%03ld_%s_%s.png", tree->rt_number, tree->rt_file->rsc_nls_domain.lang, basename);
	else
		namebuf = g_strdup_printf("%03ld_%s.png", tree->rt_number, basename);
	filename = g_build_filename(opts->output_dir, namebuf, NULL);
	err = v_write_png(vdi_handle, filename);
	if (err != 0)
	{
		KINFO(("write_image: %s: %s\n", filename, strerror(err)));
	}
	g_string_append_printf(out, "<p id=\"%s\">%s:<br /><img src=\"%s/%s\" width=\"%d\" height=\"%d\" style=\"border:0;\" alt=\"%s\"",
		tree->rt_name, tree->rt_name,
		opts->ref_output_dir ? opts->ref_output_dir : ".", namebuf,
		w, h,
		tree->rt_name);
	if (write_imagemap)
	{
		g_string_append_printf(out, " usemap=\"#%s\"", tree->rt_name);
	}
	g_string_append_printf(out, " /></p>\n");
	if (write_imagemap)
	{
		generate_imagemap(tree, opts, out);
	}
	g_free(filename);
	g_free(namebuf);
	g_free(basename);
	return err;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

/*
 * track which areas are modified by calls to objc_draw().
 * form_center does not take into account rectangles drawn
 * around the ROOT object, and there is no other way
 * to obtain the outer dimensions of what is drawn
 */
static void start_drawrect(void)
{
	_WORD pxy[4];

	pxy[0] = desk.g_x;
	pxy[1] = desk.g_y;
	pxy[2] = desk.g_x + desk.g_w - 1;
	pxy[3] = desk.g_y + desk.g_h - 1;
	vs_drawrect(phys_handle, 1, pxy);
}

/* ------------------------------------------------------------------------- */

static void end_drawrect(GRECT *gr)
{
	_WORD pxy[4];

	vs_drawrect(phys_handle, 0, pxy);
	gr->g_x = pxy[0];
	gr->g_y = pxy[1];
	gr->g_w = pxy[2] - pxy[0] + 1;
	gr->g_h = pxy[3] - pxy[1] + 1;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_dialog(RSCTREE *tree, rsc_opts *opts, GString *out)
{
	OBJECT *ob;
	GRECT gr;
	_WORD err;

	ob = tree->rt_objects.dial.di_tree;
	if (ob == NULL)
		return FALSE;
	form_center_grect(ob, &gr);

	wind_update(BEG_UPDATE);
	form_dial_grect(FMD_START, &gr, &gr);
	
	clear_screen(tree->rt_name);
	
	start_drawrect();
	objc_draw_grect(ob, ROOT, MAX_DEPTH, &desk);
	end_drawrect(&gr);

	err = write_image(tree, opts, gr.g_x, gr.g_y, gr.g_w, gr.g_h, out, opts->gen_imagemap);

	form_dial_grect(FMD_FINISH, &gr, &gr);
	wind_update(END_UPDATE);

	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_menu(RSCTREE *tree, rsc_opts *opts, GString *out)
{
	OBJECT *ob;
	_WORD thebar;
	_WORD theactive;
	_WORD themenus;
	_WORD title, menubox;
	_WORD x;
	GRECT gr;
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

	objc_offset(ob, ROOT, &gr.g_x, &gr.g_y);
	gr.g_w = ob[ROOT].ob_width;
	gr.g_h = ob[ROOT].ob_height;

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
	ob[theactive].ob_width = x;
	
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
	
	ob[ROOT].ob_width = maxx;
	ob[ROOT].ob_height = maxy;
	ob[thebar].ob_width = maxx;
	ob[themenus].ob_width = maxx;
	err = write_image(tree, opts, 0, 0, maxx, maxy, out, opts->gen_imagemap);

	menu_bar(ob, FALSE);
	form_dial_grect(FMD_FINISH, &gr, &gr);
	wind_update(END_UPDATE);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_string(RSCTREE *tree, rsc_opts *opts, GString *out)
{
	const char *str;
	_WORD err;
	GRECT gr;
	TEDINFO ted = { NULL, NULL, NULL, IBM, 0, TE_CNTR, COLSPEC_MAKE(G_BLACK, G_BLACK, TEXT_OPAQUE, 0, G_WHITE), 0, 0, 0, 0 };
	OBJECT string[1] = { { NIL, NIL, NIL, G_TEXT, OF_LASTOB, OS_NORMAL, { 0 }, 0, 0, 0, 0 } };
	_WORD len;
	
	str = tree->rt_objects.str.fr_str;
	if (str == NULL)
		return FALSE;
	str = nls_dgettext(&tree->rt_file->rsc_nls_domain, str);
	
	ted.te_ptext = (char *)NO_CONST(str);
	string[0].ob_spec.tedinfo = &ted;
	len = strlen(str);
	string[0].ob_width = len * gl_wchar;
	string[0].ob_height = gl_hchar;
	form_center(string, &string[ROOT].ob_x, &string[ROOT].ob_y, &gr.g_w, &gr.g_h);
	gr.g_x = string[ROOT].ob_x;
	gr.g_y = string[ROOT].ob_y;
	
	clear_screen(tree->rt_name);
	objc_draw_grect(string, ROOT, MAX_DEPTH, &gr);

	err = write_image(tree, opts, gr.g_x, gr.g_y, gr.g_w, gr.g_h, out, FALSE);
	if (string[0].ob_width > desk.g_w)
		g_string_append_printf(out, _("<p>string width %d exceeds desktop width %d</p>\n"), string[0].ob_width, desk.g_w);

	form_dial_grect(FMD_FINISH, &gr, &gr);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_alert(RSCTREE *tree, rsc_opts *opts, GString *out)
{
	const char *str;
	_WORD err;
	GRECT gr;
	
	str = tree->rt_objects.alert.al_str;
	if (str == NULL)
		return FALSE;
	str = nls_dgettext(&tree->rt_file->rsc_nls_domain, str);
	
	clear_screen(tree->rt_name);
	/*
	 * call our special version that only displays the dialog,
	 * and does not restore the screen background.
	 */
	start_drawrect();
	form_alert_ex(1, str, 1 | (tree->rt_file->rsc_emutos != EMUTOS_NONE ? 2 : 0));
	end_drawrect(&gr);
	
	err = write_image(tree, opts, gr.g_x, gr.g_y, gr.g_w, gr.g_h, out, FALSE);

	form_dial_grect(FMD_FINISH, &gr, &gr);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_image(RSCTREE *tree, rsc_opts *opts, GString *out)
{
	_WORD err;
	BITBLK *bit;
	_WORD *data;
	_WORD width;
	_WORD height;
	GRECT gr;
	_WORD pxy[8];
	_WORD colors[2];
	MFDB src, dst;

	bit = tree->rt_objects.bit;
	data = bit->bi_pdata;
	width = bit->bi_wb * 8;
	height = bit->bi_hl;
	if (is_mouseform(bit))
	{
		data += 5;
		height -= 5;
	}

	clear_screen(tree->rt_name);

	gr.g_x = (desk.g_w - width) / 2 + desk.g_x;
	gr.g_y = (desk.g_h - height) / 2 + desk.g_y;
	gr.g_w = width;
	gr.g_h = height;

	pxy[0] = gr.g_x;
	pxy[1] = gr.g_y;
	pxy[2] = gr.g_x + gr.g_w - 1;
	pxy[3] = gr.g_y + gr.g_h - 1;
	vs_clip(vdi_handle, 1, pxy);
	vswr_mode(vdi_handle, MD_REPLACE);
	vsf_color(vdi_handle, G_WHITE);
	vr_recfl(vdi_handle, pxy);

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = gr.g_w - 1;
	pxy[3] = gr.g_h - 1;
	pxy[4] = gr.g_x;
	pxy[5] = gr.g_y;
	pxy[6] = gr.g_x + gr.g_w - 1;
	pxy[7] = gr.g_y + gr.g_h - 1;

	src.fd_w = width;
	src.fd_h = height;
	src.fd_nplanes = 1;
	src.fd_wdwidth = (src.fd_w + 15) >> 4;
	src.fd_stand = FALSE;
	src.fd_addr = data;

	dst.fd_w = ws.ws_xres + 1;
	dst.fd_h = ws.ws_yres + 1;
	dst.fd_nplanes = xworkout[4];
	dst.fd_wdwidth = (dst.fd_w + 15) >> 4;
	dst.fd_stand = FALSE;
	dst.fd_addr = 0;

	colors[0] = G_BLACK;
	colors[1] = G_WHITE;
	vrt_cpyfm(vdi_handle, MD_TRANS, pxy, &src, &dst, colors);

	err = write_image(tree, opts, gr.g_x, gr.g_y, gr.g_w, gr.g_h, out, FALSE);

	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_tree(RSCTREE *tree, rsc_opts *opts, GString *out)
{
	_BOOL ret = TRUE;
	
	if (tree == NULL)
		return FALSE;
	switch (tree->rt_type)
	{
	case RT_DIALOG:
	case RT_FREE:
	case RT_UNKNOWN:
		ret &= draw_dialog(tree, opts, out);
		break;
	case RT_MENU:
		ret &= draw_menu(tree, opts, out);
		break;
	case RT_FRSTR:
		ret &= draw_string(tree, opts, out);
		break;
	case RT_ALERT:
		ret &= draw_alert(tree, opts, out);
		break;
	case RT_FRIMG:
	case RT_MOUSE:
		ret &= draw_image(tree, opts, out);
		break;
	case RT_BUBBLEMORE:
	case RT_BUBBLEUSER:
		break;
	}
	return ret;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_all_trees(RSCFILE *file, rsc_opts *opts, GString *out)
{
	RSCTREE *tree;
	_BOOL ret = TRUE;
	
	FOR_ALL_RSC(file, tree)
	{
		ret &= draw_tree(tree, opts, out);
	}
	return ret;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static gboolean display_tree(const char *filename, rsc_opts *opts, GString *out, _WORD treeindex)
{
	RSCFILE *file;
	gboolean retval = FALSE;
	_UWORD load_flags = XRSC_SAFETY_CHECKS;
	
	po_init(opts->po_dir, FALSE, FALSE);
	appl_init();

	menu_register(-1, program_name);
	phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
	wind_get(DESK, WF_WORKXYWH, &desk.g_x, &desk.g_y, &desk.g_w, &desk.g_h);

	file = load_all(filename, gl_wchar, gl_hchar, opts->lang, load_flags, opts->po_dir);
	if (file == NULL)
	{
		html_out_header(NULL, opts, out, _("404 Not Found"), -1, TRUE);
		g_string_append_printf(out, "%s: %s\n", rsc_basename(filename), strerror(errno));
		html_out_trailer(NULL, opts, out, -1, TRUE);
	} else
	{
		if (opts->charset)
		{
			int cset = po_get_charset_id(opts->charset);
			if (cset >= 0)
				file->rsc_nls_domain.fontset = cset;
		}

		open_screen();
		vst_font(vdi_handle, file->rsc_nls_domain.fontset);
		vst_font(phys_handle, file->rsc_nls_domain.fontset);

		html_out_header(file, opts, out, rsc_basename(filename), treeindex, FALSE);

		if (treeindex >= 0)
			retval = draw_tree(rsc_tree_index(file, treeindex, RT_ANY), opts, out);
		else
			retval = draw_all_trees(file, opts, out);

		vst_font(phys_handle, 1);
		close_screen();

		html_out_trailer(file, opts, out, treeindex, FALSE);

		rsc_file_delete(file, FALSE);
		xrsrc_free(file);
	}

	appl_exit();
	po_exit();

	return retval;
}

/* ------------------------------------------------------------------------- */

static gboolean display_file(const char *filename, rsc_opts *opts, GString *out)
{
	RSCTREE *tree;
	RSCFILE *file;
	gboolean retval = FALSE;
	_UWORD load_flags = XRSC_SAFETY_CHECKS;
	unsigned int treeindex;

	po_init(opts->po_dir, FALSE, FALSE);
	appl_init();

	menu_register(-1, program_name);
	phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
	
	file = load_all(filename, gl_wchar, gl_hchar, opts->lang, load_flags, opts->po_dir);
	if (file == NULL)
	{
		html_out_header(NULL, opts, out, _("404 Not Found"), -1, TRUE);
		g_string_append_printf(out, "%s: %s\n", rsc_basename(filename), strerror(errno));
		html_out_trailer(NULL, opts, out, -1, TRUE);
	} else
	{
		if (opts->charset)
		{
			int cset = po_get_charset_id(opts->charset);
			if (cset >= 0)
				file->rsc_nls_domain.fontset = cset;
		}

		open_screen();
		vst_font(vdi_handle, file->rsc_nls_domain.fontset);
		vst_font(phys_handle, file->rsc_nls_domain.fontset);

		html_out_header(file, opts, out, rsc_basename(filename), -1, FALSE);

		retval = TRUE;
		g_string_append(out, "<table>\n");
		treeindex = 0;
		FOR_ALL_RSC(file, tree)
		{
			char *fname;
			char *quoted;
			char *name_quoted;
			const char *image;
			
			g_string_append(out, "<tr><td>");
			switch (tree->rt_type)
			{
			case RT_DIALOG:
				image = "dialog";
				break;
			case RT_FREE:
				image = "free";
				break;
			case RT_UNKNOWN:
				image = "unknown";
				break;
			case RT_MENU:
				image = "menue";
				break;
			case RT_FRSTR:
				image = "string";
				break;
			case RT_ALERT:
				image = "alert";
				break;
			case RT_FRIMG:
				image = "image";
				break;
			case RT_MOUSE:
				image = "mouse";
				break;
			case RT_BUBBLEMORE:
			case RT_BUBBLEUSER:
			default:
				image = NULL;
				break;
			}
			if (image)
				g_string_append_printf(out, "<img src=\"images/%s.png\">", image);
			g_string_append(out, "</td>\n");

			g_string_append(out, "<td>");
			fname = g_strdup_printf("%s?url=%s&index=%u%s", cgi_scriptname, html_referer_url, treeindex, opts->cgi_cached ? "&cached=1" : "");
			quoted = html_quote_name(fname, QUOTE_UNICODE|QUOTE_NOLTR);
			name_quoted = html_quote_name(tree->rt_name, QUOTE_UNICODE);
			g_string_append_printf(out, "<a href=\"%s\">%s</a>",
				quoted,
				name_quoted);
			g_free(name_quoted);
			g_free(quoted);
			g_free(fname);
			g_string_append(out, "</td></tr>\n");
			
			treeindex++;
		}
		g_string_append(out, "</table>\n");

		vst_font(phys_handle, 1);
		close_screen();

		html_out_trailer(file, opts, out, -1, FALSE);

		rsc_file_delete(file, FALSE);
		xrsrc_free(file);
	}

	appl_exit();
	po_exit();

	return retval;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

#define hyp_utf8_fopen fopen
#define hyp_utf8_fclose fclose
#define hyp_utf8_fprintf fprintf
#define hyp_utf8_strerror strerror
#define hyp_utf8_stat stat
#define hyp_basename rsc_basename

static size_t mycurl_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	struct curl_parms *parms = (struct curl_parms *) userdata;
	
	if (size == 0 || nmemb == 0)
		return 0;
	if (parms->fp == NULL)
	{
		parms->fp = hyp_utf8_fopen(parms->filename, "wb");
		if (parms->fp == NULL)
			hyp_utf8_fprintf(parms->opts->errorfile, "%s: %s\n", parms->filename, hyp_utf8_strerror(errno));
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
		hyp_utf8_fprintf(parms->opts->errorfile, "== Info: %s", data);
		if (size == 0 || data[size - 1] != '\n')
			fputc('\n', parms->opts->errorfile);
		break;
	case CURLINFO_HEADER_OUT:
		hyp_utf8_fprintf(parms->opts->errorfile, "=> Send header %ld\n", (long)size);
		fwrite(data, 1, size, parms->opts->errorfile);
		break;
	case CURLINFO_DATA_OUT:
		hyp_utf8_fprintf(parms->opts->errorfile, "=> Send data %ld\n", (long)size);
		break;
	case CURLINFO_SSL_DATA_OUT:
		hyp_utf8_fprintf(parms->opts->errorfile, "=> Send SSL data %ld\n", (long)size);
		break;
	case CURLINFO_HEADER_IN:
		hyp_utf8_fprintf(parms->opts->errorfile, "<= Recv header %ld\n", (long)size);
		fwrite(data, 1, size, parms->opts->errorfile);
		break;
	case CURLINFO_DATA_IN:
		hyp_utf8_fprintf(parms->opts->errorfile, "<= Recv data %ld\n", (long)size);
		break;
	case CURLINFO_SSL_DATA_IN:
		hyp_utf8_fprintf(parms->opts->errorfile, "<= Recv SSL data %ld\n", (long)size);
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
	struct curl_parms parms;
	struct stat st;
	long unmet;
	long respcode;
	CURLcode curlcode;
	double size;
	char err[CURL_ERROR_SIZE];
	char *content_type;

	curl_easy_setopt(curl, CURLOPT_URL, filename);
	curl_easy_setopt(curl, CURLOPT_REFERER, filename);
	local_filename = g_build_filename(opts->output_dir, hyp_basename(filename), NULL);
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

	if (hyp_utf8_stat(local_filename, &st) == 0)
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
	hyp_utf8_fprintf(opts->errorfile, "%s: GET from %s, url=%s, curl=%d, resp=%ld, size=%ld, content=%s\n", currdate(), fixnull(cgiRemoteHost), filename, curlcode, respcode, (long)size, printnull(content_type));
	
	if (parms.fp)
	{
		hyp_utf8_fclose(parms.fp);
		parms.fp = NULL;
	}
	
	if (curlcode != CURLE_OK || stat(local_filename, &st) != 0)
	{
		html_out_header(NULL, opts, body, err, -1, TRUE);
		g_string_append_printf(body, "%s:\n%s", _("Download error"), err);
		html_out_trailer(NULL, opts, body, -1, TRUE);
		unlink(local_filename);
		g_free(local_filename);
		local_filename = NULL;
	} else if ((respcode != 200 && respcode != 304) ||
		(respcode == 200 && content_type != NULL && strcmp(content_type, "text/plain") == 0))
	{
		/* most likely the downloaded data will contain the error page */
		parms.fp = hyp_utf8_fopen(local_filename, "rb");
		if (parms.fp != NULL)
		{
			size_t nread;
			
			while ((nread = fread(err, 1, sizeof(err), parms.fp)) > 0)
				g_string_append_len(body, err, nread);
			hyp_utf8_fclose(parms.fp);
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

static gboolean has_extension(const char *filename, const char *ext)
{
	const char *p = strrchr(filename, '.');
	if (p == NULL)
		return FALSE;
	return g_ascii_strcasecmp(p + 1, ext) == 0;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

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
	_BOOL show_contents = FALSE;
	gboolean retval = FALSE;

	memset(opts, 0, sizeof(*opts));
	opts->cgi_cached = FALSE;
	opts->use_xhtml = FALSE;
	opts->to_xml = FALSE;
	opts->charset = NULL;
	opts->hidemenu = FALSE;
	opts->for_cgi = TRUE;
	opts->verbose = 0;
	opts->outfile = out;
	opts->gen_imagemap = TRUE;
	opts->errorfile = fopen("rscview.log", "a");
	if (opts->errorfile == NULL)
		opts->errorfile = stderr;
#if 0
	else
		dup2(fileno(opts->errorfile), 2);
#endif
	set_errout_handler(stdout_handler, opts->errorfile);

	body = g_string_new(NULL);
	cgiInit(body);

	{
		char *dir = rsc_path_get_dirname(cgiScriptFilename);
		char *cache_dir = g_build_filename(dir, cgi_cachedir, NULL);
		opts->output_dir = g_build_filename(cache_dir, cgiRemoteAddr, NULL);
		opts->ref_output_dir = g_build_filename(cgi_cachedir, cgiRemoteAddr, NULL);
		opts->po_dir = g_build_filename(opts->output_dir, "po", NULL);

		if (mkdir(cache_dir, 0750) < 0 && errno != EEXIST)
			fprintf(opts->errorfile, "%s: %s\n", cache_dir, strerror(errno));
		if (mkdir(opts->output_dir, 0750) < 0 && errno != EEXIST)
			fprintf(opts->errorfile, "%s: %s\n", opts->output_dir, strerror(errno));
		
		g_free(cache_dir);
		g_free(dir);
	}
	
	html_init(opts);

	if (cgiAccept && strstr(cgiAccept, "application/xhtml+xml") != NULL)
		opts->use_xhtml = TRUE;
	opts->use_xhtml = FALSE;

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
		treeindex = (int)strtol(val, NULL, 10);
		g_free(val);
	} else
	{
		treeindex = -1;
	}
	if ((val = cgiFormString("contents")) != NULL)
	{
		show_contents = (int)strtol(val, NULL, 10);
		g_free(val);
	}
	if ((val = cgiFormString("3d")) != NULL)
	{
		aes_3d = (int)strtol(val, NULL, 10);
		g_free(val);
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
		} else if (empty(hyp_basename(filename)) || (!opts->cgi_cached && g_ascii_strcasecmp(scheme, "file") == 0))
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
					html_referer_url = g_strdup(hyp_basename(filename));
					local_filename = g_build_filename(opts->output_dir, html_referer_url, NULL);
					g_free(filename);
					filename = local_filename;
				} else
				{
					html_referer_url = g_strdup(filename);
					local_filename = curl_download(curl, opts, body, filename);
					g_free(filename);
					filename = local_filename;
					if (filename)
						opts->cgi_cached = TRUE;
				}
			}
		}
		if (filename && exit_status == EXIT_SUCCESS)
		{
			if (show_contents)
				retval = display_file(filename, opts, body);
			else
				retval = display_tree(filename, opts, body, treeindex);
			if (retval == FALSE)
				exit_status = EXIT_FAILURE;
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
		const char *data;
		int len;
		cgiFormResultType result;
		int first = TRUE;
		
		g_string_truncate(body, 0);
		result = cgiFormFileFind(first, "file", &filename, NULL, &data, &len);
		if (result != cgiFormSuccess || filename == NULL || len == 0)
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
			char *rsc_filename = NULL;
			
			do {
				
				if (*filename == '\0')
				{
					g_free(filename);
#if defined(HAVE_MKSTEMPS)
					{
						int fd;
						
						filename = g_strdup("tmpfile.XXXXXX.rsc");
						local_filename = g_build_filename(opts->output_dir, filename, NULL);
						fd = mkstemps(local_filename, 4);
						if (fd > 0)
							close(fd);
					}
#elif defined(HAVE_MKSTEMP)
					{
						int fd;
						char *tmp, *tmp2;
						
						tmp = g_strdup("tmpfile.XXXXXX");
						tmp2 = g_build_filename(opts->output_dir, tmp, NULL);
						fd = mkstemp(tmp2);
						if (fd > 0)
							close(fd);
						filename = g_strconcat(tmp, ".rsc", NULL);
						local_filename = g_strconcat(tmp2, ".rsc", NULL);
						g_free(tmp2);
						g_free(tmp);
					}
#else
					{
						char *tmp, *tmp2;
						
						tmp = g_strdup("tmpfile.XXXXXX");
						tmp2 = g_build_filename(opts->output_dir, tmp, NULL);
						mktemp(tmp2);
						filename = g_strconcat(tmp, ".rsc", NULL);
						local_filename = g_strconcat(tmp2, ".rsc", NULL);
						g_free(tmp2);
						g_free(tmp);
					}
#endif
				} else
				{
					local_filename = g_build_filename(opts->output_dir, hyp_basename(filename), NULL);
				}
	
				if (first)
					hyp_utf8_fprintf(opts->errorfile, "%s: POST from %s, file=%s, size=%d", currdate(), fixnull(cgiRemoteHost), hyp_basename(filename), len);
				else
					hyp_utf8_fprintf(opts->errorfile, ", file=%s, size=%d", hyp_basename(filename), len);
	
				fp = hyp_utf8_fopen(local_filename, "wb");
				if (fp == NULL)
				{
					const char *err = hyp_utf8_strerror(errno);
					hyp_utf8_fprintf(opts->errorfile, "\n%s: %s\n", local_filename, err);
					html_out_header(NULL, opts, body, _("404 Not Found"), -1, TRUE);
					g_string_append_printf(body, "%s: %s\n", hyp_basename(filename), err);
					html_out_trailer(NULL, opts, body, -1, TRUE);
					exit_status = EXIT_FAILURE;
				} else
				{
					fwrite(data, 1, len, fp);
					fclose(fp);
					opts->cgi_cached = TRUE;
				}
				
				if (has_extension(filename, "rsc"))
				{
					if (rsc_filename != NULL)
					{
						hyp_utf8_fprintf(opts->errorfile, " duplicate RSC\n");
						html_out_header(NULL, opts, body, _("More than one resource file specified"), -1, TRUE);
						g_string_append_printf(body, "%s, %s\n", hyp_basename(rsc_filename), hyp_basename(filename));
						html_out_trailer(NULL, opts, body, -1, TRUE);
						exit_status = EXIT_FAILURE;
					} else
					{
						rsc_filename = g_strdup(local_filename);
						html_referer_url = g_strdup(filename);
					}
				}
				
				g_free(local_filename);
				first = FALSE;
				g_free(filename);
				filename = NULL;
				result = cgiFormFileFind(first, "file", &filename, NULL, &data, &len);
			} while (result == cgiFormSuccess && exit_status == EXIT_SUCCESS);
			hyp_utf8_fprintf(opts->errorfile, "\n");
			
			if (exit_status == EXIT_SUCCESS)
			{
				if (rsc_filename == NULL)
				{
					html_out_header(NULL, opts, body, _("No resource file specified"), -1, TRUE);
					g_string_append_printf(body, _("There was no resource file found in POSTED data"));
					html_out_trailer(NULL, opts, body, -1, TRUE);
					exit_status = EXIT_FAILURE;
				} else
				{
					if (show_contents)
						retval = display_file(rsc_filename, opts, body);
					else
						retval = display_tree(rsc_filename, opts, body, treeindex);
					if (retval == FALSE)
						exit_status = EXIT_FAILURE;
				}
			}
			g_free(rsc_filename);
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
		hyp_utf8_fclose(opts->errorfile);
	}
	g_free(opts->po_dir);
	g_free(opts->ref_output_dir);
	g_free(opts->output_dir);
	
	if (curl)
	{
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}
	
	return exit_status;
}

/*
export SERVER_NAME=127.0.0.2
export REMOTE_ADDR=127.0.0.1
export HTTP_REFERER=http://127.0.0.2/rscview/
export SCRIPT_FILENAME=/srv/www/htdocs/rscview/rscview.cgi
export SCRIPT_NAME=/rscview/rscview.cgi
export REQUEST_METHOD=GET
export DOCUMENT_ROOT=/srv/www/htdocs
export REQUEST_URI='/rscview/rscview.cgi?url=/rsc/orcs.rsc'
export QUERY_STRING='url=/rsc/orcs.rsc'
*/
