/*
 *  $Id: pdlg_evnt.c,v 1.6 2003/04/08 21:28:38 a_bercegeay Exp $
 */

#include "gem_aesP.h"
#include "gemx.h"

/** evaluates the passed EVNT structure and calls mt_wdlg_evnt() internally.
 *
 *  @param prn_dialog Pointer to management structure
 *  @param settings Printer settings
 *  @param events Pointer to EVNT structure
 *  @param button Selected button (or 0)
 *         - PDLG_CANCEL (1)   "Cancel" was selected 
 *         - PDLG_OK	 (2)   "OK" was pressed 
 *         .
 *         [option CHECK_NULLPTR] button may be NULL
 *  @param global_aes global AES array
 *
 *  @return 0 (Exit button selected) or 1 (Nothing happened)
 *
 *  @since mt_appl_getinfo(7) give informations on mt_pdlg_xx() functions availability
 *
 *  mt_pdlg_evnt() evaluates the passed EVNT structure and calls mt_wdlg_evnt() 
 *  internally. If one of the exit buttons was activated ("Cancel", "OK", 
 *  "Set", "Mark" or "Options") the function returns a 0 and the button that 
 *  the user has selected is returned in \p button. If the dialog was confirmed, 
 *  then the new printer settings are returned in \p settings.
 *
 */

short 
mt_pdlg_evnt(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, EVNT *events,
             short *button, short *global_aes)
{
	AES_PARAMS(206,0,2,3,0);

	aes_addrin[0] = prn_dialog;
	aes_addrin[1] = settings;
	aes_addrin[2] = events;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (button)
#endif
	*button = aes_intout[1];
	
	return aes_intout[0];
}

short (pdlg_evnt)(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, EVNT *events, short *button)
{
	return mt_pdlg_evnt(prn_dialog, settings, events, button, aes_global);
}
