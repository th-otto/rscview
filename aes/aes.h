#include "config.h"
#include <portab.h>
#include <portaes.h>
#include <portvdi.h>
#include <string.h>
#include "aesdefs.h"
#include "struct.h"
#include "ws.h"
#include "gemlib.h"
#include "gsxdefs.h"
#include "aesutils.h"

#define	ADJ3DPIX    2	/* pixel adjustment for 3D objects */


#define HW(x) (((uint32_t)(uint16_t)(x) << 16))
#define MAKE_ULONG(hi,lo) (HW(hi) | (uint16_t)(lo))
#define LOWORD(x) ((uint16_t)(uint32_t)(x))
#define HIWORD(x) ((uint16_t)((uint32_t)(x) >> 16))


/*
 * dosif.S/jbind.S
 */
void far_bcha(void);
void far_mcha(void);
void aes_wheel(void);

void unset_aestrap(void);
void set_aestrap(void);
_BOOL aestrap_intercepted(void);

void disable_interrupts(void);
void enable_interrupts(void);
void takeerr(void);
void giveerr(void);
void retake(void);

intptr_t dos_exec(const char *cmd, _WORD mode, const char *tail);

/*
 * gembase.c
 */
/* Ready List Root - a list of AESPDs linked by the p_link field, terminated
 * by zero [see gempd.c function insert_process]
 */
extern AESPD *rlr;

extern AESPD *drl;
extern AESPD *nrl;
extern EVB *eul;
extern EVB *dlr;
extern EVB *zlr;

/* In Dispatch - a byte whose value is zero when not in function
 * dsptch, and 1 when between dsptch ... switchto function calls
 */
extern char indisp;

extern _WORD fpt, fph, fpcnt;		/* forkq tail, head, count */
extern SPB wind_spb;
extern CDA *cda;
extern _WORD curpid;


/*
 * geminit.c
 */
extern _BOOL autoexec;					/* autoexec a file ?    */
extern _BOOL act3dtxt;					/* look of 3D activator text */
extern _BOOL act3dface;					/* selected look of 3D activator */
extern _BOOL ind3dtxt;					/* look of 3D indicator text */
extern _BOOL ind3dface;					/* selected look of 3D indicators */
extern _UWORD gl_indbutcol;				/* indicator button color */
extern _UWORD gl_actbutcol;				/* activator button color */
extern _UWORD gl_alrtcol;				/* alert background color */
extern _BOOL gl_aes3d;
extern char gl_logdrv;
extern _WORD crt_error;					/* critical error handler semaphore */
extern _WORD adeskp[3];					/* desktop colors & backgrounds */
extern OBJECT **aes_rsc_tree;

/*
 * number of ticks since last sample
 * while someone was waiting
 * Incremented by the tick interrupt
 */
extern int32_t NUM_TICK;
/*
 * indicates to tick handler how much time to wait before
 * sending the first tchange
 */
extern int32_t CMP_TICK;

extern void *drwaddr;

void set_mouse_to_arrow(void);
void set_mouse_to_hourglass(void);
void gr_mouse(_WORD mkind, MFORM *grmaddr);
void sh_deskf(_WORD obj, aes_private *priv);
void gem_main(void);
void setres(void);
void pinit(AESPD *ppd, CDA *pcda);
int32_t set_cache(int32_t newcacr);
_WORD pred_dinf(void);
void set_defdrv(void);


/*
 * gemaints.c
 */
void signal(EVB *e);
void zombie(EVB *e);


/*
 * ratrbp.c
 */
void drawrat(_WORD newx, _WORD newy);
void justretf(void);


/*
 * gemaplib.c
 */
extern _WORD gl_bvdisk;
extern _WORD gl_bvhard;
extern _WORD gl_recd;
extern _WORD gl_rlen;
extern uint32_t *gl_rbuf;
extern _WORD gl_play;
extern void *gl_store;
extern _WORD gl_mx;
extern _WORD gl_my;
extern _WORD gl_mnpds[NUM_PDS];

_WORD ap_init(AES_GLOBAL *pglobal);
_WORD ap_exit(void);
_WORD rd_mymsg(void *buffer);
_WORD ap_rdwr(_WORD code, _WORD id, _WORD length, _WORD *pbuff);
_WORD ap_find(const char *pname);
void ap_tplay(const uint32_t *pbuff, _WORD length, _WORD scale);
_WORD ap_trecd(uint32_t *pbuff, _WORD length);



/*
 * gemasync.c
 */
extern _WORD tbutton;
extern _WORD wwait;

EVB *get_evb(void);
void azombie(EVB *e, _UWORD ret);
void evinsert(EVB *e, EVB **root);
void takeoff(EVB *p);
EVSPEC mwait(EVSPEC mask);
EVSPEC iasync(_WORD afunc, intptr_t aparm);
_UWORD apret(EVSPEC mask);
EVSPEC acancel(EVSPEC m);


/*
 * gembind.c
 */
void xif(AESPB *pcrys_blk);


/*
 * gemcli.c
 */
extern ACCPD *gl_pacc[NUM_ACCS];		/* total of 6 desk acc, 1 from rom  */
extern _WORD gl_naccs;
extern char *gl_adacc[NUM_ACCS];		/* addresses of accessories */
extern char *sys_adacc;
extern const char stacc[];

void release(void);
void all_run(void);
_WORD sndcli(char *pfilespec, _WORD acc);
void ldaccs(void);
void free_accs(void);
_BOOL cre_aproc(void);


/*
 * gemctrl.c
 */
extern MOBLK gl_ctwait;
extern _WORD appl_msg[8];
extern _WORD ml_ocnt;

void hctl_window(_WORD w_handle, _WORD mx, _WORD my);
void hctl_button(_WORD mx, _WORD my);
void hctl_rect(_WORD mx, _WORD my);
void ct_chgown(AESPD *ppd, GRECT *pr);
void ctlmgr(void);
AESPD *ictlmgr(_WORD pid);
void ctlmouse(_BOOL mon);
void fm_own(_BOOL beg_ownit);
void akbin(EVB *e);
void adelay(EVB *e, _LONG c);
void abutton(EVB *e, _LONG p);
void amouse(EVB *e, _LONG pmo);


/*
 * gemdisp.c
 */
extern AESPD *dpd;								/* critical error process   */
extern AESPD *slr;

_BOOL forkq(FCODE f_code, _LONG f_data);
void disp_act(AESPD *p);
void suspend_act(AESPD *p);
void forker(void);
void chkkbd(void);
void disp(void);


/*
 * gemevlib.c
 */
extern _WORD gl_dclick;
extern _WORD gl_ticktime;

void ev_rets(_WORD *rets);
_WORD ev_block(_WORD code, intptr_t lvalue);
_UWORD ev_keybd(void);
_UWORD ev_button(_WORD bflgclks, _UWORD bmask, _UWORD bstate, _WORD *rets);
_UWORD ev_mouse(MOBLK *pmo, _WORD *rets);
_WORD ev_mesag(_WORD *pbuff);
_WORD ev_timer(_LONG count);
_WORD ev_mchk(MOBLK *pmo);
_WORD ev_multi(_WORD flags, MOBLK *pmo1, MOBLK *pmo2, _LONG tmcount, intptr_t buparm, _WORD *mebuff, _WORD *prets);
_WORD ev_dclick(_WORD rate, _WORD setit);


/*
 * gemflag.c
 */
void tchange(_LONG fdata);
_WORD tak_flag(SPB *sy);
void amutex(EVB *e, SPB *sy);
void unsync(SPB *sy);
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
_WORD fm_dial(_WORD fmd_type, GRECT *pi, GRECT *pt);
_WORD fm_show(_WORD string, _WORD level, ...);
_WORD fm_showv(_WORD string, _WORD level, va_list parms);
_WORD eralert(_WORD n, _WORD d);
_BOOL fm_error(_WORD n);


/*
 * gemfslib.c
 */
_WORD fs_input(char *pipath, char *pisel, _WORD *pbutton, char *lstring);
void ini_fsel(void);


/*
 * geminput.c
 */

extern _WORD button;
extern _WORD xrat;
extern _WORD yrat;
extern _WORD kstate;
extern _WORD mclick;
extern _WORD mtrans;

extern _WORD pr_button;
extern _WORD pr_xrat;
extern _WORD pr_yrat;
extern _WORD pr_mclick;

extern AESPD *gl_mowner;		/* current mouse owner  */
extern AESPD *gl_kowner;		/* current keybd owner  */
extern AESPD *gl_cowner;		/* current control rect. owner */
extern AESPD *ctl_pd;
extern GRECT ctrl;
extern _WORD gl_bclick;
extern _WORD gl_bpend;
#if AESVERSION >= 0x330
extern _WORD gl_button;
#endif
extern _WORD gl_bdesired;
extern _WORD gl_btrue;
extern _WORD gl_bdelay;

_UWORD in_mrect(MOBLK *pmo);
void set_ctrl(GRECT *pt);
void get_ctrl(GRECT *pt);
AESPD *mowner(_WORD newpd);
_UWORD dq(CQUEUE *qptr);
void fq(void);
void evremove(EVB *e, _UWORD ret);
void kchange(_LONG fdata); /* HI: char; LO: keyboard states */
void post_keybd(AESPD *p, _UWORD ch);
void bchange(_LONG fdata); /* HI: state; LO: clicks */
_WORD downorup(_WORD newmasks, intptr_t buparm);
void post_button(AESPD * p, _WORD newmask, _WORD clks);
void mchange(_LONG fdata);
void post_mouse(AESPD *p, _WORD grx, _WORD gry);
_WORD inorout(EVB *e, _WORD rx, _WORD ry);


/*
 * gemmnlib.c
 */
extern OBJECT *gl_mntree;
extern AESPD *gl_mnppd;
extern GRECT gl_rmnactv;
extern char *desk_acc[MAX_ACCS];
extern _WORD desk_pid[MAX_ACCS];
extern _WORD gl_dacnt;
extern _WORD gl_dabase;
extern _WORD gl_dabox;

_UWORD do_chg(OBJECT *tree, _WORD iitem, _UWORD chgvalue, _WORD dochg, _WORD dodraw, _WORD chkdisabled);
_WORD mn_do(_WORD *ptitle, _WORD *pitem);
void mn_bar(OBJECT *tree, _WORD showit, _WORD pid);
void mn_clsda(void);
_WORD mn_register(_WORD pid, char *pstr);
void mn_unregister(_WORD da_id);
void mn_getownid(AESPD **owner,_WORD *id,_WORD item);
void ch_wrect(GRECT *r, GRECT *n);


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
void gr_2box(_WORD flag1, _WORD cnt, GRECT *pt, _WORD xstep, _WORD ystep, _WORD flag2);
void gr_clamp(_WORD xorigin, _WORD yorigin, _WORD wmin, _WORD hmin, _WORD *pneww, _WORD *pnewh);
_WORD gr_slidebox(OBJECT *tree, _WORD parent, _WORD obj, _WORD isvert);
_WORD gr_mkstate(_WORD *pmx, _WORD *pmy, _WORD *pmstat, _WORD *pkstat);

/*
 * gemgraf.c
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
 * gemqueue.c
 */
void aqueue(_BOOL isqwrite, EVB *e, intptr_t lm);


/*
 * gemrlist.c
 */
_BOOL delrect(RLIST *rp, void *rlist);
RLIST *genrlist(_UWORD handle, _UWORD area);



/*
 * gemrom.c
 */
_WORD rom_ram(int which, void *pointer);
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


/*
 * gemsclib.c
 */
#define SCRAP_DIR_NAME  "C:\\CLIPBRD"

_WORD sc_read(char *pscrap);
_WORD sc_write(const char *pscrap);
_WORD sc_clear(void);


/*
 * gemshlib.c
 */

extern _WORD sh_doexec;
extern _WORD sh_isgem;
extern _WORD gl_shgem;
extern char *ad_envrn;
extern char *ad_shcmd;
extern char *ad_shtail;
extern _BOOL sh_iscart;
extern char *ad_path;
extern SHELL sh[];
extern _WORD gl_changerez;
extern _WORD gl_nextrez;

_WORD sh_read(char *pcmd, char *ptail);
_WORD sh_write(_WORD doex, _WORD isgem, _WORD isover, const char *pcmd, const char *ptail);
_WORD sh_get(char *pbuffer, _WORD len);
_WORD sh_put(const char *pdata, _WORD len);
_BOOL sh_tographic(void);
_BOOL sh_toalpha(void);
void sh_draw(char *lcmd, _WORD start, _WORD depth);
char *sh_name(char *ppath);
_WORD sh_envrn(char **ppath, const char *psrch);
typedef void (*SHFIND_PROC)(const char *path);
_WORD sh_find(char *pspec, SHFIND_PROC routine);
void sh_rdef(char *lpcmd, char *lpdir);
void sh_wdef(const char *lpcmd, const char *lpdir);
void sh_main(void);
void sh_curdir(char *ppath);

typedef void PRG_ENTRY(void);   /* Program entry point type */
void aes_run_rom_program(PRG_ENTRY *entry);


/*
 * gemwmlib.c
 */
extern OBJECT *gl_newdesk;
extern _WORD gl_newroot;							/* root object of new DESKTOP */
extern _WORD gl_wtop;
extern intptr_t ad_windspb;

void wm_init(void);
_WORD wm_create(_UWORD kind, GRECT *rect);
_WORD wm_find(int mx, int my);
void wm_min(_WORD kind, _WORD *ow, _WORD *oh);
#if NEWWIN
extern MEMHDR *rmhead, *rmtail;					/* rect lists memory linked list */
WINDOW *srchwp(int handle);
_WORD wm_open(_WORD handle, GRECT *rect);
_WORD wm_close(_WORD handle);
_WORD wm_delete(_WORD handle);
_WORD wm_set(_WORD handle, _WORD field, _WORD *iw);
_WORD wm_calc(_WORD type, _WORD kind, _WORD ix, _WORD iy, _WORD iw, _WORD ih, _WORD *ox, _WORD *oy, _WORD *ow, _WORD *oh);
void w_drawchange(GRECT *dirty, _UWORD skip, _UWORD stop);
#else
#define srchwp(handle) (&D.w_win[handle])
void wm_open(_WORD handle, GRECT *rect);
void wm_close(_WORD handle);
_WORD wm_delete(_WORD handle);
void wm_set(_WORD handle, _WORD field, _WORD *iw);
void wm_calc(_WORD type, _WORD kind, _WORD ix, _WORD iy, _WORD iw, _WORD ih, _WORD *ox, _WORD *oy, _WORD *ow, _WORD *oh);
void w_drawdesk(GRECT *dirty);
void w_update(_WORD bottom, GRECT *pt, _WORD top, _BOOL moved);
_BOOL wm_start(void);
#endif
#if NEWWIN | AES3D
_WORD wm_get(_WORD handle, _WORD field, _WORD *ow, const _WORD *iw);
#else
void wm_get(_WORD handle, _WORD field, _WORD *ow);
#endif
_WORD wm_update(_WORD code);
_WORD wm_new(void);
void w_setactive(void);
void ap_sendmsg(_WORD *ap_msg, _WORD type, AESPD *towhom, _WORD w3, _WORD w4, _WORD w5, _WORD w6, _WORD w7);

#if !NEWWIN
extern _WORD gl_wtop;
extern OBJECT *gl_awind;
void w_getsize(_WORD which, _WORD w_handle, GRECT *pt);
void w_bldactive(_WORD w_handle);
#endif
 

/*
 * gemwrect.c
 */
#if !NEWWIN
extern ORECT *rul;

void or_start(void);
ORECT *get_orect(void);
_WORD newrect(OBJECT *tree, _WORD wh, _WORD junkx, _WORD junky);
#endif


/*
 * jdos.c
 */
int dos_sfirst(const char *name, int attrib);
int dos_snext(void);
int dos_open(const char *name, int mode);
int dos_read(int fd, size_t size, void *buf);
int dos_write(int fd, size_t size, void *buf);
long dos_lseek(int fd, int whence, long offset);
int dos_gdir(int drive, char *pdrvpath);
int dos_mkdir(const char *path, int attr);
int dos_set(int h, _UWORD time, _UWORD date);
int dos_label(int drive, const char *name);
int dos_space(int drive, int32_t *total, int32_t *avail);
int dos_rename(const char *oldname, const char *newname);
void *dos_alloc(long size);
intptr_t dos_avail(void);
int chrout(int c);
int rawcon(int c);
int prt_chr(int c);
int dos_sdta(DTA *dta);
int dos_gdrv(void);
int dos_close(int fd);
int dos_chdir(const char *path);
int dos_sdrv(int drv);
int dos_chmod(const char *path, int attr);
int dos_delete(const char *path);
int dos_free(void *ptr);
int do_cdir(int drv, const char *path);
uint32_t isdrive(void);


/*
 * romcart.c
 */
_BOOL cart_init(void);
_BOOL cart_exec(const char *pcmd, const char *ptail);
_BOOL c_sfirst(const char *path);
_WORD ld_cartacc(void);
_BOOL cart_sfirst(char *pdta, _WORD attr); /* also referenced by DESKTOP */
_BOOL cart_snext(void); /* also referenced by DESKTOP */


/*
 * jdispa.S
 */
void cli(void);
void hcli(void);
void sti(void);
void hsti(void);
/* called repeatedly to give hand to another process - actually a
 * wrapper around disp() in gemdisp.c
 */
void dsptch(void);
void savestate(void);
/* called by disp() to end a dsptch ... switchto sequence */
void switchto(UDA *puda);
/* launches the top of rlr list, as if called from within function
 * back(AESPD *top_of_rlr)
 */
void gotopgm(void);
/* arranges for codevalue to be pushed to the stack for process p,
 * in a standard RTS stack frame, "in preparation for an RTS that
 * will start this process executing".
 */
void psetup(AESPD *pd, void *pcode);
_WORD pgmld(_WORD handle, const char *pname, intptr_t *ldaddr);


void b_delay(_WORD amnt);
void delay(int32_t ticks);


/*
 * gsx2.S
 */
_WORD mul_div(_WORD mul1, _WORD mul2, _WORD divis);


/*
 * gemjstrt.S
 */
extern _BOOL gl_rschange;
extern _WORD gl_restype;


/*
 * someone messed it up and called functions from desktop here...
 */
extern char const infdata[]; /* "DESKTOP.INF" */

#if AESVERSION >= 0x330
extern _BOOL do_once; /* used by desktop only */
#endif

void XDeselect(OBJECT *tree, _WORD obj);
_BOOL getcookie(int32_t cookie, int32_t *val);
char *scan_2(char *pcurr, _WORD *pwd);
char *escan_str(char *pcurr, char *ppstr);
char *save_2(char *pcurr, _UWORD wd);
_BOOL app_reschange(_WORD res);
const char *g_name(const char *file);
_BOOL deskmain(void);
