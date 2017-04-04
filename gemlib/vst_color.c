/*
 *  $Id: vst_color.c,v 1.7 2003/08/07 07:38:13 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** sets the color index for text output
 *
 *  @param handle Device handle
 *  @param index requested color index. If the index is invalid, color 
 *         index 1 will be selected.
 *
 *  @return selected color index
 *
 *  @since all VDI versions
 */

short
vst_color (short handle, short index)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[VDI_INTOUTMAX]; 
	
	VDI_PARAMS(vdi_control, &index, 0L, vdi_intout, vdi_dummy );
	
	VDI_TRAP (vdi_params, handle, 22, 0,1);

	return vdi_intout[0];
}
