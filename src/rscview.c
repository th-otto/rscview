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
	{ "version", no_argument, NULL, 'V' }
};


static void usage(FILE *fp)
{
	fprintf(fp, _("Usage: %s <file...>\n"), program_name);
}


static void print_version(void)
{
}


int main(int argc, char **argv)
{
	int c;
	RSCFILE *file;
	const char *filename;
	
	while ((c = getopt_long_only(argc, argv, "V", long_options, NULL)) != EOF)
	{
		switch (c)
		{
		case 'V':
			print_version();
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
		file = xrsrc_load(filename, 0);
		if (file == NULL)
		{
			fprintf(stderr, _("%s: %s\n"), program_name, strerror(errno));
		} else
		{
			char filename[PATH_MAX];
			
			strcpy(filename, file->rsc_rsxfilename);
			set_extension(filename, "xml");
			xrsrc_free(file);
		}
	}
	
	return 0;
}
