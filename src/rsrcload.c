#include "config.h"
#include <gem.h>
#include <s_endian.h>
#include <debug.h>
#include <xrsrc.h>
#include <rsrcload.h>
#include <ro_mem.h>
#include <w_draw.h>


#ifndef O_RDONLY
#  define O_RDONLY 0
#endif

#define SWAP_ALLOWED 1


static char empty[1];
static _WORD imdata[1];
static TEDINFO empty_ted = {
	empty,
	empty,
	empty,
	IBM,
	0,
	TE_LEFT,
	COLSPEC_MAKE(G_BLACK, G_BLACK, IP_HOLLOW, 0, G_WHITE),
	0,
	0,
	1,
	1
};
static BITBLK empty_bit = {
	imdata,
	2,
	1,
	0,
	0,
	G_BLACK
};
static ICONBLK empty_icon = {
	imdata,
	imdata,
	empty,
	ICOLSPEC_MAKE(G_BLACK, G_WHITE, '\0'),
	0, 0,
	0, 0, 16, 1,
	0, 0, 7, 1
};

/******************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/******************************************************************************/

/*
 * xrsrc_gaddr: like rsrc_gaddr(), taking an extra parameter for the resource
 */
_BOOL xrsrc_gaddr(RSCFILE *file, _WORD type, _WORD idx, void *gaddr)
{
	if (gaddr == NULL)
		return FALSE;
	*(void **)gaddr = NULL;
	if (file == NULL)
	{
		return FALSE;
	}
	switch (type)
	{
	case R_TREE:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_ntree)
		{
			return FALSE;
		}
		*((OBJECT **)gaddr) = file->rs_trindex[idx];
		break;
	case R_OBJECT:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nobs)
		{
			return FALSE;
		}
		*((OBJECT **)gaddr) = &file->rs_object[idx];
		break;
	case R_TEDINFO:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nted)
		{
			return FALSE;
		}
		*((TEDINFO **)gaddr) = &file->rs_tedinfo[idx];
		break;
	case R_ICONBLK:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nib)
		{
			return FALSE;
		}
		*((ICONBLK **)gaddr) = &file->rs_iconblk[idx];
		break;
	case R_BITBLK:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nbb)
		{
			return FALSE;
		}
		*((BITBLK **)gaddr) = &file->rs_bitblk[idx];
		break;
	case R_STRING:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nstring)
		{
			return FALSE;
		}
		*((char **)gaddr) = file->rs_frstr[idx];
		break;
	case R_IMAGEDATA:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nimages)
		{
			return FALSE;
		}
		*((BITBLK **)gaddr) = file->rs_frimg[idx];
		break;
	case R_OBSPEC:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nobs)
		{
			return FALSE;
		}
		*((_LONG_PTR **)gaddr) = &file->rs_object[idx].ob_spec.index;
		break;
	case R_TEPTEXT:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nted)
		{
			return FALSE;
		}
		*((char ***)gaddr) = &file->rs_tedinfo[idx].te_ptext;
		break;
	case R_TEPTMPLT:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nted)
		{
			return FALSE;
		}
		*((char ***)gaddr) = &file->rs_tedinfo[idx].te_ptmplt;
		break;
	case R_TEPVALID:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nted)
		{
			return FALSE;
		}
		*((char ***)gaddr) = &file->rs_tedinfo[idx].te_pvalid;
		break;
	case R_IBPMASK:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nib)
		{
			return FALSE;
		}
		*((char ***)gaddr) = (char **)&file->rs_iconblk[idx].ib_pmask;
		break;
	case R_IBPDATA:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nib)
		{
			return FALSE;
		}
		*((char ***)gaddr) = (char **)&file->rs_iconblk[idx].ib_pdata;
		break;
	case R_IBPTEXT:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nib)
		{
			return FALSE;
		}
		*((char ***)gaddr) = &file->rs_iconblk[idx].ib_ptext;
		break;
	case R_BIPDATA:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nbb)
		{
			return FALSE;
		}
		*((char ***)gaddr) = (char **)&file->rs_bitblk[idx].bi_pdata;
		break;
	case R_FRSTR:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nstring)
		{
			return FALSE;
		}
		*((char ***)gaddr) = &file->rs_frstr[idx];
		break;
	case R_FRIMG:
		if (idx < 0 || (_ULONG)idx >= file->header.rsh_nimages)
		{
			return FALSE;
		}
		*((BITBLK ***)gaddr) = &file->rs_frimg[idx];
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL test_header(RS_HEADER *header, _LONG filesize)
{
	_UWORD vrsn;

	vrsn = header->rsh_vrsn & RSC_VERSION_MASK;
	if (vrsn != 0 && vrsn != 1)
		return FALSE;
	if (header->rsh_rssize > filesize)
		return FALSE;
#if 0
	/* some broken construction kits fail to put correct values here */
	if (header->rsh_rssize < (filesize - sizeof(*header)))
		return FALSE;
#endif
	if (header->rsh_object > filesize)
		return FALSE;
	if (header->rsh_tedinfo > filesize)
		return FALSE;
	if (header->rsh_iconblk > filesize)
		return FALSE;
	if (header->rsh_bitblk > filesize)
		return FALSE;
	if (header->rsh_frstr > filesize)
		return FALSE;
	if (header->rsh_string > filesize)
		return FALSE;
	if (header->rsh_imdata > filesize)
		return FALSE;
	if (header->rsh_frimg > filesize)
		return FALSE;
	if (header->rsh_trindex > filesize)
		return FALSE;
	if (header->rsh_nobs > 2339) /* (65536 - sizeof(RS_HEADER)) / (RSC_SIZEOF_OBJECT + sizeof(OBJECT *)) */
		return FALSE;
	if (header->rsh_ntree > 2339)
		return FALSE;
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL test_xrsc_header(XRS_HEADER *xrsc_header, _ULONG filesize)
{
	if ((xrsc_header->rsh_vrsn & RSC_VERSION_MASK) != 3)
		return FALSE;
	if (xrsc_header->rsh_rssize > filesize)
		return FALSE;
	if (xrsc_header->rsh_object > filesize)
		return FALSE;
	if (xrsc_header->rsh_tedinfo > filesize)
		return FALSE;
	if (xrsc_header->rsh_iconblk > filesize)
		return FALSE;
	if (xrsc_header->rsh_bitblk > filesize)
		return FALSE;
	if (xrsc_header->rsh_frstr > filesize)
		return FALSE;
	if (xrsc_header->rsh_string > filesize)
		return FALSE;
	if (xrsc_header->rsh_imdata > filesize)
		return FALSE;
	if (xrsc_header->rsh_frimg > filesize)
		return FALSE;
	if (xrsc_header->rsh_trindex > filesize)
		return FALSE;
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

void xrsc_hdr2xrsc(XRS_HEADER *xrsc_header, RS_HEADER *header, size_t diff_size)
{
	xrsc_header->rsh_vrsn	 = 3 | (header->rsh_vrsn & ~RSC_VERSION_MASK);
	xrsc_header->rsh_extvrsn = XRSC_VRSN_ORCS;
	xrsc_header->rsh_object  = header->rsh_object + diff_size;
	xrsc_header->rsh_tedinfo = header->rsh_tedinfo + diff_size;
	xrsc_header->rsh_iconblk = header->rsh_iconblk + diff_size;
	xrsc_header->rsh_bitblk  = header->rsh_bitblk + diff_size;
	xrsc_header->rsh_frstr	 = header->rsh_frstr + diff_size;
	xrsc_header->rsh_string  = header->rsh_string + diff_size;
	xrsc_header->rsh_imdata  = header->rsh_imdata + diff_size;
	xrsc_header->rsh_frimg	 = header->rsh_frimg + diff_size;
	xrsc_header->rsh_trindex = header->rsh_trindex + diff_size;
	xrsc_header->rsh_nobs	 = header->rsh_nobs;
	xrsc_header->rsh_ntree	 = header->rsh_ntree;
	xrsc_header->rsh_nted	 = header->rsh_nted;
	xrsc_header->rsh_nib	 = header->rsh_nib;
	xrsc_header->rsh_nbb	 = header->rsh_nbb;
	xrsc_header->rsh_nstring = header->rsh_nstring;
	xrsc_header->rsh_nimages = header->rsh_nimages;
	xrsc_header->rsh_rssize  = header->rsh_rssize + diff_size;
}

/*** ---------------------------------------------------------------------- ***/

void xrsc_xrsc2hdr(RS_HEADER *header, XRS_HEADER *xrsc_header, size_t diff_size)
{
	header->rsh_vrsn	= 0 | (xrsc_header->rsh_vrsn & ~RSC_VERSION_MASK);
	header->rsh_object	= (_UWORD)(xrsc_header->rsh_object - diff_size);
	header->rsh_tedinfo = (_UWORD)(xrsc_header->rsh_tedinfo - diff_size);
	header->rsh_iconblk = (_UWORD)(xrsc_header->rsh_iconblk - diff_size);
	header->rsh_bitblk	= (_UWORD)(xrsc_header->rsh_bitblk - diff_size);
	header->rsh_frstr	= (_UWORD)(xrsc_header->rsh_frstr - diff_size);
	header->rsh_string	= (_UWORD)(xrsc_header->rsh_string - diff_size);
	header->rsh_imdata	= (_UWORD)(xrsc_header->rsh_imdata - diff_size);
	header->rsh_frimg	= (_UWORD)(xrsc_header->rsh_frimg - diff_size);
	header->rsh_trindex = (_UWORD)(xrsc_header->rsh_trindex - diff_size);
	header->rsh_nobs	= (_UWORD)xrsc_header->rsh_nobs;
	header->rsh_ntree	= (_UWORD)xrsc_header->rsh_ntree;
	header->rsh_nted	= (_UWORD)xrsc_header->rsh_nted;
	header->rsh_nib     = (_UWORD)xrsc_header->rsh_nib;
	header->rsh_nbb     = (_UWORD)xrsc_header->rsh_nbb;
	header->rsh_nstring = (_UWORD)xrsc_header->rsh_nstring;
	header->rsh_nimages = (_UWORD)xrsc_header->rsh_nimages;
	header->rsh_rssize	= (_UWORD)(xrsc_header->rsh_rssize - diff_size);
}

/*** ---------------------------------------------------------------------- ***/

#if SWAP_ALLOWED

#define bswap_ptr(p) bswap_32((uint32_t)(p))

static void flip_header(RS_HEADER *header)
{
	header->rsh_vrsn = bswap_16(header->rsh_vrsn);
	header->rsh_object = bswap_16(header->rsh_object);
	header->rsh_tedinfo = bswap_16(header->rsh_tedinfo);
	header->rsh_iconblk = bswap_16(header->rsh_iconblk);
	header->rsh_bitblk = bswap_16(header->rsh_bitblk);
	header->rsh_frstr = bswap_16(header->rsh_frstr);
	header->rsh_string = bswap_16(header->rsh_string);
	header->rsh_imdata = bswap_16(header->rsh_imdata);
	header->rsh_frimg = bswap_16(header->rsh_frimg);
	header->rsh_trindex = bswap_16(header->rsh_trindex);
	header->rsh_nobs = bswap_16(header->rsh_nobs);
	header->rsh_ntree = bswap_16(header->rsh_ntree);
	header->rsh_nted = bswap_16(header->rsh_nted);
	header->rsh_nib = bswap_16(header->rsh_nib);
	header->rsh_nbb = bswap_16(header->rsh_nbb);
	header->rsh_nstring = bswap_16(header->rsh_nstring);
	header->rsh_nimages = bswap_16(header->rsh_nimages);
	header->rsh_rssize = bswap_16(header->rsh_rssize);
}

/*** ---------------------------------------------------------------------- ***/

#define FLIP_DATA 0

#if FLIP_DATA
static void flip_image(size_t words, _WORD *data)
{
	size_t j;

	for (j = 0; j < words; j++)
		data[j] = bswap_16(data[j]);
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL flip_data(RSCFILE *file)
{
	_UWORD i;
	size_t words;

	{
		/***** flip data and mask of iconblocks *****/
		ICONBLK *p;

		p = file->rs_iconblk;
		for (i = 0; i < file->header.rsh_nib; i++)
		{
			words = iconblk_masksize(p) >> 1;

			flip_image(words, p->ib_pmask);
			flip_image(words, p->ib_pdata);

			p++;
		}
	}

	{
		/***** flip data of bitblocks *****/
		BITBLK *pbitblk;

		pbitblk = file->rs_bitblk;
		for (i = 0; i < file->header.rsh_nbb; i++)
		{
			words = bitblk_datasize(pbitblk) >> 1;
			flip_image(words, pbitblk[i].bi_pdata);
		}
	}

	return TRUE;
}
#endif

/*** ---------------------------------------------------------------------- ***/

static __inline__ uint16_t get_word(const char *p)
{
	return *((const uint16_t *)(p));
}

/*** ---------------------------------------------------------------------- ***/

static __inline__ uint32_t get_long(const char *p)
{
	return *((const uint32_t *)(p));
}

/*** ---------------------------------------------------------------------- ***/

static __inline__ void swap_word(char *p)
{
	*((uint16_t *)(p)) = bswap_16(*((uint16_t *)(p)));
}

/*** ---------------------------------------------------------------------- ***/

static __inline__ void swap_long(char *p)
{
	*((uint32_t *)(p)) = bswap_32(*((uint32_t *)(p)));
}

/*** ---------------------------------------------------------------------- ***/

static void xrsc_get_header(XRS_HEADER *xrsc_header, const char *buf)
{
	xrsc_header->rsh_vrsn = get_word(buf + 0x00);
	xrsc_header->rsh_extvrsn = get_word(buf + 0x02);
	xrsc_header->rsh_object = get_long(buf + 0x04);
	xrsc_header->rsh_tedinfo = get_long(buf + 0x08);
	xrsc_header->rsh_iconblk = get_long(buf + 0x0c);
	xrsc_header->rsh_bitblk = get_long(buf + 0x10);
	xrsc_header->rsh_frstr = get_long(buf + 0x14);
	xrsc_header->rsh_string = get_long(buf + 0x18);
	xrsc_header->rsh_imdata = get_long(buf + 0x1c);
	xrsc_header->rsh_frimg = get_long(buf + 0x20);
	xrsc_header->rsh_trindex = get_long(buf + 0x24);
	xrsc_header->rsh_nobs = get_long(buf + 0x28);
	xrsc_header->rsh_ntree = get_long(buf + 0x2c);
	xrsc_header->rsh_nted = get_long(buf + 0x30);
	xrsc_header->rsh_nib = get_long(buf + 0x34);
	xrsc_header->rsh_nbb = get_long(buf + 0x38);
	xrsc_header->rsh_nstring = get_long(buf + 0x3c);
	xrsc_header->rsh_nimages = get_long(buf + 0x40);
	xrsc_header->rsh_rssize = get_long(buf + 0x44);
}

/*** ---------------------------------------------------------------------- ***/

static void rsc_get_header(RS_HEADER *header, const char *buf)
{
	header->rsh_vrsn = get_word(buf + 0x00);
	header->rsh_object = get_word(buf + 0x02);
	header->rsh_tedinfo = get_word(buf + 0x04);
	header->rsh_iconblk = get_word(buf + 0x06);
	header->rsh_bitblk = get_word(buf + 0x08);
	header->rsh_frstr = get_word(buf + 0x0a);
	header->rsh_string = get_word(buf + 0x0c);
	header->rsh_imdata = get_word(buf + 0x0e);
	header->rsh_frimg = get_word(buf + 0x10);
	header->rsh_trindex = get_word(buf + 0x12);
	header->rsh_nobs = get_word(buf + 0x14);
	header->rsh_ntree = get_word(buf + 0x16);
	header->rsh_nted = get_word(buf + 0x18);
	header->rsh_nib = get_word(buf + 0x1a);
	header->rsh_nbb = get_word(buf + 0x1c);
	header->rsh_nstring = get_word(buf + 0x1e);
	header->rsh_nimages = get_word(buf + 0x20);
	header->rsh_rssize = get_word(buf + 0x22);
}

/*** ---------------------------------------------------------------------- ***/

static void flip_iconblk(char *blk)
{
	swap_long(blk + 0);
	swap_long(blk + 4);
	swap_long(blk + 8);
	swap_word(blk + 12);
	swap_word(blk + 14);
	swap_word(blk + 16);
	swap_word(blk + 18);
	swap_word(blk + 20);
	swap_word(blk + 22);
	swap_word(blk + 24);
	swap_word(blk + 26);
	swap_word(blk + 28);
	swap_word(blk + 30);
	swap_word(blk + 32);
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL flip_rsc(XRS_HEADER *header, char *rsc_buffer)
{
	_UWORD i;

	{
		/***** fix objects *****/
		char *obj;

		obj = rsc_buffer + header->rsh_object;
		for (i = 0; i < header->rsh_nobs; i++, obj += RSC_SIZEOF_OBJECT)
		{
			swap_word(obj + 0);
			swap_word(obj + 2);
			swap_word(obj + 4);
			swap_word(obj + 6);
			swap_word(obj + 8);
			swap_word(obj + 10);
			swap_long(obj + 12);
			swap_word(obj + 16);
			swap_word(obj + 18);
			swap_word(obj + 20);
			swap_word(obj + 22);
			if ((get_word(obj + 6) & OBTYPEMASK) == G_USERDEF)
			{
				/* swap userblk.ub_parm */
				char *user = rsc_buffer + (size_t)get_long(obj + 12);
				swap_long(user + 4);
			}
		}
	}

	{
		/***** flip tedinfos *****/
		char *ted;

		ted = rsc_buffer + header->rsh_tedinfo;
		for (i = 0; i < header->rsh_nted; i++, ted += RSC_SIZEOF_TEDINFO)
		{
			swap_long(ted + 0);
			swap_long(ted + 4);
			swap_long(ted + 8);
			swap_word(ted + 12);
			swap_word(ted + 14);
			swap_word(ted + 16);
			swap_word(ted + 18);
			swap_word(ted + 20);
			swap_word(ted + 22);
			swap_word(ted + 24);
			swap_word(ted + 26);
		}
	}

	{
		/***** flip iconblocks *****/
		char *p;

		p = rsc_buffer + header->rsh_iconblk;
		for (i = 0; i < header->rsh_nib; i++, p += RSC_SIZEOF_ICONBLK)
		{
			flip_iconblk(p);
		}
	}

	{
		/***** flip bitblocks *****/
		char *bit;

		bit = rsc_buffer + header->rsh_bitblk;
		for (i = 0; i < header->rsh_nbb; i++, bit += RSC_SIZEOF_BITBLK)
		{
			swap_long(bit + 0);
			swap_word(bit + 4);
			swap_word(bit + 6);
			swap_word(bit + 8);
			swap_word(bit + 10);
			swap_word(bit + 12);
		}
	}

	{
		/***** flip free strings *****/
		char *pfrstr;

		pfrstr = rsc_buffer + header->rsh_frstr;
		for (i = 0; i < header->rsh_nstring; i++, pfrstr += RSC_SIZEOF_PTR)
		{
			swap_long(pfrstr);
		}
	}

	{
		/***** flip free images *****/
		char *pfrimg;

		pfrimg = rsc_buffer + header->rsh_frimg;
		for (i = 0; i < header->rsh_nimages; i++, pfrimg += RSC_SIZEOF_PTR)
		{
			swap_long(pfrimg);
		}
	}

	{
		/***** flip trees *****/
		char *ptrindex;

		ptrindex = rsc_buffer + header->rsh_trindex;
		for (i = 0; i < header->rsh_ntree; i++, ptrindex += RSC_SIZEOF_PTR)
		{
			swap_long(ptrindex);
		}
	}

	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL intel_2_m68k(XRS_HEADER *xrsc_header, char *rsc_buffer)
{
#if 0
	/* fix font from IBM to ATARI */
	char *pstring;
	char *pend;
	_ULONG end;
	_ULONG start;

	start = xrsc_header->rsh_string;
	end = xrsc_header->rsh_rssize;
	if (xrsc_header->rsh_object  < end && xrsc_header->rsh_object  > start)
		end = xrsc_header->rsh_object;
	if (xrsc_header->rsh_tedinfo < end && xrsc_header->rsh_tedinfo > start)
		end = xrsc_header->rsh_tedinfo;
	if (xrsc_header->rsh_iconblk < end && xrsc_header->rsh_iconblk > start)
		end = xrsc_header->rsh_iconblk;
	if (xrsc_header->rsh_bitblk  < end && xrsc_header->rsh_bitblk  > start)
		end = xrsc_header->rsh_bitblk;
	if (xrsc_header->rsh_frstr	 < end && xrsc_header->rsh_frstr   > start)
		end = xrsc_header->rsh_frstr;
	if (xrsc_header->rsh_imdata  < end && xrsc_header->rsh_imdata  > start)
		end = xrsc_header->rsh_imdata;
	if (xrsc_header->rsh_frimg	 < end && xrsc_header->rsh_frimg   > start)
		end = xrsc_header->rsh_frimg;
	if (xrsc_header->rsh_trindex < end && xrsc_header->rsh_trindex > start)
		end = xrsc_header->rsh_trindex;
	pstring = (char *)(rsc_buffer + (size_t)start);
	pend = (char *)(rsc_buffer + (size_t)end);
	while (pstring < pend)
	{
		switch (*pstring)
		{
		case 0x15:
			*pstring = 0xdd; /* change paragraph from IBM to ATARI ST font */
			break;
		case 0xe1:
			*pstring = 0x9e; /* change sz from IBM to ATARI ST font */
			break;
		}
		pstring++;
	} /* while */
#endif
	return flip_rsc(xrsc_header, rsc_buffer);
}
#endif /* SWAP_ALLOWED */

/*** ---------------------------------------------------------------------- ***/

static void rsc_obfix(OBJECT *tree, _ULONG count)
{
	_WORD wchar, hchar;

	GetTextSize(&wchar, &hchar);
	/* Koordinaten fuer alle Objekte umrechnen */
	while (count)
	{
		tree->ob_x = wchar * (tree->ob_x & 0xff) + (tree->ob_x >> 8);
		tree->ob_y = hchar * (tree->ob_y & 0xff) + (tree->ob_y >> 8);
		tree->ob_width = wchar * (tree->ob_width & 0xff) + (tree->ob_width >> 8);
		tree->ob_height = hchar * (tree->ob_height & 0xff) + (tree->ob_height >> 8);
		tree++;
		count--;
	}
}

/*** ---------------------------------------------------------------------- ***/

/*
 * check wether we need to convert the resource
 * when loading into memory
 */
static _BOOL rsrc_load_works(void)
{
/*
 * we assume that it works for PureC;
 * avoid the warning from not being able
 * to optimize out the constant checks
 */
#if !defined(__PUREC__)
#define check_size(struct, size) \
	if (sizeof(struct) != size) \
	{ \
		return FALSE; \
	}
	check_size(TEDINFO, RSC_SIZEOF_TEDINFO);
	check_size(ICONBLK, RSC_SIZEOF_ICONBLK);
	check_size(BITBLK, RSC_SIZEOF_BITBLK);
	check_size(CICON, RSC_SIZEOF_CICON);
	check_size(CICONBLK, RSC_SIZEOF_CICONBLK);
	check_size(USERBLK, RSC_SIZEOF_USERBLK);
	check_size(OBJECT, RSC_SIZEOF_OBJECT);
	check_size(OBJECT *, RSC_SIZEOF_PTR);
#endif
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _WORD _CDECL draw_userdef(PARMBLK *pb)
{
	UNUSED(pb);
	return 0;
}

/*** ---------------------------------------------------------------------- ***/

/*
 * xrsrc_load: like rsrc_load()
 *
 * Traegt die Addresse der Objekt-Baeume ins global-Feld ein
 * und rechnet die Koordinaten auf die Aufloesung um.
 */
RSCFILE *xrsrc_load(const char *filename, _UWORD flags)
{
	register _UWORD UObj;
	CICONBLK *cicon_p;
	CICONBLK *cicon_dst;
	char headerbuf[max(RSC_SIZEOF_XRS_HEADER, RSC_SIZEOF_RS_HEADER)];
	RS_HEADER rs_header;
	XRS_HEADER xrsc_header;
	char *buf = NULL;
	_BOOL swap_flag = FALSE;
	_BOOL not_rsc = FALSE;
	FILE *fp;
	_ULONG filesize;
	RSCFILE *file;
	_ULONG n_ciconblks;
	_ULONG n_cicons;
	_ULONG n_userblks;
	
	cicon_p = NULL;
	cicon_dst = NULL;
	fp = fopen(filename, "rb");

	if (fp == NULL)
		return NULL;
#ifdef HAVE_FSTAT
	{
		struct stat st;
		
		if (fstat(fileno(fp), &st) < 0)
			filesize = 0, not_rsc = TRUE;
		else
			filesize = st.st_size;
	}
#else
	if (fseek(fp, 0l, SEEK_END) != 0)
		filesize = 0, not_rsc = TRUE;
	else
		filesize = ftell(fp);
	if (fseek(fp, 0l, SEEK_SET) != 0)
		not_rsc = TRUE;
#endif

	if (not_rsc || fread(headerbuf, 1, sizeof(headerbuf), fp) != sizeof(headerbuf))
	{
		not_rsc = TRUE;
	} else
	{
		rsc_get_header(&rs_header, headerbuf);
		xrsc_get_header(&xrsc_header, headerbuf);
		if (test_header(&rs_header, filesize) == FALSE)
		{
			if (test_xrsc_header(&xrsc_header, filesize) == FALSE)
			{
#if SWAP_ALLOWED
				flip_header(&rs_header);
				if (test_header(&rs_header, filesize) != FALSE)
				{
					if (filesize > 65536l && !(rs_header.rsh_vrsn & RSC_EXT_FLAG))
					{
						not_rsc = TRUE;
					} else
					{
						swap_flag = TRUE;
						xrsc_hdr2xrsc(&xrsc_header, &rs_header, 0);
					}
				} else
#endif
				{
					not_rsc = TRUE;
				}
			}
		} else
		{
			if (filesize > 65536l && !(rs_header.rsh_vrsn & RSC_EXT_FLAG))
			{
				not_rsc = TRUE;
			} else
			{
				xrsc_hdr2xrsc(&xrsc_header, &rs_header, 0);
			}
		}
	}
	if (not_rsc ||
		test_xrsc_header(&xrsc_header, filesize) == FALSE ||
		fseek(fp, 0l, SEEK_SET) != 0 ||
		(sizeof(size_t) <= 2 && filesize >= 65534l) ||
		(buf = (unsigned char *)g_malloc(sizeof(RSCFILE) + (size_t)filesize)) == NULL ||
		fread(buf + sizeof(RSCFILE), 1, (size_t)filesize, fp) != filesize
#if SWAP_ALLOWED
		|| (swap_flag != FALSE && intel_2_m68k(&xrsc_header, buf) == FALSE)
#endif
		)
	{
		g_free(buf);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	file = (RSCFILE *)buf;
	memset(file, 0, sizeof(RSCFILE));
	file->data = buf;
	buf += sizeof(RSCFILE);
	file->header = xrsc_header;
	file->swap_flag = swap_flag;
	
	/*
	 * Some resource editors fail to mark an extended RSC in the header,
	 * do a quick check first wether any color icons are present
	 */
	n_ciconblks = 0;
	n_userblks = 0;
	{
		char *pobject;
		_ULONG i;
		_UBYTE type;
		
		pobject = buf + (size_t)xrsc_header.rsh_object;
		for (i = 0; i < xrsc_header.rsh_nobs; i++, pobject += RSC_SIZEOF_OBJECT)
		{
			type = (swap_flag ^ (HOST_BYTE_ORDER == BYTE_ORDER_BIG_ENDIAN)) ? pobject[7] : pobject[6];
			if (type == G_CICON)
			{
				xrsc_header.rsh_vrsn |= RSC_EXT_FLAG;
				n_ciconblks++;
			}
			if (type == G_USERDEF)
				n_userblks++;
		}
	}

	if (xrsc_header.rsh_vrsn & RSC_EXT_FLAG)
	{
		int32_t *p;
		_BOOL ok = TRUE;

		/*
		 * an extended resource has a list of extension ptrs
		 * located at the standard rsh_rssize offset.
		 * It has at least:
		 * - the real filesize
		 * - an offset to the CICON ptr list
		 * - an endmarker (zero)
		 */
		p = (int32_t *)(buf + (size_t)xrsc_header.rsh_rssize);
		if (swap_flag)
		{
			p[RSC_EXT_FILESIZE] = bswap_32(p[RSC_EXT_FILESIZE]);
			p[RSC_EXT_CICONBLK] = bswap_32(p[RSC_EXT_CICONBLK]);
		}
		if ((uint32_t)p[RSC_EXT_FILESIZE] != filesize)
		{
			ok = FALSE;
		} else if (p[RSC_EXT_CICONBLK] < 0 || (uint32_t)p[RSC_EXT_CICONBLK] >= filesize)
		{
			ok = FALSE;
		} else
		{
			int32_t *cp = p + RSC_EXT_CICONBLK + 1;
			
			/*
			 * check wether there is also a palette
			 */
			while (ok != FALSE)
			{
				if ((uint32_t)((char *)cp - buf) >= filesize)
				{
					ok = FALSE;
				} else if (*cp == 0)
				{
					break;
				} else if (*cp != -1)
				{
					if (swap_flag)
						*cp = bswap_32(*cp);
					/* ok = FALSE; */
					if (cp == (p + RSC_EXT_PALETTE))
					{
						_WORD *palette = (_WORD *)(buf + (size_t)(*cp));
						if (swap_flag)
						{
							_WORD i;

							for (i = 0; i < 1024; i++)
							{
								palette[i] = bswap_16(palette[i]);
							}
						}
						W_Cicon_Setpalette(palette);
					}
					break;
				}
				cp++;
			}
		}

		/*
		 * the CICONBLK slot of the extensions
		 * points to an empty list of slots to store
		 * the starting offset of the color icons.
		 * It is terminated by -1
		 */
		if (ok != FALSE)
		{
			p = (int32_t *)(buf + (size_t)p[RSC_EXT_CICONBLK]);
			while (ok != FALSE)
			{
				if ((uint32_t)((char *)p - buf) >= filesize)
					ok = FALSE;
				else if (*p == -1)
					break;
				else if (*p != 0)
					ok = FALSE;
				else
					p++;
			}
			if (ok != FALSE && *p == -1)
			{
				/*
 				 * The CICONBLK structures immediately follow this list
				 */
				p++;
				cicon_p = (CICONBLK *)p;
			}
		}
		if (ok == FALSE)
		{
			xrsrc_free(file);
			return NULL;
		}
	}

	if (rsrc_load_works())
	{
		/*
		 * simple case, we just have to translate file offsets in memory addresses
		 */
		file->rs_trindex = (OBJECT **)(buf + (size_t)file->header.rsh_trindex);
		file->rs_object = (OBJECT *)(buf + (size_t)file->header.rsh_object);
		file->rs_tedinfo = (TEDINFO *)(buf + (size_t)file->header.rsh_tedinfo);
		file->rs_iconblk = (ICONBLK *)(buf + (size_t)file->header.rsh_iconblk);
		file->rs_bitblk = (BITBLK *)(buf + (size_t)file->header.rsh_bitblk);
		file->rs_frstr = (char **)(buf + (size_t)file->header.rsh_frstr);
		file->rs_frimg = (BITBLK **)(buf + (size_t)file->header.rsh_frimg);
		file->rs_ciconblk = cicon_p;

		{
			OBJECT **rs_trindex;

			rs_trindex = file->rs_trindex;
			for (UObj = 0; UObj < xrsc_header.rsh_ntree; UObj++, rs_trindex++)
			{
				*rs_trindex = (OBJECT *)(buf + (uintptr_t)(*rs_trindex));
			}
		}
	
		{
			TEDINFO *rs_tedinfo;
	
			rs_tedinfo = file->rs_tedinfo;
			for (UObj = 0; UObj < xrsc_header.rsh_nted; UObj++, rs_tedinfo++)
			{
				rs_tedinfo->te_ptext += (uintptr_t)buf;
				rs_tedinfo->te_ptmplt += (uintptr_t)buf;
				rs_tedinfo->te_pvalid += (uintptr_t)buf;
			}
		}

		{
			ICONBLK *rs_iconblk;

			rs_iconblk = file->rs_iconblk;
			for (UObj = 0; UObj < xrsc_header.rsh_nib; UObj++, rs_iconblk++)
			{
				rs_iconblk->ib_pmask = (_WORD *)(buf + (uintptr_t)(rs_iconblk->ib_pmask));
				rs_iconblk->ib_pdata = (_WORD *)(buf + (uintptr_t)(rs_iconblk->ib_pdata));
				rs_iconblk->ib_ptext = buf + (uintptr_t)rs_iconblk->ib_ptext;
				W_Fix_Bitmap(rs_iconblk->ib_pmask, rs_iconblk->ib_wicon, rs_iconblk->ib_hicon, 1);
				W_Fix_Bitmap(rs_iconblk->ib_pdata, rs_iconblk->ib_wicon, rs_iconblk->ib_hicon, 1);
			}
		}

		{
			BITBLK *rs_bitblk;
	
			rs_bitblk = file->rs_bitblk;
			for (UObj = 0; UObj < xrsc_header.rsh_nbb; UObj++, rs_bitblk++)
			{
				rs_bitblk->bi_pdata = (_WORD *)(buf + (uintptr_t)(rs_bitblk->bi_pdata));
				W_Fix_Bitmap(rs_bitblk->bi_pdata, rs_bitblk->bi_wb * 8, rs_bitblk->bi_hl, 1);
			}
		}

		{
			char **rs_frstr;
	
			rs_frstr = file->rs_frstr;
			for (UObj = 0; UObj < xrsc_header.rsh_nstring; UObj++, rs_frstr++)
			{
				*rs_frstr += (uintptr_t)buf;
			}
		}

		{
			BITBLK **rs_frimg;

			rs_frimg = file->rs_frimg;
			for (UObj = 0; UObj < xrsc_header.rsh_nimages; UObj++, rs_frimg++)
			{
				*rs_frimg = (BITBLK *)(buf + (uintptr_t)(*rs_frimg));
			}
		}

	} else
	{
		/*
		 * we have to do it the hard way
		 */
		if (file->header.rsh_ntree > 0)
		{
			char *src;
			OBJECT **dst;
			
			file->rs_trindex = g_new(OBJECT *, file->header.rsh_ntree);
			if (file->rs_trindex == NULL)
			{
				xrsrc_free(file);
				return NULL;
			}
			file->allocated |= RSC_ALLOC_TRINDEX;
			src = buf + (size_t)file->header.rsh_trindex;
			dst = file->rs_trindex;
			for (UObj = 0; UObj < xrsc_header.rsh_ntree; UObj++, src += RSC_SIZEOF_PTR, dst++)
			{
				*dst = (OBJECT *)(buf + (size_t)get_long(src));
			}
		}
		
		if (file->header.rsh_nobs > 0)
		{
			char *src;
			OBJECT *dst;
			
			file->rs_object = g_new(OBJECT, file->header.rsh_nobs);
			if (file->rs_object == NULL)
			{
				xrsrc_free(file);
				return NULL;
			}
			file->allocated |= RSC_ALLOC_OBJECT;
			src = buf + (size_t)file->header.rsh_object;
			dst = file->rs_object;
			for (UObj = 0; UObj < xrsc_header.rsh_nobs; UObj++, src += RSC_SIZEOF_OBJECT, dst++)
			{
				dst->ob_next = get_word(src + 0);
				dst->ob_head = get_word(src + 2);
				dst->ob_tail = get_word(src + 4);
				dst->ob_type = get_word(src + 6);
				dst->ob_flags = get_word(src + 8);
				dst->ob_state = get_word(src + 10);
				dst->ob_spec.index = get_long(src + 12);
				dst->ob_x = get_word(src + 16);
				dst->ob_y = get_word(src + 18);
				dst->ob_width = get_word(src + 20);
				dst->ob_height = get_word(src + 22);
			}
		}

		if (file->header.rsh_nted > 0)
		{
			char *src;
			TEDINFO *dst;
			
			file->rs_tedinfo = g_new(TEDINFO, file->header.rsh_nted);
			if (file->rs_tedinfo == NULL)
			{
				xrsrc_free(file);
				return NULL;
			}
			file->allocated |= RSC_ALLOC_TEDINFO;
			src = buf + (size_t)file->header.rsh_tedinfo;
			dst = file->rs_tedinfo;
			for (UObj = 0; UObj < xrsc_header.rsh_nted; UObj++, src += RSC_SIZEOF_TEDINFO, dst++)
			{
				dst->te_ptext = buf + (size_t)get_long(src + 0);
				dst->te_ptmplt = buf + (size_t)get_long(src + 4);
				dst->te_pvalid = buf + (size_t)get_long(src + 8);
				dst->te_font = get_word(src + 12);
				dst->te_fontid = get_word(src + 14);
				dst->te_just = get_word(src + 16);
				dst->te_color = get_word(src + 18);
				dst->te_fontsize = get_word(src + 20);
				dst->te_thickness = get_word(src + 22);
				dst->te_txtlen = get_word(src + 24);
				dst->te_tmplen = get_word(src + 26);
			}
		}

		if (file->header.rsh_nib > 0)
		{
			char *src;
			ICONBLK *dst;

			file->rs_iconblk = g_new(ICONBLK, file->header.rsh_nib);
			if (file->rs_iconblk == NULL)
			{
				xrsrc_free(file);
				return NULL;
			}
			file->allocated |= RSC_ALLOC_ICONBLK;
			src = buf + (size_t)file->header.rsh_iconblk;
			dst = file->rs_iconblk;
			for (UObj = 0; UObj < xrsc_header.rsh_nib; UObj++, src += RSC_SIZEOF_ICONBLK, dst++)
			{
				dst->ib_pmask = (_WORD *)(buf + (size_t)get_long(src + 0));
				dst->ib_pdata = (_WORD *)(buf + (size_t)get_long(src + 4));
				dst->ib_ptext = buf + (size_t)get_long(src + 8);
				dst->ib_char = get_word(src + 12);
				dst->ib_xchar = get_word(src + 14);
				dst->ib_ychar = get_word(src + 16);
				dst->ib_xicon = get_word(src + 18);
				dst->ib_yicon = get_word(src + 20);
				dst->ib_wicon = get_word(src + 22);
				dst->ib_hicon = get_word(src + 24);
				dst->ib_xtext = get_word(src + 26);
				dst->ib_ytext = get_word(src + 28);
				dst->ib_wtext = get_word(src + 30);
				dst->ib_htext = get_word(src + 32);
				W_Fix_Bitmap(dst->ib_pmask, dst->ib_wicon, dst->ib_hicon, 1);
				W_Fix_Bitmap(dst->ib_pdata, dst->ib_wicon, dst->ib_hicon, 1);
			}
		}

		if (file->header.rsh_nbb > 0)
		{
			char *src;
			BITBLK *dst;
	
			file->rs_bitblk = g_new(BITBLK, file->header.rsh_nbb);
			if (file->rs_bitblk == NULL)
			{
				xrsrc_free(file);
				return NULL;
			}
			file->allocated |= RSC_ALLOC_BITBLK;
			src = buf + (size_t)file->header.rsh_bitblk;
			dst = file->rs_bitblk;
			for (UObj = 0; UObj < xrsc_header.rsh_nbb; UObj++, src += RSC_SIZEOF_BITBLK, dst++)
			{
				dst->bi_pdata = (_WORD *)(buf + (size_t)get_long(src + 0));
				dst->bi_wb = get_word(src + 4);
				dst->bi_hl = get_word(src + 6);
				dst->bi_x = get_word(src + 8);
				dst->bi_y = get_word(src + 10);
				dst->bi_color = get_word(src + 12);
				W_Fix_Bitmap(dst->bi_pdata, dst->bi_wb * 8, dst->bi_hl, 1);
			}
		}

		if (file->header.rsh_nstring > 0)
		{
			char *src;
			char **dst;
	
			file->rs_frstr = g_new(char *, file->header.rsh_nstring);
			if (file->rs_frstr == NULL)
			{
				xrsrc_free(file);
				return NULL;
			}
			file->allocated |= RSC_ALLOC_FRSTR;
			src = buf + (size_t)file->header.rsh_frstr;
			dst = file->rs_frstr;
			for (UObj = 0; UObj < xrsc_header.rsh_nstring; UObj++, src += RSC_SIZEOF_PTR, dst++)
			{
				*dst = buf + (size_t)get_long(src);
			}
		}

		if (file->header.rsh_nimages > 0)
		{
			char *src;
			BITBLK **dst;

			file->rs_frimg = g_new(BITBLK *, file->header.rsh_nimages);
			if (file->rs_frimg == NULL)
			{
				xrsrc_free(file);
				return NULL;
			}
			file->allocated |= RSC_ALLOC_FRIMG;
			src = buf + (size_t)file->header.rsh_frimg;
			dst = file->rs_frimg;
			for (UObj = 0; UObj < xrsc_header.rsh_nimages; UObj++, src += RSC_SIZEOF_PTR, dst++)
			{
				*dst = (BITBLK *)(buf + (size_t)get_long(src));
			}
		}

		if (n_ciconblks > 0)
		{
			file->rs_ciconblk = g_new(CICONBLK, n_ciconblks);
			if (file->rs_ciconblk == NULL)
			{
				xrsrc_free(file);
				return NULL;
			}
			file->allocated |= RSC_ALLOC_CICONBLK;
			cicon_dst = file->rs_ciconblk;
		}

		if (n_userblks > 0)
		{
			file->rs_userblk = g_new(USERBLK, n_userblks);
			if (file->rs_userblk == NULL)
			{
				xrsrc_free(file);
				return NULL;
			}
			file->allocated |= RSC_ALLOC_USERBLK;
		}
	}

	n_ciconblks = 0;
	n_cicons = 0;
	n_userblks = 0;
	{
		OBJECT *rs_object = file->rs_object;
		
		if (!(flags & XRSC_NO_OBFIX))
			rsc_obfix(rs_object, xrsc_header.rsh_nobs);
		for (UObj = 0; UObj < xrsc_header.rsh_nobs; UObj++, rs_object++)
		{
			switch (rs_object->ob_type & OBTYPEMASK)
			{
			case G_BOX:
			case G_IBOX:
			case G_BOXCHAR:
			case G_EXTBOX:
				break;

			case G_STRING:
			case G_TITLE:
			case G_BUTTON:
			case G_SHORTCUT:
				/* not changed to G_STRING here;
				   might be displayed in window
				   where we use our own functions
				 */
				rs_object->ob_spec.free_string = buf + (size_t)(rs_object->ob_spec.index);
				break;

			case G_TEXT:
			case G_FTEXT:
			case G_BOXTEXT:
			case G_FBOXTEXT:
				if (file->allocated & RSC_ALLOC_TEDINFO)
				{
					_ULONG idx = (rs_object->ob_spec.index - file->header.rsh_tedinfo) / RSC_SIZEOF_TEDINFO;
					if (idx < file->header.rsh_nted)
						rs_object->ob_spec.tedinfo = &file->rs_tedinfo[idx];
					else
						rs_object->ob_spec.tedinfo = &empty_ted;
				} else
				{
					rs_object->ob_spec.free_string = buf + (size_t)(rs_object->ob_spec.index);
				}

			case G_IMAGE:
				if (file->allocated & RSC_ALLOC_BITBLK)
				{
					_ULONG idx = (rs_object->ob_spec.index - file->header.rsh_bitblk) / RSC_SIZEOF_BITBLK;
					if (idx < file->header.rsh_nbb)
						rs_object->ob_spec.bitblk = &file->rs_bitblk[idx];
					else
						rs_object->ob_spec.bitblk = &empty_bit;
				} else
				{
					rs_object->ob_spec.free_string = buf + (size_t)(rs_object->ob_spec.index);
				}
				break;

			case G_ICON:
				if (file->allocated & RSC_ALLOC_ICONBLK)
				{
					_ULONG idx = (rs_object->ob_spec.index - file->header.rsh_iconblk) / RSC_SIZEOF_ICONBLK;
					if (idx < file->header.rsh_nib)
						rs_object->ob_spec.iconblk = &file->rs_iconblk[idx];
					else
						rs_object->ob_spec.iconblk = &empty_icon;
				} else
				{
					rs_object->ob_spec.free_string = buf + (size_t)(rs_object->ob_spec.index);
				}
				break;

			case G_CICON:

				if (cicon_p == NULL)
				{
					/* !!! */
				} else
				{
					CICON *dp;
					CICONBLK *cicon;
					_LONG size;
					char *p;
					_LONG num_cicons;
					_LONG idx;

					cicon = cicon_p;
					cicon_p++;
					if (swap_flag)
					{
						flip_iconblk((char *) cicon);
						swap_long((char *) cicon + 34); /* swap cicon->mainlist */
					}
					if (file->allocated & RSC_ALLOC_CICONBLK)
					{
						char *src = (char *)cicon;
						
						rs_object->ob_spec.ciconblk = cicon_dst;
						cicon = cicon_dst++;
						cicon->monoblk.ib_ptext = (char *)(uintptr_t)get_long(src + 8);
						cicon->monoblk.ib_char = get_word(src + 12);
						cicon->monoblk.ib_xchar = get_word(src + 14);
						cicon->monoblk.ib_ychar = get_word(src + 16);
						cicon->monoblk.ib_xicon = get_word(src + 18);
						cicon->monoblk.ib_yicon = get_word(src + 20);
						cicon->monoblk.ib_wicon = get_word(src + 22);
						cicon->monoblk.ib_hicon = get_word(src + 24);
						cicon->monoblk.ib_xtext = get_word(src + 26);
						cicon->monoblk.ib_ytext = get_word(src + 28);
						cicon->monoblk.ib_wtext = get_word(src + 30);
						cicon->monoblk.ib_htext = get_word(src + 32);
						cicon->mainlist = (CICON *)(intptr_t)get_long(src + 34);
					} else
					{
						rs_object->ob_spec.ciconblk = cicon;
						cicon_dst = cicon;
					}
					size = iconblk_masksize(&cicon->monoblk);
					p = (char *)cicon_p;
					cicon->monoblk.ib_pdata = (_WORD *)p;
					p += (size_t)size;
					cicon->monoblk.ib_pmask = (_WORD *)p;
					p += (size_t)size;
					W_Fix_Bitmap(cicon->monoblk.ib_pdata, cicon->monoblk.ib_wicon, cicon->monoblk.ib_hicon, 1);
					W_Fix_Bitmap(cicon->monoblk.ib_pmask, cicon->monoblk.ib_wicon, cicon->monoblk.ib_hicon, 1);
					idx = (_LONG)(intptr_t)cicon->monoblk.ib_ptext;
					if (idx <= 0 || (buf + (size_t)idx) == p || idx < (_LONG)xrsc_header.rsh_string || idx >= (_LONG)xrsc_header.rsh_rssize)
						cicon->monoblk.ib_ptext = p;
					else
						cicon->monoblk.ib_ptext = buf + (size_t)idx;
					p += CICON_STR_SIZE;
					dp = (CICON *)p;
					num_cicons = (_LONG)(intptr_t)(cicon->mainlist);
					if (p > (buf + (size_t)filesize))
					{
						xrsrc_free(file);
						return NULL;
					}
					if (num_cicons == 0)
					{
						cicon->mainlist = NULL;
					} else
					{
						if (file->allocated & RSC_ALLOC_CICONBLK)
						{
							file->rs_cicon = g_renew(CICON, file->rs_cicon, n_cicons + num_cicons);
							if (file->rs_cicon == NULL)
							{
								xrsrc_free(file);
								return NULL;
							}
							dp = &file->rs_cicon[n_cicons];
							n_cicons += num_cicons;
						}
						cicon->mainlist = dp;
						while (num_cicons != 0)
						{
							dp->num_planes = get_word(p);
							if (swap_flag)
								dp->num_planes = bswap_16(dp->num_planes);
							p += RSC_SIZEOF_CICON;
							dp->col_data = (_WORD *)p;
							p += (size_t)(size * dp->num_planes);
							dp->col_mask = (_WORD *)p;
							p += (size_t)size;
							W_Fix_Bitmap(dp->col_data, cicon->monoblk.ib_wicon, cicon->monoblk.ib_hicon, dp->num_planes);
							W_Fix_Bitmap(dp->col_mask, cicon->monoblk.ib_wicon, cicon->monoblk.ib_hicon, 1);
							if (dp->sel_data != NULL)
							{
								dp->sel_data = (_WORD *)p;
								p += (size_t)(size * dp->num_planes);
								dp->sel_mask = (_WORD *)p;
								p += (size_t)size;
								W_Fix_Bitmap(dp->sel_data, cicon->monoblk.ib_wicon, cicon->monoblk.ib_hicon, dp->num_planes);
								W_Fix_Bitmap(dp->sel_mask, cicon->monoblk.ib_wicon, cicon->monoblk.ib_hicon, 1);
							} else
							{
								dp->sel_data = NULL;
								dp->sel_mask = NULL;
							}
							num_cicons--;
							if (num_cicons == 0)
							{
								dp->next_res = NULL;
							} else
							{
								dp->next_res = (CICON *)p;
							}
							if (file->allocated & RSC_ALLOC_CICONBLK)
								dp++;
							else
								dp = (CICON *)p;
						}
					}
					cicon_p = (CICONBLK *)p;
				}
				break;

			case G_USERDEF:
				if (file->allocated & RSC_ALLOC_USERBLK)
				{
					char *src = buf + (size_t)rs_object->ob_spec.index;
					rs_object->ob_spec.userblk = &file->rs_userblk[n_userblks++];
					rs_object->ob_spec.userblk->ub_parm = get_long(src + 4);
				} else
				{
					rs_object->ob_spec.userblk = (USERBLK *)(buf + (size_t)rs_object->ob_spec.index);
				}
				/*
				 * It's up to the application to set the appropiate function.
				 * To be on the safe side, let it point to some function
				 * that draws a box only, or simply does nothing.
				 */
				rs_object->ob_spec.userblk->ub_code = draw_userdef;
				break;
			}
		}
	}

	{
		OBJECT **rs_trindex;

		rs_trindex = file->rs_trindex;
		for (UObj = 0; UObj < xrsc_header.rsh_ntree; UObj++, rs_trindex++)
		{
			if (!(flags & XRSC_NO_ZERO_ROOT))
			{
				(*rs_trindex)[ROOT].ob_x = 0;
				(*rs_trindex)[ROOT].ob_y = 0;
			}
		}
#if 0
		if (!(flags & XRSC_NO_INSERT_POPUPS))
		{
			rsc_insert_popups(file->rs_trindex, xrsc_header.rsh_ntree, xrsc_header.rsh_nobs);
		}
#endif
	}
	
#if FLIP_DATA
	if (swap_flag)
		flip_data(file);
#endif

	return file;
}

/*** ---------------------------------------------------------------------- ***/

_BOOL xrsrc_free(RSCFILE *file)
{
	if (file == NULL)
		return FALSE;
	hrelease_objs(file->rs_object, file->header.rsh_nobs);
	if (file->allocated & RSC_ALLOC_TRINDEX)
		g_free(file->rs_trindex);
	if (file->allocated & RSC_ALLOC_OBJECT)
		g_free(file->rs_object);
	if (file->allocated & RSC_ALLOC_TEDINFO)
		g_free(file->rs_tedinfo);
	if (file->allocated & RSC_ALLOC_ICONBLK)
		g_free(file->rs_iconblk);
	if (file->allocated & RSC_ALLOC_BITBLK)
		g_free(file->rs_bitblk);
	if (file->allocated & RSC_ALLOC_FRSTR)
		g_free(file->rs_frstr);
	if (file->allocated & RSC_ALLOC_FRIMG)
		g_free(file->rs_frimg);
	if (file->allocated & RSC_ALLOC_CICONBLK)
	{
		g_free(file->rs_ciconblk);
		g_free(file->rs_cicon);
	}
	if (file->allocated & RSC_ALLOC_USERBLK)
		g_free(file->rs_userblk);
	g_free(file);
	return TRUE;
}
