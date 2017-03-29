/*
 *  $Id: n_v_ftext16n.c,v 1.4 2003/08/07 06:31:13 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** see v_ftext()
 *
 *  @param handle Device handle
 *  @param pos 
 *  @param wstr string (16 bits per character)
 *  @param num string len
 *
 *
 *
 */

void
v_ftext16n (short handle, short x, short y, const WCHAR * wstr, short num)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_ptsin[2];   

	VDI_PARAMS(vdi_control, (short *)NO_CONST(wstr), vdi_ptsin, vdi_dummy, vdi_dummy);
	
	vdi_ptsin[0] = x;
	vdi_ptsin[1] = y;

	VDI_TRAP (vdi_params, handle, 241, 1,num);
}
