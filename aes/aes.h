#include "config.h"
#include <portab.h>
#include <stdint.h>
#include <portaes.h>
#include <portvdi.h>
#include <string.h>

#include "aesdefs.h"
#include "struct.h"
#include "ws.h"
#include "gemlib.h"
#include "visibility.h"
#include "gsxdefs.h"
#include "aesutils.h"
#include "gemdos.h"

#define	ADJ3DSTD    2	/* standard pixel adjustment for 3D objects */
#define ADJ3DOUT    3	/* pixel adjustment for 3D OUTLINED objects */
#define ADJ3DSHA    2	/* pixel adjustment for 3D SHADOWED objects */

#undef LOWORD /* clashes with Win32 */
#undef HIWORD

#define HW(x) (((uint32_t)(uint16_t)(x) << 16))
#define MAKE_ULONG(hi,lo) (HW(hi) | (uint16_t)(lo))
#define LOWORD(x) ((uint16_t)(uint32_t)(x))
#define HIWORD(x) ((uint16_t)((uint32_t)(x) >> 16))


#define far_bcha            __hidden_aes_far_bcha
#define far_mcha            __hidden_aes_far_mcha
#define aes_wheel           __hidden_aes_wheel
#define tikcod              __hidden_aes_tikcod
#define unset_aestrap       __hidden_aes_unset_aestrap
#define set_aestrap         __hidden_aes_set_aestrap
#define aestrap_intercepted __hidden_aes_trap_intercepted
#define disable_interrupts  __hidden_aes_disable_interrupts
#define enable_interrupts   __hidden_aes_enable_interrupts
#define takeerr             __hidden_aes_takeerr
#define giveerr             __hidden_aes_giveerr
#define retake              __hidden_aes_retake
#define dos_exec            __hidden_aes_dos_exec
#define rlr                 __hidden_aes_rlr
#define curpid              __hidden_aes_curpid
#define autoexec            __hidden_aes_autoexec
#define act3dtxt            __hidden_aes_act3dtxt
#define act3dface           __hidden_aes_act3dface
#define ind3dtxt            __hidden_aes_ind3dtxt
#define ind3dface           __hidden_aes_ind3dface
#define gl_indbutcol        __hidden_aes_gl_indbutcol
#define gl_actbutcol        __hidden_aes_gl_actbutcol
#define gl_alrtcol          __hidden_aes_gl_alrtcol
#define gl_aes3d            __hidden_aes_gl_aes3d
#define adeskp              __hidden_aes_adeskp
#define aes_rsc_tree        __hidden_aes_aes_rsc_tree
#define aes_rsc_bitblk      __hidden_aes_aes_rsc_bitblk
#define NUM_TICK            __hidden_aes_NUM_TICK
#define CMP_TICK            __hidden_aes_CMP_TICK
#define TICKS               __hidden_aes_TICKS
#define aes_init            __hidden_aes_aes_init
#define aes_exit            __hidden_aes_aes_exit
#define set_mouse_to_arrow  __hidden_aes_set_mouse_to_arrow
#define set_mouse_to_hourglass __hidden_aes_set_mouse_to_hourglass
#define gem_main            __hidden_aes_gem_main
#define setres              __hidden_aes_setres
#define pinit               __hidden_aes_pinit
#define set_cache           __hidden_aes_set_cache
#define pred_dinf           __hidden_aes_pred_dinf
#define set_defdrv          __hidden_aes_set_defdrv
#define drawrat             __hidden_aes_drawrat
#define gl_bvdisk           __hidden_aes_gl_bvdisk
#define gl_bvhard           __hidden_aes_gl_bvhard
#define gl_recd             __hidden_aes_gl_recd
#define gl_rlen             __hidden_aes_gl_rlen
#define gl_rbuf             __hidden_aes_gl_rbuf
#define gl_play             __hidden_aes_gl_play
#define gl_store            __hidden_aes_gl_store
#define gl_mx               __hidden_aes_gl_mx
#define gl_my               __hidden_aes_gl_my
#define gl_mnpds            __hidden_aes_gl_mnpds
#define ap_init             __hidden_aes_ap_init
#define ap_exit             __hidden_aes_ap_exit
#define appl_msg            __hidden_aes_appl_msg
#define gl_ctwait           __hidden_aes_gl_ctwait
#define hctl_window         __hidden_aes_hctl_window
#define hctl_button         __hidden_aes_hctl_button
#define hctl_rect           __hidden_aes_hctl_rect
#define ct_chgown           __hidden_aes_ct_chgown
#define ictlmgr             __hidden_aes_ictlmgr
#define ctlmouse            __hidden_aes_ctlmouse
#define fm_own              __hidden_aes_fm_own
#define akbin               __hidden_aes_akbin
#define adelay              __hidden_aes_adelay
#define abutton             __hidden_aes_abutton
#define amouse              __hidden_aes_amouse
#define chk_ctrl            __hidden_aes_chk_ctrl
#define dpd                 __hidden_aes_dpd
#define slr                 __hidden_aes_slr
#define drl                 __hidden_aes_drl
#define gl_pacc             __hidden_aes_gl_pacc
#define gl_naccs            __hidden_aes_gl_naccs
#define forkq               __hidden_aes_forkq
#define disp_act            __hidden_aes_disp_act
#define suspend_act         __hidden_aes_suspend_act
#define forker              __hidden_aes_forker
#define chkkbd              __hidden_aes_chkkbd
#define disp                __hidden_aes_disp
#define gl_dclick           __hidden_aes_gl_dclick
#define gl_ticktime         __hidden_aes_gl_ticktime
#define ev_block            __hidden_aes_ev_block
#define ev_keybd            __hidden_aes_ev_keybd
#define ev_button           __hidden_aes_ev_button
#define ev_mouse            __hidden_aes_ev_mouse
#define ev_mesag            __hidden_aes_ev_mesag
#define ev_timer            __hidden_aes_ev_timer
#define ev_multi            __hidden_aes_ev_multi
#define ev_dclick           __hidden_aes_ev_dclick
#define tchange             __hidden_aes_tchange
#define tak_flag            __hidden_aes_tak_flag
#define amutex              __hidden_aes_amutex
#define unsync              __hidden_aes_unsync
#define fm_keybd            __hidden_aes_fm_keybd
#define fm_button           __hidden_aes_fm_button
#define fm_do               __hidden_aes_fm_do
#define fm_dial             __hidden_aes_fm_dial
#define fm_show             __hidden_aes_fm_show
#define eralert             __hidden_aes_eralert
#define fm_error            __hidden_aes_fm_error
#define fm_alert            __hidden_aes_fm_alert
#define fs_input            __hidden_aes_fs_input
#define fs_start            __hidden_aes_fs_start
#define button              __hidden_aes_button
#define xrat                __hidden_aes_xrat
#define yrat                __hidden_aes_yrat
#define kstate              __hidden_aes_kstate
#define mclick              __hidden_aes_mclick
#define mtrans              __hidden_aes_mtrans
#define pr_button           __hidden_aes_pr_button
#define pr_xrat             __hidden_aes_pr_xrat
#define pr_yrat             __hidden_aes_pr_yrat
#define pr_mclick           __hidden_aes_pr_mclick
#define gl_mowner           __hidden_aes_gl_mowner
#define gl_kowner           __hidden_aes_gl_kowner
#define gl_cowner           __hidden_aes_gl_cowner
#define ctl_pd              __hidden_aes_ctl_pd
#define gl_bclick           __hidden_aes_gl_bclick
#define gl_bpend            __hidden_aes_gl_bpend
#define gl_button           __hidden_aes_gl_button
#define gl_bdesired         __hidden_aes_gl_bdesired
#define gl_btrue            __hidden_aes_gl_btrue
#define gl_bdelay           __hidden_aes_gl_bdelay
#define set_ctrl            __hidden_aes_set_ctrl
#define get_ctrl            __hidden_aes_get_ctrl
#define mowner              __hidden_aes_mowner
#define dq                  __hidden_aes_dq
#define fq                  __hidden_aes_fq
#define evremove            __hidden_aes_evremove
#define kchange             __hidden_aes_kchange
#define post_keybd          __hidden_aes_post_keybd
#define bchange             __hidden_aes_bchange
#define downorup            __hidden_aes_downorup
#define post_button         __hidden_aes_post_button
#define mchange             __hidden_aes_mchange
#define post_mouse          __hidden_aes_post_mouse
#define b_click             __hidden_aes_b_click
#define get_mkown           __hidden_aes_get_mkown
#define set_mkown           __hidden_aes_set_mkown
#define wheel_change        __hidden_aes_wheel_change
#define gl_mntree           __hidden_aes_gl_mntree
#define gl_mnppd            __hidden_aes_gl_mnppd
#define gl_rmnactv          __hidden_aes_gl_rmnactv
#define desk_acc            __hidden_aes_desk_acc
#define desk_pid            __hidden_aes_desk_pid
#define gl_dacnt            __hidden_aes_gl_dacnt
#define gl_dabase           __hidden_aes_gl_dabase
#define gl_dabox            __hidden_aes_gl_dabox
#define gl_mnclick          __hidden_aes_gl_mnclick
#define mn_init             __hidden_aes_mn_init
#define do_chg              __hidden_aes_do_chg
#define mn_do               __hidden_aes_mn_do
#define mn_bar              __hidden_aes_mn_bar
#define mn_clsda            __hidden_aes_mn_clsda
#define mn_register         __hidden_aes_mn_register
#define mn_text             __hidden_aes_mn_text
#define mn_unregister       __hidden_aes_mn_unregister
#define mn_getownid         __hidden_aes_mn_getownid
#define ch_wrect            __hidden_aes_ch_wrect
#define mn_istart           __hidden_aes_mn_istart
#define mn_attach           __hidden_aes_mn_attach
#define mn_popup            __hidden_aes_mn_popup
#define mn_settings         __hidden_aes_mn_settings
#define ob_center           __hidden_aes_ob_center
#define ob_edit             __hidden_aes_ob_edit
#define ob_sst              __hidden_aes_ob_sst
#define everyobj            __hidden_aes_everyobj
#define get_par             __hidden_aes_get_par
#define ob_sysvar           __hidden_aes_ob_sysvar
#define ob_format           __hidden_aes_ob_format
#define ob_draw             __hidden_aes_ob_draw
#define ob_find             __hidden_aes_ob_find
#define ob_add              __hidden_aes_ob_add
#define ob_delete           __hidden_aes_ob_delete
#define ob_order            __hidden_aes_ob_order
#define ob_change           __hidden_aes_ob_change
#define ob_fs               __hidden_aes_ob_fs
#define ob_actxywh          __hidden_aes_ob_actxywh
#define ob_relxywh          __hidden_aes_ob_relxywh
#define ob_setxywh          __hidden_aes_ob_setxywh
#define ob_offset           __hidden_aes_ob_offset
#define ob_dxywh            __hidden_aes_ob_dxywh
#define ob_gclip            __hidden_aes_ob_gclip
#define gr_cicon            __hidden_aes_gr_cicon
#define get_color_rsc       __hidden_aes_get_color_rsc
#define free_cicon          __hidden_aes_free_cicon
#define gr_inside           __hidden_aes_gr_inside
#define gr_rect             __hidden_aes_gr_rect
#define gr_just             __hidden_aes_gr_just
#define gr_gtext            __hidden_aes_gr_gtext
#define gr_crack            __hidden_aes_gr_crack
#define gr_gicon            __hidden_aes_gr_gicon
#define gr_box              __hidden_aes_gr_box
#define gr_stepcalc         __hidden_aes_gr_stepcalc
#define gr_growbox          __hidden_aes_gr_growbox
#define gr_shrinkbox        __hidden_aes_gr_shrinkbox
#define gr_movebox          __hidden_aes_gr_movebox
#define gr_watchbox         __hidden_aes_gr_watchbox
#define gr_rubbox           __hidden_aes_gr_rubbox
#define gr_rubwind          __hidden_aes_gr_rubwind
#define gr_dragbox          __hidden_aes_gr_dragbox
#define gr_slidebox         __hidden_aes_gr_slidebox
#define gr_mkstate          __hidden_aes_gr_mkstate
#define gr_mouse            __hidden_aes_gr_mouse
#define gl_nplanes          __hidden_aes_gl_nplanes
#define gl_width            __hidden_aes_gl_width
#define gl_height           __hidden_aes_gl_height
#define gl_wchar            __hidden_aes_gl_wchar
#define gl_hchar            __hidden_aes_gl_hchar
#define gl_wschar           __hidden_aes_gl_wschar
#define gl_hschar           __hidden_aes_gl_hschar
#define gl_wptschar         __hidden_aes_gl_wptschar
#define gl_hptschar         __hidden_aes_gl_hptschar
#define gl_wsptschar        __hidden_aes_gl_wsptschar
#define gl_hsptschar        __hidden_aes_gl_hsptschar
#define gl_wbox             __hidden_aes_gl_wbox
#define gl_hbox             __hidden_aes_gl_hbox
#define gl_clip             __hidden_aes_gl_clip
#define gl_handle           __hidden_aes_gl_handle
#define gl_mode             __hidden_aes_gl_mode
#define gl_tcolor           __hidden_aes_gl_tcolor
#define gl_lcolor           __hidden_aes_gl_lcolor
#define gl_fis              __hidden_aes_gl_fis
#define gl_patt             __hidden_aes_gl_patt
#define gl_font             __hidden_aes_gl_font
#define gl_rscreen          __hidden_aes_gl_rscreen
#define gl_rfull            __hidden_aes_gl_rfull
#define gl_rzero            __hidden_aes_gl_rzero
#define gl_rcenter          __hidden_aes_gl_rcenter
#define gl_rmenu            __hidden_aes_gl_rmenu
#define aqueue              __hidden_aes_aqueue
#define delrect             __hidden_aes_delrect
#define genrlist            __hidden_aes_genrlist
#define rom_ram             __hidden_aes_rom_ram
#define rsc_free            __hidden_aes_rsc_free
#define rsc_read            __hidden_aes_rsc_read
#define rs_hdr              __hidden_aes_rs_hdr
#define rs_global           __hidden_aes_rs_global
#define rs_sglobe           __hidden_aes_rs_sglobe
#define rs_obfix            __hidden_aes_rs_obfix
#define rs_free             __hidden_aes_rs_free
#define rs_gaddr            __hidden_aes_rs_gaddr
#define rs_saddr            __hidden_aes_rs_saddr
#define do_rsfix            __hidden_aes_do_rsfix
#define rs_fixit            __hidden_aes_rs_fixit
#define rs_load             __hidden_aes_rs_load
#define rs_str              __hidden_aes_rs_str
#define sc_read             __hidden_aes_sc_read
#define sc_write            __hidden_aes_sc_write
#define sc_clear            __hidden_aes_sc_clear
#define sh_doexec           __hidden_aes_sh_doexec
#define sh_isgem            __hidden_aes_sh_isgem
#define gl_shgem            __hidden_aes_gl_shgem
#define ad_envrn            __hidden_aes_ad_envrn
#define ad_shcmd            __hidden_aes_ad_shcmd
#define ad_shtail           __hidden_aes_ad_shtail
#define sh_iscart           __hidden_aes_sh_iscart
#define ad_path             __hidden_aes_ad_path
#define sh                  __hidden_aes_sh
#define gl_changerez        __hidden_aes_gl_changerez
#define gl_nextrez          __hidden_aes_gl_nextrez
#define sh_read             __hidden_aes_sh_read
#define sh_write            __hidden_aes_sh_write
#define sh_get              __hidden_aes_sh_get
#define sh_put              __hidden_aes_sh_put
#define sh_tographic        __hidden_aes_sh_tographic
#define sh_toalpha          __hidden_aes_sh_toalpha
#define sh_draw             __hidden_aes_sh_draw
#define sh_name             __hidden_aes_sh_name
#define sh_envrn            __hidden_aes_sh_envrn
#define sh_find             __hidden_aes_sh_find
#define sh_rdef             __hidden_aes_sh_rdef
#define sh_wdef             __hidden_aes_sh_wdef
#define sh_main             __hidden_aes_sh_main
#define sh_curdir           __hidden_aes_sh_curdir
#define aes_run_rom_program __hidden_aes_aes_run_rom_program
#define gl_newdesk          __hidden_aes_gl_newdesk
#define gl_newroot          __hidden_aes_gl_newroot
#define gl_wtop             __hidden_aes_gl_wtop
#define wm_init             __hidden_aes_wm_init
#define wm_create           __hidden_aes_wm_create
#define wm_find             __hidden_aes_wm_find
#define wm_min              __hidden_aes_wm_min
#define wm_open             __hidden_aes_wm_open
#define wm_close            __hidden_aes_wm_close
#define wm_delete           __hidden_aes_wm_delete
#define wm_set              __hidden_aes_wm_set
#define w_drawdesk          __hidden_aes_w_drawdesk
#define w_update            __hidden_aes_w_update
#define wm_start            __hidden_aes_wm_start
#define wm_calc             __hidden_aes_wm_calc
#define wm_get              __hidden_aes_wm_get
#define wm_update           __hidden_aes_wm_update
#define wm_new              __hidden_aes_wm_new
#define w_setactive         __hidden_aes_w_setactive
#define ap_sendmsg          __hidden_aes_ap_sendmsg
#define gl_wtop             __hidden_aes_gl_wtop
#define gl_awind            __hidden_aes_gl_awind
#define w_getsize           __hidden_aes_w_getsize
#define w_bldactive         __hidden_aes_w_bldactive
#define or_start            __hidden_aes_or_start
#define get_orect           __hidden_aes_get_orect
#define newrect             __hidden_aes_newrect
#define do_cdir             __hidden_aes_do_cdir
#define cart_init           __hidden_aes_cart_init
#define cart_exec           __hidden_aes_cart_exec
#define c_sfirst            __hidden_aes_c_sfirst
#define ld_cartacc          __hidden_aes_ld_cartacc
#define cart_sfirst         __hidden_aes_cart_sfirst
#define cart_snext          __hidden_aes_cart_snext
#define cli                 __hidden_aes_cli
#define hcli                __hidden_aes_hcli
#define sti                 __hidden_aes_sti
#define hsti                __hidden_aes_hsti
#define dsptch              __hidden_aes_dsptch
#define savestate           __hidden_aes_savestate
#define switchto            __hidden_aes_switchto
#define gotopgm             __hidden_aes_gotopgm
#define psetup              __hidden_aes_psetup
#define pgmld               __hidden_aes_pgmld
#define b_delay             __hidden_aes_b_delay
#define delay               __hidden_aes_delay
#define gl_rschange         __hidden_aes_gl_rschange
#define gl_restype          __hidden_aes_gl_restype
#define gl_cmform           __hidden_aes_gl_cmform
#define gl_omform           __hidden_aes_gl_omform
#define infdata             __hidden_aes_infdata
#define getcookie           __hidden_aes_getcookie
#define scan_2              __hidden_aes_scan_2
#define escan_str           __hidden_aes_escan_str
#define save_2              __hidden_aes_save_2
#define app_reschange       __hidden_aes_app_reschange
#define g_name              __hidden_aes_g_name
#define deskmain            __hidden_aes_deskmain


/*
 * dosif.S/jbind.S
 */
#ifdef OS_ATARI
VISIBILITY("hidden") void far_bcha(void);
VISIBILITY("hidden") void far_mcha(void);
VISIBILITY("hidden") void aes_wheel(void);
#else
VISIBILITY("hidden") void far_bcha(_WORD newmask);
VISIBILITY("hidden") void far_mcha(_WORD x, _WORD y);
VISIBILITY("hidden") void aes_wheel(_WORD wheel_number, _WORD wheel_amount);
#endif
VISIBILITY("hidden") void tikcod(void);


VISIBILITY("hidden") void unset_aestrap(void);
VISIBILITY("hidden") void set_aestrap(void);
VISIBILITY("hidden") _BOOL aestrap_intercepted(void);

VISIBILITY("hidden") void disable_interrupts(void);
VISIBILITY("hidden") void enable_interrupts(void);
VISIBILITY("hidden") void takeerr(void);
VISIBILITY("hidden") void giveerr(void);
VISIBILITY("hidden") void retake(void);

VISIBILITY("hidden") intptr_t dos_exec(const char *cmd, _WORD mode, const char *tail);

/*
 * gembase.c
 */
/* Ready List Root - a list of AESPDs linked by the p_link field, terminated
 * by zero [see gempd.c function insert_process]
 */
VISIBILITY("hidden") extern AESPD *rlr;

VISIBILITY("hidden") extern _WORD curpid;


/*
 * geminit.c
 */
VISIBILITY("hidden") extern _BOOL autoexec;					/* autoexec a file ? */
VISIBILITY("hidden") extern _BOOL act3dtxt;					/* look of 3D activator text */
VISIBILITY("hidden") extern _BOOL act3dface;					/* selected look of 3D activator */
VISIBILITY("hidden") extern _BOOL ind3dtxt;					/* look of 3D indicator text */
VISIBILITY("hidden") extern _BOOL ind3dface;					/* selected look of 3D indicators */
VISIBILITY("hidden") extern _UWORD gl_indbutcol;				/* indicator button color */
VISIBILITY("hidden") extern _UWORD gl_actbutcol;				/* activator button color */
VISIBILITY("hidden") extern _UWORD gl_alrtcol;				/* alert background color */
VISIBILITY("hidden") extern _BOOL gl_aes3d;
VISIBILITY("hidden") extern _WORD adeskp[3];					/* desktop colors & backgrounds */
VISIBILITY("hidden") extern OBJECT **aes_rsc_tree;
VISIBILITY("hidden") extern const BITBLK *const *aes_rsc_bitblk;


/*
 * number of ticks since last sample
 * while someone was waiting
 * Incremented by the tick interrupt
 */
VISIBILITY("hidden") extern int32_t NUM_TICK;
/*
 * indicates to tick handler how much time to wait before
 * sending the first tchange
 */
VISIBILITY("hidden") extern int32_t CMP_TICK;
VISIBILITY("hidden") extern int32_t TICKS;

VISIBILITY("hidden") void aes_init(void);
VISIBILITY("hidden") void aes_exit(void);

VISIBILITY("hidden") void set_mouse_to_arrow(void);
VISIBILITY("hidden") void set_mouse_to_hourglass(void);
VISIBILITY("hidden") void gem_main(void);
VISIBILITY("hidden") void setres(void);
VISIBILITY("hidden") void pinit(AESPD *ppd, CDA *pcda);
VISIBILITY("hidden") int32_t set_cache(int32_t newcacr);
VISIBILITY("hidden") _WORD pred_dinf(void);
VISIBILITY("hidden") void set_defdrv(void);


/*
 * ratrbp.c
 */
VISIBILITY("hidden") void drawrat(_WORD newx, _WORD newy);


/*
 * gemaplib.c
 */
VISIBILITY("hidden") extern uint32_t gl_bvdisk;
VISIBILITY("hidden") extern uint32_t gl_bvhard;
VISIBILITY("hidden") extern _WORD gl_recd;
VISIBILITY("hidden") extern _WORD gl_rlen;
VISIBILITY("hidden") extern uint32_t *gl_rbuf;
VISIBILITY("hidden") extern _WORD gl_play;
VISIBILITY("hidden") extern VEX_MOTV gl_store;
VISIBILITY("hidden") extern _WORD gl_mx;
VISIBILITY("hidden") extern _WORD gl_my;
VISIBILITY("hidden") extern _WORD gl_mnpds[NUM_PDS];

VISIBILITY("hidden") _WORD ap_init(AES_GLOBAL *pglobal);
VISIBILITY("hidden") _WORD ap_exit(void);



/*
 * gemctrl.c
 */
VISIBILITY("hidden") extern _WORD appl_msg[8];
VISIBILITY("hidden") extern MOBLK gl_ctwait;

VISIBILITY("hidden") void hctl_window(_WORD w_handle, _WORD mx, _WORD my);
VISIBILITY("hidden") void hctl_button(_WORD mx, _WORD my);
VISIBILITY("hidden") void hctl_rect(_WORD mx, _WORD my);
VISIBILITY("hidden") void ct_chgown(AESPD *ppd, GRECT *pr);
VISIBILITY("hidden") AESPD *ictlmgr(void);
VISIBILITY("hidden") void ctlmouse(_BOOL mon);
VISIBILITY("hidden") void fm_own(_BOOL beg_ownit);
VISIBILITY("hidden") void akbin(EVB *e);
VISIBILITY("hidden") void adelay(EVB *e, _LONG c);
VISIBILITY("hidden") void abutton(EVB *e, _LONG p);
VISIBILITY("hidden") void amouse(EVB *e, MOBLK *pmo);
VISIBILITY("hidden") _WORD chk_ctrl(_WORD mx, _WORD my);


/*
 * gemdisp.c
 */
VISIBILITY("hidden") extern AESPD *dpd;								/* critical error process   */
VISIBILITY("hidden") extern AESPD *slr;
VISIBILITY("hidden") extern AESPD *drl;
VISIBILITY("hidden") extern ACCPD *gl_pacc[NUM_ACCS];		/* total of 6 desk acc, 1 from rom  */
VISIBILITY("hidden") extern _WORD gl_naccs;

VISIBILITY("hidden") _BOOL forkq(FCODE f_code, _LONG f_data);
VISIBILITY("hidden") void disp_act(AESPD *p);
VISIBILITY("hidden") void suspend_act(AESPD *p);
VISIBILITY("hidden") void forker(void);
VISIBILITY("hidden") void chkkbd(void);
VISIBILITY("hidden") void disp(void);


/*
 * gemevlib.c
 */
VISIBILITY("hidden") extern _WORD gl_dclick;
VISIBILITY("hidden") extern _WORD gl_ticktime;

/*
 * combine clicks/mask/state into LONG
 */
#define combine_cms(clicks, mask, state) MAKE_ULONG(clicks, (((mask) << 8) | (state)))

VISIBILITY("hidden") _WORD ev_block(_WORD code, intptr_t lvalue);
VISIBILITY("hidden") _UWORD ev_keybd(void);
VISIBILITY("hidden") _UWORD ev_button(_WORD bflgclks, _UWORD bmask, _UWORD bstate, _WORD *rets);
VISIBILITY("hidden") _UWORD ev_mouse(const MOBLK *pmo, _WORD *rets);
VISIBILITY("hidden") _WORD ev_mesag(_WORD *pbuff);
VISIBILITY("hidden") _WORD ev_timer(_LONG count);
VISIBILITY("hidden") _WORD ev_multi(_WORD flags, const MOBLK *pmo1, const MOBLK *pmo2, _LONG tmcount, intptr_t buparm, _WORD *mebuff, _WORD *prets);
VISIBILITY("hidden") _WORD ev_dclick(_WORD rate, _WORD setit);


/*
 * gemflag.c
 */
VISIBILITY("hidden") void tchange(_LONG fdata);
VISIBILITY("hidden") _WORD tak_flag(SPB *sy);
VISIBILITY("hidden") void amutex(EVB *e, SPB *sy);
VISIBILITY("hidden") void unsync(SPB *sy);


/*
 * gemfmlib.c
 */
VISIBILITY("hidden") _WORD fm_keybd(OBJECT *tree, _WORD obj, _WORD *pchar, _WORD *pnew_obj);
VISIBILITY("hidden") _WORD fm_button(OBJECT *tree, _WORD new_obj, _WORD clks, _WORD *pnew_obj);
VISIBILITY("hidden") _WORD fm_do(OBJECT *tree, _WORD start_fld);
VISIBILITY("hidden") _WORD fm_dial(_WORD fmd_type, const GRECT *pi, const GRECT *pt);
VISIBILITY("hidden") _WORD fm_show(_WORD string, _WORD level, _WORD arg);
VISIBILITY("hidden") _WORD eralert(_WORD n, _WORD d);
VISIBILITY("hidden") _BOOL fm_error(_WORD n);

/* TOS standard form_alert() maximum values */
#define MAX_LINELEN     40
#define MAX_BUTLEN      10
#define TOS_MAX_LINELEN 32
#define TOS_MAX_BUTLEN	10
#define MAX_LINENUM     5
#define MAX_BUTNUM      3

VISIBILITY("hidden") _WORD fm_alert(_WORD defbut, const char *palstr, _UWORD flags);


/*
 * gemfslib.c
 */
VISIBILITY("hidden") _WORD fs_input(char *pipath, char *pisel, _WORD *pbutton, const char *lstring);
VISIBILITY("hidden") void fs_start(void);


/*
 * geminput.c
 */

VISIBILITY("hidden") extern _WORD button;
VISIBILITY("hidden") extern _WORD xrat;
VISIBILITY("hidden") extern _WORD yrat;
VISIBILITY("hidden") extern _WORD kstate;
VISIBILITY("hidden") extern _WORD mclick;
VISIBILITY("hidden") extern _WORD mtrans;

VISIBILITY("hidden") extern _WORD pr_button;
VISIBILITY("hidden") extern _WORD pr_xrat;
VISIBILITY("hidden") extern _WORD pr_yrat;
VISIBILITY("hidden") extern _WORD pr_mclick;

VISIBILITY("hidden") extern AESPD *gl_mowner;		/* current mouse owner  */
VISIBILITY("hidden") extern AESPD *gl_kowner;		/* current keybd owner  */
VISIBILITY("hidden") extern AESPD *gl_cowner;		/* current control rect. owner */
VISIBILITY("hidden") extern AESPD *ctl_pd;
VISIBILITY("hidden") extern _WORD gl_bclick;
VISIBILITY("hidden") extern _WORD gl_bpend;
#if AESVERSION >= 0x330
VISIBILITY("hidden") extern _WORD gl_button;
#endif
VISIBILITY("hidden") extern _WORD gl_bdesired;
VISIBILITY("hidden") extern _WORD gl_btrue;
VISIBILITY("hidden") extern _WORD gl_bdelay;

_UWORD in_mrect(MOBLK *pmo);
VISIBILITY("hidden") void set_ctrl(GRECT *pt);
VISIBILITY("hidden") void get_ctrl(GRECT *pt);
VISIBILITY("hidden") AESPD *mowner(_WORD newpd);
VISIBILITY("hidden") _UWORD dq(CQUEUE *qptr);
VISIBILITY("hidden") void fq(void);
VISIBILITY("hidden") void evremove(EVB *e, _UWORD ret);
VISIBILITY("hidden") void kchange(_LONG fdata); /* HI: char; LO: keyboard states */
VISIBILITY("hidden") void post_keybd(AESPD *p, _UWORD ch);
VISIBILITY("hidden") void bchange(_LONG fdata); /* HI: state; LO: clicks */
VISIBILITY("hidden") _WORD downorup(_WORD newmasks, intptr_t buparm);
VISIBILITY("hidden") void post_button(AESPD * p, _WORD newmask, _WORD clks);
VISIBILITY("hidden") void mchange(_LONG fdata);
VISIBILITY("hidden") void post_mouse(AESPD *p, _WORD grx, _WORD gry);
VISIBILITY("hidden") void b_click(_WORD state);
VISIBILITY("hidden") void get_mkown(AESPD **pmown);
VISIBILITY("hidden") void set_mkown(AESPD *mp, AESPD *kp);
VISIBILITY("hidden") void wheel_change(_LONG parm);


/*
 * gemmnlib.c
 */
VISIBILITY("hidden") extern OBJECT *gl_mntree;
VISIBILITY("hidden") extern AESPD *gl_mnppd;
VISIBILITY("hidden") extern GRECT gl_rmnactv;
VISIBILITY("hidden") extern char *desk_acc[MAX_ACCS];
VISIBILITY("hidden") extern _WORD desk_pid[MAX_ACCS];
VISIBILITY("hidden") extern _WORD gl_dacnt;
VISIBILITY("hidden") extern _WORD gl_dabase;
VISIBILITY("hidden") extern _WORD gl_dabox;
VISIBILITY("hidden") extern _WORD gl_mnclick;

VISIBILITY("hidden") void mn_init(void);
VISIBILITY("hidden") _UWORD do_chg(OBJECT *tree, _WORD iitem, _UWORD chgvalue, _WORD dochg, _WORD dodraw, _WORD chkdisabled);
VISIBILITY("hidden") _WORD mn_do(_WORD *ptitle, _WORD *pitem);
VISIBILITY("hidden") void mn_bar(OBJECT *tree, _WORD showit, _WORD pid);
VISIBILITY("hidden") void mn_clsda(void);
VISIBILITY("hidden") _WORD mn_register(_WORD pid, char *pstr);
VISIBILITY("hidden") void mn_text(OBJECT *tree, _WORD item, const char *ptext);
VISIBILITY("hidden") void mn_unregister(_WORD da_id);
VISIBILITY("hidden") void mn_getownid(AESPD **owner,_WORD *id,_WORD item);
VISIBILITY("hidden") void ch_wrect(GRECT *r, GRECT *n);
VISIBILITY("hidden") _WORD mn_istart(_WORD id, _WORD flag, OBJECT *tree, _WORD menu, _WORD item);
VISIBILITY("hidden") _BOOL mn_attach(_WORD id, _WORD flag, OBJECT *tree, _WORD item, MENU *Menu);
VISIBILITY("hidden") _BOOL mn_popup(_WORD id, MENU *Menu, _WORD xpos, _WORD ypos, MENU *MData);
VISIBILITY("hidden") void mn_settings(_WORD flag, MN_SET *Values);


/*
 * gemobed.c
 */
VISIBILITY("hidden") void ob_center(OBJECT *tree, GRECT *pt);
VISIBILITY("hidden") _WORD ob_edit(OBJECT *tree, _WORD obj, _WORD in_char, _WORD *idx, _WORD kind);


/*
 * gemobjop.c
 */
typedef void (*EVERYOBJ_CALLBACK)(OBJECT *tree, _WORD obj, _WORD sx, _WORD sy);

VISIBILITY("hidden") char ob_sst(OBJECT *tree, _WORD obj, OBSPEC *__restrict pspec, _WORD *__restrict pstate, _WORD *__restrict ptype, _WORD *__restrict pflags, GRECT *__restrict pt, _WORD *__restrict pth);
VISIBILITY("hidden") void everyobj(OBJECT *tree, _WORD thisobj, _WORD last, EVERYOBJ_CALLBACK routine, _WORD startx, _WORD starty, _WORD maxdep);
VISIBILITY("hidden") _WORD get_par(OBJECT *tree, _WORD obj);


/*
 * gemoblib.c
 */
VISIBILITY("hidden") _WORD ob_sysvar(_UWORD mode, _UWORD which, _WORD inval1, _WORD inval2, _WORD *outval1, _WORD *outval2);
VISIBILITY("hidden") void ob_format(_WORD just, char *raw_str, char *tmpl_str, char *fmt_str);
VISIBILITY("hidden") void ob_draw(OBJECT *tree, _WORD obj, _WORD depth);
VISIBILITY("hidden") _WORD ob_find(OBJECT *tree, _WORD currobj, _WORD depth, _WORD mx, _WORD my);
VISIBILITY("hidden") void ob_add(OBJECT *tree, _WORD parent, _WORD child);
VISIBILITY("hidden") _BOOL ob_delete(OBJECT *tree, _WORD obj);
VISIBILITY("hidden") _BOOL ob_order(OBJECT *tree, _WORD mov_obj, _WORD new_pos);
VISIBILITY("hidden") _BOOL ob_change(OBJECT *tree, _WORD obj, _WORD new_state, _WORD redraw);
VISIBILITY("hidden") _UWORD ob_fs(OBJECT *tree, _WORD ob, _WORD *pflag);
VISIBILITY("hidden") void ob_actxywh(OBJECT *tree, _WORD obj, GRECT *pt);
VISIBILITY("hidden") void ob_relxywh(OBJECT *tree, _WORD obj, GRECT *pt);
VISIBILITY("hidden") void ob_setxywh(OBJECT *tree, _WORD obj, const GRECT *pt);
VISIBILITY("hidden") void ob_offset(OBJECT *tree, _WORD obj, _WORD *pxoff, _WORD *pyoff);
VISIBILITY("hidden") void ob_dxywh(OBJECT *tree, _WORD obj, _WORD *pdx, _WORD *pdy, _WORD *pdw, _WORD *pdh);
VISIBILITY("hidden") void ob_gclip(OBJECT *tree, _WORD obj, _WORD *pxoff, _WORD *pyoff, _WORD *pxcl, _WORD *pycl, _WORD *pwcl, _WORD *phcl);
VISIBILITY("hidden") void gr_cicon(_WORD state, _WORD *pmask, _WORD *pdata, const char *ptext, vdi_wchar_t ch, _WORD chx, _WORD chy, GRECT *pi, GRECT *pt, CICONBLK *cicon);
VISIBILITY("hidden") void get_color_rsc(CICONBLK **cicondata);
VISIBILITY("hidden") void free_cicon(CICONBLK **carray);


/*
 * gemgraf.c
 */
VISIBILITY("hidden") void gr_inside(GRECT *pt, _WORD th);
VISIBILITY("hidden") void gr_rect(_UWORD icolor, _UWORD ipattern, GRECT *pt);
VISIBILITY("hidden") _WORD gr_just(_WORD just, _WORD font, const char *ptext, _WORD w, _WORD h, GRECT *pt, vdi_wchar_t *textout);
VISIBILITY("hidden") void gr_gtext(_WORD just, _WORD font, const char *ptext, GRECT *pt);
VISIBILITY("hidden") void gr_crack(_UWORD color, _WORD *pbc, _WORD *ptc, _WORD *pip, _WORD *pic, _WORD *pmd);
VISIBILITY("hidden") void gr_gicon(_WORD state, _WORD *pmask, _WORD *pdata, const char *ptext, vdi_wchar_t ch, _WORD chx, _WORD chy, GRECT *pi, GRECT *pt);
VISIBILITY("hidden") void gr_box(_WORD x, _WORD y, _WORD w, _WORD h, _WORD th);


/*
 * gemgrlib.c
 */
VISIBILITY("hidden") void gr_stepcalc(_WORD orgw, _WORD orgh, const GRECT *pt, _WORD *pcx, _WORD *pcy, _WORD *pcnt, _WORD *pxstep, _WORD *pystep);
VISIBILITY("hidden") void gr_growbox(const GRECT *po, const GRECT *pt);
VISIBILITY("hidden") void gr_shrinkbox(const GRECT *po, const GRECT *pt);
VISIBILITY("hidden") void gr_movebox(_WORD w, _WORD h, _WORD srcx, _WORD srcy, _WORD dstx, _WORD dsty);
VISIBILITY("hidden") _WORD gr_watchbox(OBJECT *tree, _WORD obj, _WORD instate, _WORD outstate);
VISIBILITY("hidden") void gr_rubbox(_WORD xorigin, _WORD yorigin, _WORD wmin, _WORD hmin, _WORD *pwend, _WORD *phend);
VISIBILITY("hidden") void gr_rubwind(_WORD xorigin, _WORD yorigin, _WORD wmin, _WORD hmin, GRECT *poff, _WORD *pwend, _WORD *phend);
VISIBILITY("hidden") void gr_dragbox(_WORD w, _WORD h, _WORD sx, _WORD sy, const GRECT *pc, _WORD *pdx, _WORD *pdy);
VISIBILITY("hidden") _WORD gr_slidebox(OBJECT *tree, _WORD parent, _WORD obj, _WORD isvert);
VISIBILITY("hidden") _WORD gr_mkstate(_WORD *pmx, _WORD *pmy, _WORD *pmstat, _WORD *pkstat);
VISIBILITY("hidden") void gr_mouse(_WORD mkind, MFORM *grmaddr);


/*
 * gemgraf.c
 */
#define ORGADDR NULL

VISIBILITY("hidden") extern _WORD gl_nplanes;					/* number of planes in current res */
VISIBILITY("hidden") extern _WORD gl_width;
VISIBILITY("hidden") extern _WORD gl_height;
VISIBILITY("hidden") extern _WORD gl_wchar;
VISIBILITY("hidden") extern _WORD gl_hchar;
VISIBILITY("hidden") extern _WORD gl_wschar;
VISIBILITY("hidden") extern _WORD gl_hschar;
VISIBILITY("hidden") extern _WORD gl_wptschar;
VISIBILITY("hidden") extern _WORD gl_hptschar;
VISIBILITY("hidden") extern _WORD gl_wsptschar;
VISIBILITY("hidden") extern _WORD gl_hsptschar;
VISIBILITY("hidden") extern _WORD gl_wbox;
VISIBILITY("hidden") extern _WORD gl_hbox;
VISIBILITY("hidden") extern GRECT gl_clip;
VISIBILITY("hidden") extern _WORD gl_handle;
VISIBILITY("hidden") extern _WORD gl_mode;
VISIBILITY("hidden") extern _WORD gl_tcolor;
VISIBILITY("hidden") extern _WORD gl_lcolor;
VISIBILITY("hidden") extern _WORD gl_fis;
VISIBILITY("hidden") extern _WORD gl_patt;
VISIBILITY("hidden") extern _WORD gl_font;
VISIBILITY("hidden") extern GRECT gl_rscreen;
VISIBILITY("hidden") extern GRECT gl_rfull;
VISIBILITY("hidden") extern GRECT gl_rzero;
VISIBILITY("hidden") extern GRECT gl_rcenter;
VISIBILITY("hidden") extern GRECT gl_rmenu;



/*
 * gemqueue.c
 */
VISIBILITY("hidden") void aqueue(_BOOL isqwrite, EVB *e, intptr_t lm);


/*
 * gemrlist.c
 */
VISIBILITY("hidden") _BOOL delrect(RLIST *rp, void *rlist);
VISIBILITY("hidden") RLIST *genrlist(_UWORD handle, _UWORD area);



/*
 * gemrom.c
 */
VISIBILITY("hidden") _WORD rom_ram(int which, void *pointer);
VISIBILITY("hidden") void rsc_free(void);
VISIBILITY("hidden") _BOOL rsc_read(void);


/*
 * gemrslib.c
 */
VISIBILITY("hidden") extern RSHDR *rs_hdr;
VISIBILITY("hidden") extern AES_GLOBAL *rs_global;

VISIBILITY("hidden") void rs_sglobe(AES_GLOBAL *pglobal);
VISIBILITY("hidden") void rs_obfix(OBJECT *tree, _WORD curob);
VISIBILITY("hidden") _WORD rs_free(AES_GLOBAL *pglobal);
VISIBILITY("hidden") _WORD rs_gaddr(AES_GLOBAL *pglobal, _UWORD rtype, _UWORD rindex, void **rsaddr);
VISIBILITY("hidden") _WORD rs_saddr(AES_GLOBAL *pglobal, _UWORD rtype, _UWORD rindex, void *rsaddr);
VISIBILITY("hidden") void do_rsfix(RSHDR *hdr, _LONG size);
VISIBILITY("hidden") void rs_fixit(AES_GLOBAL *pglobal);
VISIBILITY("hidden") _WORD rs_load(AES_GLOBAL *pglobal, const char *rsfname);
VISIBILITY("hidden") char *rs_str(_UWORD stnum);


/*
 * gemsclib.c
 */
#define SCRAP_DIR_NAME  "C:\\CLIPBRD"

VISIBILITY("hidden") _WORD sc_read(char *pscrap);
VISIBILITY("hidden") _WORD sc_write(const char *pscrap);
VISIBILITY("hidden") _WORD sc_clear(void);


/*
 * gemshlib.c
 */

VISIBILITY("hidden") extern _WORD sh_doexec;
VISIBILITY("hidden") extern _WORD sh_isgem;
VISIBILITY("hidden") extern _WORD gl_shgem;
VISIBILITY("hidden") extern char *ad_envrn;
VISIBILITY("hidden") extern char *ad_shcmd;
VISIBILITY("hidden") extern char *ad_shtail;
VISIBILITY("hidden") extern _BOOL sh_iscart;
VISIBILITY("hidden") extern char *ad_path;
VISIBILITY("hidden") extern SHELL sh[];
VISIBILITY("hidden") extern _WORD gl_changerez;
VISIBILITY("hidden") extern _WORD gl_nextrez;

VISIBILITY("hidden") _WORD sh_read(char *pcmd, char *ptail);
VISIBILITY("hidden") _WORD sh_write(_WORD doex, _WORD isgem, _WORD isover, const char *pcmd, const char *ptail);
VISIBILITY("hidden") _WORD sh_get(char *pbuffer, _WORD len);
VISIBILITY("hidden") _WORD sh_put(const char *pdata, _WORD len);
VISIBILITY("hidden") _BOOL sh_tographic(void);
VISIBILITY("hidden") _BOOL sh_toalpha(void);
VISIBILITY("hidden") void sh_draw(char *lcmd, _WORD start, _WORD depth);
VISIBILITY("hidden") char *sh_name(char *ppath);
VISIBILITY("hidden") _WORD sh_envrn(char **ppath, const char *psrch);
VISIBILITY("hidden") _WORD sh_find(char *pspec);
VISIBILITY("hidden") void sh_rdef(char *lpcmd, char *lpdir);
VISIBILITY("hidden") void sh_wdef(const char *lpcmd, const char *lpdir);
VISIBILITY("hidden") void sh_main(void);
VISIBILITY("hidden") void sh_curdir(char *ppath);

typedef void PRG_ENTRY(void);   /* Program entry point type */
VISIBILITY("hidden") void aes_run_rom_program(PRG_ENTRY *entry);


/*
 * gemwmlib.c
 */
VISIBILITY("hidden") extern OBJECT *gl_newdesk;
VISIBILITY("hidden") extern _WORD gl_newroot;							/* root object of new DESKTOP */
VISIBILITY("hidden") extern _WORD gl_wtop;

VISIBILITY("hidden") void wm_init(void);
VISIBILITY("hidden") _WORD wm_create(_UWORD kind, const GRECT *rect);
VISIBILITY("hidden") _WORD wm_find(_WORD mx, _WORD my);
VISIBILITY("hidden") void wm_min(_WORD kind, _WORD *ow, _WORD *oh);
VISIBILITY("hidden") void wm_open(_WORD handle, const GRECT *rect);
VISIBILITY("hidden") void wm_close(_WORD handle);
VISIBILITY("hidden") _WORD wm_delete(_WORD handle);
VISIBILITY("hidden") _WORD wm_set(_WORD handle, _WORD field, const _WORD *iw);
#define srchwp(handle) (&D.w_win[handle])
VISIBILITY("hidden") void w_drawdesk(const GRECT *dirty);
VISIBILITY("hidden") void w_update(_WORD bottom, const GRECT *pt, _WORD top, _BOOL moved);
VISIBILITY("hidden") _BOOL wm_start(void);
VISIBILITY("hidden") _WORD wm_calc(_WORD type, _WORD kind, const GRECT *in, GRECT *out);
VISIBILITY("hidden") _WORD wm_get(_WORD handle, _WORD field, _WORD *ow, const _WORD *iw);
VISIBILITY("hidden") _WORD wm_update(_WORD code);
VISIBILITY("hidden") _WORD wm_new(void);
VISIBILITY("hidden") void w_setactive(void);
VISIBILITY("hidden") void ap_sendmsg(_WORD *ap_msg, _WORD type, AESPD *towhom, _WORD w3, _WORD w4, _WORD w5, _WORD w6, _WORD w7);

VISIBILITY("hidden") extern _WORD gl_wtop;
VISIBILITY("hidden") extern OBJECT *gl_awind;
VISIBILITY("hidden") void w_getsize(_WORD which, _WORD w_handle, GRECT *pt);
VISIBILITY("hidden") void w_bldactive(_WORD w_handle);
 

/*
 * gemwrect.c
 */
VISIBILITY("hidden") void or_start(void);
VISIBILITY("hidden") ORECT *get_orect(void);
VISIBILITY("hidden") void newrect(OBJECT *tree, _WORD wh, _WORD junkx, _WORD junky);


/*
 * jdos.c
 */
VISIBILITY("hidden") int do_cdir(int drv, const char *path);


/*
 * romcart.c
 */
VISIBILITY("hidden") _BOOL cart_init(void);
VISIBILITY("hidden") _BOOL cart_exec(const char *pcmd, const char *ptail);
VISIBILITY("hidden") _BOOL c_sfirst(const char *path);
VISIBILITY("hidden") _WORD ld_cartacc(void);
VISIBILITY("hidden") _BOOL cart_sfirst(char *pdta, _WORD attr); /* also referenced by DESKTOP */
VISIBILITY("hidden") _BOOL cart_snext(void); /* also referenced by DESKTOP */


/*
 * jdispa.S
 */
VISIBILITY("hidden") void cli(void);
VISIBILITY("hidden") void hcli(void);
VISIBILITY("hidden") void sti(void);
VISIBILITY("hidden") void hsti(void);
/* called repeatedly to give hand to another process - actually a
 * wrapper around disp() in gemdisp.c
 */
VISIBILITY("hidden") void dsptch(void);
VISIBILITY("hidden") void savestate(void);
/* called by disp() to end a dsptch ... switchto sequence */
VISIBILITY("hidden") void switchto(UDA *puda);
/* launches the top of rlr list, as if called from within function
 * back(AESPD *top_of_rlr)
 */
VISIBILITY("hidden") void gotopgm(void);
/* arranges for codevalue to be pushed to the stack for process p,
 * in a standard RTS stack frame, "in preparation for an RTS that
 * will start this process executing".
 */
VISIBILITY("hidden") void psetup(AESPD *pd, PFVOID pcode);
VISIBILITY("hidden") _WORD pgmld(_WORD handle, const char *pname, intptr_t *ldaddr);


VISIBILITY("hidden") void b_delay(_WORD amnt);
VISIBILITY("hidden") void delay(int32_t ticks);


/*
 * gemgsxif.c
 */
VISIBILITY("hidden") extern _BOOL gl_rschange;
VISIBILITY("hidden") extern _WORD gl_restype;
VISIBILITY("hidden") extern MFORM gl_cmform;				/* current aes mouse form   */
VISIBILITY("hidden") extern MFORM gl_omform;				/* old aes mouse form       */


VISIBILITY("hidden") extern char const infdata[]; /* "DESKTOP.INF" */

VISIBILITY("hidden") _BOOL getcookie(int32_t cookie, int32_t *val);
VISIBILITY("hidden") char *scan_2(char *pcurr, _WORD *pwd);
VISIBILITY("hidden") char *escan_str(char *pcurr, char *ppstr);
VISIBILITY("hidden") char *save_2(char *pcurr, _UWORD wd);
VISIBILITY("hidden") _BOOL app_reschange(_WORD res);
VISIBILITY("hidden") const char *g_name(const char *file);
VISIBILITY("hidden") _BOOL deskmain(void);


/*
 * aes.c
 */
VISIBILITY("default") _WORD aestrap(AESPB *pb);



#undef ctlmouse
#undef wm_update
#undef get_ctrl
#undef get_mkown
#undef ct_chgown
#undef sound
#undef fq
#undef ev_multi
#undef ev_button
#undef ap_rdwr
#undef sh_find
#undef dsptch
#undef post_button
#undef psetup

#define ctlmouse(x)
#define wm_update(x)
#define get_ctrl(pt)
#define get_mkown(m)
#define ct_chgown(a, b) (void)(a), (void)(b)
#define sound(a, b, c)
#define fq()
#define ev_multi(flags, mo1, mo2, count, parm, mebuff, rets) (rets[0] = 0, rets[1] = 0, rets[2] = 0, rets[3] = 0, rets[5] = 0, rets[4] = 0x1c0d, (void)(mo1), (void)(rets), MU_KEYBD)
#define ev_button(a, b, c, rets) (void)(rets)
#define ap_rdwr(a, b, c, d) (void)(b)
#define sh_find(name) 1
#define dsptch()
#define post_button(a, b, c)
#define psetup(a, b) (void)(b)
