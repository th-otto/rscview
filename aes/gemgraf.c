/*		GEMGRAF.C		04/11/84 - 09/17/85 	Lee Lorenzen			*/
/*		merge High C vers. w. 2.2				8/21/87 		mdf 	*/
/*		fix gr_gicon null text					11/18/87		mdf 	*/

/*
 *		Copyright 1999, Caldera Thin Clients, Inc.
 *				  2002-2016 The EmuTOS development team
 *
 *		This software is licenced under the GNU Public License.
 *		Please see LICENSE.TXT for further information.
 *
 *				   Historical Copyright
 *		-------------------------------------------------------------
 *		GEM Application Environment Services			  Version 2.3
 *		Serial No.	XXXX-0000-654321			  All Rights Reserved
 *		Copyright (C) 1987						Digital Research Inc.
 *		-------------------------------------------------------------
 */

#include "aes.h"

_WORD gl_nplanes;					/* number of planes in current res */
_WORD gl_width;
_WORD gl_height;
_WORD gl_nrows;
_WORD gl_ncols;
_WORD gl_wchar;
_WORD gl_hchar;
_WORD gl_wschar;
_WORD gl_hschar;
_WORD gl_wptschar;
_WORD gl_hptschar;
_WORD gl_wsptschar;
_WORD gl_hsptschar;
_WORD gl_wbox;
_WORD gl_hbox;
_WORD gl_xclip;
_WORD gl_yclip;
_WORD gl_wclip;
_WORD gl_hclip;
_WORD gl_handle;
_WORD gl_mode;
_WORD gl_mask; /* unused */
_WORD gl_tcolor;
_WORD gl_lcolor;
_WORD gl_fis;
_WORD gl_patt;
_WORD gl_font;
GRECT gl_rscreen;
GRECT gl_rfull;
GRECT gl_rzero;
GRECT gl_rcenter;
GRECT gl_rmenu;
WS gl_ws;
