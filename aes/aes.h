#include "config.h"
#include <portab.h>
#include <portaes.h>
#include <portvdi.h>
#include <string.h>
#include "ws.h"
#include "gsxdefs.h"
#include "aesutils.h"


#define	ADJ3DPIX    2	/* pixel adjustment for 3D objects */



/*
 * geminit.c
 */
extern _BOOL act3dtxt;					/* look of 3D activator text */
extern _BOOL act3dface;					/* selected look of 3D activator */
extern _BOOL ind3dtxt;					/* look of 3D indicator text */
extern _BOOL ind3dface;					/* selected look of 3D indicators */
extern _UWORD gl_indbutcol;				/* indicator button color */
extern _UWORD gl_actbutcol;				/* activator button color */
extern _UWORD gl_alrtcol;					/* alert background color */
extern _BOOL gl_aes3d;

void gr_mouse(_WORD mkind, MFORM *grmaddr);




/*
 * gemflag.c
 */
void fm_strbrk(OBJECT *tree, const char *palstr, _WORD stroff, _WORD *pcurr_id, _WORD *pnitem, _WORD *pmaxlen);
void fm_parse(OBJECT *tree, const char *palstr, _WORD *picnum, _WORD *pnummsg, _WORD *plenmsg, _WORD *pnumbut, _WORD *plenbut);
void fm_build(OBJECT *tree, _WORD haveicon, _WORD nummsg, _WORD mlenmsg, _WORD numbut, _WORD mlenbut);
_WORD fm_alert(_WORD defbut, const char *palstr);


/*
 * gemfmlib.c
 */
_WORD fm_keybd(OBJECT *tree, _WORD obj, _WORD *pchar, _WORD *pnew_obj);
_WORD fm_button(OBJECT *tree, _WORD new_obj, _WORD clks, _WORD *pnew_obj);
_WORD fm_do(OBJECT *tree, _WORD start_fld);
void fm_dial(_WORD fmd_type, GRECT *pi, GRECT *pt);
_WORD fm_show(_WORD string, _WORD level, ...);
_WORD fm_showv(_WORD string, _WORD level, va_list parms);
_WORD eralert(_WORD n, _WORD d);
_BOOL fm_error(_WORD n);


/*
 * gemobed.c
 */
void ob_center(OBJECT *tree, GRECT *pt);
_WORD ob_edit(OBJECT *tree, _WORD obj, _WORD in_char, _WORD *idx, _WORD kind);


/*
 * gemobjop.c
 */
char ob_sst(OBJECT *tree, _WORD obj, OBSPEC *pspec, _WORD *pstate, _WORD *ptype, _WORD *pflags, GRECT *pt, _WORD *pth);
typedef void (*EVERYOBJ_CALLBACK)(OBJECT *tree, _WORD obj, _WORD sx, _WORD sy);
void everyobj(OBJECT *tree, _WORD thisobj, _WORD last, EVERYOBJ_CALLBACK routine, _WORD startx, _WORD starty, _WORD maxdep);
_WORD get_par(OBJECT *tree, _WORD obj);


/*
 * gemoblib.c
 */
_WORD ob_sysvar(_UWORD mode, _UWORD which, _WORD inval1, _WORD inval2, _WORD *outval1, _WORD *outval2);
void ob_format(_WORD just, char *raw_str, char *tmpl_str, char *fmt_str);
void ob_draw(OBJECT *tree, _WORD obj, _WORD depth);
_WORD ob_find(OBJECT *tree, _WORD currobj, _WORD depth, _WORD mx, _WORD my);
void ob_add(OBJECT *tree, _WORD parent, _WORD child);
_BOOL ob_delete(OBJECT *tree, _WORD obj);
_BOOL ob_order(OBJECT *tree, _WORD mov_obj, _WORD new_pos);
_BOOL ob_change(OBJECT *tree, _WORD obj, _WORD new_state, _WORD redraw);
_UWORD ob_fs(OBJECT *tree, _WORD ob, _WORD *pflag);
void ob_actxywh(OBJECT *tree, _WORD obj, GRECT *pt);
void ob_relxywh(OBJECT *tree, _WORD obj, GRECT *pt);
void ob_setxywh(OBJECT *tree, _WORD obj, const GRECT *pt);
void ob_offset(OBJECT *tree, _WORD obj, _WORD *pxoff, _WORD *pyoff);
void ob_dxywh(OBJECT *tree, _WORD obj, _WORD *pdx, _WORD *pdy, _WORD *pdw, _WORD *pdh);
void ob_gclip(OBJECT *tree, _WORD obj, _WORD *pxoff, _WORD *pyoff, _WORD *pxcl, _WORD *pycl, _WORD *pwcl, _WORD *phcl);
CICON *match_planes(CICON *iconlist, int planes);
CICON *find_eq_or_less(CICON *iconlist, int planes);
void gr_cicon(_WORD state, _WORD *pmask, _WORD *pdata, const char *ptext, _WORD ch, _WORD chx, _WORD chy, GRECT *pi, GRECT *pt, CICONBLK *cicon);
void gsx_cblt(_WORD *saddr, _UWORD sx, _UWORD sy, _UWORD swb, _WORD *daddr, _UWORD dx, _UWORD dy, _UWORD dwb, _UWORD w, _UWORD h, _UWORD rule, _WORD numplanes);
void get_color_rsc(CICONBLK **cicondata);
void free_cicon(CICONBLK **carray);


/*
 * apgrlib.[cS]
 */
void gr_inside(GRECT *pt, _WORD th);
void gr_rect(_UWORD icolor, _UWORD ipattern, GRECT *pt);
_WORD gr_just(_WORD just, _WORD font, const char *ptext, _WORD w, _WORD h, GRECT *pt);
void gr_gtext(_WORD just, _WORD font, const char *ptext, GRECT *pt, _WORD tmode);
void gr_crack(_UWORD color, _WORD *pbc, _WORD *ptc, _WORD *pip, _WORD *pic, _WORD *pmd);
void gr_gicon(_WORD state, _WORD *pmask, _WORD *pdata, const char *ptext, _WORD ch, _WORD chx, _WORD chy, GRECT *pi, GRECT *pt);
void gr_box(_WORD x, _WORD y, _WORD w, _WORD h, _WORD th);


/*
 * gemgrlib.[cS]
 */
void gr_stepcalc(_WORD orgw, _WORD orgh, GRECT *pt, _WORD *pcx, _WORD *pcy, _WORD *pcnt, _WORD *pxstep, _WORD *pystep);
void gr_growbox(GRECT *po, GRECT *pt);
void gr_shrinkbox(GRECT *po, GRECT *pt);
void gr_xor(_WORD clipped, _WORD cnt, _WORD cx, _WORD cy, _WORD cw, _WORD ch, _WORD xstep, _WORD ystep, _WORD dowdht);
void gr_movebox(_WORD w, _WORD h, _WORD srcx, _WORD srcy, _WORD dstx, _WORD dsty);
void gr_scale(_WORD xdist, _WORD ydist, _WORD *pcnt, _WORD *pxstep, _WORD *pystep);
_WORD gr_watchbox(OBJECT *tree, _WORD obj, _WORD instate, _WORD outstate);
_BOOL gr_stilldn(_WORD out, _WORD x, _WORD y, _WORD w, _WORD h);
void gr_draw(_WORD have2box, GRECT *po, GRECT *poff);
_WORD gr_wait(GRECT *po, GRECT *poff, _WORD mx, _WORD my);
void gr_setup(_WORD color);
void gr_rubbox(_WORD xorigin, _WORD yorigin, _WORD wmin, _WORD hmin, _WORD *pwend, _WORD *phend);
void gr_rubwind(_WORD xorigin, _WORD yorigin, _WORD wmin, _WORD hmin, GRECT *poff, _WORD *pwend, _WORD *phend);
void gr_dragbox(_WORD w, _WORD h, _WORD sx, _WORD sy, GRECT *pc, _WORD *pdx, _WORD *pdy);
void gr_clamp(_WORD xorigin, _WORD yorigin, _WORD wmin, _WORD hmin, _WORD *pneww, _WORD *pnewh);
_WORD gr_slidebox(OBJECT *tree, _WORD parent, _WORD obj, _WORD isvert);
_WORD gr_mkstate(_WORD *pmx, _WORD *pmy, _WORD *pmstat, _WORD *pkstat);

/*
 * apgsxif.[cS]
 */
#define ORGADDR NULL

extern _WORD gl_nplanes;					/* number of planes in current res */
extern _WORD gl_width;
extern _WORD gl_height;
extern _WORD gl_nrows;
extern _WORD gl_ncols;
extern _WORD gl_wchar;
extern _WORD gl_hchar;
extern _WORD gl_wschar;
extern _WORD gl_hschar;
extern _WORD gl_wptschar;
extern _WORD gl_hptschar;
extern _WORD gl_wsptschar;
extern _WORD gl_hsptschar;
extern _WORD gl_wbox;
extern _WORD gl_hbox;
extern _WORD gl_xclip;
extern _WORD gl_yclip;
extern _WORD gl_wclip;
extern _WORD gl_hclip;
extern _WORD gl_nplanes;
extern _WORD gl_handle;
extern _WORD gl_mode;
extern _WORD gl_mask; /* unused */
extern _WORD gl_tcolor;
extern _WORD gl_lcolor;
extern _WORD gl_fis;
extern _WORD gl_patt;
extern _WORD gl_font;
extern GRECT gl_rscreen;
extern GRECT gl_rfull;
extern GRECT gl_rzero;
extern GRECT gl_rcenter;
extern GRECT gl_rmenu;




/*
 * gemrom.c
 */
void rsc_free(void);
_BOOL rsc_read(void);


/*
 * gemrslib.c
 */
extern RSHDR *rs_hdr;
extern AES_GLOBAL *rs_global;

void rs_sglobe(AES_GLOBAL *pglobal);
_WORD rs_obfix(OBJECT *tree, _WORD curob);
char *rs_str(_WORD stnum);
_WORD rs_free(AES_GLOBAL *pglobal);
_WORD rs_gaddr(AES_GLOBAL *pglobal, _UWORD rtype, _UWORD rindex, void **rsaddr);
_WORD rs_saddr(AES_GLOBAL *pglobal, _UWORD rtype, _UWORD rindex, void *rsaddr);
void do_rsfix(RSHDR *hdr, _WORD size);
void rs_fixit(AES_GLOBAL *pglobal);
_WORD rs_load(AES_GLOBAL *pglobal, const char *rsfname);


void *dos_alloc(long size);
void dos_free(void *ptr);
