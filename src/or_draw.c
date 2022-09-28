#include "config.h"
#include <gem.h>
#include "portvdi.h"
#include "or_draw.h"

static _WORD vdi_handle;
static EXTOB_MODE objc_mode;
_WORD color_background = G_LWHITE;
_WORD color_disabled = G_LBLACK;
_WORD color_indicator = G_LWHITE;
_WORD color_activator = G_LWHITE;
_BOOL move_indicator = FALSE;
_BOOL move_activator = FALSE;
_BOOL change_indicator = TRUE;
_BOOL change_activator = FALSE;
_BOOL op_draw3d = FALSE;
_BOOL od_clear;
_UWORD od_oldstate;
PARMBLKFUNC od_code;
PARMBLK od_parm;

#define CUR_WIDTH 2

#if CUR_WIDTH != 0
static OBJECT cur_left[] = {
	{ -1, -1, -1, G_BOXCHAR, OF_LASTOB|OF_TOUCHEXIT, OS_NORMAL, { (_LONG)(0x4001100L) }, 0,0, 2,1 },
};
static OBJECT cur_right[] = {
	{ -1, -1, -1, G_BOXCHAR, OF_LASTOB|OF_TOUCHEXIT, OS_NORMAL, { (_LONG)(0x3001100L) }, 0,0, 2,1 },
};
#endif



/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

void od_setfont(WIND_OS *os, _WORD size, _WORD color, _UWORD style)
{
	W_SetFont(os, FONT_SYSTEM, (size & TE_FONT_MASK) == SMALL ? FONT_SIZE_ICON : GetTextNormalHeight(), color, style);
	od_cw = W_CharWidth(os, ' ');
	od_ch = W_CharHeight(os, ' ');
}

static _UBYTE *char2text(_UBYTE c)
{
	static _UBYTE buf[2];

	buf[0] = c;
	return buf;
}


void od_box(_WORD advance, _BOOL draw)
{
	od_pxy.g_x += advance;
	od_pxy.g_y += advance;
	od_pxy.g_w -= advance;
	od_pxy.g_w -= advance;
	od_pxy.g_h -= advance;
	od_pxy.g_h -= advance;
	if (draw)
	{
		W_Rectangle(vdi_handle, &od_pxy);
	}
}


static void outline(_UWORD state)
{
	if (state & OS_OUTLINED)
	{
		void *pen;

		vswr_mode(vdi_handle, MD_REPLACE);
		pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_WHITE);
		od_box(-1, TRUE);
		od_box(-1, TRUE);
		W_PenDelete(vdi_handle, pen);
		pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
		od_box(-1, TRUE);
		W_PenDelete(vdi_handle, pen);
	}
}


static void cross(GRECT *gr)
{
	W_POINTS xy[2];
	void *pen;

	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_WHITE);
	vswr_mode(vdi_handle, MD_TRANS);
	xy[0].xx = gr->g_x;
	xy[0].yy = gr->g_y;
	xy[1].xx = gr->g_x + gr->g_w - 1;
	xy[1].yy = gr->g_y + gr->g_h - 1;
	W_Lines(vdi_handle, xy, 2);
	xy[0].xx = xy[1].xx;
	xy[1].xx = gr->g_x;
	W_Lines(vdi_handle, xy, 2);
	vswr_mode(vdi_handle, MD_REPLACE);
	W_PenDelete(vdi_handle, pen);
}


_BOOL color_3d(_UWORD flags, _UWORD state, _WORD *color, _WORD *pattern)
{
	_BOOL draw_3d = FALSE;

	if (*pattern == IP_HOLLOW && *color == W_PAL_WHITE &&
		objc_mode != EXTOB_HONKA && objc_mode != EXTOB_AES && objc_mode != EXTOB_OVERLAY &&
		(objc_mode != EXTOB_MAGIX || (op_draw3d && GetNumColors() >= 16)))
	{
		if (op_draw3d)
		{
			draw_3d = TRUE;
			if (GetNumColors() >= 16)
			{
				switch (flags & OF_FL3DMASK)
				{
				case OF_FL3DIND:
					*color = color_indicator;
					if (change_indicator && (state & OS_SELECTED))
						*color = W_PAL_DGRAY;
					*pattern = IP_SOLID;
					break;
				case OF_FL3DBAK:
					*color = color_background;
					*pattern = IP_SOLID;
					break;
				case OF_FL3DACT:
					*color = color_activator;
					if (change_activator && (state & OS_SELECTED))
						*color = W_PAL_DGRAY;
					*pattern = IP_SOLID;
					break;
				default:
					draw_3d = FALSE;
					break;
				}
			} else
			{
				switch (flags & OF_FL3DMASK)
				{
				case OF_FL3DIND:
					*color = W_PAL_BLACK;
					*pattern = IP_4PATT;
					break;
				case OF_FL3DBAK:
					*color = W_PAL_BLACK;
					*pattern = IP_4PATT;
					break;
				case OF_FL3DACT:
					*color = W_PAL_BLACK;
					*pattern = IP_4PATT;
					break;
				default:
					draw_3d = FALSE;
					break;
				}
			}
		} else if (state & OS_WHITEBAK)
		{
			draw_3d = TRUE;
		}
	}
	return draw_3d;
}


void shadow(_WORD framesize, _WORD framecol)
{
	W_POINTS pxy[3];
	void *pen;

	if (framesize != 0)
	{
		if (framesize < 0)
		{
			od_outrec.g_x += framesize;
			od_outrec.g_y += framesize;
			od_outrec.g_w -= framesize;
			od_outrec.g_w -= framesize;
			od_outrec.g_h -= framesize;
			od_outrec.g_h -= framesize;
			framesize = -framesize;
		}
		pxy[0].xx = od_outrec.g_x + framesize /* + 1 */;
		pxy[0].yy = od_outrec.g_y + od_outrec.g_h;
		pxy[1].xx = od_outrec.g_x + od_outrec.g_w;
		pxy[1].yy = pxy[0].yy;
		pxy[2].xx = pxy[1].xx;
		pxy[2].yy = od_outrec.g_y + framesize /* + 1 */;
		W_SetBkMode(vdi_handle, MD_REPLACE);
		pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, framecol);
		framesize += framesize;
		while (framesize > 0)
		{
			W_Lines(vdi_handle, pxy, 3);
			pxy[0].yy++;
			pxy[1].xx++;
			pxy[1].yy++;
			pxy[2].xx++;
			framesize--;
		}
		W_PenDelete(vdi_handle, pen);
	}
}


void *_draw_box(
	_WORD framesize,
	_WORD framecol,
	_BOOL filled,
	_WORD fillpattern,
	_WORD fillcol
)
{
	void *pen;

	W_SetBkMode(vdi_handle, MD_REPLACE);
	if (filled)
	{
		W_Fill_Rect(vdi_handle, &od_pxy, fillpattern, fillcol);
	}
	if (framesize > 0)
	{
		_WORD i;

		pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, framecol);
		if (framesize > MAX_FRAMESIZE)
			framesize = MAX_FRAMESIZE;
		i = framesize;
		od_box(framesize, FALSE);
		rc_copy(&od_pxy, &od_framerec);
		do {
			od_box(-1, TRUE);
		} while (--i != 0);
	} else if (framesize < 0)
	{
		_WORD i;

		pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, framecol);
		if (framesize < -(MAX_FRAMESIZE))
			framesize = -(MAX_FRAMESIZE);
		i = framesize;
		do {
			od_box(-1, TRUE);
		} while (++i != 0);
		od_box(-framesize, FALSE);
	} else
	{
		pen = NULL;
	}
	return pen;
}


static void draw_dialog_frame(_WORD pattern, _WORD color)
{
	void *pen;
	W_POINTS pxy[3];
	
	W_SetBkMode(vdi_handle, MD_REPLACE);
	W_Fill_Rect(vdi_handle, &od_pxy, pattern, color);
	od_box(3, FALSE);
	rc_copy(&od_pxy, &od_framerec);
	od_box(-1, FALSE);
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
	W_Rectangle(vdi_handle, &od_pxy);
	W_PenDelete(vdi_handle, pen);
	
	od_box(-1, FALSE);
	pxy[0].xx = od_pxy.g_x;
	pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[1].xx = od_pxy.g_x;
	pxy[1].yy = od_pxy.g_y;
	pxy[2].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[2].yy = od_pxy.g_y;
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_DGRAY);
	W_Lines(vdi_handle, pxy, 3);
	W_PenDelete(vdi_handle, pen);
	pxy[0].xx = od_pxy.g_x + 1;
	pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[1].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[1].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[2].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[2].yy = od_pxy.g_y + 1;
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_WHITE);
	W_Lines(vdi_handle, pxy, 3);
	W_PenDelete(vdi_handle, pen);
	
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_LGRAY);
	od_box(-1, TRUE);
	od_box(-1, TRUE);
	W_PenDelete(vdi_handle, pen);

	od_box(-1, FALSE);
	pxy[0].xx = od_pxy.g_x;
	pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[1].xx = od_pxy.g_x;
	pxy[1].yy = od_pxy.g_y;
	pxy[2].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[2].yy = od_pxy.g_y;
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_WHITE);
	W_Lines(vdi_handle, pxy, 3);
	W_PenDelete(vdi_handle, pen);
	pxy[0].xx = od_pxy.g_x + 1;
	pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[1].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[1].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[2].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[2].yy = od_pxy.g_y + 1;
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_DGRAY);
	W_Lines(vdi_handle, pxy, 3);
	W_PenDelete(vdi_handle, pen);

	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
	od_box(-1, TRUE);
	W_PenDelete(vdi_handle, pen);
}


void draw_underline_char(_WORD x, _WORD y, _WORD color, const _UBYTE *txt, _WORD offset)
{
	W_POINTS pxy[2];
	void *pen;
	_WORD oldmode;
	_WORD x_offset;
	
	x_offset = W_NTextWidth(vdi_handle, txt, offset);
	pxy[0].xx = x + x_offset;
	pxy[0].yy = y + od_ch - 1;
	pxy[1].xx = pxy[0].xx + W_CharWidth(vdi_handle, txt[offset]) - 1;
	pxy[1].yy = pxy[0].yy;
	oldmode = W_SetBkMode(vdi_handle, MD_REPLACE);
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, color);
	W_Lines(vdi_handle, pxy, 2);
	W_PenDelete(vdi_handle, pen);
	W_SetBkMode(vdi_handle, oldmode);
}


_WORD wclip_center(_WORD just)
{
	switch (just & TE_JUST_MASK)
	{
	case TE_LEFT:
		return -1;
	case TE_RIGHT:
		return 1;
	}
	return 0;
}


static _BOOL text_rect(_UBYTE *txt, GRECT *gr, _WORD size, _WORD centered, _BOOL centver, _WORD textcol, _WORD offset, _WORD bold, GRECT *text)
{
	_WORD x, y, w, h;

	if (offset != -1 && objc_mode != EXTOB_MAGIX && txt[offset] == '[')
	{
		strCpy(&txt[offset], &txt[offset+1]);
	}
	od_setfont(vdi_handle, size, textcol, bold);
	x = gr->g_x;
	y = gr->g_y;
	centered &= TE_JUST_MASK;
	w = W_TextWidth(vdi_handle, txt);
	h = od_ch;
	if (centered == TE_CNTR)
	{
		x += (gr->g_w - w) / 2;
	} else if (centered == TE_RIGHT)
	{
		x += gr->g_w - w;
	}
	if (centver != FALSE)
	{
		y += (gr->g_h - h) / 2;
	}
	xywh2rect(x, y, w, h, text);
	return rc_intersect(gr, text);
}


static void _draw_text(_UBYTE *txt, GRECT *gr, _WORD centered, _BOOL centver, _WORD textcol, _WORD textmode, _WORD offset, GRECT *clip)
{
	_WORD oldmode;

	if (offset == -1)
	{
		oldmode = W_SetBkMode(vdi_handle, textmode ? textmode : MD_TRANS);
		W_Text(vdi_handle, clip->g_x, clip->g_y, txt);
	} else
	{
		if (textmode)
		{
			oldmode = W_SetBkMode(vdi_handle, textmode);
			W_ClipText(vdi_handle, gr, txt, wclip_center(centered), centver ? 0 : -1);
		} else
		{
			oldmode = W_SetBkMode(vdi_handle, MD_TRANS);
			W_Text(vdi_handle, clip->g_x, clip->g_y, txt);
		}
		draw_underline_char(clip->g_x, clip->g_y, textcol, txt, offset);
	}
	W_SetBkMode(vdi_handle, oldmode);
}


void draw_text(_UBYTE *txt, GRECT *gr, _WORD size, _WORD centered, _BOOL centver, _WORD textcol, _WORD textmode, _WORD offset, _WORD bold)
{
	GRECT clip;

	text_rect(txt, gr, size, centered, centver, textcol, offset, bold, &clip);
	_draw_text(txt, gr, centered, centver, textcol, textmode, offset, &clip);
}


void draw_char(_UBYTE c, GRECT *gr, _WORD size, _WORD centered, _BOOL centver, _WORD textcol, _WORD textmode)
{
	_WORD x, y, w, h;
	_WORD oldmode;

	od_setfont(vdi_handle, size, textcol, TXT_ATTR_NONE);
	x = gr->g_x;
	y = gr->g_y;
	h = gr->g_h;
	w = W_CharWidth(vdi_handle, c);
	if (centered == TE_CNTR)
	{
		x += (gr->g_w - w) / 2;
	} else if (centered == TE_RIGHT)
	{
		x += gr->g_w - w;
	}
	if (centver != FALSE)
	{
		h = W_CharHeight(vdi_handle, c);
		y += (gr->g_h - h) / 2;
	}
	oldmode = W_SetBkMode(vdi_handle, textmode ? MD_REPLACE : MD_TRANS);
	W_Char(vdi_handle, x, y, w, h, c);
	W_SetBkMode(vdi_handle, oldmode);
}


static void draw_ted(TEDINFO *ted, _BOOL formatted, _BOOL box, _WORD bold, _UBYTE fillchar, _UWORD flags, _UWORD state, _WORD *framesize, _WORD *framecol)
{
	_UBYTE *cp, *txt;
	_UWORD color;
	_WORD offset = -1;
	_WORD centered = ted->te_just;
	_WORD textmode;
	void *pen;

	color = ted->te_color;
	if (formatted)
	{
		cp = txt = calc_text(NULL, ted->te_ptmplt, ted->te_tmplen, ted->te_ptext, ted->te_txtlen, ted->te_pvalid, FALSE, fillchar, 0);
		if (objc_mode == EXTOB_NONE)
			offset = -1;
		if (cp != NULL)
		{
			if (objc_mode == EXTOB_ORCS)
			{
				if (ted_extended(ted))
				{
					cp += ted->te_offset;
					cp[ted->te_displen] = '\0';
					if ((int)strLen(ted->te_ptext) > ted->te_displen)
					{
						_WORD wchar, hchar;

						cp[ted->te_displen - 2 * CUR_WIDTH] = '\0';
						GetTextSize(&wchar, &hchar);
						od_pxy.g_x += CUR_WIDTH * wchar;
					}
					centered = TE_LEFT;
				}
			}
		}
	} else
	{
		_UBYTE *p;

		cp = txt = g_strdup(ted->te_ptext);
		if (cp != NULL)
		{
			switch (is_ext_type(&objc_options, G_TEXT, OF_NONE, OS_NORMAL))
			{
			case EXTTYPE_EXIT:
			case EXTTYPE_RADIO:
			case EXTTYPE_CHECK:
			case EXTTYPE_CHECK_SWITCH:
			case EXTTYPE_CHECK_BOOLEAN:
			case EXTTYPE_UNDO2:
			case EXTTYPE_HELP:
			case EXTTYPE_DEFAULT:
			case EXTTYPE_SYSGEM_BUTTON:
			case EXTTYPE_SYSGEM_RADIO:
				if ((p = strchr(cp, '[')) != NULL)
					offset = (_WORD)(p - cp);
				break;
			case EXTTYPE_UNDO:
				if ((objc_mode == EXTOB_ORCS || objc_mode == EXTOB_MYDIAL) && (p = strchr(cp, '[')) != NULL)
					offset = (_WORD)(p - cp);
				break;
			default:
				break;
			}
		}
	}
	*framesize = ted->te_thickness;
	if (*framesize >= 128)
		*framesize = *framesize - 256;
	*framecol = COLSPEC_GET_FRAMECOL(color);
	textmode = COLSPEC_GET_TEXTMODE(color) ? MD_REPLACE : MD_TRANS;
	if (box)
	{
		_WORD fcolor, pattern;
#if 0
		if (offset != -1)
		{
			od_box(-1, FALSE);
			rc_copy(&od_pxy, &od_framerec);
		}
#endif
		fcolor = COLSPEC_GET_INTERIORCOL(color);
		pattern = COLSPEC_GET_FILLPATTERN(color);
		if (!(state & OS_SELECTED))
			if (color_3d(flags, state, &fcolor, &pattern) && !(flags & OF_EDITABLE))
				textmode = MD_TRANS;
		pen = _draw_box(*framesize, *framecol, TRUE, pattern, fcolor);
		W_PenDelete(vdi_handle, pen);
	} else
	{
#if 1
		_WORD fcolor, pattern;

		if (!formatted || !(state & OS_SELECTED))
		{
			fcolor = COLSPEC_GET_INTERIORCOL(color);
			pattern = COLSPEC_GET_FILLPATTERN(color);
			if (od_clear | color_3d(flags, state, &fcolor, &pattern))
			{
				textmode = MD_TRANS;
				if (cp != NULL)
				{
					GRECT clip;
					_WORD textcol;

					textcol = COLSPEC_GET_TEXTCOL(color);
					text_rect(cp, &od_pxy, ted->te_font, centered, TRUE, textcol, offset, bold, &clip);
					W_SetBkMode(vdi_handle, MD_REPLACE);
					W_Fill_Rect(vdi_handle, &clip, pattern, fcolor);
					_draw_text(cp, &od_pxy, centered, TRUE, textcol, textmode, offset, &clip);
					g_free(txt);
					cp = NULL;
				} else
				{
					pen = _draw_box(0, W_PAL_WHITE, TRUE, pattern, fcolor);
					W_PenDelete(vdi_handle, pen);
				}
			}
		}
#endif
	}
	if (cp != NULL)
	{
		draw_text(cp, &od_pxy, ted->te_font, centered, TRUE,
			COLSPEC_GET_TEXTCOL(color), textmode, offset, bold);
		g_free(txt);
	}
	if (state & OS_SELECTED)
		W_Invert_Rect(vdi_handle, &od_framerec);
}


#define FRAMESIZE	1
#define COL_SET     W_PAL_BLACK
#define COL_RESET	W_PAL_WHITE

typedef _UBYTE BIG_IMAGE[32];
typedef _UBYTE SMALL_IMAGE[16];

typedef struct {
	BIG_IMAGE _big_off_mask;	 void *big_off_mask;
	BIG_IMAGE _big_off_data;	 void *big_off_data;
	BIG_IMAGE _big_on_mask;      void *big_on_mask;
	BIG_IMAGE _big_on_data;      void *big_on_data;
	SMALL_IMAGE _small_off_mask; void *small_off_mask;
	SMALL_IMAGE _small_off_data; void *small_off_data;
	SMALL_IMAGE _small_on_mask;  void *small_on_mask;
	SMALL_IMAGE _small_on_data;  void *small_on_data;
} MAC_IMAGE;


static MAC_IMAGE images[NUM_MAC_IMAGES] = {
{	/* MAC_RADIO */
	{ 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x0F, 0xF0, 0x1F, 0xF8, 0x1F, 0xF8, 0x3F, 0xFC, 0x3F, 0xFC,
	  0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8, 0x1F, 0xF8, 0x0F, 0xF0, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00},
	0,
	{ 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x0C, 0x30, 0x10, 0x08, 0x10, 0x08, 0x20, 0x04, 0x20, 0x04,
	  0x20, 0x04, 0x20, 0x04, 0x10, 0x08, 0x10, 0x08, 0x0C, 0x30, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00},
	0,
	{ 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x0F, 0xF0, 0x1F, 0xF8, 0x1F, 0xF8, 0x3F, 0xFC, 0x3F, 0xFC,
	  0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8, 0x1F, 0xF8, 0x0F, 0xF0, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00},
	0,
	{ 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x0C, 0x30, 0x10, 0x08, 0x13, 0xC8, 0x27, 0xE4, 0x27, 0xE4,
	  0x27, 0xE4, 0x27, 0xE4, 0x13, 0xC8, 0x10, 0x08, 0x0C, 0x30, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00},
	0,
	{ 0x3C, 0x00, 0x7E, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x7E, 0x00, 0x3C, 0x00},
	0,
	{ 0x3C, 0x00, 0x42, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x42, 0x00, 0x3C, 0x00},
	0,
	{ 0x3C, 0x00, 0x7E, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x7E, 0x00, 0x3C, 0x00},
	0,
	{ 0x3C, 0x00, 0x42, 0x00, 0x99, 0x00, 0xBD, 0x00, 0xBD, 0x00, 0x99, 0x00, 0x42, 0x00, 0x3C, 0x00},
	0
},
{	/* MAC_CHECK */
	{ 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC,
	  0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00},
	0,
	{ 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFC, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04,
	  0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00},
	0,
	{ 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC,
	  0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00},
	0,
	{ 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFC, 0x30, 0x0C, 0x28, 0x14, 0x24, 0x24, 0x22, 0x44, 0x21, 0x84,
	  0x21, 0x84, 0x22, 0x44, 0x24, 0x24, 0x28, 0x14, 0x30, 0x0C, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00},
	0,
	{ 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00},
	0,
	{ 0xFF, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0xFF, 0x00},
	0,
	{ 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00},
	0,
	{ 0xFF, 0x00, 0xC3, 0x00, 0xA5, 0x00, 0x99, 0x00, 0x99, 0x00, 0xA5, 0x00, 0xC3, 0x00, 0xFF, 0x00},
	0
},
{	/* MAC_BOOLEAN */
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	0,
	{ 0xE0, 0x00, 0x80, 0x00, 0xC0, 0x00, 0x80, 0x02, 0xE0, 0x07, 0x00, 0x7E, 0x00, 0xFC, 0x00, 0xCC,
	  0x00, 0xCC, 0x00, 0xFC, 0x01, 0xF8, 0xE3, 0x80, 0xA3, 0x00, 0xE0, 0x00, 0xA0, 0x00, 0xA0, 0x00},
	0,
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	0,
	{ 0xE0, 0x00, 0x80, 0x00, 0xC0, 0x00, 0x83, 0x00, 0xE3, 0x80, 0x01, 0xF8, 0x00, 0xFC, 0x00, 0xCC,
	  0x00, 0xCC, 0x00, 0xFC, 0x00, 0x7E, 0xE0, 0x07, 0xA0, 0x02, 0xE0, 0x00, 0xA0, 0x00, 0xA0, 0x00},
	0,
	{ 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00},
	0,
	{ 0xE0, 0x00, 0xC1, 0x00, 0xE3, 0x00, 0x05, 0x00, 0x0E, 0x00, 0xEC, 0x00, 0xE0, 0x00, 0xA0, 0x00},
	0,
	{ 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00},
	0,
	{ 0xE0, 0x00, 0xC0, 0x00, 0xEC, 0x00, 0x0E, 0x00, 0x05, 0x00, 0xE3, 0x00, 0xE1, 0x00, 0xA0, 0x00},
	0
},
{	/* MAC_SWITCH */
	{ 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE,
	  0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0x00, 0x00},
	0,
	{ 0x60, 0x02, 0x90, 0x06, 0x90, 0x02, 0x90, 0x02, 0x60, 0x02, 0x00, 0x00, 0x0F, 0x00, 0x0E, 0x00,
	  0x0F, 0x00, 0x0B, 0x80, 0x01, 0xC0, 0x00, 0xE0, 0x00, 0x70, 0x00, 0x38, 0x00, 0x10, 0x00, 0x00},
	0,
	{ 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE,
	  0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0x00, 0x00},
	0,
	{ 0x60, 0x02, 0x90, 0x06, 0x90, 0x02, 0x90, 0x02, 0x60, 0x02, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x70,
	  0x00, 0xF0, 0x01, 0xD0, 0x03, 0x80, 0x07, 0x00, 0x0E, 0x00, 0x1C, 0x00, 0x08, 0x00, 0x00, 0x00},
	0,
	{ 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80},
	0,
	{ 0xE1, 0x80, 0xA0, 0x80, 0xE0, 0x80, 0x00, 0x00, 0x30, 0x00, 0x30, 0x00, 0x08, 0x00, 0x04, 0x00},
	0,
	{ 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x80},
	0,
	{ 0xE1, 0x80, 0xA0, 0x80, 0xE0, 0x80, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x08, 0x00, 0x10, 0x00},
	0
},
{	/* MAC_H_RADIO */
	{ 0x00, 0x00, 0x07, 0xC0, 0x1F, 0xF0, 0x3F, 0xF8, 0x7F, 0xFC, 0x7F, 0xFC, 0xFF, 0xFE, 0xFF, 0xFE,
	  0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0x7F, 0xFC, 0x7F, 0xFC, 0x3F, 0xF8, 0x1F, 0xF0, 0x07, 0xC0},
	0,
	{ 0x00, 0x00, 0x07, 0xC0, 0x18, 0x30, 0x20, 0x08, 0x40, 0x04, 0x40, 0x04, 0x80, 0x02, 0x80, 0x02,
	  0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x40, 0x04, 0x40, 0x04, 0x20, 0x08, 0x18, 0x30, 0x07, 0xC0},
	0,
	{ 0x00, 0x00, 0x07, 0xC0, 0x1F, 0xF0, 0x3F, 0xF8, 0x7F, 0xFC, 0x7F, 0xFC, 0xFF, 0xFE, 0xFF, 0xFE,
	  0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0x7F, 0xFC, 0x7F, 0xFC, 0x3F, 0xF8, 0x1F, 0xF0, 0x07, 0xC0},
	0,
	{ 0x00, 0x00, 0x07, 0xC0, 0x18, 0x30, 0x20, 0x08, 0x47, 0xC4, 0x4F, 0xE4, 0x9F, 0xF2, 0x9F, 0xF2,
	  0x9F, 0xF2, 0x9F, 0xF2, 0x9F, 0xF2, 0x4F, 0xE4, 0x47, 0xC4, 0x20, 0x08, 0x18, 0x30, 0x07, 0xC0},
	0,
	{ 0x3C, 0x00, 0x7E, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x7E, 0x00, 0x3C, 0x00},
	0,
	{ 0x3C, 0x00, 0x42, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x42, 0x00, 0x3C, 0x00},
	0,
	{ 0x3C, 0x00, 0x7E, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x7E, 0x00, 0x3C, 0x00},
	0,
	{ 0x3C, 0x00, 0x42, 0x00, 0x99, 0x00, 0xBD, 0x00, 0xBD, 0x00, 0x99, 0x00, 0x42, 0x00, 0x3C, 0x00},
	0
},
{	/* MAC_H_CHECK */
	{ 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE,
	  0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE},
	0,
	{ 0x00, 0x00, 0xFF, 0xFE, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02,
	  0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0xFF, 0xFE},
	0,
	{ 0x00, 0x00, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE,
	  0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE},
	0,
	{ 0x00, 0x00, 0xFF, 0xFE, 0x80, 0x02, 0xB0, 0x1A, 0xB8, 0x3A, 0x9C, 0x72, 0x8E, 0xE2, 0x87, 0xC2,
	  0x83, 0x82, 0x87, 0xC2, 0x8E, 0xE2, 0x9C, 0x72, 0xB8, 0x3A, 0xB0, 0x1A, 0x80, 0x02, 0xFF, 0xFE},
	0,
	{ 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00},
	0,
	{ 0xFF, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0xFF, 0x00},
	0,
	{ 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00},
	0,
	{ 0xFF, 0x00, 0x81, 0x00, 0xA5, 0x00, 0x99, 0x00, 0x99, 0x00, 0xA5, 0x00, 0x81, 0x00, 0xFF, 0x00},
	0
},
{	/* MAC_H_LIST */
	{ 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	0,
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFC, 0x1F, 0xF8, 0x0F, 0xF0, 0x37, 0xEC,
	  0x1B, 0xD8, 0x0D, 0xB0, 0x06, 0x60, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	0,
	{ 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	0,
	{ 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x01, 0xBF, 0xFD, 0x9F, 0xF9, 0x8F, 0xF1, 0xB7, 0xED,
	  0x9B, 0xD9, 0xCD, 0xB3, 0xE6, 0x67, 0xF3, 0xCF, 0xF9, 0x9F, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF},
	0,
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	0,
	{ 0x00, 0x00, 0x0F, 0xF0, 0x03, 0xC0, 0x01, 0x80, 0x0C, 0x30, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00},
	0,
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	0,
	{ 0xFF, 0xFF, 0xF0, 0x0F, 0xFC, 0x3F, 0xFE, 0x7F, 0xF3, 0xCF, 0xFC, 0x3F, 0xFE, 0x7F, 0xFF, 0xFF},
	0
},
};


static BIG_IMAGE _big_circle = {
	  0x00, 0x00, 0x00, 0x00, 0x13, 0xc0, 0x1c, 0x30, 0x1c, 0x08, 0x00, 0x08, 0x20, 0x04, 0x20, 0x04,
	  0x20, 0x04, 0x20, 0x04, 0x10, 0x00, 0x10, 0x38, 0x0c, 0x38, 0x03, 0xc8, 0x00, 0x00, 0x00, 0x00
};
static void *big_circle = _big_circle;
static SMALL_IMAGE _small_circle = {
	  0x47, 0xf0, 0x7c, 0x1e, 0x78, 0x07, 0x7c, 0x00, 0x00, 0x3e, 0xe0, 0x1e, 0x78, 0x3e, 0x0f, 0xe2
};
static void *small_circle = _small_circle;
static BIG_IMAGE _big_threestate = {
	0x00, 0x00, 0x00, 0x00, 0x3F, 0xFC, 0x24, 0x44, 0x20, 0x04, 0x31, 0x14, 0x20, 0x04, 0x24, 0x44, 
	0x20, 0x04, 0x31, 0x14, 0x20, 0x04, 0x24, 0x44, 0x20, 0x04, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00
};
static void *big_threestate = _big_threestate;
static SMALL_IMAGE _small_threestate = {
	0xFF, 0xFF, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0xFF, 0xFF
};
static void *small_threestate = _small_threestate;
static BIG_IMAGE _threestate_3d = {
	0x22, 0x22, 0x00, 0x00, 0x88, 0x88, 0x00, 0x00, 0x22, 0x22, 0x00, 0x00, 0x88, 0x88, 0x00, 0x00, 
	0x22, 0x22, 0x00, 0x00, 0x88, 0x88, 0x00, 0x00, 0x22, 0x22, 0x00, 0x00, 0x88, 0x88, 0x00, 0x00
};
static void *threestate_3d = _threestate_3d;
	

static _BOOL get_macimage(_WORD type, _WORD state, _WORD *width, _WORD *height, _UBYTE **mask, _UBYTE **data)
{
	MAC_IMAGE *image;
	_WORD cw, ch;

	if (type < 0 || type >= NUM_MAC_IMAGES)
		return FALSE;
	image = &images[type];
	GetTextSize(&cw, &ch);
	if (ch < 13)
	{
		*width = 16;
		*height = 8;
		if (state & OS_SELECTED)
		{
			*mask = (_UBYTE *)image->small_on_mask;
			*data = (_UBYTE *)image->small_on_data;
		} else
		{
			*mask = (_UBYTE *)image->small_off_mask;
			*data = (_UBYTE *)image->small_off_data;
		}
	} else
	{
		*width = *height = 16;
		if (state & OS_SELECTED)
		{
			*mask = (_UBYTE *)image->big_on_mask;
			*data = (_UBYTE *)image->big_on_data;
		} else
		{
			*mask = (_UBYTE *)image->big_off_mask;
			*data = (_UBYTE *)image->big_off_data;
		}
	}
	return TRUE;
}


_UWORD _draw_mac(_WORD flags, _WORD state, _WORD x, _WORD y, _WORD type, _WORD *w, _BOOL *draw_3d)
{
	GRECT gr;
	_WORD color, pattern;
	void *pen;
	_WORD h;
	_UBYTE *data, *mask;
	_WORD pencolor;
	
	GetTextSize(w, &h);
	gr.g_x = x;
	gr.g_y = y;
	gr.g_w = *w = 16;
	gr.g_h = h;
	color = W_PAL_WHITE;
	pattern = IP_HOLLOW;
	*draw_3d = color_3d(flags, state, &color, &pattern);
	pencolor = *draw_3d && (state & OS_DISABLED) && GetNumColors() >= 16 ? color_disabled : W_PAL_BLACK;
	
	switch (type)
	{
	case MAC_RADIO:
		if (h >= 16)
		{
			if (objc_mode == EXTOB_MAGIX)
			{
				gr.g_w -= 1;
				gr.g_h -= 1;
			} else
			{
				gr.g_x += 2;
				gr.g_y += 2;
				gr.g_w -= 4;
				gr.g_h -= 4;
			}
		}
#if 1
		if (h == 16 || h == 8)
		{
			_UBYTE *data, *mask;

			if (get_macimage(type, state, w, &h, &mask, &data))
				W_Draw_Icon(vdi_handle, x, y, (*w + 15) & 0xfff0, h, data, mask, COL_SET, COL_RESET, FALSE);
		} else
#endif
		{
#if 0
			W_Fill_Rect(vdi_handle, &gr, pattern, color);
#endif
			if (*draw_3d && 0)
			{
			} else
			{
				pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, pencolor);
				W_Ellipse(vdi_handle, &gr, TRUE, IP_SOLID, W_PAL_WHITE);
				W_Ellipse(vdi_handle, &gr, FALSE, IP_SOLID, pencolor);
				if (state & OS_SELECTED)
				{
					gr.g_x += 3;
					gr.g_y += 3;
					gr.g_w -= 6;
					gr.g_h -= 6;
					W_PenDelete(vdi_handle, pen);
					pen = W_PenCreate(vdi_handle, 0, W_PEN_NULL, W_PAL_WHITE);
					W_Ellipse(vdi_handle, &gr, TRUE, IP_SOLID, pencolor);
				}
				W_PenDelete(vdi_handle, pen);
			}
		}
		break;

	case MAC_CHECK:
		if (*draw_3d)
		{
			W_POINTS pxy[4];

			if (h >= 16)
			{
				if (objc_mode == EXTOB_MAGIX)
				{
					gr.g_w -= 1;
					gr.g_h -= 1;
				} else
				{
					gr.g_x += 1;
					gr.g_y += 1;
					gr.g_w -= 2;
					gr.g_h -= 2;
				}
			}
			pen = W_PenCreate(vdi_handle, 1, W_PEN_NULL, pencolor);
			W_Fill_Rect(vdi_handle, &gr, IP_SOLID, W_PAL_WHITE);
			W_PenDelete(vdi_handle, pen);

			pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_WHITE);
			pxy[0].xx = gr.g_x + 1;
			pxy[0].yy = gr.g_y + gr.g_h - 1;
			pxy[1].xx = gr.g_x + gr.g_w - 1;
			pxy[1].yy = pxy[0].yy;
			pxy[2].xx = pxy[1].xx;
			pxy[2].yy = gr.g_y + 1;
			W_Lines(vdi_handle, pxy, 3);
			W_PenDelete(vdi_handle, pen);

			if (GetNumColors() > 2)
			{
				pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_LGRAY);
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + gr.g_h - 2;
				pxy[1].xx = gr.g_x + gr.g_w - 2;
				pxy[1].yy = pxy[0].yy;
				pxy[2].xx = pxy[1].xx;
				pxy[2].yy = gr.g_y + 2;
				W_Lines(vdi_handle, pxy, 3);
				W_PenDelete(vdi_handle, pen);
			}

			pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, pencolor);
			pxy[0].xx = gr.g_x;
			pxy[0].yy = gr.g_y + gr.g_h - 1;
			pxy[1].xx = pxy[0].xx;
			pxy[1].yy = gr.g_y;
			pxy[2].xx = gr.g_x + gr.g_w - 1;
			pxy[2].yy = pxy[1].yy;
			W_Lines(vdi_handle, pxy, 3);
			pxy[0].xx = gr.g_x + 1;
			pxy[0].yy = gr.g_y + gr.g_h - 2;
			pxy[1].xx = pxy[0].xx;
			pxy[1].yy = gr.g_y + 1;
			pxy[2].xx = gr.g_x + gr.g_w - 2;
			pxy[2].yy = pxy[1].yy;
			W_Lines(vdi_handle, pxy, 3);
			W_PenDelete(vdi_handle, pen);

			if (state & OS_SELECTED)
			{
				pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, pencolor);
				pxy[0].xx = gr.g_x + 3;
				pxy[0].yy = gr.g_y + 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + gr.g_h - 3;
				W_Lines(vdi_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 4;
				pxy[0].yy = gr.g_y + 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + gr.g_h - 4;
				W_Lines(vdi_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + 3;
				pxy[1].xx = gr.g_x + gr.g_w - 4;
				pxy[1].yy = gr.g_y + gr.g_h - 3;
				W_Lines(vdi_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 3;
				pxy[0].yy = gr.g_y + gr.g_h - 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + 3;
				W_Lines(vdi_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 4;
				pxy[0].yy = gr.g_y + gr.g_h - 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + 4;
				W_Lines(vdi_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 3;
				pxy[0].yy = gr.g_y + gr.g_h - 4;
				pxy[1].xx = gr.g_x + gr.g_w - 4;
				pxy[1].yy = gr.g_y + 3;
				W_Lines(vdi_handle, pxy, 2);
				W_PenDelete(vdi_handle, pen);
			}
		} else
		{
			if (h >= 16)
			{
				if (objc_mode == EXTOB_MAGIX)
				{
					gr.g_w -= 1;
					gr.g_h -= 1;
				} else
				{
					gr.g_x += 2;
					gr.g_y += 2;
					gr.g_w -= 4;
					gr.g_h -= 4;
				}
			}
			W_Fill_Rect(vdi_handle, &gr, pattern, color);
			pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
			W_Fill_Rect(vdi_handle, &gr, IP_SOLID, W_PAL_WHITE);
			W_Rectangle(vdi_handle, &gr);
			if (state & OS_SELECTED)
			{
				W_POINTS pxy[2];

				pxy[0].xx = gr.g_x;
				pxy[0].yy = gr.g_y;
				pxy[1].xx = gr.g_x + gr.g_w - 1;
				pxy[1].yy = gr.g_y + gr.g_h - 1;
				W_Lines(vdi_handle, pxy, 2);
				pxy[0].xx = gr.g_x;
				pxy[0].yy = gr.g_y + gr.g_h - 1;
				pxy[1].xx = gr.g_x + gr.g_w - 1;
				pxy[1].yy = gr.g_y;
				W_Lines(vdi_handle, pxy, 2);
			}
			W_PenDelete(vdi_handle, pen);
		}
		break;

	case MAC_H_CHECK:
		if (h >= 16)
		{
			gr.g_y += 1;
			gr.g_w -= 1;
			gr.g_h -= 1;
		}
		W_Fill_Rect(vdi_handle, &gr, pattern, color);
		if (0 && *w == 16 && h == 16 && get_macimage(type, state, w, &h, &mask, &data))
		{
			W_Draw_Image(vdi_handle, x, y, (*w + 15) & 0xfff0, h, data, COL_SET, W_PAL_WHITE, MD_TRANS);
		} else if (*draw_3d && 0)
		{
		} else
		{
			pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, pencolor);
			W_Rectangle(vdi_handle, &gr);
			if (state & OS_SELECTED)
			{
				W_POINTS pxy[2];

				if (h >= 16)
				{
					vsl_width(vdi_handle, 2);
				}
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + 2;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + gr.g_h - 3;
				W_Lines(vdi_handle, pxy, 2);
				if (h >= 16)
					W_Pixel(vdi_handle, pxy + 1, 1, pencolor);
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + gr.g_h - 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + 2;
				W_Lines(vdi_handle, pxy, 2);
				if (h >= 16)
					W_Pixel(vdi_handle, pxy + 1, 1, pencolor);
			}
			W_PenDelete(vdi_handle, pen);
		}
		break;
	
	case MAC_THREESTATE:
		if (*draw_3d && h >= 16)
		{
			W_POINTS pxy[4];

			if (h >= 16)
			{
				if (objc_mode == EXTOB_MAGIX)
				{
					gr.g_w -= 1;
					gr.g_h -= 1;
				} else
				{
					gr.g_x += 1;
					gr.g_y += 1;
					gr.g_w -= 2;
					gr.g_h -= 2;
				}
			}
			pen = W_PenCreate(vdi_handle, 1, W_PEN_NULL, pencolor);
			W_Fill_Rect(vdi_handle, &gr, IP_SOLID, W_PAL_WHITE);
			W_PenDelete(vdi_handle, pen);

			pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_WHITE);
			pxy[0].xx = gr.g_x + 1;
			pxy[0].yy = gr.g_y + gr.g_h - 1;
			pxy[1].xx = gr.g_x + gr.g_w - 1;
			pxy[1].yy = pxy[0].yy;
			pxy[2].xx = pxy[1].xx;
			pxy[2].yy = gr.g_y + 1;
			W_Lines(vdi_handle, pxy, 3);
			W_PenDelete(vdi_handle, pen);

			if (GetNumColors() > 2)
			{
				pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_LGRAY);
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + gr.g_h - 2;
				pxy[1].xx = gr.g_x + gr.g_w - 2;
				pxy[1].yy = pxy[0].yy;
				pxy[2].xx = pxy[1].xx;
				pxy[2].yy = gr.g_y + 2;
				W_Lines(vdi_handle, pxy, 3);
				W_PenDelete(vdi_handle, pen);
			}

			pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, pencolor);
			pxy[0].xx = gr.g_x;
			pxy[0].yy = gr.g_y + gr.g_h - 1;
			pxy[1].xx = pxy[0].xx;
			pxy[1].yy = gr.g_y;
			pxy[2].xx = gr.g_x + gr.g_w - 1;
			pxy[2].yy = pxy[1].yy;
			W_Lines(vdi_handle, pxy, 3);
			pxy[0].xx = gr.g_x + 1;
			pxy[0].yy = gr.g_y + gr.g_h - 2;
			pxy[1].xx = pxy[0].xx;
			pxy[1].yy = gr.g_y + 1;
			pxy[2].xx = gr.g_x + gr.g_w - 2;
			pxy[2].yy = pxy[1].yy;
			W_Lines(vdi_handle, pxy, 3);
			W_PenDelete(vdi_handle, pen);
			
			if (get_macimage(MAC_BOOLEAN, OS_NORMAL, w, &h, &mask, &data))
			{
				data = (_UBYTE *)threestate_3d;
				W_Draw_Icon(vdi_handle, gr.g_x + 2, gr.g_y + 2, gr.g_w - 5, gr.g_h - 5, data, mask, COL_SET, COL_RESET, FALSE);
			}
		} else
		{
			if (get_macimage(MAC_CHECK, OS_NORMAL, w, &h, &mask, &data))
			{
				data = (_UBYTE *)(*w == h ? big_threestate : small_threestate);
				W_Draw_Icon(vdi_handle, x, y, (*w + 15) & 0xfff0, h, data, mask, COL_SET, COL_RESET, FALSE);
			}
		}
		break;
	
	default:
		if (get_macimage(type, state, w, &h, &mask, &data))
			W_Draw_Icon(vdi_handle, x, y, (*w + 15) & 0xfff0, h, data, mask, COL_SET, COL_RESET, FALSE);
		break;
	}
	if ((state & OS_OUTLINED) && objc_mode != EXTOB_MAGIX)
	{
		GRECT xgr;

		xgr.g_x = x - FRAMESIZE;
		xgr.g_y = y - FRAMESIZE;
		xgr.g_w = *w + 2 * FRAMESIZE;
		xgr.g_h = h + 2 * FRAMESIZE;
		pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, pencolor);
		W_Rectangle(vdi_handle, &xgr);
		W_PenDelete(vdi_handle, pen);
	}
	return state & ~(OS_OUTLINED|OS_SELECTED|OS_CHECKED);
}


static _WORD _draw_inner_frame(_UWORD state, _UWORD flags, _WORD x, _WORD y, const _UBYTE *str)
{
	_WORD color, pattern;
	_WORD mode;
	_WORD w, h;
	void *pen;
	W_POINTS pxy[6];
	_BOOL draw_3d;

	od_setfont(vdi_handle, state & OS_CHECKED ? SMALL : IBM, W_PAL_BLACK, 0);
	od_pxy.g_y += od_ch / 2;
	od_pxy.g_h -= od_ch / 2;
	color = W_PAL_WHITE;
	pattern = IP_HOLLOW;
	draw_3d = color_3d(flags, state, &color, &pattern);
	if (od_clear || draw_3d)
		mode = MD_TRANS;
	else
		mode = MD_REPLACE;

	if (state & OS_OUTLINED)
	{
		od_box(-1, FALSE);
	}
	W_SetBkMode(vdi_handle, MD_REPLACE);

	W_Fill_Rect(vdi_handle, &od_pxy, pattern, color);
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
	if (str != NULL /* && *str != '\0' */)
	{
		W_TextExtent(vdi_handle, FONT_CURRENT, FONT_SIZE_CURRENT, TXT_ATTR_NONE, str, &w, &h);
		x += od_cw;
		pxy[0].xx = x;			pxy[0].yy = od_pxy.g_y;
		pxy[1].xx = od_pxy.g_x; pxy[1].yy = pxy[0].yy;
		pxy[2].xx = pxy[1].xx;	pxy[2].yy = pxy[0].yy + od_pxy.g_h - 1;
		pxy[3].xx = pxy[1].xx + od_pxy.g_w - 1; pxy[3].yy = pxy[2].yy;
		pxy[4].xx = pxy[3].xx;	pxy[4].yy = pxy[0].yy;
		pxy[5].xx = x + w;		pxy[5].yy = pxy[0].yy;
		W_Lines(vdi_handle, pxy, 6);
		if (draw_3d)
		{
			W_PenDelete(vdi_handle, pen);
			pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_WHITE);
			pxy[0].xx = od_pxy.g_x + 1;
			pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
			pxy[1].xx = od_pxy.g_x + od_pxy.g_w - 1;
			pxy[1].yy = pxy[0].yy;
			pxy[2].xx = pxy[1].xx;
			pxy[2].yy = od_pxy.g_y + 1;
			W_Lines(vdi_handle, pxy, 3);
		}
		W_SetBkMode(vdi_handle, mode);
		W_Text(vdi_handle, x, y, str);
		xywh2rect(x, y, w, h, &od_pxy);
		rc_copy(&od_pxy, &od_framerec);
		if (state & OS_CROSSED)
		{
			W_PenDelete(vdi_handle, pen);
			pen = _draw_box(1, W_PAL_BLACK, FALSE, IP_HOLLOW, W_PAL_WHITE);
		}
		if (od_clear)
		{
			if (((state & OS_SELECTED) && !(od_oldstate & OS_SELECTED)) ||
				(!(state & OS_SELECTED) && (od_oldstate & OS_SELECTED)) )
			{
				W_Invert_Rect(vdi_handle, &od_pxy);
			}
		} else
		{
			if (state & OS_SELECTED)
				W_Invert_Rect(vdi_handle, &od_pxy);
		}
		W_SetBkMode(vdi_handle, MD_REPLACE);
	} else
	{
		od_box(0, TRUE);
	}
	W_PenDelete(vdi_handle, pen);
	return state & ~(OS_OUTLINED|OS_SELECTED|OS_CROSSED|OS_CHECKED);
}

/* -------------------------------------------------------------------------- */

static void FilledRect(_WORD x0, _WORD y0, _WORD x1, _WORD y1, _WORD color)
{
	GRECT gr;
	
	gr.g_x = x0;
	gr.g_y = y0;
	gr.g_w = x1 - x0 + 1;
	gr.g_h = y1 - y0 + 1;
	W_Fill_Rect(vdi_handle, &gr, IP_SOLID, color);
}

/* -------------------------------------------------------------------------- */

static void Line(_WORD x0, _WORD y0, _WORD x1, _WORD y1, _WORD color)
{
	void *pen;
	W_POINTS p[2];
	
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, color);

	p[0].xx = x0;
	p[0].yy = y0;
	p[1].xx = x1;
	p[1].yy = y1;
	W_Lines(vdi_handle, p, 2);
	W_PenDelete(vdi_handle, pen);
}

/* -------------------------------------------------------------------------- */

static void draw_box5(_WORD x0, _WORD y0, _WORD x1, _WORD y1)
{
	_WORD color;

	color = W_PAL_BLACK;
	if (op_draw3d)
	{
		color = W_PAL_DGRAY;
		x1--;
		y1--;
	}

	Line(x0, y0, x0, y1, color);
	Line(x0, y1, x1, y1, color);
	Line(x1, y1, x1, y0, color);
	Line(x0, y0, x1, y0, color);

	if (op_draw3d)
	{
		Line(x0 + 1, y0 + 1, x0 + 1, y1 - 1, W_PAL_WHITE);
		Line(x0, y1 + 1, x1 + 1, y1 + 1, W_PAL_WHITE);
		Line(x1 + 1, y1 + 1, x1 + 1, y0, W_PAL_WHITE);
		Line(x0 + 1, y0 + 1, x1 - 1, y0 + 1, W_PAL_WHITE);
	}
}

/* -------------------------------------------------------------------------- */

static _WORD CenterX(_WORD x1, _WORD x2, const char *text)
{
	_WORD x;
	_WORD len;

	len = W_TextWidth(vdi_handle, text);
	x = x2 - x1;
	x /= 2;
	x += x1;
	x = x - (len / 2);
	x++;
	return x;
}

/* -------------------------------------------------------------------------- */

static void _draw_outer_frame(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, const _UBYTE *text, _UWORD flags, _UWORD state)
{
	_WORD x0, y0, x1, y1;
	_WORD len;

	UNUSED(type);
	UNUSED(flags);
	/*
	 * adjust outer dimensions.
	 */
	{
		_WORD cw, ch;
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		GetTextSize(&cw, &ch);
		xdiff = 3;
		ydiff = 3 + (ch / 4);
		wdiff = 3 * 2;
		hdiff = (3 + (ch / 4)) * 2;
		if (x != 0)
			x -= xdiff;
		if (y != 0)
			y -= ydiff;
		w += wdiff;
		h += hdiff;
	}
	
	x0 = x;
	y0 = y;
	x1 = x + w - 1;
	y1 = y + h - 1;

	W_SetBkMode(vdi_handle, MD_REPLACE);
	if (state & OS_WHITEBAK)				/* Whiteback gesetzt? */
	{
		FilledRect(x0, y0, x1, y1, op_draw3d ? color_background : W_PAL_WHITE);
	}

	draw_box5(x0, y0, x1, y1);

	if (empty(text))
		return;

	od_setfont(vdi_handle, (state & OS_CHECKED) ? SMALL : IBM, W_PAL_BLACK, TXT_ATTR_NONE);

	if (state & OS_CROSSED)
	{
		x = CenterX(x0, x1, text);
	} else
	{
		x = x0 + 10;
	}
	y = y0 - (od_ch / 2);
	y++;
	if (op_draw3d && (state & OS_CHECKED))
		y++;
	len = W_TextWidth(vdi_handle, text);
	/*
	 * repaint the background of the text by removing the lines
	 * drawn from draw_box5() above
	 */
	if (op_draw3d)
	{
		Line(x, y0, x + len - 1, y0, color_background);
		Line(x, y0 + 1, x + len - 1, y0 + 1, color_background);
	} else
	{
		Line(x, y0, x + len - 1, y0, W_PAL_WHITE);
	}
	W_SetBkMode(vdi_handle, MD_TRANS);
	W_Text(vdi_handle, x, y, text);
	W_SetBkMode(vdi_handle, MD_REPLACE);
	od_setfont(vdi_handle, IBM, W_PAL_BLACK, TXT_ATTR_NONE);
}


#if 0
_WORD _CDECL draw_inner_frame(PARMBLK *pb)
{
	GRECT gr;
	_WORD state, flags;

	xywh2rect(pb->pb_xc, pb->pb_yc, pb->pb_wc, pb->pb_hc, &gr);
	W_Clip_Rect(vdi_handle, TRUE, &gr);
	od_oldstate = pb->pb_prevstate;
	flags = pb->pb_tree[pb->pb_obj].ob_flags;
	state = _draw_inner_frame(pb->pb_currstate, flags, pb->pb_x, pb->pb_y, (_UBYTE *)(pb->pb_parm));
	W_Clip_Rect(vdi_handle, FALSE, &gr);
	return state;
}


_WORD _CDECL draw_mac(PARMBLK *pb)
{
	_WORD w;
	_BOOL draw_3d;

	return _draw_mac(pb->pb_tree[pb->pb_obj].ob_flags, pb->pb_currstate, pb->pb_x, pb->pb_y, (_WORD)(_LONG)pb->pb_parm, &w, &draw_3d);
}


_WORD _CDECL draw_circle(PARMBLK *pb)
{
	GRECT gr;
	_WORD state;

	xywh2rect(pb->pb_xc, pb->pb_yc, pb->pb_wc, pb->pb_hc, &gr);
	W_Clip_Rect(vdi_handle, TRUE, &gr);
	od_oldstate = pb->pb_prevstate;
	state = _draw_circle(pb->pb_currstate, pb->pb_x, pb->pb_y, pb->pb_w, pb->pb_h);
	W_Clip_Rect(vdi_handle, FALSE, &gr);
	return state;
}


_WORD _CDECL draw_under_line(PARMBLK *pb)
{
	GRECT gr;
	_WORD state;

	xywh2rect(pb->pb_xc, pb->pb_yc, pb->pb_wc, pb->pb_hc, &gr);
	W_Clip_Rect(vdi_handle, TRUE, &gr);
	od_oldstate = pb->pb_prevstate;
	state = _draw_under_line(pb->pb_currstate, pb->pb_x, pb->pb_y, pb->pb_w, pb->pb_h, (_UBYTE *)(pb->pb_parm), -1);
	W_Clip_Rect(vdi_handle, FALSE, &gr);
	return state;
}
#endif


static _WORD _draw_under_line(_UWORD state, _WORD x, _WORD y, _WORD w, _WORD h, _UBYTE *str, _WORD offset)
{
	W_POINTS pxy[2];
	void *pen;
	_WORD bold;
	_WORD size;
	
	if ((state & OS_WHITEBAK) && objc_mode != EXTOB_MAGIX)
		bold = TXT_ATTR_BOLD;
	else
		bold = TXT_ATTR_NONE;
	size = IBM;
	if (objc_mode == EXTOB_MAGIC || objc_mode == EXTOB_ORCS)
	{
		if (state & OS_CHECKED)
			size = SMALL;
		if (state & OS_WHITEBAK)
			bold |= TXT_ATTR_BOLD;
		if (state & OS_DRAW3D)
			bold |= TXT_ATTR_SHADOWED;
	}
	od_setfont(vdi_handle, size, W_PAL_BLACK, bold);
	y += (h - od_ch) / 2;
	pxy[0].xx = x;
	pxy[0].yy = y + od_ch;
	pxy[1].xx = x + w - 1;
	pxy[1].yy = pxy[0].yy;
	if (str != NULL && *str != '\0')
	{
#if 0
		W_SetBkMode(vdi_handle, MD_REPLACE);
		W_Text(vdi_handle, x, y, str);
#else
		GRECT gr;

		gr.g_x = x;
		gr.g_y = y;
		gr.g_w = w;
		gr.g_h = h;
		if (bold & TXT_ATTR_SHADOWED)
		{
			bold &= ~TXT_ATTR_SHADOWED;
			gr.g_x += 2;
			gr.g_y += 2;
			draw_text(str, &gr, size, TE_LEFT, FALSE, W_PAL_BLACK, MD_TRANS, offset, bold);
			gr.g_x -= 1;
			gr.g_y -= 1;
			draw_text(str, &gr, size, TE_LEFT, FALSE, W_PAL_WHITE, MD_TRANS, offset, bold);
			gr.g_x -= 1;
			gr.g_y -= 1;
			draw_text(str, &gr, size, TE_LEFT, FALSE, W_PAL_BLACK, MD_TRANS, offset, bold);
		} else
		{
			draw_text(str, &gr, size, TE_LEFT, FALSE, W_PAL_BLACK, MD_TRANS, offset, bold);
		}
#endif
	}
	if (objc_mode == EXTOB_MAGIC)
	{
		if (state & (OS_SHADOWED|OS_OUTLINED))
		{
			pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
			W_Lines(vdi_handle, pxy, 2);
			if (state & OS_OUTLINED)
			{
				pxy[0].yy += 2;
				pxy[1].yy += 2;
				W_Lines(vdi_handle, pxy, 2);
			}
			W_PenDelete(vdi_handle, pen);
		}
	} else
	{
		outline(state);
		if (bold == TXT_ATTR_NONE && offset == -1)
		{
			pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
			W_Lines(vdi_handle, pxy, 2);
			W_PenDelete(vdi_handle, pen);
		}
	}
	return state & ~(OS_SELECTED|OS_SHADOWED|OS_OUTLINED|OS_WHITEBAK|OS_CHECKED|OS_DRAW3D);
}


_UWORD _draw_circle(_UWORD state, _WORD x, _WORD y, _WORD w, _WORD h)
{
	void *data;
	void *pen;

	xywh2rect(x, y, w, h, &od_pxy);
	rc_copy(&od_pxy, &od_framerec);
	rc_copy(&od_pxy, &od_outrec);
	pen = _draw_box(-1, W_PAL_BLACK, TRUE, IP_HOLLOW, W_PAL_WHITE);
	if (state & OS_SHADOWED)
		shadow(-1, W_PAL_BLACK);
	W_PenDelete(vdi_handle, pen);

	od_setfont(vdi_handle, IBM, W_PAL_BLACK, TXT_ATTR_NONE);
	if (od_ch < 13)
	{
		w = 16;
		h = 8;
		data = small_circle;
	} else
	{
		w = h = 16;
		data = big_circle;
	}
	W_Draw_Image(vdi_handle, x, y, w, h, data, COL_SET, W_PAL_WHITE, MD_TRANS);
	return state & ~(OS_SELECTED|OS_OUTLINED|OS_CROSSED|OS_CHECKED|OS_SHADOWED);
}


void _draw_image(BITBLK *bi, _WORD x, _WORD y, _BOOL selected, _BOOL convert)
{
	_WORD w, h;

	x += bi->bi_x;
	y += bi->bi_y;
	w = bi->bi_wb * 8;
	h = bi->bi_hl;
	xywh2rect(x, y, w, h, &od_framerec);
	if (od_clear)
		if ((!selected && (od_oldstate & OS_SELECTED)) ||
			(selected && !(od_oldstate & OS_SELECTED)))
			W_Invert_Rect(vdi_handle, &od_framerec);
/*
	if (dst.fd_nplanes == 1)
	{
		vro_cpyfm(vdi_handle, selected ? NOT_SORD : S_OR_D, _pxy, &src, &dst);
	} else
*/
	{
		void *pdata = bi->bi_pdata;

		if (selected)
			W_Draw_Image(vdi_handle, x, y, w, h, pdata, W_PAL_WHITE, bi->bi_color, MD_ERASE);
		else
			W_Draw_Image(vdi_handle, x, y, w, h, pdata, bi->bi_color, W_PAL_WHITE, MD_TRANS);
	}
}


static void _draw_icon(ICONBLK *ib, _WORD x, _WORD y, _BOOL selected, _BOOL convert)
{
	_UBYTE c;
	_UWORD icolor;
	_UBYTE *txt;
	_UWORD maskcol, datacol;
	_WORD w, h;
	void *pdata, *pmask;

	x += ib->ib_xicon;
	y += ib->ib_yicon;
	icolor = ib->ib_char;
	datacol = ICOLSPEC_GET_DATACOL(icolor);
	maskcol = ICOLSPEC_GET_MASKCOL(icolor);
	w = ib->ib_wicon;
	h = ib->ib_hicon;
	xywh2rect(x, y, w, h, &od_framerec);
	W_SetBkMode(vdi_handle, MD_REPLACE);
	pdata = ib->ib_pdata;
	pmask = ib->ib_pmask;
	W_Draw_Icon(vdi_handle, x, y, w, h, pdata, pmask, datacol, maskcol, selected);
	if ((txt = ib->ib_ptext) != NULL && ib->ib_wtext > 0)
	{
		_WORD offset;
		_UBYTE *str = txt;

		xywh2rect(x - ib->ib_xicon + ib->ib_xtext, y - ib->ib_yicon + ib->ib_ytext, ib->ib_wtext, ib->ib_htext, &od_pxy);
		offset = -1;
		if (strchr(str, '[') != NULL)
		{
			offset = (_WORD)(strchr(str, '[') - (char *)str);
			if (offset != -1)
			{
				str = g_strdup(str);
				if (str == NULL)
				{
					str = ib->ib_ptext;
					offset = -1;
				}
			}
		}

		if (selected)
		{
			W_Fill_Rect(vdi_handle, &od_pxy, IP_SOLID, datacol);
			draw_text(str, &od_pxy, SMALL, TE_CNTR, TRUE, maskcol, 0, offset, TXT_ATTR_NONE);
		} else
		{
			W_Fill_Rect(vdi_handle, &od_pxy, IP_SOLID, maskcol);
			draw_text(str, &od_pxy, SMALL, TE_CNTR, TRUE, datacol, 0, offset, TXT_ATTR_NONE);
		}
		if (offset != -1)
			g_free(str);
	}
	c = ICOLSPEC_GET_CHARACTER(icolor);
	if (c != 0)
	{
		GRECT gr;

		xywh2rect(x + ib->ib_xchar, y + ib->ib_ychar, CHARWIDTH, CHARHEIGHT, &gr);
		draw_text(char2text(c), &gr, SMALL, TE_LEFT, FALSE, selected ? maskcol : datacol, 0, -1, TXT_ATTR_NONE);
	}
}

/*** ---------------------------------------------------------------------- ***/

static void _draw_cicon(CICONBLK *cib, _WORD x, _WORD y, _BOOL selected, _BOOL convert)
{
	_UBYTE c;
	_UBYTE *txt;
	_UWORD icolor;
	_UWORD maskcol, datacol;
	_WORD w, h;
	_WORD dx, dy;
	CICON *cicon;

	icolor = cib->monoblk.ib_char;
	datacol = ICOLSPEC_GET_DATACOL(icolor);
	maskcol = ICOLSPEC_GET_MASKCOL(icolor);
	w = cib->monoblk.ib_wicon;
	h = cib->monoblk.ib_hicon;

	dx = x + cib->monoblk.ib_xicon;
	dy = y + cib->monoblk.ib_yicon;
	xywh2rect(dx, dy, w, h, &od_framerec);

	cicon = cicon_best_match(cib, GetNumPlanes());
	if (cicon == NULL)
		cicon = cicon_find_plane(cib->mainlist, 1);
	if (cicon != NULL)
	{
		W_Draw_Cicon(vdi_handle, dx, dy, w, h, cicon, datacol, maskcol, selected, convert);
	}

	if (cicon == NULL)
	{
		void *pdata = cib->monoblk.ib_pdata;
		void *pmask = cib->monoblk.ib_pmask;
		
		W_Draw_Icon(vdi_handle, dx, dy, w, h, pdata, pmask, datacol, maskcol, selected);
	}
	if ((txt = cib->monoblk.ib_ptext) != NULL && txt[0] != '\0')
	{
		_WORD offset;
		_WORD oldmode = W_SetBkMode(vdi_handle, MD_TRANS);

		od_pxy.g_x = x + cib->monoblk.ib_xtext;
		od_pxy.g_y = y + cib->monoblk.ib_ytext;
		od_pxy.g_w = cib->monoblk.ib_wtext;
		od_pxy.g_h = cib->monoblk.ib_htext;
		offset = -1;
		if (selected)
		{
			W_Fill_Rect(vdi_handle, &od_pxy, IP_SOLID, datacol);
			draw_text(cib->monoblk.ib_ptext, &od_pxy, SMALL, TE_CNTR, TRUE, maskcol, MD_TRANS, offset, TXT_ATTR_NONE);
		} else
		{
			W_Fill_Rect(vdi_handle, &od_pxy, IP_SOLID, maskcol);
			draw_text(cib->monoblk.ib_ptext, &od_pxy, SMALL, TE_CNTR, TRUE, datacol, MD_TRANS, offset, TXT_ATTR_NONE);
		}
		W_SetBkMode(vdi_handle, oldmode);
	}
	c = ICOLSPEC_GET_CHARACTER(icolor);
	if (c != 0)
	{
		GRECT gr;

		xywh2rect(x + cib->monoblk.ib_xicon + cib->monoblk.ib_xchar, y + cib->monoblk.ib_yicon + cib->monoblk.ib_ychar, CHARWIDTH, CHARHEIGHT, &gr);
		draw_text(char2text(c), &gr, SMALL, TE_LEFT, FALSE, selected ? maskcol : datacol, 0, -1, TXT_ATTR_NONE);
	}
}

/*** ---------------------------------------------------------------------- ***/

static _UWORD draw_check_radio(_WORD flags, _WORD state, _WORD x, _WORD y, _UBYTE *cp, _WORD offset, _WORD type)
{
	_WORD w;
	_BOOL draw_3d;

	od_setfont(vdi_handle, IBM, W_PAL_BLACK, TXT_ATTR_NONE);
	if (flags & 0x0800)
	{
		od_pxy.g_w -= 16;
		state = _draw_mac(flags, state, x + od_pxy.g_w, y, type, &w, &draw_3d);
		od_pxy.g_w -= od_cw;
	} else
	{
		state = _draw_mac(flags, state, x, y, type, &w, &draw_3d);
		od_pxy.g_x += w + od_cw;
		od_pxy.g_w -= w + od_cw;
	}
	if (draw_3d)
	{
		if (GetNumColors() >= 16)
		{
			draw_text(cp, &od_pxy, IBM, TE_LEFT, TRUE, state & OS_DISABLED ? color_disabled : W_PAL_BLACK, MD_TRANS, offset, TXT_ATTR_NONE);
		} else
		{
			draw_text(cp, &od_pxy, IBM, TE_LEFT, TRUE, W_PAL_BLACK, MD_TRANS, offset, TXT_ATTR_NONE);
			if (state & OS_DISABLED)
			{
				od_pxy.g_x = x;
				od_pxy.g_w = w;
				W_Disable_Rect(vdi_handle, &od_pxy);
			}
		}
	} else
	{
		draw_text(cp, &od_pxy, IBM, TE_LEFT, TRUE, W_PAL_BLACK, MD_TRANS, offset, TXT_ATTR_NONE);
		if (state & OS_DISABLED)
		{
			W_Disable_Rect(vdi_handle, &od_pxy);
		}
	}
	return state & ~OS_DISABLED;
}

/*** ---------------------------------------------------------------------- ***/

static _UWORD draw_threestate(_WORD flags, _WORD state, _WORD x, _WORD y, _UBYTE *cp, _WORD offset)
{
	switch (state & (OS_SELECTED|OS_CHECKED))
	{
	case OS_NORMAL:
		state = draw_check_radio(flags, state & ~(OS_SELECTED|OS_CHECKED), x, y, cp, offset, MAC_THREESTATE);
		break;
	case OS_CHECKED:
		state = draw_check_radio(flags, (state & ~(OS_SELECTED|OS_CHECKED)) | OS_SELECTED, x, y, cp, offset, MAC_CHECK);
		break;
	default:
		state = draw_check_radio(flags, state & ~(OS_SELECTED|OS_CHECKED), x, y, cp, offset, MAC_CHECK);
		break;
	}
	return state;
}

/*** ---------------------------------------------------------------------- ***/

_WORD _CDECL draw_userdef(PARMBLK *pb)
{
	GRECT gr;
	void *pen;

	xywh2rect(pb->pb_xc, pb->pb_yc, pb->pb_wc, pb->pb_hc, &gr);
	W_Clip_Rect(vdi_handle, TRUE, &gr);
	/* draw an IBOX type frame */
	xywh2rect(pb->pb_x, pb->pb_y, pb->pb_w, pb->pb_h, &od_pxy);
	W_SetBkMode(vdi_handle, MD_REPLACE);
	W_Clear_Rect(vdi_handle, &od_pxy);
	od_box(1, FALSE);
	rc_copy(&od_pxy, &od_framerec);
	pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
	od_box(-1, TRUE);
	W_PenDelete(vdi_handle, pen);
	rc_copy(&od_pxy, &od_outrec);
	outline(pb->pb_currstate);
	if (od_clear)
	{
		if (((pb->pb_currstate & OS_SELECTED) && !(pb->pb_prevstate & OS_SELECTED)) ||
			(!(pb->pb_currstate & OS_SELECTED) && (pb->pb_prevstate & OS_SELECTED)) )
		{
			W_Invert_Rect(vdi_handle, &od_pxy);
		}
	} else
	{
		if (pb->pb_currstate & OS_SELECTED)
			W_Invert_Rect(vdi_handle, &od_pxy);
	}
	W_Clip_Rect(vdi_handle, FALSE, &gr);
	return pb->pb_currstate & ~(OS_OUTLINED|OS_SELECTED); /* nothing more to do for GEM */
}


static void show_cursor(_WORD x, _WORD y, OBJECT *objects)
{
	_WORD wchar, hchar;

	GetTextSize(&wchar, &hchar);
#ifdef __TURBOC__
#pragma warn -stv
#endif
	TO_draw_obj(x, y, wchar * CUR_WIDTH, hchar, G_BOXCHAR, objects[ROOT].ob_spec, OF_NONE, OS_NORMAL, FALSE, FALSE);
#ifdef __TURBOC__
#pragma warn .stv
#endif
}

#ifdef OS_ATARI
#include "ciconrsc.h"
#endif


void TO_draw_obj(
	_WORD x, _WORD y, _WORD w, _WORD h,
	_UWORD type,
	OBSPEC obspec,
	_UWORD flags, _UWORD state,
	_BOOL for_menu,
	_BOOL is_root
)
{
	_WORD framesize;
	_WORD framecol;
	_WORD offset;
	_UBYTE *cp;
	_BOOL ted_check;
	_WORD bold;
	_WORD centered, centver;
	_WORD textmode;
	void *pen;

	framesize = 0;
	framecol = W_PAL_BLACK;
	ted_check = FALSE;
	bold = TXT_ATTR_NONE;
	xywh2rect(x, y, w, h, &od_pxy);
	rc_copy(&od_pxy, &od_framerec);
	rc_copy(&od_pxy, &od_outrec);
	if (flags & OF_INDIRECT)
		obspec = *(obspec.indirect);
	W_SetBkMode(vdi_handle, MD_REPLACE);
	W_SetBkColor(vdi_handle, W_PAL_WHITE);

	switch (type & OBTYPEMASK)
	{
	case G_BOX:
	case G_BOXCHAR:
	case G_EXTBOX:
		switch (is_ext_type(&objc_options, type, flags, state))
		{
		case EXTTYPE_CIRCLE:
			state = _draw_circle(state, x, y, w, h);
			break;
		case EXTTYPE_SLIDE:
			{
				MY_SLIDE slide;
				MY_SLIDE *slider;
			
				if ((type & OBTYPEMASK) == G_BOX)
				{
					_LONG val, count;
					
					MemSetZeroStruct(slide);
					slider = &slide;
					slide.sl_value = &val;
					slide.sl_count = &count;
					slide.sl_pixel_pos = 30;
					slide.sl_pixel_size = 25;
					count = 100;
					val = 30;
					framesize = OBSPEC_GET_FRAMESIZE(obspec);
				} else
				{
					slider = SLIDER_GETOBSPEC(obspec);
					framesize = OBSPEC_GET_FRAMESIZE(slider->sl_obspec);
				}
				state = hdraw_slider(x, y, w, h, slider, state);
			}
			break;
		case EXTTYPE_FLYMOVER:
			{
				W_POINTS pxy[2];

				pxy[0].xx = od_pxy.g_x;
				pxy[0].yy = od_pxy.g_y;
				pxy[1].xx = pxy[0].xx + od_pxy.g_w - 1;
				pxy[1].yy = pxy[0].yy + od_pxy.g_h - 1;
				pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
				W_SetBkMode(vdi_handle, MD_REPLACE);
				W_Lines(vdi_handle, pxy, 2);
				W_PenDelete(vdi_handle, pen);
				state &= ~OS_CROSSED;
			}
			break;
		case EXTTYPE_OUTERFRAME:
			_draw_outer_frame(x, y, w, h, type, NULL, flags, state);
			state = OS_NORMAL;
			break;
		case EXTTYPE_SYSGEM_BOX:
			if (for_menu)
				sysgem_draw_mbox(x, y, w, h, type, obspec, flags, state);
			else
				sysgem_draw_box(x, y, w, h, type, obspec, flags, state, is_root);
			state = OS_NORMAL;
			break;
		case EXTTYPE_SYSGEM_BOXCHAR:
			if (for_menu && 0)
				sysgem_draw_mbox(x, y, w, h, type, obspec, flags, state);
			else
				sysgem_draw_boxchar(x, y, w, h, type, obspec, flags, state);
			state = OS_NORMAL;
			break;
		case EXTTYPE_SYSGEM_BAR1:
		case EXTTYPE_SYSGEM_BAR2:
		case EXTTYPE_SYSGEM_BAR3:
			/*
			 * must be handled by caller, because sysgem_draw_bar()
			 * needs the idx of the next object;
			 * if we get here, the next object does not exist yet or
			 * has the wrong type, fall through to normal G_BOX type
			 */
		default:
			textmode = OBSPEC_GET_TEXTMODE(obspec);
			{
				_WORD color;
				_WORD pattern;
				_BOOL filled = TRUE;

				framesize = OBSPEC_GET_FRAMESIZE(obspec);
				color = OBSPEC_GET_INTERIORCOL(obspec);
				pattern = OBSPEC_GET_FILLPATTERN(obspec);
				if (color_3d(flags, state, &color, &pattern))
				{
					textmode = FALSE;
					if (GetNumColors() >= 16 || op_use3d == DRAW3D_FORCE)
					{
						if (type == (G_BOXCHAR | (G_OBJX_SHORTCUT << 8)))
						{
							_UBYTE empty = '\0';

							framesize--;
							if (framesize < 0)
							{
								x -= framesize;
								y -= framesize;
								w += 2 * framesize;
								h += 2 * framesize;
							}
							hdraw_button(state, flags, x, y, w, h, &empty, FALSE, -1, framesize);
							filled = FALSE;
							framesize = 0;
							if (state & OS_SELECTED)
							{
								od_pxy.g_x++;
								od_pxy.g_y++;
							}
							state &= ~OS_SELECTED;
						} else if ((type & OBTYPEMASK) == G_BOX &&
							framesize == 2 &&
							(state & OS_OUTLINED) &&
							(flags & OF_FL3DMASK) != 0)
						{
							/*
							 * Aussenrahmen fuer Dialogboxen
							 */
							draw_dialog_frame(pattern, color);
							framesize = 0;
							state &= ~OS_OUTLINED;
							filled = FALSE;
						} else if (framesize != 0 && 0 &&
							(objc_mode == EXTOB_MAGIX ||
							 objc_mode == EXTOB_MAGIC ||
							 objc_mode == EXTOB_ORCS))
						{
							_UBYTE empty = '\0';
							
							hdraw_button(state, flags, x, y, w, h, &empty, FALSE, -1, framesize);
							filled = FALSE;
							framesize = 0;
						}
						if (pattern == IP_SOLID)
							state &= ~OS_SELECTED;
					}
				}
				pen = _draw_box(framesize, OBSPEC_GET_FRAMECOL(obspec), filled, pattern, color);
				W_PenDelete(vdi_handle, pen);
			}
			if ((type & OBTYPEMASK) == G_BOXCHAR)
			{
				draw_char(OBSPEC_GET_CHARACTER(obspec), &od_pxy, IBM,
					TE_CNTR, TRUE, OBSPEC_GET_TEXTCOL(obspec), textmode);
			}
			outline(state);
			if (state & OS_SELECTED)
			{
				if (state & OS_CROSSED)
					cross(&od_framerec);
				state &= ~OS_CROSSED;
				W_Invert_Rect(vdi_handle, &od_framerec);
			}
			break;
		}
		break;

	case G_IBOX:
		if (for_menu)
		{
			state = OS_NORMAL;
		} else
		{
			pen = _draw_box(framesize = OBSPEC_GET_FRAMESIZE(obspec),
				OBSPEC_GET_FRAMECOL(obspec),
				FALSE, IP_HOLLOW, W_PAL_WHITE);
			if (framesize < 0)
			{
				od_box(0, TRUE);
			}
			W_PenDelete(vdi_handle, pen);
			outline(state);
			if (od_clear)
			{
				if ( ((state & OS_SELECTED) && !(od_oldstate & OS_SELECTED)) ||
					 (!(state & OS_SELECTED) && (od_oldstate & OS_SELECTED)) )
				{
					W_Invert_Rect(vdi_handle, &od_framerec);
				}
			} else
			{
				if (state & OS_SELECTED)
					W_Invert_Rect(vdi_handle, &od_framerec);
			}
		}
		switch (is_ext_type(&objc_options, type, flags, state))
		{
		case EXTTYPE_FLYMOVER:
			{
				W_POINTS pxy[2];

				pxy[0].xx = od_pxy.g_x;
				pxy[0].yy = od_pxy.g_y;
				pxy[1].xx = pxy[0].xx + od_pxy.g_w - 1;
				pxy[1].yy = pxy[0].yy + od_pxy.g_h - 1;
				pen = W_PenCreate(vdi_handle, 1, W_PEN_SOLID, W_PAL_BLACK);
				vswr_mode(vdi_handle, MD_REPLACE);
				W_Lines(vdi_handle, pxy, 2);
				W_PenDelete(vdi_handle, pen);
				state &= ~OS_CROSSED;
			}
			break;
		default:
			break;
		}
		break;

	case G_FTEXT:
	case G_FBOXTEXT:
		ted_check = TRUE;
		/* FALL THROUGH */
	case G_TEXT:
	case G_BOXTEXT:
		switch (is_ext_type(&objc_options, type, flags, state))
		{
		case EXTTYPE_LIST:
			/* node representing popup, handled in main loop */
			framesize = 0;
			break;
		case EXTTYPE_POPUP_SIMPLE:
		case EXTTYPE_POPUP_SINGLE:
		case EXTTYPE_POPUP_MULTIPLE:
			state = hdraw_popup(type, x, y, w, h, obspec, flags, state, NULL);
			break;
		case EXTTYPE_LIST_BOX:
			{
				TEDINFO *ted = obspec.tedinfo;
				_UWORD color = ted->te_color;

				pen = _draw_box(framesize = ted->te_thickness, COLSPEC_GET_FRAMECOL(color),
					TRUE, COLSPEC_GET_FILLPATTERN(color), COLSPEC_GET_INTERIORCOL(color));
				W_PenDelete(vdi_handle, pen);
				outline(state);
			}
			break;
		case EXTTYPE_TITLE:
			if (ted_check)
			{
				TEDINFO *ted = obspec.tedinfo;

				cp = calc_text(NULL, ted->te_ptmplt, ted->te_tmplen, ted->te_ptext, ted->te_txtlen, ted->te_pvalid, FALSE, '\0', 0);
				if (cp != NULL)
				{
					state = hdraw_title(state, x, y, w, h, cp);
					g_free(cp);
				}
			} else
			{
				state = hdraw_title(state, x, y, w, h, obspec.tedinfo->te_ptext);
			}
			break;

		case EXTTYPE_MOVER:
			if (ted_check)
				cp = strchr(obspec.tedinfo->te_ptmplt, '$');
			else
				cp = strchr(obspec.tedinfo->te_ptext, '$');
			if (cp != NULL)
				*cp = '\0';
			draw_ted(obspec.tedinfo, ted_check,
				(type & OBTYPEMASK) == G_BOXTEXT || (type & OBTYPEMASK) == G_FBOXTEXT,
				TXT_ATTR_NONE, ' ', flags, state, &framesize, &framecol);
			outline(state);
			if (cp != NULL)
				*cp = '$';
			break;
		
		case EXTTYPE_SYSGEM_TEXT:
			sysgem_draw_text(x, y, w, h, type, obspec, flags, state);
			state = OS_NORMAL;
			break;
			
		case EXTTYPE_SYSGEM_EDIT:
			sysgem_draw_edit(x, y, w, h, type, obspec, flags, state);
			state = OS_NORMAL;
			break;
			
		case EXTTYPE_SYSGEM_LISTBOX:
		case EXTTYPE_SYSGEM_CIRCLE:
			sysgem_draw_listbox(x, y, w, h, type, obspec, flags, state);
			state = OS_NORMAL;
			break;
			
		default:
			{
				_UBYTE fillchar = '_';
				TEDINFO ted;

				MemCpyStruct(ted, *obspec.tedinfo);
				if (!(flags & OF_EDITABLE))
					fillchar = ' ';
				draw_ted(&ted, ted_check,
					(type & OBTYPEMASK) == G_BOXTEXT || (type & OBTYPEMASK) == G_FBOXTEXT,
					bold, fillchar, flags, state, &framesize, &framecol);
				outline(state);
			}
			break;
		}
		break;

	case G_MTEXT:
		if (mtext_draw != FUNK_NULL)
		{
			MTEDINFO *mted = (MTEDINFO *)obspec.tedinfo;

			framesize = (*mtext_draw)(vdi_handle, mted, state & OS_SELECTED ? TRUE : FALSE);
			outline(state);
			(*mtext_show_cursor)(mted, x, y, show_cursor, cur_left, cur_right);
		}
		break;

	case G_IMAGE:
		_draw_image(obspec.bitblk, x, y, state & OS_SELECTED ? TRUE : FALSE, (type & OBEXTTYPEMASK) == G_OBJX_MUST_CONVERT);
		outline(state);
		break;

	case G_ICON:
		_draw_icon(obspec.iconblk, x, y, state & OS_SELECTED ? TRUE : FALSE, (type & OBEXTTYPEMASK) == G_OBJX_MUST_CONVERT);
		break;

	case G_CICON:
		_draw_cicon(obspec.ciconblk, x, y, state & OS_SELECTED ? TRUE : FALSE, (type & OBEXTTYPEMASK) == G_OBJX_MUST_CONVERT);
		break;

	case G_USERDEF:
		if (od_code == 0)
			od_code = draw_userdef;
		od_parm.pb_currstate = state;
		od_parm.pb_x = x;
		od_parm.pb_y = y;
		od_parm.pb_w = w;
		od_parm.pb_h = h;
		state = (*od_code)(&od_parm);
		break;

	case G_BUTTON:
		framesize = -1;
		if (flags & OF_EXIT)
			framesize--;
		if (flags & OF_DEFAULT)
			framesize--;
		offset = -1;
		centered = TE_CNTR;
		centver = TRUE;
		{
			_UBYTE *txt = g_strdup(obspec.free_string);
			_WORD wr_mode = MD_REPLACE;

			if (objc_mode != EXTOB_MAGIX)
			{
				cp = strchr(txt, '[');
				if (cp != NULL)
					offset = (_WORD)(cp - txt);
			}
			cp = txt;

			switch (is_ext_type(&objc_options, type, flags, state))
			{
			case EXTTYPE_EXIT:
			case EXTTYPE_UNDO2:
			case EXTTYPE_DEFAULT:
				od_box(-1, FALSE);
				rc_copy(&od_pxy, &od_framerec);
				if ((objc_mode == EXTOB_ORCS || objc_mode == EXTOB_MAGIC) && op_draw3d)
				{
					state = hdraw_button(state, flags, od_pxy.g_x, od_pxy.g_y, od_pxy.g_w, od_pxy.g_h, cp, FALSE, offset, framesize);
					cp = NULL;
				} else
				{
					if (objc_mode == EXTOB_MAGIX)
					{
						offset = (state >> 8) & 0xff;
						if (offset >= 0xfe || offset >= (int)strlen(cp))
							offset = -1;
						if (op_draw3d)
						{
							state = hdraw_button(state, flags, od_pxy.g_x, od_pxy.g_y, od_pxy.g_w, od_pxy.g_h, cp, FALSE, offset, framesize);
							cp = NULL;
						}
					}
				}
				break;
			case EXTTYPE_UNDO:
				if (objc_mode == EXTOB_ORCS ||
					objc_mode == EXTOB_MYDIAL ||
					objc_mode == EXTOB_MAGIC)
				{
					od_box(-1, FALSE);
					rc_copy(&od_pxy, &od_framerec);
				} else
				{
					offset = -1;
				}
				break;
			case EXTTYPE_RADIO:
				if (objc_mode == EXTOB_MAGIX)
				{
					offset = (state >> 8) & 0xff;
					if (offset >= 0xfe || offset >= (int)strlen(cp))
						offset = -1;
				}
				state = draw_check_radio(flags, state, x, y, cp, offset, state & OS_WHITEBAK ? MAC_H_RADIO : MAC_RADIO);
				cp = NULL;
				break;
			
			case EXTTYPE_CHECK:
				if (objc_mode == EXTOB_MAGIX)
				{
					offset = (state >> 8) & 0x7f;
					if (offset >= 0x7e || offset >= (int)strlen(cp))
						offset = -1;
				}
				state = draw_check_radio(flags, state, x, y, cp, offset, state & OS_WHITEBAK ? MAC_H_CHECK : MAC_CHECK);
				cp = NULL;
				break;
			
			case EXTTYPE_CHECK_SWITCH:
				if (objc_mode == EXTOB_MAGIX)
				{
					offset = (state >> 8) & 0x7f;
					if (offset >= 0x7e || offset >= (int)strlen(cp))
						offset = -1;
				}
				state = draw_check_radio(flags, state, x, y, cp, offset, MAC_SWITCH);
				cp = NULL;
				break;
			
			case EXTTYPE_CHECK_BOOLEAN:
				if (objc_mode == EXTOB_MAGIX)
				{
					offset = (state >> 8) & 0x7f;
					if (offset >= 0x7e || offset >= (int)strlen(cp))
						offset = -1;
				}
				state = draw_check_radio(flags, state, x, y, cp, offset, MAC_BOOLEAN);
				cp = NULL;
				break;
			
			case EXTTYPE_THREESTATE:
				state = draw_threestate(flags, state, x, y, cp, offset);
				cp = NULL;
				break;
			
			case EXTTYPE_LINE:
				if (objc_mode != EXTOB_MAGIC)
					if (!(flags & (OF_EXIT|OF_TOUCHEXIT)))
						offset = -1;
				state = _draw_under_line(state, x, y, w, h, cp, offset);
				cp = NULL;
				break;
			
			case EXTTYPE_INNERFRAME:
				state = _draw_inner_frame(state, flags, x, y, cp);
				cp = NULL;
				break;

			case EXTTYPE_TITLE:
				state = hdraw_title(state, x, y, w, h, cp);
				cp = NULL;
				break;
			
			case EXTTYPE_MOVER:
				cp = DialogTitelTxt(cp, state, FALSE);
				if (cp != NULL)
				{
					state = hdraw_button(state, flags, x, y, w, h, cp, FALSE, -1, framesize);
					g_free(cp);
					cp = NULL;
				}
				break;
			
			case EXTTYPE_SLIDE:
				{
					MY_SLIDE slider;

					MemSetZeroStruct(slider);
					state = hdraw_slider(x, y, w, h, &slider, state);
				}
				cp = NULL;
				break;
			
			case EXTTYPE_LIST_BOX:
				pen = _draw_box(framesize, W_PAL_BLACK, TRUE, IP_HOLLOW, W_PAL_WHITE);
				W_PenDelete(vdi_handle, pen);
				cp = NULL;
				break;

			case EXTTYPE_SYSGEM_BUTTON:
			case EXTTYPE_SYSGEM_TOUCHEXIT:
				sysgem_draw_button(x, y, w, h, type, obspec, flags, state, txt, offset);
				state = OS_NORMAL;
				cp = NULL;
				break;

			case EXTTYPE_SYSGEM_RADIO:
				sysgem_draw_radio(x, y, w, h, type, obspec, flags, state, txt, offset);
				state = OS_NORMAL;
				cp = NULL;
				break;

			case EXTTYPE_SYSGEM_SELECT:
				sysgem_draw_select(x, y, w, h, type, obspec, flags, state, txt, offset);
				state = OS_NORMAL;
				cp = NULL;
				break;

			case EXTTYPE_SYSGEM_FRAME:
				sysgem_draw_frame(x, y, w, h, type, obspec.free_string, flags, state);
				state = OS_NORMAL;
				cp = NULL;
				break;

			case EXTTYPE_SYSGEM_HELP:
				sysgem_draw_help(x, y, w, h, type, obspec, flags, state, txt, offset);
				state = OS_NORMAL;
				cp = NULL;
				break;

			case EXTTYPE_SYSGEM_NOTEBOOK:
				sysgem_draw_notebook(x, y, w, h, type, obspec, flags, state, txt, offset);
				state = OS_NORMAL;
				cp = NULL;
				break;

			default:
				if (objc_mode == EXTOB_MAGIC && op_draw3d && !(flags & OF_TOUCHEXIT))
				{
					od_box(-1, FALSE);
					rc_copy(&od_pxy, &od_framerec);
					state = hdraw_button(state, flags, od_pxy.g_x, od_pxy.g_y, od_pxy.g_w, od_pxy.g_h, cp, FALSE, offset, framesize);
					cp = NULL;
				} else if (objc_mode == EXTOB_MAGIX && op_draw3d)
				{
					rc_copy(&od_pxy, &od_framerec);
					state = hdraw_button(state, flags, od_pxy.g_x, od_pxy.g_y, od_pxy.g_w, od_pxy.g_h, cp, FALSE, offset, framesize);
					cp = NULL;
				} else
				{
					offset = -1;
				}
				break;
			}

			if (cp != NULL && wr_mode == MD_REPLACE)
			{
				pen = _draw_box(framesize, W_PAL_BLACK, TRUE, IP_HOLLOW, W_PAL_WHITE);
				W_PenDelete(vdi_handle, pen);
			}
			outline(state);
			if (cp != NULL)
			{
				draw_text(cp, &od_pxy, IBM, centered, centver, W_PAL_BLACK, wr_mode, offset, bold);
			}
			if (state & OS_SELECTED)
				W_Invert_Rect(vdi_handle, &od_framerec);
			g_free(txt);
		}
		break;

	case G_TITLE:
		od_box(1, FALSE);
		/* FALL THROUGH */
	case G_STRING:
	case G_SHORTCUT:
		{
			_WORD color, pattern;

			color = W_PAL_WHITE;
			pattern = IP_HOLLOW;
			if (od_clear | color_3d(flags, state, &color, &pattern))
			{
				W_Fill_Rect(vdi_handle, &od_pxy, pattern, color);
				if (pattern == IP_SOLID)
					state &= ~OS_SELECTED;
			}
		}
		{
			_UBYTE *txt;

			offset = -1;
			txt = g_strdup(obspec.free_string);
			if (objc_mode != EXTOB_MAGIX && txt != NULL &&
				((type & OBEXTTYPEMASK) != 0))
			{
				if ((cp = strchr(txt, '[')) != NULL)
					offset = (_WORD)(cp - txt);
			}
			cp = txt;
			switch (is_ext_type(&objc_options, type, flags, state))
			{
			case EXTTYPE_EXIT:
			case EXTTYPE_UNDO2:
			case EXTTYPE_DEFAULT:
				od_box(-1, FALSE);
				rc_copy(&od_pxy, &od_framerec);
				if (objc_mode == EXTOB_ORCS && op_draw3d)
				{
					framesize = -1;
					if (flags & OF_EXIT)
						framesize--;
					if (flags & OF_DEFAULT)
						framesize--;
					state = hdraw_button(state, flags, od_pxy.g_x, od_pxy.g_y, od_pxy.g_w, od_pxy.g_h, cp, FALSE, offset, framesize);
					cp = NULL;
				} else
				{
					if (objc_mode == EXTOB_MAGIX)
					{
						offset = (state >> 8) & 0xff;
						if (offset >= 0xfe || offset >= (int)strlen(cp))
							offset = -1;
					}
				}
				break;
			case EXTTYPE_UNDO:
				if (objc_mode == EXTOB_ORCS ||
					objc_mode == EXTOB_MYDIAL)
				{
					od_box(-1, FALSE);
					rc_copy(&od_pxy, &od_framerec);
				} else
				{
					offset = -1;
				}
				break;
			case EXTTYPE_RADIO:
				if (objc_mode == EXTOB_MAGIX)
				{
					offset = (state >> 8) & 0xff;
					if (offset >= 0xfe || offset >= (int)strlen(cp))
						offset = -1;
				}
				state = draw_check_radio(flags, state, x, y, cp, offset, MAC_RADIO);
				cp = NULL;
				break;
			case EXTTYPE_CHECK:
				if (objc_mode == EXTOB_MAGIX)
				{
					offset = (state >> 8) & 0x7f;
					if (offset >= 0x7e || offset >= (int)strlen(cp))
						offset = -1;
				}
				state = draw_check_radio(flags, state, x, y, cp, offset, MAC_CHECK);
				cp = NULL;
				break;
			case EXTTYPE_CHECK_SWITCH:
				if (objc_mode == EXTOB_MAGIX)
				{
					offset = (state >> 8) & 0x7f;
					if (offset >= 0x7e || offset >= (int)strlen(cp))
						offset = -1;
				}
				state = draw_check_radio(flags, state, x, y, cp, offset, MAC_SWITCH);
				cp = NULL;
				break;
			case EXTTYPE_CHECK_BOOLEAN:
				if (objc_mode == EXTOB_MAGIX)
				{
					offset = (state >> 8) & 0x7f;
					if (offset >= 0x7e || offset >= (int)strlen(cp))
						offset = -1;
				}
				state = draw_check_radio(flags, state, x, y, cp, offset, MAC_BOOLEAN);
				cp = NULL;
				break;
			case EXTTYPE_THREESTATE:
				state = draw_threestate(flags, state, x, y, cp, offset);
				cp = NULL;
				break;
			case EXTTYPE_LINE:
				if (objc_mode != EXTOB_MAGIC)
					if (!(flags & (OF_EXIT|OF_TOUCHEXIT)))
						offset = -1;
				state = _draw_under_line(state, x, y, w, h, cp, offset);
				cp = NULL;
				break;
			case EXTTYPE_INNERFRAME:
				state = _draw_inner_frame(state, flags, x, y, cp);
				cp = NULL;
				break;
			case EXTTYPE_NICELINE:
				W_SetBkMode(vdi_handle, MD_REPLACE);
				if (state & OS_WHITEBAK)
				{
					W_Fill_Rect(vdi_handle, &od_framerec, IP_SOLID, W_PAL_WHITE);
				}
				od_pxy.g_y = y + h / 2 - 1;
				od_pxy.g_h = 2;
				if (GetNumColors() >= 16)
					W_Fill_Rect(vdi_handle, &od_pxy, IP_SOLID, color_disabled);
				else
					W_Fill_Rect(vdi_handle, &od_pxy, IP_4PATT, W_PAL_BLACK);
				state = OS_NORMAL;
				cp = NULL;
				break;
			case EXTTYPE_SYSGEM_STRING:
				if (for_menu)
				{
					sysgem_draw_mentry(x, y, w, h, type, obspec, flags, state, txt, offset);
				} else
				{
					sysgem_draw_string(x, y, w, h, type, obspec, flags, state, txt, offset);
				}
				state = OS_NORMAL;
				cp = NULL;
				break;
			default:
				if (objc_mode != EXTOB_MAGIC  && objc_mode != EXTOB_ORCS)
					offset = -1;
				break;
			}
			if (txt != NULL)
			{
				if (cp != NULL)
				{
					draw_text(txt, &od_pxy, IBM, TE_LEFT, TRUE, W_PAL_BLACK, FALSE, offset, bold);
				}
				g_free(txt);
			}
		}
		if (state & OS_SELECTED)
			W_Invert_Rect(vdi_handle, &od_pxy);
		break;
	}

	if (state & OS_CHECKED)
	{
		rc_copy(&od_framerec, &od_pxy);
		od_pxy.g_x += 2;
		draw_char(CHAR_CHECK, &od_pxy, IBM, TE_LEFT, FALSE, state & OS_SELECTED ? W_PAL_WHITE : W_PAL_BLACK, 0);
		od_pxy.g_x -= 2;
	}

	if (state & OS_CROSSED)
		cross(&od_framerec);

	if (state & OS_DISABLED)
		W_Disable_Rect(vdi_handle, &od_framerec);

	if (state & OS_SHADOWED)
		shadow(framesize, framecol);

#if CUR_WIDTH != 0
	if (ted_check != FALSE)
	{
		if (objc_mode == EXTOB_ORCS)
		{
			if (ted_extended(obspec.tedinfo) &&
				(int)strLen(obspec.tedinfo->te_ptext) > obspec.tedinfo->te_displen)
			{
				_WORD wchar, hchar;

				GetTextSize(&wchar, &hchar);
				show_cursor(x+(obspec.tedinfo->te_displen-CUR_WIDTH)*wchar, y, cur_right);
				show_cursor(x, y, cur_left);
			}
		}
	}
#endif
}

/*** ---------------------------------------------------------------------- ***/

void od_open(_WORD _vdi_handle, GRECT *clip)
{
	vdi_handle = _vdi_handle;
	/*
	 * can't use HideMouse here, because this function may be called
	 * from USERDEFs, which are called from AES
	 */
	v_hide_c(vdi_handle);
	W_Clip_Rect(vdi_handle, TRUE, clip);
	od_parm.pb_xc = clip->g_x;
	od_parm.pb_yc = clip->g_y;
	od_parm.pb_wc = clip->g_w;
	od_parm.pb_hc = clip->g_h;
	rc_copy(&old_clip, clip);
	return old;
}


void od_close(void)
{
	v_show_c(vdi_handle, FALSE);
}


_BOOL objc_draw_init(void)
{
	sysgem_init();
	return TRUE;
}


void objc_draw_exit(void)
{
	sysgem_exit();
}

/* ------------------------------------------------------------------------- */

void ob_draw_dialog(_WORD _vdi_handle, OBJECT *tree, _WORD start, _WORD depth, GRECT *gr, EXTOB_MODE mode)
{
	od_open(_vdi_handle, gr);
	objc_mode = mode;
	tree = tree;
	start = start;
	depth = depth;
	od_close();
}
