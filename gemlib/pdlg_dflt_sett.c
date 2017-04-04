/*
 *  $Id: pdlg_dflt_sett.c,v 1.5 2003/02/28 20:46:41 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** Initialises the structure to which \p settings points.
 *
 *  @param prn_dialog Pointer to management structure
 *  @param settings Printer settings
 *  @param global_aes global AES array
 *
 *  @return 1
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 *  @note Structure length can be found with mt_pdlg_get_setsize().
 *
 */

short
mt_pdlg_dflt_settings(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, short *global_aes)
{
	AES_PARAMS(205,1,1,2,0);

	aes_intin[0] = 7;
	
	aes_addrin[0] = prn_dialog;
	aes_addrin[1] = settings;

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short (pdlg_dflt_settings)(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings)
{
	return mt_pdlg_dflt_settings(prn_dialog, settings, aes_global);
}
