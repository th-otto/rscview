#include "config.h"
#include <gem.h>
#include "portvdi.h"
#include "or_draw.h"

static _WORD vdi_handle;

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

void ob_draw_dialog(_WORD _vdi_handle, OBJECT *tree, _WORD start, _WORD depth, GRECT *gr, EXTOB_MODE mode)
{
	vdi_handle = _vdi_handle;
	tree = tree;
	start = start;
	depth = depth;
	gr = gr;
	mode = mode;
}
