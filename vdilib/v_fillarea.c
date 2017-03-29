/*
 *  $Id: v_fillarea.c,v 1.7 2003/08/07 06:59:17 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** draws a filled area, for example a complex polygon.
 *
 *  @param handle Device handle
 *  @param count 
 *  @param pxy coordinates
 *
 *  @since all VDI versions
 *
 *  The area is filled using the
 *	following current attributes:
 *	 - fill area color
 *	 - interior style (hollow, solid, pattern, hatch
 *	   or user-defined)
 *	 - writing mode
 *	 - style index
 *
 *	The area is outlined with a solid line of the
 *	current fill area color if the fill area
 *	perimeter visibility is on, which is the
 *	default at Open Workstation. *
 *
 */

void
v_fillarea (short handle, short count, short pxy[])
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, pxy, vdi_dummy, vdi_dummy );
	
	VDI_TRAP (vdi_params, handle, 9, count,0);
}
