/*		GEMMNLIB.C		04/26/84 - 08/14/86 	Lowell Webster			*/
/*		merge High C vers. w. 2.2				8/21/87 		mdf 	*/
/*		fix mn_bar -- bar too wide				11/19/87		mdf 	*/

/*
 *		Copyright 1999, Caldera Thin Clients, Inc.
 *				  2002-2016 The EmuTOS development team
 *
 *		This software is licenced under the GNU Public License.
 *		Please see LICENSE.TXT for further information.
 *
 *				   Historical Copyright
 *		-------------------------------------------------------------
 *		GEM Application Environment Services			  Version 3.0
 *		Serial No.	XXXX-0000-654321			  All Rights Reserved
 *		Copyright (C) 1987						Digital Research Inc.
 *		-------------------------------------------------------------
 */

#include "aes.h"
#include "gempd.h"


#define MTH 1                                   /* menu thickness       */

#define THESCREEN 0
#define THEBAR 1
#define THEACTIVE 2
#define THEMENUS (tree[THESCREEN].ob_tail)

/*** STATE DEFINITIONS FOR menu_state ***********************************/

#define INBAR   1       /* mouse position       outside menu bar & mo dn */
                        /* multi wait           mo up | in menu bar     */
                        /* moves                -> 5  ,  ->2            */

#define OUTTITLE 2      /* mouse position       over title && mo dn     */
                        /* multiwait            mo up | out title rect  */
                        /* moves                -> 5  , ->1 ->2  ->3    */

#define OUTITEM 3       /* mouse position       over item && mo dn      */
                        /* multi wait           mo up | out item rect   */
                        /* moves                -> 5  , ->1 ->2 ->3 ->4 */

#define INBARECT 4      /* mouse position       out menu rect && bar && mo dn*/
                        /* multi wait   mo up | in menu rect | in menu bar */
                        /* moves        -> 5  , -> 3         , -> 2     */


OBJECT *gl_mntree;
AESPD *gl_mnppd;
GRECT gl_rmnactv;
char *desk_acc[MAX_ACCS];
AESPD *desk_ppd[MAX_ACCS];
_WORD gl_dacnt;
_WORD gl_dabase;
_WORD gl_dabox;
_WORD gl_mnclick;

