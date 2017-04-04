#include "gem_vdiP.h"

#ifdef __GNUC__

long vq_vgdos(void)
{
	register long x __asm__("%d0");
	
	__asm__ volatile (
		"moveq	#-2,%d0\n\t"
		"trap	#2"
		: "=r"(x)
		:
		: "d1","d2","a0","a1","a2","memory"
	);
	return x;
}

#endif
