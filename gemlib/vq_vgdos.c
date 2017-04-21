#include "gem_vdiP.h"

#if defined(__GNUC__) && defined(__m68000__) && !defined(PRIVATE_VDI)

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

#else

extern int _I_dont_care_that_ISO_C_forbids_an_empty_source_file_;

#endif
