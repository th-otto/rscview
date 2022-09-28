#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include "writebmp.h"					/* typedefs, common macros, public prototypes */
#include "debug.h"


#define BMP_STD_HSIZE 40

typedef struct {
	/* file header */
	unsigned char magic[2];		/* BM */
	unsigned char filesize[4];
	unsigned char xHotSpot[2];     
	unsigned char yHotSpot[2];
	unsigned char offbits[4];		/* offset to data */

	/* info header */
	union {
		struct {
			unsigned char hsize[4];			/* size of info header (40) */
			unsigned char width[4];			/* width in pixels */
			unsigned char height[4];		/* height in pixels */
			unsigned char planes[2];		/* always 1 */
			unsigned char bitcount[2];		/* bits per pixel: 1, 4, 8, 24 or 32 */
			unsigned char compression[4];	/* compression method */
#define BMP_RGB 0
#define BMP_RLE8 1
#define BMP_RLE4 2
#define BMP_BITFIELDS 3
#define BMP_JPEG 4
#define BMP_PNG 5
			unsigned char sizeImage[4];		/* size of data */
			unsigned char xPelsPerMeter[4];
			unsigned char yPelsPerMeter[4];
			unsigned char clrUsed[4];		/* # of colors used */
			unsigned char clrImportant[4];	/* # of important colors */
			unsigned char RedMask[4];
			unsigned char GreenMask[4];
			unsigned char BlueMask[4];
			unsigned char AlphaMask[4];
		} bitmapinfoheader;
		struct {
			unsigned char hsize[4];			/* size of info header (12) */
			unsigned char width[2];			/* width in pixels */
			unsigned char height[2];		/* height in pixels */
			unsigned char planes[2];		/* always 1 */
			unsigned char bitcount[2];		/* bits per pixel: 1, 4, 8, 24 or 32 */
		} bitmapcoreheader;
	} bmp_info_header;
} BMP_HEADER;


#define BMP_HEADER_SIZE (14 + 64)
#define BMP_HEADER_BUFSIZE (BMP_HEADER_SIZE + 256 * 4)


#define put_long(l) \
	*buf++ = (unsigned char)((l)	   ); \
	*buf++ = (unsigned char)((l) >>  8); \
	*buf++ = (unsigned char)((l) >> 16); \
	*buf++ = (unsigned char)((l) >> 24)
#define put_word(w) \
	*buf++ = (unsigned char)((w)	   ); \
	*buf++ = (unsigned char)((w) >>  8)
#define put_byte(b) \
	*buf++ = (unsigned char)(b)

#define get_long() \
	(((unsigned long)(buf[3]) << 24) | \
	 ((unsigned long)(buf[2]) << 16) | \
	 ((unsigned long)(buf[1]) <<  8) | \
	 ((unsigned long)(buf[0])		)), buf += 4
#define get_word() \
	(((unsigned short)(buf[1]) <<  8) | \
	 ((unsigned short)(buf[0])		)), buf += 2
#define get_byte() \
	*buf++


/******************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/******************************************************************************/

writebmp_info *writebmp_new(void)
{
	writebmp_info *wbmpinfo = (writebmp_info *)malloc(sizeof(*wbmpinfo));
	
	if (wbmpinfo == NULL)
		return NULL;
	memset(wbmpinfo, 0, sizeof(*wbmpinfo));

	return wbmpinfo;
}

/*** ---------------------------------------------------------------------- ***/

void writebmp_exit(writebmp_info *wbmpinfo)
{
	writebmp_cleanup(wbmpinfo);
	free(wbmpinfo);
}

/*** ---------------------------------------------------------------------- ***/

static long bmp_rowsize(long width, int planes)
{
	long bmp_bytes = width;
	
	switch (planes)
	{
	case 1:
		/* get_dib_stride */
		bmp_bytes = ((((bmp_bytes) + 7) >> 3) + 3) & ~3;
		break;
	case 4:
		bmp_bytes = (((bmp_bytes + 1) >> 1) + 3) & ~3;
		break;
	case 8:
		bmp_bytes = (bmp_bytes + 3) & ~3;
		break;
	case 24:
		bmp_bytes = ((bmp_bytes * 3) + 3) & ~3;
		break;
	case 32:
		bmp_bytes = bmp_bytes << 2;
		break;
	default:
		bmp_bytes = 0;
		break;
	}
	return bmp_bytes;
}

/*** ---------------------------------------------------------------------- ***/

static unsigned char *bmp_put_palette(unsigned char *buf, writebmp_info *wbmpinfo)
{
	short ncolors, i;

	ncolors = wbmpinfo->num_palette;

	switch (wbmpinfo->bpp)
	{
	case 8:
		for (i = 0; i < ncolors; i++)
		{
			put_byte(wbmpinfo->palette[i].blue);
			put_byte(wbmpinfo->palette[i].green);
			put_byte(wbmpinfo->palette[i].red);
			put_byte(0);
		}
		break;
	case 4:
		for (i = 0; i < ncolors; i++)
		{
			put_byte(wbmpinfo->palette[i].blue);
			put_byte(wbmpinfo->palette[i].green);
			put_byte(wbmpinfo->palette[i].red);
			put_byte(0);
		}
		break;
	case 1:
		for (i = 0; i < ncolors; i++)
		{
			put_byte(wbmpinfo->palette[i].blue);
			put_byte(wbmpinfo->palette[i].green);
			put_byte(wbmpinfo->palette[i].red);
			put_byte(0);
		}
		break;
	}
	
	return buf;
}

/*** ---------------------------------------------------------------------- ***/

static int writebmp_init(writebmp_info *wbmpinfo, unsigned char **bufp)
{
	unsigned char *buf;
	int ncolors;
	long bmp_bytes;
	long datasize;
	unsigned long cmapsize;
	unsigned long len, headlen;
	
	if (wbmpinfo->bpp <= 8)
	{
	} else if (wbmpinfo->bpp == 24)
	{
	} else if (wbmpinfo->bpp == 32)
	{
	} else
	{
		return EINVAL;
	}

	ncolors = wbmpinfo->num_palette != 0 && wbmpinfo->bpp <= 8 ? wbmpinfo->num_palette : 0;
	bmp_bytes = bmp_rowsize(wbmpinfo->width, wbmpinfo->bpp);
	datasize = bmp_bytes * wbmpinfo->height;
	cmapsize = 4 * ncolors;
	headlen = 14 + BMP_STD_HSIZE + cmapsize;
	len = headlen + datasize;

	wbmpinfo->buf = (unsigned char *)malloc(len);
	if (wbmpinfo->buf == NULL)
		return errno;
	buf = wbmpinfo->buf;
	put_byte('B');
	put_byte('M');
	put_long(len);
	put_long(0l);
	put_long(headlen);

	put_long(BMP_STD_HSIZE);        /* biSize */
	put_long(wbmpinfo->width);      /* biWidth */
	put_long(wbmpinfo->height);     /* biHeight */
	put_word(1);					/* biPlanes */
	put_word(wbmpinfo->bpp);		/* biBitsPerPixel */
	put_long(BMP_RGB);              /* biCompression */
	put_long(datasize);             /* biSizeImage */
	put_long(0l);					/* biXPelsPerMeter */
	put_long(0l);					/* biYPelsPerMeter */
	put_long((long)(ncolors));      /* biClrUsed */
	put_long(0l);                   /* biClrImportant */
	
	if (ncolors != 0)
	{
		buf = bmp_put_palette(buf, wbmpinfo);
	}
	
	*bufp = buf;
	
	return 0;
}

/*** ---------------------------------------------------------------------- ***/

int writebmp_output(writebmp_info *wbmpinfo)
{
	unsigned long srcrowbytes;
	int rc;
	unsigned char *buf = NULL;
	size_t len;
	
	if (wbmpinfo == NULL || wbmpinfo->image_data == NULL || wbmpinfo->outfile == NULL)
		return EINVAL;
	if ((rc = writebmp_init(wbmpinfo, &buf)) != 0)
		return rc;

	/* calculate rowbytes on basis of image type; note that this becomes much
	 * more complicated if we choose to support PBM type, ASCII PNM types, or
	 * 16-bit-per-sample binary data [currently not an official NetPBM type] */

	srcrowbytes = wbmpinfo->rowbytes;
	if (srcrowbytes == 0)
	{
		if (wbmpinfo->bpp <= 8)
			srcrowbytes = wbmpinfo->width;
		else if (wbmpinfo->bpp == 24)
			srcrowbytes = wbmpinfo->width * 3;
		else
			srcrowbytes = wbmpinfo->width * 4;
	}
	
	rc = 0;
	{
		long y;
		const unsigned char *src = wbmpinfo->image_data + wbmpinfo->height * srcrowbytes;
		unsigned char *dst = buf;
		long bmp_bytes = bmp_rowsize(wbmpinfo->width, wbmpinfo->bpp);
		
		for (y = wbmpinfo->height; y > 0; --y)
		{
			src -= srcrowbytes;
			memcpy(dst, src, bmp_bytes);
			dst += bmp_bytes;
		}
		len = dst - wbmpinfo->buf;
	}

	if (fwrite(wbmpinfo->buf, 1, len, wbmpinfo->outfile) != len)
		return errno;
	return rc;
}

/*** ---------------------------------------------------------------------- ***/

void writebmp_cleanup(writebmp_info *wbmpinfo)
{
	if (wbmpinfo)
	{
		free(wbmpinfo->buf);
		wbmpinfo->buf = NULL;
	}
}
