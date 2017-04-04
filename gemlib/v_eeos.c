/*
 *  $Id: v_eeos.c,v 1.6 2003/08/07 06:56:47 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** The screen is erased from the cursor position to the end of the screen
 *
 *  @param handle Device handle
 *
 *  @since all VDI versions
 *
 */

void
v_eeos (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_ESC (vdi_params, handle, 5,9, 0,0);
}
