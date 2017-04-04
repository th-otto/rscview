/*	GEMBASE.C	1/28/84	- 01/07/85	Lee Jay Lorenzen	*/

/*
 *	-------------------------------------------------------------
 *	GEM Application Environment Services		  Version 1.0
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985			Digital Research Inc.
 *	-------------------------------------------------------------
 */

#include "aes.h"

AESPD *rlr;
AESPD *drl;
AESPD *nrl;

EVB *eul;
EVB *dlr;
EVB *zlr;

char indisp;

_WORD fpt;
_WORD fph;
_WORD fpcnt; /* forkq tail, head, count */

SPB wind_spb;

CDA *cda;

_WORD curpid;