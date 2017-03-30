#include "config.h"
#include <gem.h>
#include <w_draw.h>
#include <getopt.h>
#include <errno.h>
#include "nls.h"
#include "fileio.h"

char const program_name[] = "rscview";
char const program_version[] = VERSION;
char const program_date[] = "28.03.2017";

static struct option const long_options[] = {
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
	
	while ((c = getopt_long_only(argc, argv, "hV", long_options, NULL)) != EOF)
	{
		switch (c)
		{
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
			char outfilename[PATH_MAX];
			
			strcpy(outfilename, file->rsc_rsxfilename);
			set_extension(outfilename, "xml");
			if (rsc_xml_source(file, &counter, outfilename, file->data) == FALSE)
				exit_status = EXIT_FAILURE;
			xrsrc_free(file);
		} else
		{
			exit_status = EXIT_FAILURE;
		}
	}
	
	return exit_status;
}
