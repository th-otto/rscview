/*
 *  $Id: n_v_clsbm.c,v 1.6 2003/08/07 06:26:01 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** closes an offscreen bitmap. If the VDI has allocated the memory
 *  for the bitmap, this call frees the memory.
 *
 *  @param handle Device handle
 *
 *  @since since EdDI 1.00
 *
 */

void
v_clsbm (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 101,1, 0,0);
}
