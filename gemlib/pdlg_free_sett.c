/*
 *  $Id: pdlg_free_sett.c,v 1.5 2003/02/28 20:46:41 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** Releases the memory allocated with mt_pdlg_new_settings().
 *
 *  @param settings Printer settings
 *  @param global_aes global AES array
 *
 *  @return unknown ???
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 */

short
mt_pdlg_free_settings(PRN_SETTINGS *settings, short *global_aes)
{
	AES_PARAMS(205,1,1,1,0);

	aes_intin[0] = 6;

	aes_addrin[0] = settings;

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short (pdlg_free_settings)(PRN_SETTINGS *settings)
{
	return mt_pdlg_free_settings(settings, aes_global);
}
