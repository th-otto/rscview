#define _GNU_SOURCE

#include "config.h"
#include <gem.h>
#include <object.h>
#include <ro_mem.h>
#include "fileio.h"
#include <rsc.h>

FILE *ffp = NULL;
const char *fname;

static _BOOL fopen_mode;

#define g_box			"G_BOX"
#define g_text			"G_TEXT"
#define g_boxtext		"G_BOXTEXT"
#define g_image         "G_IMAGE"
#define g_userdef		"G_USERDEF"
#define g_ibox			"G_IBOX"
#define g_button		"G_BUTTON"
#define g_boxchar		"G_BOXCHAR"
#define g_string		"G_STRING"
#define g_ftext         "G_FTEXT"
#define g_fboxtext		"G_FBOXTEXT"
#define g_icon			"G_ICON"
#define g_cicon         "G_CICON"
#define g_title         "G_TITLE"
#define g_swbutton		"G_SWBUTTON"
#define g_popup			"G_POPUP"
#define g_edit			"G_EDIT"
#define g_shortcut		"G_SHORTCUT"
#define g_slist			"G_SLIST"
#define g_extbox		"G_EXTBOX"
#define g_oblink		"G_OBLINK"
#define g_unknown		"  ERROR: Unknown type"

/******************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/******************************************************************************/

_BOOL file_close(_BOOL status)
{
	if (ffp != NULL)
	{
		fflush(ffp);
		if (ferror(ffp))
			status = FALSE;
		if (ffp != stdout)
			fclose(ffp);
		if (fname != NULL && status == FALSE)
			(*(fopen_mode ? err_fwrite : err_fread))(fname);
		ffp = NULL;
	}
	fname = NULL;
	return status;
}

/*** ---------------------------------------------------------------------- ***/

_BOOL file_create(const char *filename, const char *mode)
{
	if ((ffp = fopen(filename, mode)) == NULL)
	{
		err_fcreate(filename);
		return FALSE;
	}
	fopen_mode = TRUE;
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

_BOOL file_open(const char *filename, const char *mode)
{
	if ((ffp = fopen(filename, mode)) == NULL)
		return FALSE;
	fopen_mode = FALSE;
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

const char *rtype_name(_WORD type)
{
	switch (type)
	{
		case RT_DIALOG : return "form/dialog";
		case RT_MENU   : return "menu";
		case RT_FREE   : return "free form";
		case RT_ALERT  : return "Alert string";
		case RT_FRSTR  : return "Free string";
		case RT_FRIMG  : return "Free image";
		case RT_MOUSE  : return "Mouse cursor";
		case RT_BUBBLEMORE : return "BubbleMore";
		case RT_BUBBLEUSER : return "BubbleUser";
		default:
		case RT_UNKNOWN: return "unknown form";
	}
}

/*** ---------------------------------------------------------------------- ***/

const char *rtype_name_short(_WORD type)
{
	switch (type)
	{
		case RT_DIALOG : return "dialog";
		case RT_MENU   : return "menu";
		case RT_FREE   : return "free";
		case RT_ALERT  : return "alert";
		case RT_FRSTR  : return "string";
		case RT_FRIMG  : return "image";
		case RT_MOUSE  : return "cursor";
		case RT_BUBBLEMORE : return "more";
		case RT_BUBBLEUSER : return "user";
		default:
		case RT_UNKNOWN: return "unknown";
	}
}

/*** ---------------------------------------------------------------------- ***/

const char *type_name(_WORD type)
{
	switch (type)
	{
		case G_BOX: return g_box;
		case G_IBOX: return g_ibox;
		case G_BOXCHAR: return g_boxchar;
		case G_STRING: return g_string;
		case G_TITLE: return g_title;
		case G_BUTTON: return g_button;
		case G_TEXT: return g_text;
		case G_FTEXT: return g_ftext;
		case G_BOXTEXT: return g_boxtext;
		case G_FBOXTEXT: return g_fboxtext;
		case G_IMAGE: return g_image;
		case G_ICON: return g_icon;
		case G_CICON: return g_cicon;
		case G_USERDEF: return g_userdef;
		case G_SWBUTTON: return g_swbutton;
		case G_POPUP: return g_popup;
		case G_EDIT: return g_edit;
		case G_SHORTCUT: return g_shortcut;
		case G_SLIST: return g_slist;
		case G_EXTBOX: return g_extbox;
		case G_OBLINK: return g_oblink;
		default: return g_unknown;
	}
}

/*** ---------------------------------------------------------------------- ***/

char *rsx_basename(const char *name)
{
	static char namebuf[FNAMELEN+1];
	char *dotp;

	strcpy(namebuf, name);
	dotp = strrchr(namebuf, '.');
	if (dotp != NULL &&
		(strcasecmp(dotp + 1, "rsc") == 0))
		*dotp = '\0';
	return namebuf;
}

/*** ---------------------------------------------------------------------- ***/

void set_extension(char *filename, const char *ext)
{
	const char *p;
	char *p2;

	p = basename(filename);
	if ((p2 = strrchr(p, '.')) == NULL)
	{
		strncat(filename, ".", PATH_MAX);
		p2 = strchr(p, '.');
	}
	if (*ext != '\0')
		++p2;
	*p2 = '\0';
	strncat(filename, ext, PATH_MAX);
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL is_menu(OBJECT *tree)
{
	_WORD titleline;
	_WORD titlebox;
	_WORD workscreen;
	_WORD title, menubox;

	if (tree == NULL ||
		tree[ROOT].ob_type != G_IBOX ||
		(titleline = tree[ROOT].ob_head) == NIL ||
		tree[titleline].ob_type != G_BOX ||
		(workscreen = tree[tree[ROOT].ob_head].ob_next) == titleline ||
		tree[workscreen].ob_type != G_IBOX ||
		tree[workscreen].ob_next != titleline ||
		(titlebox = tree[titleline].ob_head) == NIL ||
		tree[titlebox].ob_type != G_IBOX ||
#if 0
		tree[titlebox].ob_next != titlebox ||
#endif
		(title = tree[titlebox].ob_head) == NIL ||
		(menubox = tree[workscreen].ob_head) == NIL)
		return FALSE;
	do
	{
		if (tree[title].ob_type != G_TITLE ||
			tree[title].ob_head != NIL ||
			tree[menubox].ob_type != G_BOX ||
			tree[menubox].ob_head == NIL)
			return FALSE;
		title = tree[title].ob_next;
		menubox = tree[menubox].ob_next;
	} while (title != titlebox && menubox != workscreen);
	if (title != titlebox || menubox != workscreen)
		return FALSE;
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL any_exit(OBJECT *tree)
{
	_WORD i = 0;
	_UWORD flags;
	
	for (;;)
	{
		flags = tree[i].ob_flags;
		if (!(flags & OF_HIDETREE))
		{
			if (!(tree[i].ob_state & OS_DISABLED) &&
				((flags & OF_TOUCHEXIT) || ((flags & OF_EXIT) && (flags & (OF_SELECTABLE|OF_DEFAULT)))))
					return TRUE;
		}
		if (tree[i].ob_flags & OF_LASTOB)
			return FALSE;
		++i;
	}		
}

static _BOOL Form_Al_is_Str_Ok(const char *str)
{
	if (str != NULL &&
		str[0] == '[' &&
		str[1] >= '0' &&
#if 0
		str[1] <= '0' + MAX_ALERT_SYM &&
#endif
		str[2] == ']' &&
		str[3] == '[' &&
#if 0
		strlen(str) < FO_BUFSIZE &&
#endif
		(str = strchr(str + 4, ']')) != NULL &&
		str[1] == '[' &&
		(str = strchr (str + 2, ']')) != NULL &&
		str[1] == '\0')
		return TRUE;
	return FALSE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL rsc_load_trees(RSCFILE *file)
{
	_ULONG i;
	_WORD type;
	RSCTREE *tree;
	char name[MAXNAMELEN+1];
	
	for (i = 0; i < file->header.rsh_ntree; i++)
	{
		OBJECT *ob = file->rs_trindex[i];
		type = is_menu(ob) ? RT_MENU :
			   any_exit(ob) ? RT_DIALOG : RT_UNKNOWN;
		sprintf(name, "TREE%03ld", i + 1);
		if ((tree = rsc_add_tree(file, type, name, ob)) == NULL)
		{
			return FALSE;
		}
	}

	for (i = 0; i < file->header.rsh_nstring; i++)
	{
		char *str = file->rs_frstr[i];
		type = Form_Al_is_Str_Ok(str) ? RT_ALERT : RT_FRSTR;
		sprintf(name, "STR%03ld", i + 1);
		if (rsc_add_tree(file, type, name, str) == NULL)
		{
			g_free(str);
			return FALSE;
		}
	}

	for (i = 0; i < file->header.rsh_nimages; i++)
	{
		BITBLK *bitblk = file->rs_frimg[i];
		sprintf(name, "IMAGE%03ld", i + 1);
		if ((tree = rsc_add_tree(file, RT_FRIMG, name, bitblk)) == NULL)
		{
			return FALSE;
		}
		if (is_mouseform(tree))
		{
			tree->rt_type = RT_MOUSE;
			sprintf(tree->rt_name, "MOUSE%03ld", i + 1);
		}
	}
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

RSCFILE *load_all(const char *filename, _UWORD flags)
{
	RSCFILE *file;
	
	file = xrsrc_load(filename, flags);
	if (file == NULL)
		return NULL;
	
	rsc_load_trees(file);
	
	return file; 
}
