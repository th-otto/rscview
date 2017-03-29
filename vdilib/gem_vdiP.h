/*
 *  $Id: gem_vdiP.h,v 1.11 2009/12/20 14:21:02 alanh Exp $
 */

#ifndef _GEM_VDI_P_
# define _GEM_VDI_P_

#include <portvdi.h>

#ifndef NULL
#  define NULL ((void *)0)
#endif

/* Array sizes in vdi control block */
#define VDI_CNTRLMAX     16		/* max size of vdi_control[] ; actually 15; use 16 to make it long aligned */
#define VDI_INTINMAX   1024		/**< max size of vdi_intin[] */
#define VDI_INTOUTMAX   256		/**< max size of vdi_intout[] */
#define VDI_PTSINMAX    256		/**< max size of vdi_ptsin[] */
#define VDI_PTSOUTMAX   256		/**< max size of vdi_ptsout[] */


#ifdef __GNUC__

/* to avoid "dereferencing type-punned pointer" */
static __inline void **__vdi_intin_ptr(short n, short *vdi_intin)
{
	return ((void**)(vdi_intin   +n));
}
#define vdi_intin_ptr(n)  *__vdi_intin_ptr(n, vdi_intin)

static __inline long *__vdi_intin_long(short n, short *vdi_intin)
{
	return ((long *)(vdi_intin   +n));
}
#define vdi_intin_long(n)  *__vdi_intin_long(n, vdi_intin)

static __inline long *__vdi_intout_long(short n, short *vdi_intout)
{
	return ((long *)(vdi_intout   +n));
}
#define vdi_intout_long(n)  *__vdi_intout_long(n, vdi_intout)

static __inline void **__vdi_intout_ptr(short n, short *vdi_intout)
{
	return ((void **)(vdi_intout   +n));
}
#define vdi_intout_ptr(n)  *__vdi_intout_ptr(n, vdi_intout)

static __inline long *__vdi_ptsout_long(short n, short *vdi_ptsout)
{
	return ((long *)(vdi_ptsout   +n));
}
#define vdi_ptsout_long(n)  *__vdi_ptsout_long(n, vdi_ptsout)

static __inline long *__vdi_ptsin_long(short n, short *vdi_ptsin)
{
	return ((long *)(vdi_ptsin   +n));
}
#define vdi_ptsin_long(n)  *__vdi_ptsin_long(n, vdi_ptsin)

static __inline void **__vdi_control_ptr(short n, short *vdi_control)
{
	return ((void**)(vdi_control   +n));
}
#define vdi_control_ptr(n)  *__vdi_control_ptr(n, vdi_control)

#else

#define vdi_control_ptr(n)   *((void**)(vdi_control +n))
#define vdi_intin_ptr(n)     *((void**)(vdi_intin   +n))
#define vdi_intin_long(n)     *((long*)(vdi_intin   +n))
#define vdi_intout_long(n)   *((long*) (vdi_intout  +n))
#define vdi_intout_ptr(n)   *((void**) (vdi_intout  +n))
#define vdi_ptsout_long(n)   *((long*) (vdi_ptsout  +n))
#define vdi_ptsin_long(n)   *((long*) (vdi_ptsin  +n))

#endif


#define VDI_TRAP_ESC(vdipb, handle, opcode, subop, nptsin, nintin) \
	vdi_control[0] = opcode;  \
	vdi_control[1] = nptsin; \
	vdi_control[2] = 0; \
	vdi_control[3] = nintin; \
	vdi_control[4] = 0; \
	vdi_control[5] = subop;   \
	vdi_control[6] = handle;  \
	vdi (&vdipb);

#define VDI_TRAP_00(vdipb, handle, opcode) \
	VDI_TRAP_ESC (vdipb, handle, opcode, 0, 0, 0)


#define VDI_N_PTSOUT vdi_control[2]
#define VDI_N_INTOUT vdi_control[4]

#define VDI_TRAP(vdipb, handle, opcode, nptsin, nintin) \
	VDI_TRAP_ESC(vdipb, handle, opcode, 0, nptsin, nintin)

#define VDI_PARAMS(_control,_intin,_ptsin,_intout,_ptsout) \
	VDIPB vdi_params;         \
	vdi_params.control = _control;   \
	vdi_params.intin   = _intin;   \
	vdi_params.ptsin   = _ptsin;   \
	vdi_params.intout  = _intout;   \
	vdi_params.ptsout  = _ptsout;


/* special feature for VDI bindings: pointer in parameters (for return values)
 * could be NULL (nice idea by Martin Elsasser against dummy variables) 
 */
#define CHECK_NULLPTR 1

/* special feature for VDI bindings: set VDIPB::intout and VDIPB::ptsout to
 * vdi_dummy array intead of NULL against crashes if some crazy VDI drivers
 * tries to write something in ptsout/intout.
 */ 
#define USE_VDI_DUMMY 1

#if USE_VDI_DUMMY
	/* use dummy array vdi_dummy[] from vdi_dummy.c */
	extern short vdi_dummy[];
#else
	/* replace vdi_dummy in VDIPB by NULL pointer */
	#define vdi_dummy 0L
#endif

# endif /* _GEM_VDI_P_ */
