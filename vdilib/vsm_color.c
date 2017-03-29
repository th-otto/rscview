/*
 *  $Id: vsm_color.c,v 1.7 2003/08/07 07:35:42 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** sets the color index for markers
 *
 *  @param handle Device handle
 *  @param index marker color. If the index is invalid, 
 *         color index 1 will be selected.
 *
 *  @return selected color
 *
 *  @since all VDI versions
 *
 */

short
vsm_color (short handle, short index)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[VDI_INTOUTMAX]; 
	
	VDI_PARAMS(vdi_control, &index, 0L, vdi_intout, vdi_dummy );
	
	VDI_TRAP (vdi_params, handle, 20, 0,1);

	return vdi_intout[0];
}
