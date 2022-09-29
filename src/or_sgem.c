#include "config.h"
#include <gem.h>
#include "portvdi.h"
#include "or_draw.h"
#include "w_draw.h"

typedef struct {
	_UBYTE *big;
	void *big_p;
	_UBYTE *small;
	void *small_p;
} SYSGEM_IMAGE;

struct _sysgem {
	_WORD shortcut_color;
	_WORD frametext_color;
	_WORD selb_color;
	_WORD selt_color;
	_WORD help_color;
	_BOOL like_mac;
	_BOOL edit_mono;
	_BOOL low_resolution;
	_BOOL bergner;
};
static struct _sysgem sysgem;

typedef _UBYTE BIG_IMAGE[32];
typedef _UBYTE SMALL_IMAGE[16];

static BIG_IMAGE IMG_PFEIL1 = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 
0x0F, 0xF0, 0x0F, 0xF0, 0x07, 0xE0, 0x03, 0xC0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static _UBYTE IMG_PFEIL1_DISABLED[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 
0x05, 0x50, 0x0A, 0xA0, 0x05, 0x40, 0x02, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static BIG_IMAGE IMG_PFEIL4 = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0xE0, 0x1F, 0xF0, 0x1F, 0xF8, 
0x1F, 0xF8, 0x1F, 0xF0, 0x00, 0xE0, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static _UBYTE IMG_PFEIL4_DISABLED[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xA0, 0x15, 0x50, 0x0A, 0xA8, 
0x15, 0x50, 0x0A, 0xA0, 0x00, 0x40, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static BIG_IMAGE IMG_PFEIL5 = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x07, 0x00, 0x0F, 0xF8, 0x1F, 0xF8, 
0x1F, 0xF8, 0x0F, 0xF8, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static _UBYTE IMG_PFEIL5_DISABLED[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x05, 0x50, 0x0A, 0xA8, 
0x15, 0x50, 0x0A, 0xA8, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static BIG_IMAGE IMG_PFEIL6 = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0xC0, 0x07, 0xE0, 0x0F, 0xF0, 0x0F, 0xF0, 
0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static _UBYTE IMG_PFEIL6_DISABLED[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x40, 0x02, 0xA0, 0x05, 0x50, 0x0A, 0xA0, 
0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static BIG_IMAGE IMG_CIRCLE = {
0x00, 0x00, 0x00, 0x00, 0x13, 0xC0, 0x1C, 0x30, 0x1C, 0x08, 0x00, 0x08, 0x20, 0x04, 0x20, 0x04, 
0x20, 0x04, 0x20, 0x04, 0x10, 0x00, 0x10, 0x38, 0x0C, 0x38, 0x03, 0xC8, 0x00, 0x00, 0x00, 0x00};

static _UBYTE IMG_CIRCLE_DISABLED[] = {
0x00, 0x00, 0x00, 0x00, 0x11, 0x40, 0x08, 0x20, 0x14, 0x00, 0x00, 0x08, 0x00, 0x04, 0x20, 0x00, 
0x00, 0x04, 0x20, 0x00, 0x10, 0x00, 0x00, 0x28, 0x04, 0x10, 0x02, 0x88, 0x00, 0x00, 0x00, 0x00};

static BIG_IMAGE IMG_SEL_DESEL_NORM_HIGH = {
0x00, 0x00, 0x7F, 0xFE, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 
0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x7F, 0xFE, 0x00, 0x00};

static BIG_IMAGE IMG_SEL_SEL_NORM_HIGH = {
0x00, 0x00, 0x7F, 0xFE, 0x40, 0x02, 0x58, 0x1A, 0x5C, 0x3A, 0x4E, 0x72, 0x47, 0xE2, 0x43, 0xC2, 
0x43, 0xC2, 0x47, 0xE2, 0x4E, 0x72, 0x5C, 0x3A, 0x58, 0x1A, 0x40, 0x02, 0x7F, 0xFE, 0x00, 0x00};

static BIG_IMAGE IMG_SEL_DESEL_DIS_HIGH = {
0x00, 0x00, 0x55, 0x54, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 
0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x2A, 0xAA, 0x00, 0x00};

static BIG_IMAGE IMG_SEL_SEL_DIS_HIGH = {
0x00, 0x00, 0x55, 0x54, 0x00, 0x02, 0x48, 0x10, 0x14, 0x2A, 0x4A, 0x50, 0x04, 0xA2, 0x41, 0x40, 
0x02, 0x82, 0x45, 0x20, 0x0A, 0x52, 0x54, 0x28, 0x08, 0x12, 0x40, 0x00, 0x2A, 0xAA, 0x00, 0x00};

static BIG_IMAGE IMG_CIR_DESEL_NORM_HIGH = {
0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x0C, 0x60, 0x10, 0x10, 0x20, 0x08, 0x20, 0x08, 0x40, 0x04, 
0x40, 0x04, 0x40, 0x04, 0x20, 0x08, 0x20, 0x08, 0x10, 0x10, 0x0C, 0x60, 0x03, 0x80, 0x00, 0x00};

static BIG_IMAGE IMG_CIR_SEL_NORM_HIGH = {
0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x0C, 0x60, 0x10, 0x10, 0x23, 0x88, 0x27, 0xC8, 0x4F, 0xE4, 
0x4F, 0xE4, 0x4F, 0xE4, 0x27, 0xC8, 0x23, 0x88, 0x10, 0x10, 0x0C, 0x60, 0x03, 0x80, 0x00, 0x00};

static BIG_IMAGE IMG_CIR_DESEL_DIS_HIGH = {
0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x08, 0x20, 0x00, 0x00, 0x20, 0x08, 0x00, 0x00, 0x40, 0x04, 
0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x20, 0x08, 0x00, 0x00, 0x08, 0x20, 0x02, 0x80, 0x00, 0x00};

static BIG_IMAGE IMG_CIR_SEL_DIS_HIGH = {
0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x08, 0x20, 0x00, 0x00, 0x22, 0x88, 0x05, 0x40, 0x4A, 0xA4, 
0x05, 0x40, 0x4A, 0xA4, 0x05, 0x40, 0x22, 0x88, 0x00, 0x00, 0x08, 0x20, 0x02, 0x80, 0x00, 0x00};

static SMALL_IMAGE IMG_SEL_DESEL_NORM_LOW = {
0x3F, 0xFC, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x3F, 0xFC, 0x00, 0x00};

static SMALL_IMAGE IMG_SEL_SEL_NORM_LOW = {
0x3F, 0xFC, 0x38, 0x1C, 0x26, 0x64, 0x21, 0x84, 0x26, 0x64, 0x38, 0x1C, 0x3F, 0xFC, 0x00, 0x00};

static SMALL_IMAGE IMG_SEL_DESEL_DIS_LOW = {
0x2A, 0xA8, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x2A, 0xA8, 0x00, 0x00};

static SMALL_IMAGE IMG_SEL_SEL_DIS_LOW = {
0x2A, 0xA4, 0x10, 0x08, 0x22, 0x44, 0x01, 0x80, 0x22, 0x44, 0x10, 0x00, 0x2A, 0xAC, 0x00, 0x00};

static SMALL_IMAGE IMG_CIR_DESEL_NORM_LOW = {
0x0F, 0xF0, 0x78, 0x1E, 0xE0, 0x07, 0xC0, 0x03, 0xC0, 0x03, 0xE0, 0x07, 0x78, 0x1E, 0x0F, 0xF0};

static SMALL_IMAGE IMG_CIR_SEL_NORM_LOW = {
0x0F, 0xF0, 0x78, 0x1E, 0xE3, 0xC7, 0xCF, 0xF3, 0xCF, 0xF3, 0xE3, 0xC7, 0x78, 0x1E, 0x0F, 0xF0};

static SMALL_IMAGE IMG_CIR_DESEL_DIS_LOW = {
0x0A, 0xA0, 0x50, 0x14, 0xA0, 0x05, 0x40, 0x02, 0xA0, 0x01, 0x40, 0x02, 0x50, 0x0A, 0x05, 0x50};

static SMALL_IMAGE IMG_CIR_SEL_DIS_LOW = {
0x0A, 0xA0, 0x50, 0x14, 0xA2, 0x85, 0x45, 0x42, 0x8A, 0xA1, 0x42, 0x82, 0x50, 0x0A, 0x05, 0x50};

static SYSGEM_IMAGE img_pfeil1 = { IMG_PFEIL1, 0, 0, 0 };
static SYSGEM_IMAGE img_pfeil1_disabled = { IMG_PFEIL1_DISABLED, 0, 0, 0 };
static SYSGEM_IMAGE img_pfeil4 = { IMG_PFEIL4, 0, 0, 0 };
static SYSGEM_IMAGE img_pfeil4_disabled = { IMG_PFEIL4_DISABLED, 0, 0, 0 };
static SYSGEM_IMAGE img_pfeil5 = { IMG_PFEIL5, 0, 0, 0 };
static SYSGEM_IMAGE img_pfeil5_disabled = { IMG_PFEIL5_DISABLED, 0, 0, 0 };
static SYSGEM_IMAGE img_pfeil6 = { IMG_PFEIL6, 0, 0, 0 };
static SYSGEM_IMAGE img_pfeil6_disabled = { IMG_PFEIL6_DISABLED, 0, 0, 0 };
static SYSGEM_IMAGE img_circle = { IMG_CIRCLE, 0, 0, 0 };
static SYSGEM_IMAGE img_circle_disabled = { IMG_CIRCLE_DISABLED, 0, 0, 0 };
static SYSGEM_IMAGE img_sel_desel_norm = { IMG_SEL_DESEL_NORM_HIGH, 0, IMG_SEL_DESEL_NORM_LOW, 0 };
static SYSGEM_IMAGE img_sel_sel_norm = { IMG_SEL_SEL_NORM_HIGH, 0, IMG_SEL_SEL_NORM_LOW, 0 };
static SYSGEM_IMAGE img_sel_desel_dis = { IMG_SEL_DESEL_DIS_HIGH, 0, IMG_SEL_DESEL_DIS_LOW, 0 };
static SYSGEM_IMAGE img_sel_sel_dis = { IMG_SEL_SEL_DIS_HIGH, 0, IMG_SEL_SEL_DIS_LOW, 0 };
static SYSGEM_IMAGE img_cir_desel_norm = { IMG_CIR_DESEL_NORM_HIGH, 0, IMG_CIR_DESEL_NORM_LOW, 0 };
static SYSGEM_IMAGE img_cir_sel_norm = { IMG_CIR_SEL_NORM_HIGH, 0, IMG_CIR_SEL_NORM_LOW, 0 };
static SYSGEM_IMAGE img_cir_desel_dis = { IMG_CIR_DESEL_DIS_HIGH, 0, IMG_CIR_DESEL_DIS_LOW, 0 };
static SYSGEM_IMAGE img_cir_sel_dis = { IMG_CIR_SEL_DIS_HIGH, 0, IMG_CIR_SEL_DIS_LOW, 0 };

/******************************************************************************/
/* -------------------------------------------------------------------------- */
/******************************************************************************/

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

static void rect3d(_WORD x0, _WORD y0, _WORD x1, _WORD y1, _BOOL sel)
{
	_WORD col_do = W_PAL_BLACK;
	_WORD col_du = W_PAL_BLACK;

	if (op_draw3d)
	{
		col_do = W_PAL_WHITE;
		col_du = W_PAL_DGRAY;
		if (sel)
		{
			col_do = W_PAL_DGRAY;
			col_du = W_PAL_WHITE;
		}
	}
	Line(x0, y1, x0, y0, col_do);
	Line(x0, y0, x1, y0, col_do);
	Line(x0, y1, x1, y1, col_du);
	Line(x1, y1, x1, y0, col_du);
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

static void xRect(_WORD x0, _WORD y0, _WORD x1, _WORD y1, _WORD color)
{
	GRECT gr;
	
	gr.g_x = x0;
	gr.g_y = y0;
	gr.g_w = x1 - x0 + 1;
	gr.g_h = y1 - y0 + 1;
	vsl_color(od_handle, color);
	W_Rectangle(od_handle, &gr);
}

/* -------------------------------------------------------------------------- */

static void Frame(_WORD x0, _WORD y0, _WORD x1, _WORD y1, _WORD thick, _WORD color)
{
	_WORD i;

	if (thick != 0)
	{
		i = 0;
		for (;;)
		{
			xRect(x0, y0, x1, y1, color);
			if (thick < 0)
			{
				i--;
				if (i == thick)
					return;
				x0--;
				y0--;
				x1++;
				y1++;
			} else
			{
				i++;
				if (i == thick)
					return;
				x0++;
				y0++;
				x1--;
				y1--;
			}
		}
	}
}

/* ------------------------------------------------------------------- */

static void DrawPattern(_WORD x0, _WORD y0, _WORD x1, _WORD y1, _WORD pattern, _WORD interiorcol)
{
	GRECT gr;

	gr.g_x = x0;
	gr.g_y = y0;
	gr.g_w = x1 - x0 + 1;
	gr.g_h = y1 - y0 + 1;

	W_Fill_Rect(od_handle, &gr, pattern, interiorcol, FALSE);
}

/* ------------------------------------------------------------------- */

static void PaintPattern(_WORD x0, _WORD y0, _WORD x1, _WORD y1, OBSPEC obspec)
{
	_WORD framesize = OBSPEC_GET_FRAMESIZE(obspec);

	if (framesize == 0)
	{
		DrawPattern(x0, y0, x1, y1, OBSPEC_GET_FILLPATTERN(obspec), OBSPEC_GET_INTERIORCOL(obspec));
	} else
	{
		DrawPattern(x0 + 1, y0 + 1, x1 - 1, y1 - 1, OBSPEC_GET_FILLPATTERN(obspec), OBSPEC_GET_INTERIORCOL(obspec));
		Frame(x0, y0, x1, y1, framesize, OBSPEC_GET_FRAMECOL(obspec));
	}
}

/* -------------------------------------------------------------------------- */

static void draw_box0(_WORD x0, _WORD y0, _WORD x1, _WORD y1)
{
	FilledRect(x0, y0, x1, y1, op_draw3d ? color_background : W_PAL_WHITE);
}

/* -------------------------------------------------------------------------- */

static void draw_box1(_WORD x0, _WORD y0, _WORD x1, _WORD y1)
{
	draw_box0(x0, y0, x1, y1);
	Frame(x0 - 1, y0 - 1, x1 + 1, y1 + 1, -1, op_draw3d ? W_PAL_DGRAY : W_PAL_BLACK);
}

/* -------------------------------------------------------------------------- */

static void draw_box4(_WORD x0, _WORD y0, _WORD x1, _WORD y1, _WORD color)
{
	if (op_draw3d)
	{
		x0 -= 3;
		y0 -= 3;
		x1 += 3;
		y1 += 3;

		FilledRect(x0 - 1, y0 - 1, x1 - 1, y1 - 1, W_PAL_WHITE);

		Line(x0, y1, x0, y0, color);
		Line(x0, y0, x1, y0, color);

		x0--;
		y0--;
		x1++;
		y1++;

		Line(x0, y1, x0, y0, color);
		Line(x0, y0, x1, y0, color);
		Line(x0 + 1, y1, x1, y1, W_PAL_LGRAY);
		Line(x1, y1, x1, y0 + 1, W_PAL_LGRAY);
	}
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

static void PaintShortCut(_WORD x, _WORD y, _UWORD state, const _UBYTE *text, _WORD shortcut_pos)
{
	_WORD x0;
	_WORD w;
	char sh[2];
	_WORD col;

	vswr_mode(od_handle, MD_REPLACE);
	col = sysgem.shortcut_color;
	if (shortcut_pos < 0)
		return;
	if (state & OS_DISABLED)
	{
		if (op_draw3d)
			col = W_PAL_LGRAY;
		else
			col = W_PAL_WHITE;
	}

	y--;

	y += od_ch;
	sh[0] = text[shortcut_pos];
	sh[1] = 0;
	x0 = W_NTextWidth(od_handle, text, shortcut_pos);
	w = W_NTextWidth(od_handle, sh, 1);
	x0 = x + x0 - 1;
	Line(x0, y, x0 + w, y, col);
}

/* -------------------------------------------------------------------------- */

static _WORD CenterY(_WORD y1, _WORD y2)
{
	_WORD y;

	y = y2 - y1;
	y /= 2;
	y += y1;
	y = y - (od_ch / 2);
	y++;
	return y;
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

static void CopyToScreen(_BOOL draw_3d, void *img_h, void *img_l, _WORD x, _WORD y)
{
	_WORD fg, bg;
	void *data;
	_WORD h;
	
	if (draw_3d)
	{
		fg = W_PAL_BLACK;
		bg = color_background;
	} else
	{
		fg = W_PAL_BLACK;
		bg = W_PAL_WHITE;
	}
	if (sysgem.low_resolution && img_l != NULL)
	{
		data = img_l;
		h = 8;
	} else
	{
		data = img_h;
		h = 16;
	}
	W_Draw_Image(od_handle, x, y, 16, h, data, fg, bg, MD_REPLACE);
}

/******************************************************************************/
/* -------------------------------------------------------------------------- */
/******************************************************************************/

/*
 * OBJ_BUTTON: G_BUTTON, OF_EXIT
 * OBJ_TOUCH: G_BUTTON, OF_TOUCHEXIT
 */
void sysgem_draw_button(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut_pos)
{
	_WORD x0, y0, x1, y1;
	_WORD fill;
	_WORD txtcolor;
	_WORD effects = TXT_NORMAL;
	
	UNUSED(obspec.index);
	UNUSED(type);
	
	/*
	 * adjust outer dimensions.
	 * In the library, this is done by manipulating the object coordinates
	 * (in NewUserblock)
	 */
	{
		_WORD cw, ch;
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		GetTextSize(&cw, &ch);
		if (sysgem.bergner)
		{
			xdiff = 1;
			ydiff = 2;
			wdiff = 2;
			hdiff = 2;
		} else
		{
			xdiff = 1;
			ydiff = 2 + (ch / 4);
			wdiff = 2 + (cw / 4);
			hdiff = 2 + (ch / 2);
			if ((type & 0xff00) == 0x400)
				xdiff = ydiff = 0;
		}
		if (x != 0)
			x -= xdiff;
		if (y != 0)
			y -= ydiff;
		w += wdiff;
		h += hdiff;
	}
	
	x0 = x;
	y0 = y;
	x1 = x0 + w - 1;
	y1 = y0 + h - 1;
	fill = W_PAL_WHITE;
	txtcolor = W_PAL_BLACK;

	od_setfont(od_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
	if (state & OS_DISABLED)
	{
		x0++;
		y0++;
		x1--;
		y1--;
		x0++;
		y0++;
		x1--;
		y1--;
		effects = TXT_LIGHT;
		if (op_draw3d)
		{
			rect3d(x0, y0, x1, y1, TRUE);
		} else
		{
			Frame(x0, y0, x1, y1, W_PAL_BLACK, -1);
		}
		goto button_text;
	}

	if (state & OS_SELECTED)
	{
		fill = W_PAL_BLACK;
		if (op_draw3d == FALSE)
			txtcolor = W_PAL_WHITE;
	}

	if (flags & OF_DEFAULT)
	{
		if (sysgem.like_mac)
		{
			Line(x0 + 1, y0, x1 - 1, y0, W_PAL_BLACK);
			Line(x1, y0 + 1, x1, y1 - 1, W_PAL_BLACK);
			Line(x0, y0 + 1, x0, y1 - 1, W_PAL_BLACK);
			Line(x0 + 1, y1, x1 - 1, y1, W_PAL_BLACK);
		} else
		{
			Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
		}
		x0++;
		y0++;
		x1--;
		y1--;
		x0++;
		y0++;
		x1--;
		y1--;
		Frame(x0, y0, x1, y1, -2, W_PAL_BLACK);
		x0++;
		y0++;
		x1--;
		y1--;
		FilledRect(x0, y0, x1, y1, op_draw3d ? color_activator : fill);
		if (op_draw3d)
		{
			rect3d(x0, y0, x1, y1, (state & OS_SELECTED) != 0);
		}
		goto button_text;
	}

	if (flags & OF_TOUCHEXIT)
	{
		x0++;
		y0++;
		x1--;
		y1--;
		x0++;
		y0++;
		x1--;
		y1--;
		FilledRect(x0, y0, x1, y1, op_draw3d ? color_background : W_PAL_WHITE);
		draw_box5(x0, y0, x1, y1);
		goto button_text;
	}

	if (flags & OF_EXIT)
	{
		x0++;
		y0++;
		x1--;
		y1--;
		if (sysgem.like_mac)
		{
			Line(x0 + 1, y0, x1 - 1, y0, W_PAL_BLACK);
			Line(x1, y0 + 1, x1, y1 - 1, W_PAL_BLACK);
			Line(x0, y0 + 1, x0, y1 - 1, W_PAL_BLACK);
			Line(x0 + 1, y1, x1 - 1, y1, W_PAL_BLACK);
		} else
		{
			Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
		}
		x0++;
		y0++;
		x1--;
		y1--;
		Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
		x0++;
		y0++;
		x1--;
		y1--;
		FilledRect(x0, y0, x1, y1, op_draw3d ? color_activator : fill);
		if (op_draw3d)
		{
			rect3d(x0, y0, x1, y1, (state & OS_SELECTED) != 0);
		}
	}

  button_text:

	vswr_mode(od_handle, MD_TRANS);
	y = CenterY(y0, y1);
	od_setfont(od_handle, IBM, txtcolor, effects);
	if (shortcut_pos >= 0)
	{
		/* text is already a copy and can be modified */
		memmove(text + shortcut_pos, text + shortcut_pos + 1, strlen(text + shortcut_pos + 1) + 1);
		x = CenterX(x0, x1, text);
		W_Text(od_handle, x, y, text);
	} else
	{
		x = CenterX(x0, x1, text);
		W_Text(od_handle, x, y, text);
	}
	od_setfont(od_handle, IBM, txtcolor, TXT_NORMAL);
	PaintShortCut(x, y, state, text, shortcut_pos);
	vswr_mode(od_handle, MD_REPLACE);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_RADIO: G_BUTTON, OF_RBUTTON
 */
void sysgem_draw_radio(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut_pos)
{
	_WORD x0, y0;
	_UWORD effect;

	UNUSED(flags);
	UNUSED(type);
	UNUSED(obspec.index);
	UNUSED(w);
	
	/*
	 * no adjustments for outer dimensions.
	 */

	x0 = x;
	y0 = y;
	y0 = CenterY(y0, y0 + h - 1);

	effect = TXT_NORMAL;

	if (state & OS_SELECTED)
	{
		if (state & OS_DISABLED)
		{
			CopyToScreen(op_draw3d, img_cir_sel_dis.big_p, img_cir_sel_dis.small_p, x0, y0);
			effect = TXT_LIGHT;
		} else
		{
			CopyToScreen(op_draw3d, img_cir_sel_norm.big_p, img_cir_sel_norm.small_p, x0, y0);
		}
	} else
	{
		if (state & OS_DISABLED)
		{
			CopyToScreen(op_draw3d, img_cir_desel_dis.big_p, img_cir_desel_dis.small_p, x0, y0);
			effect = TXT_LIGHT;
		} else
		{
			CopyToScreen(op_draw3d, img_cir_desel_norm.big_p, img_cir_desel_norm.small_p, x0, y0);
		}
	}
	vswr_mode(od_handle, MD_TRANS);
	od_setfont(od_handle, IBM, W_PAL_BLACK, effect);
	if (shortcut_pos >= 0)
	{
		/* text is already a copy and can be modified */
		memmove(text + shortcut_pos, text + shortcut_pos + 1, strlen(text + shortcut_pos + 1) + 1);
	}
	W_Text(od_handle, x0 + 24, y0, text);
	vst_effects(od_handle, TXT_NORMAL);
	PaintShortCut(x0 + 24, y0, state, text, shortcut_pos);
	vswr_mode(od_handle, MD_REPLACE);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_SELECT: G_BUTTON, OF_SELECTABLE
 */
void sysgem_draw_select(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut_pos)
{
	_WORD x0, y0;
	_UWORD effect;

	UNUSED(flags);
	UNUSED(type);
	UNUSED(obspec.index);
	UNUSED(w);
	
	/*
	 * no adjustments for outer dimensions.
	 */

	x0 = x;
	y0 = y;
	y0 = CenterY(y0, y0 + h - 1);

	effect = TXT_NORMAL;

	if (state & OS_SELECTED)
	{
		if (state & OS_DISABLED)
		{
			CopyToScreen(op_draw3d, img_sel_sel_dis.big_p, img_sel_sel_dis.small_p, x0, y0);
			effect = TXT_LIGHT;
		} else
		{
			CopyToScreen(op_draw3d, img_sel_sel_norm.big_p, img_sel_sel_norm.small_p, x0, y0);
		}
	} else
	{
		if (state & OS_DISABLED)
		{
			CopyToScreen(op_draw3d, img_sel_desel_dis.big_p, img_sel_desel_dis.small_p, x0, y0);
			effect = TXT_LIGHT;
		} else
		{
			CopyToScreen(op_draw3d, img_sel_desel_norm.big_p, img_sel_desel_norm.small_p, x0, y0);
		}
	}
	vswr_mode(od_handle, MD_TRANS);
	od_setfont(od_handle, IBM, W_PAL_BLACK, effect);
	if (shortcut_pos >= 0)
	{
		/* text is already a copy and can be modified */
		memmove(text + shortcut_pos, text + shortcut_pos + 1, strlen(text + shortcut_pos + 1) + 1);
	}
	W_Text(od_handle, x0 + 24, y0, text);
	vst_effects(od_handle, TXT_NORMAL);
	PaintShortCut(x0 + 24, y0, state, text, shortcut_pos);
	vswr_mode(od_handle, MD_REPLACE);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_CHAR: G_BOXCHAR, ext != 11
 */
void sysgem_draw_boxchar(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state)
{
	_WORD x0, y0, x1, y1;
	_WORD txt_col;
	_WORD fill_col;
	char text[2];
	_WORD ext;
	void *image;

	UNUSED(flags);
	
	/*
	 * adjust outer dimensions.
	 * In the library, this is done by manipulating the object coordinates
	 * (in NewUserblock)
	 */
	{
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		xdiff = 0;
		ydiff = 0;
		wdiff = 2;
		hdiff = 2;
		if (x != 0)
			x -= xdiff;
		if (y != 0)
			y -= ydiff;
		w += wdiff;
		h += hdiff;
	}
	
	txt_col = (state & OS_SELECTED) ? W_PAL_WHITE : W_PAL_BLACK;
	fill_col = (state & OS_SELECTED) ? W_PAL_BLACK : W_PAL_WHITE;
	x0 = x;
	y0 = y;
	x1 = x0 + w - 1;
	y1 = y0 + h - 1;
	text[0] = OBSPEC_GET_CHARACTER(obspec);
	text[1] = 0;
	ext = (type & OBEXTTYPEMASK) >> 8;

	if (sysgem.low_resolution)
		ext = 0;

	if (state & OS_SHADOWED)
	{
		x1 -= 2;
		y1 -= 2;
	}
	if (op_draw3d)
	{
		txt_col = W_PAL_BLACK;
		fill_col = W_PAL_LGRAY;
	}
	x = ((x1 - x0) / 2) - 8 + x0 + 1;
	y = ((y1 - y0) / 2) - 8 + y0 + 1;

	FilledRect(x0 + 1, y0 + 1, x1 - 1, y1 - 1, fill_col);

	switch (ext)
	{
	case 1:
		image = state & OS_DISABLED ? img_pfeil1_disabled.big_p : img_pfeil1.big_p;
		CopyToScreen(op_draw3d, image, NULL, x, y);
		goto do_3d;
	case 2:
		text[0] = 240;
		goto normal;
	case 3:
		image = state & OS_DISABLED ? img_pfeil4_disabled.big_p : img_pfeil4.big_p;
		CopyToScreen(op_draw3d, image, NULL, x, y);
		goto do_3d;
	case 4:
		image = state & OS_DISABLED ? img_pfeil5_disabled.big_p : img_pfeil5.big_p;
		CopyToScreen(op_draw3d, image, NULL, x, y);
		goto do_3d;
	case 5:
		image = state & OS_DISABLED ? img_pfeil6_disabled.big_p : img_pfeil6.big_p;
		CopyToScreen(op_draw3d, image, NULL, x, y);
		goto do_3d;
	case 6:
		image = state & OS_DISABLED ? img_circle_disabled.big_p : img_circle.big_p;
		CopyToScreen(op_draw3d, image, NULL, x, y);
		goto do_3d;
	case 7:
		image = state & OS_DISABLED ? img_pfeil1_disabled.big_p : img_pfeil1.big_p;
		CopyToScreen(op_draw3d, image, NULL, x, y);
		goto frame;
	case 8:
		image = state & OS_DISABLED ? img_pfeil4_disabled.big_p : img_pfeil4.big_p;
		CopyToScreen(op_draw3d, image, NULL, x, y);
		goto frame;
	case 9:
		image = state & OS_DISABLED ? img_pfeil5_disabled.big_p : img_pfeil5.big_p;
		CopyToScreen(op_draw3d, image, NULL, x, y);
		goto frame;
	case 10:
		image = state & OS_DISABLED ? img_pfeil6_disabled.big_p : img_pfeil6.big_p;
		CopyToScreen(op_draw3d, image, NULL, x, y);
		goto frame;
	case 12:
	default:
	  normal:
		x = CenterX(x0, x1, text);
		y = CenterY(y0, y1);
		if (text[0] == '\003' || text[0] == '\004')
		{
			y++;
		}
		vswr_mode(od_handle, MD_TRANS);
		vst_color(od_handle, txt_col);
		vst_effects(od_handle, TXT_NORMAL);
		if (state & OS_DISABLED)
			vst_effects(od_handle, TXT_LIGHT);
		W_Text(od_handle, x, y, text);
		vst_effects(od_handle, TXT_NORMAL);
		vswr_mode(od_handle, MD_REPLACE);
		Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
		if (ext == 12)
			goto frame;
		break;
	}

  do_3d:

	if (!(state & OS_SHADOWED))
	{
		rect3d(x0, y0, x1, y1, (state & OS_SELECTED) != 0);
	} else
	{
		rect3d(x0, y0, x1, y1, FALSE);
	}
	if (state & OS_SHADOWED)
	{
		Line(x0 + 1, y1 + 1, x1 + 1, y1 + 1, W_PAL_BLACK);
		Line(x1 + 1, y1 + 1, x1 + 1, y0 + 1, W_PAL_BLACK);
		if (!op_draw3d)
		{
			Line(x0 + 1, y1 + 2, x1 + 1, y1 + 2, W_PAL_BLACK);
			Line(x1 + 2, y1 + 2, x1 + 2, y0 + 1, W_PAL_BLACK);
		}
	}

	return;
  frame:

	Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
	if (op_draw3d)
		rect3d(x0 + 1, y0 + 1, x1 - 1, y1 - 1, (state & OS_SELECTED) != 0);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_TEXT: G_TEXT/G_BOXTEXT, ext != 2 - 3
 */
void sysgem_draw_text(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state)
{
	_WORD x0, y0, x1, y1;
	TEDINFO *ted;
	_UWORD eff;
	char *text;
	_UBYTE ext;
	_WORD shortcut_pos;
	char *p;
	
	UNUSED(flags);
	/*
	 * adjust outer dimensions.
	 * In the library, this is done by manipulating the object coordinates
	 * (in NewUserblock)
	 */
	{
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		if ((type & OBTYPEMASK) == G_BOXTEXT)
		{
			if (state & OS_SHADOWED)
			{
				xdiff = 1;
				ydiff = 1;
				wdiff = 2;
				hdiff = 3;
			} else
			{
				xdiff = 0;
				ydiff = 0;
				wdiff = 0;
				hdiff = 2;
			}
			if (x != 0)
				x -= xdiff;
			if (y != 0)
				y -= ydiff;
			w += wdiff;
			h += hdiff;
		}
	}
	
	ted = obspec.tedinfo;
	x0 = x + 1;
	y0 = y + 1;
	x1 = x + w - 2;
	y1 = y + h - 3;
	eff = TXT_NORMAL;

	if (state & OS_DISABLED)
		eff = TXT_LIGHT;

	ext = (type & OBTYPEMASK) >> 8;
	
	/* if (state & OS_WHITEBAK) */
	{
		FilledRect(x0, y0, x1 + 1, y1 + 2, !op_draw3d || ext == 1 ? W_PAL_WHITE : color_background);
	}
		
	if ((type & OBTYPEMASK) == G_BOXTEXT)
	{
		x0--;
		y0--;
		x1++;
		y1++;
		/*
		 * !!! the sysgem library accessed te_color here to get the framesize
		 */
		if (ted->te_thickness != 0)
		{
			rect3d(x0, y0, x1, y1, FALSE);
			if (state & OS_SHADOWED)
			{
				Line(x0 + 1, y1 + 1, x1 + 1, y1 + 1, W_PAL_BLACK);
				Line(x1 + 1, y1 + 1, x1 + 1, y0 + 1, W_PAL_BLACK);
				if (!op_draw3d)
				{
					Line(x0 + 1, y1 + 2, x1 + 1, y1 + 2, W_PAL_BLACK);
					Line(x1 + 2, y1 + 2, x1 + 2, y0 + 1, W_PAL_BLACK);
				}
			}
		}
	} else
	{
		if (state & OS_SELECTED)
			eff |= TXT_THICKENED;
	}

	od_setfont(od_handle, ted->te_font, COLSPEC_GET_TEXTCOL(ted->te_color), eff);

	text = g_strdup(ted->te_ptext);
	shortcut_pos = -1;
	if ((p = strchr(text, '[')) != NULL)
	{
		shortcut_pos = (_WORD)(p - text);
		memmove(text + shortcut_pos, text + shortcut_pos + 1, strlen(text + shortcut_pos + 1) + 1);
	}
	switch (ted->te_just & TE_JUST_MASK)
	{
	case TE_LEFT:
		x = x0;
		break;
	case TE_RIGHT:
		x = x1 - W_TextWidth(od_handle, text) + 1;
		break;
	case 2:
		x = CenterX(x0, x1, text);
		break;
	}

	vswr_mode(od_handle, MD_TRANS);
	y = CenterY(y0, y1);
	
	W_Text(od_handle, x, y, text);
	vst_effects(od_handle, TXT_NORMAL);
	if ((type & OBTYPEMASK) == G_BOXTEXT)
		y--;
	PaintShortCut(x, y, state, text, shortcut_pos);
	g_free(text);
	od_setfont(od_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
	vswr_mode(od_handle, MD_REPLACE);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_FRAME: G_BUTTON, ! OF_EXIT|OF_RBUTTON|OF_SELECTABLE|OF_TOUCHEXIT
 */
void sysgem_draw_frame(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, const char *text, _UWORD flags, _UWORD state)
{
	_WORD x0, y0, x1, y1;
	_WORD len;

	UNUSED(type);
	UNUSED(flags);
	/*
	 * adjust outer dimensions.
	 * In the library, this is done by manipulating the object coordinates
	 * (in NewUserblock)
	 */
	{
		_WORD cw, ch;
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		GetTextSize(&cw, &ch);
		if (sysgem.bergner)
		{
			xdiff = 1;
			ydiff = 2;
			wdiff = 2;
			hdiff = 2;
		} else
		{
			xdiff = 1;
			ydiff = 2 + (ch / 4);
			wdiff = 2 + (cw / 4);
			hdiff = 2 + (ch / 2);
		}
		if (x != 0)
			x -= xdiff;
		if (y != 0)
			y -= ydiff;
		w += wdiff;
		h += hdiff;
	}
	
	x0 = x;
	y0 = y + 1;
	x1 = x + w - 2;
	y1 = y + h - 2;

	vswr_mode(od_handle, MD_REPLACE);
	if (state & OS_WHITEBAK)				/* Whiteback gesetzt? */
	{
		FilledRect(x0, y0, x1, y1, op_draw3d ? color_background : W_PAL_WHITE);
	}

	draw_box5(x0, y0, x1, y1);

	if (empty(text))
		return;

	od_setfont(od_handle, (state & OS_CHECKED) ? SMALL : IBM, sysgem.frametext_color, TXT_NORMAL);

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

/* -------------------------------------------------------------------------- */

/*
 * OBJ_STRING: G_STRING, any
 */
void sysgem_draw_string(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut_pos)
{
	_WORD x0, y0, x1, y1;
	_WORD textcolor;
	_UWORD eff;
	
	UNUSED(flags);
	UNUSED(obspec.index);
	
	/*
	 * no adjustments for outer dimensions.
	 */

	x0 = x /* + 1 */ ;
	y0 = y /* + 1 */ ;
	x1 = x + w - 1;
	y1 = y + h - 1 /*2 */ ;
	eff = TXT_NORMAL;

	if (state & OS_DISABLED)
		eff = TXT_LIGHT;

	textcolor = W_PAL_BLACK;

	if (state & OS_SELECTED)
	{
		if (op_draw3d)
		{
			textcolor = W_PAL_LGRAY;
			FilledRect(x0, y0, x1, y1, W_PAL_BLACK);
		} else
		{
			textcolor = W_PAL_WHITE;
			FilledRect(x0, y0, x1, y1, W_PAL_BLACK);
		}
	} else
	{
		if (op_draw3d)
		{
			FilledRect(x0, y0, x1, y1, color_background);
		} else
		{
			FilledRect(x0, y0, x1, y1, W_PAL_WHITE);
		}
	}

	od_setfont(od_handle, IBM, textcolor, eff);
	vswr_mode(od_handle, MD_TRANS);
	y = CenterY(y0, y1);
	if (shortcut_pos >= 0)
	{
		memmove(text + shortcut_pos, text + shortcut_pos + 1, strlen(text + shortcut_pos + 1) + 1);
	}
	if (((type & OBEXTTYPEMASK) >> 8) == 1)
	{
		x = x1 - W_TextWidth(od_handle, text) - 1;
	} else
	{
		x = x0;
	}
	W_Text(od_handle, x, y, text);
	vst_effects(od_handle, TXT_NORMAL);
	if ((state & OS_SHADOWED) || (state & OS_OUTLINED))
	{
		Line(x0, y1 + 1, x1, y1 + 1, W_PAL_BLACK);
		if (state & OS_OUTLINED)
		{
			Line(x0, y1 + 3, x1, y1 + 3, W_PAL_BLACK);
		}
	} else
	{
		PaintShortCut(x, y, state, text, shortcut_pos);
	}
	vswr_mode(od_handle, MD_REPLACE);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_BOX: G_BOX, ext != 50-52
 */
void sysgem_draw_box(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, _BOOL is_root)
{
	_WORD x0, y0, x1, y1;
	_UBYTE ext;
	
	UNUSED(flags);
	/*
	 * adjust outer dimensions.
	 * In the library, this is done by manipulating the object coordinates
	 * (in NewUserblock)
	 */
	{
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		xdiff = 0;
		ydiff = 0;
		wdiff = 2;
		hdiff = 2;
		if (x != 0)
			x -= xdiff;
		if (y != 0)
			y -= ydiff;
		w += wdiff;
		h += hdiff;
	}
	
	x0 = x;
	y0 = y;
	x1 = x0 + w - 1;
	y1 = y0 + h - 1;

	ext = (type & OBEXTTYPEMASK) >> 8;
	
	vswr_mode(od_handle, MD_REPLACE);
	if (is_root && ext != 16)
	{
		draw_box0(x0 - 1, y0, x1 + 1, y1 + 1);
		return;
	}

	switch (ext)
	{
	default:
	case 0:
	  normal:
		PaintPattern(x0, y0, x1, y1, obspec);
		break;
	case 2:
		rect3d(x0, y0, x1, y1, !(state & OS_SELECTED));
		break;
	case 3:
		rect3d(x0 + 1, y0 + 1, x1 - 1, y1 - 1, !(state & OS_SELECTED));
		rect3d(x0, y0, x1, y1, !(state & OS_SELECTED));
		break;
	case 4:
		if (op_draw3d || sysgem.edit_mono || (state & OS_WHITEBAK))
		{
			x0 -= 2;
			y0 -= 3;
			x1 += 2;
			y1 += 3;

			Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
			x0++;
			y0++;
			x1--;
			y1--;
			FilledRect(x0, y0, x1, y1, W_PAL_WHITE);
		}
		break;
	case 5:
		draw_box5(x0, y0, x1, y1);
		break;
	case 6:
		if (op_draw3d)
		{
			draw_box4(x0 - 3, y0 - 3, x1 + 3, y1 + 3, W_PAL_BLACK);
		} else
		{
			Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
		}
		break;
	case 7:
		Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
		x0++;
		y0++;
		x1--;
		y1--;
		FilledRect(x0, y0, x1, y1, W_PAL_WHITE);
		break;
	case 8:
		if (op_draw3d == FALSE)
			goto normal;
		Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
		x0++;
		y0++;
		x1--;
		y1--;
		rect3d(x0, y0, x1, y1, FALSE);
		x0++;
		y0++;
		x1--;
		y1--;
		FilledRect(x0, y0, x1, y1, color_background);
		break;
	case 9:
		if (op_draw3d == FALSE)
			goto normal;
		Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
		x0++;
		y0++;
		x1--;
		y1--;
		FilledRect(x0, y0, x1, y1, color_activator);
		break;
	case 11:
		draw_box0(x0, y0, x1, y1);
		break;
	case 12:
		draw_box1(x0, y0, x1, y1);
		break;
	case 13:
		x0++;
		x1--;
		if (op_draw3d)
		{
			rect3d(x0, y0, x1, y1, !(state & OS_SELECTED));
			x0++;
			y0++;
			x1--;
			y1--;
		}
		/* fall through */
	case 14:
	  u14:
		if (state & OS_SELECTED)
		{
			FilledRect(x0, y0, x1, y1, op_draw3d ? W_PAL_DGRAY : W_PAL_BLACK);
		} else
		{
			FilledRect(x0, y0, x1, y1, op_draw3d ? W_PAL_LGRAY : W_PAL_WHITE);
		}
		break;
	case 15:
		x0++;
		x1--;
		y0--;
		Line(x0, y0, x1, y0, W_PAL_BLACK);
		Line(x0, y1, x1, y1, W_PAL_BLACK);
		y0++;
		y1--;
		goto u14;
	case 16:							/* Untergrund fr Reiter */
		FilledRect(x0, y0, x1, y1, op_draw3d ? color_background : W_PAL_WHITE);
		Line(x0, y1 + 1, x1, y1 + 1, W_PAL_BLACK);
		break;
	case 17:
		if (op_draw3d)
		{
			y1--;
			Line(x0, y1, x1, y1, W_PAL_DGRAY);
			y1++;
			Line(x0, y1, x1, y1, W_PAL_WHITE);
		} else
		{
			Line(x0, y1, x1, y1, W_PAL_BLACK);
		}
		break;
	case 18:
		if (op_draw3d)
		{
			Line(x0 + 1, y0, x0 + 1, y1, W_PAL_WHITE);
			Line(x0 + 0, y0, x0 + 1, y1, W_PAL_DGRAY);
		} else
		{
			Line(x0 + 1, y0, x0 + 1, y1, W_PAL_BLACK);
		}
		break;
	}
}

void sysgem_draw_notebook(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut_pos)
{
	_WORD x0, y0, x1, y1;
	
	UNUSED(flags);
	UNUSED(obspec.index);
	UNUSED(type);
	/*
	 * adjust outer dimensions.
	 * In the library, this is done by manipulating the object coordinates
	 * (in NewUserblock)
	 */
	{
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		xdiff = 1;
		ydiff = 2;
		wdiff = 2;
		hdiff = 2;
		if (x != 0)
			x -= xdiff;
		if (y != 0)
			y -= ydiff;
		w += wdiff;
		h += hdiff;
	}
	
	vswr_mode(od_handle, MD_REPLACE);
	x0 = x;
	y0 = y;
	x1 = x0 + w - 1;
	y1 = y0 + h - 1;

	y0++;
	y1++;

	Line(x0, y1, x0, y0 + 4, W_PAL_BLACK);
	Line(x0, y0 + 4, x0 + 4, y0, W_PAL_BLACK);
	Line(x0 + 4, y0, x1 - 4, y0, W_PAL_BLACK);
	Line(x1 - 4, y0, x1, y0 + 4, W_PAL_BLACK);
	Line(x1, y0 + 4, x1, y1, W_PAL_BLACK);

	if (state & OS_SELECTED)
	{
		Line(x0 + 1, y1, x1 - 1, y1, op_draw3d ? color_background : W_PAL_WHITE);
	}
	if (op_draw3d)
	{
		x0++;
		y0++;
		x1--;
		y1--;
		Line(x0, y1, x0, y0 + 4, W_PAL_WHITE);
		Line(x0, y0 + 4, x0 + 4, y0, W_PAL_WHITE);
		Line(x0 + 4, y0, x1 - 4, y0, W_PAL_WHITE);
		Line(x1 - 4, y0, x1, y0 + 4, W_PAL_DGRAY);
		Line(x1, y0 + 4, x1, y1, W_PAL_DGRAY);
	}

	vswr_mode(od_handle, MD_TRANS);
	if (shortcut_pos >= 0)
	{
		memmove(text + shortcut_pos, text + shortcut_pos + 1, strlen(text + shortcut_pos + 1) + 1);
	}
	od_setfont(od_handle, IBM, W_PAL_BLACK, state & OS_DISABLED ? TXT_LIGHT : TXT_NORMAL);
	x0 = CenterX(x0, x1, text);
	y0 = CenterY(y0, y1);
	W_Text(od_handle, x0, y0, text);
	vswr_mode(od_handle, MD_REPLACE);
	PaintShortCut(x0, y0, state, text, shortcut_pos);
	vst_effects(od_handle, TXT_NORMAL);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_EDIT: G_FTEXT/G_FBOXTEXT, all
 */
void sysgem_draw_edit(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state)
{
	_WORD x0, y0, x1, y1;
	TEDINFO *ted;
	_WORD i;
	
	UNUSED(flags);
	UNUSED(type);
	/*
	 * adjust outer dimensions.
	 * In the library, this is done by manipulating the object coordinates
	 * (in NewUserblock)
	 */
	{
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		xdiff = 1;
		ydiff = 2;
		wdiff = 6;
		hdiff = 4;
		if (x != 0)
			x -= xdiff;
		if (y != 0)
			y -= ydiff;
		w += wdiff;
		h += hdiff;
	}
	
	vswr_mode(od_handle, MD_REPLACE);
	x0 = x;
	y0 = y;
	x1 = x0 + w - 0;				/* 2 */
	y1 = y0 + h - 1;
	ted = obspec.tedinfo;

	if (!(state & OS_WHITEBAK))			/* whiteback */
	{
		Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
	}
	x0++;
	y0++;
	x1--;
	y1--;
	FilledRect(x0, y0, x1, y1, W_PAL_WHITE);

	x0++;
	y0++;
	x1--;
	y1--;
	vswr_mode(od_handle, MD_REPLACE);
	od_setfont(od_handle, ted->te_font, COLSPEC_GET_TEXTCOL(ted->te_color), TXT_NORMAL);
	/* ??? te_ptmplt not drawn?? */
	for (i = 0; i < (ted->te_txtlen - 1); i++)
	{
		if (ted->te_ptext[i] != '@')
		{
			W_Text(od_handle, x0 + 2, y0, ted->te_ptext);
			break;
		}
	}
	vswr_mode(od_handle, MD_REPLACE);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_HELP: G_BUTTON, exttype 1 + 2
 */
void sysgem_draw_help(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut_pos)
{
	_WORD x0, y0, x1, y1;
	_WORD col;

	if (flags & OF_EXIT)
	{
		sysgem_draw_button(x, y, w, h, type, obspec, flags, state, text, shortcut_pos);
		return;
	}

	/*
	 * adjust outer dimensions.
	 * In the library, this is done by manipulating the object coordinates
	 * (in NewUserblock)
	 */
	{
		_WORD cw, ch;
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		GetTextSize(&cw, &ch);
		if (sysgem.bergner)
		{
			xdiff = 1;
			ydiff = 2;
			wdiff = 2;
			hdiff = 2;
		} else
		{
			xdiff = 1;
			ydiff = 2 + (ch / 4);
			wdiff = 2 + (cw / 4);
			hdiff = 2 + (ch / 2);
		}
		if (x != 0)
			x -= xdiff;
		if (y != 0)
			y -= ydiff;
		w += wdiff;
		h += hdiff;
	}
	
	vswr_mode(od_handle, MD_REPLACE);
	x0 = x;
	y0 = y;
	x1 = x0 + w - 1;
	y1 = y0 + h - 1;
	col = W_PAL_BLACK;

	x0++;
	y0++;
	x1--;
	y1--;
	if (op_draw3d == FALSE)
	{
		Line(x0 + 1, y1 + 1, x1 + 1, y1 + 1, W_PAL_BLACK);
		Line(x1 + 1, y0 + 1, x1 + 1, y1 + 1, W_PAL_BLACK);
	}
	rect3d(x0, y0, x1, y1, state & OS_SELECTED);
	x0++;
	y0++;
	x1--;
	y1--;
	if (op_draw3d)
	{
		FilledRect(x0, y0, x1, y1, color_background);
	} else
	{
		if (state & OS_SELECTED)
		{
			FilledRect(x0, y0, x1, y1, W_PAL_BLACK);
			col = W_PAL_WHITE;
		} else
		{
			FilledRect(x0, y0, x1, y1, W_PAL_WHITE);
		}
	}

	od_setfont(od_handle, SMALL, col, state & OS_DISABLED ? TXT_LIGHT : TXT_NORMAL);
	vswr_mode(od_handle, MD_TRANS);
	if (shortcut_pos >= 0)
	{
		memmove(text + shortcut_pos, text + shortcut_pos + 1, strlen(text + shortcut_pos + 1) + 1);
	}
	x0 = CenterX(x0, x1, text);
	y0 = CenterY(y0, y1);
	W_Text(od_handle, x0, y0, text);
	PaintShortCut(x0, y0, state, text, shortcut_pos);
	od_setfont(od_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
	vswr_mode(od_handle, MD_REPLACE);
}

/*
 * OBJ_LISTBOX: G_TEXT/G_BOXTEXT, exttype 2
 * OBJ_CIRCLE: G_TEXT/G_BOXTEXT, exttype 3
 */
void sysgem_draw_listbox(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state)
{
	_WORD x0, y0, x1, y1;
	char *text;
	char *p;
	_WORD cw, ch;
	_WORD shortcut_pos;
	
	UNUSED(flags);
	/*
	 * adjust outer dimensions.
	 * In the library, this is done by manipulating the object coordinates
	 * (in NewUserblock)
	 */
	{
		_WORD xdiff, ydiff, wdiff, hdiff;
		
		GetTextSize(&cw, &ch);
		xdiff = 1;
		ydiff = 2;
		wdiff = (3 * cw) + (cw / 2);
		hdiff = 5;
		if (x != 0)
			x -= xdiff;
		if (y != 0)
			y -= ydiff;
		w += wdiff;
		h += hdiff;
	}
	
	vswr_mode(od_handle, MD_REPLACE);
	x0 = x;
	y0 = y;
	x1 = x0 + w - 2;
	y1 = y0 + h - 2;
	w = (2 * cw) + 1;

	/* if ((p->pb_currstate ^ p->pb_prevstate) & OS_SELECTED)
		goto do_draw; */

	Frame(x0, y0, x1 - w, y1, -1, W_PAL_BLACK);
	FilledRect(x0 + 1, y0 + 1, x1 - w - 1, y1 - 1, W_PAL_WHITE);
	x = x1 - w - 2;
	Frame(x, y0, x1, y1, -1, W_PAL_BLACK);
	FilledRect(x + 1, y0 + 1, x1 - 1, y1 - 1, op_draw3d ? color_background : W_PAL_WHITE);

	x = ((x1 - x) / 2) - 8 + x + 1;
	y = ((y1 - y0) / 2) - 8 + y0 + 1;

	od_setfont(od_handle, IBM, W_PAL_BLACK, state & OS_DISABLED ? TXT_LIGHT : TXT_NORMAL);
	if ((type & OBEXTTYPEMASK) == (2 << 8)) /* OBJ_LISTBOX */
	{
		if (sysgem.low_resolution)
		{
			vswr_mode(od_handle, MD_TRANS);
			W_Text(od_handle, x + (cw / 2), y, "\002");
			vswr_mode(od_handle, MD_REPLACE);
		} else
		{
			CopyToScreen(op_draw3d, img_pfeil1.big_p, NULL, x, y);
		}
	} else
	{
		if (sysgem.low_resolution)
		{
			vswr_mode(od_handle, MD_TRANS);
			W_Text(od_handle, x + (cw / 2), y, "O");
			vswr_mode(od_handle, MD_REPLACE);
		} else
		{
			CopyToScreen(op_draw3d, img_circle.big_p, NULL, x, y);
		}
	}

/*  do_draw: */

	x = x1 - w - 2;

	if (op_draw3d)
	{
		rect3d(x + 1, y0 + 1, x1 - 1, y1 - 1, state & OS_SELECTED);
	} else
	{
		Line(x0 + 1, y1 + 1, x1 + 1, y1 + 1, W_PAL_BLACK);
		Line(x1 + 1, y1 + 1, x1 + 1, y0 + 1, W_PAL_BLACK);
	}

	vswr_mode(od_handle, MD_TRANS);
	x0 = x0 + 3;
	y0 = CenterY(y0, y1);
	text = g_strdup(obspec.tedinfo->te_ptext);
	shortcut_pos = -1;
	p = strchr(text, '[');
	if (p != NULL)
	{
		shortcut_pos = (_WORD)(p - text);
		memmove(text + shortcut_pos, text + shortcut_pos + 1, strlen(text + shortcut_pos + 1) + 1);
	}
	W_Text(od_handle, x0, y0, text);
	vswr_mode(od_handle, MD_REPLACE);
	PaintShortCut(x0, y0, state, text, shortcut_pos);
	g_free(text);
	vst_effects(od_handle, TXT_NORMAL);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_BALKEN1: G_BOX, ext 50
 * OBJ_BALKEN2: G_BOX, ext 51
 * OBJ_BALKEN3: G_BOX, ext 52
 */
void sysgem_draw_bar(_WORD pb_x, _WORD pb_y, _WORD pb_w, _WORD pb_h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, _WORD next_w, _WORD next_h, OBSPEC next_obspec)
{
	_WORD x0, y0, x1, y1;
	_WORD w, h, i;
	char txt[10];
	
	UNUSED(flags);
	UNUSED(state);
	UNUSED(obspec.index);
	
	/*
	 * no adjustments for outer dimensions.
	 */

	vswr_mode(od_handle, MD_REPLACE);

	x0 = pb_x;
	y0 = pb_y;
	x1 = x0 + pb_w - 1;
	y1 = y0 + pb_h - 1;

	Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);

	w = next_w;
	h = next_h;

	if (pb_w > pb_h)
	{
		x0++;
		y0++;
		x1--;
		y1--;
		if (x0 + w >= x1)
			w = x1 - x0;
		PaintPattern(x0, y0, x0 + w, y1, next_obspec);
		x0 += w;
		if (x0 < x1)
			FilledRect(x0, y0, x1, y1, W_PAL_WHITE);
	} else
	{
		x0++;
		y0++;
		x1--;
		y1--;
		PaintPattern(x0, y0 + h, x1, y1, next_obspec);
		y1 -= h;
		if (y0 < y1)
			FilledRect(x0, y0, x1, y1, W_PAL_WHITE);
	}

	if ((type & OBEXTTYPEMASK) == (51 << 8))
	{
		_WORD font;
		
		x0 = pb_x;
		y0 = pb_y;
		x1 = x0 + pb_w - 1;
		y1 = y0 + pb_h - 1;
		w = next_w;
		h = next_h;

		font = IBM;
		if (pb_w > pb_h)
		{
			if (pb_w == 0)
				i = 100;
			else
				i = (_WORD) (((_LONG) (w) * 100L) / (_LONG) (pb_w));
			if (h < 16)
				font = SMALL;
		} else
		{
			if (pb_h == 0)
				i = 100;
			else
				i = (_WORD) (((_LONG) (h) * 100L) / (_LONG) (pb_h));
			if (w < 40)
				font = SMALL;
		}
		od_setfont(od_handle, font, W_PAL_BLACK, TXT_NORMAL);
		sprintf(txt, "%d %%", i);
		x0 = CenterX(x0, x1, txt);
		y0 = CenterY(y0, y1);

		if (GetNumColors() >= 16)
		{
			_WORD color = OBSPEC_GET_INTERIORCOL(next_obspec);
			if (color == W_PAL_WHITE || color == W_PAL_BLACK)
			{
				vswr_mode(od_handle, MD_XOR);
			} else
			{
				vswr_mode(od_handle, MD_TRANS);
			}
		} else
		{
			vswr_mode(od_handle, MD_XOR);
		}
		W_Text(od_handle, x0, y0, txt);
	}
	od_setfont(od_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
	vswr_mode(od_handle, MD_REPLACE);
}

/* -------------------------------------------------------------------------- */

static _UWORD sysgem_draw_mline(_WORD x, _WORD y, _WORD w, _WORD h)
{
	GRECT gr;

	gr.g_x = x;
	gr.g_y = y + (h / 2) - 1;
	gr.g_w = w;
	gr.g_h = 2;

	W_Fill_Rect(od_handle, &gr, IP_4PATT, W_PAL_BLACK, FALSE);
	return 0;
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_MENTRY: entry in menu
 * OBJ_MTITLE: title in menu
 */
void sysgem_draw_mentry(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut_pos)
{
	_WORD x0, y0, x1, y1;

	UNUSED(flags);
	UNUSED(obspec.index);
	
	/*
	 * no adjustments for outer dimensions.
	 */

	vswr_mode(od_handle, MD_REPLACE);
	od_setfont(od_handle, IBM, W_PAL_BLACK, TXT_NORMAL);
	x0 = x;
	y0 = y;
	x1 = x0 + w - 1;
	y1 = y0 + h - 1;

	if (text != NULL)
	{
		if (text[0] == '-' && text[1] == '-' && text[2] == '-' && (state & OS_DISABLED))
		{
			if (op_draw3d)
			{
				x0 += 2;
				y0 += (od_ch / 2);
				x1 -= 2;
				Line(x0, y0, x1, y0, color_activator);
				y0++;
				Line(x0, y0, x1, y0, W_PAL_WHITE);
			} else
			{
				sysgem_draw_mline(x, y, w, h);
			}
			return;
		}
		if (shortcut_pos >= 0)
		{
			memmove(text + shortcut_pos, text + shortcut_pos + 1, strlen(text + shortcut_pos + 1) + 1);
		}
		if ((type & OBTYPEMASK) == G_TITLE)
		{
			y = y1 - 1;
		} else
		{
			y = y1;
		}
		if (state & OS_SELECTED)
		{
			FilledRect(x0, y0, x1, y, op_draw3d ? sysgem.selb_color : W_PAL_BLACK);
		} else
		{
			FilledRect(x0, y0, x1, y, op_draw3d ? color_background : W_PAL_WHITE);
		}
		y0 = CenterY(y0, y1 - 1);
		vswr_mode(od_handle, MD_TRANS);
		if (state & OS_DISABLED)
		{
			vst_effects(od_handle, TXT_LIGHT);
		}
		if (state & OS_SELECTED)
		{
			vst_color(od_handle, op_draw3d ? sysgem.selt_color : W_PAL_WHITE);
			if (state & OS_CHECKED)
			{
				W_Text(od_handle, x0 + 2, y0, "");
			}
			W_Text(od_handle, x0, y0, text);
		} else
		{
			if (state & OS_CHECKED)
			{
				W_Text(od_handle, x0 + 2, y0, "");
			}
			W_Text(od_handle, x0, y0, text);
		}
	}
	vst_color(od_handle, W_PAL_BLACK);
	vst_effects(od_handle, TXT_NORMAL);
	vswr_mode(od_handle, MD_REPLACE);
}

/* -------------------------------------------------------------------------- */

/*
 * OBJ_MBOX: G_BOX in menu
 */
void sysgem_draw_mbox(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state)
{
	_WORD x0, y0, x1, y1;

	UNUSED(flags);
	UNUSED(state);
	UNUSED(type);
	UNUSED(obspec.index);
	
	/*
	 * no adjustments for outer dimensions.
	 */

	vswr_mode(od_handle, MD_REPLACE);
	x0 = x;
	y0 = y;
	x1 = x0 + w - 1;
	y1 = y0 + h - 1;

#if 0
	if (obj <= 3)
	{
		x0--;
		x1++;
		y0--;
		y1++;
		Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
		x0++;
		x1--;
		y0++;
		y1--;
		FilledRect(x0, y0, x1, y1, op_draw3d ? color_background : W_PAL_WHITE);
		return;
	}
#endif
	x0--;
	x1++;
	y1++;
	Frame(x0, y0, x1, y1, -1, W_PAL_BLACK);
	x0++;
	x1--;
	y1--;
	FilledRect(x0, y0, x1, y1, op_draw3d ? color_background : W_PAL_WHITE);
}

/******************************************************************************/
/* -------------------------------------------------------------------------- */
/******************************************************************************/

static void fix_image(SYSGEM_IMAGE *image)
{
	image->big_p = image->big;
	if (image->small != NULL)
	{
		image->small_p = image->small;
	}
}

/* -------------------------------------------------------------------------- */

static void release_image(SYSGEM_IMAGE *image)
{
	if (image->small != NULL)
	{
	}
}

/* -------------------------------------------------------------------------- */

void sysgem_exit(void)
{
	release_image(&img_pfeil1);
	release_image(&img_pfeil1_disabled);
	release_image(&img_pfeil4);
	release_image(&img_pfeil4_disabled);
	release_image(&img_pfeil5);
	release_image(&img_pfeil5_disabled);
	release_image(&img_pfeil6);
	release_image(&img_pfeil6_disabled);
	release_image(&img_circle);
	release_image(&img_circle_disabled);
	release_image(&img_sel_desel_norm);
	release_image(&img_sel_sel_norm);
	release_image(&img_sel_desel_dis);
	release_image(&img_sel_sel_dis);
	release_image(&img_cir_desel_norm);
	release_image(&img_cir_sel_norm);
	release_image(&img_cir_desel_dis);
	release_image(&img_cir_sel_dis);
}

/* -------------------------------------------------------------------------- */

void sysgem_init(void)
{
	_WORD cw, ch;
	
	GetTextSize(&cw, &ch);
	if (GetNumColors() >= 16)
	{
		sysgem.shortcut_color = W_PAL_RED;
		sysgem.frametext_color = W_PAL_RED;
		sysgem.selb_color = W_PAL_RED;
		sysgem.selt_color = W_PAL_WHITE;
		sysgem.help_color = W_PAL_YELLOW;
	} else
	{
		sysgem.shortcut_color = W_PAL_BLACK;
		sysgem.frametext_color = W_PAL_BLACK;
		sysgem.selb_color = W_PAL_BLACK;
		sysgem.selt_color = W_PAL_BLACK;
		sysgem.help_color = W_PAL_BLACK;
	}
	sysgem.like_mac = TRUE;
	sysgem.edit_mono = FALSE;
	sysgem.low_resolution = ch < 13;
	sysgem.bergner = FALSE;
	
	fix_image(&img_pfeil1);
	fix_image(&img_pfeil1_disabled);
	fix_image(&img_pfeil4);
	fix_image(&img_pfeil4_disabled);
	fix_image(&img_pfeil5);
	fix_image(&img_pfeil5_disabled);
	fix_image(&img_pfeil6);
	fix_image(&img_pfeil6_disabled);
	fix_image(&img_circle);
	fix_image(&img_circle_disabled);
	fix_image(&img_sel_desel_norm);
	fix_image(&img_sel_sel_norm);
	fix_image(&img_sel_desel_dis);
	fix_image(&img_sel_sel_dis);
	fix_image(&img_cir_desel_norm);
	fix_image(&img_cir_sel_norm);
	fix_image(&img_cir_desel_dis);
	fix_image(&img_cir_sel_dis);
}
