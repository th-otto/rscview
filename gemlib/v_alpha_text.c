/*
 *  $Id: v_alpha_text.c,v 1.6 2003/08/07 06:54:13 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** 
 *
 *  @param handle Device handle
 *  @param str 
 *
 *  @since 
 *
 *  @sa
 *
 *
 *
 */

void
v_alpha_text (short handle, const char *str)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[VDI_INTINMAX];   
	short vdi_intout[VDI_INTOUTMAX]; /* from TOS.HYP */
	register short n = vdi_str2array (str, vdi_intin);

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 5,25, 0,n);
}
