#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param color_space color space
 *  @param color COLOR_ENTRY of the color
 *
 *  @return the pixel value
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

uint32_t v_color2value (short handle, int32_t color_space, COLOR_ENTRY * color)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[6];   
	short vdi_intout[2]; 

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy);
	
	vdi_intin_long(0) = color_space;
	*(COLOR_ENTRY*)&vdi_intin[2] = *color;

	VDI_TRAP (vdi_params, handle, 204, 0,6);

	return vdi_intout_long(0);
}
