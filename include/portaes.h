/*****************************************************************************
 * PORTAES.H
 *****************************************************************************/

#ifndef __PORTAES_H__
#define __PORTAES_H__

#ifndef __PORTAB_H__
#  include <portab.h>
#endif
#ifndef __GRECT_H__
#  include <grect.h>
#endif

EXTERN_C_BEG

/* Object structures */

#define OBTYPEMASK    0x00ff
#define OBEXTTYPEMASK 0xff00


/*
 * Macros to manipulate a TEDINFO color word
 */

#define COLSPEC_GET_FRAMECOL(color)    (((color) >> 12) & 0x0f)
#define COLSPEC_GET_TEXTCOL(color)     (((color) >>  8) & 0x0f)
#define COLSPEC_GET_TEXTMODE(color)    (((color) >>  7) & 0x01)
#define COLSPEC_GET_FILLPATTERN(color) (((color) >>  4) & 0x07)
#define COLSPEC_GET_INTERIORCOL(color) (((color)      ) & 0x0f)

#define COLSPEC_SET_FRAMECOL(color, framecol)       color = ( ((color) & 0x0fff) | (((framecol)    & 0x0f) << 12) )
#define COLSPEC_SET_TEXTCOL(color, textcol)         color = ( ((color) & 0xf0ff) | (((textcol)     & 0x0f) <<  8) )
#define COLSPEC_SET_TEXTMODE(color, textmode)       color = ( ((color) & 0xff7f) | (((textmode)    & 0x01) <<  7) )
#define COLSPEC_SET_FILLPATTERN(color, fillpattern) color = ( ((color) & 0xff8f) | (((fillpattern) & 0x07) <<  4) )
#define COLSPEC_SET_INTERIORCOL(color, interiorcol) color = ( ((color) & 0xfff0) | (((interiorcol) & 0x0f)      ) )

/* bfobspec.textmode/bfcolspec.textmode: */

#define TEXT_TRANSPARENT    0
#define TEXT_OPAQUE     1

#define COLSPEC_MAKE(framecol, textcol, textmode, fillpattern, interiorcol) \
	((((framecol)    & 0x0f) << 12) | \
	 (((textcol)     & 0x0f) <<  8) | \
	 (((textmode)    & 0x01) <<  7) | \
	 (((fillpattern) & 0x07) <<  4) | \
	 (((interiorcol) & 0x0f)      ))

typedef struct _tedinfo
{
	char	*te_ptext;		/* ptr to text (must be 1st)    */
	char	*te_ptmplt;		/* ptr to template              */
	char	*te_pvalid;		/* ptr to validation            */
	_WORD	te_font;		/* font                         */
	_WORD	te_fontid;		/* font id                      */
	_WORD	te_just;		/* justification: left, right...*/
	_UWORD	te_color;		/* color information            */
	_WORD	te_fontsize;	/* junk word                    */
	_WORD	te_thickness;	/* border thickness             */
	_WORD	te_txtlen;		/* text string length           */
	_WORD	te_tmplen;		/* template string length       */
} TEDINFO;
#define te_junk1 te_fontid
#define te_junk2 te_fontsize

/*
 * Macros to manipulate a ICONBLK color word
 */
#define ICOLSPEC_GET_DATACOL(color)   ( ((color) >> 12) & 0x0f )
#define ICOLSPEC_GET_MASKCOL(color)   ( ((color) >>  8) & 0x0f )
#define ICOLSPEC_GET_CHARACTER(color) ( ((color)      ) & 0xff )

#define ICOLSPEC_SET_DATACOL(color, datacol) color = ( ((color) & 0x0fff) | (((datacol) & 0x0f) << 12) )
#define ICOLSPEC_SET_MASKCOL(color, maskcol) color = ( ((color) & 0xf0ff) | (((maskcol) & 0x0f) <<  8) )
#define ICOLSPEC_SET_CHARACTER(color, ch)    color = ( ((color) & 0xff00) | (((ch)      & 0xff)      ) )

#define ICOLSPEC_MAKE(datacol, maskcol, ch) \
	((((datacol)    & 0x0f) << 12) | \
	 (((maskcol)    & 0x0f) <<  8) | \
	 (((ch)         & 0xff)      ))

typedef struct _iconblk
{
	_WORD	*ib_pmask;
	_WORD	*ib_pdata;
	char	*ib_ptext;
	_WORD	ib_char;
	_WORD	ib_xchar;
	_WORD	ib_ychar;
	_WORD	ib_xicon;
	_WORD	ib_yicon;
	_WORD	ib_wicon;
	_WORD	ib_hicon;
	_WORD	ib_xtext;
	_WORD	ib_ytext;
	_WORD	ib_wtext;
	_WORD	ib_htext;
} ICONBLK;


typedef struct _bitblk
{
	_WORD	*bi_pdata;			/* ptr to bit forms data        */
	_WORD	bi_wb;				/* width of form in bytes       */
	_WORD	bi_hl;				/* height in lines              */
	_WORD	bi_x;				/* source x in bit form         */
	_WORD	bi_y;				/* source y in bit form         */
	_WORD	bi_color;			/* foreground color             */
} BITBLK;


typedef struct _cicon {
	_WORD	num_planes;			/* number of planes in the following data */
	_WORD	*col_data;			/* pointer to color bitmap in standard form */
	_WORD	*col_mask;			/* pointer to single plane mask of col_data */
	_WORD	*sel_data;			/* pointer to color bitmap of selected icon */
	_WORD	*sel_mask;			/* pointer to single plane mask of selected icon */
	struct _cicon *next_res;	/* pointer to next icon for a different resolution */
} CICON; /* AES >= 3.3 */

typedef struct _ciconblk {
	ICONBLK monoblk;			/* default monochrome icon */
	CICON *mainlist;			/* list of color icons for different resolutions */
} CICONBLK; /* AES >= 3.3 */

#define CICON_STR_SIZE 12

struct __parmblk;

typedef _WORD _CDECL (*PARMBLKFUNC)(struct __parmblk *pb);
typedef struct
{
	PARMBLKFUNC ub_code;
	_LONG_PTR ub_parm;
} USERBLK;

typedef struct
{
	unsigned character   :  8;
	signed   framesize   :  8;
	unsigned framecol    :  4;
	unsigned textcol     :  4;
	unsigned textmode    :  1;
	unsigned fillpattern :  3;
	unsigned interiorcol :  4;
} bfobspec;
typedef bfobspec BFOBSPEC;

typedef struct objc_colorword
{
	unsigned	borderc : 4;
	unsigned	textc   : 4;
	unsigned	opaque  : 1;
	unsigned	pattern : 3;
	unsigned	fillc   : 4;
} OBJC_COLORWORD;

/*
 * Macros to manipulate a OBSPEC info
 */
#define OBSPEC_GET_CHARACTER(obspec)   ((unsigned char) ( (((obspec).index) >> 24) & 0xff ))
#define OBSPEC_GET_FRAMESIZE(obspec)   ((signed char)   ( (((obspec).index) >> 16) & 0xff ))
#define OBSPEC_GET_FRAMECOL(obspec)    ((unsigned char) ( (((obspec).index) >> 12) & 0x0f ))
#define OBSPEC_GET_TEXTCOL(obspec)     ((unsigned char) ( (((obspec).index) >>  8) & 0x0f ))
#define OBSPEC_GET_TEXTMODE(obspec)    ((unsigned char) ( (((obspec).index) >>  7) & 0x01 ))
#define OBSPEC_GET_FILLPATTERN(obspec) ((unsigned char) ( (((obspec).index) >>  4) & 0x07 ))
#define OBSPEC_GET_INTERIORCOL(obspec) ((unsigned char) ( (((obspec).index)      ) & 0x0f ))

#define OBSPEC_SET_CHARACTER(obspec, ch)            (obspec).index = ( (((obspec).index) & 0x00ffffffl) | ((((_ULONG)((ch)          & 0xff)) << 24)) )
#define OBSPEC_SET_FRAMESIZE(obspec, framesize)     (obspec).index = ( (((obspec).index) & 0xff00ffffl) | ((((_ULONG)((framesize)   & 0xff)) << 16)) )
#define OBSPEC_SET_FRAMECOL(obspec, framecol)       (obspec).index = ( (((obspec).index) & 0xffff0fffl) | ((((_ULONG)((framecol)    & 0x0f)) << 12)) )
#define OBSPEC_SET_TEXTCOL(obspec, textcol)         (obspec).index = ( (((obspec).index) & 0xfffff0ffl) | ((((_ULONG)((textcol)     & 0x0f)) <<  8)) )
#define OBSPEC_SET_TEXTMODE(obspec, textmode)       (obspec).index = ( (((obspec).index) & 0xffffff7fl) | ((((_ULONG)((textmode)    & 0x01)) <<  7)) )
#define OBSPEC_SET_FILLPATTERN(obspec, fillpattern) (obspec).index = ( (((obspec).index) & 0xffffff8fl) | ((((_ULONG)((fillpattern) & 0x07)) <<  4)) )
#define OBSPEC_SET_INTERIORCOL(obspec, interiorcol) (obspec).index = ( (((obspec).index) & 0xfffffff0l) | ((((_ULONG)((interiorcol) & 0x0f))      )) )

#define OBSPEC_MAKE(ch, framesize, framecol, textcol, textmode, fillpattern, interiorcol) \
	   ( \
		((((_ULONG)((ch)          & 0xff)) << 24)) | \
		((((_ULONG)((framesize)   & 0xff)) << 16)) | \
		((((_ULONG)((framecol)    & 0x0f)) << 12)) | \
		((((_ULONG)((textcol)     & 0x0f)) <<  8)) | \
		((((_ULONG)((textmode)    & 0x01)) <<  7)) | \
		((((_ULONG)((fillpattern) & 0x07)) <<  4)) | \
		((((_ULONG)((interiorcol) & 0x0f))      )) )

#define OBSPEC_SET_OBSPEC(obspec, ch, framesize, framecol, textcol, textmode, fillpattern, interiorcol) \
	obspec.index = OBSPEC_MAKE(ch, framesize, framecol, textcol, textmode, fillpattern, interiorcol)


#ifndef _SWINFO
#define _SWINFO
typedef struct {
	char	*string;					/* etwa "TOS|KAOS|MAG!X" */
	_WORD	num;						/* Nr. der aktuellen Zeichenkette */
	_WORD	maxnum;						/* maximal erlaubtes <num> */
} SWINFO;
#endif /* SWINFO */

#ifndef _POPINFO
#define _POPINFO
typedef struct {
	struct	_object *tree;				/* Popup- Menue */
	_WORD	obnum;						/* aktuelles Objekt von <tree> */
} POPINFO;
#endif

typedef union obspecptr
{
	_LONG_PTR	index;
	union obspecptr *indirect;
	bfobspec	obspec;
	TEDINFO		*tedinfo;
	ICONBLK		*iconblk;
	BITBLK		*bitblk;
	USERBLK		*userblk;
	CICONBLK	*ciconblk;
	char		*free_string;
} OBSPEC;


typedef struct _object
{
	_WORD	ob_next;					/* -> object's next sibling */
	_WORD	ob_head;					/* -> head of object's children */
	_WORD	ob_tail;					/* -> tail of object's children */
	_UWORD	ob_type;					/* object type: BOX, CHAR,... */
	_UWORD	ob_flags;					/* object flags */
	_UWORD	ob_state;					/* state: OS_SELECTED, OPEN, ... */
	OBSPEC	ob_spec;					/* "out": -> anything else */
	_WORD	ob_x;						/* upper left corner of object */
	_WORD	ob_y;						/* upper left corner of object */
	_WORD	ob_width;					/* object width */
	_WORD	ob_height;					/* object height */
} OBJECT;


typedef struct __parmblk
{
	OBJECT	*pb_tree;
	_WORD	pb_obj;
	_UWORD	pb_prevstate;
	_UWORD	pb_currstate;
	_WORD	pb_x, pb_y, pb_w, pb_h;
	_WORD	pb_xc, pb_yc, pb_wc, pb_hc;
	_LONG	pb_parm;
} PARMBLK;

/****** Object definitions **********************************************/

/* graphic types of obs */
#define G_BOX           20
#define G_TEXT          21
#define G_BOXTEXT       22
#define G_IMAGE         23
#define G_USERDEF       24
#define G_PROGDEF       G_USERDEF
#define G_IBOX          25
#define G_BUTTON        26
#define G_BOXCHAR       27
#define G_STRING        28
#define G_FTEXT         29
#define G_FBOXTEXT      30
#define G_ICON          31
#define G_TITLE         32
#define G_CICON         33              /* AES >= 3.3 */
#define G_CLRICN		G_CICON			/* From ViewMAX beta. Incompatible with Atari colour icons. */
#define G_SWBUTTON      34              /* MAG!X */
#define G_DTMFDB		34				/* ViewMax: for internal AES use only: desktop image */
#define G_POPUP         35              /* MAG!X */
#define G_RESVD1        36              /* MagiC 3.1 */
#define G_WINTITLE		36				/* MagiC internal window title */
#define G_EDIT			37				/* MagiC extended edit object */
#define G_SHORTCUT		38				/* MagiC 6 menu entry with shortcut */
#define G_WORDCUT       G_SHORTCUT
#define G_SLIST			39				/* XaAES scrolling list */
#define G_EXTBOX		40				/* XaAES */
#define G_OBLINK		41				/* XaAES */


/* Object flags */
#define OF_NONE            0x0000
#define OF_SELECTABLE      0x0001
#define OF_DEFAULT         0x0002
#define OF_EXIT            0x0004
#define OF_EDITABLE        0x0008
#define OF_RBUTTON         0x0010
#define OF_LASTOB          0x0020
#define OF_TOUCHEXIT       0x0040
#define OF_HIDETREE        0x0080
#define OF_INDIRECT        0x0100
/* 3D objects AES 3.4	*/
#define OF_FL3DMASK        0x0600
#define OF_FL3DNONE        0x0000
#define OF_FL3DIND		   0x0200
#define OF_FL3DBAK		   0x0400
#define OF_FL3DACT		   0x0600
#define OF_SUBMENU         0x0800         /* falcon aes hierarchical menus */
#define OF_FLAG11		   OF_SUBMENU
#define OF_FLAG12		   0x1000
#define OF_FLAG13		   0x2000
#define OF_FLAG14		   0x4000
#define OF_FLAG15		   0x8000
/* ViewMAX */
#define OF_ESCCANCEL		0x0200
#define OF_BITBUTTON		0x0400
#define OF_SCROLLER 		0x0800
#define OF_FLAG3D			0x1000
#define OF_USECOLORCAT		0x2000


/* Object states */
#define OS_NORMAL		0x0000
#define OS_SELECTED		0x0001
#define OS_CROSSED		0x0002
#define OS_CHECKED		0x0004
#define OS_DISABLED		0x0008
#define OS_OUTLINED		0x0010
#define OS_SHADOWED		0x0020
#define OS_WHITEBAK		0x0040
#define OS_DRAW3D		0x0080
#define OS_STATE08		0x0100
#define OS_STATE09		0x0200
#define OS_STATE10		0x0400
#define OS_STATE11		0x0800
#define OS_STATE12		0x1000
#define OS_STATE13		0x2000
#define OS_STATE14		0x4000
#define OS_STATE15		0x8000
/* ViewMAX */
#define OS_HIGHLIGHTED	0x0100
#define OS_UNHIGHLIGHTED 0x0200

/* Object colors - default pall. */
#define G_WHITE			0
#define G_BLACK			1
#define G_RED			2
#define G_GREEN			3
#define G_BLUE			4
#define G_CYAN			5
#define G_YELLOW		6
#define G_MAGENTA		7
#define G_LWHITE		8
#define G_LBLACK		9
#define G_LRED			10
#define G_LGREEN		11
#define G_LBLUE			12
#define G_LCYAN			13
#define G_LYELLOW		14
#define G_LMAGENTA		15


#if !defined(__USE_GEMLIB) || defined(__GEMLIB_OLDNAMES)

/* object flags */
#define NONE		 	OF_NONE
#define SELECTABLE		OF_SELECTABLE
#define DEFAULT			OF_DEFAULT
#define EXIT			OF_EXIT
#define EDITABLE		OF_EDITABLE
#define RBUTTON			OF_RBUTTON
#define LASTOB			OF_LASTOB
#define TOUCHEXIT		OF_TOUCHEXIT
#define HIDETREE		OF_HIDETREE
#define INDIRECT		OF_INDIRECT
/* 3D objects AES 3.4	*/
#define FL3DMASK        OF_FL3DMASK
#define FL3DNONE        OF_FL3DNONE
#define FL3DIND         OF_FL3DIND         /* 3D Indicator      AES 4.0 */
#define FL3DBAK         OF_FL3DBAK         /* 3D Background     AES 4.0 */
#define FL3DACT         OF_FL3DACT         /* 3D Activator      AES 4.0 */
#define SUBMENU			OF_SUBMENU	/* bit 11 */
#define FLAG11			OF_FLAG11
#define FLAG12			OF_FLAG12
#define FLAG13			OF_FLAG13
#define FLAG14			OF_FLAG14
#define FLAG15			OF_FLAG15

/* ViewMAX */
#define ESCCANCEL		OF_ESCCANCEL
#define BITBUTTON		OF_BITBUTTON
#define SCROLLER 		OF_SCROLLER
#define FLAG3D			OF_FLAG3D
#define USECOLORCAT		OF_USECOLORCAT

/* Object states */
#define NORMAL          OS_NORMAL
#define SELECTED        OS_SELECTED
#define CROSSED         OS_CROSSED
#define CHECKED         OS_CHECKED
#define DISABLED        OS_DISABLED
#define OUTLINED        OS_OUTLINED
#define SHADOWED        OS_SHADOWED
#define WHITEBAK        OS_WHITEBAK            /* TOS         */
#define DRAW3D          OS_DRAW3D            /* GEM 2.x     */
/* ViewMAX */
#define HIGHLIGHTED		OS_HIGHLIGHTED
#define UNHIGHLIGHTED	OS_UNHIGHLIGHTED

/* Object colors */
#if !defined(__COLORS)
/*
 * using AES-colors and BGI-colors
 * is not possible
 */
#define __COLORS
#define WHITE            0
#define BLACK            1
#define RED              2
#define GREEN            3
#define BLUE             4
#define CYAN             5
#define YELLOW           6
#define MAGENTA          7
#define LWHITE           8
#define LBLACK           9
#define LRED            10
#define LGREEN          11
#define LBLUE           12
#define LCYAN           13
#define LYELLOW         14
#define LMAGENTA        15
/* ViewMAX */
#define DWHITE			LWHITE
#define DBLACK			LBLACK
#define DRED			LRED
#define DGREEN			LGREEN
#define DBLUE			LBLUE
#define DCYAN			LCYAN
#define DYELLOW			LYELLOW
#define DMAGENTA		LMAGENTA
#endif /* __COLORS */

#endif

#define NIL (-1)
#define DESK			 0
#define ROOT             0
#define MAX_LEN         81              /* max string length */
#define MAX_DEPTH        8              /* max depth of search or draw */


/* font types */
#define GDOS_PROP        0 /* Speedo GDOS font */
#define GDOS_MONO        1 /* Speedo GDOS font, force monospace output */
#define GDOS_BITM        2 /* GDOS bit map font */
#define IBM              3
#define SMALL            5
#define TE_FONT_MASK     7


/* editable text field definitions */
#define ED_START        0
#define ED_INIT         1
#define ED_CHAR         2
#define ED_END          3
#define ED_CRSR         100            /* MAG!X */
#define ED_DRAW         103            /* MAG!X 2.00 */

#define EDSTART			ED_START	/* alias */
#define EDINIT			ED_INIT		/* alias */
#define EDCHAR			ED_CHAR		/* alias */
#define EDEND 			ED_END		/* alias */


/* editable text justification */
#define TE_LEFT         0
#define TE_RIGHT        1
#define TE_CNTR         2
#define TE_JUST_MASK    3

/* inside patterns */
#define IP_HOLLOW		0
#define IP_1PATT		1
#define IP_2PATT		2
#define IP_3PATT		3
#define IP_4PATT		4
#define IP_5PATT		5
#define IP_6PATT		6
#define IP_SOLID		7


/* data structure types */
#define R_TREE           0
#define R_OBJECT         1
#define R_TEDINFO        2
#define R_ICONBLK        3
#define R_BITBLK         4
#define R_STRING         5              /* gets pointer to free strings */
#define R_IMAGEDATA      6              /* gets pointer to free images */
#define R_OBSPEC         7
#define R_TEPTEXT        8              /* sub-pointers in TEDINFO */
#define R_TEPTMPLT       9
#define R_TEPVALID      10
#define R_IBPMASK       11              /* sub-pointers in ICONBLK */
#define R_IBPDATA       12
#define R_IBPTEXT       13
#define R_BIPDATA       14              /* sub-pointers in BITBLK */
#define R_FRSTR         15              /* gets addr of pointer to free strings */
#define R_FRIMG         16              /* gets addr of pointer to free images  */

typedef struct rshdr
{
	_UWORD	rsh_vrsn;
	_UWORD	rsh_object;
	_UWORD	rsh_tedinfo;
	_UWORD	rsh_iconblk;	/* list of ICONBLKS */
	_UWORD	rsh_bitblk;
	_UWORD	rsh_frstr;
	_UWORD	rsh_string;
	_UWORD	rsh_imdata;		/* image data */
	_UWORD	rsh_frimg;
	_UWORD	rsh_trindex;
	_UWORD	rsh_nobs;		/* counts of various structs */
	_UWORD	rsh_ntree;
	_UWORD	rsh_nted;
	_UWORD	rsh_nib;
	_UWORD	rsh_nbb;
	_UWORD	rsh_nstring;
	_UWORD	rsh_nimages;
	_UWORD	rsh_rssize;		/* total bytes in resource */
} RSHDR;


/* wind calc flags */
#define WC_BORDER 0
#define WC_WORK   1


/* Mouse form definition block */

typedef struct mfstr
{
	_WORD	mf_xhot;
	_WORD	mf_yhot;
	_WORD	mf_nplanes;
	_WORD	mf_fg;
	_WORD	mf_bg;
	_WORD	mf_mask[16];
	_WORD	mf_data[16];
} MFORM;


/* Utilities */
void rc_copy(const GRECT *src, GRECT *dst);
_WORD rc_equal(const GRECT *r1, const GRECT *r2);
_WORD rc_intersect(const GRECT *src, GRECT *dst);
GRECT *array_to_grect (const _WORD *array, GRECT *area);
_WORD *grect_to_array (const GRECT *area, _WORD *array);

extern _WORD gl_apid;
extern _WORD gl_ap_version;
extern _WORD aes_global[];

typedef union
{
	void *spec;			/* PC_GEM */
	long l;
	short pi[2];
} aes_private;

/* At last give in to the fact that it is a struct, NOT an array */
typedef struct _aes_global {
	_WORD ap_version;
	_WORD ap_count;
	_WORD ap_id;
	aes_private *ap_private;
	OBJECT **ap_ptree;
	void *ap_rscmem; /* RSHDR or RSXHDR */
	_UWORD ap_rsclen; /* note: short only; unusable with resource >64k */
	_WORD ap_planes;
	_WORD ap_res1;
	_WORD ap_res2;
	_WORD ap_bvdisk;
	_WORD ap_bvhard;
} AES_GLOBAL;

#define	_AESversion   (((AES_GLOBAL *)aes_global)->ap_version)
#define	_AESnumapps   (((AES_GLOBAL *)aes_global)->ap_count)
#define	_AESapid      (((AES_GLOBAL *)aes_global)->ap_id)
#define	_AESappglobal ((_LONG)(((AES_GLOBAL *)aes_global)->ap_private))
#define	_AESrscfile   (((AES_GLOBAL *)aes_global)->ap_ptree)
#define	_AESrscmem    (((AES_GLOBAL *)aes_global)->ap_rscmem)
#define	_AESrsclen    (((AES_GLOBAL *)aes_global)->ap_rsclen)
#define	_AESmaxchar   (((AES_GLOBAL *)aes_global)->ap_bvdisk)
#define	_AESminchar   (((AES_GLOBAL *)aes_global)->ap_bvhard)

typedef struct
{
	const _WORD *control;
	_WORD *global;
	_WORD *intin;
	_WORD *intout;
	const void **addrin;
	void **addrout;
} AESPARBLK;

typedef AESPARBLK AESPB; /* MagiC name */

/*
 * the AES entry point
 */
extern _WORD aes(AESPB *pb);

/****** Application definitions *****************************************/

_WORD appl_init(void);
_WORD appl_exit(void);

/****** Object prototypes ************************************************/

/* the objc_sysvar ob_swhich values */
#define LK3DIND      1                  /* AES 4.0     */
#define LK3DACT      2                  /* AES 4.0     */
#define INDBUTCOL    3                  /* AES 4.0     */
#define ACTBUTCOL    4                  /* AES 4.0     */
#define BACKGRCOL    5                  /* AES 4.0     */
#define AD3DVALUE    6                  /* AES 4.0     */
#define MX_ENABLE3D  10                 /* MagiC 3.0   */
#define MENUCOL		 11                 /* MagiC 6     */

#define OB_GETVAR 0
#define OB_SETVAR 1

/* objc_change modes */
#define NO_DRAW			0	/* object will not be redrawn, see mt_objc_change() */
#define REDRAW 			1	/* object will be redrawn, see mt_objc_change() */

/* objc_order modes */
#define OO_LAST			-1	/* make object the last child, see mt_objc_order() */
#define OO_FIRST		0	/* make object the first child, see mt_objc_order() */

/* objc_sysvar modes */
#define SV_INQUIRE		0	/* inquire sysvar data, see mt_objc_sysvar() */
#define SV_SET 			1	/* set sysvar data, see mt_objc_sysvar() */

_WORD objc_add( OBJECT *ob_atree, _WORD ob_aparent, _WORD ob_achild );
_WORD objc_delete( OBJECT *ob_dltree, _WORD ob_dlobject );
_WORD objc_draw( OBJECT *ob_drtree, _WORD ob_drstartob,
               _WORD ob_drdepth, _WORD ob_drxclip, _WORD ob_dryclip,
               _WORD ob_drwclip, _WORD ob_drhclip );
_WORD objc_draw_grect(OBJECT *, _WORD Start, _WORD Depth, const GRECT *r);
_WORD objc_find( OBJECT *ob_ftree, _WORD ob_fstartob, _WORD ob_fdepth,
               _WORD ob_fmx, _WORD ob_fmy );
_WORD objc_offset( OBJECT *ob_oftree, _WORD ob_ofobject,
                 _WORD *ob_ofxoff, _WORD *ob_ofyoff );
_WORD objc_order( OBJECT *ob_ortree, _WORD ob_orobject,
                _WORD ob_ornewpos );
_WORD objc_edit( OBJECT *ob_edtree, _WORD ob_edobject,
               _WORD ob_edchar, _WORD *ob_edidx, _WORD ob_edkind );
_WORD objc_change( OBJECT *ob_ctree, _WORD ob_cobject,
                 _WORD ob_cresvd, _WORD ob_cxclip, _WORD ob_cyclip,
                 _WORD ob_cwclip, _WORD ob_chclip,
                 _WORD ob_cnewstate, _WORD ob_credraw );
_WORD objc_change_grect( OBJECT *ob_ctree, _WORD ob_cobject, _WORD ob_cresvd, const GRECT *clip, _WORD ob_cnewstate, _WORD ob_credraw );
_WORD objc_sysvar( _WORD ob_svmode, _WORD ob_svwhich,  /* AES 4.0 */
                 _WORD ob_svinval1, _WORD ob_svinval2,
                 _WORD *ob_svoutval1, _WORD *ob_svoutval2); /* AES 4.0 */
_WORD objc_xfind( OBJECT *ob_ftree, _WORD ob_fstartob, _WORD ob_fdepth,
               _WORD ob_fmx, _WORD ob_fmy );
_VOID objc_wchange( OBJECT *ob_ctree, _WORD ob_cobject,
                 _WORD ob_cnewstate, GRECT *clip, _WORD whandle);
_VOID objc_wdraw( OBJECT *ob_drtree, _WORD ob_drstartob,
               _WORD ob_drdepth, GRECT *clip, _WORD whandle);
_WORD objc_wedit( OBJECT *ob_edtree, _WORD ob_edobject,
               _WORD ob_edchar, _WORD *ob_edidx, _WORD ob_edkind, _WORD whandle );
_WORD objc_xedit( OBJECT *ob_edtree, _WORD ob_edobject,
               _WORD ob_edchar, _WORD *ob_edidx, _WORD ob_edkind, GRECT *r );


EXTERN_C_END

#endif /* __PORTAES_H__ */
