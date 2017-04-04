/*
 *  $Id: n_v_create_itab.c,v 1.6 2003/08/07 06:28:22 a_bercegeay Exp $
 */

#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param ctab 
 *  @param bits 
 *
 *  @return 
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

ITAB_REF
v_create_itab (short handle, COLOR_TAB * ctab, short bits)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[3 + VDI_NPTRINTS];
	short vdi_intout[VDI_NPTRINTS];

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy);
	
	vdi_intin_ptr(0, COLOR_TAB *) = ctab;
	vdi_intin    [VDI_NPTRINTS] = bits;
	vdi_intin    [VDI_NPTRINTS + 1] = 0;
	vdi_intin    [VDI_NPTRINTS + 2] = 0;

	VDI_TRAP (vdi_params, handle, 208, 0, 3 + VDI_NPTRINTS);

	return vdi_intout_ptr(0, ITAB_REF);
}