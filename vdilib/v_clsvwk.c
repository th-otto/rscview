/*
 *  $Id: v_clsvwk.c,v 1.7 2003/08/07 06:54:13 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** closes a virtual workstation
 *
 *  @param handle Device handle
 *
 *  @since all VDI versions
 *
 */

void
v_clsvwk (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_00 (vdi_params, handle, 101);
}
