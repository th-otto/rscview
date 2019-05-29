#include "gem_vdiP.h"

/** returns a string from the keyboard, and returns if the maximum string length is reached or if the user has pressed 
 *  RETURN.
 *
 *  @param handle Device handle
 *  @param len is the maximum length of the string returned in \p vdi_intout.
 *         If \p len 
 *         is negative, the absolute value is considered to be the maximum length and scan 
 *         codes are returned instead of ASCII values.
 *  @param echo 0: no output 1: echo
 *  @param echoxy coordinates of echo area
 *  @param str input buffer
 *
 *  @since all VDI versions
 *
 */

void
vrq_string16 (short handle, short len, short echo, short echoxy[], short *str)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[2];   
	
	VDI_PARAMS(vdi_control, vdi_intin, echoxy, str, vdi_dummy );
	
	vdi_intin[0]      = len;
	vdi_intin[1]      = echo;
	
	VDI_TRAP (vdi_params, handle, 31, 1,2);
}
