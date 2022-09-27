#include "gem_vdiP.h"

void vs_drawrect(short handle, short clip_flag, short pxy[])
{
	short vdi_control[VDI_CNTRLMAX]; 
	
	VDI_PARAMS(vdi_control, &clip_flag, vdi_dummy, vdi_dummy, pxy);
	
	VDI_TRAP_ESC(vdi_params, handle, 96, 2, 0, 1);
}
