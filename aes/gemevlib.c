/*		GEMEVLIB.C		1/28/84 - 09/12/85		Lee Jay Lorenzen		*/
/*		merge High C vers. w. 2.2 & 3.0 		8/20/87 		mdf 	*/

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
 *		Copyright (C) 1986						Digital Research Inc.
 *		-------------------------------------------------------------
 */

#include "aes.h"

#define STATIC


static _WORD const gl_dcrates[5] = { 450, 330, 275, 220, 165 };

STATIC _WORD gl_dcindex;
/*
 * # of ticks to wait to see if a second click will occur
 */
_WORD gl_dclick;
_WORD gl_ticktime;



#if NYI
/*
 *	Stuff the return array with the mouse x, y, button, and keyboard state.
 */
static void ev_rets(_WORD *lrets)
{
	if (mtrans)
	{
		lrets[0] = pr_xrat;
		lrets[1] = pr_yrat;
	} else
	{
		lrets[0] = xrat;
		lrets[1] = yrat;
	}

	lrets[2] = tbutton;
	lrets[3] = kstate;
	mtrans = 0;
}


/*
 *	Routine to block for a certain async event and return a single return code.
 */
_WORD ev_block(_WORD code, intptr_t lvalue)
{
	EVSPEC msk;

	mwait(msk = iasync(code, lvalue));
	return apret(msk);
}


/*
 * AES #20 - evnt_keybd - Wait for a keyboard event
 *
 *	Wait for a key to be ready at the keyboard and return it. 
 */
_UWORD ev_keybd(void)
{
	return ev_block(AKBIN, (intptr_t)0);
}


/*
 * AES #21 - evnt_button - Wait for a mouse button event.
 *
 *	Wait for the mouse buttons to reach the state where:
 *		((bmask & (bstate ^ button)) == 0) != bflag
 *	Clicks is how many times to wait for it to reach the state, but the
 *	routine should return how many times it actually reached the state
 *	before some time interval.
 *
 *	High bit of bflgclks determines whether to return when state is
 *	entered or left. This is called bflag.
 *	The default case is bflag = 0 and we are waiting to ENTER the
 *	indicated state. If bflag = 1 then we are waiting to LEAVE the state.
 */
_UWORD ev_button(_WORD bflgclks, _UWORD bmask, _UWORD bstate, _WORD *lrets)
{
	_WORD ret;
	uint32_t parm;

	parm = HIWORD(bflgclks) | (uint16_t)((bmask << 8) | bstate);
	ret = ev_block(ABUTTON, parm);
	ev_rets(lrets);

	return ret;
}


/*
 * AES #22 - evnt_mouse - Wait for the mouse pointer to enter or leave a specified area of the screen.
 *
 *	Wait for the mouse to leave or enter a specified rectangle.
 */
_UWORD ev_mouse(const MOBLK *pmo, _WORD *lrets)
{
	_WORD ret;

	ret = ev_block(AMOUSE, (intptr_t)pmo);
	ev_rets(lrets);
	lrets[2] = button;

	return ret;
}


/*
 * AES #23 - evnt_mesag - Wait for an AES message.
 *
 *	Wait for a message to be received in applications message pipe.
 *	Then read it into pbuff.
 */
_WORD ev_mesag(_WORD *pbuff)
{
#if AESVERSION > 0x320
	if (rlr->p_qindex > 0)
	{
		return ap_rdwr(AQRD, rlr->p_pid, 16, pbuff);
	} else
#endif
	{
		if (!rd_mymsg(pbuff))
			return ap_rdwr(AQRD, rlr->p_pid, 16, pbuff);
	}

	return TRUE;
}


/*
 * AES #24 - evnt_timer - Wait for a given time interval. 
 *
 *	Wait the specified time to be completed.
 */
_WORD ev_timer(_LONG count)
{
	return ev_block(ADELAY, count / gl_ticktime);
}


/*
 *	Used by ev_multi() to check on mouse rectangle events
 */
static _WORD ev_mchk(MOBLK *pmo)
{
	if (rlr == gl_mowner && pmo->m_out != inside(xrat, yrat, &pmo->m_gr))
		return TRUE;
	return FALSE;
}


/*
 * AES #25 - evnt_multi - Wait for an arbitrary event.
 *
 *	Do a multi-wait on the specified events.
 */
_WORD ev_multi(_WORD flags, MOBLK *pmo1, MOBLK *pmo2, _LONG tmcount, intptr_t buparm, _WORD *mebuff, _WORD *prets)
{
	QPB m;
	EVSPEC wmask, kbmsk, bumsk, m1msk, m2msk, qrmsk, tmmsk;
	EVSPEC which;
	_WORD what;
	CQUEUE *pc;

	/* say nothing has happened yet   */
	what = wmask = kbmsk = bumsk = m1msk = m2msk = qrmsk = tmmsk = 0;

	/* do a pre-check for a keystroke & then clear out the forkq */
	chkkbd();
	forker();

	/* if a character is ready then get it */
	if (flags & MU_KEYBD)
	{
		pc = &rlr->p_cda->c_q;
		if (pc->c_cnt)
		{
			prets[4] = dq(pc);
			what |= MU_KEYBD;
		}
	}

	/* if we own the mouse then do quick checks */
	if (flags & MU_BUTTON)
	{
		/* quick check button */
		if (rlr == gl_mowner)
		{
			if ((mtrans > 1) && downorup(pr_button, buparm))
			{
				tbutton = pr_button;	/* changed */
				what |= MU_BUTTON;
				prets[5] = pr_mclick;
			} else
			{
				if (downorup(button, buparm))
				{
					tbutton = button;	/* changed */
					what |= MU_BUTTON;
					prets[5] = mclick;
				}
			}

			if (mowner(tbutton) != rlr)
				what &= ~MU_BUTTON;
		}
	}
	
	/* quick check mouse rectangle */
	if (flags & MU_M1)
	{
		if (ev_mchk(pmo1))
			what |= MU_M1;
	}
	if (flags & MU_M2)
	{
		if (ev_mchk(pmo2))
			what |= MU_M2;
	}

	/* quick check timer */
	if (flags & MU_TIMER)
	{
		if (wmask == 0 && tmcount == 0)
			what |= MU_TIMER;
	}

	/* quick check message */
	if (flags & MU_MESAG)
	{
		if (rlr->p_qindex > 0)
		{
			ev_mesag(mebuff);
			what |= MU_MESAG;
		} else
		{
			if (rd_mymsg(mebuff))
				what |= MU_MESAG;
		}
	}

	/* check for quick out if something has already happened */
	which = 0; /* quiet compiler */
	if (what == 0)
	{
		/* wait for a keystroke */
		if (flags & MU_KEYBD)
			wmask |= kbmsk = iasync(AKBIN, 0L);
		/* wait for a button */
		if (flags & MU_BUTTON)
			wmask |= bumsk = iasync(ABUTTON, buparm);
		/* wait for mouse rect. */
		if (flags & MU_M1)
			wmask |= m1msk = iasync(AMOUSE, (intptr_t)pmo1);
		/* wait for mouse rect. */
		if (flags & MU_M2)
			wmask |= m2msk = iasync(AMOUSE, (intptr_t)pmo2);
		/* wait for message */
		if (flags & MU_MESAG)
		{
			m.qpb_ppd = rlr;
			m.qpb_cnt = 16;
			m.qpb_buf = mebuff;
			wmask |= qrmsk = iasync(AQRD, (intptr_t)&m);
		}
		/* wait for timer */
		if (flags & MU_TIMER)
			wmask |= tmmsk = iasync(ADELAY, tmcount / gl_ticktime);
		/* wait for events */
		which = mwait(wmask);
		/* cancel outstanding events */
		which |= acancel(wmask);
	}
	
	/* get the returns */
	ev_rets(prets);
	if (!(flags & MU_BUTTON))
		prets[2] = button;

	/* do aprets() if something hasn't already happened */
	if (what == 0)
	{
		if (which & kbmsk)
		{
			prets[4] = apret(kbmsk);
			what |= MU_KEYBD;
		}
		if (which & bumsk)
		{
			prets[5] = apret(bumsk);
			prets[2] = tbutton;
			what |= MU_BUTTON;
		}
		if (which & m1msk)
		{
			apret(m1msk);
			what |= MU_M1;
		}
		if (which & m2msk)
		{
			apret(m2msk);
			what |= MU_M2;
		}
		if (which & qrmsk)
		{
			apret(qrmsk);
			what |= MU_MESAG;
		}
		if (which & tmmsk)
		{
			apret(tmmsk);
			what |= MU_TIMER;
		}
	}

	/* return what happened */
	return what;
}


/*
 * AES #26 - evnt_dclick - Obtain or set the time delay between the two clicks of a double-elick.
 */
_WORD ev_dclick(_WORD rate, _WORD setit)
{
	if (setit && (unsigned int)rate < (sizeof(gl_dcrates) / sizeof(gl_dcrates[0])))
	{
		gl_dcindex = rate;
		gl_dclick = gl_dcrates[gl_dcindex] / gl_ticktime;
	}

	return gl_dcindex;
}
#endif
