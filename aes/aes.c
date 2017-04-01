#include "aes.h"

/* max sizes for arrays */
#define C_SIZE 4
#define I_SIZE 16
#define O_SIZE 7
#define AI_SIZE 2
#define AO_SIZE 1


static int dspcnt;


#ifdef ENABLE_KDEBUG
static void aestrace(const char* message)
{
    char appname[AP_NAMELEN+1];
    const char *src = rlr->p_name;
    char *dest = appname;

    while (dest < &appname[AP_NAMELEN] && *src != ' ')
        *dest++ = *src++;
    *dest++ = '\0';

    kprintf("AES: %s: %s\n", appname, message);
}
#else
#define aestrace(a)
#endif


static _WORD crysbind(AESPB *pb)
{
	AES_GLOBAL *pglobal = (AES_GLOBAL *)pb->global;
	_WORD opcode = pb->control[0];
	const _WORD *int_in = pb->intin;
	_WORD *int_out = pb->intout;
	const void **addr_in = pb->addrin;
	_BOOL unsupported = FALSE;
	_WORD ret = TRUE;
	
	switch (opcode)
	{
	case 10:
		aestrace("appl_init()");
        /* reset dispatcher count to let the app run a while */
        dspcnt = 0;
        ret = ap_init(pglobal);
        break;
	}
	
	(void) unsupported;
	(void) int_in;
	(void) addr_in;
	(void) int_out;
	
	return ret;
}


_WORD aes(AESPB *pb)
{
	return crysbind(pb);
}
