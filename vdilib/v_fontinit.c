/*
 *  $Id: v_fontinit.c,v 1.4 2003/08/07 06:59:17 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** this escape install the given font as font system.
 *
 *  @param handle Device handle
 *  @param font_header
 *
 *  @since ???
 *
 *  @note this function is not descibed in any doc. Usage at your
 *  own risk! BTW, it seems that characters must be 8 pixels width (constant),
 *  and the format used must be the motorola format.
 *
 */

void
v_fontinit (short handle, const void * font_header)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, (short*)&font_header, 0L, vdi_dummy, vdi_dummy );
		
	VDI_TRAP_ESC (vdi_params, handle, 5,102, 0,sizeof(void *)/sizeof(short));
}
