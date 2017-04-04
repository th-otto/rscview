/*
 *  $Id: v_pline.c,v 1.7 2003/08/07 07:01:52 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** connects all points in \p pxy with lines
 *
 *  @param handle Device handle
 *  @param count 
 *  @param pxy coordinates
 *
 *  @since all VDI versions
 *
 */

void
v_pline (short handle, short count, short pxy[])
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, pxy, vdi_dummy, vdi_dummy );
	
	VDI_TRAP (vdi_params, handle, 6, count,0);
}
