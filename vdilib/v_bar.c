/*
 *  $Id: v_bar.c,v 1.7 2003/08/07 06:54:13 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** draws a filled rectangle. In contrast to "FILLED RECTANGLE" a 
 *  border is drawn.
 *
 *  @param handle Device handle
 *  @param pxy coordinates of the rectangle
 *
 *  @since all VDI versions
 *
 */

void
v_bar (short handle, short pxy[])
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, pxy, vdi_dummy, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 11,1, 2,0);
}
