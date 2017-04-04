/*
 *  $Id: pdlg_rmv_sub_dlgs.c,v 1.5 2003/02/28 20:46:41 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** removes the application's custom sub-dialogs
 *
 *  @param prn_dialog Pointer to management structure
 *  @param global_aes global AES array
 *
 *  @return unknown ???
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 */

short
mt_pdlg_remove_sub_dialogs(PRN_DIALOG *prn_dialog, short *global_aes)
{
	AES_PARAMS(205,1,1,1,0);

	aes_intin[0] = 4;

	aes_addrin[0] = prn_dialog;

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short (pdlg_remove_sub_dialogs)(PRN_DIALOG *prn_dialog)
{
	return mt_pdlg_remove_sub_dialogs(prn_dialog, aes_global);
}
