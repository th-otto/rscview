#include "aes.h"
#include "gempd.h"
#include "debug.h"
#include "gem_rsc.h"

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
		AOUT_LEN < naddrout) { \
		nf_debugprintf("AES(%d): wrong #parameters\n", opcode); \
	}


static _WORD crysbind(AESPB *pb)
{
	AES_GLOBAL *pglobal = (AES_GLOBAL *)pb->global;
	_WORD opcode = OP_CODE;
	const _WORD *int_in = pb->intin;
	_WORD *int_out = pb->intout;
	void **addr_in = pb->addrin;
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
        if (rlr == NULL)
        	aes_init();
        ret = ap_init(pglobal);
        break;

	case 11:
		aestrace("appl_read()");
		AES_PARAMS(11,2,1,1,0);
		break;

	case 12:
		aestrace("appl_write()");
		AES_PARAMS(12,2,1,1,0);
		break;

	case 13:
		aestrace("appl_find()");
		AES_PARAMS(13,0,1,1,0);
		break;

	case 14:
		aestrace("appl_tplay()");
		AES_PARAMS(14,2,1,1,0);
		break;

	case 15:
		aestrace("appl_trecord()");
		AES_PARAMS(15,1,1,1,0);
		break;

	case 16:
		aestrace("appl_bvset()");
		AES_PARAMS(16,2,1,0,0);
		break;

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

	case 17:
		aestrace("appl_yield()");
		AES_PARAMS(17,0,1,0,0);
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


	/* Event Manager */
	case 20:
		aestrace("evnt_keybd()");
		AES_PARAMS(20,0,1,0,0);
		break;

	case 21:
		aestrace("evnt_button()");
		AES_PARAMS(21,3,5,0,0);
		break;

	case 22:
		aestrace("evnt_mouse()");
		AES_PARAMS(22,5,5,0,0);
		break;

	case 23:
		aestrace("evnt_mesag()");
		AES_PARAMS(23,0,1,1,0);
		break;

	case 24:
		aestrace("evnt_timer()");
		AES_PARAMS(24,2,1,0,0);
		break;

	case 25:
		aestrace("evnt_multi()");
		AES_PARAMS(25,16,7,1,0);
		break;

	case 26:
		aestrace("evnt_dclick()");
		AES_PARAMS(26,2,1,0,0);
		break;


	/* Menu Manager */
	case 30:
		aestrace("menu_bar()");
		AES_PARAMS(30,1,1,1,0);
		break;

	case 31:
		aestrace("menu_icheck()");
		AES_PARAMS(31,2,1,1,0);
		break;

	case 32:
		aestrace("menu_ienable()");
		AES_PARAMS(32,2,1,1,0);
		break;

	case 33:
		aestrace("menu_tnormal()");
		AES_PARAMS(33,2,1,1,0);
		break;

	case 34:
		aestrace("menu_text()");
		AES_PARAMS(34,1,1,2,0);
		break;

	case 35:
		aestrace("menu_register()");
		AES_PARAMS(35,1,1,1,0);
		break;

	case 36:
		/* distinguish between menu_unregister() and menu_popup() */
		if (IN_LEN == 1)
		{
			aestrace("menu_unregister()");
			AES_PARAMS(36,1,1,0,0);
#if CONF_WITH_PCGEM
			mn_unregister(int_in[0]);
#else
			unsupported = TRUE;
#endif
		} else
		{
			aestrace("menu_popup()");
			AES_PARAMS(36,2,1,2,0);
			unsupported = TRUE;
		}
		break;

	case 37:
		/* distinguish between menu_click() and menu_attach() */
		/*
		 * although menu_click() is PC-GEM only, it's always
		 * enabled because the desktop uses it.
		 */
		if (AIN_LEN == 0)
		{
			aestrace("menu_click()");
			AES_PARAMS(37,2,1,0,0);
			if (int_in[1])
				gl_mnclick = int_in[0];
			ret = gl_mnclick;
		} else
		{
			aestrace("menu_attach()");
			AES_PARAMS(37,2,1,2,0);
			unsupported = TRUE;
		}
		break;

	case 38:
		aestrace("menu_istart()");
		AES_PARAMS(38,3,1,1,0);
		unsupported = TRUE;
		break;

	case 39:
		aestrace("menu_settings()");
		AES_PARAMS(39,1,1,1,0);
		unsupported = TRUE;
		break;


	/* Object Manager */
	case 40:
		aestrace("objc_add()");
		AES_PARAMS(40,2,1,1,0);
		ob_add((OBJECT *)addr_in[0], int_in[0], int_in[1]);
		break;

	case 41:
		aestrace("objc_delete()");
		AES_PARAMS(41,1,1,1,0);
		ob_delete((OBJECT *)addr_in[0], int_in[0]);
		break;

	case 42:
		aestrace("objc_draw()");
		AES_PARAMS(42,6,1,1,0);
		gsx_sclip((const GRECT *)&int_in[2]);
		ob_draw((OBJECT *)addr_in[0], int_in[0], int_in[1]);
		break;

	case 43:
		aestrace("objc_find()");
		AES_PARAMS(43,4,1,1,0);
		ret = ob_find((OBJECT *)addr_in[0], int_in[0], int_in[1], int_in[2], int_in[3]);
		break;

	case 44:
		aestrace("objc_offset()");
		AES_PARAMS(44,1,3,1,0);
		if (gl_aes3d)
			ob_gclip((OBJECT *)addr_in[0], int_in[0], &int_out[1], &int_out[2], &int_out[3], &int_out[4], &int_out[5], &int_out[6]);
		else
			ob_offset((OBJECT *)addr_in[0], int_in[0], &int_out[1], &int_out[2]);
		break;

	case 45:
		aestrace("objc_order()");
		AES_PARAMS(45,2,1,1,0);
		ob_order((OBJECT *)addr_in[0], int_in[0], int_in[1]);
		break;

	case 46:
		aestrace("objc_edit()");
		AES_PARAMS(46,4,2,1,0);
		int_out[1] = int_in[2];
		ret = ob_edit((OBJECT *)addr_in[0], int_in[0], int_in[1], &int_out[1], int_in[3]);
		break;

	case 47:
		aestrace("objc_change()");
		AES_PARAMS(47,8,1,1,0);
		gsx_sclip((const GRECT *)&int_in[2]);
		ob_change((OBJECT *)addr_in[0], int_in[0], int_in[6], int_in[7]);
		break;

	case 48:
		aestrace("objc_sysvar()");
		AES_PARAMS(48,4,3,0,0);
		if (gl_aes3d)
			ret = ob_sysvar(int_in[0], int_in[1], int_in[2], int_in[3], &int_out[1], &int_out[2]);
		break;

	case 49:
		aestrace("objc_xfind()");
		AES_PARAMS(49,4,1,1,0);
		ret = ob_find((OBJECT *)addr_in[0], int_in[0], int_in[1], int_in[2], int_in[3]);
		break;
	

	/* Form Manager */
	case 50:
		aestrace("form_do()");
		AES_PARAMS(50,1,1,1,0);
#if NYI
		ret = fm_do((OBJECT *)addr_in[0], int_in[0]);
#endif
		break;	

	case 51:
		aestrace("form_dial()");
		AES_PARAMS(51,9,1,0,0);
#if NYI
		fm_dial(int_in[0], (const GRECT *)&int_in[1], (const GRECT *)&int_in[5]);
#endif
		break;

	case 52:
		aestrace("form_alert()");
		AES_PARAMS(52,1,1,1,0);
		ret = fm_alert(int_in[0], (const char *)addr_in[0]);
		break;

	case 53:
		aestrace("form_error()");
		AES_PARAMS(53,1,1,0,0);
		ret = fm_error(int_in[0]);
		break;

	case 54:
		aestrace("form_center()");
		AES_PARAMS(54,0,5,1,0);
		ob_center((OBJECT *)addr_in[0], (GRECT *)&int_out[1]);
		break;

	case 55:
		aestrace("form_keybd()");
		AES_PARAMS(55,3,3,1,0);
		gsx_sclip(&gl_rfull);
		int_out[2] = int_in[1];
		int_out[1] = int_in[2];
		ret = fm_keybd((OBJECT *)addr_in[0], int_in[0], &int_out[2], &int_out[1]);
		break;

	case 56:
		aestrace("form_button()");
		AES_PARAMS(56,2,2,1,0);
		gsx_sclip(&gl_rfull);
		ret = fm_button((OBJECT *)addr_in[0], int_in[0], int_in[1], &int_out[1]);
		break;
	

	/* Graphics Manager */
	case 70:
		aestrace("graf_rubberbox()");
		AES_PARAMS(70,4,3,0,0);
#if NYI
		gr_rubbox(int_in[0], int_in[1], int_in[2], int_in[3], &int_out[1], &int_out[2]);
#endif
		break;

	case 71:
		aestrace("graf_dragbox()");
		AES_PARAMS(71,8,3,0,0);
#if NYI
		gr_dragbox(int_in[0], int_in[1], int_in[2], int_in[3], (const GRECT *)&int_in[4], &int_out[1], &int_out[2]);
#endif
		break;

	case 72:
		aestrace("graf_movebox()");
		AES_PARAMS(72,6,1,0,0);
#if NYI
		gr_movebox(int_in[0], int_in[1], int_in[2], int_in[3], int_in[4], int_in[5]);
#endif
		break;

	case 73:
		aestrace("graf_growbox()");
		AES_PARAMS(73,8,1,0,0);
#if NYI
		gr_growbox((const GRECT *)&int_in[0], (const GRECT *)&int_in[4]);
#endif
		break;

	case 74:
		aestrace("graf_shrinkbox()");
		AES_PARAMS(74,8,1,0,0);
#if NYI
		gr_shrinkbox((const GRECT *)&int_in[0], (const GRECT *)&int_in[4]);
#endif
		break;

	case 75:
		aestrace("graf_watchbox()");
		AES_PARAMS(75,4,1,1,0);
#if NYI
		ret = gr_watchbox((OBJECT *)addr_in[0], int_in[1], int_in[2], int_in[3]);
#endif
		break;

	case 76:
		aestrace("graf_slidebox()");
		AES_PARAMS(76,3,1,1,0);
#if NYI
		ret = gr_slidebox((OBJECT *)addr_in[0], int_in[0], int_in[1], int_in[2]);
#endif
		break;

	case 77:
/*
 * AES #77 - graf_handle - Obtain the VDI handle of the AES workstation. 
 */
		aestrace("graf_handle()");
		AES_PARAMS(77,0,5,0,0);
		int_out[1] = gl_wchar;
		int_out[2] = gl_hchar;
		int_out[3] = gl_wbox;
		int_out[4] = gl_hbox;
		ret = gl_handle;
		break;

	case 78:
		aestrace("graf_mouse()");
		AES_PARAMS(78,1,1,1,0);
		ctlmouse(FALSE);
		gr_mouse(int_in[0], (MFORM *)addr_in[0]);
		ctlmouse(TRUE);
		break;

	case 79:
		aestrace("graf_mkstate()");
		AES_PARAMS(79,0,5,0,0);
#if NYI
		ret = gr_mkstate(&int_out[1], &int_out[2], &int_out[3], &int_out[4]);
#endif
		break;
	

	/* Scrap Manager */
	case 80:
		aestrace("scrap_read()");
		AES_PARAMS(80,0,1,1,0);
#if NYI
		ret = sc_read((char *)addr_in[0]);
#endif
		break;

	case 81:
		aestrace("scrap_write()");
		AES_PARAMS(81,0,1,1,0);
#if NYI
		ret = sc_write((const char *)addr_in[0]);
#endif
		break;

	case 82:
		aestrace("scrap_clear()");
		AES_PARAMS(82,0,1,0,0);
#if CONF_WITH_PCGEM
		ret = sc_clear();
#endif
		break;
	

	/* File Selector Manager */
	case 90:
		aestrace("fs_input()");
		AES_PARAMS(90,0,2,2,0);
#if NYI
		ret = fs_input((char *)addr_in[0], (char *)addr_in[1], &int_out[1], (char *)NO_CONST(aes_rsc_string[ITEMSLCT]));
#endif
		break;

	case 91:
		if (AIN_LEN >= 4)
		{
			aestrace("fsel_boxinput()");
		} else
		{
			aestrace("fsel_exinput()");
		}
		AES_PARAMS(91,0,2,3,0);
#if NYI
		ret = fs_input((char *)addr_in[0], (char *)addr_in[1], &int_out[1], (char *)addr_in[2]);
#endif
		break;


	/* Window Manager */
	case 99:
		aestrace("wind_draw()");
		AES_PARAMS(99,2,1,0,0);
		unsupported = TRUE;
		break;

	case 100:
		if (OUT_LEN >= 5)
		{
			aestrace("wind_xcreate()");
		} else
		{
			aestrace("wind_create()");
		}
		AES_PARAMS(100,5,1,0,0);
#if NYI
		ret = wm_create(int_in[0], (const GRECT *)&int_in[1]);
#endif
		break;

	case 101:
		aestrace("wind_open()");
		AES_PARAMS(101,5,1,0,0);
#if NYI
		wm_open(int_in[0], (const GRECT *)&int_in[1]);
#endif
		break;

	case 102:
		aestrace("wind_close()");
		AES_PARAMS(102,1,1,0,0);
#if NYI
		wm_close(int_in[0]);
#endif
		break;

	case 103:
		aestrace("wind_delete()");
		AES_PARAMS(103,1,1,0,0);
#if NYI
		wm_delete(int_in[0]);
#endif
		break;

	case 104:
		aestrace("wind_get()");
		AES_PARAMS(104,2,5,0,0);
#if NYI
		ret = wm_get(int_in[0], int_in[1], &int_out[1], (const _WORD *)&int_in[2]);
#endif
		break;

	case 105:
		aestrace("wind_set()");
		AES_PARAMS(105,6,1,0,0);
#if NYI
		ret = wm_set(int_in[0], int_in[1], (_WORD *)&int_out[2]);
#endif
		break;

	case 106:
		aestrace("wind_find()");
		AES_PARAMS(106,2,1,0,0);
#if NYI
		ret = wm_find(int_in[0], int_in[1]);
#endif
		break;

	case 107:
		aestrace("wind_update()");
		AES_PARAMS(107,1,1,0,0);
#if NYI
		ret = wm_update(int_in[0]);
#endif
		break;

	case 108:
		aestrace("wind_calc()");
		AES_PARAMS(108,6,5,0,0);
#if NYI
		ret = wm_calc(int_in[0], int_in[1], (const GRECT *)&int_in[2], (GRECT *)&int_out[1]);
#endif
		break;

	case 109:
		aestrace("wind_new()");
		AES_PARAMS(109,0,0,0,0);
#if NYI
		ret = wm_new();
#endif
		break;


	/* Resource Manager */
	case 110:
		aestrace("rsrc_load()");
		AES_PARAMS(110,0,1,1,0);
#if NYI
		ret = rs_load(pglobal, (const char *)addr_in[0]);
#endif
		break;

	case 111:
		aestrace("rsrc_free()");
		AES_PARAMS(111,0,1,0,0);
#if NYI
		ret = rs_free(pglobal);
#endif
		break;

	case 112:
		aestrace("rsrc_gaddr()");
		AES_PARAMS(112,2,1,0,1);
#if NYI
		ret = rs_gaddr(pglobal, int_in[0], int_in[1], &pb->addrout[0]);
#endif
		break;

	case 113:
		aestrace("rsrc_saddr()");
		AES_PARAMS(113,2,1,1,0);
#if NYI
		ret = rs_saddr(pglobal, int_in[0], int_in[1], addr_in[0]);
#endif
		break;

	case 114:
		aestrace("rsrc_obfix()");
		AES_PARAMS(114,1,1,1,0);
#if NYI
		ret = rs_obfix((OBJECT *)addr_in[0], int_in[0]);
#endif
		break;

	case 115:
		aestrace("rsrc_rcfix()");
		AES_PARAMS(115,0,1,1,0);
		unsupported = TRUE;
		break;
	

	/* Shell Manager */
	case 120:
		aestrace("shel_read()");
		AES_PARAMS(120,0,1,2,0);
#if NYI
		ret = sh_read((char *)addr_in[0], (char *)addr_in[1]);
#endif
		break;

	case 121:
		aestrace("shel_write()");
		AES_PARAMS(121,3,1,2,0);
#if NYI
		ret = sh_write(int_in[0], int_in[1], int_in[2], (char *)addr_in[0], (char *)addr_in[1]);
#endif
		break;

	case 122:
		aestrace("shel_get()");
		AES_PARAMS(122,1,1,1,0);
#if NYI
		ret = sh_get((char *)addr_in[0], int_in[0]);
#endif
		break;

	case 123:
		aestrace("shel_put()");
		AES_PARAMS(123,1,1,1,0);
#if NYI
		ret = sh_put((char *)addr_in[0], int_in[0]);
#endif
		break;

	case 124:
		aestrace("shel_find()");
		AES_PARAMS(124,0,1,1,0);
#if NYI
		ret = sh_find((char *)addr_in[0]);
#endif
		break;

	case 125:
		aestrace("shel_envrn()");
		AES_PARAMS(125,0,1,2,0);
#if NYI
		ret = sh_envrn((char **)addr_in[0], (char *)addr_in[1]);
#endif
		break;

	case 126:
		aestrace("shel_rdef()");
		AES_PARAMS(126,0,1,2,0);
#if CONF_WITH_PCGEM
		sh_rdef((char *)addr_in[0], (char *)addr_in[0]);
#else
		unsupported = TRUE;
#endif
		break;

	case 127:
		aestrace("shel_wdef()");
		AES_PARAMS(127,0,1,2,0);
#if CONF_WITH_PCGEM
		sh_wdef((const char *)addr_in[0], (const char *)addr_in[0]);
#else
		unsupported = TRUE;
#endif
		break;

	case 128:
		aestrace("shel_help()");
		AES_PARAMS(128,1,1,2,0);
		unsupported = TRUE;
		break;

	default:
		unsupported = TRUE;
		break;
	}
	
	RET_CODE = ret;
		
	UNUSED(addr_in);
	UNUSED(lbuparm);
	UNUSED(timeval);
	
	return ret;
}


_WORD aestrap(AESPB *pb)
{
	return crysbind(pb);
}
