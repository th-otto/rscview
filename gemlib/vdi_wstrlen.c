/*
 *  $Id: vdi_wstrlen.c,v 1.3 2003/02/28 20:46:43 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

/** return the length of a w-string
 *
 *  @param wstr a string with 16 bits per character, null-terminated.
 *
 *  @return the length of the string
 *
 *
 */

short
vdi_wstrlen (const short *wstr)
{
	register short len = 0;
	
	while (*wstr++)
		len++;
	
	return len;
}
