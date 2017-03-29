/*
 *  $Id: vdi_dummy.c,v 1.1 2003/05/11 17:52:21 a_bercegeay Exp $
 */

#include "gem_vdiP.h"

#if USE_VDI_DUMMY
#undef max
#define max(x,y)    (((x)>(y))?(x):(y))
short vdi_dummy[max(max(max(max(VDI_CNTRLMAX, VDI_INTINMAX), VDI_INTOUTMAX), VDI_PTSOUTMAX), VDI_PTSINMAX)];
#endif
