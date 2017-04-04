/*
 *  $Id: n_v_delete_itab.c,v 1.6 2003/08/07 06:28:22 a_bercegeay Exp $
 */

#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param itab 
 *
 *  @return 
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

short
v_delete_itab (short handle, ITAB_REF itab)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[1]; 

	VDI_PARAMS(vdi_control, (short*)&itab, 0L, vdi_intout, vdi_dummy);

	VDI_TRAP_ESC (vdi_params, handle, 208,1, 0,2);

	return vdi_intout[0];
}
