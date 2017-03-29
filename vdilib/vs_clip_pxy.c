/*
 *  $Id: vs_clip_pxy.c,v 1.5 2003/12/22 15:09:48 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** same as vs_clip(handle,1,pxy)
 *
 *  @param handle Device handle
 *  @param pxy same as vs_clip()
 *
 */

void
vs_clip_pxy (short handle, PXY pxy[])
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[1] = {1};   
	
	VDI_PARAMS(vdi_control, vdi_intin, (short*)pxy, vdi_dummy, vdi_dummy );

	VDI_TRAP (vdi_params, handle, 129, 2,1);
}
