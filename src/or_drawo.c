#include "config.h"
#include <gem.h>
#include "portvdi.h"
#include "or_draw.h"
#include "w_draw.h"
#include "rsc_lang.h"

#define OF_POPUP           0x1000
#define is_popup_with_arrows(tree) ((tree)[ROOT].ob_flags & 0x8000)


_LONG (*listbox_get_selected)(OBJECT *tree, _WORD head);
void (*listbox_set_selected)(OBJECT *tree, _WORD head, _LONG val);


static void _obj_draw(OBJECT *tree, _WORD parent, _WORD start, _WORD depth, _WORD x, _WORD y, _BOOL next, RSC_LANG lang, _BOOL for_menu);


/*** ---------------------------------------------------------------------- ***/

static popup_type is_popup(OBJECT *head)
{
	if (objc_mode == EXTOB_ORCS)
	{
		switch (head->ob_type)
		{
		case (G_OBJX_POPUP_SIMPLE << 8) | G_FTEXT:
		case (G_OBJX_POPUP_SIMPLE << 8) | G_FBOXTEXT:
			return POPUP_ORCS_SIMPLE;
		case (G_OBJX_POPUP_SINGLE << 8) | G_FTEXT:
		case (G_OBJX_POPUP_SINGLE << 8) | G_FBOXTEXT:
			return POPUP_ORCS_SINGLE;
		case (G_OBJX_POPUP_MULTIPLE << 8) | G_FTEXT:
		case (G_OBJX_POPUP_MULTIPLE << 8) | G_FBOXTEXT:
			return POPUP_ORCS_MULTIPLE;
		}
		if (head->ob_flags & OF_POPUP)
			return POPUP_ORCS;
	}
	if (objc_mode == EXTOB_HONKA || objc_mode == EXTOB_ORCS)
	{
		switch (head->ob_type)
		{
		case (G_OBJX_LIST << 8) | G_TEXT:
		case (G_OBJX_LIST << 8) | G_FTEXT:
		case (G_OBJX_LIST << 8) | G_BOXTEXT:
		case (G_OBJX_LIST << 8) | G_FBOXTEXT:
			return POPUP_HONKA;
		case (G_OBJX_LIST_BOX << 8) | G_BOXTEXT:
			return POPUP_HONKA_BOX;
		case (G_OBJX_LIST_BOX << 8) | G_BUTTON:
			return POPUP_ORCS_BOX;
		}
	}
	/*
	 * SysGems OBJ_LISTBOX/OBJ_LISTBOX are actually similar,
	 * but we can't handle that here since there is no
	 * object containing the popup
	 */
	return POPUP_NONE;
}

/*** ---------------------------------------------------------------------- ***/

static OBJECT *get_popup_tree(OBJECT *tree, _WORD *head)
{
	switch (is_popup(&tree[*head]))
	{
	case POPUP_ORCS:
	case POPUP_ORCS_SINGLE:
		return tree;
	case POPUP_ORCS_MULTIPLE:
	case POPUP_ORCS_SIMPLE:
		if (tree[*head].ob_head == NIL)
		{
			TEDINFO *ted = tree[*head].ob_spec.tedinfo;
			if (*ted->te_ptext == '\0')
			{
				*head = ROOT;
				return (OBJECT *)(ted->te_pvalid);
			}
			return NULL;
		}
		return tree;
	case POPUP_HONKA:
		tree = (OBJECT *) (tree[*head].ob_spec.tedinfo->te_pvalid);
		*head = ROOT;
		return tree;
	case POPUP_NONE:
	case POPUP_HONKA_BOX:
	default:
		break;
	}
	return NULL;
}

/*** ---------------------------------------------------------------------- ***/

_WORD get_popup_selected(OBJECT *tree, _WORD head, _WORD *valp)
{
	_WORD idx = NIL;
	_WORD val = 0;

	switch (is_popup(&tree[head]))
	{
	case POPUP_ORCS:
	case POPUP_ORCS_SIMPLE:
		tree = get_popup_tree(tree, &head);
		if (tree != NULL)
		{
			idx = tree[head].ob_head;
			if (idx != NIL &&
				tree[idx].ob_head != NIL &&
				idx == tree[head].ob_tail)
			{
				head = idx;
				idx = tree[idx].ob_head;
			}
			while (!(tree[idx].ob_state & OS_CHECKED))
			{
				if (tree[idx].ob_next == head)
					break;
				idx = tree[idx].ob_next;
				val++;
			}
			if (!(tree[idx].ob_state & OS_CHECKED))
			{
				val = 0;
				idx = tree[head].ob_head;
			}
		}
		if (tree && is_popup_with_arrows(tree))
			val--;
		break;

	case POPUP_ORCS_MULTIPLE:
		tree = get_popup_tree(tree, &head);
		if (tree != NULL)
		{
			idx = tree[head].ob_head;
			if (idx != NIL &&
				tree[idx].ob_head != NIL &&
				idx == tree[head].ob_tail)
			{
				head = idx;
				idx = tree[idx].ob_head;
			}
		}
		break;

	case POPUP_ORCS_SINGLE:
		if (listbox_get_selected != FUNK_NULL)
			val = (_WORD)listbox_get_selected(tree, head);
		break;

	case POPUP_HONKA:
		tree = get_popup_tree(tree, &head);
		if (tree != NULL)
		{
			idx = tree[head].ob_head;
			while (!(tree[idx].ob_state & OS_CHECKED))
			{
				if (tree[idx].ob_next == head)
					break;
				idx = tree[idx].ob_next;
				val++;
			}
			if (!(tree[idx].ob_state & OS_CHECKED))
			{
				val = 0;
				idx = tree[head].ob_head;
			}
		}
		break;

	default:
		break;
	}
	if (valp != NULL)
	{
		*valp = val;
	}
	return idx;
}

static void draw_1obj(OBJECT *tree, _WORD idx, _WORD x, _WORD y, _WORD w, _WORD h, _WORD state, RSC_LANG lang, _BOOL for_menu, _BOOL is_root)
{
	OBJECT *obj = &tree[idx];
	LANG_ARRAY arr;
	_UWORD flags = obj->ob_flags;
	OBSPEC obspec = obj->ob_spec;
	
	if (objc_mode == EXTOB_SYSGEM && idx == ROOT)
	{
		if ((obj->ob_type & OBEXTTYPEMASK) == (121 << 8))
		{
			objc_mode = EXTOB_NONE;
		}
		if ((obj->ob_type & OBEXTTYPEMASK) != (120 << 8))
		{
			state = OS_NORMAL;
			flags = OF_NONE;
			switch (obj->ob_type & OBTYPEMASK)
			{
			case G_BOX:
			case G_BOXCHAR:
			case G_IBOX:
			case G_EXTBOX:
				obspec.index = OBSPEC_MAKE('\0', 0, W_PAL_WHITE, W_PAL_WHITE, 0, IP_HOLLOW, W_PAL_WHITE);
				break;
			}
		}
	}
	switch (obj->ob_type & OBTYPEMASK)
	{
#ifdef __TURBOC__
#pragma warn -stv
#endif
	case G_USERDEF:
		switch (obj->ob_type & OBEXTTYPEMASK)
		{
		default:
			od_parm.pb_tree = tree;
			od_parm.pb_obj = idx;
			od_parm.pb_parm = obspec.userblk->ub_parm;
			od_code = obspec.userblk->ub_code;
			if (od_clear)
				od_parm.pb_prevstate = od_oldstate;
			else
				od_parm.pb_prevstate = state;
			TO_draw_obj(x, y, w, h, obj->ob_type, obspec, flags, state, for_menu, is_root);
		}
		break;

	case G_IMAGE:
		TO_draw_obj(x, y, w, h, obj->ob_type & OBTYPEMASK, obspec, flags, state, for_menu, is_root);
		break;

	case G_ICON:
		rsc_lang_split(arr, obspec.iconblk->ib_ptext);
		obspec.iconblk->ib_ptext = arr[lang].start;
		TO_draw_obj(x, y, w, h, obj->ob_type & OBTYPEMASK, obspec, flags, state, for_menu, is_root);
		obspec.iconblk->ib_ptext = arr[0].start;
		rsc_lang_unsplit(arr);
		break;

	case G_CICON:
		if (obspec.ciconblk->monoblk.ib_ptext)
		{
			rsc_lang_split(arr, obspec.ciconblk->monoblk.ib_ptext);
			obspec.ciconblk->monoblk.ib_ptext = arr[lang].start;
			TO_draw_obj(x, y, w, h, obj->ob_type & OBTYPEMASK, obspec, flags, state, for_menu, is_root);
			obspec.ciconblk->monoblk.ib_ptext = arr[0].start;
			rsc_lang_unsplit(arr);
		}
		break;

	case G_STRING:
	case G_BUTTON:
	case G_TITLE:
	case G_SHORTCUT:
		if ((obj->ob_type & OBEXTTYPEMASK) == (G_OBJX_NOLANG << 8))
		{
			TO_draw_obj(x, y, w, h, obj->ob_type, obspec, flags, state, for_menu, is_root);
		} else
		{
			rsc_lang_split(arr, obspec.free_string);
			obspec.free_string = arr[lang].start;
			TO_draw_obj(x, y, w, h, obj->ob_type, obspec, flags, state, for_menu, is_root);
			obspec.free_string = arr[0].start;
			rsc_lang_unsplit(arr);
		}
		break;

	case G_TEXT:
	case G_BOXTEXT:
		if (rsc_lang_split(arr, obspec.tedinfo->te_ptext))
		{
#if 0
			obspec.tedinfo->te_txtlen = (_WORD)strLen(arr[lang].start) + 1;
#endif
		}
		obspec.tedinfo->te_ptext = arr[lang].start;
		TO_draw_obj(x, y, w, h, obj->ob_type, obspec, flags, state, for_menu, is_root);
		obspec.tedinfo->te_ptext = arr[0].start;
		rsc_lang_unsplit(arr);
		break;

	case G_FTEXT:
	case G_FBOXTEXT:
		if (rsc_lang_split(arr, obspec.tedinfo->te_ptmplt))
			obspec.tedinfo->te_tmplen = (_WORD)strlen(arr[lang].start) + 1;
		obspec.tedinfo->te_ptmplt = arr[lang].start;
		TO_draw_obj(x, y, w, h, obj->ob_type, obspec, flags, state, for_menu, is_root);
		obspec.tedinfo->te_ptmplt = arr[0].start;
		rsc_lang_unsplit(arr);
		break;

	case G_BOX:
	case G_EXTBOX:
		switch (is_ext_type(objc_mode, obj->ob_type, flags, state))
		{
		case EXTTYPE_SYSGEM_BAR1:
		case EXTTYPE_SYSGEM_BAR2:
			{
				_WORD next_obj = tree[idx].ob_head;
				if (next_obj == NIL)
					next_obj = tree[idx].ob_next;
				if (next_obj != NIL && next_obj != ROOT && tree[next_obj].ob_type == ((52 << 8) | G_BOX))
					sysgem_draw_bar(x, y, w, h, obj->ob_type, obspec, flags, state, tree[next_obj].ob_width, tree[next_obj].ob_height, tree[next_obj].ob_spec);
				else
					TO_draw_obj(x, y, w, h, obj->ob_type, obspec, flags, state, for_menu, is_root);
			}
			break;
		case EXTTYPE_SYSGEM_BAR3:
			/* placeholder object, drawn already by sysgem_draw_bar */
			break;
		default:
			TO_draw_obj(x, y, w, h, obj->ob_type, obspec, flags, state, for_menu, is_root);
			break;
		}
		break;
		
	default:
		TO_draw_obj(x, y, w, h, obj->ob_type, obspec, flags, state, for_menu, is_root);
		break;
#ifdef __TURBOC__
#pragma warn .stv
#endif
	}
}


/*
 * draw the single object that represents
 * a popup while it is closed
 */
static void popup_draw(OBJECT *tree, _WORD start, _WORD depth, _WORD px, _WORD py, RSC_LANG lang)
{
	_WORD idx;

	if (depth < 0)
		return;
	switch (is_popup(&tree[start]))
	{
	case POPUP_ORCS:
		idx = tree[start].ob_head;
		if (idx != NIL)
		{
			idx = get_popup_selected(tree, start, NULL);
			if (idx == NIL)
				idx = tree[start].ob_head;
			if (idx != NIL)
			{
				draw_1obj(tree, idx, px, py, tree[idx].ob_width, tree[idx].ob_height,
					(tree[idx].ob_state & ~OS_CHECKED) | (tree[start].ob_state & OS_SHADOWED), lang, FALSE, FALSE);
				if (tree[idx].ob_head != NIL)
				{
					_obj_draw(tree, idx, tree[idx].ob_head, depth-1, px, py, TRUE, lang, FALSE);
				}
			}
		} else
		{
			draw_1obj(tree, start, px, py, tree[start].ob_width, tree[start].ob_height, tree[start].ob_state, lang, FALSE, FALSE);
		}
		break;

	case POPUP_ORCS_SIMPLE:
	case POPUP_ORCS_MULTIPLE:
		{
			GRECT gr;
			OBJECT *pop;
			_WORD pop_head;

			pop_head = start;
			pop = get_popup_tree(tree, &pop_head);
			tree += start;
			hdraw_popup_size(od_handle, px, py, tree->ob_width, tree->ob_height,
				tree->ob_spec, tree->ob_flags, tree->ob_state, &gr);
			tree -= start;
			if (pop == NULL || pop[pop_head].ob_head == NIL)
			{
				TEDINFO *ted = tree[start].ob_spec.tedinfo;

				/*
				 * Application error; popup tree has not been
				 * inserted, or was not automatically found;
				 * draw the ROOT object instead
				 */
				draw_text(ted->te_ptext, &gr, ted->te_font, ted->te_just, TRUE, COLSPEC_GET_TEXTCOL(ted->te_color), COLSPEC_GET_TEXTMODE(ted->te_color), -1, FALSE);
				if (pop == NULL)
				{
					gr.g_w--;
					W_Disable_Rect(od_handle, &gr);
				}
			} else
			{
				/*
				 * get the index of the selected object
				 * and draw only that object
				 */
				idx = get_popup_selected(tree, start, NULL);
				if (idx == NIL)
					idx = pop[pop_head].ob_head;
				if (idx != NIL)
				{
					draw_1obj(pop, idx, gr.g_x, gr.g_y, gr.g_w, gr.g_h,
						(pop[idx].ob_state & ~(OS_CHECKED|OS_SHADOWED)), lang, FALSE, FALSE);
					if (pop[idx].ob_head != NIL)
					{
						_obj_draw(pop, idx, pop[idx].ob_head, depth-1, px, py, TRUE, lang, FALSE);
					}
				}
				if (tree[start].ob_state & OS_DISABLED)
				{
					GRECT dis;

					xywh2rect(px, py, tree[start].ob_width, tree[start].ob_height, &dis);
					W_Disable_Rect(od_handle, &dis);
				}
			}
		}
		break;

	case POPUP_HONKA:
		{
			GRECT gr;
			OBJECT *pop;
			_WORD pop_head;

			pop_head = start;
			pop = get_popup_tree(tree, &pop_head);
			if (pop == NULL || pop[pop_head].ob_head == NIL)
			{
				TEDINFO *ted;

				tree += start;
				ted = tree->ob_spec.tedinfo;
				xywh2rect(px, py, tree->ob_width, tree->ob_height, &od_pxy);
				rc_copy(&od_pxy, &od_framerec);
#if 0
				if (mtext_draw != FUNK_NULL)
				{
					MTEDINFO mted;
					TEDINFO *ted = tree->ob_spec.tedinfo;

					(*mtext_init_mted)(&mted, ted);
					(*mtext_set_text)(&mted, ted->te_ptmplt, ted->te_tmplen, ted->te_pvalid, TRUE);
					(*mtext_draw)(od_handle, &mted, GetObjState(tree, 0, OS_SELECTED));
					SetObjState(tree, 0, OS_DISABLED);
				} else
#endif
				{
					char *textedbuf;
					_UWORD color;

					textedbuf = calc_text(ted->te_ptmplt, ted->te_tmplen, ted->te_ptext, ted->te_txtlen, ted->te_pvalid, TRUE, ' ', 0);
					if (textedbuf != NULL)
					{
						color = ted->te_color;
						draw_text(textedbuf, &od_pxy, ted->te_font, ted->te_just, TRUE, COLSPEC_GET_TEXTCOL(color), COLSPEC_GET_TEXTMODE(color), -1, FALSE);
						g_free(textedbuf);
					}
				}
			} else
			{
				idx = get_popup_selected(tree, start, NULL);
				if (idx == NIL)
					idx = pop[pop_head].ob_head;
				tree += start;
				hdraw_list(px, py, tree->ob_width, tree->ob_height,
					tree->ob_spec, tree->ob_flags, tree->ob_state, &gr);
				draw_1obj(pop, idx, gr.g_x, gr.g_y, gr.g_w, gr.g_h, pop[idx].ob_state & ~OS_CHECKED, lang, FALSE, FALSE);
				if (tree->ob_state & OS_DISABLED)
				{
					GRECT dis;

					xywh2rect(px, py, tree->ob_width, tree->ob_height, &dis);
					W_Disable_Rect(od_handle, &dis);
				}
				if (pop[idx].ob_head != NIL)
				{
					_obj_draw(pop, idx, pop[idx].ob_head, depth-1, gr.g_x, gr.g_y, TRUE, lang, FALSE);
				}
			}
		}
		break;
	default:
		break;
	}
}


static void _obj_draw(
	OBJECT *tree,
	_WORD parent,
	_WORD start,
	_WORD depth,
	_WORD px, _WORD py,
	_BOOL next,
	RSC_LANG lang,
	_BOOL for_menu
)
{
	_WORD child;
	_WORD x, y;
	OBJECT *obj;
	popup_type type;

	if (depth < 0)
		return;
	for (;;)
	{
		if (!((obj = &tree[start])->ob_flags & OF_HIDETREE))
		{
			x = px + obj->ob_x;
			y = py + obj->ob_y;
			type = is_popup(obj);
			switch (type)
			{
			case POPUP_NONE:
			case POPUP_ORCS_SIMPLE:
			case POPUP_ORCS_SINGLE:
			case POPUP_ORCS_MULTIPLE:
				draw_1obj(tree, start, x, y, obj->ob_width, obj->ob_height, obj->ob_state, lang, for_menu, parent == NIL);
				break;
			default:
				break;
			}
			od_clear = FALSE;

			switch (type)
			{
			case POPUP_ORCS:
			case POPUP_HONKA:
				popup_draw(tree, start, depth - 1, x, y, lang);
				break;
			case POPUP_ORCS_SIMPLE:
			case POPUP_ORCS_SINGLE:
				popup_draw(tree, start, depth - 1, x, y, lang);
				break;
			case POPUP_ORCS_MULTIPLE:
				popup_draw(tree, start, depth - 1, x, y, lang);
				/* do not draw the child objects */
				break;
			case POPUP_HONKA_BOX:
			case POPUP_ORCS_BOX:
				draw_1obj(tree, start, x, y, obj->ob_width, obj->ob_height, obj->ob_state, lang, FALSE, parent == NIL);
				/* FALL THROUGH */
			default:
				if ((child = obj->ob_head) != NIL)
				{
					switch (is_ext_type(objc_mode, tree[start].ob_type, tree[start].ob_flags, tree[start].ob_state))
					{
					case EXTTYPE_SYSGEM_FRAME:
					case EXTTYPE_SYSGEM_BUTTON:
					case EXTTYPE_SYSGEM_TOUCHEXIT:
					case EXTTYPE_SYSGEM_HELP:
						/*
						 * sysgem library handles this by modifying ob_y, which will move
						 * also all child objects
						 */
						if ((tree[start].ob_type & 0xff00) != 0x400)
						{
							_WORD cw, ch;
							GetTextSize(&cw, &ch);
							x -= 1;
							y -= 2 + (ch / 4);
						}
						break;
					default:
						break;
					}
					_obj_draw(tree, start, child, depth - 1, x, y, TRUE, lang, for_menu);
				}
			}
		}
		start = obj->ob_next;
		if (next == FALSE || start == parent)
			break;
	}
}


void obj_draw_area(OBJECT *tree, _WORD start, _WORD depth, _BOOL for_menu)
{
	_WORD xc, yc;

	od_oldstate = tree[start].ob_state;
	if (start != ROOT)
	{
		od_clear = TRUE;
	}
	objc_offset(tree, start, &xc, &yc);
	_obj_draw(tree, NIL, start, depth, xc - tree[start].ob_x, yc - tree[start].ob_y, FALSE, RSC_LANG_DEFAULT, for_menu);
	od_clear = FALSE;
}
