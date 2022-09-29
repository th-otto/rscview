/*	GSXDEFS.H	05/06/84 - 12/08/84	Lee Lorenzen		*/

/*
 *       Copyright 1999, Caldera Thin Clients, Inc.                      
 *       This software is licenced under the GNU Public License.         
 *       Please see LICENSE.TXT for further information.                 
 *                                                                       
 *                  Historical Copyright
 *	-------------------------------------------------------------
 *	GEM Application Environment Services		  Version 3.0
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1986			Digital Research Inc.
 *	-------------------------------------------------------------
 */

#ifndef GSXDEFS_H
#define GSXDEFS_H 1

#include "ws.h"

typedef void (*PFVOID)(void);

#define gl_moff     __hidden_aes_gl_moff
#define gl_mouse    __hidden_aes_gl_mouse
#define tiksav      __hidden_aes_tiksav
#define gsx_malloc  __hidden_aes_gsx_malloc
#define gsx_mfree   __hidden_aes_gsx_mfree
#define gsx_mret    __hidden_aes_gsx_mret
#define gsx_init    __hidden_aes_gsx_init
#define gsx_graphic __hidden_aes_gsx_graphic
#define gsx_wsclose __hidden_aes_gsx_wsclose
#define gsx_wsclear __hidden_aes_gsx_wsclear
#define ratinit     __hidden_aes_ratinit
#define ratexit     __hidden_aes_ratexit
#define bb_save     __hidden_aes_bb_save
#define bb_restore  __hidden_aes_bb_restore
#define gsx_mfset   __hidden_aes_gsx_mfset
#define gsx_xmfset  __hidden_aes_gsx_xmfset
#define gsx_mxmy    __hidden_aes_gsx_mxmy
#define gsx_tick    __hidden_aes_gsx_tick
#define gsx_button  __hidden_aes_gsx_button
#define gsx_kstate  __hidden_aes_gsx_kstate
#define gsx_moff    __hidden_aes_gsx_moff
#define gsx_mon     __hidden_aes_gsx_mon
#define gsx_char    __hidden_aes_gsx_char
#define gsx_nplanes __hidden_aes_gsx_nplanes
#define sound       __hidden_aes_sound
#define gsx_fix     __hidden_aes_gsx_fix
#define gsx_blt     __hidden_aes_gsx_blt
#define gsx_sclip   __hidden_aes_gsx_sclip
#define gsx_gclip   __hidden_aes_gsx_gclip
#define gsx_chkclip __hidden_aes_gsx_chkclip
#define gsx_cline   __hidden_aes_gsx_cline
#define gsx_attr    __hidden_aes_gsx_attr
#define gsx_box     __hidden_aes_gsx_box
#define bb_screen   __hidden_aes_bb_screen
#define gsx_trans   __hidden_aes_gsx_trans
#define gsx_start   __hidden_aes_gsx_start
#define bb_fill     __hidden_aes_bb_fill
#define gsx_tcalc   __hidden_aes_gsx_tcalc
#define gsx_tblt    __hidden_aes_gsx_tblt
#define gsx_xbox    __hidden_aes_gsx_xbox
#define gsx_xcbox   __hidden_aes_gsx_xcbox


VISIBILITY("hidden") extern int gl_moff;
VISIBILITY("hidden") extern _BOOL gl_mouse;					/* mouse on flag        */
VISIBILITY("hidden") extern VEX_TIMV tiksav;


/*
 * gemgsxif.c
 */

VISIBILITY("hidden") _BOOL gsx_malloc(void);
VISIBILITY("hidden") void gsx_mfree(void);
VISIBILITY("hidden") void gsx_mret(void **pmaddr, long *pmlen);


VISIBILITY("hidden") void gsx_init(void);
VISIBILITY("hidden") void gsx_graphic(_BOOL tographic);
VISIBILITY("hidden") void gsx_wsclose(void);
VISIBILITY("hidden") void gsx_wsclear(void);
VISIBILITY("hidden") void ratinit(void);
VISIBILITY("hidden") void ratexit(void);
VISIBILITY("hidden") void bb_save(GRECT *ps);
VISIBILITY("hidden") void bb_restore(GRECT *ps);
VISIBILITY("hidden") void gsx_mfset(MFORM *pmfnew);
VISIBILITY("hidden") void gsx_xmfset(MFORM *pmfnew);
VISIBILITY("hidden") void gsx_mxmy(_WORD *pmx, _WORD *pmy);
VISIBILITY("hidden") _WORD gsx_tick(VEX_TIMV tcode, VEX_TIMV *ptsave);
VISIBILITY("hidden") _WORD gsx_button(void);
VISIBILITY("hidden") _WORD gsx_kstate(void);
VISIBILITY("hidden") void gsx_moff(void);
VISIBILITY("hidden") void gsx_mon(void);
VISIBILITY("hidden") _WORD gsx_char(void);
VISIBILITY("hidden") _WORD gsx_nplanes(void);
VISIBILITY("hidden") _BOOL sound(_BOOL isfreq, _WORD freq, _WORD duration);


/*
 * apgsxif.c
 */
VISIBILITY("hidden") void gsx_fix(MFDB *pfd, _WORD *theaddr, _WORD wb, _WORD h);
VISIBILITY("hidden") void gsx_blt(_WORD *saddr, _UWORD sx, _UWORD sy, _UWORD sw, _WORD *daddr, _UWORD dx, _UWORD dy, _UWORD dw, _UWORD w, _UWORD h, _UWORD rule, _WORD fgcolor, _WORD bgcolor);
VISIBILITY("hidden") void gsx_sclip(const GRECT *pt);
VISIBILITY("hidden") void gsx_gclip(GRECT *pt);
VISIBILITY("hidden") _BOOL gsx_chkclip(GRECT *pt);
VISIBILITY("hidden") void gsx_cline(_UWORD x1, _UWORD y1, _UWORD x2, _UWORD y2);
VISIBILITY("hidden") void gsx_attr(_BOOL text, _UWORD mode, _UWORD color);
VISIBILITY("hidden") void gsx_box(const GRECT *pt);
VISIBILITY("hidden") void bb_screen(_WORD scrule, _WORD scsx, _WORD scsy, _WORD scdx, _WORD scdy, _WORD scw, _WORD sch);
VISIBILITY("hidden") void gsx_trans(_WORD *saddr, _UWORD sw, _WORD *daddr, _UWORD dw, _UWORD h, _WORD fg, _WORD bg);
VISIBILITY("hidden") void gsx_start(void);
VISIBILITY("hidden") void bb_fill(_WORD mode, _WORD fis, _WORD patt, _WORD hx, _WORD hy, _WORD hw, _WORD hh);
VISIBILITY("hidden") _WORD gsx_tcalc(_WORD font, const char *ptext, _WORD *ptextw, _WORD *ptexth, vdi_wchar_t *textout);
VISIBILITY("hidden") void gsx_tblt(_WORD tb_f, _WORD x, _WORD y, const vdi_wchar_t *wtext, _WORD len);
VISIBILITY("hidden") void gsx_xbox(GRECT *pt);
VISIBILITY("hidden") void gsx_xcbox(GRECT *pt);

#endif /* GSXDEFS_H */
