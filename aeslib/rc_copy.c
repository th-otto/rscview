/*
 *  $Id: rc_copy.c,v 1.2 2003/02/28 20:46:41 a_bercegeay Exp $
 */

#include "gem_aesP.h"

/** copy a GRECT structure
 *
 *  @param src
 *  @param dst
 *
 *  @return always 1.
 *
 */
 
void
rc_copy (const GRECT * src, GRECT * dst)
{
	*dst = *src;
}
