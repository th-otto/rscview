#include "gem_vdiP.h"

/** 
 *
 *  @param src input string (standard null-terminated C-string)
 *  @param des output string (VDI format, each char occupied 16 bits)
 *
 *  @return the len of the string
 *
 */

short
vdi_str2array (const char *src, short *des)
{
	short len = 0;
	const unsigned char *c  = (const unsigned char *) src;

	while (*c)
	{
		*(des++) = *(c++);
		len++;
	}
	return len;
}


/** 
 *
 *  @param src input string (standard null-terminated C-string)
 *  @param des output string (VDI format, each char occupied 16 bits)
 *  @param len maximum len of the string
 *
 *  @return the len of the string
 *
 */

short
vdi_str2arrayn (const char *src, short *des, short len)
{
	short i = 0;
	const unsigned char *c  = (const unsigned char *) src;

	while (i < len && *c)
	{
		*(des++) = *(c++);
		i++;
	}
	return i;
}
