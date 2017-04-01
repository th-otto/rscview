/*
 * gemgsxif.c - AES's interface to VDI (gsx)
 *
 * Copyright 2002-2016 The EmuTOS development team
 *			 1999, Caldera Thin Clients, Inc.
 *			 1987, Digital Research Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 *                  Historical Copyright
 *	-------------------------------------------------------------
 *	GEM Application Environment Services		  Version 1.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985			Digital Research Inc.
 *	-------------------------------------------------------------
 */

/*		GEMGSXIF.C		05/06/84 - 06/13/85 	Lee Lorenzen			*/
/*		merge High C vers. w. 2.2				8/21/87 		mdf 	*/

/*
 * Calls used in Crystal:
 *
 * vsf_interior();
 * vr_recfl();
 * vst_height();
 * vsl_type();
 * vsl_udsty();
 * vsl_width();
 * v_pline();
 * vst_clip();
 * vex_butv();
 * vex_motv();
 * vex_curv();
 * vex_timv();
 * vr_cpyfm();
 * v_opnwk();
 * v_clswk();
 * vq_extnd();
 * v_clsvwk( handle )
 * v_opnvwk( pwork_in, phandle, pwork_out )
 */

#include "aes.h"

#define STATIC

STATIC _BOOL gl_graphic;

