/*
 *  $Id: n_v_ctab_idx2value.c,v 1.6 2003/08/07 06:28:22 a_bercegeay Exp $
 */

#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param index 
 *
 *  @return 
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

unsigned long
v_ctab_idx2value (short handle, short index)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[2]; 

	VDI_PARAMS(vdi_control, &index, 0L, vdi_intout, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 206,5, 0,1);

	return vdi_intout_long(0);
}
