/*
 *  $Id: n_vq_prn_scaling.c,v 1.6 2003/08/07 06:38:20 a_bercegeay Exp $
 */

#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *
 *  @return 
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

long
vq_prn_scaling (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[2] = {-1,-1};   
	short vdi_intout[2]; 

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 5,39, 0,2);

	return (vdi_control[4] == 2 ? vdi_intout_long(0) : -1L);
}
