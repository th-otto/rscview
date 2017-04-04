/*
 *  $Id: pdlg_rmv_printers.c,v 1.5 2003/02/28 20:46:41 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** removes the printers installed with mt_pdlg_add_printers() from the binding.
 *
 *  @param prn_dialog Pointer to management structure
 *  @param global_aes global AES array
 *
 *  @return unknown ???
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 *  @note mt_pdlg_remove_printers() must be called before mt_pdlg_delete().
 *
 */

short
mt_pdlg_remove_printers(PRN_DIALOG *prn_dialog, short *global_aes)
{
	AES_PARAMS(205,1,1,1,0);

	aes_intin[0] = 1;

	aes_addrin[0] = prn_dialog;

	AES_TRAP(aes_params);

	return aes_intout[0];
}


short (pdlg_remove_printers)(PRN_DIALOG *prn_dialog)
{
	return mt_pdlg_remove_printers(prn_dialog, aes_global);
}
