#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param point desired character height
 *  @param wchar  \n
 *         [option CHECK_NULLPTR] wchar may be NULL
 *  @param hchar  \n
 *         [option CHECK_NULLPTR] hchar may be NULL
 *  @param wcell  \n
 *         [option CHECK_NULLPTR] wcell may be NULL
 *  @param hcell  \n
 *         [option CHECK_NULLPTR] hcell may be NULL
 *
 *  @return character height set in points
 *
 *  @since SpeedoGDOS 4.0
 *
 *
 *
 */

short
vst_arbpt (short handle, short point,
           short *wchar, short *hchar, short *wcell, short *hcell)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[2]; 
	short vdi_ptsout[4]; 

	VDI_PARAMS(vdi_control, &point, 0L, vdi_intout, vdi_ptsout );
	vdi_intout[0] = 0;
	
	VDI_TRAP (vdi_params, handle, 246, 0,1);

#if CHECK_NULLPTR
	if (wchar)
#endif
	*wchar = vdi_ptsout[0];
#if CHECK_NULLPTR
	if (hchar)
#endif
	*hchar = vdi_ptsout[1];
#if CHECK_NULLPTR
	if (wcell)
#endif
	*wcell = vdi_ptsout[2];
#if CHECK_NULLPTR
	if (hcell)
#endif
	*hcell = vdi_ptsout[3];
	
	if (VDI_N_INTOUT >= 1)
		return vdi_intout[0];
	return 0;
}
