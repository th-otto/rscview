#include "aes.h"
#include "gempd.h"
#include "debug.h"

/* max sizes for arrays */
#define C_SIZE 4
#define I_SIZE 16
#define O_SIZE 7
#define AI_SIZE 2
#define AO_SIZE 1

#define OP_CODE pb->control[0]
#define IN_LEN  pb->control[1]
#define OUT_LEN pb->control[2]
#define AIN_LEN pb->control[3]
#define AOUT_LEN pb->control[4]

#define RET_CODE int_out[0]


static int dspcnt;

#undef CONF_WITH_PCGEM
#define CONF_WITH_PCGEM 0

#define dsptch()


#ifdef ENABLE_KDEBUG
static void aestrace(const char *message)
{
	char appname[AP_NAMELEN+1];
	const char *src = rlr->p_name;
	char *dest = appname;

	while (dest < &appname[AP_NAMELEN] && *src != ' ')
		*dest++ = *src++;
	*dest++ = '\0';

	nf_debugprintf("AES: %s: %s\n", appname, message);
}
#else
#define aestrace(a)
#endif


#define AES_PARAMS(opcode,nintin,nintout,naddrin,naddrout) \
	if (IN_LEN < nintin || \
		OUT_LEN < nintout || \
		AIN_LEN < naddrin || \
		AOUT_LEN < naddrout) \
		nf_debugprintf("AES(%d): wrong #parameters\n", opcode)


static _WORD crysbind(AESPB *pb)
{
	AES_GLOBAL *pglobal = (AES_GLOBAL *)pb->global;
	_WORD opcode = OP_CODE;
	const _WORD *int_in = pb->intin;
	_WORD *int_out = pb->intout;
	const void **addr_in = pb->addrin;
	_BOOL unsupported = FALSE;
	_WORD ret = TRUE;
	_LONG timeval;
	intptr_t lbuparm;
	
	switch (opcode)
	{
	/* Application Manager */
	case 10:
		aestrace("appl_init()");
		AES_PARAMS(10,0,1,0,0);
        /* reset dispatcher count to let the app run a while */
        dspcnt = 0;
        ret = ap_init(pglobal);
        break;

#if NYI
	case 11:
		aestrace("appl_read()");
		AES_PARAMS(11,2,1,1,0);
		ret = ap_rdwr(AQRD, int_in[0], int_in[1], (_WORD *)NO_CONST(addr_in[0]));
		break;
#endif

#if NYI
	case 12:
		aestrace("appl_write()");
		AES_PARAMS(12,2,1,1,0);
		ret = ap_rdwr(AQWRT, int_in[0], int_in[1], (_WORD *)NO_CONST(addr_in[0]));
		break;
#endif

#if NYI
	case 13:
		aestrace("appl_find()");
		AES_PARAMS(13,0,1,1,0);
		ret = ap_find((const char *)addr_in[0]);
		break;
#endif

#if NYI
	case 14:
		aestrace("appl_tplay()");
		AES_PARAMS(14,2,1,1,0);
		ap_tplay((const uint32_t *)addr_in[0], int_in[0], int_in[1]);
		break;

	case 15:
		aestrace("appl_trecord()");
		AES_PARAMS(15,1,1,1,0);
		ret = ap_trecd((uint32_t *)NO_CONST(addr_in[0]), int_in[0]);
		break;
#endif

#if CONF_WITH_PCGEM
/*
 * AES #16 - appl_bvset - Set the available logical drives for the file-selector. 
 */
	case 16:
		aestrace("appl_bvset()");
		AES_PARAMS(16,2,1,0,0);
		gl_bvdisk = HW(int_in[0]);
		gl_bvhard = HW(int_in[1]);
		break;

/*
 * AES #18 - appl_xbvset - Set the available logical drives for the file-selector.
 */
	case 18:
		/* distinguish between appl_search() and appl_xbvset() */
		if (IN_LEN == 3)
		{
			aestrace("appl_search()");
			AES_PARAMS(18,1,3,1,0);
			unsupported = TRUE;
		} else
		{
			aestrace("appl_xbvset()");
			AES_PARAMS(18,1,0,0,0);
			switch (int_in[0])
			{
			case 0:
				int_out[1] = gl_bvdisk;
				int_out[2] = gl_bvdisk >> 16;
				int_out[3] = gl_bvhard;
				int_out[4] = gl_bvhard >> 16;
				break;
			case 1:
				gl_bvdisk = (_ULONG)(uintptr_t)addr_in[0];
				gl_bvhard = (_ULONG)(uintptr_t)addr_in[1];
				break;
			}
		}
		break;
#endif

/*
 * AES #17 - appl_yield - Force AES process-switch. 
 */
	/*
	 * was never implemented in TOS GEM, but calling trap #2
	 * with D0 = 201 does the same thing
	 */
	case 17:
		aestrace("appl_yield()");
		AES_PARAMS(17,0,1,0,0);
		dsptch();
		break;

	case 19:
		aestrace("appl_exit()");
		AES_PARAMS(19,0,1,0,0);
		ap_exit();
		break;

	
	case 129:
		aestrace("appl_control()");
		AES_PARAMS(129,2,1,1,0);
		unsupported = TRUE;
		break;

	case 130:
		if (AIN_LEN >= 4)
		{
			aestrace("appl_getinfo_str()");
			AES_PARAMS(130,1,1,4,0);
		} else
		{
			aestrace("appl_getinfo()");
			AES_PARAMS(130,1,5,0,0);
		}
		unsupported = TRUE;
		break;


#if NYI
	/* Event Manager */
	case 20:
		aestrace("evnt_keybd()");
		AES_PARAMS(20,0,1,0,0);
		ret = ev_keybd();
		break;

	case 21:
		aestrace("evnt_button()");
		AES_PARAMS(21,3,5,0,0);
		ret = ev_button(int_in[0], int_in[1], int_in[2], &int_out[1]);
		break;

	case 22:
		aestrace("evnt_mouse()");
		AES_PARAMS(22,5,5,0,0);
		ret = ev_mouse((const MOBLK *)&int_in[0], &int_out[1]);
		break;

	case 23:
		aestrace("evnt_mesag()");
		AES_PARAMS(23,0,1,1,0);
		ret = ev_mesag((_WORD *)NO_CONST(addr_in[0]));
		break;

	case 24:
		aestrace("evnt_timer()");
		AES_PARAMS(24,2,1,0,0);
		ev_timer(MAKE_ULONG(int_in[1], int_in[0]));
		break;

	case 25:
		aestrace("evnt_multi()");
		AES_PARAMS(25,16,7,1,0);
		timeval = 0;
		if (int_in[0] & MU_TIMER)
			timeval = MAKE_ULONG(int_in[15], int_in[14]);
		lbuparm = HW(int_in[1]) | (uint16_t)((int_in[2] << 8) | int_in[3]);
		ret = ev_multi(int_in[0], (MOBLK *)NO_CONST(&int_in[4]), (MOBLK *)NO_CONST(&int_in[9]), timeval, lbuparm, (_WORD *)NO_CONST(addr_in[0]), &int_out[1]);
		break;

	case 26:
		aestrace("evnt_dclick()");
		AES_PARAMS(26,2,1,0,0);
		ret = ev_dclick(int_in[0], int_in[1]);
		break;
#endif

	}
	
	RET_CODE = ret;
		
	UNUSED(addr_in);
	UNUSED(int_in);
	UNUSED(lbuparm);
	UNUSED(timeval);
	
	return ret;
}


_WORD aes(AESPB *pb)
{
	return crysbind(pb);
}
