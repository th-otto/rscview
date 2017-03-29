/*
 *  $Id: n_v_setrgb.c,v 1.6 2003/08/07 06:33:49 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** 
 *
 *  @param handle Device handle
 *  @param type 
 *  @param r 
 *  @param g 
 *  @param b 
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

void
v_setrgb (short handle, short type, short r, short g, short b)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[3];   

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_dummy, vdi_dummy);
	
	vdi_intin[0] = r;
	vdi_intin[1] = g;
	vdi_intin[2] = b;

	VDI_TRAP_ESC (vdi_params, handle, 138,type, 0,3);
}
