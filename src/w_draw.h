/*****************************************************************************
 * W_DRAW.H
 *****************************************************************************/

#ifndef __W_DRAW_H__
#define __W_DRAW_H__

#ifndef __PORTAB_H__
#  include <portab.h>
#endif

EXTERN_C_BEG


#define INVERT      1
#define FILL        2
#define OUTLINE     4

/* Object colors */
#ifndef __OPEN_SYS_H__
#undef WHITE
#undef BLACK
#undef RED
#undef GREEN
#undef BLUE
#undef CYAN
#undef YELLOW
#undef MAGENTA
#undef LWHITE
#undef LBLACK
#undef LRED
#undef LGREEN
#undef LBLUE
#undef LCYAN
#undef LYELLOW
#undef LMAGENTA

#define WHITE dont_use_this
#define BLACK dont_use_this
#define RED dont_use_this
#define GREEN dont_use_this
#define BLUE dont_use_this
#define CYAN dont_use_this
#define YELLOW dont_use_this
#define MAGENTA dont_use_this
#define LWHITE dont_use_this
#define LBLACK dont_use_this
#define LRED dont_use_this
#define LGREEN dont_use_this
#define LBLUE dont_use_this
#define LCYAN dont_use_this
#define LYELLOW dont_use_this
#define LMAGENTA dont_use_this
#endif

#define W_PAL_WHITE           G_WHITE
#define W_PAL_BLACK           G_BLACK
#define W_PAL_RED             G_RED
#define W_PAL_GREEN           G_GREEN
#define W_PAL_BLUE            G_BLUE
#define W_PAL_CYAN            G_CYAN
#define W_PAL_YELLOW          G_YELLOW
#define W_PAL_MAGENTA         G_MAGENTA
#define W_PAL_LGRAY           G_LWHITE
#define W_PAL_DGRAY           G_LBLACK
#define W_PAL_DRED            G_LRED
#define W_PAL_DGREEN          G_LGREEN
#define W_PAL_DBLUE           G_LBLUE
#define W_PAL_DCYAN           G_LCYAN
#define W_PAL_DYELLOW         G_LYELLOW
#define W_PAL_DMAGENTA        G_LMAGENTA
#define W_PAL_MAX 16


void W_Clip_Rect(_WORD handle, _BOOL on, const GRECT *gr);
void W_Get_Clip_Rect(_WORD handle, GRECT *gr);

#define MAX_POINTS 64

typedef struct
{
	_WORD   xx, yy;
} W_POINTS;

#define W_PEN_SOLID            0
#define W_PEN_LONGDASH         1
#define W_PEN_DOT              2
#define W_PEN_DASHDOT          3
#define W_PEN_DASH             4
#define W_PEN_DASHDOTDOT       5
#define W_PEN_DOTDOT           6
#define W_PEN_NULL             7
#define W_PEN_USER             8

#define ARROW_BEG 0x01
#define ARROW_END 0x02

void xywh2rect(_WORD x, _WORD y, _WORD w, _WORD h, GRECT *gr);

void *W_PenCreate(_WORD handle, _WORD width, _UWORD style, _WORD colorNr);
void W_PenDelete(_WORD handle, const void *obj);
void W_Lines(_WORD handle, const W_POINTS points[], _WORD npoints);
void W_Arrow(_WORD handle, W_POINTS pxy[2], _UWORD where);


void W_Fill_Area(_WORD handle, const W_POINTS points[], _WORD npoints, _WORD style, _WORD color);
void W_Clear_Rect(_WORD handle, const GRECT *gr);
void W_Fill_Rect(_WORD handle, const GRECT *gr, _WORD style, _WORD color, _BOOL perimeter);
void W_Invert_Rect(_WORD handle, const GRECT *gr);
void W_Disable_Rect(_WORD handle, const GRECT *gr);
void W_Pixel(_WORD handle, const W_POINTS pxy[], _WORD numpoints, _WORD color);


void W_Rectangle(_WORD handle, const GRECT *gr);
void W_RoundBox(_WORD handle, const GRECT *gr);
void W_MoveBox(_WORD handle, const GRECT *gr);

void W_Ellipse(_WORD handle, const GRECT *gr, _BOOL fuellen, _WORD style, _WORD colorNr, _BOOL perimeter);
void W_TDFrame(_WORD handle, const GRECT *r, _WORD height, _LONG flags);

#ifndef __MFDB__
#define __MFDB__
typedef struct
{
	void  *fd_addr;
	_WORD   fd_w;
	_WORD   fd_h;
	_WORD   fd_wdwidth;
	_WORD   fd_stand;
	_WORD   fd_nplanes;
	_WORD   fd_r1;
	_WORD   fd_r2;
	_WORD   fd_r3;
} MFDB;
#endif

_WORD W_NTextWidth(_WORD handle, const char *text, size_t len);
_WORD W_TextWidth(_WORD handle, const char *text);
void W_Text(_WORD handle, _WORD xx, _WORD yy, const char *str);
void W_ClipText(_WORD handle, GRECT *gr, const char *str, _WORD hdir, _WORD vdir);

void W_Draw_Image(_WORD handle, _WORD x, _WORD y, _WORD w, _WORD h, void *data, _WORD fg, _WORD bg, _WORD mode);
void W_Draw_Icon(_WORD handle, _WORD x, _WORD y, _WORD w, _WORD h, void *data, void *mask, _WORD datacol, _WORD maskcol, _BOOL selected);
_BOOL W_Cicon_Setpalette(_WORD *palette);
void W_Draw_Cicon(_WORD handle, _WORD x, _WORD y, _WORD w, _WORD h, CICON *cicon, _WORD datacol, _WORD maskcol, _WORD state);
void W_Draw_Picture(_WORD handle, _WORD x, _WORD y, GRECT *area, MFDB *pic);
#ifdef __PORTAES_H__
CICON *cicon_best_match(CICONBLK *cicon, _WORD planes);
CICON *cicon_find_plane(CICON *list, _WORD planes);
#endif


EXTERN_C_END

#endif /* __W_DRAW_H__ */
