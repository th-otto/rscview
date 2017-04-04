/*
 *  $Id: v_clswk.c,v 1.7 2003/08/07 06:54:13 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** closes a physical workstation
 *
 *  @param handle Device handle
 *
 *  @since all VDI versions
 *
 *  If you call v_clswk() for a printer driver and if you have not called v_updwk() 
 *  or v_clear_disp_list() before, buffered commands will be executed and sent to 
 *  the printer.
 *
 */

void
v_clswk (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy);
	
	VDI_TRAP_00 (vdi_params, handle, 2);
}
