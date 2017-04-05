/*
 *  $Id: pdlg_get_setsize.c,v 1.4 2003/02/28 20:46:41 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** returns the length of the PRN_SETTINGS structure
 *
 *  @param global_aes global AES array
 *
 *  @return length of the PRN_SETTINGS structure in bytes
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 */

int32_t
mt_pdlg_get_setsize(short *global_aes)
{
	AES_PARAMS(204,1,2,0,0);

	aes_intin[0] = 0;

	AES_TRAP(aes_params);

	return aes_intout_long(0);
}


long (pdlg_get_setsize)(void)
{
	return mt_pdlg_get_setsize(aes_global);
}
