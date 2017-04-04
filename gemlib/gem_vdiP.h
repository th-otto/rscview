/*
 *  $Id: gem_vdiP.h,v 1.11 2009/12/20 14:21:02 alanh Exp $
 */

#ifndef _GEM_VDI_P_
# define _GEM_VDI_P_

#include <stdint.h>

#if defined(__TOS__) || defined(__atarist__)
#  define OS_ATARI 1
#endif

# ifndef _GEMLIB_H_
#  include "mt_gem.h"
# endif

#ifndef NULL
#  define NULL ((void *)0)
#endif

#ifndef NO_CONST
#  ifdef __GNUC__
#	 define NO_CONST(p) __extension__({ union { const void *cs; void *s; } _x; _x.cs = p; _x.s; })
#  else
#	 define NO_CONST(p) ((void *)(p))
#  endif
#endif


/*
 * control[0]: opcode
 * control[1]: # ptsin
 * control[2]: # ptsout
 * control[3]: # intin
 * control[4]: # intput
 * control[5]: sub-opcode for escapes, GDPs etc
 * control[6]: vdi handle
 * control[7-8]: 1st ptr arg
 * control[9-10]: 2nd ptr arg
 * control[11-12]: 3nd ptr arg (vr_transfer_bits)
 */
 
/* Array sizes in vdi control block */
#define VDI_CNTRLMAX     16		/* max size of vdi_control[] ; actually 15; use 16 to make it long aligned */
#define VDI_INTINMAX   1024		/**< max size of vdi_intin[] */
#define VDI_INTOUTMAX   256		/**< max size of vdi_intout[] */
#define VDI_PTSINMAX    256		/**< max size of vdi_ptsin[] */
#define VDI_PTSOUTMAX   256		/**< max size of vdi_ptsout[] */

#define VDI_OPCODE   vdi_control[0]
#define VDI_N_PTSIN  vdi_control[1]
#define VDI_N_PTSOUT vdi_control[2]
#define VDI_N_INTIN  vdi_control[3]
#define VDI_N_INTOUT vdi_control[4]
#define VDI_SUBCODE  vdi_control[5]
#define VDI_HANDLE   vdi_control[6]


#define VDI_NPTRINTS (sizeof(void *) / sizeof(short))


#ifdef __GNUC__

/* to avoid "dereferencing type-punned pointer" */
static __inline int32_t *__vdi_intin_long(short n, short *vdi_intin)
{
	return ((int32_t *)(vdi_intin   +n));
}
#define vdi_intin_long(n)  *__vdi_intin_long(n, vdi_intin)

static __inline int32_t *__vdi_intout_long(short n, short *vdi_intout)
{
	return ((int32_t *)(vdi_intout   +n));
}
#define vdi_intout_long(n)  *__vdi_intout_long(n, vdi_intout)

static __inline void **__vdi_intout_ptr(short n, short *vdi_intout)
{
	return ((void **)(vdi_intout   +n));
}
#define vdi_intout_ptr(n, t)  *((t *)__vdi_intout_ptr(n, vdi_intout))

static __inline int32_t *__vdi_ptsout_long(short n, short *vdi_ptsout)
{
	return ((int32_t *)(vdi_ptsout   +n));
}
#define vdi_ptsout_long(n)  *__vdi_ptsout_long(n, vdi_ptsout)

static __inline int32_t *__vdi_ptsin_long(short n, short *vdi_ptsin)
{
	return ((int32_t *)(vdi_ptsin   +n));
}
#define vdi_ptsin_long(n)  *__vdi_ptsin_long(n, vdi_ptsin)

static __inline void **__vdi_intin_ptr(short n, short *vdi_intin)
{
	return ((void**)(vdi_intin + n * VDI_NPTRINTS));
}
#define vdi_intin_ptr(n, t)  *((t *)__vdi_intin_ptr(n, vdi_intin))

static __inline void **__vdi_control_ptr(short n, short *vdi_control)
{
	return ((void**)(vdi_control + 7 + n * VDI_NPTRINTS));
}
#define vdi_control_ptr(n, t)  *((t *)__vdi_control_ptr(n, vdi_control))

#else

#define vdi_control_ptr(n, t)   *((t *)(vdi_control + 7 + (n) * VDI_NPTRINTS))
#define vdi_intin_ptr(n, t)     *((t *)(vdi_intin + (n) * VDI_NPTRINTS))
#define vdi_intin_long(n)       *((int32_t *)(vdi_intin + (n)))
#define vdi_intout_long(n)      *((int32_t *)(vdi_intout + (n)))
#define vdi_intout_ptr(n, t)    *((t *)(vdi_intout + n * VDI_NPTRINTS))
#define vdi_ptsout_long(n)      *((int32_t *)(vdi_ptsout + n))
#define vdi_ptsin_long(n)       *((int32_t *)(vdi_ptsin + n))

#endif

#if defined(__GNUC__) && defined(__mc68000__)

#if defined(__GNUC_INLINE__) && (__GNUC__ > 2 || __GNUC_MINOR__ > 5)

static inline void
_vdi_trap_esc (VDIPB * vdipb,
               int32_t cntrl_0_1, int32_t cntrl_3, int32_t cntrl_5, short handle)
{
	__asm__ volatile (
		"movea.l	%0,a0\n\t"	/* &vdipb */
		"move.l	a0,d1\n\t"
		"move.l	(a0),a0\n\t"	/* vdipb->control */
		"move.l	%1,(a0)+\n\t"	/* cntrl_0, cntrl_1 */
		"move.l	%2,(a0)+\n\t"	/* cntrl_2, cntrl_3 */
		"move.l	%3,(a0)+\n\t"	/* cntrl_4, cntrl_5 */
		"move.w	%4,(a0)\n\t"	/* handle */
		"move.w	#115,d0\n\t"	/* 0x0073 */
		"trap	#2"
		:
		: "g"(vdipb), "g"(cntrl_0_1), "g"(cntrl_3), "g"(cntrl_5), "g"(handle)
		: "d0", "d1", "d2", "a0", "a1", "a2", "memory", "cc"
	);
}
#define VDI_TRAP_ESC(vdipb, handle, opcode, subop, cntrl_1, cntrl_3) \
	_vdi_trap_esc (&vdipb, (opcode##uL<<16)|cntrl_1, cntrl_3, subop, handle)

static inline void
_vdi_trap_00 (VDIPB * vdipb, int32_t cntrl_0_1, short handle)
{
	__asm__ volatile (
		"movea.l %0,a0\n\t"	/* &vdipb */
		"move.l  a0,d1\n\t"
		"move.l  (a0),a0\n\t"	/* vdipb->control */
		"move.l  %1,(a0)+\n\t"	/* cntrl_0, cntrl_1 */
		"moveq   #0,d0\n\t"
		"move.l  d0,(a0)+\n\t"	/* cntrl_2, cntrl_3 */
		"move.l  d0,(a0)+\n\t"	/* cntrl_4, cntrl_5 */
		"move.w  %2,(a0)\n\t"	/* handle */
		"move.w  #115,d0\n\t"	/* 0x0073 */
		"trap    #2"
		:
		: "g"(vdipb), "g"(cntrl_0_1), "g"(handle)
		: "d0", "d1", "d2", "a0", "a1", "a2", "memory", "cc"
	);
}
#define VDI_TRAP_00(vdipb, handle, opcode) \
	_vdi_trap_00 (&vdipb, (opcode##uL<<16), handle)


#else /* no usage of gnu inlines, go the old way */

#define VDI_TRAP_ESC(vdipb, handle, opcode, subop, nptsin, nintin) \
	VDI_OPCODE     = opcode;  \
	VDI_N_PTSIN    = nptsin; \
	VDI_N_PTSOUT   = 0; \
	VDI_N_INTIN    = nintin; \
	VDI_N_PTSIN    = 0; \
	VDI_SUBCODE    = subop;   \
	VDI_HANDLE     = handle;  \
	vdi (&vdipb);

#define VDI_TRAP_00(vdipb, handle, opcode) \
	VDI_TRAP_ESC (vdipb, handle, opcode, 0, 0, 0)

#endif

#else

/*
 * not GNU-C, or not m68k, must provide external entry point
 */
void vditrap(VDIPB *vdipb);

#define VDI_TRAP_ESC(vdipb, handle, opcode, subop, nptsin, nintin) \
	VDI_OPCODE     = opcode;  \
	VDI_N_PTSIN    = nptsin; \
	VDI_N_PTSOUT   = 0; \
	VDI_N_INTIN    = nintin; \
	VDI_N_PTSIN    = 0; \
	VDI_SUBCODE    = subop;   \
	VDI_HANDLE     = handle;  \
	vditrap(&vdipb);

#define VDI_TRAP_00(vdipb, handle, opcode) \
	VDI_TRAP_ESC (vdipb, handle, opcode, 0, 0, 0)

#endif


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
