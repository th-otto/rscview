/*
 *  $Id: n_v_read_default_settings.c,v 1.2 2005/02/23 21:10:19 a_bercegeay Exp $
 */

#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param settings Pointer to a structur of the Typ PRN_SETTINGS
 *
 *  @return 0 for an error
 *
 *  @since NVDI 5.00
 *
 *
 *
 */

short v_read_default_settings( short handle, PRN_SETTINGS *settings )
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[VDI_NPTRINTS];
	short vdi_intout[1]; 

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, 0L);

	vdi_intin_ptr(0, PRN_SETTINGS *) = settings;
	
	VDI_TRAP (vdi_params, handle, 182, 0, VDI_NPTRINTS);

	if ( vdi_control[4] >= 1 )
		return vdi_intout[0];

	return 0;
}
