/*****************************************************************************
 * XRSRC.H
 *****************************************************************************/

#ifndef __XRSRC_H__
#define __XRSRC_H__

EXTERN_C_BEG


/*
 * resource file header
 */

typedef struct _rs_header {
/* 0x00 */	_UWORD  rsh_vrsn;			/* version number (should be 1) */
/* 0x02 */	_UWORD  rsh_object;			/* offset to first object */
/* 0x04 */	_UWORD  rsh_tedinfo;		/* offset to TEDINFO structures */
/* 0x06 */	_UWORD  rsh_iconblk;		/* offset to ICONBLK structures */
/* 0x08 */	_UWORD  rsh_bitblk;			/* offset to BITBLK structures */
/* 0x0a */	_UWORD  rsh_frstr;			/* offset to free string (alert box texts) */
/* 0x0c */	_UWORD  rsh_string;			/* offset to string pool */
/* 0x0e */	_UWORD  rsh_imdata;			/* offset to image data */
/* 0x10 */	_UWORD  rsh_frimg;			/* offset to free images */
/* 0x12 */	_UWORD  rsh_trindex;		/* offset to tree addresses */
/* 0x14 */	_UWORD  rsh_nobs;			/* number of objects */
/* 0x16 */	_UWORD  rsh_ntree;			/* number of trees */
/* 0x18 */	_UWORD  rsh_nted;			/* number of TEDINFOs */
/* 0x1a */	_UWORD  rsh_nib;			/* number of ICONBLKs */
/* 0x1c */	_UWORD  rsh_nbb;			/* number of BITBLKs */
/* 0x1e */	_UWORD  rsh_nstring;		/* number of free strings */
/* 0x20 */	_UWORD  rsh_nimages;		/* number of free images */
/* 0x22 */	_UWORD  rsh_rssize;			/* total resource size */
} RS_HEADER;
#define RSC_SIZEOF_RS_HEADER ((size_t)(0x24))

typedef struct _xrs_header {
/* 0x00 */	_UWORD  rsh_vrsn;			/* should be 3 */
/* 0x02 */	_UWORD  rsh_extvrsn;		/* not used, initialised to 'OR' for ORCS, 'IN' for Interface, 'RM' for RSM */
/* 0x04 */	_ULONG  rsh_object;			/* offset to first object */
/* 0x08 */	_ULONG  rsh_tedinfo;		/* offset to TEDINFO structures */
/* 0x0c */	_ULONG  rsh_iconblk;		/* offset to ICONBLK structures */
/* 0x10 */	_ULONG  rsh_bitblk;			/* offset to BITBLK structures */
/* 0x14 */	_ULONG  rsh_frstr;			/* offset to free strings (alert box texts) */
/* 0x18 */	_ULONG  rsh_string;			/* offset to string pool */
/* 0x1c */	_ULONG  rsh_imdata;			/* offset to image data */
/* 0x20 */	_ULONG  rsh_frimg;			/* offset to free images */
/* 0x24 */	_ULONG  rsh_trindex;		/* offset to tree addresses */
/* 0x28 */	_ULONG  rsh_nobs;			/* number of objects */
/* 0x2c */	_ULONG  rsh_ntree;			/* number of trees */
/* 0x30 */	_ULONG  rsh_nted;			/* number of TEDINFOs */
/* 0x34 */	_ULONG  rsh_nib;			/* number of ICONBLKs */
/* 0x38 */	_ULONG  rsh_nbb;			/* number of BITBLKs */
/* 0x3c */	_ULONG  rsh_nstring;		/* number of free strings */
/* 0x40 */	_ULONG  rsh_nimages;		/* number of free images */
/* 0x44 */	_ULONG  rsh_rssize;			/* total resource size */
} XRS_HEADER;
#define RSC_SIZEOF_XRS_HEADER ((size_t)(0x48))

#define XRSC_VRSN_INTERFACE 0x494e
#define XRSC_VRSN_ORCS      0x4F52
#define XRSC_VRSN_RSM       0x524d


#define RS_DIFF_SIZE (RSC_SIZEOF_BRS_HEADER - RSC_SIZEOF_RS_HEADER)
#define XRS_DIFF_SIZE (RSC_SIZEOF_XRS_HEADER - RSC_SIZEOF_RS_HEADER)
#define RS_THRESHOLD 65536l

#define RSC_EXT_FILESIZE	0
#define RSC_EXT_CICONBLK	1
#define RSC_EXT_PALETTE		2
#define RSC_EXT_ENDMARK		3
#define RSC_EXT_SIZE		4	/* number of known extension slots */

#define RSC_VERSION_MASK	0x03
#define RSC_EXT_FLAG		0x04

#define IS_XRSC_HEADER(handle) ((((XRS_HEADER *)(handle))->rsh_vrsn & RSC_VERSION_MASK) == 0x03)

void xrsc_hdr2xrsc(XRS_HEADER *xrsc_header, RS_HEADER *header, size_t diff_size);
void xrsc_xrsc2hdr(RS_HEADER *header, XRS_HEADER *xrsc_header, size_t diff_size);


EXTERN_C_END

#endif /* __XRSRC_H__ */
