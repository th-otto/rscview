/*
 *  $Id: pdlg_save_dflt_sett.c,v 1.5 2003/02/28 20:46:41 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** 
 *
 *  @param prn_dialog 
 *  @param settings 
 *  @param global_aes global AES array
 *
 *  @return 
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 *
 *
 */

short
mt_pdlg_save_default_settings(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, short *global_aes)
{
	AES_PARAMS(205,1,1,2,0);

	aes_intin[0] = 10;

	aes_addrin[0] = prn_dialog;
	aes_addrin[1] = settings;

	AES_TRAP(aes_params);

	return aes_intout[0];
}


short (pdlg_save_default_settings)(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings)
{
	return mt_pdlg_save_default_settings(prn_dialog, settings, aes_global);
}
