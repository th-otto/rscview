#include "config.h"
#include <gem.h>
#include "portvdi.h"
#include "or_draw.h"
#include "w_draw.h"

_WORD od_handle;
EXTOB_MODE objc_mode;
_WORD color_background = W_PAL_LGRAY;
_WORD color_disabled = W_PAL_DGRAY;
_WORD color_indicator = W_PAL_LGRAY;
_WORD color_activator = W_PAL_LGRAY;
_WORD move_indicator = FALSE;
_WORD move_activator = FALSE;
_WORD change_indicator = TRUE;
_WORD change_activator = FALSE;
_BOOL op_draw3d = FALSE;
_BOOL od_clear;
_UWORD od_oldstate;
PARMBLKFUNC od_code;
PARMBLK od_parm;

#define CUR_WIDTH 2
#define MAX_FRAMESIZE 4
#define te_displen te_junk1
#define te_offset  te_junk2
#define ted_extended(ted) ((ted)->te_displen != 0)

#define MAC_RADIO 0
#define MAC_CHECK 1
#define MAC_SWITCH 2
#define MAC_BOOLEAN 3
#define MAC_H_RADIO 4
#define MAC_H_CHECK 5
#define MAC_H_LIST 6
#define NUM_MAC_IMAGES 7
#define MAC_THREESTATE 100

#define CHAR_CHECK       0x08
#define CHAR_LISTBOX     0xf0

#define KEY_CODEMASK       0xff00
#define KEY_SHIFTMASK      0xe000
#define OF_MOVEABLE        0x0800


_WORD od_cw, od_ch;
GRECT od_pxy;
GRECT od_framerec;
GRECT od_outrec;

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

_UBYTE *calc_text(const char *ptmplt, _WORD tmplen, _UBYTE *ptext, _WORD txtlen, const char *pvalid, _BOOL reset, _UBYTE fillchar, _WORD max_cur_pos)
{
	_BOOL stop = FALSE, curstop = FALSE;
	_WORD tmpi, txti, vali;
	_UBYTE *cp, *textedbuf;
	_WORD cpos;
	_UBYTE *templatep;
	
	txti = 0;
	vali = 0;
	if ((textedbuf = g_new(_UBYTE, tmplen)) == NULL)
		return NULL;
	/*
	 * FIXME UTF8: '_' in template represents a char, not a byte;
	 * result string may be longer than template
	 */
	templatep = g_strdup(ptmplt);
	tmplen = tmplen - 1;
	if (templatep == NULL)
	{
		g_free(textedbuf);
		return NULL;
	}
	cpos = 0;
	cp = textedbuf;
	for (tmpi = 0; tmpi < tmplen; tmpi++)
	{
		if (templatep[tmpi] == '_')
		{
			if (txti < (txtlen - 1) &&
				!stop && (ptext[txti] != '@' || txti > 0 || pvalid[vali] == '@') &&
				ptext[txti] != '\0')
			{
				if (pvalid[vali] == '*')
				{
					*cp++ = '*';
				} else if (ptext[txti] == '@' && pvalid[vali] != '@' && (ptext[txti + 1] == '@' || ptext[txti + 1] == '\0'))
				{
					*cp = fillchar;
					if (fillchar != '\0')
						cp++;
					stop = TRUE;
					curstop = TRUE;
				} else
				{
					*cp++ = ptext[txti];
				}
				txti++;
				vali++;
				/*
				 * point pvalid back to last char if
				 * it is shorter than the number
				 * of input positions.
				 * This is not an error but a not
				 * so well documented behaviour of input fields
				 */
				if (pvalid[vali] == '\0')
					--vali;
				if (reset && !curstop)
				{
					if (tmpi >= max_cur_pos)
					{
						curstop = TRUE;
					} else
					{
						cpos++;
					}
				}
			} else
			{
				*cp = fillchar;
				if (fillchar != '\0')
					cp++;
				if (reset)
				{
					ptext[txti++] = '\0';
					if (txti < txtlen)
						ptext[txti] = '\0';
				} else
				{
					txti++;
				}
				stop = TRUE;
				curstop = TRUE;
			}
		} else
		{
			*cp++ = templatep[tmpi];
			if (!curstop)
				cpos++;
		}
	}
	if (reset)
	{
	}
	*cp = '\0';
	g_free(templatep);
	return textedbuf;
}

/*** ---------------------------------------------------------------------- ***/

static char *DialogTitelTxt(const char *free_string, _WORD state, _BOOL intern)
{
	const char *txtStr, *start;
	char *retV;
	size_t  len;
	
	if (free_string == NULL || ((state & OS_DRAW3D) && intern))
		return NULL;

	txtStr = free_string;
	start = strrchr(txtStr, '$');
	if (start != NULL)
	{
		if (intern)
		{
			txtStr = start + 1;
			len = strlen(txtStr);
		} else
		{
			len = (size_t) (start - txtStr);
		}
	} else
	{
		len = strlen(txtStr);
	}
	retV = g_new(_UBYTE, len + 1);
	if (retV != NULL)
	{
		memcpy(retV, txtStr, len);
		retV[len] = '\0';
	}
	return retV;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

void od_setfont(_WORD handle, _WORD size, _WORD color, _UWORD style)
{
	_WORD dummy;
	
	vst_height(handle, (size & TE_FONT_MASK) == SMALL ? 4 : 13, &dummy, &dummy, &od_cw, &od_ch);
	vst_color(handle, color);
	vst_effects(handle, style);
}

static char *char2text(_UBYTE c)
{
	static char buf[2];

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
		W_Rectangle(od_handle, &od_pxy);
	}
}


static void outline(_UWORD state)
{
	if (state & OS_OUTLINED)
	{
		vswr_mode(od_handle, MD_REPLACE);
		vsl_color(od_handle, W_PAL_WHITE);
		od_box(-1, TRUE);
		od_box(-1, TRUE);
		vsl_color(od_handle, W_PAL_BLACK);
		od_box(-1, TRUE);
	}
}


static void cross(GRECT *gr)
{
	W_POINTS xy[2];

	vsl_color(od_handle, W_PAL_WHITE);
	vswr_mode(od_handle, MD_TRANS);
	xy[0].xx = gr->g_x;
	xy[0].yy = gr->g_y;
	xy[1].xx = gr->g_x + gr->g_w - 1;
	xy[1].yy = gr->g_y + gr->g_h - 1;
	W_Lines(od_handle, xy, 2);
	xy[0].xx = xy[1].xx;
	xy[1].xx = gr->g_x;
	W_Lines(od_handle, xy, 2);
	vswr_mode(od_handle, MD_REPLACE);
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
		vswr_mode(od_handle, MD_REPLACE);
		vsl_color(od_handle, framecol);
		framesize += framesize;
		while (framesize > 0)
		{
			W_Lines(od_handle, pxy, 3);
			pxy[0].yy++;
			pxy[1].xx++;
			pxy[1].yy++;
			pxy[2].xx++;
			framesize--;
		}
	}
}


static void _draw_box(
	_WORD framesize,
	_WORD framecol,
	_BOOL filled,
	_WORD fillpattern,
	_WORD fillcol
)
{
	vswr_mode(od_handle, MD_REPLACE);
	if (filled)
	{
		W_Fill_Rect(od_handle, &od_pxy, fillpattern, fillcol, FALSE);
	}
	if (framesize > 0)
	{
		_WORD i;

		vsl_color(od_handle, framecol);
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

		vsl_color(od_handle, framecol);
		if (framesize < -(MAX_FRAMESIZE))
			framesize = -(MAX_FRAMESIZE);
		i = framesize;
		do {
			od_box(-1, TRUE);
		} while (++i != 0);
		od_box(-framesize, FALSE);
	}
}


static void draw_dialog_frame(_WORD pattern, _WORD color)
{
	W_POINTS pxy[3];
	
	vswr_mode(od_handle, MD_REPLACE);
	W_Fill_Rect(od_handle, &od_pxy, pattern, color, FALSE);
	od_box(3, FALSE);
	rc_copy(&od_pxy, &od_framerec);
	od_box(-1, FALSE);
	vsl_color(od_handle, W_PAL_BLACK);
	W_Rectangle(od_handle, &od_pxy);
	
	od_box(-1, FALSE);
	pxy[0].xx = od_pxy.g_x;
	pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[1].xx = od_pxy.g_x;
	pxy[1].yy = od_pxy.g_y;
	pxy[2].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[2].yy = od_pxy.g_y;
	vsl_color(od_handle, W_PAL_DGRAY);
	W_Lines(od_handle, pxy, 3);
	pxy[0].xx = od_pxy.g_x + 1;
	pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[1].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[1].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[2].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[2].yy = od_pxy.g_y + 1;
	vsl_color(od_handle, W_PAL_WHITE);
	W_Lines(od_handle, pxy, 3);
	
	vsl_color(od_handle, W_PAL_LGRAY);
	od_box(-1, TRUE);
	od_box(-1, TRUE);

	od_box(-1, FALSE);
	pxy[0].xx = od_pxy.g_x;
	pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[1].xx = od_pxy.g_x;
	pxy[1].yy = od_pxy.g_y;
	pxy[2].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[2].yy = od_pxy.g_y;
	vsl_color(od_handle, W_PAL_WHITE);
	W_Lines(od_handle, pxy, 3);
	pxy[0].xx = od_pxy.g_x + 1;
	pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[1].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[1].yy = od_pxy.g_y + od_pxy.g_h - 1;
	pxy[2].xx = od_pxy.g_x + od_pxy.g_w - 1;
	pxy[2].yy = od_pxy.g_y + 1;
	vsl_color(od_handle, W_PAL_DGRAY);
	W_Lines(od_handle, pxy, 3);

	vsl_color(od_handle, W_PAL_BLACK);
	od_box(-1, TRUE);
}


void draw_underline_char(_WORD x, _WORD y, _WORD color, const char *txt, _WORD offset)
{
	W_POINTS pxy[2];
	_WORD x_offset;
	
	UNUSED(txt);
	x_offset = offset * od_cw;
	pxy[0].xx = x + x_offset;
	pxy[0].yy = y + od_ch - 1;
	pxy[1].xx = pxy[0].xx + od_cw - 1;
	pxy[1].yy = pxy[0].yy;
	vswr_mode(od_handle, MD_REPLACE);
	vsl_color(od_handle, color);
	W_Lines(od_handle, pxy, 2);
}


static _WORD wclip_center(_WORD just)
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


static _BOOL text_rect(char *txt, GRECT *gr, _WORD size, _WORD centered, _BOOL centver, _WORD textcol, _WORD offset, _WORD bold, GRECT *text)
{
	_WORD x, y, w, h;

	if (offset != -1 && objc_mode != EXTOB_MAGIX && txt[offset] == '[')
	{
		memmove(&txt[offset], &txt[offset + 1], strlen(&txt[offset + 1]) + 1);
	}
	od_setfont(od_handle, size, textcol, bold);
	x = gr->g_x;
	y = gr->g_y;
	centered &= TE_JUST_MASK;
	w = W_TextWidth(od_handle, txt);
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


static void _draw_text(char *txt, GRECT *gr, _WORD centered, _BOOL centver, _WORD textcol, _WORD textmode, _WORD offset, GRECT *clip)
{
	if (offset == -1)
	{
		vswr_mode(od_handle, textmode ? textmode : MD_TRANS);
		W_Text(od_handle, clip->g_x, clip->g_y, txt);
	} else
	{
		if (textmode)
		{
			vswr_mode(od_handle, textmode);
			W_ClipText(od_handle, gr, txt, wclip_center(centered), centver ? 0 : -1);
		} else
		{
			vswr_mode(od_handle, MD_TRANS);
			W_Text(od_handle, clip->g_x, clip->g_y, txt);
		}
		draw_underline_char(clip->g_x, clip->g_y, textcol, txt, offset);
	}
	vswr_mode(od_handle, MD_REPLACE);
}


void draw_text(char *txt, GRECT *gr, _WORD size, _WORD centered, _BOOL centver, _WORD textcol, _WORD textmode, _WORD offset, _WORD bold)
{
	GRECT clip;

	text_rect(txt, gr, size, centered, centver, textcol, offset, bold, &clip);
	_draw_text(txt, gr, centered, centver, textcol, textmode, offset, &clip);
}


static void draw_char(_UBYTE c, GRECT *gr, _WORD size, _WORD centered, _BOOL centver, _WORD textcol, _WORD textmode)
{
	_WORD x, y, w, h;
	char buf[2];
	
	od_setfont(od_handle, size, textcol, TXT_NORMAL);
	x = gr->g_x;
	y = gr->g_y;
	h = gr->g_h;
	w = od_cw;
	if (centered == TE_CNTR)
	{
		x += (gr->g_w - w) / 2;
	} else if (centered == TE_RIGHT)
	{
		x += gr->g_w - w;
	}
	if (centver != FALSE)
	{
		h = od_ch;
		y += (gr->g_h - h) / 2;
	}
	vswr_mode(od_handle, textmode ? MD_REPLACE : MD_TRANS);
	buf[0] = c;
	buf[1] = '\0';
	v_gtextn(od_handle, x, y, buf, 1);
	vswr_mode(od_handle, MD_REPLACE);
}


static void draw_ted(TEDINFO *ted, _BOOL formatted, _BOOL box, _WORD bold, _UBYTE fillchar, _UWORD flags, _UWORD state, _WORD *framesize, _WORD *framecol)
{
	char *cp, *txt;
	_UWORD color;
	_WORD offset = -1;
	_WORD centered = ted->te_just;
	_WORD textmode;

	color = ted->te_color;
	if (formatted)
	{
		cp = txt = calc_text(ted->te_ptmplt, ted->te_tmplen, ted->te_ptext, ted->te_txtlen, ted->te_pvalid, FALSE, fillchar, 0);
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
					if ((int)strlen(ted->te_ptext) > ted->te_displen)
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
		char *p;

		cp = txt = g_strdup(ted->te_ptext);
		if (cp != NULL)
		{
			switch (is_ext_type(objc_mode, G_TEXT, OF_NONE, OS_NORMAL))
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
		_draw_box(*framesize, *framecol, TRUE, pattern, fcolor);
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
					vswr_mode(od_handle, MD_REPLACE);
					W_Fill_Rect(od_handle, &clip, pattern, fcolor, FALSE);
					_draw_text(cp, &od_pxy, centered, TRUE, textcol, textmode, offset, &clip);
					g_free(txt);
					cp = NULL;
				} else
				{
					_draw_box(0, W_PAL_WHITE, TRUE, pattern, fcolor);
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
		W_Invert_Rect(od_handle, &od_framerec);
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


static _UWORD _draw_mac(_WORD flags, _WORD state, _WORD x, _WORD y, _WORD type, _WORD *w, _BOOL *draw_3d)
{
	GRECT gr;
	_WORD color, pattern;
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
	
	fprintf(stderr, "draw_mac: %d %d %d %d %04x\n", type, color, pattern, *draw_3d, state);
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
				W_Draw_Icon(od_handle, x, y, (*w + 15) & 0xfff0, h, data, mask, COL_SET, COL_RESET, FALSE);
		} else
#endif
		{
#if 0
			W_Fill_Rect(od_handle, &gr, pattern, color, FALSE);
#endif
			if (*draw_3d && 0)
			{
			} else
			{
				vsl_color(od_handle, pencolor);
				W_Ellipse(od_handle, &gr, TRUE, IP_SOLID, W_PAL_WHITE, TRUE);
				W_Ellipse(od_handle, &gr, FALSE, IP_SOLID, pencolor, TRUE);
				if (state & OS_SELECTED)
				{
					gr.g_x += 3;
					gr.g_y += 3;
					gr.g_w -= 6;
					gr.g_h -= 6;
					vsl_color(od_handle, W_PAL_WHITE);
					W_Ellipse(od_handle, &gr, TRUE, IP_SOLID, W_PAL_WHITE, FALSE);
					vsl_color(od_handle, W_PAL_BLACK);
				}
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
			vsl_color(od_handle, pencolor);
			W_Fill_Rect(od_handle, &gr, IP_SOLID, W_PAL_WHITE, FALSE);

			vsl_color(od_handle, W_PAL_WHITE);
			pxy[0].xx = gr.g_x + 1;
			pxy[0].yy = gr.g_y + gr.g_h - 1;
			pxy[1].xx = gr.g_x + gr.g_w - 1;
			pxy[1].yy = pxy[0].yy;
			pxy[2].xx = pxy[1].xx;
			pxy[2].yy = gr.g_y + 1;
			W_Lines(od_handle, pxy, 3);

			if (GetNumColors() > 2)
			{
				vsl_color(od_handle, W_PAL_LGRAY);
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + gr.g_h - 2;
				pxy[1].xx = gr.g_x + gr.g_w - 2;
				pxy[1].yy = pxy[0].yy;
				pxy[2].xx = pxy[1].xx;
				pxy[2].yy = gr.g_y + 2;
				W_Lines(od_handle, pxy, 3);
			}

			vsl_color(od_handle, pencolor);
			pxy[0].xx = gr.g_x;
			pxy[0].yy = gr.g_y + gr.g_h - 1;
			pxy[1].xx = pxy[0].xx;
			pxy[1].yy = gr.g_y;
			pxy[2].xx = gr.g_x + gr.g_w - 1;
			pxy[2].yy = pxy[1].yy;
			W_Lines(od_handle, pxy, 3);
			pxy[0].xx = gr.g_x + 1;
			pxy[0].yy = gr.g_y + gr.g_h - 2;
			pxy[1].xx = pxy[0].xx;
			pxy[1].yy = gr.g_y + 1;
			pxy[2].xx = gr.g_x + gr.g_w - 2;
			pxy[2].yy = pxy[1].yy;
			W_Lines(od_handle, pxy, 3);

			if (state & OS_SELECTED)
			{
				pxy[0].xx = gr.g_x + 3;
				pxy[0].yy = gr.g_y + 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + gr.g_h - 3;
				W_Lines(od_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 4;
				pxy[0].yy = gr.g_y + 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + gr.g_h - 4;
				W_Lines(od_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + 3;
				pxy[1].xx = gr.g_x + gr.g_w - 4;
				pxy[1].yy = gr.g_y + gr.g_h - 3;
				W_Lines(od_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 3;
				pxy[0].yy = gr.g_y + gr.g_h - 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + 3;
				W_Lines(od_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 4;
				pxy[0].yy = gr.g_y + gr.g_h - 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + 4;
				W_Lines(od_handle, pxy, 2);
				pxy[0].xx = gr.g_x + 3;
				pxy[0].yy = gr.g_y + gr.g_h - 4;
				pxy[1].xx = gr.g_x + gr.g_w - 4;
				pxy[1].yy = gr.g_y + 3;
				W_Lines(od_handle, pxy, 2);
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
			W_Fill_Rect(od_handle, &gr, pattern, color, FALSE);
			vsl_color(od_handle, W_PAL_BLACK);
			W_Fill_Rect(od_handle, &gr, IP_SOLID, W_PAL_WHITE, FALSE);
			W_Rectangle(od_handle, &gr);
			if (state & OS_SELECTED)
			{
				W_POINTS pxy[2];

				pxy[0].xx = gr.g_x;
				pxy[0].yy = gr.g_y;
				pxy[1].xx = gr.g_x + gr.g_w - 1;
				pxy[1].yy = gr.g_y + gr.g_h - 1;
				W_Lines(od_handle, pxy, 2);
				pxy[0].xx = gr.g_x;
				pxy[0].yy = gr.g_y + gr.g_h - 1;
				pxy[1].xx = gr.g_x + gr.g_w - 1;
				pxy[1].yy = gr.g_y;
				W_Lines(od_handle, pxy, 2);
			}
		}
		break;

	case MAC_H_CHECK:
		if (h >= 16)
		{
			gr.g_y += 1;
			gr.g_w -= 1;
			gr.g_h -= 1;
		}
		W_Fill_Rect(od_handle, &gr, pattern, color, FALSE);
		if (0 && *w == 16 && h == 16 && get_macimage(type, state, w, &h, &mask, &data))
		{
			W_Draw_Image(od_handle, x, y, (*w + 15) & 0xfff0, h, data, COL_SET, W_PAL_WHITE, MD_TRANS);
		} else if (*draw_3d && 0)
		{
		} else
		{
			vsl_color(od_handle, pencolor);
			W_Rectangle(od_handle, &gr);
			if (state & OS_SELECTED)
			{
				W_POINTS pxy[2];

				if (h >= 16)
				{
					vsl_width(od_handle, 2);
				}
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + 2;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + gr.g_h - 3;
				W_Lines(od_handle, pxy, 2);
				if (h >= 16)
					W_Pixel(od_handle, pxy + 1, 1, pencolor);
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + gr.g_h - 3;
				pxy[1].xx = gr.g_x + gr.g_w - 3;
				pxy[1].yy = gr.g_y + 2;
				W_Lines(od_handle, pxy, 2);
				if (h >= 16)
					W_Pixel(od_handle, pxy + 1, 1, pencolor);
				vsl_width(od_handle, 1);
			}
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
			vsl_color(od_handle, pencolor);
			W_Fill_Rect(od_handle, &gr, IP_SOLID, W_PAL_WHITE, FALSE);

			vsl_color(od_handle, W_PAL_WHITE);
			pxy[0].xx = gr.g_x + 1;
			pxy[0].yy = gr.g_y + gr.g_h - 1;
			pxy[1].xx = gr.g_x + gr.g_w - 1;
			pxy[1].yy = pxy[0].yy;
			pxy[2].xx = pxy[1].xx;
			pxy[2].yy = gr.g_y + 1;
			W_Lines(od_handle, pxy, 3);

			if (GetNumColors() > 2)
			{
				vsl_color(od_handle, W_PAL_LGRAY);
				pxy[0].xx = gr.g_x + 2;
				pxy[0].yy = gr.g_y + gr.g_h - 2;
				pxy[1].xx = gr.g_x + gr.g_w - 2;
				pxy[1].yy = pxy[0].yy;
				pxy[2].xx = pxy[1].xx;
				pxy[2].yy = gr.g_y + 2;
				W_Lines(od_handle, pxy, 3);
			}

			vsl_color(od_handle, pencolor);
			pxy[0].xx = gr.g_x;
			pxy[0].yy = gr.g_y + gr.g_h - 1;
			pxy[1].xx = pxy[0].xx;
			pxy[1].yy = gr.g_y;
			pxy[2].xx = gr.g_x + gr.g_w - 1;
			pxy[2].yy = pxy[1].yy;
			W_Lines(od_handle, pxy, 3);
			pxy[0].xx = gr.g_x + 1;
			pxy[0].yy = gr.g_y + gr.g_h - 2;
			pxy[1].xx = pxy[0].xx;
			pxy[1].yy = gr.g_y + 1;
			pxy[2].xx = gr.g_x + gr.g_w - 2;
			pxy[2].yy = pxy[1].yy;
			W_Lines(od_handle, pxy, 3);
			
			if (get_macimage(MAC_BOOLEAN, OS_NORMAL, w, &h, &mask, &data))
			{
				data = (_UBYTE *)threestate_3d;
				W_Draw_Icon(od_handle, gr.g_x + 2, gr.g_y + 2, gr.g_w - 5, gr.g_h - 5, data, mask, COL_SET, COL_RESET, FALSE);
			}
		} else
		{
			if (get_macimage(MAC_CHECK, OS_NORMAL, w, &h, &mask, &data))
			{
				data = (_UBYTE *)(*w == h ? big_threestate : small_threestate);
				W_Draw_Icon(od_handle, x, y, (*w + 15) & 0xfff0, h, data, mask, COL_SET, COL_RESET, FALSE);
			}
		}
		break;
	
	default:
		if (get_macimage(type, state, w, &h, &mask, &data))
			W_Draw_Icon(od_handle, x, y, (*w + 15) & 0xfff0, h, data, mask, COL_SET, COL_RESET, FALSE);
		break;
	}
	if ((state & OS_OUTLINED) && objc_mode != EXTOB_MAGIX)
	{
		GRECT xgr;

		xgr.g_x = x - FRAMESIZE;
		xgr.g_y = y - FRAMESIZE;
		xgr.g_w = *w + 2 * FRAMESIZE;
		xgr.g_h = h + 2 * FRAMESIZE;
		vsl_color(od_handle, pencolor);
		W_Rectangle(od_handle, &xgr);
	}
	return state & ~(OS_OUTLINED|OS_SELECTED|OS_CHECKED);
}


static _WORD _draw_inner_frame(_UWORD state, _UWORD flags, _WORD x, _WORD y, const char *str)
{
	_WORD color, pattern;
	_WORD mode;
	_WORD w, h;
	W_POINTS pxy[6];
	_BOOL draw_3d;

	od_setfont(od_handle, state & OS_CHECKED ? SMALL : IBM, W_PAL_BLACK, 0);
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
	vswr_mode(od_handle, MD_REPLACE);

	W_Fill_Rect(od_handle, &od_pxy, pattern, color, FALSE);
	vsl_color(od_handle, W_PAL_BLACK);
	if (str != NULL /* && *str != '\0' */)
	{
		w = (_WORD)strlen(str) * od_cw;
		h = od_ch;
		x += od_cw;
		pxy[0].xx = x;			pxy[0].yy = od_pxy.g_y;
		pxy[1].xx = od_pxy.g_x; pxy[1].yy = pxy[0].yy;
		pxy[2].xx = pxy[1].xx;	pxy[2].yy = pxy[0].yy + od_pxy.g_h - 1;
		pxy[3].xx = pxy[1].xx + od_pxy.g_w - 1; pxy[3].yy = pxy[2].yy;
		pxy[4].xx = pxy[3].xx;	pxy[4].yy = pxy[0].yy;
		pxy[5].xx = x + w;		pxy[5].yy = pxy[0].yy;
		W_Lines(od_handle, pxy, 6);
		if (draw_3d)
		{
			vsl_color(od_handle, W_PAL_WHITE);
			pxy[0].xx = od_pxy.g_x + 1;
			pxy[0].yy = od_pxy.g_y + od_pxy.g_h - 1;
			pxy[1].xx = od_pxy.g_x + od_pxy.g_w - 1;
			pxy[1].yy = pxy[0].yy;
			pxy[2].xx = pxy[1].xx;
			pxy[2].yy = od_pxy.g_y + 1;
			W_Lines(od_handle, pxy, 3);
		}
		vswr_mode(od_handle, mode);
		W_Text(od_handle, x, y, str);
		xywh2rect(x, y, w, h, &od_pxy);
		rc_copy(&od_pxy, &od_framerec);
		if (state & OS_CROSSED)
		{
			_draw_box(1, W_PAL_BLACK, FALSE, IP_HOLLOW, W_PAL_WHITE);
		}
		if (od_clear)
		{
			if (((state & OS_SELECTED) && !(od_oldstate & OS_SELECTED)) ||
				(!(state & OS_SELECTED) && (od_oldstate & OS_SELECTED)) )
			{
				W_Invert_Rect(od_handle, &od_pxy);
			}
		} else
		{
			if (state & OS_SELECTED)
				W_Invert_Rect(od_handle, &od_pxy);
		}
		vswr_mode(od_handle, MD_REPLACE);
	} else
	{
		od_box(0, TRUE);
	}
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
	W_Fill_Rect(od_handle, &gr, IP_SOLID, color, FALSE);
}

/* -------------------------------------------------------------------------- */

static void Line(_WORD x0, _WORD y0, _WORD x1, _WORD y1, _WORD color)
{
	W_POINTS p[2];
	
	vsl_color(od_handle, color);

	p[0].xx = x0;
	p[0].yy = y0;
	p[1].xx = x1;
	p[1].yy = y1;
	W_Lines(od_handle, p, 2);
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

	len = W_TextWidth(od_handle, text);
	x = x2 - x1;
	x /= 2;
	x += x1;
	x = x - (len / 2);
	x++;
	return x;
}

/* -------------------------------------------------------------------------- */

static void _draw_outer_frame(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, const char *text, _UWORD flags, _UWORD state)
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

	vswr_mode(od_handle, MD_REPLACE);
	if (state & OS_WHITEBAK)				/* Whiteback gesetzt? */
	{
		FilledRect(x0, y0, x1, y1, op_draw3d ? color_background : W_PAL_WHITE);
	}

	draw_box5(x0, y0, x1, y1);

	if (empty(text))
		return;

	od_setfont(od_handle, (state & OS_CHECKED) ? SMALL : IBM, W_PAL_BLACK, TXT_NORMAL);

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
	len = W_TextWidth(od_handle, text);
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
	vswr_mode(od_handle, MD_TRANS);
	W_Text(od_handle, x, y, text);
	vswr_mode(od_handle, MD_REPLACE);
	od_setfont(od_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
}


static _WORD _draw_under_line(_UWORD state, _WORD x, _WORD y, _WORD w, _WORD h, char *str, _WORD offset)
{
	W_POINTS pxy[2];
	_WORD bold;
	_WORD size;
	
	if ((state & OS_WHITEBAK) && objc_mode != EXTOB_MAGIX)
		bold = TXT_THICKENED;
	else
		bold = TXT_NORMAL;
	size = IBM;
	if (objc_mode == EXTOB_MAGIC || objc_mode == EXTOB_ORCS)
	{
		if (state & OS_CHECKED)
			size = SMALL;
		if (state & OS_WHITEBAK)
			bold |= TXT_THICKENED;
		if (state & OS_DRAW3D)
			bold |= TXT_SHADOWED;
	}
	od_setfont(od_handle, size, W_PAL_BLACK, bold);
	y += (h - od_ch) / 2;
	pxy[0].xx = x;
	pxy[0].yy = y + od_ch;
	pxy[1].xx = x + w - 1;
	pxy[1].yy = pxy[0].yy;
	if (str != NULL && *str != '\0')
	{
#if 0
		vswr_mode(od_handle, MD_REPLACE);
		W_Text(od_handle, x, y, str);
#else
		GRECT gr;

		gr.g_x = x;
		gr.g_y = y;
		gr.g_w = w;
		gr.g_h = h;
		if (bold & TXT_SHADOWED)
		{
			bold &= ~TXT_SHADOWED;
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
			vsl_color(od_handle, W_PAL_BLACK);
			W_Lines(od_handle, pxy, 2);
			if (state & OS_OUTLINED)
			{
				pxy[0].yy += 2;
				pxy[1].yy += 2;
				W_Lines(od_handle, pxy, 2);
			}
		}
	} else
	{
		outline(state);
		if (bold == TXT_NORMAL && offset == -1)
		{
			vsl_color(od_handle, W_PAL_BLACK);
			W_Lines(od_handle, pxy, 2);
		}
	}
	return state & ~(OS_SELECTED|OS_SHADOWED|OS_OUTLINED|OS_WHITEBAK|OS_CHECKED|OS_DRAW3D);
}


static _UWORD _draw_circle(_UWORD state, _WORD x, _WORD y, _WORD w, _WORD h)
{
	void *data;

	xywh2rect(x, y, w, h, &od_pxy);
	rc_copy(&od_pxy, &od_framerec);
	rc_copy(&od_pxy, &od_outrec);
	_draw_box(-1, W_PAL_BLACK, TRUE, IP_HOLLOW, W_PAL_WHITE);
	if (state & OS_SHADOWED)
		shadow(-1, W_PAL_BLACK);

	od_setfont(od_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
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
	W_Draw_Image(od_handle, x, y, w, h, data, COL_SET, W_PAL_WHITE, MD_TRANS);
	return state & ~(OS_SELECTED|OS_OUTLINED|OS_CROSSED|OS_CHECKED|OS_SHADOWED);
}


static void _draw_image(BITBLK *bi, _WORD x, _WORD y, _BOOL selected)
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
			W_Invert_Rect(od_handle, &od_framerec);
/*
	if (dst.fd_nplanes == 1)
	{
		vro_cpyfm(od_handle, selected ? NOT_SORD : S_OR_D, _pxy, &src, &dst);
	} else
*/
	{
		void *pdata = bi->bi_pdata;

		if (selected)
			W_Draw_Image(od_handle, x, y, w, h, pdata, W_PAL_WHITE, bi->bi_color, MD_ERASE);
		else
			W_Draw_Image(od_handle, x, y, w, h, pdata, bi->bi_color, W_PAL_WHITE, MD_TRANS);
	}
}


static void _draw_icon(ICONBLK *ib, _WORD x, _WORD y, _BOOL selected)
{
	_UBYTE c;
	_UWORD icolor;
	char *txt;
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
	vswr_mode(od_handle, MD_REPLACE);
	pdata = ib->ib_pdata;
	pmask = ib->ib_pmask;
	W_Draw_Icon(od_handle, x, y, w, h, pdata, pmask, datacol, maskcol, selected);
	if ((txt = ib->ib_ptext) != NULL && ib->ib_wtext > 0)
	{
		_WORD offset;
		char *str = txt;

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
			W_Fill_Rect(od_handle, &od_pxy, IP_SOLID, datacol, FALSE);
			draw_text(str, &od_pxy, SMALL, TE_CNTR, TRUE, maskcol, 0, offset, TXT_NORMAL);
		} else
		{
			W_Fill_Rect(od_handle, &od_pxy, IP_SOLID, maskcol, FALSE);
			draw_text(str, &od_pxy, SMALL, TE_CNTR, TRUE, datacol, 0, offset, TXT_NORMAL);
		}
		if (offset != -1)
			g_free(str);
	}
	c = ICOLSPEC_GET_CHARACTER(icolor);
	if (c != 0)
	{
		GRECT gr;

		xywh2rect(x + ib->ib_xchar, y + ib->ib_ychar, 6, 8, &gr);
		draw_text(char2text(c), &gr, SMALL, TE_LEFT, FALSE, selected ? maskcol : datacol, 0, -1, TXT_NORMAL);
	}
}

/*** ---------------------------------------------------------------------- ***/

static void _draw_cicon(CICONBLK *cib, _WORD x, _WORD y, _WORD state)
{
	_UBYTE c;
	char *txt;
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
		W_Draw_Cicon(od_handle, dx, dy, w, h, cicon, datacol, maskcol, state);
	}

	if (cicon == NULL)
	{
		void *pdata = cib->monoblk.ib_pdata;
		void *pmask = cib->monoblk.ib_pmask;
		
		W_Draw_Icon(od_handle, dx, dy, w, h, pdata, pmask, datacol, maskcol, (state & OS_SELECTED) != 0);
	}
	if ((txt = cib->monoblk.ib_ptext) != NULL && txt[0] != '\0')
	{
		_WORD offset;

		vswr_mode(od_handle, MD_TRANS);
		od_pxy.g_x = x + cib->monoblk.ib_xtext;
		od_pxy.g_y = y + cib->monoblk.ib_ytext;
		od_pxy.g_w = cib->monoblk.ib_wtext;
		od_pxy.g_h = cib->monoblk.ib_htext;
		offset = -1;
		if (state & OS_SELECTED)
		{
			W_Fill_Rect(od_handle, &od_pxy, IP_SOLID, datacol, FALSE);
			draw_text(cib->monoblk.ib_ptext, &od_pxy, SMALL, TE_CNTR, TRUE, maskcol, MD_TRANS, offset, TXT_NORMAL);
		} else
		{
			W_Fill_Rect(od_handle, &od_pxy, IP_SOLID, maskcol, FALSE);
			draw_text(cib->monoblk.ib_ptext, &od_pxy, SMALL, TE_CNTR, TRUE, datacol, MD_TRANS, offset, TXT_NORMAL);
		}
		vswr_mode(od_handle, MD_REPLACE);
	}
	c = ICOLSPEC_GET_CHARACTER(icolor);
	if (c != 0)
	{
		GRECT gr;

		xywh2rect(x + cib->monoblk.ib_xicon + cib->monoblk.ib_xchar, y + cib->monoblk.ib_yicon + cib->monoblk.ib_ychar, 6, 8, &gr);
		draw_text(char2text(c), &gr, SMALL, TE_LEFT, FALSE, state & OS_SELECTED ? maskcol : datacol, 0, -1, TXT_NORMAL);
	}
}

/*** ---------------------------------------------------------------------- ***/

static _UWORD draw_check_radio(_WORD flags, _WORD state, _WORD x, _WORD y, char *cp, _WORD offset, _WORD type)
{
	_WORD w;
	_BOOL draw_3d;

	od_setfont(od_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
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
			draw_text(cp, &od_pxy, IBM, TE_LEFT, TRUE, state & OS_DISABLED ? color_disabled : W_PAL_BLACK, MD_TRANS, offset, TXT_NORMAL);
		} else
		{
			draw_text(cp, &od_pxy, IBM, TE_LEFT, TRUE, W_PAL_BLACK, MD_TRANS, offset, TXT_NORMAL);
			if (state & OS_DISABLED)
			{
				od_pxy.g_x = x;
				od_pxy.g_w = w;
				W_Disable_Rect(od_handle, &od_pxy);
			}
		}
	} else
	{
		draw_text(cp, &od_pxy, IBM, TE_LEFT, TRUE, W_PAL_BLACK, MD_TRANS, offset, TXT_NORMAL);
		if (state & OS_DISABLED)
		{
			W_Disable_Rect(od_handle, &od_pxy);
		}
	}
	return state & ~OS_DISABLED;
}

/*** ---------------------------------------------------------------------- ***/

static _UWORD draw_threestate(_WORD flags, _WORD state, _WORD x, _WORD y, char *cp, _WORD offset)
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

static _WORD _CDECL draw_userdef(PARMBLK *pb)
{
	GRECT gr;

	xywh2rect(pb->pb_xc, pb->pb_yc, pb->pb_wc, pb->pb_hc, &gr);
	/* draw an IBOX type frame */
	xywh2rect(pb->pb_x, pb->pb_y, pb->pb_w, pb->pb_h, &od_pxy);
	vswr_mode(od_handle, MD_REPLACE);
	W_Clear_Rect(od_handle, &od_pxy);
	od_box(1, FALSE);
	rc_copy(&od_pxy, &od_framerec);
	vsl_color(od_handle, W_PAL_BLACK);
	od_box(-1, TRUE);
	rc_copy(&od_pxy, &od_outrec);
	outline(pb->pb_currstate);
	if (od_clear)
	{
		if (((pb->pb_currstate & OS_SELECTED) && !(pb->pb_prevstate & OS_SELECTED)) ||
			(!(pb->pb_currstate & OS_SELECTED) && (pb->pb_prevstate & OS_SELECTED)) )
		{
			W_Invert_Rect(od_handle, &od_pxy);
		}
	} else
	{
		if (pb->pb_currstate & OS_SELECTED)
			W_Invert_Rect(od_handle, &od_pxy);
	}
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
	char *cp;
	_BOOL ted_check;
	_WORD bold;
	_WORD centered, centver;
	_WORD textmode;

	framesize = 0;
	framecol = W_PAL_BLACK;
	ted_check = FALSE;
	bold = TXT_NORMAL;
	xywh2rect(x, y, w, h, &od_pxy);
	rc_copy(&od_pxy, &od_framerec);
	rc_copy(&od_pxy, &od_outrec);
	if (flags & OF_INDIRECT)
		obspec = *(obspec.indirect);
	vswr_mode(od_handle, MD_REPLACE);

	switch (type & OBTYPEMASK)
	{
	case G_BOX:
	case G_BOXCHAR:
	case G_EXTBOX:
		switch (is_ext_type(objc_mode, type, flags, state))
		{
		case EXTTYPE_CIRCLE:
			state = _draw_circle(state, x, y, w, h);
			break;
#if 0
		case EXTTYPE_SLIDE:
			{
				MY_SLIDE slide;
				MY_SLIDE *slider;
			
				if ((type & OBTYPEMASK) == G_BOX)
				{
					_LONG val, count;
					
					memset(&slide, 0, sizeof(slide));
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
#endif
		case EXTTYPE_FLYMOVER:
			{
				W_POINTS pxy[2];

				pxy[0].xx = od_pxy.g_x;
				pxy[0].yy = od_pxy.g_y;
				pxy[1].xx = pxy[0].xx + od_pxy.g_w - 1;
				pxy[1].yy = pxy[0].yy + od_pxy.g_h - 1;
				vsl_color(od_handle, W_PAL_BLACK);
				vswr_mode(od_handle, MD_REPLACE);
				W_Lines(od_handle, pxy, 2);
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
					if (GetNumColors() >= 16)
					{
						if (type == (G_BOXCHAR | (G_OBJX_SHORTCUT << 8)))
						{
							char empty = '\0';

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
							char empty = '\0';
							
							hdraw_button(state, flags, x, y, w, h, &empty, FALSE, -1, framesize);
							filled = FALSE;
							framesize = 0;
						}
						if (pattern == IP_SOLID)
							state &= ~OS_SELECTED;
					}
				}
				_draw_box(framesize, OBSPEC_GET_FRAMECOL(obspec), filled, pattern, color);
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
				W_Invert_Rect(od_handle, &od_framerec);
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
			_draw_box(framesize = OBSPEC_GET_FRAMESIZE(obspec),
				OBSPEC_GET_FRAMECOL(obspec),
				FALSE, IP_HOLLOW, W_PAL_WHITE);
			if (framesize < 0)
			{
				od_box(0, TRUE);
			}
			outline(state);
			if (od_clear)
			{
				if ( ((state & OS_SELECTED) && !(od_oldstate & OS_SELECTED)) ||
					 (!(state & OS_SELECTED) && (od_oldstate & OS_SELECTED)) )
				{
					W_Invert_Rect(od_handle, &od_framerec);
				}
			} else
			{
				if (state & OS_SELECTED)
					W_Invert_Rect(od_handle, &od_framerec);
			}
		}
		switch (is_ext_type(objc_mode, type, flags, state))
		{
		case EXTTYPE_FLYMOVER:
			{
				W_POINTS pxy[2];

				pxy[0].xx = od_pxy.g_x;
				pxy[0].yy = od_pxy.g_y;
				pxy[1].xx = pxy[0].xx + od_pxy.g_w - 1;
				pxy[1].yy = pxy[0].yy + od_pxy.g_h - 1;
				vsl_color(od_handle, W_PAL_BLACK);
				vswr_mode(od_handle, MD_REPLACE);
				W_Lines(od_handle, pxy, 2);
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
		switch (is_ext_type(objc_mode, type, flags, state))
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

				_draw_box(framesize = ted->te_thickness, COLSPEC_GET_FRAMECOL(color),
					TRUE, COLSPEC_GET_FILLPATTERN(color), COLSPEC_GET_INTERIORCOL(color));
				outline(state);
			}
			break;
		case EXTTYPE_TITLE:
			if (ted_check)
			{
				TEDINFO *ted = obspec.tedinfo;

				cp = calc_text(ted->te_ptmplt, ted->te_tmplen, ted->te_ptext, ted->te_txtlen, ted->te_pvalid, FALSE, '\0', 0);
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
				TXT_NORMAL, ' ', flags, state, &framesize, &framecol);
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

				ted = *obspec.tedinfo;
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

#if 0
	case G_MTEXT:
		if (mtext_draw != FUNK_NULL)
		{
			MTEDINFO *mted = (MTEDINFO *)obspec.tedinfo;

			framesize = (*mtext_draw)(od_handle, mted, state & OS_SELECTED ? TRUE : FALSE);
			outline(state);
			(*mtext_show_cursor)(mted, x, y, show_cursor, cur_left, cur_right);
		}
		break;
#endif

	case G_IMAGE:
		_draw_image(obspec.bitblk, x, y, state & OS_SELECTED ? TRUE : FALSE);
		outline(state);
		break;

	case G_ICON:
		_draw_icon(obspec.iconblk, x, y, state & OS_SELECTED ? TRUE : FALSE);
		break;

	case G_CICON:
		_draw_cicon(obspec.ciconblk, x, y, state);
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
			char *txt = g_strdup(obspec.free_string);
			_WORD wr_mode = MD_REPLACE;

			if (objc_mode != EXTOB_MAGIX)
			{
				cp = strchr(txt, '[');
				if (cp != NULL)
					offset = (_WORD)(cp - txt);
			}
			cp = txt;

			switch (is_ext_type(objc_mode, type, flags, state))
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
#if 0
				{
					MY_SLIDE slider;

					memset(&slider, 0, sizeof(slider));
					state = hdraw_slider(x, y, w, h, &slider, state);
				}
#endif
				cp = NULL;
				break;
			
			case EXTTYPE_LIST_BOX:
				_draw_box(framesize, W_PAL_BLACK, TRUE, IP_HOLLOW, W_PAL_WHITE);
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
				_draw_box(framesize, W_PAL_BLACK, TRUE, IP_HOLLOW, W_PAL_WHITE);
			}
			outline(state);
			if (cp != NULL)
			{
				draw_text(cp, &od_pxy, IBM, centered, centver, W_PAL_BLACK, wr_mode, offset, bold);
			}
			if (state & OS_SELECTED)
				W_Invert_Rect(od_handle, &od_framerec);
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
				W_Fill_Rect(od_handle, &od_pxy, pattern, color, FALSE);
				if (pattern == IP_SOLID)
					state &= ~OS_SELECTED;
			}
		}
		{
			char *txt;

			offset = -1;
			txt = g_strdup(obspec.free_string);
			if (objc_mode != EXTOB_MAGIX && txt != NULL &&
				((type & OBEXTTYPEMASK) != 0))
			{
				if ((cp = strchr(txt, '[')) != NULL)
					offset = (_WORD)(cp - txt);
			}
			cp = txt;
			switch (is_ext_type(objc_mode, type, flags, state))
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
				vswr_mode(od_handle, MD_REPLACE);
				if (state & OS_WHITEBAK)
				{
					W_Fill_Rect(od_handle, &od_framerec, IP_SOLID, W_PAL_WHITE, FALSE);
				}
				od_pxy.g_y = y + h / 2 - 1;
				od_pxy.g_h = 2;
				if (GetNumColors() >= 16)
					W_Fill_Rect(od_handle, &od_pxy, IP_SOLID, color_disabled, FALSE);
				else
					W_Fill_Rect(od_handle, &od_pxy, IP_4PATT, W_PAL_BLACK, FALSE);
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
				if (objc_mode != EXTOB_MAGIC && objc_mode != EXTOB_ORCS)
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
			W_Invert_Rect(od_handle, &od_pxy);
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
		W_Disable_Rect(od_handle, &od_framerec);

	if (state & OS_SHADOWED)
		shadow(framesize, framecol);

#if CUR_WIDTH != 0
	if (ted_check != FALSE)
	{
		if (objc_mode == EXTOB_ORCS)
		{
			if (ted_extended(obspec.tedinfo) &&
				(int)strlen(obspec.tedinfo->te_ptext) > obspec.tedinfo->te_displen)
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

/*****************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/*****************************************************************************/

_WORD hdraw_button(_WORD state, _WORD flags, _WORD x, _WORD y, _WORD w, _WORD h, char *txtStr, _BOOL extBut, _WORD offset, _WORD framesize)
{
	GRECT work;
	_WORD color, pattern;
	_BOOL draw_3d;

	xywh2rect(x, y, w, h, &work);

	color = W_PAL_WHITE;
	pattern = IP_HOLLOW;
	color_3d(flags, state, &color, &pattern);
	draw_3d = GetNumColors() >= 16 && op_draw3d;
	if (!draw_3d)
	{
		W_Clear_Rect(od_handle, &work);
	} else if (objc_mode == EXTOB_HONKA || extBut)
	{
		GRECT use;
		W_POINTS points[3];
		
		vsl_color(od_handle, W_PAL_BLACK);

		use.g_x = x - 2;
		use.g_y = y - 3;
		use.g_w = w + 4;
		use.g_h = h + 6;
		if (flags & OF_DEFAULT)
		{
			W_Rectangle(od_handle, &use);
			use.g_x -= 1;
			use.g_y -= 1;
			use.g_w += 2;
			use.g_h += 2;
		}
		points[0].xx = use.g_x + 1;
		points[0].yy = use.g_y;
		points[1].xx = use.g_x + use.g_w - 2;
		points[1].yy = points[0].yy;
		W_Lines(od_handle, points, 2);

		points[0].xx = use.g_x + use.g_w - 1;
		points[0].yy = use.g_y + 1;
		points[1].xx = points[0].xx;
		points[1].yy = use.g_y + use.g_h - 2;
		W_Lines(od_handle, points, 2);

		points[0].xx = use.g_x + 1;
		points[0].yy = use.g_y + use.g_h - 1;
		points[1].xx = use.g_x + use.g_w - 2;
		points[1].yy = points[0].yy;
		W_Lines(od_handle, points, 2);

		points[0].xx = use.g_x;
		points[0].yy = use.g_y + 1;
		points[1].xx = points[0].xx;
		points[1].yy = use.g_y + use.g_h - 2;
		W_Lines(od_handle, points, 2);

		if (state & OS_SELECTED)
		{
			use.g_x = x - 1;
			use.g_y = y - 2;
			use.g_w = w + 2;
			use.g_h = h + 4;
			W_Fill_Rect(od_handle, &use, IP_SOLID, W_PAL_DGRAY, FALSE);
		} else
		{
			use.g_x = x - 1;
			use.g_y = y - 2;
			use.g_w = w + 2;
			use.g_h = h + 4;

			vsl_color(od_handle, W_PAL_WHITE);
			points[0].xx = use.g_x;
			points[0].yy = use.g_y + use.g_h - 1;
			points[1].xx = points[0].xx;
			points[1].yy = use.g_y;
			points[2].xx = use.g_x + use.g_w - 1;
			points[2].yy = points[1].yy;
			W_Lines(od_handle, points, 3);

			points[0].xx++;
			points[0].yy--;
			points[1].xx++;
			points[1].yy++;
			points[2].xx--;
			points[2].yy++;
			W_Lines(od_handle, points, 3);

			vsl_color(od_handle, W_PAL_DGRAY);
			points[0].xx = use.g_x;
			points[0].yy = use.g_y + use.g_h - 1;
			points[1].xx = use.g_x + use.g_w - 1;
			points[1].yy = points[0].yy;
			points[2].xx = points[1].xx;
			points[2].yy = use.g_y;
			W_Lines(od_handle, points, 3);

			points[0].xx++;
			points[0].yy--;
			points[1].xx--;
			points[1].yy--;
			points[2].xx--;
			points[2].yy++;
			W_Lines(od_handle, points, 3);

			use.g_x = x + 1;
			use.g_y = y;
			use.g_w = w - 2;
			use.g_h = h;
			W_Fill_Rect(od_handle, &use, IP_SOLID, W_PAL_LGRAY, FALSE);
		}
	} else
	{
		W_POINTS points[3];

		if (offset >= 0)
		{
			x -= 1;
			y -= 1;
			w += 2;
			h += 2;
			xywh2rect(x, y, w, h, &work);
		}
		W_Fill_Rect(od_handle, &work, pattern, color, FALSE);

		work.g_x -= 2;
		work.g_y -= 2;
		work.g_w += 4;
		work.g_h += 4;

		if (state & OS_SELECTED)
			vsl_color(od_handle, W_PAL_WHITE);
		else
			vsl_color(od_handle, W_PAL_DGRAY);
		points[0].xx = x;
		points[0].yy = y + h;
		points[1].xx = x + w;
		points[1].yy = points[0].yy;
		points[2].xx = points[1].xx;
		points[2].yy = y;
		W_Lines(od_handle, points, 3);
		if (framesize < -2)
		{
			points[0].xx--;
			points[0].yy++;
			points[1].xx++;
			points[1].yy++;
			points[2].xx++;
			points[2].yy--;
			W_Lines(od_handle, points, 3);
			work.g_x -= 1;
			work.g_y -= 1;
			work.g_w += 2;
			work.g_h += 2;
		}

		if (state & OS_SELECTED)
			vsl_color(od_handle, W_PAL_DGRAY);
		else
			vsl_color(od_handle, W_PAL_WHITE);
		points[0].xx = x - 1;
		points[0].yy = y + h - 1;
		points[1].xx = points[0].xx;
		points[1].yy = y - 1;
		points[2].xx = x + w - 1;
		points[2].yy = points[1].yy;
		W_Lines(od_handle, points, 3);
		if (framesize < -2)
		{
			points[0].xx--;
			points[0].yy++;
			points[1].xx--;
			points[1].yy--;
			points[2].xx++;
			points[2].yy--;
			W_Lines(od_handle, points, 3);
		}

		if (framesize < -1)
		{
			vsl_color(od_handle, W_PAL_BLACK);
			W_Rectangle(od_handle, &work);
		}
	}

	{
		_UBYTE *taste;
		_WORD xpos, ypos;
		_WORD style;
		_WORD cw, ch;

		style = TXT_NORMAL;
		color = W_PAL_BLACK;
		if (state & OS_DISABLED)
		{
			if (GetNumColors() >= 16)
			{
				color = color_disabled;
			} else
			{
				style = TXT_LIGHT;
				color = W_PAL_WHITE;
			}
		}
		od_setfont(od_handle, IBM, color, style);

		taste = txtStr;
		if (offset >= 0)
		{
			taste += offset;
			if (extBut)
				*taste++ = '\0';
			else if (*taste == '[')
				memmove(taste, taste + 1, strlen(taste + 1) + 1);
		}

		cw = W_TextWidth(od_handle, txtStr);
		ch = od_ch;
		xpos = x + (w - 1 - cw) / 2;
		ypos = y + (h - ch) / 2;
		if (state & OS_SELECTED)
		{
			xpos++;
			ypos++;
		}

		vswr_mode(od_handle, MD_TRANS);
		W_Text(od_handle, xpos, ypos, txtStr);

		if (extBut && offset > 0)
		{
			od_setfont(od_handle, SMALL, color, style);
			cw = W_TextWidth(od_handle, txtStr);
			ch = od_ch;
			xpos = x + w - 1 - cw - 2;
			ypos = y + (h - ch) / 2 + 1;
			if (state & OS_SELECTED)
			{
				xpos++;
				ypos++;
			}
			W_Text(od_handle, xpos, ypos, taste);
		}
		od_setfont(od_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
		vswr_mode(od_handle, MD_REPLACE);
		if (!extBut && offset >= 0)
		{
			if ((state & OS_DISABLED) && GetNumColors() >= 16)
				color = color_disabled;
			else
				color = W_PAL_BLACK;
			draw_underline_char(xpos, ypos, color, txtStr, offset);
		}
	}

	if (draw_3d)
	{
		state &= ~(OS_SELECTED|OS_DISABLED);
	} else
	{
		vsl_color(od_handle, W_PAL_BLACK);
		od_box(-1, TRUE);
		od_box(-1, TRUE);
		if (flags & OF_DEFAULT)
			od_box(-1, TRUE);
	}

	state &= ~(OS_OUTLINED | OS_SHADOWED);

	return state;
}

/*** ---------------------------------------------------------------------- ***/

static _UBYTE RB0[] = {
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x03, 0x00, 0xC0, 0x00, 0x05, 0x00,
0xCF, 0xFF, 0xFB, 0x00, 0xC8, 0x00, 0x0D, 0x00, 0xC8, 0x00, 0x0B, 0x00, 0xC8, 0x1E, 0x0D, 0x00,
0xC8, 0x3F, 0x0B, 0x00, 0xC8, 0x33, 0x0D, 0x00, 0xC8, 0x33, 0x0B, 0x00, 0xC8, 0x06, 0x0D, 0x00,
0xC8, 0x06, 0x0B, 0x00, 0xC8, 0x0C, 0x0D, 0x00, 0xC8, 0x0C, 0x0B, 0x00, 0xC8, 0x00, 0x0D, 0x00,
0xC8, 0x0C, 0x0B, 0x00, 0xC8, 0x0C, 0x0D, 0x00, 0xC8, 0x00, 0x0B, 0x00, 0xC8, 0x00, 0x0D, 0x00,
0xCF, 0xFF, 0xFB, 0x00, 0xD5, 0x55, 0x55, 0x00, 0xEA, 0xAA, 0xAB, 0x00, 0xC0, 0x00, 0x00, 0x00};
static BITBLK rs_frage = {  NULL,   4,  24,	 0,   0, 0x0001 };

#ifndef OS_WINDOWS
static _UBYTE RB1[] = {
0xFF, 0xE0, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00,
0x00, 0x7F, 0xF8, 0x00, 0x00, 0x20, 0x0C, 0x00, 0x00, 0x10, 0x0E, 0x00, 0x00, 0x08, 0x0F, 0x00,
0x00, 0x04, 0x0F, 0x80, 0x00, 0x02, 0x0F, 0xC0, 0x00, 0x01, 0x0F, 0xE0, 0x00, 0x00, 0x8F, 0xF0,
0x00, 0x00, 0x40, 0x10, 0x00, 0x00, 0x20, 0x10, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x08, 0x10,
0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0x02, 0x10, 0x00, 0x00, 0x01, 0x18, 0x00, 0x00, 0x00, 0x9C,
0x00, 0x00, 0x00, 0x5E, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x01};
static BITBLK rs_beweg = {  NULL,   4,  24,	 0,   0, 0x0001 };
#endif

_WORD hdraw_title(_WORD state, _WORD x, _WORD y, _WORD w, _WORD h, const char *titleStr)
{

	xywh2rect(x, y, w, h+2, &od_pxy);
	_draw_box(2, W_PAL_BLACK, TRUE, IP_SOLID, W_PAL_WHITE);

#ifndef OS_WINDOWS
	if (!(state & OS_WHITEBAK))
	{
		if (rs_beweg.bi_pdata == NULL)
		{
			rs_beweg.bi_pdata = (_WORD *)RB1;
		}
		_draw_image(&rs_beweg, x + w - 32, y, FALSE);
	}
#endif

	if (!(state & OS_DRAW3D))
	{
		if (*titleStr != '\0')
		{
			if (rs_frage.bi_pdata == NULL)
			{
				rs_frage.bi_pdata = (_WORD *)RB0;
			}
			_draw_image(&rs_frage, x, y, FALSE);
		}
	}

	if (*titleStr != '\0')
	{
		_UBYTE	*ttStr;

		if ((ttStr = DialogTitelTxt(titleStr, state, FALSE)) != NULL)
		{
			xywh2rect(x, y, w, h, &od_pxy);
			draw_text(ttStr, &od_pxy, IBM, TE_CNTR, TRUE, W_PAL_BLACK, 0, -1, TXT_NORMAL);
			g_free(ttStr);
		}
	}
	return state & ~(OS_DRAW3D | OS_WHITEBAK | OS_CROSSED | OS_OUTLINED);
}

/*** ---------------------------------------------------------------------- ***/

static _WORD w_text(_WORD handle,
	_WORD xx, _WORD yy,
	_WORD size, const _UBYTE *from,
	_WORD color, _BOOL draw)
{
	_UBYTE *wkCh, *to;
	_WORD pos = -1;

	to = g_strdup(from);
	if (to != NULL)
	{
		wkCh = to;
		while (*wkCh != '\0')
		{
			if (*wkCh == '[')
			{
				*wkCh = '\0';
				if (draw)
					pos = W_TextWidth(handle, to);
				memmove(wkCh, wkCh + 1, strlen(wkCh + 1) + 1);
			} else
			{
				if (*wkCh == '_')
				{
					*wkCh = '\0';
					break;
				}
				wkCh++;
			}
		}

		od_setfont(handle, size, color, TXT_NORMAL);
		if (draw)
		{
			W_Text(handle, xx, yy, to);

			if (pos != -1)
			{
				draw_underline_char(xx, yy, color, to, pos);
			}
		}
		pos = W_TextWidth(handle, to);
		g_free(to);
	}
	return pos;
}

/*** ---------------------------------------------------------------------- ***/

_WORD hdraw_list(_WORD x, _WORD y, _WORD w, _WORD h,
	OBSPEC obspec, _UWORD flags, _UWORD state, GRECT *gr)
{
	_WORD iconDiff;
	_BOOL showIcon = FALSE;
	TEDINFO *ted = obspec.tedinfo;
	_WORD textw;
	_UWORD color;
	_WORD framesize;
	_WORD framecol;
	_BOOL draw_3d;

	if ((flags & OF_EDITABLE) || (flags & OF_SELECTABLE))
		showIcon = TRUE;
	if (state & OS_DISABLED)
		showIcon = FALSE;

	if (showIcon)
		iconDiff = 16;
	else
		iconDiff = 0;

	color = ted->te_color;
	xywh2rect(x, y, w, h, &od_pxy);
	rc_copy(&od_pxy, &od_outrec);
	framecol = COLSPEC_GET_FRAMECOL(color);
	_draw_box(framesize = ted->te_thickness, framecol,
		TRUE, COLSPEC_GET_FILLPATTERN(color), COLSPEC_GET_INTERIORCOL(color));

	if (state & OS_SHADOWED)
		shadow(framesize, framecol);

	textw = w_text(od_handle, x, y, ted->te_font, ted->te_ptmplt, COLSPEC_GET_TEXTCOL(color), TRUE);

	gr->g_x = x + textw,
	gr->g_y = y;
	gr->g_w = w - textw - iconDiff;
	gr->g_h = h;

	state &= ~(OS_SELECTED | OS_SHADOWED | OS_CHECKED);

	if (showIcon)
		state = _draw_mac(flags, state, x + w - iconDiff, y, MAC_H_LIST, &w, &draw_3d);

	return state;
}

/*** ---------------------------------------------------------------------- ***/

void hdraw_popup_size(_WORD handle, _WORD x, _WORD y, _WORD w, _WORD h,
	OBSPEC obspec, _UWORD flags, _UWORD state, GRECT *gr)
{
	_WORD iconDiff;
	TEDINFO *ted = obspec.tedinfo;
	_WORD textw;
	_UWORD color;

	UNUSED(flags);
	iconDiff = 16;
	if (state & OS_CHECKED)
		iconDiff = 0;

	color = ted->te_color;
	textw = w_text(handle, x, y, ted->te_font, ted->te_ptmplt, COLSPEC_GET_TEXTCOL(color), FALSE);

	gr->g_x = x + textw;
	gr->g_y = y;
	gr->g_w = w - textw - iconDiff;
	gr->g_h = h;
}

/*** ---------------------------------------------------------------------- ***/

_WORD hdraw_popup(_WORD type, _WORD x, _WORD y, _WORD w, _WORD h,
	OBSPEC obspec, _UWORD flags, _UWORD state, GRECT *gr)
{
	_WORD iconDiff;
	TEDINFO *ted = obspec.tedinfo;
	_WORD textw;
	_UWORD color;
	_WORD framesize;
	_WORD framecol;

	iconDiff = 16;
	if (state & OS_CHECKED)
		iconDiff = 0;

	color = ted->te_color;

	if (od_clear)
	{
		_WORD fillcol, fillpattern;

		vswr_mode(od_handle, MD_REPLACE);
		fillcol = COLSPEC_GET_INTERIORCOL(color);
		fillpattern = COLSPEC_GET_FILLPATTERN(color);

		if (color_3d(flags, state, &fillcol, &fillpattern) &&
			GetNumColors() >= 16 && (state & OS_SELECTED))
		{
			fillcol = W_PAL_DGRAY;
			fillpattern = IP_SOLID;
			state &= ~OS_SELECTED;
		}
		W_Fill_Rect(od_handle, &od_pxy, fillpattern, fillcol, FALSE);
	}
	vswr_mode(od_handle, COLSPEC_GET_TEXTMODE(color) ? MD_REPLACE : MD_TRANS);

	textw = w_text(od_handle, x, y, ted->te_font, ted->te_ptmplt, COLSPEC_GET_TEXTCOL(color), TRUE);
	if (state & OS_SELECTED)
	{
		xywh2rect(x, y, textw, h, &od_pxy);
		W_Invert_Rect(od_handle, &od_pxy);
	}

	od_pxy.g_x = x + textw;
	od_pxy.g_y = y;
	od_pxy.g_w = w - textw - iconDiff;
	od_pxy.g_h = h;
	if (gr != NULL)
		rc_copy(&od_pxy, gr);

	od_pxy.g_w += iconDiff;
	rc_copy(&od_pxy, &od_outrec);
	framecol = COLSPEC_GET_FRAMECOL(color);
	_draw_box(framesize = ted->te_thickness, framecol,
		TRUE, COLSPEC_GET_FILLPATTERN(color), COLSPEC_GET_INTERIORCOL(color));

	if (state & OS_SHADOWED)
		shadow(framesize, framecol);

	switch (type & OBEXTTYPEMASK)
	{
	case G_OBJX_POPUP_MULTIPLE << 8: /* EXTTYPE_POPUP_MULTIPLE */
	case G_OBJX_POPUP_SINGLE << 8: /* EXTTYPE_POPUP_SINGLE */
		od_pxy.g_w -= iconDiff;
		W_ClipText(od_handle, &od_pxy, ted->te_ptext, wclip_center(ted->te_just), 0);
		break;
	}

	state &= ~(OS_SELECTED | OS_SHADOWED | OS_CHECKED);

	if (iconDiff != 0)
	{
		W_POINTS pxy[2];
		GRECT icon;

		x = x + w - iconDiff;
		pxy[0].xx = x - 1;
		pxy[0].yy = y;
		pxy[1].xx = pxy[0].xx;
		pxy[1].yy = y + h - 1;
		vsl_color(od_handle, framecol);
		W_Lines(od_handle, pxy, 2);
		xywh2rect(x, y, iconDiff, h, &icon);
		switch (type & OBEXTTYPEMASK)
		{
		case G_OBJX_POPUP_SINGLE << 8: /* EXTTYPE_POPUP_SINGLE */
			draw_char(CHAR_LISTBOX, &icon, ted->te_font, TE_CNTR, TRUE, COLSPEC_GET_TEXTCOL(color), COLSPEC_GET_TEXTMODE(color));
			break;
		case G_OBJX_POPUP_SIMPLE << 8: /* EXTTYPE_POPUP_SIMPLE */
			state = _draw_circle(state, x, y, iconDiff, h);
			break;
		case G_OBJX_POPUP_MULTIPLE << 8: /* EXTTYPE_POPUP_MULTIPLE */
			draw_char(CHAR_CHECK, &icon, ted->te_font, TE_CNTR, TRUE, COLSPEC_GET_TEXTCOL(color), COLSPEC_GET_TEXTMODE(color));
			break;
		}
	}
	vswr_mode(od_handle, MD_REPLACE);

	return state;
}

/*****************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/*****************************************************************************/

enum ext_type is_ext_type(EXTOB_MODE mode, _UWORD type, _UWORD flags, _UWORD state)
{
	_UWORD basetype = type & OBTYPEMASK;
	
	switch (mode)
	{
	case EXTOB_MAGIX:
		if (state & OS_WHITEBAK)
		{
			switch (basetype)
			{
			case G_BUTTON:
				if ((state & 0xff00) == 0xfe00)
					return EXTTYPE_INNERFRAME;
				if (flags & OF_RBUTTON)
					return EXTTYPE_RADIO;
				if (state & 0x8000)
					return EXTTYPE_CHECK;
				return EXTTYPE_EXIT;
			case G_STRING:
				if ((state & 0xff00) == 0xff00)
					return EXTTYPE_LINE;
				return EXTTYPE_EXIT;
			case G_SHORTCUT:
				/* if ((state & OS_DISABLED) &&
					tree->ob_spec.free_string[0] == '-')
					return EXTTYPE_NICELINE; */
				break;
			}
		}
		break;
	
	case EXTOB_NONE:
	case EXTOB_AES:
	case EXTOB_OVERLAY:
	case EXTOB_WEGA:
	case EXTOB_HONKA:
	case EXTOB_WIN:
	case EXTOB_UNKNOWN:
		break;
	
	case EXTOB_GEISS1:
	case EXTOB_GEISS2:
	case EXTOB_MYDIAL:
	case EXTOB_FLYDIAL:
	case EXTOB_FACEVALUE:
	case EXTOB_ORCS:
	case EXTOB_MAGIC:
		switch (type & OBEXTTYPEMASK)
		{
		case G_OBJX_MOVER << 8:
			if (flags & OF_TOUCHEXIT)
				if ((state & (OS_OUTLINED|OS_CROSSED)) == (OS_OUTLINED|OS_CROSSED))
					return EXTTYPE_FLYMOVER;
			if (mode == EXTOB_MAGIC && basetype == G_IBOX)
				return EXTTYPE_FLYMOVER;
			break;
		case G_OBJX_SHORTCUT << 8:
			switch (basetype)
			{
			case G_BUTTON:
				if (!(flags & (OF_RBUTTON | OF_EXIT)))
				{
					if (mode == EXTOB_ORCS && (state & OS_DRAW3D))
						return EXTTYPE_CHECK_SWITCH;
					return EXTTYPE_CHECK;
				}
				/* fall through */
			case G_STRING:
				if (flags & OF_RBUTTON)
					return EXTTYPE_RADIO;
				/* fall through */
			case G_SHORTCUT:
			default:
				if (flags & 0x2000)
					return EXTTYPE_UNDO;
				if ((flags & MYDIAL_UNDO_FLAG) && mode == EXTOB_MYDIAL)
					return EXTTYPE_UNDO;
				if (mode == EXTOB_MAGIC)
				{
					if (flags & 0x4000)
						return EXTTYPE_UNDO;
					if (flags & 0x8000)
						return EXTTYPE_HELP;
				}
				if (!(flags & OF_RBUTTON))
					if ((basetype == G_BUTTON && (flags & OF_EXIT)) ||
						(basetype == G_STRING && !(flags & OF_EXIT)))
					return EXTTYPE_EXIT;
				break;
			}
			break;
		case G_OBJX_LINE << 8:
			switch (mode)
			{
			default:
				switch (basetype)
				{
				case G_BUTTON:
				case G_STRING:
					return EXTTYPE_LINE;
				}
				break;
			case EXTOB_MAGIC:
				switch (basetype)
				{
				case G_IMAGE:
				case G_ICON:
				case G_CICON:
					break;
				default:
					if (state & (OS_SHADOWED|OS_OUTLINED|OS_CHECKED|OS_DISABLED|OS_DRAW3D|OS_WHITEBAK))
						return EXTTYPE_LINE;
					return EXTTYPE_EXIT;
				}
				break;
			}
			break;
		case G_OBJX_FRAME << 8:
			switch (basetype)
			{
			case G_BUTTON:
				return EXTTYPE_INNERFRAME;
			case G_BOX:
				return EXTTYPE_OUTERFRAME;
			}
			break;
		case G_OBJX_NICELINE << 8:
		/* case G_OBJX_NOTEBOOK << 8: same as G_OBJX_NICELINE */
			switch (basetype)
			{
			case G_IMAGE:
			case G_ICON:
			case G_CICON:
				break;
			case G_STRING:
			case G_SHORTCUT:
				if (state & OS_DISABLED)
					return EXTTYPE_NICELINE;
				return EXTTYPE_HELP;
			default:
				return EXTTYPE_HELP;
			}
			break;
		case G_OBJX_CIRCLE << 8:
			switch (basetype)
			{
			case G_BOXCHAR:
				return EXTTYPE_CIRCLE;
			}
			break;
		case G_OBJX_THREESTATE << 8:
			switch (basetype)
			{
			case G_BUTTON:
				return EXTTYPE_THREESTATE;
			}
			break;
		case G_OBJX_LONGEDIT << 8:
			switch (basetype)
			{
			case G_FTEXT:
			case G_FBOXTEXT:
				return EXTTYPE_LONGEDIT;
			}
			break;
		case G_OBJX_DEFAULT << 8:
			switch (basetype)
			{
			case G_IMAGE:
			case G_ICON:
			case G_CICON:
				break;
			default:
				if (flags & OF_DEFAULT)
					return EXTTYPE_DEFAULT;
				if ((flags & OF_EXIT) || ((flags & OF_TOUCHEXIT) && mode == EXTOB_MYDIAL))
					return EXTTYPE_UNDO2;
				break;
			}
			break;
		}
		break;
	
	case EXTOB_SYSGEM:
		switch (basetype)
		{
		case G_BUTTON:
			if ((type & OBEXTTYPEMASK) == (1 << 8))
				return EXTTYPE_SYSGEM_HELP;
			if ((type & OBEXTTYPEMASK) == (2 << 8))
				return EXTTYPE_SYSGEM_HELP;
			if ((type & OBEXTTYPEMASK) == (3 << 8))
				return EXTTYPE_SYSGEM_NOTEBOOK;
			if (flags & OF_EXIT)
				return EXTTYPE_SYSGEM_BUTTON;
			if (flags & OF_RBUTTON)
				return EXTTYPE_SYSGEM_RADIO;
			if (flags & OF_SELECTABLE)
				return EXTTYPE_SYSGEM_SELECT;
			if (flags & OF_TOUCHEXIT)
				return EXTTYPE_SYSGEM_TOUCHEXIT;
			return EXTTYPE_SYSGEM_FRAME;
		
		case G_BOX:
			if ((type & OBEXTTYPEMASK) == (50 << 8))
				return EXTTYPE_SYSGEM_BAR1;
			if ((type & OBEXTTYPEMASK) == (51 << 8))
				return EXTTYPE_SYSGEM_BAR2;
			if ((type & OBEXTTYPEMASK) == (52 << 8))
				return EXTTYPE_SYSGEM_BAR3;
			return EXTTYPE_SYSGEM_BOX;
		
		case G_BOXCHAR:
			if ((type & OBEXTTYPEMASK) != (11 << 8))
				return EXTTYPE_SYSGEM_BOXCHAR;
			break;
		
		case G_TEXT:
		case G_BOXTEXT:
			switch (type & OBEXTTYPEMASK)
			{
			case 2 << 8:
				return EXTTYPE_SYSGEM_LISTBOX;
			case 3 << 8:
				return EXTTYPE_SYSGEM_CIRCLE;
			}
			return EXTTYPE_SYSGEM_TEXT;
		case G_FTEXT:
		case G_FBOXTEXT:
			/* if (sysgem.self_edit) */
				return EXTTYPE_SYSGEM_EDIT;
		
		case G_STRING:
		case G_TITLE:
		case G_SHORTCUT:
			return EXTTYPE_SYSGEM_STRING;
		}
		break;
	}

	if (mode == EXTOB_FACEVALUE)
	{
		switch (basetype)
		{
		case G_BOXTEXT:
			switch (type & OBEXTTYPEMASK)
			{
			case 30 << 8:
				if (flags & OF_TOUCHEXIT)
				{
					if (state & 0x0400)
						return EXTTYPE_POPUP_SINGLE;
				}
				break;
			default:
				if (flags & 0x8000)
					return EXTTYPE_HELP;
				break;
			}
			break;
		}
	}
	
	if (mode == EXTOB_ORCS)
	{
		if ((flags & (OF_TOUCHEXIT|OF_MOVEABLE)) == (OF_TOUCHEXIT|OF_MOVEABLE))
			return EXTTYPE_MOVER;
		switch (basetype)
		{
		case G_BUTTON:
			switch (type & OBEXTTYPEMASK)
			{
			case G_OBJX_SHORTCUT << 8:
				if (!(flags & (OF_RBUTTON | OF_EXIT)))
				{
					if (state & OS_DRAW3D)
						return EXTTYPE_CHECK_SWITCH;
					else if (!(state & OS_SHADOWED))
						return EXTTYPE_CHECK;
				}
				if (flags & OF_RBUTTON)
					return EXTTYPE_RADIO;
				if (flags & OF_EXIT)
					return EXTTYPE_EXIT;
				break;
			case G_OBJX_TITLE << 8:
				if (flags & OF_TOUCHEXIT)
					return EXTTYPE_TITLE;
				break;
			case G_OBJX_LIST_BOX << 8:
				return EXTTYPE_LIST_BOX;
			}
			if ((flags & KEY_SHIFTMASK) != 0 && (state & KEY_CODEMASK) != 0)
				return EXTTYPE_EXIT;
			break;

		case G_USERDEF:
			switch (type & OBEXTTYPEMASK)
			{
			case G_OBJX_MY_USERDEF << 8:
				return EXTTYPE_OBJWIND;
			}
			break;

		case G_STRING:
		case G_SHORTCUT:
			break;
			
		case G_TEXT:
		case G_BOXTEXT:
			switch (type & OBEXTTYPEMASK)
			{
			case G_OBJX_LIST << 8:
				return EXTTYPE_LIST;
			case G_OBJX_TITLE << 8:
				return EXTTYPE_TITLE;
			case G_OBJX_LIST_BOX << 8:
				return EXTTYPE_LIST_BOX;
			}
			return EXTTYPE_EXIT;
		
		case G_FTEXT:
		case G_FBOXTEXT:
			switch (type & OBEXTTYPEMASK)
			{
			case G_OBJX_LIST << 8:
				return EXTTYPE_LIST;
			case G_OBJX_TITLE << 8:
				return EXTTYPE_TITLE;
			case G_OBJX_LIST_BOX << 8:
				return EXTTYPE_LIST_BOX;
			case G_OBJX_POPUP_SINGLE << 8:
				return EXTTYPE_POPUP_SINGLE;
			case G_OBJX_POPUP_SIMPLE << 8:
				return EXTTYPE_POPUP_SIMPLE;
			case G_OBJX_POPUP_MULTIPLE << 8:
				return EXTTYPE_POPUP_MULTIPLE;
			}
			return EXTTYPE_EXIT;
		
		case G_BOX:
		case G_BOXCHAR:
		case G_EXTBOX:
			switch (type & OBEXTTYPEMASK)
			{
			case G_OBJX_SLIDE << 8:
				return EXTTYPE_SLIDE;
			default:
				if ((flags & (OF_MOVEABLE|OF_SELECTABLE|OF_EXIT)) == (OF_MOVEABLE|OF_EXIT|OF_SELECTABLE))
					return EXTTYPE_CLOSER;
				break;
			}
			break;
		}
	}
	return EXTTYPE_NONE;
}

/*****************************************************************************/
/*** --------------------------------------------------------------------- ***/
/*****************************************************************************/

static void save_lineattr(_WORD handle, _WORD attrib[6])
{
	vql_attributes(handle, attrib);
}

/*** --------------------------------------------------------------------- ***/

static void restore_lineattr(_WORD handle, _WORD attrib[6])
{
	vsl_type(handle, attrib[0]);
	vsl_color(handle, attrib[1]);
	vswr_mode(handle, attrib[2]);
	vsl_width(handle, attrib[3]);
	vsl_ends(handle, attrib[4], attrib[5]);
}

/*** --------------------------------------------------------------------- ***/

static void save_textattr(_WORD handle, _WORD attrib[10])
{
	vqt_attributes(handle, attrib);
}

/*** --------------------------------------------------------------------- ***/

static void restore_textattr(_WORD handle, _WORD attrib[10])
{
	_WORD dummy;
	
	vst_font(handle, attrib[0]);
	vst_color(handle, attrib[1]);
	vst_rotation(handle, attrib[2]);
	vst_alignment(handle, attrib[3], attrib[4], &dummy, &dummy);
	/*
	 * do not restore writing mode here,
	 * it is off-by-1 in most TOS versions,
	 * but maybe not when using fVDI/NVDI.
	 * restore it using some of the other functions
	 */
	/* vswr_mode(handle, attrib[5]); */
	vst_height(handle, attrib[7], &dummy, &dummy, &dummy, &dummy);
}

/*** --------------------------------------------------------------------- ***/

static void save_fillattr(_WORD handle, _WORD attrib[5])
{
	vqf_attributes(handle, attrib);
}

/*** --------------------------------------------------------------------- ***/

static void restore_fillattr(_WORD handle, _WORD attrib[5])
{
	vsf_interior(handle, attrib[0]);
	vsf_color(handle, attrib[1]);
	vsf_style(handle, attrib[2]);
	vswr_mode(handle, attrib[3]);
	vsf_perimeter(handle, attrib[4]);
}

/*** --------------------------------------------------------------------- ***/

static void save_markattr(_WORD handle, _WORD attrib[5])
{
	vqm_attributes(handle, attrib);
}

/*** --------------------------------------------------------------------- ***/

static void restore_markattr(_WORD handle, _WORD attrib[5])
{
	vsm_type(handle, attrib[0]);
	vsm_color(handle, attrib[1]);
	vswr_mode(handle, attrib[2]);
	vsm_height(handle, attrib[3]);
	/* vsm_width(handle, attrib[4]); */
}

/*** --------------------------------------------------------------------- ***/

static void save_clip(_WORD handle, _WORD clip[5])
{
	_WORD wo[57];
	
	vq_extnd(handle, 1, wo);
	clip[0] = wo[45];
	clip[1] = wo[46];
	clip[2] = wo[47];
	clip[3] = wo[48];
	clip[4] = wo[19];
}

/*** --------------------------------------------------------------------- ***/

static void restore_clip(_WORD handle, _WORD clip[5])
{
	vs_clip(handle, clip[4], clip);
}

/*** --------------------------------------------------------------------- ***/

static void od_open(_WORD vdi_handle, const GRECT *clip)
{
	_WORD dummy;

	od_handle = vdi_handle;
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

	vsf_interior(vdi_handle, FIS_SOLID);
	vsf_style(vdi_handle, IP_SOLID);
	vsf_perimeter(vdi_handle, FALSE);

	vswr_mode(vdi_handle, MD_REPLACE);

	vsl_ends(vdi_handle, 0, 0);
	vsl_type(vdi_handle, LT_SOLID);
	vsl_width(vdi_handle, 1);
	vsl_color(vdi_handle, G_BLACK);

	vst_alignment(vdi_handle, TA_LEFT, TA_TOP, &dummy, &dummy);
	od_setfont(vdi_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
	vswr_mode(vdi_handle, MD_REPLACE);
}

/* ------------------------------------------------------------------------- */

static void od_close(const GRECT *gr)
{
	v_show_c(od_handle, FALSE);
	W_Clip_Rect(od_handle, FALSE, gr);
}

/* ------------------------------------------------------------------------- */

_BOOL objc_draw_init(void)
{
	_WORD dummy;
	
	objc_sysvar(SV_INQUIRE, LK3DIND, FALSE, FALSE, &move_indicator, &change_indicator);
	objc_sysvar(SV_INQUIRE, LK3DACT, TRUE, FALSE, &move_activator, &dummy);
	objc_sysvar(SV_INQUIRE, INDBUTCOL, G_LWHITE, 0, &color_indicator, &dummy);
	objc_sysvar(SV_INQUIRE, ACTBUTCOL, G_LWHITE, 0, &color_activator, &dummy);
	objc_sysvar(SV_INQUIRE, BACKGRCOL, G_LWHITE, 0, &color_background, &dummy);
	op_draw3d = color_indicator != G_WHITE || color_activator != G_WHITE || color_background != G_WHITE;
	sysgem_init();
	return TRUE;
}

/* ------------------------------------------------------------------------- */

void objc_draw_exit(void)
{
	sysgem_exit();
}

/* ------------------------------------------------------------------------- */

void ob_draw_dialog(_WORD vdi_handle, OBJECT *tree, _WORD start, _WORD depth, const GRECT *gr, EXTOB_MODE mode)
{
	_WORD lineattrib[6];
	_WORD textattrib[10];
	_WORD fillattrib[5];
	_WORD markattrib[5];
	_WORD clip[5];
	
	/*
	 * since we are going to use the AES vdi handle,
	 * save the attributes and restore them later
	 */
	save_lineattr(vdi_handle, lineattrib);
	save_textattr(vdi_handle, textattrib);
	save_fillattr(vdi_handle, fillattrib);
	save_markattr(vdi_handle, markattrib);
	save_clip(vdi_handle, clip);

	od_open(vdi_handle, gr);
	objc_mode = mode;
	obj_draw_area(tree, start, depth, FALSE);
	od_close(gr);

	restore_textattr(vdi_handle, textattrib);
	restore_lineattr(vdi_handle, lineattrib);
	restore_fillattr(vdi_handle, fillattrib);
	restore_markattr(vdi_handle, markattrib);
	restore_clip(vdi_handle, clip);
}
