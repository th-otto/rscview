/*****************************************************************************
 * RSC.C
 * rsc tree functions
 *****************************************************************************/

#include "config.h"
#include <gem.h>
#include <mobject.h>
#include <rsc.h>
#include <xrsrc.h>
#include <ro_mem.h>

static rsc_options op_rsc_opts = {
	'@',	/* ted_fillchar */
	2,		/* menu_leftmargin */
	1,		/* menu_rightmargin */
	2,		/* menu_minspace */
	FALSE,	/* menu_fillspace */
	500,	/* menu_maxchars */
	FALSE,	/* alert_limits */
	3,		/* alert_max_icon */
	30,		/* alert_max_linesize */
	FALSE,	/* crc_string */
	FALSE,	/* dummy_icons */
	TRUE,	/* ted_small_valid */
	0,		/* layer_save_mode */
	TRUE,	/* objc_size_check */
	FALSE,	/* magic_buttons */
	TRUE,	/* menu_keycheck */
};

/******************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/******************************************************************************/

void rsc_tree_count(RSCFILE *file)
{
	RSCTREE *tree;
	_LONG ntrees, nimages, nstrings;
	_LONG bghmore, bghuser;
	
	ntrees = nimages = nstrings = bghmore = bghuser = 0;
	FOR_ALL_RSC(file, tree)
	{
		switch (tree->rt_type)
		{
		case RT_DIALOG:
		case RT_FREE:
		case RT_UNKNOWN:
		case RT_MENU:
			tree->rt_index = ntrees++;
			break;
		case RT_FRSTR:
		case RT_ALERT:
			tree->rt_index = nstrings++;
			break;
		case RT_FRIMG:
		case RT_MOUSE:
			tree->rt_index = nimages++;
			break;
		case RT_BUBBLEMORE:
			tree->rt_index = bghmore++;
			break;
		case RT_BUBBLEUSER:
			tree->rt_index = bghuser++;
			break;
		default: /* this should not happen */
			tree->rt_index = 0;
			break;
		}
	}
}

/*** ---------------------------------------------------------------------- ***/

static _WORD obj_count(OBJECT *tree, _WORD parent, _WORD n)
{
	_WORD ob = parent == NIL ? ROOT : tree[parent].ob_head;

	do
	{
		/* OB_INDEX(ob) = n; */
		n++;
		if (tree[ob].ob_head != NIL)
			n = obj_count(tree, ob, n);
		ob = tree[ob].ob_next;
	} while (ob != parent);
	return n;
}

/*** ---------------------------------------------------------------------- ***/

_VOID rsc_count_all(RSCFILE *file)
{
	RSCTREE *tree;
	
	FOR_ALL_RSC(file, tree)
	{
		switch (tree->rt_type)
		{
		case RT_DIALOG:
		case RT_FREE:
		case RT_UNKNOWN:
			if (tree->rt_objects.dial.di_tree)
				obj_count(tree->rt_objects.dial.di_tree, NIL, 0);
			break;
		case RT_MENU:
			if (tree->rt_objects.menu.mn_tree)
				obj_count(tree->rt_objects.menu.mn_tree, NIL, 0);
			break;
		case RT_FRSTR:
		case RT_ALERT:
			break;
		case RT_FRIMG:
		case RT_MOUSE:
			break;
		case RT_BUBBLEMORE:
		case RT_BUBBLEUSER:
			break;
		default: /* this should not happen */
			break;
		}
	}
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL rsc_insert_tree(RSCFILE *file, RSCTREE *tree, _VOID *object)
{
	RSCTREE *next;
	_BOOL duplicates = FALSE;
	
	tree->rt_file = file;
	switch (tree->rt_type)
	{
	case RT_DIALOG:
	case RT_FREE:
	case RT_UNKNOWN:
		tree->rt_objects.dial.di_tree = (OBJECT *)object;
		tree->rt_objects.dial.di_popup = NULL;
		tree->rt_objects.dial.di_popup_tree = NULL;
		tree->rt_objects.dial.di_x = 0;
		tree->rt_objects.dial.di_y = 0;
		FOR_ALL_RSC(file, next)
		{
			if (next->rt_type == RT_FRIMG ||
				next->rt_type == RT_MOUSE ||
				next->rt_type == RT_FRSTR ||
				next->rt_type == RT_ALERT)
				break;
		}
		break;
	case RT_MENU:
		tree->rt_objects.menu.mn_tree = (OBJECT *)object;
		tree->rt_objects.menu.mn_title = NULL;
		tree->rt_objects.menu.mn_menu = NULL;
		tree->rt_objects.menu.mn_submenu = NULL;
		FOR_ALL_RSC(file, next)
		{
			if (next->rt_type == RT_FRIMG ||
				next->rt_type == RT_MOUSE ||
				next->rt_type == RT_FRSTR ||
				next->rt_type == RT_ALERT)
				break;
		}
		break;
	case RT_FRSTR:
		tree->rt_objects.str.fr_str = (_UBYTE *)object;
		FOR_ALL_RSC(file, next)
		{
			if (next->rt_type == RT_FRIMG ||
				next->rt_type == RT_MOUSE)
				break;
		}
		break;
	case RT_ALERT:
		tree->rt_objects.alert.al_str = (_UBYTE *)object;
		FOR_ALL_RSC(file, next)
		{
			if (next->rt_type == RT_FRIMG ||
				next->rt_type == RT_MOUSE)
				break;
		}
		break;
	case RT_FRIMG:
	case RT_MOUSE:
		tree->rt_objects.bit = (BITBLK *)object;
		FOR_ALL_RSC(file, next)
		{
			if (next->rt_type == RT_BUBBLEMORE ||
				next->rt_type == RT_BUBBLEUSER)
				break;
		}
		break;
	case RT_BUBBLEMORE:
	case RT_BUBBLEUSER:
		tree->rt_objects.bgh = (BGHINFO *)object;
		next = &file->rsc_treehead;
		break;
	default: /* this should not happen */
		return FALSE;
	}
	tree->rt_next = next;
	tree->rt_prev = next->rt_prev;
	tree->rt_prev->rt_next = tree;
	tree->rt_next->rt_prev = tree;
	file->rsc_ntrees++;
	rsc_tree_count(file);
	return duplicates;
}

/*** ---------------------------------------------------------------------- ***/

RSCTREE *rsc_add_tree(RSCFILE *file, _WORD type, const _UBYTE *name, _VOID *object)
{
	RSCTREE *tree;
	
	if ((tree = g_new0(RSCTREE, 1)) == NULL)
		return NULL;
	tree->rt_type = type;
	strcpy(tree->rt_name, name);
	tree->rt_cmnt = NULL;
	rsc_insert_tree(file, tree, object);
	return tree;
}

/*** ---------------------------------------------------------------------- ***/

static void rsc_tree_dispose(RSCTREE *tree)
{
	switch (tree->rt_type)
	{
	case RT_DIALOG:
	case RT_FREE:
	case RT_UNKNOWN:
		tree->rt_objects.dial.di_tree = NULL;
		break;
	case RT_MENU:
		tree->rt_objects.menu.mn_tree = NULL;
		break;
	case RT_FRSTR:
		g_free(tree->rt_objects.str.fr_str);
		bgh_delete(tree->rt_objects.str.fr_bgh);
		break;
	case RT_ALERT:
		g_free(tree->rt_objects.alert.al_str);
		bgh_delete(tree->rt_objects.alert.al_bgh);
		break;
	case RT_FRIMG:
	case RT_MOUSE:
		/* bit_free(tree->rt_objects.bit); */
		break;
	case RT_BUBBLEUSER:
	case RT_BUBBLEMORE:
		bgh_delete(tree->rt_objects.bgh);
		break;
	}
	g_free(tree->rt_cmnt);
	g_free(tree);
}

/*** ---------------------------------------------------------------------- ***/

void rsc_tree_delete(RSCTREE *tree)
{
	RSCFILE *file = tree->rt_file;
	
	tree->rt_prev->rt_next = tree->rt_next;
	tree->rt_next->rt_prev = tree->rt_prev;
	file->rsc_ntrees--;
	rsc_tree_count(file);
	rsc_tree_dispose(tree);
}

/*** ---------------------------------------------------------------------- ***/

static void rsc_init_file(RSCFILE *file)
{
	file->rsc_ntrees = 0;
	file->rsc_treehead.rt_next =
	file->rsc_treehead.rt_prev = &file->rsc_treehead;
	file->rsc_flags = RF_C | RF_RSO | RF_RSC;
	file->rsc_flags2 = 0;
	file->rsc_extob.mode = EXTOB_NONE;
	file->rsc_cmnt = NULL;
	file->rsc_emutos = EMUTOS_NONE;
	file->rsc_emutos_frstrcond_name = NULL;
	file->rsc_emutos_frstrcond_string = NULL;
	file->rsc_emutos_othercond_name = NULL;
	file->rsc_emutos_othercond_string = NULL;
	file->rsc_swap_flag = FALSE;
	file->rsc_opts = op_rsc_opts;
	file->rsc_rsm_crc = RSC_CRC_NONE;
	file->rsc_opts.crc_string = FALSE;
	file->rsc_crc_for_string = RSC_CRC_NONE;
}

/*** ---------------------------------------------------------------------- ***/

RSC_RSM_CRC rsc_rsm_calc_crc(const void *buf, _ULONG size)
{
	RSC_RSM_CRC crc = 0x5555;
	const signed char *p = (const signed char *) buf;
	_ULONG i;
	
	for (i = 0; i < size; i++)
		crc += *p++;
	return crc & 0x7fff;
}

/*** ---------------------------------------------------------------------- ***/

RSCFILE *rsc_new_file(const _UBYTE *filename, const _UBYTE *basename)
{
	RSCFILE *file;
	
	if ((file = g_new(RSCFILE, 1)) == NULL)
		return NULL;
	if (filename != NULL)
	{
		strcpy(file->rsc_rsxfilename, filename);
	} else
	{
		strcpy(file->rsc_rsxfilename, "");
	}
	strcpy(file->rsc_rsxname, basename);
	rsc_init_file(file);
	return file;
}

/*** ---------------------------------------------------------------------- ***/

void rsc_file_delete(RSCFILE *file, _BOOL all)
{
	while (file->rsc_ntrees != 0)
		rsc_tree_delete(file->rsc_treehead.rt_next);
	if (all)
	{
		g_free(file->rsc_cmnt);
		g_free(file->rsc_emutos_frstrcond_name);
		g_free(file->rsc_emutos_frstrcond_string);
		g_free(file->rsc_emutos_othercond_name);
		g_free(file->rsc_emutos_othercond_string);
		g_free(file);
	} else
	{
		g_free(file->rsc_cmnt);
		g_free(file->rsc_emutos_frstrcond_name);
		g_free(file->rsc_emutos_frstrcond_string);
		g_free(file->rsc_emutos_othercond_name);
		g_free(file->rsc_emutos_othercond_string);
		rsc_init_file(file);
	}
}

/*** ---------------------------------------------------------------------- ***/

RSC_RSM_CRC rsc_get_crc_string(const char *str)
{
	RSC_RSM_CRC crc;
	
	if (str == NULL)
		return RSC_CRC_NONE;
	if (strlen(str) != (RSM_CRC_STRLEN - 1))
		return RSC_CRC_NONE;
	if (strncmp(str, RSM_CRC_STRING, 11) != 0)
		return RSC_CRC_NONE;
	if (strcmp(str + 15, RSM_CRC_STRING + 15) != 0)
		return RSC_CRC_NONE;
	crc = (RSC_RSM_CRC)strtoul(str + 11, NULL, 16);
	return crc;
}

/*** ---------------------------------------------------------------------- ***/

_BOOL rsc_is_crc_string(const char *str)
{
	return rsc_get_crc_string(str) != RSC_CRC_NONE;
}

/*** ---------------------------------------------------------------------- ***/

void rsc_remove_crc_string(RSCFILE *file)
{
	RSCTREE *tree;
	
	FOR_ALL_RSC(file, tree)
	{
		if (tree->rt_type == RT_FRSTR &&
			rsc_is_crc_string(tree->rt_objects.str.fr_str))
		{
			/* like rsc_tree_delete(), but dont mark the just loaded file as changed */
			tree->rt_prev->rt_next = tree->rt_next;
			tree->rt_next->rt_prev = tree->rt_prev;
			file->rsc_ntrees--;
			rsc_tree_count(file);
			rsc_tree_dispose(tree);
			break;
		}
	}
}

/*** ---------------------------------------------------------------------- ***/

const char *ob_name(RSCFILE *file, OBJECT *tree, _WORD ob)
{
	UNUSED(file);
	UNUSED(tree);
	UNUSED(ob);
	return NULL;
}

/*** ---------------------------------------------------------------------- ***/

const char *ob_cmnt(RSCFILE *file, OBJECT *tree, _WORD ob)
{
	UNUSED(file);
	UNUSED(tree);
	UNUSED(ob);
	return NULL;
}

