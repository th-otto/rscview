/*
 *  $Id: vs_mute.c,v 1.3 2003/08/07 07:23:16 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** This escape set/unset/return the tone mute flag
 *
 *  @param handle Device handle
 *  @param action:
 *         - (-1) inquire the mute flag
 *         - 0 clear the mute flag (sound on)
 *         - 1 set the mute flag (sound off)
 *
 *  @return the mute flag set
 *
 *  @since PC-GEM >= 2.0
 *
 *
 */

short
vs_mute (short handle, short action)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[VDI_INTOUTMAX]; 
	
	VDI_PARAMS(vdi_control, &action, 0L, vdi_intout, vdi_dummy );
		
	VDI_TRAP_ESC (vdi_params, handle, 5,62, 0,1);
	
	return vdi_intout[0];
}