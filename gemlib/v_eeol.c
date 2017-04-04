/*
 *  $Id: v_eeol.c,v 1.6 2003/08/07 06:56:47 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** This function erases the screen from the cursor position to the end of line
 *
 *  @param handle Device handle
 *
 *  @since all VDI versions
 *
 */

void
v_eeol (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_ESC (vdi_params, handle, 5,10, 0,0);
}
