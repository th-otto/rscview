/*****************************************************************************
 * W_DRAW.H
 *****************************************************************************/

#ifndef __W_DRAW_H__
#define __W_DRAW_H__

#ifndef __PORTAB_H__
#  include <portab.h>
#endif
#ifndef __GRECT_H__
#  include <grect.h>
#endif
#include <portvdi.h>

EXTERN_C_BEG


#define INVERT      1
#define FILL        2
#define OUTLINE     4

CICON *cicon_best_match(CICONBLK *cicon, _WORD planes);
CICON *cicon_find_plane(CICON *list, _WORD planes);

_BOOL W_Fix_Bitmap(_VOID **data, _UWORD w, _UWORD h, _WORD nplanes);
_VOID W_Release_Bitmap(_VOID **data, _UWORD w, _UWORD h, _WORD nplanes);
#define W_Fix_Bitmap(d, w, h, n) W_Fix_Bitmap((_VOID **)(&(d)), w, h, n)
#define W_Release_Bitmap(d, w, h, n) W_Release_Bitmap((_VOID **)(&(d)), w, h, n)

_BOOL W_Cicon_Setpalette(_WORD *palette);

EXTERN_C_END

#endif /* __W_DRAW_H__ */
