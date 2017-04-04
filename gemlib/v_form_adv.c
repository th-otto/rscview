/*
 *  $Id: v_form_adv.c,v 1.6 2003/08/07 06:59:17 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** is required only for printers.
 *	It advances the printer page. This escape can
 *	be used instead of invoking a Clear Workstation
 *	function if it is desirable to retain the
 *	current printer display list while advancing to
 *	the next page.
 *
 *  @param handle Device handle
 *
 *  @since all VDI versions
 *
 */

void
v_form_adv (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_ESC (vdi_params, handle, 5,20, 0,0);
}
