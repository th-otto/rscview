#include "config.h"
#include <gem.h>
#include <w_draw.h>
#include <getopt.h>
#include <errno.h>
#include "nls.h"
#include "fileio.h"
#include "ws.h"
#include "debug.h"


char const program_name[] = "rscview";
char const program_version[] = VERSION;
char const program_date[] = "28.03.2017";

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
static _WORD workin[11];


static void open_screen(void)
{
	int i;
	_WORD pxy[8];

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


static gboolean xml_out = FALSE;

static struct option const long_options[] = {
	{ "xml", no_argument, NULL, 'X' },
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


static void clear_screen(char *title)
{
	static char empty[1] = { 0 };
	static TEDINFO tedinfo = {
		NULL, empty, empty, IBM, 1, TE_CNTR, 0x1100, 0x0, 1, 2,1
	};
	static OBJECT desktop[] = {
		{ NIL, 1, 2, G_BOX, OF_NONE, OS_NORMAL, { OBSPEC_MAKE(0, 0, BLACK, BLACK, TRUE, IP_SOLID, GREEN) }, 0, 0, 0, 0 },
		{ 2, NIL, NIL, G_BOX, OF_NONE, OS_NORMAL, { OBSPEC_MAKE(0, -1, BLACK, BLACK, FALSE, IP_HOLLOW, WHITE) }, 0, 0, 0, 0 },
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


static _BOOL draw_dialog(RSCTREE *tree)
{
	OBJECT *ob;
	_WORD x, y, w, h;
	_WORD pxy[4];
	char filename[PATH_MAX];
	_WORD err;
	
	ob = tree->rt_objects.dial.di_tree;
	if (ob == NULL)
		return FALSE;
	form_center(ob, &x, &y, &w, &h);
	wind_update(BEG_UPDATE);
	form_dial(FMD_START, x, y, w, h, x, y, w, h);
	clear_screen(tree->rt_name);
	
	form_dial(FMD_FINISH, x, y, w, h, x, y, w, h);
	wind_update(END_UPDATE);
	pxy[0] = x;
	pxy[1] = y;
	pxy[2] = x + w - 1;
	pxy[3] = y + h - 1;
	vs_clip(vdi_handle, 0, pxy);
	sprintf(filename, "%s.png", tree->rt_name);
	err = v_write_png(vdi_handle, filename);
	if (err != 0)
		nf_debugprintf("write_png: %s: %s\n", filename, strerror(err));
	return err == 0;
}


static void draw_menu(RSCTREE *tree)
{
	OBJECT *ob;
	
	/* NYI */
	ob = tree->rt_objects.menu.mn_tree;
	if (ob == NULL)
		return;
}


static void draw_all_trees(RSCFILE *file)
{
	RSCTREE *tree;
	char *str;
	
	FOR_ALL_RSC(file, tree)
	{
		switch (tree->rt_type)
		{
		case RT_DIALOG:
		case RT_FREE:
		case RT_UNKNOWN:
			draw_dialog(tree);
			break;
		case RT_MENU:
			draw_menu(tree);
			break;
		case RT_FRSTR:
			str = tree->rt_objects.str.fr_str;
			break;
		case RT_ALERT:
			str = tree->rt_objects.alert.al_str;
			break;
		case RT_FRIMG:
		case RT_MOUSE:
			break;
		case RT_BUBBLEMORE:
		case RT_BUBBLEUSER:
			break;
		}
	}
}


int main(int argc, char **argv)
{
	int c;
	RSCFILE *file;
	const char *filename;
	int exit_status = EXIT_SUCCESS;
	rsc_counter counter;
	
	while ((c = getopt_long_only(argc, argv, "XhV", long_options, NULL)) != EOF)
	{
		switch (c)
		{
		case 'X':
			xml_out = TRUE;
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
			
			appl_init();
			
			menu_register(-1, program_name);
			phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
			wind_get(DESK, WF_WORKXYWH, &desk.g_x, &desk.g_y, &desk.g_w, &desk.g_h);
			nf_debugprintf("desktop: %d %d %d %d\n", desk.g_x, desk.g_y, desk.g_w, desk.g_h);
			open_screen();
			
			draw_all_trees(file);
			
			close_screen();
			
			appl_exit();
			
			xrsrc_free(file);
		} else
		{
			exit_status = EXIT_FAILURE;
		}
	}
	
	return exit_status;
}
