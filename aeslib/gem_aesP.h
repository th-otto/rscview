/*
 *  $Id: gem_aesP.h,v 1.9 2009/12/20 14:21:02 alanh Exp $
 */

#ifndef _GEM_AES_P_
# define _GEM_AES_P_

#include <portaes.h>

#ifndef NULL
#  define NULL ((void *)0)
#endif


#define AES_TRAP(aespb) aes(&aespb)

#ifdef __GNUC__

static __inline long *__aes_intout_long(short n, short *aes_intout)
{
	return ((long *)(aes_intout   +n));
}
#define aes_intout_long(n)  *__aes_intout_long(n, aes_intout)

#else

#define aes_intout_long(n)  *((long *)(aes_intout+(n)))

#endif


#define AES_PARAMS(opcode,nintin,nintout,naddrin,naddrout) \
	static short    aes_control[AES_CTRLMAX]={opcode,nintin,nintout,naddrin,naddrout}; \
	short			aes_intin[AES_INTINMAX];			  \
	short			aes_intout[AES_INTOUTMAX];			  \
	long			aes_addrin[AES_ADDRINMAX];			  \
	long			aes_addrout[AES_ADDROUTMAX];		  \
 														  \
	AESPB aes_params;									  \
  	aes_params.control = &aes_control[0];				  \
  	aes_params.global  = &global_aes[0];				  \
  	aes_params.intin   = &aes_intin[0]; 				  \
  	aes_params.intout  = &aes_intout[0];				  \
  	aes_params.addrin  = &aes_addrin[0];				  \
  	aes_params.addrout = &aes_addrout[0]



/* special feature for AES bindings: pointer in parameters (for return values)
 * could be NULL (nice idea by Martin Elsasser against dummy variables) 
 */
#define CHECK_NULLPTR 1


#endif /* _GEM_AES_P_ */
