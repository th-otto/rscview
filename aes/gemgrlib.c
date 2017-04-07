#include "aes.h"
#include "gem_rsc.h"



/*
 * AES #78 - graf_mouse - Change the appearance of the mouse pointer.
 *
 * Graf mouse
 */
void gr_mouse(_WORD mkind, MFORM *grmaddr)
{
	MFORM omform;

	if ((_UWORD) mkind > USER_DEF)
	{
		switch (mkind)
		{
		case M_OFF:
			gsx_moff();
			break;

		case M_ON:
			gsx_mon();
			break;

#if AESVERSION >= 0x320
		case M_SAVE:						/* save mouse form  */
			rlr->p_mouse = gl_cmform;
			break;

		case M_RESTORE:						/* restore saved mouse form */
			omform = rlr->p_mouse;
			gsx_mfset(&omform);
			break;

		case M_PREV:						/* restore previous mouse form  */
			omform = gl_omform;
			gsx_mfset(&omform);
			break;
#endif
		}
	} else
	{
		if (mkind != USER_DEF)				/* set new mouse form   */
		{
			if (mkind < MICE00 || mkind > MICE07)
				mkind = MICE00;
			grmaddr = (MFORM *)aes_rsc_bitblk[mkind]->bi_pdata;
		}

		gsx_mfset(grmaddr);
	}
}
