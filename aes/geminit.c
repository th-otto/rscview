/*		GEMINIT.C		4/23/84 - 08/14/85		Lee Lorenzen			*/
/*		GEMCLI.C		1/28/84 - 08/14/85		Lee Jay Lorenzen		*/
/*		GEM 2.0 		10/31/85				Lowell Webster			*/
/*		merge High C vers. w. 2.2				8/21/87 		mdf 	*/
/*		fix command tail handling				10/19/87		mdf 	*/

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
#include "gem_rsc.h"


#define CACHE_ON	0x00003919L
#define CACHE_OFF	0x00000808L
#define LONGFRAME	*(_WORD *)(0x59eL)

#define STATIC


#if AESVERSION >= 0x330
_BOOL do_once;
#endif
int32_t gl_vdo;
char **aes_rsc_frstr;
OBJECT **aes_rsc_tree;
intptr_t drawstk;
_BOOL autoexec;					/* autoexec a file ?    */
STATIC char g_autoboot[CMDLEN];
STATIC _WORD g_flag;
_BOOL ctldown;					/* ctrl key down ? */ /* referenced by DESKTOP */

_WORD crt_error;				/* critical error handler semaphore     */

/* set in jbind.s, checked by dispatcher    */
_WORD adeskp[3];				/* desktop colors & backgrounds */
STATIC _WORD awinp[3];			/* window colors & backgrounds */
_UWORD d_rezword;				/* default resolution for sparrow */
char gl_logdrv;


#define Getrez() trp14(4)
#define Blitmode(on) trp14(64, on)
#define VcheckMode(mode) trp14(95, mode)

#define Cconws(x) trap(9, x)
#define Kbshift(a) bios(11, a)


STATIC char aautopath[CMDLEN];


void sh_deskf(_WORD obj, aes_private *priv)
{
	OBJECT *tree;

	tree = aes_rsc_tree[DESKTOP];
	priv->l = tree[obj].ob_spec.index;
}
