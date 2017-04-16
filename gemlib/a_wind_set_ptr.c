#include "gem_aesP.h"

short wind_set_ptr (short WindowHandle, short What, void *p1, void *p2)
{
	AES_PARAMS(105,6,1,0,0);

	aes_intin[0]                  = WindowHandle;
	aes_intin[1]                  = What;

	*(const char**)(aes_intin +4) = 0;
	*(const void**)(aes_intin +2) = p1;

	switch (What)
	{
	case WF_INFO:
	case WF_NAME:
	case WF_NEWDESK:
		if (sizeof(void *) > 4 && p2 != NULL)
		{
			KINFO(("wind_set_ptr() with 2 ptr not supported on this machine\n"));
			return 0;
		}
		break;
	default:
		KINFO(("wind_set_str() called for type %d\n", What));
		break;
	}
	
	*(const void**)(aes_intin +4) = p2;
	
	AES_TRAP(aes_params);

	return aes_intout[0];
}
