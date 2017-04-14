#include "config.h"
#include <gem.h>
#include <w_draw.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>
#include "nls.h"
#include "fileio.h"
#include "rsc.h"
#include "ws.h"
#include "debug.h"


char const program_name[] = "rscview";
char const program_version[] = VERSION;
char const program_date[] = "28.03.2017";

nls_domain nls_default_domain = { "rscview", NULL };


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


void GetTextSize(_WORD *wchar, _WORD *hchar)
{
	*wchar = gl_wchar;
	*hchar = gl_hchar;
}



/*
 * Menue-Indices
 */
/* the box containing the whole menubar */
#define menu_the_bar(menu) (menu[ROOT].ob_head)
/* the ibox containing the titles */
#define menu_the_active(menu) (menu[menu_the_bar(menu)].ob_head)
/* the first title entry */
#define menu_the_first(menu) (menu[menu_the_active(menu)].ob_head)
/* the last title entry */
#define menu_the_last(menu) (menu[menu_the_active(menu)].ob_tail)
/* the ibox containing the menu subboxes */
#define menu_the_menus(menu) (menu[menu_the_bar(menu)].ob_next)


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
	vsf_color(vdi_handle, GREEN);
	
	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = ws.ws_xres;
	pxy[3] = ws.ws_yres;
	vr_recfl(vdi_handle, pxy);

	vsf_color(vdi_handle, WHITE);
}


static void close_screen(void)
{
	v_clsvwk(vdi_handle);
}


static void clear_screen(char *title)
{
	static char empty[1] = { 0 };
	static TEDINFO tedinfo = {
		NULL, empty, empty, IBM, 1, TE_CNTR, 0x1100, 0x0, 1, 2,1
	};
	static OBJECT desktop[] = {
		{ NIL, 1, 2, G_BOX, OF_NONE, OS_NORMAL, { OBSPEC_MAKE(0, 0, G_BLACK, G_BLACK, TRUE, IP_SOLID, GREEN) }, 0, 0, 0, 0 },
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


static void str_lwr(char *name)
{
	while (*name)
	{
		*name = tolower(*name);
		name++;
	}
}


static _WORD write_png(RSCTREE *tree, _WORD x, _WORD y, _WORD w, _WORD h)
{
	_WORD pxy[4];
	char basename[MAXNAMELEN + 1];
	char filename[PATH_MAX];
	_WORD err;
	
	if (verbose)
		printf("%s %ld %s: %dx%d\n", rtype_name(tree->rt_type), tree->rt_index, tree->rt_name, w, h);
	pxy[0] = x;
	pxy[1] = y;
	pxy[2] = x + w - 1;
	pxy[3] = y + h - 1;
	vs_clip(vdi_handle, 1, pxy);
	strcpy(basename, tree->rt_name);
	str_lwr(basename);
	sprintf(filename, "%s.png", basename);
	err = v_write_png(vdi_handle, filename);
	if (err != 0)
		nf_debugprintf("write_png: %s: %s\n", filename, strerror(err));
	return err;
}


static _BOOL draw_dialog(RSCTREE *tree)
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
	
	err = write_png(tree, x, y, w, h);

	form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
	wind_update(END_UPDATE);

	return err == 0;
}


static _BOOL draw_menu(RSCTREE *tree)
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
	
	err = write_png(tree, 0, 0, maxx, maxy);

	menu_bar(ob, FALSE);
	form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
	wind_update(END_UPDATE);
	
	return err == 0;
}


static _BOOL draw_string(RSCTREE *tree)
{
	/* can't do much here */
	if (verbose)
		printf("%s %ld %s\n", rtype_name(tree->rt_type), tree->rt_index, tree->rt_name);
	return TRUE;
}


static _BOOL draw_alert(RSCTREE *tree)
{
	const char *str;
	_WORD err;
	_WORD wo[57];
	_WORD x, y, w, h;
	
	str = tree->rt_objects.alert.al_str;	
	if (str == NULL)
		return FALSE;
	
	clear_screen(tree->rt_name);
	/*
	 * call our special version that only displays dialog,
	 * and does not restore screen
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
	
	err = write_png(tree, x, y, w, h);

	form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
	
	return err == 0;
}


static _BOOL draw_all_trees(RSCFILE *file)
{
	RSCTREE *tree;
	_BOOL ret;
	
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


static struct option const long_options[] = {
	{ "xml", no_argument, NULL, 'X' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "version", no_argument, NULL, 'V' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};


static void usage(FILE *fp)
{
	fprintf(fp, _("Usage: %s <file...>\n"), program_name);
}


static void print_version(void)
{
	printf(_("%s version %s, %s\n"), program_name, program_version, program_date);
}


int main(int argc, char **argv)
{
	int c;
	RSCFILE *file;
	const char *filename;
	int exit_status = EXIT_SUCCESS;
	rsc_counter counter;
	
	while ((c = getopt_long_only(argc, argv, "vXhV", long_options, NULL)) != EOF)
	{
		switch (c)
		{
		case 'X':
			xml_out = TRUE;
			break;
		
		case 'v':
			verbose = TRUE;
			break;
		
		case 'V':
			print_version();
			return EXIT_SUCCESS;
		
		case 'h':
			usage(stdout);
			return EXIT_SUCCESS;

		case 0:
			break;
		
		default:
			usage(stderr);
			return EXIT_FAILURE;
		}
	}
	
	if (optind >= argc)
	{
		fprintf(stderr, _("%s: missing arguments\n"), program_name);
		return EXIT_FAILURE;
	}
	
	appl_init();
	
	menu_register(-1, program_name);
	phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
	wind_get(DESK, WF_WORKXYWH, &desk.g_x, &desk.g_y, &desk.g_w, &desk.g_h);

	while (optind < argc)
	{
		filename = argv[optind++];
		file = load_all(filename, XRSC_SAFETY_CHECKS);
		if (file != NULL)
		{
			if (xml_out)
			{
				char outfilename[PATH_MAX];
				
				strcpy(outfilename, file->rsc_rsxfilename);
				set_extension(outfilename, "xml");
				if (rsc_xml_source(file, &counter, outfilename, file->data) == FALSE)
					exit_status = EXIT_FAILURE;
			}
			
			open_screen();
			
			if (draw_all_trees(file) == FALSE)
				exit_status = EXIT_FAILURE;
			
			close_screen();
			
			rsc_file_delete(file, FALSE);
			xrsrc_free(file);
		} else
		{
			exit_status = EXIT_FAILURE;
		}
	}
	
	appl_exit();
		
	return exit_status;
}
