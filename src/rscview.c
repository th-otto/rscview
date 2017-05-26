#include "config.h"
#include <gem.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include "portvdi.h"
#include "nls.h"
#include "fileio.h"
#include "rsc.h"
#include "ws.h"
#include "debug.h"
#include "pofile.h"
#ifdef _WIN32
#include <direct.h>
#endif
#include "../vdi/writepng.h" /* for writepng_version_info */


char const program_name[] = "rscview";
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
static _BOOL xml_out = FALSE;
static _BOOL verbose = FALSE;
static const char *pngdir;
static const char *htmlout_name;
static const char *html_dir;
static FILE *htmlout_file;
static _BOOL gen_imagemap;

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

	vsf_color(vdi_handle, G_WHITE);
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

static void str_lwr(char *name)
{
	while (*name)
	{
		*name = tolower(*name);
		name++;
	}
}

/* ------------------------------------------------------------------------- */

static _WORD write_png(RSCTREE *tree, _WORD x, _WORD y, _WORD w, _WORD h, _BOOL write_imagemap)
{
	_WORD pxy[4];
	char basename[MAXNAMELEN + 1];
	char filename[PATH_MAX];
	_WORD err;
	char *p;
	
	if (verbose)
		printf("%s %ld %s: %dx%d\n", rtype_name(tree->rt_type), tree->rt_number, tree->rt_name, w, h);
	pxy[0] = x;
	pxy[1] = y;
	pxy[2] = x + w - 1;
	pxy[3] = y + h - 1;
	vs_clip(vdi_handle, 1, pxy);
	strcpy(basename, tree->rt_name);
	str_lwr(basename);
	p = filename;
	if (pngdir)
	{
		int len;
		
#ifdef _WIN32
		(void) _mkdir(pngdir);
#else
		(void) mkdir(pngdir, 0755);
#endif
		strcpy(p, pngdir);
		len = strlen(p);
		p += len;
		if (len > 0 && p[-1] != '/')
			*p++ = '/';
	}
	if (tree->rt_file->rsc_nls_domain.lang)
		sprintf(p, "%03ld_%s_%s.png", tree->rt_number, tree->rt_file->rsc_nls_domain.lang, basename);
	else
		sprintf(p, "%03ld_%s.png", tree->rt_number, basename);
	err = v_write_png(vdi_handle, filename);
	if (err != 0)
	{
		KINFO(("write_png: %s: %s\n", filename, strerror(err)));
	}
	if (htmlout_file)
	{
		fprintf(htmlout_file, "<p>%s:<br /><img src=\"%s/%s\" alt=\"%s\"",
			tree->rt_name,
			html_dir ? html_dir : ".", p,
			tree->rt_name);
		if (write_imagemap)
		{
			fprintf(htmlout_file, " usemap=\"#%s\"", tree->rt_name);
		}
		fprintf(htmlout_file, " /></p>\n");
		if (write_imagemap)
		{
			OBJECT *obj;
			_WORD j;
			GRECT gr;
			
			fprintf(htmlout_file, "<map name=\"%s\">\n", tree->rt_name);
			if (tree->rt_type == RT_MENU)
			{
				obj = tree->rt_objects.menu.mn_tree;
				objc_offset(obj, ROOT, &gr.g_x, &gr.g_y);
				gr.g_w = obj[ROOT].ob_width;
				gr.g_h = obj[ROOT].ob_height;
			} else
			{
				obj = tree->rt_objects.dial.di_tree;
				form_center_grect(obj, &gr);
			}
			
			for (j = 0;;)
			{
				const char *name = ob_name(tree->rt_file, tree, j);
				
				if (name != NULL)
				{
					_WORD x, y;
					
					objc_offset(obj, j, &x, &y);
					fprintf(htmlout_file, "<area shape=\"rect\" coords=\"%d,%d,%d,%d\" title=\"%s\">\n",
						x - gr.g_x, y - gr.g_y,
						x - gr.g_x + obj[j].ob_width - 1, y - gr.g_y + obj[j].ob_height - 1,
						name);
				}
				if (obj[j].ob_flags & OF_LASTOB)
					break;
				j++;
			}
			fprintf(htmlout_file, "</map>\n");
		}
	}
	return err;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static _BOOL draw_dialog(RSCTREE *tree)
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
	
	objc_draw_grect(ob, ROOT, MAX_DEPTH, &gr);
	
	err = write_png(tree, gr.g_x, gr.g_y, gr.g_w, gr.g_h, gen_imagemap);

	form_dial_grect(FMD_FINISH, &gr, &gr);
	wind_update(END_UPDATE);

	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_menu(RSCTREE *tree)
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
	
	err = write_png(tree, 0, 0, maxx, maxy, gen_imagemap);

	menu_bar(ob, FALSE);
	form_dial_grect(FMD_FINISH, &gr, &gr);
	wind_update(END_UPDATE);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_string(RSCTREE *tree)
{
	/* can't do much here */
	if (verbose)
		printf("%s %ld %s\n", rtype_name(tree->rt_type), tree->rt_number, tree->rt_name);
	return TRUE;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_alert(RSCTREE *tree)
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
	
	err = write_png(tree, x, y, w, h, FALSE);

	form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_all_trees(RSCFILE *file)
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
			ret &= draw_dialog(tree);
			break;
		case RT_MENU:
			ret &= draw_menu(tree);
			break;
		case RT_FRSTR:
			ret &= draw_string(tree);
			break;
		case RT_ALERT:
			ret &= draw_alert(tree);
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

enum rscview_opt {
	OPT_VERBOSE = 'v',
	OPT_XML = 'X',
	OPT_LANG = 'l',
	OPT_PODIR = 'p',
	OPT_PNGDIR = 'P',
	OPT_CHARSET = 'c',
	OPT_VERSION = 'V',
	OPT_HELP = 'h',
	
	OPT_SETVAR = 0,
	OPT_OPTERROR = '?',
	
	OPT_CREATE_HTML = 256,
	OPT_HTML_DIR,
	OPT_IMAGEMAP
};

static struct option const long_options[] = {
	{ "xml", no_argument, NULL, OPT_XML },
	{ "verbose", no_argument, NULL, OPT_VERBOSE },
	{ "lang", required_argument, NULL, OPT_LANG },
	{ "podir", required_argument, NULL, OPT_PODIR },
	{ "pngdir", required_argument, NULL, OPT_PNGDIR },
	{ "charset", required_argument, NULL, OPT_CHARSET },
	{ "create-html", required_argument, NULL, OPT_CREATE_HTML },
	{ "imagemap", no_argument, NULL, OPT_IMAGEMAP },
	{ "html-dir", required_argument, NULL, OPT_HTML_DIR },
	{ "version", no_argument, NULL, OPT_VERSION },
	{ "help", no_argument, NULL, OPT_HELP },
	{ NULL, no_argument, NULL, 0 }
};

/* ------------------------------------------------------------------------- */

static void usage(FILE *fp)
{
	fprintf(fp, _("%s - Create png files from GEM resource files\n"), program_name);
	fprintf(fp, _("Usage: %s [<options>] <file...>\n"), program_name);
	fprintf(fp, _("Options:\n"));
	fprintf(fp, _("   -v, --verbose        emit some progress messages\n"));
	fprintf(fp, _("   -l, --lang <lang>    read <lang>.po for translation\n")); 
	fprintf(fp, _("   -p, --podir <dir>    lookup po-files in <dir>\n"));
	fprintf(fp, _("   -c, --charset <name> use <charset> for display, overriding entry from po-file\n"));
	fprintf(fp, _("   -P, --pngdir <dir>   write output files to <dir>\n"));
	fprintf(fp, _("       --version        print version and exit\n"));
	fprintf(fp, _("       --help           print this help and exit\n"));
}

/* ------------------------------------------------------------------------- */

static void stdout_handler(void *data, const char *fmt, va_list args)
{
	vfprintf((FILE *)data, fmt, args);
}

/* ------------------------------------------------------------------------- */

static void print_version(void)
{
	printf(_("%s version %s, %s\n"), program_name, program_version, program_date);
	set_errout_handler(stdout_handler, stdout);
	writepng_version_info();
}

/* ------------------------------------------------------------------------- */

int main(int argc, char **argv)
{
	int c;
	RSCFILE *file;
	const char *filename;
	int exit_status = EXIT_SUCCESS;
	const char *lang = NULL;
	const char *po_dir = NULL;
	const char *charset = NULL;
	
	while ((c = getopt_long_only(argc, argv, "c:l:p:P:vXhV", long_options, NULL)) != EOF)
	{
		switch ((enum rscview_opt) c)
		{
		case OPT_XML:
			xml_out = TRUE;
			break;
		
		case OPT_CHARSET:
			charset = optarg;
			break;
			
		case OPT_LANG:
			lang = optarg;
			break;
		
		case OPT_PODIR:
			po_dir = optarg;
			break;
		
		case OPT_PNGDIR:
			pngdir = optarg;
			break;
		
		case OPT_CREATE_HTML:
			htmlout_name = optarg;
			break;
		
		case OPT_HTML_DIR:
			html_dir = optarg;
			break;
		
		case OPT_IMAGEMAP:
			gen_imagemap = TRUE;
			break;
			
		case OPT_VERBOSE:
			verbose = TRUE;
			break;
		
		case OPT_VERSION:
			print_version();
			return EXIT_SUCCESS;
		
		case OPT_HELP:
			usage(stdout);
			return EXIT_SUCCESS;

		case OPT_SETVAR:
			/* option which just sets a var */
			break;
		
		case OPT_OPTERROR:
		default:
			usage(stderr);
			return EXIT_FAILURE;
		}
	}
	
	if (optind >= argc)
	{
		errout(_("%s: missing arguments\n"), program_name);
		return EXIT_FAILURE;
	}
	
	if (htmlout_name)
	{
		htmlout_file = fopen(htmlout_name, "w");
		if (htmlout_file == NULL)
		{
			errout(_("%s: %s: %s\n"), program_name, htmlout_name, strerror(errno));
			return EXIT_FAILURE;
		}
	}
	
	po_init(po_dir);
	appl_init();
	
	menu_register(-1, program_name);
	phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
	wind_get(DESK, WF_WORKXYWH, &desk.g_x, &desk.g_y, &desk.g_w, &desk.g_h);

	while (optind < argc)
	{
		filename = argv[optind++];
		file = load_all(filename, lang, XRSC_SAFETY_CHECKS, po_dir);
		if (file != NULL)
		{
			if (charset)
			{
				int cset = po_get_charset_id(charset);
				if (cset >= 0)
					file->rsc_nls_domain.fontset = cset;
			}
			if (xml_out)
			{
				char outfilename[PATH_MAX];
				rsc_counter counter;
				
				strcpy(outfilename, file->rsc_rsxfilename);
				set_extension(outfilename, "xml");
				if (rsc_xml_source(file, &counter, outfilename, file->data) == FALSE)
					exit_status = EXIT_FAILURE;
			} else
			{
				open_screen();
				vst_font(vdi_handle, file->rsc_nls_domain.fontset);
				vst_font(phys_handle, file->rsc_nls_domain.fontset);
				
				if (draw_all_trees(file) == FALSE)
					exit_status = EXIT_FAILURE;
				
				vst_font(phys_handle, 1);
				close_screen();
			}
							
			rsc_file_delete(file, FALSE);
			xrsrc_free(file);
		} else
		{
			exit_status = EXIT_FAILURE;
		}
	}
	
	appl_exit();
	po_exit();
		
	if (htmlout_file != NULL)
	{
		fclose(htmlout_file);
		htmlout_file = NULL;
	}
	
	return exit_status;
}
