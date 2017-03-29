#include "config.h"
#include <gem.h>
#include <w_draw.h>
#include <font.h>
#include <fonthdr.h>

#define FONT_WIDTH   8
#define FONT_HEIGHT 16


void GetTextSize(_WORD *wchar, _WORD *hchar)
{
	*wchar = FONT_WIDTH;
	*hchar = FONT_HEIGHT;
}


void font_init(void)
{
}
