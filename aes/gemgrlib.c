#include "aes.h"

/*
 *	Routine to watch the mouse while the button is down and
 *	it stays inside/outside of the specified rectangle.
 *	Return TRUE as long as the mouse is down.  Block until the
 *	mouse moves into or out of the specified rectangle.
 */

#if 0
_BOOL gr_stilldn(_WORD out, _WORD x, _WORD y, _WORD w, _WORD h)
{
	_BOOL status;
	MOBLK tmpmoblk;

	/* form a MOBLK */
	tmpmoblk.m_out = out;
	r_set(&tmpmoblk.m_gr, x, y, w, h);

	for (;;)
	{
		forker();
		if (!(button & 0x01))
		{
			status = FALSE;
			break;
		} else
		{
			if (out != inside(xrat, yrat, &tmpmoblk.m_gr))
			{
				status = TRUE;
				break;
			}
		}
	}
	return status;
}
#elif 0
_BOOL gr_stilldn(_WORD out, _WORD x, _WORD y, _WORD w, _WORD h)
{
	_WORD rets[6];
	_WORD event;
	MOBLK tmpmoblk;
	
	/* form a MOBLK */
	tmpmoblk.m_out = out;
	r_set(&tmpmoblk.m_gr, x, y, w, h);

	for (;;)
	{
		event = ev_multi(MU_BUTTON | MU_M1 | MU_TIMER, &tmpmoblk, NULL, 0L, combine_cms(1, 1, 0), NULL, &rets[0]);

		if (event & MU_BUTTON)			/* button up */
			return FALSE;

		if (event & MU_M1)
			return TRUE;

		if (event & MU_TIMER)
		{
			if (!(rets[2] & 0x01))
				return FALSE;
		}
	}
}
#else
_BOOL gr_stilldn(_WORD out, _WORD x, _WORD y, _WORD w, _WORD h)
{
	int16_t rets[6];
	int16_t event;
	MOBLK tmpmoblk;

	/* form a MOBLK */
	tmpmoblk.m_out = out;
	r_set(&tmpmoblk.m_gr, x, y, w, h);

	/*
	 * compiler had better put the values out, x, y, w, h in the 
	 * right order on the stack to form a MOBLK WTF
	 */
	event = ev_multi(MU_BUTTON | MU_M1, &tmpmoblk, NULL, 0L, combine_cms(1, 1, 0), NULL, &rets[0]);

	if (event & MU_BUTTON)			/* button up */
		return FALSE;
	return TRUE;
}
#endif
