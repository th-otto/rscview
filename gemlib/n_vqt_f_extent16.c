/*
 *  $Id: n_vqt_f_extent16.c,v 1.7 2003/08/07 06:43:50 a_bercegeay Exp $
 */

#include "gem_vdiP.h"
#include "mt_gemx.h"

/** see vqt_f_extent()
 *
 *  @param handle Device handle
 *  @param wstr 
 *  @param extent 
 *
 *  @since NVDI 3.00
 *
 *
 *
 */

void
vqt_f_extent16 (short handle, const WCHAR * wstr, short extent[])
{
	short vdi_control[VDI_CNTRLMAX]; 
	register short n = vdi_wstrlen ((const short *)wstr);

	VDI_PARAMS(vdi_control, (const short *)wstr, 0L, vdi_dummy, extent);

	VDI_TRAP (vdi_params, handle, 240, 0,n);
}