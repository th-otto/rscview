#include "config.h"
#include <stdint.h>
#include <gem.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include "portvdi.h"
#include "nls.h"
#include "fileio.h"
#include "or_draw.h"
#include "rsc.h"
#include "ws.h"
#include "debug.h"
#include "pofile.h"
#ifdef _WIN32
#include <direct.h>
#endif
#include "../vdi/writepng.h" /* for writepng_version_info */


char const program_name[] = "rscview";
char const program_version[] = RSCVIEW_VERSION;
char const program_date[] = RSCVIEW_DATE;

nls_domain nls_default_domain = { "rscview", NULL, CHARSET_ST, NULL };


/*
 * gui variables
 */
static _WORD gl_hchar;
static _WORD gl_wchar;
static _WORD gl_wbox;
static _WORD gl_hbox;							/* system sizes */
static GRECT desk;
static _WORD phys_handle;						/* physical workstation handle */
static _WORD vdi_handle;						/* virtual screen handle */
static WS ws;
static _WORD xworkout[57];

/*
 * program options
 */
static _BOOL xml_out = FALSE;
static _BOOL verbose = FALSE;
static _BOOL use_timestamps = FALSE;
static const char *pngdir;
static const char *htmlout_name;
static const char *pnglist_name;
static const char *html_dir;
static FILE *htmlout_file;
static FILE *pnglist_file;
static _BOOL gen_imagemap;
static _BOOL aes_3d = FALSE;
#define IMAGE_PNG 0
#define IMAGE_BMP 1
static int image_format = IMAGE_PNG;

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static void open_screen(void)
{
	int i;
	_WORD pxy[8];
	_WORD workin[11];
	_WORD dummy;

	vdi_handle = phys_handle;
	for (i = 0; i < 10; i++)
		workin[i] = 1;
	workin[10] = 2;
	v_opnvwk(workin, &vdi_handle, &ws.ws_xres);
	vq_extnd(vdi_handle, 1, xworkout);
	vsf_interior(vdi_handle, FIS_SOLID);
	vsf_perimeter(vdi_handle, FALSE);
	vswr_mode(vdi_handle, MD_REPLACE);
	vsf_color(vdi_handle, G_GREEN);
	
	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = ws.ws_xres;
	pxy[3] = ws.ws_yres;
	vr_recfl(vdi_handle, pxy);

	vsf_color(vdi_handle, G_WHITE);
	
	if (aes_3d)
	{
		objc_sysvar(SV_SET, LK3DIND, FALSE, TRUE, &dummy, &dummy);
		objc_sysvar(SV_SET, LK3DACT, TRUE, FALSE, &dummy, &dummy);
		objc_sysvar(SV_SET, INDBUTCOL, G_LWHITE, 0, &dummy, &dummy);
		objc_sysvar(SV_SET, ACTBUTCOL, G_LWHITE, 0, &dummy, &dummy);
		objc_sysvar(SV_SET, BACKGRCOL, G_LWHITE, 0, &dummy, &dummy);
	} else
	{
		objc_sysvar(SV_SET, LK3DIND, FALSE, FALSE, &dummy, &dummy);
		objc_sysvar(SV_SET, LK3DACT, FALSE, FALSE, &dummy, &dummy);
		objc_sysvar(SV_SET, INDBUTCOL, G_WHITE, 0, &dummy, &dummy);
		objc_sysvar(SV_SET, ACTBUTCOL, G_WHITE, 0, &dummy, &dummy);
		objc_sysvar(SV_SET, BACKGRCOL, G_WHITE, 0, &dummy, &dummy);
	}
}

/* ------------------------------------------------------------------------- */

static void close_screen(void)
{
	v_clsvwk(vdi_handle);
}

/*** ---------------------------------------------------------------------- ***/

_WORD GetNumColors(void)
{
	static _WORD num_colors;
	
	if (num_colors == 0)
	{
		num_colors = ws.ws_ncolors;
		if (num_colors == 0 || num_colors == -1 || ws.ws_npals == 0)
			num_colors = 32766; /* more than we need */
	}
	return num_colors;
}

/* ------------------------------------------------------------------------- */

void GetTextSize(_WORD *width, _WORD *height)
{
	*width = gl_wchar;
	*height = gl_hchar;
}

/* ------------------------------------------------------------------------- */

static void clear_screen(char *title)
{
	static char empty[1] = { 0 };
	static TEDINFO tedinfo = {
		NULL, empty, empty, IBM, 1, TE_CNTR, 0x1100, 0x0, 1, 2,1
	};
	static OBJECT desktop[] = {
		{ NIL, 1, 2, G_BOX, OF_NONE, OS_NORMAL, { OBSPEC_MAKE(0, 0, G_BLACK, G_BLACK, TRUE, IP_SOLID, G_GREEN) }, 0, 0, 0, 0 },
		{ 2, NIL, NIL, G_BOX, OF_NONE, OS_NORMAL, { OBSPEC_MAKE(0, -1, G_BLACK, G_BLACK, FALSE, IP_HOLLOW, G_WHITE) }, 0, 0, 0, 0 },
		{ 0, NIL, NIL, G_TEXT, OF_LASTOB, OS_NORMAL, { (_LONG_PTR)&tedinfo }, 0, 0, 0, 0 },
	};
	int i;
	
	for (i = 0; i < 3; i++)
		desktop[i].ob_width = desk.g_x + desk.g_w;
	desktop[ROOT].ob_height = desk.g_y + desk.g_h;
	desktop[1].ob_height = gl_hchar + 2;
	desktop[2].ob_height = gl_hchar + 3;
	tedinfo.te_ptext = title;
	
	objc_draw(desktop, ROOT, MAX_DEPTH, 0, 0, desk.g_x + desk.g_w, desk.g_y + desk.g_h);
}

/* ------------------------------------------------------------------------- */

static void str_lwr(char *name)
{
	while (*name)
	{
		*name = tolower((unsigned char)*name);
		name++;
	}
}

/* ------------------------------------------------------------------------- */

static void generate_imagemap(RSCTREE *tree)
{
	OBJECT *obj;
	_WORD j;
	GRECT gr;
	_WORD dx, dy;
	
	fprintf(htmlout_file, "<map name=\"%s\">\n", tree->rt_name);
	if (tree->rt_type == RT_MENU)
	{
		obj = tree->rt_objects.menu.mn_tree;
		objc_offset(obj, ROOT, &gr.g_x, &gr.g_y);
		gr.g_w = obj[ROOT].ob_width;
		gr.g_h = obj[ROOT].ob_height;
		dx = dy = 0;
	} else
	{
		obj = tree->rt_objects.dial.di_tree;
		form_center_grect(obj, &gr);
		objc_offset(obj, ROOT, &dx, &dy);
		dx = dx - gr.g_x;
		dy = dy - gr.g_y;
	}
	
	/*
	 * if area definitions overlap, the first one will be used,
	 * so we have to output them in reverse order
	 */
	for (j = 0; !(obj[j].ob_flags & OF_LASTOB); j++)
	{
	}
	
	while (j >= 0)
	{
		const char *name;
		_WORD x, y, w, h;
		_WORD type;
		char sbuf[128];
		
		objc_offset(obj, j, &x, &y);
		x -= gr.g_x;
		y -= gr.g_y;
		w = obj[j].ob_width;
		h = obj[j].ob_height;
		type = obj[j].ob_type & OBTYPEMASK;
		/* if object #1 is a dialog title, recalc the width */
		if (tree->rt_file->rsc_emutos != EMUTOS_NONE && j == 1 && type == G_STRING && obj[j].ob_y == gl_hchar)
			w = strlen(obj[j].ob_spec.free_string) * gl_wchar;
		fprintf(htmlout_file, "<area shape=\"rect\" coords=\"%d,%d,%d,%d\" title=\"Object #%d",
			x, y,
			x + w - 1, y + h - 1,
			j);
		name = ob_name(tree->rt_file, tree, j);
		if (name)
		{
			fprintf(htmlout_file, "&#10;%s", name);
		}
		x = x - dx;
		y = y - dy;
		fprintf(htmlout_file, "&#10;type = %s", type_name(type));
		if ((obj[j].ob_type & ~OBTYPEMASK) != 0)
			fprintf(htmlout_file, "&#10;exttype = %d", (obj[j].ob_type >> 8) & 0xff);
		fprintf(htmlout_file, "&#10;x = %d", x / gl_wchar);
		if (x % gl_wchar != 0)
			fprintf(htmlout_file, " + %d", x % gl_wchar);
		fprintf(htmlout_file, "&#10;y = %d", y / gl_hchar);
		if (y % gl_hchar != 0)
			fprintf(htmlout_file, " + %d", y % gl_wchar);
		fprintf(htmlout_file, "&#10;w = %d", w / gl_wchar);
		if (w % gl_wchar != 0)
			fprintf(htmlout_file, " + %d", w % gl_wchar);
		fprintf(htmlout_file, "&#10;h = %d", h / gl_hchar);
		if (h % gl_hchar != 0)
			fprintf(htmlout_file, " + %d", h % gl_wchar);
		fprintf(htmlout_file, "&#10;flags = %s", flags_name(sbuf, obj[j].ob_flags, tree->rt_file->rsc_emutos));
		fprintf(htmlout_file, "&#10;state = %s", state_name(sbuf, obj[j].ob_state));
		
		fprintf(htmlout_file, "\" />\n");
		j--;
	}
	fprintf(htmlout_file, "</map>\n");
}

/* ------------------------------------------------------------------------- */

static _WORD write_image(RSCTREE *tree, _WORD x, _WORD y, _WORD w, _WORD h, _BOOL write_imagemap)
{
	_WORD pxy[4];
	char basename[PATH_MAX];
	char filename[PATH_MAX];
	_WORD err;
	char *p;
	
	pxy[0] = x;
	pxy[1] = y;
	pxy[2] = x + w - 1;
	pxy[3] = y + h - 1;
	vs_clip(vdi_handle, 1, pxy);
	if (use_timestamps)
	{
		time_t t = time(NULL);
		struct tm *tp = gmtime(&t);
		sprintf(basename, "%s_%04d%02d%02d%02d%02d%02d",
			tree->rt_name,
			tp->tm_year + 1900,
			tp->tm_mon + 1,
			tp->tm_mday,
			tp->tm_hour,
			tp->tm_min,
			tp->tm_sec);
	} else
	{
		strcpy(basename, tree->rt_name);
	}
	str_lwr(basename);
	if (aes_3d)
		strcat(basename, "_3d");
	p = filename;
	if (pngdir)
	{
		int len;
		
#ifdef _WIN32
		(void) _mkdir(pngdir);
#else
		(void) mkdir(pngdir, 0755);
#endif
		strcpy(p, pngdir);
		len = strlen(p);
		p += len;
		if (len > 0 && p[-1] != '/')
			*p++ = '/';
	}
	switch (image_format)
	{
	case IMAGE_PNG:
		if (tree->rt_file->rsc_nls_domain.lang)
			sprintf(p, "%03ld_%s_%s.png", tree->rt_number, tree->rt_file->rsc_nls_domain.lang, basename);
		else
			sprintf(p, "%03ld_%s.png", tree->rt_number, basename);
		err = v_write_png(vdi_handle, filename);
		break;
	case IMAGE_BMP:
		if (tree->rt_file->rsc_nls_domain.lang)
			sprintf(p, "%03ld_%s_%s.bmp", tree->rt_number, tree->rt_file->rsc_nls_domain.lang, basename);
		else
			sprintf(p, "%03ld_%s.bmp", tree->rt_number, basename);
		err = v_write_bmp(vdi_handle, filename);
		break;
	default:
		return EINVAL;
	}
	if (verbose)
		printf("%s %ld %s: %dx%d -> %s\n", rtype_name(tree->rt_type), tree->rt_number, tree->rt_name, w, h, filename);
	if (err != 0)
	{
		KINFO(("write_image: %s: %s\n", filename, strerror(err)));
	}
	if (htmlout_file)
	{
		fprintf(htmlout_file, "<p id=\"%s\">%s:<br /><img src=\"%s/%s\" alt=\"%s\"",
			tree->rt_name, tree->rt_name,
			html_dir ? html_dir : ".", p,
			tree->rt_name);
		if (write_imagemap)
		{
			fprintf(htmlout_file, " usemap=\"#%s\"", tree->rt_name);
		}
		fprintf(htmlout_file, " /></p>\n");
		if (write_imagemap)
		{
			generate_imagemap(tree);
		}
	}
	if (pnglist_file)
	{
		fprintf(pnglist_file, "$files[] = array('num' => %ld, 'file' => '%s', 'name' => '%s');\n", tree->rt_number, p, tree->rt_name);
	}
	return err;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

/*
 * track which areas are modified by calls to objc_draw().
 * form_center does not take into account rectangles drawn
 * around the ROOT object, and there is no other way
 * to obtain the outer dimensions of what is drawn
 */
static void start_drawrect(void)
{
	_WORD pxy[4];

	pxy[0] = desk.g_x;
	pxy[1] = desk.g_y;
	pxy[2] = desk.g_x + desk.g_w - 1;
	pxy[3] = desk.g_y + desk.g_h - 1;
	vs_drawrect(phys_handle, 1, pxy);
}

/* ------------------------------------------------------------------------- */

static void end_drawrect(GRECT *gr)
{
	_WORD pxy[4];

	vs_drawrect(phys_handle, 0, pxy);
	gr->g_x = pxy[0];
	gr->g_y = pxy[1];
	gr->g_w = pxy[2] - pxy[0] + 1;
	gr->g_h = pxy[3] - pxy[1] + 1;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_dialog(RSCTREE *tree, EXTOB_MODE mode)
{
	OBJECT *ob;
	GRECT gr;
	_WORD err;

	ob = tree->rt_objects.dial.di_tree;
	if (ob == NULL)
		return FALSE;
	form_center_grect(ob, &gr);

	wind_update(BEG_UPDATE);
	form_dial_grect(FMD_START, &gr, &gr);
	
	clear_screen(tree->rt_name);
	
	start_drawrect();
	if (mode == EXTOB_NONE || mode == EXTOB_AES)
		objc_draw_grect(ob, ROOT, MAX_DEPTH, &desk);
	else
		ob_draw_dialog(phys_handle, ob, ROOT, MAX_DEPTH, &desk, mode);
	end_drawrect(&gr);

	err = write_image(tree, gr.g_x, gr.g_y, gr.g_w, gr.g_h, gen_imagemap);

	form_dial_grect(FMD_FINISH, &gr, &gr);
	wind_update(END_UPDATE);

	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_menu(RSCTREE *tree)
{
	OBJECT *ob;
	_WORD thebar;
	_WORD theactive;
	_WORD themenus;
	_WORD title, menubox;
	_WORD x;
	GRECT gr;
	_WORD err;
	_WORD maxx, maxy;
	
	ob = tree->rt_objects.menu.mn_tree;
	if (ob == NULL)
		return FALSE;
	/*
	 * set the width of the root object, the titlebar, and the background box
	 * to screen width
	 */
	ob[ROOT].ob_width = ob[menu_the_bar(ob)].ob_width = ob[menu_the_menus(ob)].ob_width = desk.g_x + desk.g_w;
	/*
	 * adjust the height of the root object and the
	 * background box to fill up the screen
	 */
	ob[ROOT].ob_height = desk.g_y + desk.g_h;
	ob[menu_the_menus(ob)].ob_height = ob[ROOT].ob_height - ob[menu_the_menus(ob)].ob_y;

	objc_offset(ob, ROOT, &gr.g_x, &gr.g_y);
	gr.g_w = ob[ROOT].ob_width;
	gr.g_h = ob[ROOT].ob_height;

	wind_update(BEG_UPDATE);
	
	/*
	 * draw the menu titles
	 */
	clear_screen(tree->rt_name);
	menu_bar(ob, TRUE);

	/*
	 * reposition the submenu boxes so that they don't overlap
	 */
	thebar = menu_the_bar(ob);
	if (thebar == NIL)
		return FALSE;
	themenus = ob[thebar].ob_next;
	if (themenus == thebar)
		return FALSE;
	theactive = ob[thebar].ob_head;
	if (theactive == NIL)
		return FALSE;
	title = ob[theactive].ob_head;
	if (title == NIL)
		return FALSE;
	menubox = ob[themenus].ob_head;
	if (menubox == NIL)
		return FALSE;
	x = ob[menubox].ob_x;
	do
	{
		ob[menubox].ob_x = x;
		/* ob[title].ob_x = x + gl_wchar; */
		x += ob[menubox].ob_width + 1;
		title = ob[title].ob_next;
		menubox = ob[menubox].ob_next;
	} while (title != theactive && menubox != themenus);
	
	/*
	 * draw the boxes
	 */
	maxx = maxy = 0;
	menubox = ob[themenus].ob_head;
	do
	{
		_WORD mx, my, mw, mh;
		
		objc_offset(ob, menubox, &mx, &my);
		mw = ob[menubox].ob_width;
		mh = ob[menubox].ob_height;
		mx -= 1;
		my -= 1;
		mw += 2;
		mh += 2;
		objc_draw(ob, menubox, MAX_DEPTH, mx, my, mw, mh);
		menubox = ob[menubox].ob_next;
		mx = mx + mw;
		my = my + mh;
		if (mx > maxx)
			maxx = mx;
		if (my > maxy)
			maxy = my;
	} while (menubox != themenus);
	
	err = write_image(tree, 0, 0, maxx, maxy, gen_imagemap);

	menu_bar(ob, FALSE);
	form_dial_grect(FMD_FINISH, &gr, &gr);
	wind_update(END_UPDATE);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_string(RSCTREE *tree)
{
	const char *str;
	_WORD err;
	GRECT gr;
	TEDINFO ted = { NULL, NULL, NULL, IBM, 0, TE_CNTR, COLSPEC_MAKE(G_BLACK, G_BLACK, TEXT_OPAQUE, 0, G_WHITE), 0, 0, 0, 0 };
	OBJECT string[1] = { { NIL, NIL, NIL, G_TEXT, OF_LASTOB, OS_NORMAL, { 0 }, 0, 0, 0, 0 } };
	_WORD len;
	
	str = tree->rt_objects.str.fr_str;
	if (str == NULL)
		return FALSE;
	str = nls_dgettext(&tree->rt_file->rsc_nls_domain, str);
	
	ted.te_ptext = (char *)NO_CONST(str);
	string[0].ob_spec.tedinfo = &ted;
	len = strlen(str);
	string[0].ob_width = len * gl_wchar;
	string[0].ob_height = gl_hchar;
	form_center(string, &string[ROOT].ob_x, &string[ROOT].ob_y, &gr.g_w, &gr.g_h);
	gr.g_x = string[ROOT].ob_x;
	gr.g_y = string[ROOT].ob_y;
	
	clear_screen(tree->rt_name);
	objc_draw_grect(string, ROOT, MAX_DEPTH, &gr);

	err = write_image(tree, gr.g_x, gr.g_y, gr.g_w, gr.g_h, FALSE);

	form_dial_grect(FMD_FINISH, &gr, &gr);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

/*
 * the logic of these functions should match the ones in gemfmalt.c
 */
#define MAX_LINENUM     5
#define MAX_LINELEN     40
#define MAX_BUTNUM      3
#define MAX_BUTLEN      10
#define TOS_MAX_LINELEN 32
#define TOS_MAX_BUTLEN      10

#define endstring(a)	( ((a)==']') || ((a)=='\0') )
#define endsubstring(a) ( ((a)=='|') || ((a)==']') || ((a)=='\0') )
#define isduplicate(a,b) ( (a!='\0') && ((a)==(b)) )

struct button_translation {
	char treename[MAXNAMELEN + 1];
	struct button_translation *next;
};

struct alert_button {
	char treename[MAXNAMELEN + 1];
	char english[MAX_BUTLEN + 1];
	char translation[MAX_BUTLEN + 1];
	int buttonnum;
	struct alert_button *next;
};

struct alert_button *alert_buttons;

static void add_button_translation(RSCTREE *tree, int buttonnum, struct alert_button *english, const char *translation)
{
	struct alert_button *button;
	
	(void)buttonnum;
	for (button = alert_buttons; button; button = button->next)
	{
		if (strcmp(english->english, button->english) == 0)
		{
			if (button->translation[0])
			{
				if (strcmp(translation, button->translation) != 0)
				{
					char *str1 = nls_conv_to_utf8(tree->rt_file->rsc_nls_domain.fontset, translation, STR0TERM, quote_html);
					char *str2 = nls_conv_to_utf8(tree->rt_file->rsc_nls_domain.fontset, button->translation, STR0TERM, quote_html);
					KINFO(("Warning: translation %s in %s was translated to %s in %s\n", str1, tree->rt_name, str2, button->treename));
					g_free(str2);
					g_free(str1);
				}
			} else
			{
				strcpy(button->translation, translation);
			}
			return;
		}
	}
	KINFO(("internal error: button %s not found\n", english->english));
}


static struct alert_button *add_button(RSCTREE *tree, int buttonnum, const char *name)
{
	struct alert_button *button;
	
	for (button = alert_buttons; button; button = button->next)
		if (strcmp(name, button->english) == 0)
			return button;
	button = g_new(struct alert_button, 1);
	strcpy(button->treename, tree->rt_name);
	strcpy(button->english, name);
	button->translation[0] = '\0';
	button->buttonnum = buttonnum;
	button->next = alert_buttons;
	alert_buttons = button;
	return button;
}


static const char *fm_strbrk(RSCTREE *tree, int is_button, int is_translation, _WORD maxnum, _WORD maxlen, _WORD maxtoslen, const char *alert, _WORD *pnum, _WORD *plen, struct alert_button **buttons)
{
	int i, len;
	char *p;
	char buf[MAX_LINELEN + 1];
	const char *which = is_button ? "button" : "line";
	const char *type = is_translation ? "translation" : "definition";
	
	*plen = 0;

	if (*alert == '[')					/* ignore a leading [ */
		alert++;

	for (i = 0; ; )
	{
		p = buf;
		for (len = 0; ; )
		{
			if (endsubstring(*alert))
			{
                if (isduplicate(*alert, *(alert+1)))
                {
                    alert++;        /* || or ]] found: skip a character */
                } else
                {
					break;
				}
			}
			if (len == maxtoslen)
			{
				KINFO(("Warning: alert %s #%d in %s of %s exceeds TOS standard length %d\n", which, i, type, tree->rt_name, maxtoslen));
			} else if (len == maxlen)
			{
				KINFO(("Warning: alert %s #%d in %s of %s exceeds EmuTOS max length %d\n", which, i, type, tree->rt_name, maxlen));
			}
			len++;
			if (len < maxlen)
				*p++ = *alert;
			alert++;
		}
		*p = '\0';

		if (len > *plen)				/* track max substring length */
			*plen = len;

		if (is_button)
		{
			g_strchomp(buf);
			g_strchug(buf);
			if (is_button && i < maxnum)
			{
				struct alert_button *button;
				
				if (is_translation)
				{
					if (buttons[i])
						add_button_translation(tree, i, buttons[i], buf);
				} else
				{
					button = add_button(tree, i, buf);
					if (i < maxnum)
						buttons[i] = button;
				}
			}
		}

		i++;
		if (endstring(*alert))			/* end of all substrings */
		{
			break;
		}
		alert++;
	}
	if (i > maxnum)					/* too many substrings */
	{
		KINFO(("Warning: %s of %s has more than %d substrings\n", type, tree->rt_name, maxnum));
	}

	for (;;)							/* eat any remaining characters */
	{
		if (endstring(*alert))
			break;
		alert++;
	}

	if (is_button && is_translation && *pnum != i)
	{
		KINFO(("Warning: %s of %s has %d instead of %d buttons\n", type, tree->rt_name, i, *pnum));
	}
	*pnum = i;	/* count of substrings found */

	if (*alert)							/* if not at null byte, */
		alert++;						/* point to next one    */

	return alert;
}


static void fm_parse(RSCTREE *tree, const char *alert, int is_translation, struct alert_button **buttons, _WORD *numbut)
{
	_WORD nummsg;
	_WORD mlenmsg;
	_WORD mlenbut;
	const char *type = is_translation ? "translation" : "definition";

	if (alert[1] < '0' || alert[1] > '3')
	{
		KINFO(("Warning: invalid icon number 0x%02x in %s of %s\n", (unsigned char)alert[1], type, tree->rt_name));
	}

	alert = fm_strbrk(tree, FALSE, is_translation, MAX_LINENUM, MAX_LINELEN, TOS_MAX_LINELEN, alert + 3, &nummsg, &mlenmsg, NULL);
	fm_strbrk(tree, TRUE, is_translation, MAX_BUTNUM, MAX_BUTLEN, TOS_MAX_BUTLEN, alert, numbut, &mlenbut, buttons);
}


static _BOOL draw_alert(RSCTREE *tree, EXTOB_MODE mode)
{
	const char *str;
	const char *translation;
	_WORD err;
	GRECT gr;
	struct alert_button *buttons[MAX_BUTNUM];
	int i;
	_WORD numbut;

	str = tree->rt_objects.alert.al_str;
	if (str == NULL)
		return FALSE;
	translation = nls_dgettext(&tree->rt_file->rsc_nls_domain, str);

	/*
	 * parse the alert string, so we can print warnings if needed.
	 */
	for (i = 0; i < MAX_BUTNUM; i++)
		buttons[i] = NULL;
	fm_parse(tree, str, FALSE, buttons, &numbut);
	if (str != translation)
		fm_parse(tree, translation, TRUE, buttons, &numbut);

	clear_screen(tree->rt_name);
	/*
	 * call our special version that only displays the dialog,
	 * and does not restore the screen background.
	 */
	start_drawrect();
	if (mode == EXTOB_NONE || mode == EXTOB_AES)
		form_alert_ex(1, translation, 1 | (tree->rt_file->rsc_emutos != EMUTOS_NONE ? 2 : 0));
	else
		ob_draw_alert(phys_handle, 1, translation, &desk, mode);
	end_drawrect(&gr);
	
	err = write_image(tree, gr.g_x, gr.g_y, gr.g_w, gr.g_h, FALSE);

	form_dial_grect(FMD_FINISH, &gr, &gr);
	
	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_image(RSCTREE *tree)
{
	_WORD err;
	BITBLK *bit;
	_WORD *data;
	_WORD width;
	_WORD height;
	GRECT gr;
	_WORD pxy[8];
	_WORD colors[2];
	MFDB src, dst;

	bit = tree->rt_objects.bit;
	data = bit->bi_pdata;
	width = bit->bi_wb * 8;
	height = bit->bi_hl;
	if (is_mouseform(bit))
	{
		data += 5;
		height -= 5;
	}

	clear_screen(tree->rt_name);

	gr.g_x = (desk.g_w - width) / 2 + desk.g_x;
	gr.g_y = (desk.g_h - height) / 2 + desk.g_y;
	gr.g_w = width;
	gr.g_h = height;

	pxy[0] = gr.g_x;
	pxy[1] = gr.g_y;
	pxy[2] = gr.g_x + gr.g_w - 1;
	pxy[3] = gr.g_y + gr.g_h - 1;
	vs_clip(vdi_handle, 1, pxy);
	vswr_mode(vdi_handle, MD_REPLACE);
	vsf_color(vdi_handle, G_WHITE);
	vr_recfl(vdi_handle, pxy);

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = gr.g_w - 1;
	pxy[3] = gr.g_h - 1;
	pxy[4] = gr.g_x;
	pxy[5] = gr.g_y;
	pxy[6] = gr.g_x + gr.g_w - 1;
	pxy[7] = gr.g_y + gr.g_h - 1;

	src.fd_w = width;
	src.fd_h = height;
	src.fd_nplanes = 1;
	src.fd_wdwidth = (src.fd_w + 15) >> 4;
	src.fd_stand = FALSE;
	src.fd_addr = data;

	dst.fd_w = ws.ws_xres + 1;
	dst.fd_h = ws.ws_yres + 1;
	dst.fd_nplanes = xworkout[4];
	dst.fd_wdwidth = (dst.fd_w + 15) >> 4;
	dst.fd_stand = FALSE;
	dst.fd_addr = 0;

	colors[0] = G_BLACK;
	colors[1] = G_WHITE;
	vrt_cpyfm(vdi_handle, MD_TRANS, pxy, &src, &dst, colors);

	err = write_image(tree, gr.g_x, gr.g_y, gr.g_w, gr.g_h, FALSE);

	return err == 0;
}

/* ------------------------------------------------------------------------- */

static _BOOL draw_all_trees(RSCFILE *file)
{
	RSCTREE *tree;
	_BOOL ret = TRUE;
	
	if (pnglist_file)
	{
		fprintf(pnglist_file, "<?php\n");
	}
	FOR_ALL_RSC(file, tree)
	{
		switch (tree->rt_type)
		{
		case RT_DIALOG:
		case RT_FREE:
		case RT_UNKNOWN:
			ret &= draw_dialog(tree, file->rsc_extob.mode);
			break;
		case RT_MENU:
			ret &= draw_menu(tree);
			break;
		case RT_FRSTR:
			ret &= draw_string(tree);
			break;
		case RT_ALERT:
			ret &= draw_alert(tree, file->rsc_extob.mode);
			break;
		case RT_FRIMG:
		case RT_MOUSE:
			ret &= draw_image(tree);
			break;
		case RT_BUBBLEMORE:
		case RT_BUBBLEUSER:
			break;
		}
	}
	if (pnglist_file)
	{
		fprintf(pnglist_file, "?>\n");
	}
	while (alert_buttons != NULL)
	{
		struct alert_button *alert = alert_buttons;
		alert_buttons = alert->next;
		free(alert);
	}
	return ret;
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

#define ADJ3DSTD    2   /* standard pixel adjustment for 3D objects */

static RSCTREE *tree_find(RSCFILE *file, const char *name)
{
	RSCTREE *tree;

	FOR_ALL_RSC(file, tree)
	{
		switch (tree->rt_type)
		{
		case RT_DIALOG:
		case RT_FREE:
		case RT_UNKNOWN:
		case RT_MENU:
			if (strcmp(tree->rt_name, name) == 0)
				return tree;
			break;
		}
	}
	errout(_("%s: tree not found: %s\n"), program_name, name);
	return NULL;
}

/*** ---------------------------------------------------------------------- ***/

static RSCTREE *str_find(RSCFILE *file, const char *name)
{
	RSCTREE *tree;

	FOR_ALL_RSC(file, tree)
	{
		switch (tree->rt_type)
		{
		case RT_FRSTR:
			if (strcmp(tree->rt_name, name) == 0)
				return tree;
			break;
		}
	}
	errout(_("%s: string not found: %s\n"), program_name, name);
	return NULL;
}

/*** ---------------------------------------------------------------------- ***/

static OBJECT *obj_with_name(RSCFILE *file, RSCTREE *tree, const char *name)
{
	const char *p;
	_WORD ob;
	
	if (file == NULL || tree == NULL)
		return NULL;
	p = tree->rt_obnames;
	for (ob = 0; ob < tree->rt_nobs; ob++, p += MAXNAMELEN + 1)
		if (strcmp(p, name) == 0)
			return tree->rt_objects.dial.di_tree + ob;
	errout(_("%s: object not found: %s\n"), program_name, name);
	return NULL;
}

/*** ---------------------------------------------------------------------- ***/

/*
 * Fix the file-selector dialog from EmuTOS,
 * for displaying 3D-objects.
 * Should be synced with fs_start() in emutos/aes/gemfslib.c
 */
static void fix_emutos_aes(RSCFILE *file)
{
	RSCTREE *tree;
	OBJECT *obj;
	OBJECT *filearea = NULL;
	TEDINFO *ted;
	_WORD i, row;
	OBJECT *root;

	tree = tree_find(file, "FSELECTR");
	if (tree == NULL)
		return;
	root = tree->rt_objects.dial.di_tree;

	obj = obj_with_name(file, tree, "FSTITLE");
	if (obj != NULL && obj->ob_type == G_STRING)
	{
		RSCTREE *str;
		_WORD len;

		str = str_find(file, "ITEMSLCT");
		if (str != NULL)
		{
			obj->ob_spec.free_string = str->rt_objects.str.fr_str;
			len = strlen(obj->ob_spec.free_string) * gl_wchar;
			if (len > root->ob_width)
				len = root->ob_width;
			obj->ob_x = (root->ob_width - len) / 2;
		}
	}

	if (!aes_3d)
		return;

	/*
	 * adjust the positions and/or dimensions of all the objects within
	 * FILEAREA (the IBOX that contains the closer, title, scroll bar,
	 * and filenames), plus FILEAREA itself
	 *
	 * at the same time, we set any 3D object bits required
	 */
	obj = obj_with_name(file, tree, "FCLSBOX");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DACT;
		obj->ob_x += ADJ3DSTD;
		obj->ob_y += ADJ3DSTD;
	}
	
	obj = obj_with_name(file, tree, "FTITLE");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DBAK;
		obj->ob_x += 2 * ADJ3DSTD - 1;
		obj->ob_height += 2 * ADJ3DSTD;
		/* use pattern 4, just like TOS4 */
		ted = obj->ob_spec.tedinfo;
		ted->te_color = (ted->te_color & 0xff8f) | (IP_4PATT<<4);
	}

	obj = obj_with_name(file, tree, "FILEBOX");
	if (obj != NULL)
	{
		obj->ob_y += 3 * ADJ3DSTD - 1;
		obj->ob_width += ADJ3DSTD;
	}

	obj = obj_with_name(file, tree, "SCRLBAR");
	if (obj != NULL)
	{
		obj->ob_x += 2 * ADJ3DSTD - 1;
		obj->ob_y += 3 * ADJ3DSTD - 1;
	}

	obj = obj_with_name(file, tree, "FUPAROW");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DACT;
		obj->ob_x += ADJ3DSTD;
		obj->ob_y += ADJ3DSTD;
		obj->ob_width -= 2 * ADJ3DSTD;
	}
	
	obj = obj_with_name(file, tree, "FSVSLID");
	if (obj != NULL)
	{
		obj->ob_y += 3 * ADJ3DSTD;
		obj->ob_height -= 6 * ADJ3DSTD;
		/* use pattern 4, just like TOS4 */
		obj->ob_spec.index = (obj->ob_spec.index & 0xffffff8fL) | (IP_4PATT<<4);
	}

	obj = obj_with_name(file, tree, "FSVELEV");
	if (obj != NULL)
	{
		/* we only adjust x/w of FSVELEV here, because y/h are set dynamically */
		obj->ob_flags |= OF_FL3DACT;
		obj->ob_x += ADJ3DSTD;
		obj->ob_width -= 2 * ADJ3DSTD;
		/* obj->ob_height -= 2 * ADJ3DSTD; */
	}

	obj = obj_with_name(file, tree, "FDNAROW");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DACT;
		obj->ob_x += ADJ3DSTD;
		obj->ob_y -= ADJ3DSTD;
		obj->ob_width -= 2 * ADJ3DSTD;
	}

	obj = obj_with_name(file, tree, "FILEAREA");
	if (obj != NULL)
	{
		filearea = obj;
		obj->ob_y -= 6;		/* squeeze things together vertically */
		obj->ob_width += 2 * ADJ3DSTD - 1;
		obj->ob_height += 3 * ADJ3DSTD - 1;
	}

	/*
	 * adjust the position of all the drive-letter boxes, plus FSDRIVES
	 * (the IBOX that includes them)
	 *
	 * at the same time we mark them as 3D indicators
	 */
	obj = obj_with_name(file, tree, "FS1STDRV");
	if (obj != NULL)
	{
		_WORD first_y = obj->ob_y;
		_ULONG drivebits = 0x105; /* just some arbitrary value for display purposes */

		for (i = 0, row = 0; obj->ob_type == G_BOXCHAR; i++, obj++, row++)
		{
			if (obj->ob_y == first_y)
				row = 0;
			obj->ob_flags |= OF_FL3DIND;
			obj->ob_x += 2 * ADJ3DSTD;
			obj->ob_y += 2 * ADJ3DSTD + row * (3 * ADJ3DSTD - 1);
			if (!(drivebits & 1))
				obj->ob_state |= OS_DISABLED;
			if (i == 2)
				obj->ob_state |= OS_SELECTED;
			drivebits >>= 1;
		}
	}

#if 0
	/* FSDRIVES is an IBOX and invisible; no need to adjust it */
	obj = obj_with_name(file, tree, "FSDRIVES");
	if (obj != NULL)
	{
		obj->ob_height += 2 * ADJ3DSTD + DRIVE_ROWS * (3 * ADJ3DSTD - 1);
		obj->ob_width += 4 * ADJ3DSTD;
	}
#endif

	/*
	 * finally, handle the remaining objects that have ROOT as a parent, plus ROOT
	 *
	 * FSOK/FSCANCEL must move left to avoid interfering with FSDRIVES in lower resolutions
	 */
	obj = obj_with_name(file, tree, "FSDIRECT");
	if (obj != NULL)
		obj->ob_flags |= OF_FL3DBAK;
	obj = obj_with_name(file, tree, "FSSELECT");
	if (obj != NULL)
		obj->ob_flags |= OF_FL3DBAK;
	obj = obj_with_name(file, tree, "FSDRVTXT");
	if (obj != NULL)
		obj->ob_flags |= OF_FL3DBAK;

	obj = obj_with_name(file, tree, "FSOK");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DACT;
		if (filearea != NULL)
			obj->ob_x = filearea->ob_x;
		obj->ob_y += ADJ3DSTD;
	}

	obj = obj_with_name(file, tree, "FSCANCEL");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DACT;
		if (filearea != NULL)
			obj->ob_x = filearea->ob_x + filearea->ob_width - obj->ob_width;
		obj->ob_y += ADJ3DSTD;
	}
	
	obj = root;
	obj->ob_flags |= OF_FL3DBAK;
    obj->ob_height += 2 * ADJ3DSTD;
}


/*
 * Fix the dialogs from EmuTOS,
 * for displaying 3D-objects.
 * Should be synced with adjust_3d_objects() in emutos/desk/deskmain.c
 */
static void fix_emutos_desk(RSCFILE *file)
{
	RSCTREE *tree;
	OBJECT *obj;

	tree = tree_find(file, "ADDESKCF");
	if (tree == NULL)
		return;

	if (!aes_3d)
		return;

	obj = obj_with_name(file, tree, "DCFUNPRV");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DACT;
		obj->ob_y -= 2 * ADJ3DSTD;
	}

	obj = obj_with_name(file, tree, "DCFUNNXT");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DACT;
		obj->ob_y += ADJ3DSTD;
	}

	obj = obj_with_name(file, tree, "DCMNUPRV");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DACT;
		obj->ob_y -= 2 * ADJ3DSTD;
	}

	obj = obj_with_name(file, tree, "DCMNUNXT");
	if (obj != NULL)
	{
		obj->ob_flags |= OF_FL3DACT;
		obj->ob_y += ADJ3DSTD;
	}
}

#undef ADJ3DSTD

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

enum rscview_opt {
	OPT_VERBOSE = 'v',
	OPT_XML = 'X',
	OPT_LANG = 'l',
	OPT_PODIR = 'p',
	OPT_PNGDIR = 'P',
	OPT_CHARSET = 'c',
	OPT_3D = '3',
	OPT_TIMESTAMPS = 'T',
	OPT_VERSION = 'V',
	OPT_HELP = 'h',
	
	OPT_SETVAR = 0,
	OPT_OPTERROR = '?',
	
	OPT_CREATE_HTML = 256,
	OPT_QUOTE_HTML,
	OPT_HTML_DIR,
	OPT_IMAGEMAP,
	OPT_CREATE_PNGLIST,
	OPT_REPORT_PO,
	OPT_REPORT_RSC,
	OPT_IMAGE_FORMAT
};

static struct option const long_options[] = {
	{ "xml", no_argument, NULL, OPT_XML },
	{ "verbose", no_argument, NULL, OPT_VERBOSE },
	{ "lang", required_argument, NULL, OPT_LANG },
	{ "podir", required_argument, NULL, OPT_PODIR },
	{ "pngdir", required_argument, NULL, OPT_PNGDIR },
	{ "charset", required_argument, NULL, OPT_CHARSET },
	{ "create-html", required_argument, NULL, OPT_CREATE_HTML },
	{ "create-pnglist", required_argument, NULL, OPT_CREATE_PNGLIST },
	{ "quote-html", no_argument, NULL, OPT_QUOTE_HTML },
	{ "imagemap", no_argument, NULL, OPT_IMAGEMAP },
	{ "html-dir", required_argument, NULL, OPT_HTML_DIR },
	{ "timestamps", no_argument, NULL, OPT_TIMESTAMPS },
	{ "report-po", no_argument, NULL, OPT_REPORT_PO },
	{ "report-rsc", no_argument, NULL, OPT_REPORT_RSC },
	{ "3d", no_argument, NULL, OPT_3D },
	{ "image-format", required_argument, NULL, OPT_IMAGE_FORMAT },
	{ "version", no_argument, NULL, OPT_VERSION },
	{ "help", no_argument, NULL, OPT_HELP },
	{ NULL, no_argument, NULL, 0 }
};

/* ------------------------------------------------------------------------- */

static void usage(FILE *fp)
{
	fprintf(fp, _("%s - Create png files from GEM resource files\n"), program_name);
	fprintf(fp, _("Usage: %s [<options>] <file...>\n"), program_name);
	fprintf(fp, _("Options:\n"));
	fprintf(fp, _("   -v, --verbose               emit some progress messages\n"));
	fprintf(fp, _("   -l, --lang <lang>           read <lang>.po for translation\n")); 
	fprintf(fp, _("   -p, --podir <dir>           lookup po-files in <dir>\n"));
	fprintf(fp, _("   -c, --charset <name>        use <charset> for display, overriding entry from po-file\n"));
	fprintf(fp, _("   -P, --pngdir <dir>          write output files to <dir>\n"));
	fprintf(fp, _("   -T, --timestamps            use timestamps in filenames\n"));
	fprintf(fp, _("   -3, --3d                    use AES 3D flags\n"));
	fprintf(fp, _("       --create-html <file>    create HTML file with references to images\n"));
	fprintf(fp, _("       --imagemap              create an imagemap for use in HTML\n"));
	fprintf(fp, _("       --create-pnglist <file> create PHP scriptlet for written images\n"));
	fprintf(fp, _("       --report-po             report inconsistencies in PO files\n"));
	fprintf(fp, _("       --report-rsc            report inconsistencies in RSC files\n"));
	fprintf(fp, _("       --version               print version and exit\n"));
	fprintf(fp, _("       --help                  print this help and exit\n"));
}

/* ------------------------------------------------------------------------- */

static void stdout_handler(void *data, const char *fmt, va_list args)
{
	vfprintf((FILE *)data, fmt, args);
}

/* ------------------------------------------------------------------------- */

static void print_version(void)
{
	printf(_("%s version %s, %s\n"), program_name, program_version, program_date);
	set_errout_handler(stdout_handler, stdout);
	writepng_version_info();
}

/* ------------------------------------------------------------------------- */

int main(int argc, char **argv)
{
	int c;
	RSCFILE *file;
	const char *filename;
	int exit_status = EXIT_SUCCESS;
	const char *lang = NULL;
	const char *po_dir = NULL;
	const char *charset = NULL;
	_UWORD load_flags = XRSC_SAFETY_CHECKS;
	
	while ((c = getopt_long_only(argc, argv, "c:l:p:P:TvX3hV", long_options, NULL)) != EOF)
	{
		switch ((enum rscview_opt) c)
		{
		case OPT_XML:
			xml_out = TRUE;
			break;
		
		case OPT_CHARSET:
			charset = optarg;
			break;
			
		case OPT_LANG:
			lang = optarg;
			break;
		
		case OPT_PODIR:
			po_dir = optarg;
			break;
		
		case OPT_PNGDIR:
			pngdir = optarg;
			break;
		
		case OPT_CREATE_HTML:
			htmlout_name = optarg;
			break;
		
		case OPT_QUOTE_HTML:
			quote_html |= QUOTE_HTML;
			break;
		
		case OPT_CREATE_PNGLIST:
			pnglist_name = optarg;
			break;
		
		case OPT_HTML_DIR:
			html_dir = optarg;
			break;
		
		case OPT_IMAGEMAP:
			gen_imagemap = TRUE;
			break;
			
		case OPT_VERBOSE:
			verbose = TRUE;
			break;
		
		case OPT_TIMESTAMPS:
			use_timestamps = TRUE;
			break;
		
		case OPT_REPORT_PO:
			load_flags |= XRSC_REPORT_PO;
			break;
		
		case OPT_REPORT_RSC:
			load_flags |= XRSC_REPORT_RSC;
			break;
		
		case OPT_3D:
			aes_3d = TRUE;
			break;
		
		case OPT_IMAGE_FORMAT:
			if (strcmp(optarg, "png") == 0)
			{
				image_format = IMAGE_PNG;
			} else if (strcmp(optarg, "bmp") == 0)
			{
				image_format = IMAGE_BMP;
			} else
			{
				usage(stderr);
				return EXIT_FAILURE;
			}
			break;

		case OPT_VERSION:
			print_version();
			return EXIT_SUCCESS;
		
		case OPT_HELP:
			usage(stdout);
			return EXIT_SUCCESS;

		case OPT_SETVAR:
			/* option which just sets a var */
			break;
		
		case OPT_OPTERROR:
		default:
			usage(stderr);
			return EXIT_FAILURE;
		}
	}
	
	if (optind >= argc)
	{
		errout(_("%s: missing arguments\n"), program_name);
		return EXIT_FAILURE;
	}
	
	if (htmlout_name)
	{
		htmlout_file = fopen(htmlout_name, "w");
		if (htmlout_file == NULL)
		{
			errout(_("%s: %s: %s\n"), program_name, htmlout_name, strerror(errno));
			return EXIT_FAILURE;
		}
	}
	
	if (pnglist_name)
	{
		pnglist_file = fopen(pnglist_name, "w");
		if (pnglist_file == NULL)
		{
			errout(_("%s: %s: %s\n"), program_name, pnglist_name, strerror(errno));
			return EXIT_FAILURE;
		}
	}
	
	po_init(po_dir, lang != NULL, TRUE);
	appl_init();
	
	menu_register(-1, program_name);
	phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
	wind_get(DESK, WF_WORKXYWH, &desk.g_x, &desk.g_y, &desk.g_w, &desk.g_h);

	while (optind < argc)
	{
		filename = argv[optind++];
		file = load_all(filename, gl_wchar, gl_hchar, lang, load_flags, po_dir);
		if (file != NULL)
		{
			if (file->rsc_emutos == EMUTOS_AES)
			{
				fix_emutos_aes(file);
			}
			if (file->rsc_emutos == EMUTOS_DESK)
			{
				fix_emutos_desk(file);
			}
			if (charset)
			{
				int cset = po_get_charset_id(charset);
				if (cset >= 0)
					file->rsc_nls_domain.fontset = cset;
			}
			if (xml_out)
			{
				char outfilename[PATH_MAX];
				rsc_counter counter;
				
				strcpy(outfilename, file->rsc_rsxfilename);
				set_extension(outfilename, "xml");
				if (rsc_xml_source(file, &counter, outfilename, file->data) == FALSE)
					exit_status = EXIT_FAILURE;
			} else
			{
				open_screen();
				vst_font(vdi_handle, file->rsc_nls_domain.fontset);
				vst_font(phys_handle, file->rsc_nls_domain.fontset);
				
				if (draw_all_trees(file) == FALSE)
					exit_status = EXIT_FAILURE;
				
				vst_font(phys_handle, 1);
				close_screen();
			}
							
			rsc_file_delete(file, FALSE);
			xrsrc_free(file);
		} else
		{
			exit_status = EXIT_FAILURE;
		}
	}
	
	appl_exit();
	po_exit();
		
	if (htmlout_file != NULL)
	{
		fclose(htmlout_file);
		htmlout_file = NULL;
	}
	
	if (pnglist_file != NULL)
	{
		fclose(pnglist_file);
		pnglist_file = NULL;
	}
	
	return exit_status;
}
