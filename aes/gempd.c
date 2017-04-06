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


static _BOOL fapd(const char *pname, _WORD pid, AESPD *ppd)
{
	register _BOOL ret;
	char temp[AP_NAMELEN + 1];

	ret = FALSE;
	temp[AP_NAMELEN] = 0;
	if (pname != NULL)
	{
		memcpy(temp, ppd->p_name, AP_NAMELEN);
		ret = streq(pname, temp);
	} else
	{
		ret = ppd->p_pid == pid;
	}
	return ret;
}


AESPD *fpdnm(const char *pname, _UWORD pid)
{
	_WORD i;

	for (i = 0; i < NUM_PDS; i++)
	{
		if (fapd(pname, pid, &D.g_pd[i]))
			return &D.g_pd[i];
	}
	for (i = 0; i < gl_naccs; i++)
	{
		if (fapd(pname, pid, &gl_pacc[i]->ac_pd))
			return &gl_pacc[i]->ac_pd;
	}
	return NULL;
}


static AESPD *getpd(void)
{
	AESPD *p;

	if (curpid < NUM_PDS)				/* get a new AESPD     */
	{
		p = &D.g_pd[curpid];
		p->p_pid = curpid++;
	} else								/* otherwise get it     */
	{									/* accessory's AESPD list  */
		p = &gl_pacc[gl_naccs]->ac_pd;
		p->p_pid = NUM_PDS + gl_naccs++;
	}

	p->p_uda->u_insuper = 1;
	/* return the pd we got */
	return p;
}


void p_nameit(AESPD *p, const char *pname)
{
	memset(p->p_name, ' ', AP_NAMELEN);
	strscn(pname, p->p_name, '.');
}


AESPD *pstart(PFVOID pcode, const char *pfilespec, intptr_t ldaddr)
{
	AESPD *px;

	/* create process to execute it */
	px = getpd();
	px->p_ldaddr = ldaddr;
	/* copy in name of file */
	p_nameit(px, pfilespec);
	/* cs, ip, use 0 flags  */
	/* psetup(px, pcode); */
	(void) pcode;
	/* link him up */
	px->p_stat = PS_RUN;
	px->p_link = drl;
	drl = px;

	/* fake a switch to it */
	rlr = px;
	cda = px->p_cda;
	
	return px;
}


/* put pd pi into list *root at the end   */
void insert_process(AESPD *pi, AESPD **root)
{
	AESPD *p, *q;

	/* find the end */
	for (p = (q = (AESPD *)root)->p_link; p; p = (q = p)->p_link)
		;
	/* link him in */
	pi->p_link = p;
	q->p_link = pi;
}
