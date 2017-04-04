/*
 *  $Id: pdlg_delete.c,v 1.5 2003/02/28 20:46:41 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** releases the memory for the print dialog
 *
 *  @param prn_dialog Pointer to management structure
 *  @param global_aes global AES array
 *
 *  @return  1
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 */

short 
mt_pdlg_delete(PRN_DIALOG *prn_dialog, short *global_aes)
{
	AES_PARAMS(201,0,1,1,0);

	aes_addrin[0] = prn_dialog;

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short (pdlg_delete)(PRN_DIALOG *prn_dialog)
{
	return mt_pdlg_delete(prn_dialog, aes_global);
}
