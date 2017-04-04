/*
 *  $Id: n_v_killoutline.c,v 1.6 2003/08/07 06:33:49 a_bercegeay Exp $
 */

#include "gem_vdiP.h"
#include "mt_gemx.h"

void v_killoutline (short handle, fsm_component_t *component)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[VDI_NPTRINTS];

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_dummy, vdi_dummy);
	
	vdi_intin_ptr(0, fsm_component_t *) = component;
	
	VDI_TRAP (vdi_params, handle, 242, 0, VDI_NPTRINTS);
}
