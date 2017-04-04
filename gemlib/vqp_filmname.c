/*
 *  $Id: vqp_filmname.c,v 1.3 2003/08/07 07:15:37 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** this escape returns the name of a film.
 *
 *  @param handle Device handle
 *  @param index
 *  @param name
 *
 *  @return 0 (function not supported or wrong film index), or any positive
 *   value otherwise.
 *
 *  @since PC-GEM >= 2.0
 *
 *  @note a wrong film index returns an empty filmname.
 *
 */

short vqp_filmname (short handle, short index, char * name)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[VDI_INTOUTMAX]; 
	short vdi_intin[1]; 

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy );

	vdi_intin[0] = index;
		
	VDI_TRAP_ESC (vdi_params, handle, 5,91, 0,1);
	
	vdi_array2str( vdi_intout, name, vdi_control[4]);
	
	return vdi_control[4];
}
