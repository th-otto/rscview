/*
 *  $Id: pdlg_close.c,v 1.6 2003/04/08 21:28:38 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** closes the window of the print dialog
 *
 *  @param prn_dialog Pointer to management structure
 *  @param x X-coordinates of the window \n
 *         [option CHECK_NULLPTR] x may be NULL
 *  @param y Y-coordinates of the window \n
 *         [option CHECK_NULLPTR] y may be NULL
 *  @param global_aes global AES array
 *
 *  @return 1
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 */

short
mt_pdlg_close(PRN_DIALOG *prn_dialog, short *x, short *y, short *global_aes)
{
	AES_PARAMS(203,0,3,1,0);

	aes_intout[1] = -1;
	aes_intout[2] = -1;

	aes_addrin[0] = prn_dialog;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (x)
#endif
	*x = aes_intout[1];
#if CHECK_NULLPTR
	if (y)
#endif
	*y = aes_intout[2];
	
	return aes_intout[0];
}

short (pdlg_close)(PRN_DIALOG *prn_dialog, short *x, short *y)
{
	return mt_pdlg_close(prn_dialog, x, y, aes_global);
}
