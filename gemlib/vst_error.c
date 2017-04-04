/*
 *  $Id: vst_error.c,v 1.7 2003/08/07 07:38:13 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** 
 *
 *  @param handle Device handle
 *  @param mode 
 *  @param errorvar 
 *
 *  @since 
 *
 *  @sa
 *
 *
 *
 */

void
vst_error (short handle, short mode, short *errorvar)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[1 + VDI_NPTRINTS];   
	short vdi_intout[VDI_INTOUTMAX]; /* todo (doc wanted) */
	short vdi_ptsout[VDI_PTSOUTMAX]; /* todo (doc wanted) */
	
	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_ptsout );
	
	vdi_intin    [0] = mode;
	vdi_intin_ptr(1, short *) = errorvar;
	
	VDI_TRAP (vdi_params, handle, 245, 0, 1 + VDI_NPTRINTS);
}
