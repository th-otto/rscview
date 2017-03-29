/*
 *  $Id: v_exit_cur.c,v 1.6 2003/08/07 06:59:17 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** "EXIT ALPHA MODE" switches back to graphics mode.
 *
 *  @param handle Device handle
 *
 *  @since all VDI versions
 *
 */

void
v_exit_cur (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_ESC (vdi_params, handle, 5,2, 0,0);
}
