#define _GNU_SOURCE

#include "config.h"
#include <gem.h>
#include <object.h>
#include <ro_mem.h>
#include "fileio.h"

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
#if defined(__TURBOC__) && defined(OS_ATARI)
		/*
		 * Turbo-C schliesst die System-Datei nicht, wenn
		 * ein Fehler aufgetreten ist
		 */
		if (ffp != stdout && (ffp->Flags & 3))
		{
			close(ffp->Handle);
			if (ffp->Flags & 0x08)
				free(ffp->BufStart);
			ffp->Flags = 0;
		}
#endif
		ffp = NULL;
	}
	fname = NULL;
	return status;
}

/*** ---------------------------------------------------------------------- ***/

_BOOL file_create(const _UBYTE *filename, const _UBYTE *mode)
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

_BOOL file_open(const _UBYTE *filename, const _UBYTE *mode)
{
	if ((ffp = fopen(filename, mode)) == NULL)
		return FALSE;
	fopen_mode = FALSE;
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

const _UBYTE *rtype_name(_WORD type)
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

const _UBYTE *rtype_name_short(_WORD type)
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

CONST _UBYTE *type_name(_WORD type)
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

_UBYTE *rsx_basename(CONST _UBYTE *name)
{
	static _UBYTE namebuf[FNAMELEN+1];
	_UBYTE *dotp;

	strcpy(namebuf, name);
	dotp = strrchr(namebuf, '.');
	if (dotp != NULL &&
		(strcasecmp(dotp + 1, ".rsc") == 0))
		*dotp = '\0';
	return namebuf;
}

/*** ---------------------------------------------------------------------- ***/

_VOID set_extension(_UBYTE *filename, CONST _UBYTE *ext)
{
	CONST _UBYTE *p;
	_UBYTE *p2;

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
