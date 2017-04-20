#ifndef _GEM_AES_P_
# define _GEM_AES_P_

#include <stdint.h>

#if defined(__TOS__) || defined(__atarist__)
#  define OS_ATARI 1
#endif

# ifndef _GEMLIB_H_
#  include "gem.h"
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

/* Array sizes in aes control block */

/** size of the aes_control[] array */
#define AES_CTRLMAX		6		/* actually 5; use 6 to make it long aligned */
/** size of the aes_global[] array */
#define AES_GLOBMAX		(sizeof(AES_GLOBAL) / sizeof(aes_global[0]))
/** size of the aes_intin[] array */
#define AES_INTINMAX 		16
/** size of the aes_intout[] array */
#define AES_INTOUTMAX		16
/** size of the aes_addrin[] array */
#define AES_ADDRINMAX		16
/** size of the aes_addrout[] array */
#define AES_ADDROUTMAX		16

#define N_PTRINTS (sizeof(void *) / sizeof(short))

#if defined(__GNUC__) && defined(__mc68000__) && !defined(PRIVATE_AES)

#define _AES_TRAP(pb) \
	__asm__ volatile ( \
		"move.l	%0,d1\n\t"	/* &aespb */ \
		"move.w	#200,d0\n\t" \
		"trap	#2" \
		: \
		: "g"(pb) \
		: "d0","d1","d2","a0","a1","a2","memory","cc" \
	)

#if defined(__GNUC_INLINE__) && (__GNUC__ > 2 || __GNUC_MINOR__ > 5)

static inline void _aes_trap (AESPB *aespb)
{
	_AES_TRAP(aespb);
}
#define AES_TRAP(aespb) _aes_trap(&aespb)

#else /* no usage of gnu inlines, go the old way */

#define AES_TRAP(aespb) aes(&aespb)

#endif

#else

/*
 * not GNU-C, or not m68k, must provide external entry point
 */
short aestrap(AESPB *pb);

#define _AES_TRAP(aespb) aestrap(aespb)
#define AES_TRAP(aespb) aestrap(&aespb)

#endif


#ifdef __GNUC__

static __inline int32_t *__aes_intout_long(short n, short *aes_intout)
{
	return ((int32_t *)(aes_intout   +n));
}
#define aes_intout_long(n)  *__aes_intout_long(n, aes_intout)

static __inline void **__aes_intout_ptr(short n, short *aes_intout)
{
	return ((void **)(aes_intout   +n));
}
#define aes_intout_ptr(n, t)  *((t *)__aes_intout_ptr(n, aes_intout))

#else

#define aes_intout_long(n)  *((int32_t *)(aes_intout+(n)))

#endif


#define AES_PARAMS(opcode,nintin,nintout,naddrin,naddrout) \
	static short const aes_control[AES_CTRLMAX]={opcode,nintin,nintout,naddrin,naddrout}; \
	short			aes_intin[AES_INTINMAX];			  \
	short			aes_intout[AES_INTOUTMAX];			  \
	void	        *aes_addrin[AES_ADDRINMAX];			  \
	void            *aes_addrout[AES_ADDROUTMAX];		  \
 														  \
	AESPB aes_params;									  \
  	aes_params.control = aes_control;				  \
  	aes_params.global  = aes_global;				  \
  	aes_params.intin   = aes_intin; 				  \
  	aes_params.intout  = aes_intout;				  \
  	aes_params.addrin  = aes_addrin;				  \
  	aes_params.addrout = aes_addrout



/* special feature for AES bindings: pointer in parameters (for return values)
 * could be NULL (nice idea by Martin Elsasser against dummy variables) 
 */
#define CHECK_NULLPTR 1


#endif /* _GEM_AES_P_ */
