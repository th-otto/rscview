#define _GNU_SOURCE

#include "config.h"
#include <gem.h>
#include <object.h>
#include <ro_mem.h>
#include "fileio.h"
#include <rsc.h>
#include <time.h>
#include "debug.h"

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

RSCTREE *rsc_tree_index(RSCFILE *file, _UWORD idx, _UWORD type)
{
	RSCTREE *tree;

	FOR_ALL_RSC(file, tree)
	{
		switch (type)
		{
		case RT_UNKNOWN:
		case RT_FREE:
		case RT_DIALOG:
		case RT_MENU:
			switch (tree->rt_type)
			{
			case RT_UNKNOWN:
			case RT_FREE:
			case RT_DIALOG:
			case RT_MENU:
				if (idx == 0)
					return tree;
				idx--;
				break;
			}
			break;
		case RT_ALERT:
		case RT_FRSTR:
			switch (tree->rt_type)
			{
			case RT_ALERT:
			case RT_FRSTR:
				if (idx == 0)
					return tree;
				idx--;
				break;
			}
			break;
		case RT_FRIMG:
		case RT_MOUSE:
			switch (tree->rt_type)
			{
			case RT_FRIMG:
			case RT_MOUSE:
				if (idx == 0)
					return tree;
				idx--;
				break;
			}
			break;
		case RT_BUBBLEMORE:
			switch (tree->rt_type)
			{
			case RT_BUBBLEMORE:
				if (idx == 0)
					return tree;
				idx--;
				break;
			}
			break;
		case RT_BUBBLEUSER:
			switch (tree->rt_type)
			{
			case RT_BUBBLEUSER:
				if (idx == 0)
					return tree;
				idx--;
				break;
			}
			break;
		default:
			return NULL;
		}
	}
	return NULL;
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
		(workscreen = tree[titleline].ob_next) == titleline ||
		tree[workscreen].ob_type != G_IBOX ||
		tree[workscreen].ob_next != ROOT ||
		(titlebox = tree[titleline].ob_head) == NIL ||
		tree[titlebox].ob_type != G_IBOX ||
		tree[titlebox].ob_next != titleline ||
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
	const char *prefix;
	
	for (i = 0; i < file->header.rsh_ntree; i++)
	{
		OBJECT *ob = file->rs_trindex[i];
		if (is_menu(ob))
		{
			type = RT_MENU;
			prefix = "MENU";
		} else if (any_exit(ob))
		{
			type = RT_DIALOG;
			prefix = "DIALOG";
		} else
		{
			type = RT_UNKNOWN;
			prefix = "TREE";
		}
		sprintf(name, "%s%03ld", prefix, i + 1);
		if ((tree = rsc_add_tree(file, type, name, ob)) == NULL)
		{
			return FALSE;
		}
	}

	for (i = 0; i < file->header.rsh_nstring; i++)
	{
		char *str = file->rs_frstr[i];
		if (Form_Al_is_Str_Ok(str))
		{
			type = RT_ALERT;
			prefix = "ALERT";
		} else
		{
			type = RT_FRSTR;
			prefix = "STR";
		}
		sprintf(name, "%s%03ld", prefix, i + 1);
		if (rsc_add_tree(file, type, name, str) == NULL)
		{
			g_free(str);
			return FALSE;
		}
	}

	for (i = 0; i < file->header.rsh_nimages; i++)
	{
		BITBLK *bitblk = file->rs_frimg[i];
		if (is_mouseform(bitblk))
		{
			type = RT_MOUSE;
			prefix = "MOUSE";
		} else
		{
			type = RT_FRIMG;
			prefix = "IMAGE";
		}
		sprintf(name, "%s%03ld", prefix, i + 1);
		if ((tree = rsc_add_tree(file, type, name, bitblk)) == NULL)
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL is_emutos_aes(RSCFILE *file)
{
	RSCTREE *tree;
	
	if (strcasecmp(rsx_basename(file->rsc_rsxname), "gem") != 0 &&
		strcasecmp(rsx_basename(file->rsc_rsxname), "gem_rsc") != 0)
		return FALSE;
	/*
	 * check for exactly 3 dialogs (fileselector, alert template & desktop)
	 */
	if ((tree = rsc_tree_index(file, 2, RT_DIALOG)) == NULL)
		return FALSE;
	if (Objc_Count(tree->rt_objects.dial.di_tree, ROOT) != 3)
		return FALSE;
	if (rsc_tree_index(file, 3, RT_DIALOG) != NULL)
		return FALSE;
	/*
	 * check for at least 11 images (3 icons & 8 mouse types)
	 */
	if (rsc_tree_index(file, 10, RT_MOUSE) == NULL)
		return FALSE;
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL is_emutos_icon(RSCFILE *file)
{
	if (strcasecmp(rsx_basename(file->rsc_rsxname), "icon") != 0)
		return FALSE;
	if (rsc_tree_index(file, 1, RT_DIALOG) == NULL)
		return FALSE;
	if (rsc_tree_index(file, 2, RT_DIALOG) != NULL)
		return FALSE;
	if (rsc_tree_index(file, 0, RT_FRSTR) != NULL)
		return FALSE;
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL is_emutos_desktop(RSCFILE *file)
{
	RSCTREE *rsctree;
	OBJECT *ob;

#define ADMENU 0
#define ADFFINFO 1

#define ADDINFO 3
#define DELABEL 4
#define DEVERSN 5
#define DECOPYRT 7

#define ADCPYDEL 7

#define STFOINFO 4
#define STDELETE 7

	if (strcasecmp(rsx_basename(file->rsc_rsxname), "desktop") != 0)
		return FALSE;
	if ((rsctree = rsc_tree_index(file, ADDINFO, RT_UNKNOWN)) == NULL || rsctree->rt_type != RT_DIALOG)
		return FALSE;
	if (Objc_Count(rsctree->rt_objects.dial.di_tree, ROOT) < 10)
		return FALSE;
	ob = rsctree->rt_objects.dial.di_tree + 2;
	if (ob->ob_type != G_STRING)
		return FALSE;
	if (strcmp(ob->ob_spec.free_string, "- EmuTOS -") != 0)
		return FALSE;
	ob = rsctree->rt_objects.dial.di_tree + DELABEL;
	if (ob->ob_type != G_STRING)
		return FALSE;
	
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

#define gettext(x) x

static void xlate_obj_array(OBJECT *obj_array, _LONG nobs)
{
	OBJECT *obj;

	for (obj = obj_array; --nobs >= 0; obj++)
	{
		switch (obj->ob_type)
		{
		case G_TEXT:
		case G_BOXTEXT:
		case G_FTEXT:
		case G_FBOXTEXT:
			obj->ob_spec.tedinfo->te_ptmplt = gettext(obj->ob_spec.tedinfo->te_ptmplt);
			break;
		case G_STRING:
		case G_BUTTON:
		case G_TITLE:
			obj->ob_spec.free_string = gettext(obj->ob_spec.free_string);
			break;
		default:
			break;
		}
	}
}

/*** ---------------------------------------------------------------------- ***/

#define CENTRE_ALIGNED  0x8000
#define RIGHT_ALIGNED   0x4000

/*
 *  Align text objects according to special values in ob_flags
 *
 *  Translations typically have a length different from the original
 *  English text.  In order to keep dialogs looking tidy in all
 *  languages, it is often useful to centre- or right-align text
 *  objects.  The AES does not provide an easy way of doing this
 *  (alignment in TEDINFO objects affects the text within the object,
 *  as well as object positioning).
 *
 *  To allow centre- or right-alignment alignment of text objects,
 *  we steal unused bits in ob_flags to indicate the required
 *  alignment.  Note that this does not cause any incompatibilities
 *  because this extra function is performed outside the AES, and
 *  only for the internal desktop resource.  Furthermore, we zero
 *  out the stolen bits after performing the alignment.
 *
 *  Also note that this aligns the *object*, not the text within
 *  the object.  It is perfectly reasonable (and common) to have
 *  left-aligned text within a right-aligned TEDINFO object.
 */
static void align_objects(OBJECT *obj_array, int nobj)
{
	OBJECT *obj;
	char *p;
	_WORD len;		 /* string length in pixels */
	_WORD wchar, hchar;
	
	GetTextSize(&wchar, &hchar);
	for (obj = obj_array; --nobj >= 0; obj++)
	{
		switch(obj->ob_type)
		{
		case G_STRING:
		case G_TEXT:
		case G_FTEXT:
		case G_BOXTEXT:
		case G_FBOXTEXT:
			if (obj->ob_type == G_STRING)
				p = obj->ob_spec.free_string;
			else
				p = obj->ob_spec.tedinfo->te_ptmplt;
			len = strlen(p) * wchar;
			if (obj->ob_flags & CENTRE_ALIGNED)
			{
				obj->ob_x += (obj->ob_width - len) / 2;
				if (obj->ob_x < 0)
					obj->ob_x = 0;
				obj->ob_width = len;
			} else if (obj->ob_flags & RIGHT_ALIGNED)
			{
				obj->ob_x += obj->ob_width - len;
				if (obj->ob_x < 0)
					obj->ob_x = 0;
				obj->ob_width = len;
			}
			obj->ob_flags &= ~(CENTRE_ALIGNED | RIGHT_ALIGNED);
			break;
		default:
			break;
		}
	}
}

/*
 *  Horizontally centre dialog title: this is done dynamically to
 *  handle translated titles.
 *
 *  If object 1 of a tree is a G_STRING and its y position equals
 *  one character height, we assume it's the title.
 */
static void centre_title(OBJECT *root)
{
	OBJECT *title;
	_WORD len;
	_WORD wchar, hchar;
	
	GetTextSize(&wchar, &hchar);

	title = root + 1;

	if (title->ob_type == G_STRING && title->ob_y == hchar)
	{
		len = strlen(title->ob_spec.free_string) * wchar;
		if (len > root->ob_width)
			len = root->ob_width;
		title->ob_x = (root->ob_width - len) / 2;
	}
}

/*** ---------------------------------------------------------------------- ***/

static void centre_titles(RSCFILE *file)
{
	_ULONG i;
	
	for (i = 0; i < file->header.rsh_ntree; i++)
		centre_title(file->rs_trindex[i]);
}

/*** ---------------------------------------------------------------------- ***/

static void adjust_menu(OBJECT *obj_array)
{
#define OBJ(i) (&obj_array[i])

	int i;	/* index in the menu bar */
	int n, x;
	OBJECT *menu = OBJ(0);
	OBJECT *mbar = OBJ(OBJ(menu->ob_head)->ob_head);
	OBJECT *title;

	/*
	 * first, set ob_x & ob_width for all the menu headings, and
	 * determine the required width of the (translated) menu bar.
	 */
	for (i = mbar->ob_head, title = OBJ(i), x = 0; i <= mbar->ob_tail; i++, title++, x += n)
	{
		n = strlen(title->ob_spec.free_string);
		title->ob_x = x;
		title->ob_width = n;
	}
	mbar->ob_width = x;

	KDEBUG(("desktop menu bar: x=0x%04x, w=0x%04x\n",mbar->ob_x,mbar->ob_width));
#undef OBJ
}

static void emutos_desktop_fix(RSCFILE *file)
{
    OBJECT *tree = file->rs_trindex[ADDINFO];
	static char version[10];
	static char copyright_year[5];
	static char empty[1];
	time_t now;
	struct tm *tm;
	_WORD wchar, hchar;
	
	GetTextSize(&wchar, &hchar);
	
	now = time(NULL);
	tm = localtime(&now);
	sprintf(version, "%04d%02d%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
	sprintf(copyright_year, "%04d", tm->tm_year + 1900);
	
    /* translate strings in objects */
    xlate_obj_array(file->rs_object, file->header.rsh_nobs);

    /* insert the version number */
    tree[DEVERSN].ob_spec.free_string = version;

    /* slightly adjust the about box for a timestamp build */
    if (version[1] != '.')
    {
        tree[DELABEL].ob_spec.free_string = empty;  /* remove the word "Version" */
        tree[DEVERSN].ob_x -= 6 * wchar;          /* and move the start of the string */
    }

    /* insert the version number */
    tree[DECOPYRT].ob_spec.free_string = copyright_year;

    /* adjust the size and coordinates of menu items */
    adjust_menu(file->rs_trindex[ADMENU]);

    /*
     * perform special object alignment - this must be done after
     * translation and coordinate fixing
     */
    align_objects(file->rs_object, file->header.rsh_nobs);
    
    tree = file->rs_trindex[ADFFINFO];
    tree[1].ob_spec.free_string = file->rs_frstr[STFOINFO];
    
    tree = file->rs_trindex[ADCPYDEL];
    tree[1].ob_spec.free_string = file->rs_frstr[STDELETE];
    
    centre_titles(file);
}


#undef ADMENU
#undef ADFFINFO
#undef ADDINFO
#undef DELABEL
#undef DEVERSN
#undef DECOPYRT

static void emutos_aes_fix(RSCFILE *file)
{
    /* translate strings in objects */
    xlate_obj_array(file->rs_object, file->header.rsh_nobs);
	centre_titles(file);
}

/*** ---------------------------------------------------------------------- ***/

RSCFILE *load_all(const char *filename, _UWORD flags)
{
	RSCFILE *file;
	
	file = xrsrc_load(filename, flags);
	if (file == NULL)
		return NULL;
	
	rsc_load_trees(file);
	
	if (is_emutos_desktop(file))
	{
		file->rsc_emutos = EMUTOS_DESK;
		file->rsc_output_prefix = g_strdup("desktop");
		file->rsc_output_basename = g_strdup("desk_rsc");
		file->rsc_flags2 |= RF_ROMABLE | RF_IMAGEWORDS;
		file->rsc_flags |= RF_CSOURCE2;
		file->rsc_emutos_frstrcond_name = g_strdup("STICNTYP");
		file->rsc_emutos_frstrcond_string = g_strdup("#ifndef TARGET_192");
		file->rsc_emutos_othercond_name = g_strdup("ADTTREZ");
		file->rsc_emutos_othercond_string = g_strdup("#ifndef TARGET_192");
		nf_debugprintf("EmuTOS desktop resource loaded\n");
		emutos_desktop_fix(file);
	} else if (is_emutos_icon(file))
	{
		file->rsc_emutos = EMUTOS_ICONS;
		file->rsc_output_prefix = g_strdup("icons");
		file->rsc_output_basename = g_strdup("icons");
		file->rsc_flags2 |= RF_ROMABLE | RF_IMAGEWORDS;
		file->rsc_flags |= RF_CSOURCE2;
		nf_debugprintf("EmuTOS icons resource loaded\n");
	} else if (is_emutos_aes(file))
	{
		file->rsc_emutos = EMUTOS_AES;
		file->rsc_output_prefix = g_strdup("gem");
		file->rsc_output_basename = g_strdup("gem_rsc");
		file->rsc_flags2 |= RF_ROMABLE | RF_IMAGEWORDS;
		file->rsc_flags |= RF_CSOURCE2;
		file->rsc_emutos_othercond_name = g_strdup("APPS");
		file->rsc_emutos_othercond_string = g_strdup("#if 0");
		nf_debugprintf("EmuTOS gem resource loaded\n");
		emutos_aes_fix(file);
	}
	
	return file; 
}
