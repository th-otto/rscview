/*
 *  $Id: v_enter_cur.c,v 1.6 2003/08/07 06:59:17 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** This function enables text mode. The screen is cleared and the text
 *  cursor is displayed in the upper left cell of the screen.
 *
 *  @param handle Device handle
 *
 *  @since all VDI versions
 *
 */

void
v_enter_cur (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_ESC (vdi_params, handle, 5,3, 0,0);
}
