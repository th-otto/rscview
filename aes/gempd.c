/*	GEMPD.C		1/27/84 - 02/09/85	Lee Jay Lorenzen	*/
/*	pstart bugs	2/12/85 - 03/22/85	LKW			*/
/*	Reg Opt		03/09/85		Derek Mui		*/
/*	Move insert_process to GEMDISP	07/12/85	Derek Mui	*/

/*
 *	-------------------------------------------------------------
 *	GEM Application Environment Services		  Version 1.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985			Digital Research Inc.
 *	-------------------------------------------------------------
 */

#include "aes.h"
#include "gemlib.h"
#include "gempd.h"


void p_nameit(AESPD *p, const char *pname)
{
	memset(p->p_name, ' ', AP_NAMELEN);
	strscn(pname, p->p_name, '.');
}
