/*
 *  $Id: vqt_extent16.c,v 1.4 2003/08/07 07:15:37 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** see vqt_extent()
 *
 *  @param handle Device handle
 *  @param wstr string in 16-bit format (16 bits per character)
 *  @param extent coordinates of the rectangle
 *
 */

void
vqt_extent16 (short handle, const short *wstr, short extent[])
{
	register short n = vdi_wstrlen (wstr);
	short vdi_control[VDI_CNTRLMAX]; 
	
	VDI_PARAMS( vdi_control, (short *)NO_CONST(wstr), 0L, vdi_dummy, extent);
	
	VDI_TRAP (vdi_params, handle, 116, 0,n);
}