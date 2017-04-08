#include "config.h"
#include <gem.h>
#include <w_draw.h>
#include <getopt.h>
#include <errno.h>
#include "nls.h"
#include "fileio.h"
#include "ws.h"


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
static _WORD xdesk, ydesk, hdesk, wdesk;
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
		file = xrsrc_load(filename, XRSC_SAFETY_CHECKS);
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
			wind_get(DESK, WF_WORKXYWH, &xdesk, &ydesk, &wdesk, &hdesk);
			open_screen();
			
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
