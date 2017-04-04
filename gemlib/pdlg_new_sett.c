/*
 *  $Id: pdlg_new_sett.c,v 1.4 2003/02/28 20:46:41 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** Memory for printer settings is allocated, the structure is initialised 
 *  and returned
 *
 *  @param prn_dialog Pointer to management structure
 *  @param global_aes global AES array
 *
 *  @return Printer settings
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 */

PRN_SETTINGS *
mt_pdlg_new_settings(PRN_DIALOG *prn_dialog, short *global_aes)
{
	AES_PARAMS(205,1,0,1,1);

	aes_intin[0] = 5;
	
	aes_addrin[0] = prn_dialog;

	AES_TRAP(aes_params);
	
	return (PRN_SETTINGS*) aes_addrout[0];
}


PRN_SETTINGS *(pdlg_new_settings)(PRN_DIALOG *prn_dialog)
{
	return mt_pdlg_new_settings(prn_dialog, aes_global);
}
