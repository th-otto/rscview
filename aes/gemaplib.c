/*		GEMAPLIB.C		03/15/84 - 08/21/85 	Lee Lorenzen			*/
/*		merge High C vers. w. 2.2 & 3.0 		8/19/87 		mdf 	*/

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


#define TCHNG 0
#define BCHNG 1
#define MCHNG 2
#define KCHNG 3

_WORD gl_bvdisk;
_WORD gl_bvhard;
_WORD gl_recd;
_WORD gl_rlen;
FPD *gl_rbuf;
_WORD gl_play;
void *gl_store;
_WORD gl_mx;
_WORD gl_my;


/*
 * AES #10 - appl_init - Registers the application under AES. 
 *
 * Application Init
 */
_WORD ap_init(AES_GLOBAL *pglobal)
{
	_WORD	 pid;
	char	scdir[32];

	pid = rlr->p_pid;

    pglobal->ap_version = AESVERSION;  		/* version number     */
    pglobal->ap_count = MULTITOS ? 0 : 1;   /* # concurrent procs */
    pglobal->ap_id = pid;
    sh_deskf(0, &pglobal->ap_private);
    pglobal->ap_planes = gl_nplanes;
    pglobal->ap_3resv = &D;
	pglobal->ap_bvdisk = gl_bvdisk;
	pglobal->ap_bvdisk = gl_bvhard;

	strcpy(scdir, SCRAP_DIR_NAME);

	scdir[0] = gl_logdrv;			/* set drive letter 	*/
	sc_write(scdir);

	rlr->p_msgtosend = FALSE;
	rlr->p_flags |= AP_OPEN;		/* appl_init() done */

	return pid;
}


