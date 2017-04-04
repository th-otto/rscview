/*
 *  $Id: v_set_app_buff.c,v 1.7 2003/08/07 07:05:33 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** 
 *
 *  @param handle Device handle
 *  @param buf_p 
 *  @param size 
 *
 *  @since 
 *
 *  @sa
 *
 *
 *
 */

void
v_set_app_buff (short handle, void **buf_p, short size)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[1 + VDI_NPTRINTS];   

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_dummy, vdi_dummy );
	
	vdi_intin_ptr(0, void *) = buf_p;
	vdi_intin    [VDI_NPTRINTS] = size;
	
	VDI_TRAP_ESC (vdi_params, handle, -1,6, 0, 1 + VDI_NPTRINTS);
}
