#ifndef GSXDEFS_H
#define GSXDEFS_H 1

#include "ws.h"

/*
 * gemgsxif.[cS]
 */

void gsx_mfree(void);
void gsx_mret(void **pmaddr, long *pmlen);


void gsx_graphic(_BOOL tographic);
void gsx_escapes(_WORD esc_code);
void av_hardcopy(void);
void gsx_wsopen(void);
void gsx_wsclose(void);
void bb_set(_WORD sx, _WORD sy, _WORD sw, _WORD sh, _WORD *pts1, _WORD *pts2, MFDB *pfd, MFDB *psrc, MFDB *pdst);
void bb_save(GRECT *ps);
void bb_restore(GRECT *ps);
void gsx_resetmb(void);
void gsx_mxmy(_WORD *pmx, _WORD *pmy);
_WORD gsx_button(void);
void gsx_moff(void);
void gsx_mon(void);

_WORD av_opnwk(_WORD *pwork_in, _WORD *phandle, _WORD *pwork_out);
void av_pline(_WORD count, _WORD *pxyarray);
void avst_clip(_WORD clip_flag, _WORD pxyarray);
void avst_height(_WORD height, _WORD *pchr_width, _WORD *pchr_height, _WORD *pcell_width, _WORD *pcell_height);
void avr_recfl(_WORD *pxyarray, MFDB *pdesMFDB);
void avro_cpyfm(_WORD wr_mode, _WORD *pxyarray, MFDB *psrcMFDB, MFDB *pdesMFDB);
void avrt_cpyfm(_WORD wr_mode, _WORD *pxyarray, MFDB *psrcMFDB, MFDB *pdesMFDB, _WORD fgcolor, _WORD bgcolor);
void avr_trnfm(MFDB *psrcMFDB, MFDB *pdesMFDB);
void avsl_width(_WORD width);

/*
 * apgsxif.[cS]
 */
void gsx_fix(MFDB *pfd, _WORD *theaddr, _WORD wb, _WORD h);
_WORD gsx_blt(_WORD *saddr, _UWORD sx, _UWORD sy, _UWORD swb, _WORD *daddr, _UWORD dx, _UWORD dy, _UWORD dwb, _UWORD w, _UWORD h, _UWORD rule, _WORD fgcolor, _WORD bgcolor);
void gsx_sclip(const GRECT *pt);
void gsx_gclip(GRECT *pt);
_BOOL gsx_chkclip(GRECT *pt);
void gsx_pline(_WORD offx, _WORD offy, _WORD cnt, const _WORD *pts);
void gsx_cline(_UWORD x1, _UWORD y1, _UWORD x2, _UWORD y2);
void gsx_attr(_UWORD text, _UWORD mode, _UWORD color);
void gsx_bxpts(GRECT *pt);
void gsx_box(GRECT *pt);
void bb_screen(_WORD scrule, _WORD scsx, _WORD scsy, _WORD scdx, _WORD scdy, _WORD scw, _WORD sch);
void gsx_trans(_WORD *saddr, _UWORD swb, _WORD *daddr, _UWORD dwb, _UWORD h, _WORD fg, _WORD bg);
void gsx_start(void);
void bb_fill(_WORD mode, _WORD fis, _WORD patt, _WORD hx, _WORD hy, _WORD hw, _WORD hh);
_WORD gsx_tcalc(_WORD font, const char *ptext, _WORD ptextw, _WORD ptexth, _WORD pnumchs);
_WORD gsx_tblt(_WORD tb_f, _WORD x, _WORD y, const char *text, int len);
void gsx_xbox(GRECT *pt);
void gsx_xcbox(GRECT *pt);
void gsx_xline(_WORD ptscount, _WORD *ppoints);
_UWORD ch_width(_WORD fn);
_UWORD ch_height(_WORD fn);

#endif /* GSXDEFS_H */
