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
#include "gempd.h"


#define TCHNG 0
#define BCHNG 1
#define MCHNG 2
#define KCHNG 3

uint32_t gl_bvdisk;
uint32_t gl_bvhard;
_WORD gl_recd;
_WORD gl_rlen;
uint32_t *gl_rbuf;
_WORD gl_play;
VEX_MOTV gl_store;
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
	pglobal->ap_bvdisk = gl_bvdisk >> 16;
	pglobal->ap_bvhard = gl_bvhard >> 16;

	strcpy(scdir, SCRAP_DIR_NAME);

	scdir[0] = gl_logdrv;			/* set drive letter 	*/
	sc_write(scdir);

	rlr->p_msgtosend = FALSE;
	rlr->p_flags |= AP_OPEN;		/* appl_init() done */

	return pid;
}


/*
 * AES #19 - appl_exit - Deregister an application from the AES.
 *
 * Application Exit
 */
_WORD ap_exit(void)
{
#if NYI
	mn_clsda();
	if (rlr->p_qindex)
		ap_rdwr(AQRD, rlr->p_pid, rlr->p_qindex, (_WORD *) D.g_valstr);

	all_run();
	release();
#if SUBMENUS
	mn_free(rlr->p_pid);
#endif
#endif
	return TRUE;
}


#if NYI
/*
 * Read the internal process message
 */
_WORD rd_mymsg(void *buffer)
{
	if (rlr->p_msgtosend)				/* there is a message   */
	{
		memcpy(buffer, rlr->p_message, 16);
		rlr->p_msgtosend = FALSE;		/* message is sent  */
		return TRUE;
	}
	return FALSE;
}
#endif


#if NYI
/*
 * AES #11 - appl_read - Receives a message from another application. 
 * AES #12 - appl_write - Send AES message to another application. 
 *
 *	APplication READ or WRITE
 */
_WORD ap_rdwr(_WORD code, _WORD id, _WORD length, _WORD *pbuff)
{
	QPB m;
	
	m.qpb_ppd = fpdnm(NULL, id);
	m.qpb_cnt = length;
	m.qpb_buf = pbuff;
	return ev_block(code, (intptr_t)&m);
}
#endif


#if NYI
/*
 * AES #13 - appl_find - Obtain the application ID number of an AES application. 
 *
 *	APplication FIND
 */
_WORD ap_find(const char *pname)
{
	register AESPD *p;
	char temp[9];

	strlcpy(temp, sizeof(temp), pname);

	p = fpdnm(temp, 0);
	return p ? p->p_pid : -1;
}
#endif


#if NYI
/*
 * AES #14 - appl_tplay - Play back AES events. 
 *
 *	Application Tape Player
 */
void ap_tplay(const uint32_t *pbuff, _WORD length, _WORD scale)
{
	register _WORD i;
	uint32_t code;
	uint32_t data;
	FPD f;
	
	dsptch();							/* dispatch everybody   */
	gl_play = FALSE;
	gl_mx = xrat;
	gl_my = yrat;

	for (i = 0; i < length; i++)
	{
		/* get an event to play */
		code = *pbuff++;
		data = *pbuff++;
		
		/* convert it to machine specific form */

		f.f_code = 0;
		f.f_data = data;
		switch ((_WORD)code)
		{
		case TCHNG:
			ev_timer((f.f_data * 100L) / scale);
			break;
		case MCHNG:
			if (!gl_play)
			{
				/* disconnect the cursor from VDI until the playing is done */
				vex_curv(gl_handle, (VEX_CURV)justretf, &drwaddr);
				vex_motv(gl_handle, (VEX_MOTV)justretf, &gl_store);
			}
			f.f_code = mchange;
			gl_play = TRUE;
			break;
		case BCHNG:
			f.f_code = bchange;
			break;
		case KCHNG:
			f.f_code = kchange;
			break;
		}
		/* play it */
		if (f.f_code)
			forkq(f.f_code, f.f_data);

		/* let someone else hear it and respond */
		dsptch();
	}

	if (gl_play)						/* connect back the mouse */
	{
#if UNLINKED
		_WORD dummy;
		
		/* do a mouse sample mode */
		vsin_mode(gl_handle, 1, 2);
		drawrat(gl_mx, gl_my);
		vsm_locator(gl_handle, gl_mx, gl_my, &dummy, &dummy, &dummy);
#endif
		vex_curv(gl_handle, drwaddr, NULL);
		vex_motv(gl_handle, gl_store, NULL);

#if UNLINKED
		xrat = gl_mx;
		yrat = gl_my;
#endif

		gl_play = FALSE;
	}
}
#endif


#if NYI
/*
 * AES #15 - appl_trecord - Record AES events. 
 *
 *	APplication Tape RECorDer
 */
_WORD ap_trecd(uint32_t *pbuff, _WORD length)
{
	if (length <= 0)
		return 0;
	/* start recording in forker() */
	cli();
	gl_recd = TRUE;
	gl_rlen = length;
	gl_rbuf = pbuff;
	sti();
	/* 1/10 of a second sample rate */
	while (gl_recd)
		ev_timer(100L);
	/* done recording so figure out length */
	cli();
	gl_recd = FALSE;
	length = length - gl_rlen;
	gl_rlen = 0;
	gl_rbuf = NULL;
	sti();
	return length;
}
#endif
