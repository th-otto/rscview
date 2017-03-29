/*
 *  $Id: vq_key_s.c,v 1.7 2003/08/07 07:11:43 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** "SAMPLE KEYBOARD STATE INFORMATION" returns the state of CONTROL, ALTERNATE 
 *  and the SHIFT key(s).
 *
 *  @param handle Device handle
 *  @param state keyboard (bit numbers):
 *         - 0: Shift right
 *         - 1: Shift left
 *         - 2: Control
 *         - 3: Alternate
 *
 *  @since all VDI versions
 *
 *  @note Use the AES event functions to inquire the keyboard state
 *
 *
 */

void
vq_key_s (short handle, short *state)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[VDI_INTOUTMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_intout, vdi_dummy );
	
	VDI_TRAP_00 (vdi_params, handle, 128);
	*state = vdi_intout[0];
}
