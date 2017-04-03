/*		GEMINPUT.C		1/28/84 - 09/12/85		Lee Jay Lorenzen		*/
/*		merge High C vers. w. 2.2				8/21/87 		mdf 	*/

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
#include "gemlib.h"
#include "gsxdefs.h"

#define STATIC

#define MB_DOWN 0x01


_WORD button;
_WORD xrat;
_WORD yrat;
_WORD kstate;
_WORD mclick;
_WORD mtrans;

_WORD pr_button;
_WORD pr_xrat;
_WORD pr_yrat;
_WORD pr_mclick;


AESPD *gl_mowner;		/* current mouse owner  */
AESPD *gl_kowner;		/* current keybd owner  */
AESPD *gl_cowner;		/* current control rect. owner */

/*
 * screen manager process that controls the mouse when it is
 * outside the control rectangle.
 */
AESPD *ctl_pd;

/* current control rect. */
STATIC GRECT ctrl;


/*
 * # of times into the desired button state
 */
_WORD gl_bclick;

/*
 * the current desired button state
 */
_WORD gl_bdesired;

/*
 * the current true button state
 */
_WORD gl_btrue;

/*
 * number of pending events desiring more than a single click
 */
_WORD gl_bpend;

#if AESVERSION >= 0x330
_WORD gl_button;
#endif

/*
 * the current amount of time before the
 * button event is considered finished
 */
_WORD gl_bdelay;


#if NYI

/*
 * Check if the current click will transfer mouse ownership
 */
AESPD *mowner(_WORD newmask)
{
	_WORD wh;
	register _WORD mx, my;
	AESPD *m;
	GRECT t;

	m = gl_mowner;

	if (newmask == MB_DOWN)
	{
		mx = xrat;
		my = yrat;
		/* if inside ctrl rect then owned by active process */
		if (inside(mx, my, &ctrl))
		{
			m = gl_cowner;
		} else							/* if in menu bar then  */
		{								/* owned by ctrl mgr    */
			m = ctl_pd;
			if (!inside(mx, my, &gl_rmenu))
			{							/* Hit any window?  */
				wh = wm_find(mx, my);
				if (wh && wh != NIL)
				{
					wm_get(wh, WF_WORKXYWH, &t.g_x, NULL);
					m = ctl_pd;
				} else					/* hit window 0 background */
				{
					m = srchwp(0)->w_owner;
				}
			}
		}
	}

	return m;
}


/*
 *	Routine to check if the mouse is in part of the screen owned by
 *	the control manager.  If so, return -1; otherwise return 0 if it
 *	is over the desktop, or +1 if it is over the active window.
 */
_WORD chk_ctrl(_WORD mx, _WORD my)
{
	/* if inside ctrl rect then owned by active process */
	if (inside(mx, my, &ctrl))
		return 1;

	/* if in menu bar then owned by ctrl mgr  */
	if (inside(mx, my, &gl_rmenu))
		return -1;

	/* if on any window beside the desktop then ctrl mgr owns */
	return wm_find(mx, my) ? -1 : 0;
}


/*
 *	Button click code call that is from the button interrupt code
 *	with interrupts off.
 */
void b_click(_WORD state)
{
	/* ignore it unless it represents a change */
	if (state != gl_btrue)
	{
		/* see if we've already set up a wait */
		if (gl_bdelay)
		{
			/* if the change is into the desired state, increment cnt */
			if (state == gl_bdesired)
			{
				gl_bclick++;
				gl_bdelay += 3;
			}
		} else
		{
			/*
			 * if someone cares about multiple clicks and this is not
			 * a null mouse then set up delay else just fork it
			 */
			if (gl_bpend && state)
			{
				/* start click cnt at 1, establish desired state and set wait flag */
				gl_bclick = 1;
				gl_bdesired = state;
				/* button delay set in ev_dclick() */
				gl_bdelay = gl_dclick;
			} else
			{
				forkq(bchange, MAKE_ULONG(state, 1));
			}
		}
		/* update true state of the mouse */
		gl_btrue = state;
	}
}


/*
 *	Button delay code that is called from the tick interrupt code
 *	with interrupts off.
 */
void b_delay(_WORD amnt)
{
	/* see if we have a delay for mouse click in progress */
	if (gl_bdelay)
	{
		/* see if decrementing delay cnt causes delay to be over */
		gl_bdelay -= amnt;
		if (gl_bdelay == 0)
		{
			forkq(bchange, MAKE_ULONG(gl_bdesired, gl_bclick));
			if (gl_bdesired != gl_btrue)
			{
				forkq(bchange, MAKE_ULONG(gl_btrue, 1));
			}
		}
	}
}


/*
 *	Set the current control rectangle which is the part of the screen
 *	owned by the active process.  Normally, the work area of the top window.
 */
void set_ctrl(GRECT *pt)
{
	rc_copy(pt, &ctrl);
}


/*
 *	Get the current control rectangle which is the part of the screen
 *	owned by the active process.  Normally, the work area of the top
 *	window, but sometimes the whole screen during form fill-in.
 */
void get_ctrl(GRECT *pt)
{
	rc_copy(&ctrl, pt);
}


/*
 *	Used by form_do to remember the current keyboard and mouse owners.
 */
void get_mkown(AESPD **pmown)
{
	*pmown = gl_mowner;
}


/*
 *	Used by control manager and form_do() to give the mouse or keyboard
 *	to another process.  The mouse should only be transferred with the
 *	buttons in an up state.
 */
void set_mkown(AESPD *mp, AESPD *kp)
{
	/* change the owner */
	gl_cowner = gl_mowner = mp;

	/*
	 * pretend mouse moved to get the right form showing and
	 * get the mouse event posted correctly
	 */
	post_mouse(gl_mowner, xrat, yrat);

	/*
	 * post a button event in case the new owner was waiting
	 */
	post_button(gl_mowner, button, 1);

	gl_kowner = kp;
}


/*
 *	eNQueue a character on a circular keyboard buffer.
 */
static void nq(_UWORD ch, CQUEUE *qptr)
{
	if (qptr->c_cnt < KBD_SIZE)
	{
		qptr->c_buff[qptr->c_rear++] = ch;
		if (qptr->c_rear == KBD_SIZE)
			qptr->c_rear = 0;
		qptr->c_cnt++;
	}
}


/*
 *	DeQueue a character from a circular keyboard buffer.
 */
_UWORD dq(CQUEUE *qptr)
{
	_WORD q2;

	qptr->c_cnt--;
	q2 = qptr->c_front++;
	if (qptr->c_front == KBD_SIZE)
		qptr->c_front = 0;
	return qptr->c_buff[q2];
}


/*
 *	Flush the characters from a circular keyboard buffer.
 */
void fq(void)
{
	while (cda->c_q.c_cnt)
		dq(&cda->c_q);
}


/*
 * Remove the event from the CDA.
 * ret may have number of clicks
 * or the desired character.
 * evinsert is in the gemasync.c
 */
void evremove(EVB *e, _UWORD ret)
{
	/* unlink this EVB, and apret() */
	e->e_return |= (_UWORD) ret;			/* will remove and free the EVB */
	e->e_pred->e_link = e->e_link;
	if (e->e_link)
		e->e_link->e_pred = e->e_pred;
	zombie(e);
}


void kchange(_LONG fdata)
{
	_UWORD ch = HIWORD(fdata);
	_WORD kstat = LOWORD(fdata);

	kstate = kstat;
	if (ch)
		post_keybd(gl_kowner, ch);
}


void post_keybd(AESPD *p, _UWORD ch)
{
	CDA *c;
	EVB *e;

	c = p->p_cda;
	/* if someone is waiting, wake him up */
	if ((e = c->c_iiowait) != NULL)
	{
		evremove(e, ch);
	} else
	{
		/* no one is waiting, just toss it in the buffer */
		nq(ch, &c->c_q);
	}
}


/*
 * forker will come here
 */
void bchange(_LONG fdata)
{
	_WORD newmask;
	_WORD clicks;
	
	newmask = HIWORD(fdata);
	clicks = LOWORD(fdata);
	/* see if this button event causes an ownership change to or from ctrlmgr */
	if (gl_mowner != ctl_pd)
		gl_mowner = mowner(newmask);

	mtrans++;
	pr_button = button;
	pr_mclick = mclick;
	pr_xrat = xrat;
	pr_yrat = yrat;
	button = newmask;
	mclick = clicks;
	post_button(gl_mowner, button, clicks);
}


_WORD downorup(_WORD newmask, intptr_t buparm)
{
	register _WORD flag, mask, val;

	/*
	 * decode buparm that was encoded by combine_cms()
	 */
	flag = (buparm >> 24) & 0x00ffL;	/* clicks */
	mask = (buparm >> 8) & 0x00ffL;		/* which button */
	val = buparm & 0x00ffL;				/* up or down */
	return ((mask & (val ^ newmask)) == 0) != flag;
}


static _WORD inorout(EVB *e, _WORD rx, _WORD ry)
{
	MOBLK mo;

	/* in or out of specified rectangle */
	mo.m_out = ((e->e_flag & EVMOUT) != 0);
	mo.m_gr.g_x = HIWORD(e->e_parm);
	mo.m_gr.g_y = LOWORD(e->e_parm);
	mo.m_gr.g_w = HIWORD(e->e_return);
	mo.m_gr.g_h = LOWORD(e->e_return);

	return mo.m_out != inside(rx, ry, &mo.m_gr);
}


/*
 *	Routine to walk the list of button events and remove
 *	the ones that are satisfied.
 */
void post_button(AESPD *p, _WORD newmask, _WORD clks)
{
	CDA *c;
	EVB *e1, *e;
	_UWORD clicks;

	c = p->p_cda;
	for (e = c->c_bsleep; e; e = e1)
	{
		e1 = e->e_link;
		if (downorup(newmask, e->e_parm))
		{
			/* decrement counting semaphore if one of the multi-click guys was satisfied */
			clicks = HIWORD(e->e_parm) & 0x00ff;
			if (clicks > 1)
				gl_bpend--;

			e->e_return = HW(newmask);		/* changed */
			evremove(e, clks > clicks ? clicks : clks);
		}
	}
}


/*
 * forker of mouse change
 */
void mchange(_LONG fdata)
{
	_WORD rx1, ry1;
	_WORD rx;
	_WORD ry;
	_WORD dummy;
	
	rx1 = HIWORD(fdata);
	ry1 = LOWORD(fdata);
	/* zero out button wait if mouse moves more then a little */
	vq_mouse(gl_handle, &dummy, &rx, &ry);

	if (gl_bdelay && ((xrat - rx > 2) || (xrat - rx < -2) || (yrat - ry > 2) || (yrat - ry < -2)))
		b_delay(gl_bdelay);
	/* xrat, yrat hold true */
	xrat = rx;
	yrat = ry;
	/* post the event */
	if (gl_play)
	{
		_WORD dummy;
		
		/* do a mouse sample mode */
		vsin_mode(gl_handle, 1, 2);
		drawrat(rx1, ry1);
		vsm_locator(gl_handle, rx1, rx1, &dummy, &dummy, &dummy);
		xrat = rx1;
		yrat = ry1;
	}

	/*
	 * give mouse to screen handler when not button down and
	 * there is an active menu and it will satisfy his event
	 */
	if (!button && gl_mntree && gl_ctwait.m_out != inside(xrat, yrat, &gl_ctwait.m_gr))
		gl_mowner = ctl_pd;

	post_mouse(gl_mowner, xrat, yrat);
}


void wheel_change(_WORD wheel_number, _WORD wheel_amount)
{
	_WORD wh;
	_WORD type;

	/* Ignore the wheel messages if the menu is active */
	if (gl_mowner == ctl_pd)
		return;

	if (wheel_amount > 0)
		type = WA_DNLINE;
	else if (wheel_amount < 0)
		type = WA_UPLINE;
	else
		return;

	/* We have a problem here.
	 * This function is called by forker(), where rlr is set to -1.
	 * As a result, we can't call ap_sendmsg() which internally calls iasync().
	 * Uncomment the following code when some solution has been found.
	 */
/*
	assert(rlr != (AESPD *)-1);
	wh = wm_find(xrat, yrat);
	ap_sendmsg(appl_msg, WM_ARROWED, D.w_win[wh].w_owner, wh, type, 0, 0, 0);
*/
	(void)wh; /* silent warning */
	(void)type; /* silent warning */
	(void)wheel_number;
}


/*
 *	Routine to walk the list of mouse events and remove
 *	the ones that are satisfied.
 */
void post_mouse(AESPD *p, _WORD grx, _WORD gry)
{
	register EVB *e;
	register EVB *e1;

	/* check event list to signal waiting pr's */
	for (e = p->p_cda->c_msleep; e; e = e1)
	{
		e1 = e->e_link;
		if (inorout(e, grx, gry))
			evremove(e, 0);
	}
}


/* wait for keybd input */
void akbin(EVB *e)
{
	/* find vcb to input, point c at it */
	if (cda->c_q.c_cnt)
	{
		/* another satisfied customer */
		e->e_return = dq(&cda->c_q);
		zombie(e);
	} else								/* time to zzzzz... */
	{
		evinsert(e, &cda->c_iiowait);
	}
}


/*
 * wait for timer
 */
void adelay(EVB *e, _LONG c)
{
	EVB *p, *q;

	if (c == 0)
		c = 1;

	cli();
	if (CMP_TICK)
	{
		/*
		 * if already counting down, then reset CMP_TICK to the lower
		 * number, but let NUM_TICK grow from its accumulated value
		 */
		if (c <= CMP_TICK)
			CMP_TICK = c;
	} else
	{
		/*
		 * if we aren't currently counting down for someone else,
		 * then start ticking
		 */
		CMP_TICK = c;
		/* start NUM_TICK out at zero */
		NUM_TICK = 0;
	}

	e->e_flag |= EVDELAY;
	q = (EVB *)((char *) &dlr - offsetof(EVB, e_link));
	for (p = dlr; p; p = (q = p)->e_link)
	{
		if (c <= p->e_parm)
			break;
		c -= p->e_parm;
	}
	e->e_pred = q;
	q->e_link = e;
	e->e_parm = c;
	e->e_link = p;
	if (p)
	{
		c = p->e_parm - c;
		p->e_pred = e;
		p->e_parm = c;
	}
	sti();
}


/*
 * wait for button
 */
void abutton(EVB *e, _LONG p)
{
	register _WORD bclicks;

	if (downorup(button, p))
	{
		e->e_return = HW(button);		/* changed */
		zombie(e);						/* 'nuff said       */
	} else
	{
		/*
		 * increment counting semaphore to show someone cares
		 * about multiple clicks
		 */
		bclicks = HIWORD(p) & 0xff;
		if (bclicks > 1)
			gl_bpend++;
		e->e_parm = p;
		evinsert(e, &cda->c_bsleep);
	}
}


/*
 * wait for mouse rectangle
 */
void amouse(EVB *e, MOBLK *pmo)
{
	MOBLK mob;

	mob = *pmo;

	/* if already in (or out) of rectangle, signal immediately */
	if (mob.m_out != inside(xrat, yrat, &mob.m_gr))
	{
		zombie(e);
	} else
	{
		if (mob.m_out)
			e->e_flag |= EVMOUT;
		else
			e->e_flag &= ~EVMOUT;
		e->e_parm = MAKE_ULONG(mob.m_gr.g_x, mob.m_gr.g_y);
		e->e_return = MAKE_ULONG(mob.m_gr.g_w, mob.m_gr.g_h);
		evinsert(e, &cda->c_msleep);
	}
}

#endif
