#ifndef __WRITEBMP_H__
#define __WRITEBMP_H__ 1

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

typedef struct _writebmp_info {
	long width;
	long height;
	unsigned long rowbytes;
	FILE *outfile;
	const unsigned char *image_data;
	int bpp;
	int num_palette;
	struct {
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	} palette[256];
	unsigned char *buf;
} writebmp_info;


/* prototypes for public functions in writebmp.c */

writebmp_info *writebmp_new(void);
void writebmp_exit(writebmp_info *wbmpinfo);

int writebmp_output(writebmp_info *wbmpinfo);

void writebmp_cleanup(writebmp_info *wbmpinfo);

#endif /* __WRITEBMP_H__ */
