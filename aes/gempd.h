/*
 * EmuTOS aes
 *
 * Copyright (C) 2002-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMPD_H
#define GEMPD_H

#define pd_index __hidden_aes_pd_index
#define fpdnm __hidden_aes_fpdnm
#define p_nameit __hidden_aes_p_nameit
#define p_setappdir __hidden_aes_p_setappdir
#define pstart __hidden_aes_pstart
#define insert_process __hidden_aes_insert_process


/* returns the AESPD for the given index [NOT the pid] */
VISIBILITY("hidden") AESPD *pd_index(_WORD i);

/* returns the AESPD for the given name, or if pname is NULL, the given pid) */
VISIBILITY("hidden") AESPD *fpdnm(const char *pname, _UWORD pid);

/* name an AESPD from the 8 first chars of the given string, stopping at the
 * first '.' (remove the file extension)
 */
VISIBILITY("hidden") void p_nameit(AESPD *p, const char *pname);

/* set the application directory of an AESPD */
VISIBILITY("hidden") void p_setappdir(AESPD *p, const char *pfilespec);

VISIBILITY("hidden") AESPD *pstart(PFVOID pcode, const char *pfilespec, intptr_t ldaddr);

/* insert the process pi at the end of the process list pointed to by root */
VISIBILITY("hidden") void insert_process(AESPD *pi, AESPD **root);

#endif
