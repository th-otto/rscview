#include "config.h"
#include <gem.h>
#include "portvdi.h"
#include "or_draw.h"
#define RSC_NAMED_FUNCTIONS 1
#define hfix_objs(hdr, ob, num_objs)
#define hrelease_objs(hdr, ob)
#include <fo_al.rsh>

#define FO_NUM_LINES	5
#define FO_NUM_BUTTONS	6
#define FO_MAX_STR_LENG	40
#define FO_MAX_BUT_LENG	20

#define FO_AES_NUM_LINES	5
#define FO_AES_NUM_BUTTONS	3
#define FO_AES_MAX_STR_LENG	30
#define FO_AES_MAX_BUT_LENG	10

#define FO_BUFSIZE	(1 + 4 + FO_NUM_LINES*FO_MAX_STR_LENG + (FO_NUM_LINES-1) + 2 + FO_NUM_BUTTONS*FO_MAX_BUT_LENG + (FO_NUM_BUTTONS-1) + 1 + 1)
/***                 |   |   |                              |                  |   |                                |                    |   |
*                    |   |   |                              |                  |   |                                |                    |   +- string terminator
*                    |   |   |                              |                  |   |                                |                    +- last bracket
*                    |   |   |                              |                  |   |                                +- button separators
*                    |   |   |                              |                  |   +- button strings
*                    |   |   |                              |                  +- separators
*                    |   |   |                              +- string separators
*                    |   |   +- strings
*                    |   +- symbol + separators
*                    +- default
***/


#define FO_NONE     	0
#define FO_HINT     	FO_NONE
#define FO_ATTENTION	1
#define FO_QUESTION     2
#define FO_STOP	        3
#define FO_INFO         4
#define FO_DISK         5
#define FO_ERROR        6
#define FO_QUESTION2    7
#define FO_HALT         8
#define FO_RUBBISH      9

#define FO_MAX_SYM	9

#define X_ABSTAND_BUTTON (2 * wchar)
#define Y_ABSTAND_BUTTON hchar
#define X_ABSTAND_TEXT (wchar + wchar/2)
#define Y_ABSTAND_TEXT hchar

typedef struct _alert_string
{
	_WORD   sym;
	_UBYTE *strtext[FO_NUM_LINES];
	_WORD   str_len[FO_NUM_LINES];
	_WORD   num_strings;
	_WORD   max_str_len;
	_UBYTE *buttext[FO_NUM_BUTTONS];
	_WORD   but_len[FO_NUM_BUTTONS];
	_WORD   num_buttons;
	_WORD   max_but_len;
	_WORD	default_button;
	_WORD	undo_button;
	_UBYTE  strbuf[FO_BUFSIZE];
} ALERT_STRING;

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static OBJECT *WrkTree(_WORD treeNr)
{
	static _BOOL rsc_loaded;

	if (!rsc_loaded)
	{
		_WORD wchar, hchar;
		
		GetTextSize(&wchar, &hchar);
		fo_al_rsc_load(wchar, hchar);
		rsc_loaded = TRUE;
	}
	return rs_trindex[treeNr];
}

/*** ---------------------------------------------------------------------- ***/

static _WORD next_alert_string(_UBYTE **dstp, const _UBYTE **src, _WORD *maxlen)
{
	_WORD len;
	const _UBYTE *cp;
	_UBYTE *dst;

	cp = *src;
	dst = *dstp;
	if (*cp == '\0' || *cp == ']')
	{
		len = -1;
	} else
	{
		len = 0;
		while (*cp != '\0')
		{
			if (*cp == '\\' &&
				(cp[1] == '[' || cp[1] == ']' || cp[1] == '\\' || cp[1] == '|'))
			{
				if ((*maxlen) > 1)
				{
					*dst++ = cp[1];
					(*maxlen)--;
					len++;
				}
				cp += 2;
			} else if (*cp == '|' || *cp == ']')
			{
				if (cp[0] == cp[1])
				{
					if ((*maxlen) > 1)
					{
						*dst++ = cp[1];
						(*maxlen)--;
						len++;
					}
					cp += 2;
				} else
				{
					break;
				}
			} else
			{
				if ((*maxlen) > 1)
				{
					*dst++ = *cp;
					(*maxlen)--;
					len++;
				}
				cp++;
			}
		}
	}
	*dst++ = '\0';
	(*maxlen)--;
	*dstp = dst;
	if (*cp == '|')
		*src = cp + 1;
	else
		*src = cp;
	return len;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL split_alert_string(_WORD def, ALERT_STRING *alert, const _UBYTE *str, EXTOB_MODE mode)
{
	_UBYTE *dst;
	_WORD i;
	_WORD len;
	_WORD maxlen;

	alert->sym = 0;
	alert->num_strings = 0;
	alert->max_str_len = 0;
	alert->num_buttons = 0;
	alert->max_but_len = 0;
	alert->default_button = def;
	alert->undo_button = NIL;
	
	if (*str++ != '[')
		return FALSE;

	{
		_UBYTE sym = *str++;

		if (sym > FO_MAX_SYM + '0')
			sym = FO_MAX_SYM + '0';
		else if (sym < '0')
			sym = '0';
		alert->sym = sym - '0';
	}
	if (*str++ != ']')
		return FALSE;

	if (*str++ != '[')
		return FALSE;
	dst = alert->strbuf;
	maxlen = FO_BUFSIZE;
	for (i = 0; i < FO_NUM_LINES; i++)
	{
		alert->strtext[i] = dst;
		len = next_alert_string(&dst, &str, &maxlen);
		if (len < 0)
			break;
		alert->str_len[i] = len;
		if (len > alert->max_str_len)
			alert->max_str_len = len;
	}
	alert->num_strings = i;
	if (alert->num_strings == 0)
	{
		alert->strtext[0] = dst;
		*dst++ = '\0';
		alert->str_len[0] = 0;
		alert->num_strings = 1;
	}
	if (*str++ != ']')
		return FALSE;

	if (*str++ != '[')
		return FALSE;
	for (i = 0; i < FO_NUM_BUTTONS; i++)
	{
		alert->buttext[i] = dst;
		if (mode == EXTOB_ORCS ||
			mode == EXTOB_MYDIAL ||
			mode == EXTOB_MAGIC)
		{
			if (*str == '.')
			{
				alert->default_button = i + 1;
				str++;
			} else if (*str == ':')
			{
				alert->undo_button = i + 1;
				str++;
			}
		}
		len = next_alert_string(&dst, &str, &maxlen);
		if (len < 0)
			break;
		alert->but_len[i] = len;
		if (maxlen > 5)
		{
			dst += 5;			/* leave room for function key */
			maxlen -= 5;
		}
		if (len > alert->max_but_len)
			alert->max_but_len = len;
	}
	alert->num_buttons = i;
	if (alert->num_buttons == 0)
	{
		alert->buttext[0] = dst;
		*dst++ = '\0';
		alert->but_len[0] = 0;
		alert->num_buttons = 1;
	}
	if (*str++ != ']')
		return FALSE;

	return TRUE;
}

/* ------------------------------------------------------------------------- */

void ob_draw_alert(_WORD vdi_handle, _WORD defbutton, const char *str, const GRECT *gr, EXTOB_MODE mode)
{
	ALERT_STRING info;
	OBJECT *rs_alert;
	OBJECT *icons;
	_WORD nobs, buttons;
	_WORD wchar, hchar;
	OBJECT alert[1			/* box */
				 +       1	/* title */
				 +       1	/* symbol */
				 +       FO_NUM_LINES		/* strings */
				 +       FO_NUM_BUTTONS		/* buttons */
	];
	GRECT al_grect;

	if ((rs_alert = WrkTree(F_ALERT)) == NULL)
		return;
	if ((icons = WrkTree(ICONS)) == NULL)
		return;
	GetTextSize(&wchar, &hchar);
	alert[ROOT] = rs_alert[ROOT];
	alert[ROOT].ob_head = NIL;
	alert[ROOT].ob_tail = NIL;
	nobs = 1;
	split_alert_string(defbutton, &info, str, mode);

	switch (mode)
	{
	case EXTOB_HONKA:
		{
			_WORD txtStartPos, yoff, breiteButton, maxLen;
			_WORD i;
			_WORD idx;

			txtStartPos = X_ABSTAND_TEXT;

			switch (info.sym)
			{
				default:
				case FO_ATTENTION: idx = IC_ICON_ATTENTION; break;
				case FO_QUESTION: idx = IC_ICON_QUESTION; break;
				case FO_STOP: idx = IC_ICON_STOP; break;
				case 4: idx = IC_ICON_ERROR; break;
				case 5: idx = IC_ICON_RUBBISH; break;
				case 6: idx = IC_ICON_DISK; break;
				case 7: idx = IC_ICON_INFO; break;
				case 8: idx = IC_ICON_QUESTION2; break;
				case 9: idx = IC_ICON_HALT; break;
			}
			
			{
				alert[nobs] = rs_alert[FA_TITLE];
				objc_add(alert, ROOT, nobs);
				if (info.sym == 0)
					alert[nobs].ob_spec.free_string = icons[IC_TEXT_TIP].ob_spec.free_string;
				else
					alert[nobs].ob_spec.free_string = icons[IC_TEXT_TIP + 1 + (idx - IC_ICON_ATTENTION)].ob_spec.free_string;
				nobs++;
			}

			if (info.sym != 0)
			{
				alert[nobs] = icons[idx];
				alert[nobs].ob_x = icons[IC_ICON_ATTENTION].ob_x;
				alert[nobs].ob_y = icons[IC_ICON_ATTENTION].ob_y;
				txtStartPos += alert[nobs].ob_width + alert[nobs].ob_x;
				objc_add(alert, ROOT, nobs);
				nobs++;
			}

			yoff = alert[FA_TITLE].ob_y + alert[FA_TITLE].ob_height + Y_ABSTAND_TEXT;

			maxLen = 0;
			for (i = 0; i < info.num_strings; i++)
			{
				alert[nobs] = rs_alert[FA_TEXT + i];
				alert[nobs].ob_spec.tedinfo->te_ptext = info.strtext[i];
				alert[nobs].ob_x = txtStartPos;
				alert[nobs].ob_y = yoff;
				alert[nobs].ob_width = info.str_len[i] * wchar;
				yoff += alert[nobs].ob_height;
				objc_add(alert, ROOT, nobs);
				nobs++;
				{
					_WORD leng = info.str_len[i] * wchar + txtStartPos + X_ABSTAND_TEXT;

					if (leng > maxLen)
						maxLen = leng;
				}
			}

			if (info.sym != 0)
			{
				_WORD hoeheIcon = icons[IC_ICON_ATTENTION].ob_y + icons[IC_ICON_ATTENTION].ob_height;

				if (yoff < hoeheIcon)
					yoff = hoeheIcon;
			}
			yoff += Y_ABSTAND_BUTTON;

			info.max_but_len += 2 + 5;
			breiteButton = (info.max_but_len * wchar + X_ABSTAND_BUTTON) * info.num_buttons - X_ABSTAND_BUTTON + 2 * X_ABSTAND_TEXT;
			if (maxLen < breiteButton)
				maxLen = breiteButton;
			{
				_WORD len = (_WORD) strlen(alert[FA_TITLE].ob_spec.free_string) * wchar + 2 * 24;

				if (maxLen < len)
					maxLen = len;
			}

			alert[ROOT].ob_width = maxLen;
			alert[FA_TITLE].ob_width = maxLen;

			buttons = nobs;
			alert[ROOT].ob_height = yoff + rs_alert[FA_BUTTON].ob_height + Y_ABSTAND_BUTTON;

			for (i = 0; i < info.num_buttons; i++)
			{
				{
					static const _UBYTE *key1[] = { "F8" };
					static const _UBYTE *key2[] = { "F1", "F10" };
					static const _UBYTE *key3[] = { "F1", "F8", "F10" };
					const _UBYTE **useKey;

					switch (info.num_buttons)
					{
						case 1:	useKey = key1; break;
						case 2: useKey = key2; break;
						default: useKey = key3;	break;
					}
					strcat(info.buttext[i], "[");
					strcat(info.buttext[i], useKey[i]);
				}
				alert[nobs] = rs_alert[FA_BUTTON + i];
				alert[nobs].ob_spec.free_string = info.buttext[i];
				alert[nobs].ob_width = info.max_but_len * wchar;
				alert[nobs].ob_x = maxLen - (info.num_buttons - i) * (alert[nobs].ob_width + X_ABSTAND_BUTTON) + X_ABSTAND_BUTTON - X_ABSTAND_TEXT;
				alert[nobs].ob_y = yoff;
				objc_add(alert, ROOT, nobs);
				nobs++;
			}
			if (info.num_buttons == 1)
				info.default_button = 1;
		}
		break;
			
	default:
		{
			_WORD xoff, yoff, left_margin, top_margin;
			_WORD i;
			_WORD minw, minh;

			left_margin = wchar;
			top_margin = hchar / 2;

			if (GetNumColors() >= 16)
			{
				OBSPEC_SET_FRAMESIZE(alert[ROOT].ob_spec, 2);
				left_margin += wchar;
			}
			
			minh = 0;
			xoff = left_margin;
			yoff = top_margin;
			if (info.sym != 0)
			{
				_WORD idx;
				
				switch (info.sym)
				{
					default:
					case FO_ATTENTION: idx = IC_ICON_ATTENTION; break;
					case FO_QUESTION: idx = IC_ICON_QUESTION; break;
					case FO_STOP: idx = IC_ICON_STOP; break;
					case FO_ERROR: idx = IC_ICON_ERROR; break;
					case FO_RUBBISH: idx = IC_ICON_RUBBISH; break;
					case FO_DISK: idx = IC_ICON_DISK; break;
					case FO_INFO: idx = IC_ICON_INFO; break;
					case FO_QUESTION2: idx = IC_ICON_QUESTION2; break;
					case FO_HALT: idx = IC_ICON_HALT; break;
				}
				alert[nobs] = icons[idx];
				alert[nobs].ob_x = xoff;
				alert[nobs].ob_y = yoff;
				xoff += alert[nobs].ob_width + wchar;
				objc_add(alert, ROOT, nobs);
				minh = yoff + alert[nobs].ob_height;
				nobs++;
			}

			for (i = 0; i < info.num_strings; i++)
			{
				alert[nobs].ob_spec.free_string = info.strtext[i];
				alert[nobs].ob_next = NIL;
				alert[nobs].ob_head = NIL;
				alert[nobs].ob_tail = NIL;
				alert[nobs].ob_type = G_STRING;
				alert[nobs].ob_flags = OF_NONE;
				alert[nobs].ob_state = OS_NORMAL;
				alert[nobs].ob_x = xoff;
				alert[nobs].ob_y = yoff;
				alert[nobs].ob_width = info.str_len[i] * wchar;
				alert[nobs].ob_height = hchar;
				yoff += hchar;
				objc_add(alert, ROOT, nobs);
				nobs++;
			}
			if (yoff < minh)
				yoff = minh;
			
			/*
			 * Leerraum zwischen Text und Buttons
			 */
			yoff += hchar;
			
			minw = (info.max_str_len + 1) * wchar + xoff;
			xoff = left_margin;
			buttons = nobs;
			for (i = 0; i < info.num_buttons; i++)
			{
				if (i != 0)
					xoff += 2 * wchar;
				alert[nobs].ob_spec.free_string = info.buttext[i];
				alert[nobs].ob_next = NIL;
				alert[nobs].ob_head = NIL;
				alert[nobs].ob_tail = NIL;
				alert[nobs].ob_type = G_BUTTON;
				alert[nobs].ob_flags = OF_SELECTABLE | OF_EXIT;
				alert[nobs].ob_state = OS_NORMAL;
				alert[nobs].ob_x = xoff;
				alert[nobs].ob_y = yoff;
				alert[nobs].ob_width = (info.max_but_len + 1) * wchar;
				alert[nobs].ob_height = hchar;
				switch (mode)
				{
				case EXTOB_ORCS:
					alert[nobs].ob_state = 0x6d00 + (i << 8);
					alert[nobs].ob_type |= G_OBJX_SHORTCUT << 8;
					alert[nobs].ob_flags |= OF_FL3DACT;
					break;
				case EXTOB_MYDIAL:
				case EXTOB_FLYDIAL:
				case EXTOB_MAGIC:
					alert[nobs].ob_type |= G_OBJX_SHORTCUT << 8;
					alert[nobs].ob_flags |= OF_FL3DACT;
					break;
				default:
					break;
				}
				xoff += alert[nobs].ob_width;
 				objc_add(alert, ROOT, nobs);
				nobs++;
			}
			xoff += left_margin;
			yoff += hchar; /* Platz, den Buttons einnehmen */
			yoff += top_margin;
			
			if (minw < xoff)
			{
				minw = xoff;
			} else
			{
				xoff = (minw - xoff) / 2;
				for (i = buttons; i < nobs; i++)
					alert[i].ob_x += xoff;
			}
			alert[ROOT].ob_width = minw;
			alert[ROOT].ob_height = yoff;
		}
		break;
	}

	alert[nobs - 1].ob_flags |= OF_LASTOB;
	if (info.default_button > 0)
	{
		if (info.default_button > info.num_buttons)
		{
			/* Pling(); */
			info.default_button = 1;
		}
		alert[buttons + info.default_button - 1].ob_flags |= OF_DEFAULT;
	}
	if (info.undo_button > 0 && (mode == EXTOB_ORCS || mode == EXTOB_MYDIAL || mode == EXTOB_MAGIC))
	{
		alert[buttons + info.undo_button - 1].ob_type |= G_OBJX_DEFAULT << 8;
	}
	
	if (mode == EXTOB_ORCS)
		alert[ROOT].ob_flags |= OF_TOUCHEXIT | OF_MOVEABLE;

	form_center_grect(alert, &al_grect);
	if (mode == EXTOB_NONE || mode == EXTOB_AES)
		objc_draw_grect(alert, ROOT, MAX_DEPTH, gr);
	else
		ob_draw_dialog(vdi_handle, alert, ROOT, MAX_DEPTH, gr, mode);
}
