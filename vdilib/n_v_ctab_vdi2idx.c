/*
 *  $Id: n_v_ctab_vdi2idx.c,v 1.6 2003/08/07 06:28:22 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** 
 *
 *  @param handle Device handle
 *  @param vdi_index 
 *
 *  @return 
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

short
v_ctab_vdi2idx (short handle, short vdi_index)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[1]; 

	VDI_PARAMS(vdi_control, &vdi_index, 0L, vdi_intout, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 206,4, 0,1);

	return vdi_intout[0];
}
