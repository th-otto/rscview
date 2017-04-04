/*
 *  $Id: v_curup.c,v 1.6 2003/08/07 06:56:47 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** The cursor is moved up one row. If the cursor is already in the first row, 
 *  nothing happens.
 *
 *  @param handle Device handle
 *
 *  @since all VDI versions
 *
 */

void
v_curup (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_ESC (vdi_params, handle, 5,4, 0,0);
}
