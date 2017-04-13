/*****************************************************************************
 * XMLOUT.C
 *****************************************************************************/

#include "config.h"
#include <gem.h>
#include <mobject.h>
#include <object.h>
#include <s_endian.h>
#include <time.h>
#include "fileio.h"
#include "rsc.h"

static char const default_header1[] = \
	"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
	"<!DOCTYPE rscfile SYSTEM \"http://www.tho-otto.de/dtd/resource.dtd\" [\n"
	"<!ENTITY nul \"&#9216;\">              <!-- 0x00 U+2400 -->\n"
	"<!ENTITY uparrow \"&#8679;\">          <!-- 0x01 U+21E7 -->\n"
	"<!ENTITY downarrow \"&#8681;\">        <!-- 0x02 U+21E9 -->\n"
	"<!ENTITY rightarrow \"&#8680;\">       <!-- 0x03 U+21E8 -->\n"
	"<!ENTITY leftarrow \"&#8678;\">        <!-- 0x04 U+21E6 -->\n"
	"<!ENTITY ballotbox \"&#9744;\">        <!-- 0x05 U+2610 -->\n"
	"<!ENTITY ballotboxcheck \"&#9745;\">   <!-- 0x06 U+2611 -->\n"
	"<!ENTITY ballotboxx \"&#9746;\">       <!-- 0x07 U+2612 -->\n"
	"<!ENTITY checkmark \"&#10003;\">       <!-- 0x08 U+2713 -->\n"
	"<!ENTITY watch \"&#8986;\">            <!-- 0x09 U+231A -->\n"
	"<!ENTITY bell \"&#9086;\">             <!-- 0x0a U+237E -->\n"
	"<!ENTITY br \"<br/>\">                 <!-- 0x0a        -->\n"
	"<!ENTITY eightnote \"&#9834;\">        <!-- 0x0b U+266a -->\n"
	"<!ENTITY ff \"&#9228;\">               <!-- 0x0c U+240c -->\n"
	"<!ENTITY cr \"&#9229;\">               <!-- 0x0d U+240d -->\n"
	"<!ENTITY mountain \"&#9968;\">         <!-- 0x0e U+26f0 -->\n"
	"<!ENTITY umbrella \"&#9969;\">         <!-- 0x0f U+26f1 -->\n"
	"<!ENTITY circledzero \"&#9450;\">      <!-- 0x10 U+24ea -->\n"
	"<!ENTITY circledone \"&#9312;\">       <!-- 0x11 U+2460 -->\n"
	"<!ENTITY circledtwo \"&#9313;\">       <!-- 0x12 U+2461 -->\n"
	"<!ENTITY circledthree \"&#9314;\">     <!-- 0x13 U+2462 -->\n"
	"<!ENTITY circledfour \"&#9315;\">      <!-- 0x14 U+2463 -->\n"
	"<!ENTITY circledfive \"&#9316;\">      <!-- 0x15 U+2464 -->\n"
	"<!ENTITY circledsix \"&#9317;\">       <!-- 0x16 U+2465 -->\n"
	"<!ENTITY circledseven \"&#9318;\">     <!-- 0x17 U+2466 -->\n"
	"<!ENTITY circledeight \"&#9319;\">     <!-- 0x18 U+2467 -->\n"
	"<!ENTITY circlednine \"&#9320;\">      <!-- 0x19 U+2468 -->\n"
	"<!ENTITY capitalschwa \"&#399;\">      <!-- 0x1a U+018f -->\n"
	"<!ENTITY esc \"&#9243;\">              <!-- 0x1b U+241b -->\n"
	"<!ENTITY fountain \"&#9970;\">         <!-- 0x1c U+26f2 -->\n"
	"<!ENTITY flaginhole \"&#9971;\">       <!-- 0x1d U+26f3 -->\n"
	"<!ENTITY ferry \"&#9972;\">            <!-- 0x1e U+26f4 -->\n"
	"<!ENTITY sailboat \"&#9973;\">         <!-- 0x1f U+26f5 -->\n"
	"<!ENTITY del \"&#9249;\">              <!-- 0x7f U+2421 -->\n"
	"<!ENTITY increment \"&#8710;\">        <!-- 0x7f U+2206 -->\n"
	"]>\n"
	"<!-- created by %s %s -->\n"
	"\n"
;

#define default_header2 "\
\n\
<rscfile name=\"%s.rsc\" generator=\"%s\" version=\"%s\">\n\
"

static _BOOL do_comment = TRUE;

static unsigned short const atari_to_utf16[256] = {
/* 00 */	0x2400, 0x21e7, 0x21e9, 0x21e8, 0x21e6, 0x2610, 0x2611, 0x2612,
/* 08 */	0x2713, 0x231a, 0x237e, 0x266a, 0x240c, 0x240d, 0x26f0, 0x26f1,
/* 10 */	0x24ea, 0x2460, 0x2461, 0x2462, 0x2463, 0x2464, 0x2465, 0x2466,
/* 18 */	0x2467, 0x2468, 0x018f, 0x241b, 0x26f2, 0x26f3, 0x26f4, 0x26f5,
/* 20 */	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
/* 28 */	0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
/* 30 */	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
/* 38 */	0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
/* 40 */	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
/* 48 */	0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
/* 50 */	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
/* 58 */	0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
/* 60 */	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
/* 68 */	0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
/* 70 */	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
/* 78 */	0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x2206,
/* 80 */	0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7,
/* 88 */	0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
/* 90 */	0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9,
/* 98 */	0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x00df, 0x0192,
/* a0 */	0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba,
/* a8 */	0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
/* b0 */	0x00e3, 0x00f5, 0x00d8, 0x00f8, 0x0153, 0x0152, 0x00c0, 0x00c3,
/* b8 */	0x00d5, 0x00a8, 0x00b4, 0x2020, 0x00b6, 0x00a9, 0x00ae, 0x2122,
/* c0 */	0x0133, 0x0132, 0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5,
/* c8 */	0x05d6, 0x05d7, 0x05d8, 0x05d9, 0x05db, 0x05dc, 0x05de, 0x05e0,
/* d0 */	0x05e1, 0x05e2, 0x05e4, 0x05e6, 0x05e7, 0x05e8, 0x05e9, 0x05ea,
/* d8 */	0x05df, 0x05da, 0x05dd, 0x05e3, 0x05e5, 0x00a7, 0x2227, 0x221e,
/* e0 */	0x03b1, 0x03b2, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4,
/* e8 */	0x03a6, 0x0398, 0x03a9, 0x03b4, 0x222e, 0x03c6, 0x2208, 0x2229,
/* f0 */	0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248,
/* f8 */	0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x00b3, 0x00af
};

/******************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/******************************************************************************/

static void mobj2index(OBJECT *tree, _WORD parent, XRS_HEADER *xrsc_header, RSCFILE *file)
{
	_WORD mob;
	
	mob = parent == NIL ? ROOT : tree[parent].ob_head;
	do
	{
		switch (tree[mob].ob_type & OBTYPEMASK)
		{
		case G_STRING:
		case G_TITLE:
		case G_BUTTON:
		case G_SHORTCUT:
			file->rsc_nstrings += 1;
			break;
		case G_BOX:
		case G_IBOX:
		case G_BOXCHAR:
		case G_EXTBOX:
		default:
			break;
		case G_TEXT:
		case G_FTEXT:
		case G_BOXTEXT:
		case G_FBOXTEXT:
			xrsc_header->rsh_nted++;
			file->rsc_nstrings += 3;
			break;
		case G_IMAGE:
			xrsc_header->rsh_nbb++;
			file->rsc_nimages++;
			break;
		case G_ICON:
			xrsc_header->rsh_nib++;
			file->rsc_nimages += 2;
			file->rsc_nstrings += 1;
			break;
		case G_CICON:
			file->rsc_nciconblks++;
			{
				CICON *cic = tree[mob].ob_spec.ciconblk->mainlist;
				while (cic != NULL)
				{
					file->rsc_ncicons++;
					cic = cic->next_res;
				}
			}
			file->rsc_nstrings += 1;
			break;
		case G_USERDEF:
			file->rsc_nuserblks++;
			break;
		}
		xrsc_header->rsh_nobs++;
		if (tree[mob].ob_head != NIL)
			mobj2index(tree, tree[mob].ob_head, xrsc_header, file);
		mob = tree[mob].ob_next;
	} while (mob != parent);
}

/*** ---------------------------------------------------------------------- ***/

static void index_init(XRS_HEADER *xrsc_header, RSCFILE *file, char *buf)
{
	UNUSED(buf);
	xrsc_header->rsh_nobs = 0;
	xrsc_header->rsh_ntree = 0;
	xrsc_header->rsh_nted = 0;
	xrsc_header->rsh_nib = 0;
	xrsc_header->rsh_nbb = 0;
	xrsc_header->rsh_nstring = 0;
	xrsc_header->rsh_nimages = 0;
	file->rsc_nstrings = 0;
	file->rsc_nimages = 0;
	file->rsc_nuserblks = 0;
	file->rsc_nciconblks = 0;
	file->rsc_ncicons = 0;
}

/*** ---------------------------------------------------------------------- ***/

static void index_trees(RSCFILE *file, XRS_HEADER *xrsc_header, char *buf)
{
	RSCTREE *tree;
	OBJECT *ob;

	index_init(xrsc_header, file, buf);
	FOR_ALL_RSC(file, tree)
	{
		switch (tree->rt_type)
		{
		case RT_DIALOG:
		case RT_FREE:
		case RT_UNKNOWN:
		case RT_MENU:
			if (tree->rt_type == RT_MENU)
			{
				ob = tree->rt_objects.menu.mn_tree;
			} else
			{
				ob = tree->rt_objects.dial.di_tree;
			}
			if (ob != NULL)
			{
				tree->rt_index = xrsc_header->rsh_ntree;
				mobj2index(ob, NIL, xrsc_header, file);
				xrsc_header->rsh_ntree++;
			}
			break;
		case RT_FRSTR:
		case RT_ALERT:
			tree->rt_index = xrsc_header->rsh_nstring;
			xrsc_header->rsh_nstring += 1;
			file->rsc_nstrings += 1;
			break;
		case RT_FRIMG:
		case RT_MOUSE:
			tree->rt_index = xrsc_header->rsh_nimages;
			xrsc_header->rsh_nimages += 1;
			file->rsc_nimages += 1;
			xrsc_header->rsh_nbb++;
			break;
		}
	}
	if (file->rsc_opts.crc_string)
	{
		xrsc_header->rsh_nstring += 1;
		file->rsc_nstrings += 1;
	}
}

/******************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/******************************************************************************/

static void _mobj_offset(OBJECT *tree, _WORD ob, _WORD *xp, _WORD *yp)
{
	_WORD x, y;

	x = y = 0;
	while (ob != NIL)
	{
		x += tree[ob].ob_x;
		y += tree[ob].ob_y;
		ob = Objc_Get_Parent(tree, ob);
	}
	*xp = x;
	*yp = y;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL xml_indent(int level)
{
	int i;
	
	for (i = 0; i < level; i++)
		outstr("  ");
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

#define putprop(name, fmt, val) output3("<%s>" fmt "</%s>\n", name, val, name)

#define putflag(name, val) putprop(name, "%d", (val) != 0)

/*** ---------------------------------------------------------------------- ***/

static _BOOL xml_quotestring(const char *name, const char *str, _WORD mx)
{
	_WORD i, n;
	unsigned char c;
	unsigned short wc;
	
	if (name)
	{
		output("<%s>", name);
	}
	if (mx == -1)
	{
		n = (str ? (_WORD)strlen(str) : 0) + 1;
		mx = n;
	} else
	{
		n = mx;
	}
	mx--;
	for (i = 0; i < mx; i++)
	{
		if (i >= n)
		{
			outstr("&nul;");
		} else
		{
			c = *str++;
			switch (c)
			{
			case '\0':
				outstr("&nul;");
				break;
			case 0x01:
				outstr("&uparrow;");
				break;
			case 0x02:
				outstr("&downarrow;");
				break;
			case 0x03:
				outstr("&rightarrow;");
				break;
			case 0x04:
				outstr("&leftarrow;");
				break;
			case 0x05:
				outstr("&ballotbox;");
				break;
			case 0x06:
				outstr("&ballotboxcheck;");
				break;
			case 0x07:
				outstr("&ballotboxx;");
				break;
			case 0x08:
				outstr("&checkmark;");
				break;
			case 0x09:
				outstr("&watch;");
				break;
			case 0x0a:
				outstr("&br;");
				break;
			case 0x0b:
				outstr("&eightnote;");
				break;
			case 0x0c:
				outstr("&ff;");
				break;
			case 0x0d:
				outstr("&cr;");
				break;
			case 0x0e:
				outstr("&mountain;");
				break;
			case 0x0f:
				outstr("&umbrella;");
				break;
			case 0x10:
				outstr("&circledzero;");
				break;
			case 0x11:
				outstr("&circledone;");
				break;
			case 0x12:
				outstr("&circledtwo;");
				break;
			case 0x13:
				outstr("&circledthree;");
				break;
			case 0x14:
				outstr("&circledfour;");
				break;
			case 0x15:
				outstr("&circledfive;");
				break;
			case 0x16:
				outstr("&circledsix;");
				break;
			case 0x17:
				outstr("&circledseven;");
				break;
			case 0x18:
				outstr("&circledeight;");
				break;
			case 0x19:
				outstr("&circlednine;");
				break;
			case 0x1a:
				outstr("&capitalschwa;");
				break;
			case 0x1b:
				outstr("&esc;");
				break;
			case 0x1c:
				outstr("&fountain;");
				break;
			case 0x1d:
				outstr("&flaginhole;");
				break;
			case 0x1e:
				outstr("&ferry;");
				break;
			case 0x1f:
				outstr("&sailboat;");
				break;
			case 0x7f:
				outstr("&increment;");
				break;
			case '<':
				outstr("&lt;");
				break;
			case '>':
				outstr("&gt;");
				break;
			case '"':
				outstr("&quot;");
				break;
			case '\'':
				outstr("&apos;");
				break;
			case '&':
				outstr("&amp;");
				break;
			default:
				wc = atari_to_utf16[c];
				if (wc >= 0x20 && wc < 0x80)
				{
					outc(c);
				} else
				{
					output("&#%u;", wc);
				}
				break;
			}
		}
	}
	if (name)
	{
		output("</%s>\n", name);
	}
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL xml_comment(cstringarray cmnt, int level)
{
	cstringarray p;

	if ((p = cmnt) == NULL)
		return TRUE;
	while (*p != '\0')
	{
		xml_indent(level);
		xml_quotestring("comment", p, -1);
		while (*p++ != '\0')
			;
	}
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL xml_bgh(cstringarray bgh, _WORD idx, int level)
{
	cstringarray p;

	if ((p = bgh) == NULL)
		return TRUE;
	while (*p != '\0')
	{
		xml_indent(level);
		output("<bubblegem index=\"%d\">", idx);
		xml_quotestring(NULL, p, -1);
		outstr("</bubblegem>\n");
		while (*p++ != '\0')
			;
	}
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL out_data(const unsigned char *data, _ULONG size)
{
	_ULONG i;
	_UWORD x;
	
#define bytes_per_line 32
	outstr("<![CDATA[\n");
	x = 0;
	for (i = 0; i < size; i++)
	{
		output("%02x", *data);
		data++;
		if (++x == bytes_per_line)
		{
			outc('\n');
			x = 0;
		}
	}
	if (x != 0)
	{
		outc('\n');
	}
	outstr("]]>\n");
#undef bytes_per_line
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL out_bitblk(BITBLK *bit, int level)
{
	xml_indent(level); putprop("x", "%d", bit->bi_x);
	xml_indent(level); putprop("y", "%d", bit->bi_y);
	xml_indent(level); putprop("width", "%d", bit->bi_wb * 8);
	xml_indent(level); putprop("height", "%d", bit->bi_hl);
	xml_indent(level); putprop("color", "%u", bit->bi_color);
	xml_indent(level); outstr("<data>\n");
	if (out_data((const unsigned char *)bit->bi_pdata, bitblk_datasize(bit)) == FALSE)
		return FALSE;
	xml_indent(level); outstr("</data>\n");
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL out_iconblk(ICONBLK *icon, int level)
{
	xml_indent(level); putprop("character", "%d", ICOLSPEC_GET_CHARACTER(icon->ib_char));
	xml_indent(level); putprop("datacolor", "%d", ICOLSPEC_GET_DATACOL(icon->ib_char));
	xml_indent(level); putprop("maskcolor", "%d", ICOLSPEC_GET_MASKCOL(icon->ib_char));
	xml_indent(level); putprop("x", "%d", icon->ib_xicon);
	xml_indent(level); putprop("y", "%d", icon->ib_yicon);
	xml_indent(level); putprop("width", "%d", icon->ib_wicon);
	xml_indent(level); putprop("height", "%d", icon->ib_hicon);
	xml_indent(level); putprop("xchar", "%d", icon->ib_xchar);
	xml_indent(level); putprop("ychar", "%d", icon->ib_ychar);
	xml_indent(level); putprop("xtext", "%d", icon->ib_xtext);
	xml_indent(level); putprop("ytext", "%d", icon->ib_ytext);
	xml_indent(level); putprop("wtext", "%d", icon->ib_wtext);
	xml_indent(level); putprop("htext", "%d", icon->ib_htext);
	xml_indent(level); xml_quotestring("text", icon->ib_ptext, -1);
	xml_indent(level); outstr("<data>\n");
	if (out_data((const unsigned char *)icon->ib_pdata, iconblk_masksize(icon)) == FALSE)
		return FALSE;
	xml_indent(level); outstr("</data>\n");
	xml_indent(level); outstr("<mask>\n");
	if (out_data((const unsigned char *)icon->ib_pmask, iconblk_masksize(icon)) == FALSE)
		return FALSE;
	xml_indent(level); outstr("</mask>\n");
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL xml_ob_object(RSCFILE *file, XRS_HEADER *xrsc_header, RSCTREE *rsctree, _WORD parent, _BOOL verbose, int level)
{
	_WORD ob;
	_WORD x, y, w, h;
	_WORD cw, ch;
	_UWORD flags, state, type;
	OBJECT *tree = rsctree->rt_objects.dial.di_tree;;
	
	ob = parent == NIL ? ROOT : tree[parent].ob_head;
	GetTextSize(&cw, &ch);
	do
	{
		flags = tree[ob].ob_flags;
		state = tree[ob].ob_state;
		type = tree[ob].ob_type & OBTYPEMASK;
		_mobj_offset(tree, ob, &x, &y);
		x /= cw;
		y /= ch;
		w = tree[ob].ob_width / cw;
		h = tree[ob].ob_height / ch;
		xml_indent(level);
		output3("<object type=\"%s\" index=\"%d\" name=\"%s\">\n", type_name(type), ob, fixnull(ob_name(file, rsctree, ob)));
		if (do_comment)
			xml_comment(ob_cmnt(file, rsctree, ob), level + 1);
		xml_bgh(NULL, ob, level + 1);
		xml_indent(level + 1); putprop("x", "%d", tree[ob].ob_x);
		xml_indent(level + 1); putprop("y", "%d", tree[ob].ob_y);
		xml_indent(level + 1); putprop("xabs", "%d", x);
		xml_indent(level + 1); putprop("yabs", "%d", y);
		xml_indent(level + 1); putprop("width", "%d", w);
		xml_indent(level + 1); putprop("height", "%d", h);
		xml_indent(level + 1); putprop("flags", "%u", flags);
		xml_indent(level + 1); putprop("state", "%u", state);
		xml_indent(level + 1); putprop("type", "%u", type);
		xml_indent(level + 1); putprop("exttype", "%u", tree[ob].ob_type >> 8);
		switch (type)
		{
		case G_STRING:
		case G_TITLE:
		case G_BUTTON:
		case G_SHORTCUT:
			xml_indent(level + 1); outstr("<string>\n");
			xml_indent(level + 2); xml_quotestring("text", tree[ob].ob_spec.free_string, -1);
			xml_indent(level + 1); outstr("</string>\n");
			break;
		case G_BOX:
		case G_IBOX:
		case G_BOXCHAR:
		case G_EXTBOX:
			xml_indent(level + 1); outstr("<box>\n");
			xml_indent(level + 2); putprop("character", "%u", OBSPEC_GET_CHARACTER(tree[ob].ob_spec));
			xml_indent(level + 2); putprop("framesize", "%d", OBSPEC_GET_FRAMESIZE(tree[ob].ob_spec));
			xml_indent(level + 2); putprop("framecolor", "%d", OBSPEC_GET_FRAMECOL(tree[ob].ob_spec));
			xml_indent(level + 2); putprop("textcolor", "%d", OBSPEC_GET_TEXTCOL(tree[ob].ob_spec));
			xml_indent(level + 2); putprop("opaque", "%d", OBSPEC_GET_TEXTMODE(tree[ob].ob_spec));
			xml_indent(level + 2); putprop("fillpattern", "%d", OBSPEC_GET_FILLPATTERN(tree[ob].ob_spec));
			xml_indent(level + 2); putprop("fillcolor", "%d", OBSPEC_GET_INTERIORCOL(tree[ob].ob_spec));
			xml_indent(level + 1); outstr("</box>\n");
			break;
		case G_TEXT:
		case G_FTEXT:
		case G_BOXTEXT:
		case G_FBOXTEXT:
			{
				TEDINFO *ted = tree[ob].ob_spec.tedinfo;
				
				xml_indent(level + 1); outstr("<tedinfo>\n");
				xml_indent(level + 2); xml_quotestring("text", ted->te_ptext, ted->te_txtlen);
				xml_indent(level + 2); xml_quotestring("template", ted->te_ptmplt, ted->te_tmplen);
				xml_indent(level + 2); xml_quotestring("valid", ted->te_pvalid, -1);
				xml_indent(level + 2); putprop("font", "%d", ted->te_font);
				xml_indent(level + 2); putprop("junk1", "%d", ted->te_junk1);
				xml_indent(level + 2); putprop("just", "%d", ted->te_just);
				xml_indent(level + 2); outstr("<color>\n");
				xml_indent(level + 3); putprop("framecolor", "%d", COLSPEC_GET_FRAMECOL(ted->te_color));
				xml_indent(level + 3); putprop("textcolor", "%d", COLSPEC_GET_TEXTCOL(ted->te_color));
				xml_indent(level + 3); putprop("opaque", "%d", COLSPEC_GET_TEXTMODE(ted->te_color));
				xml_indent(level + 3); putprop("fillpattern", "%d", COLSPEC_GET_FILLPATTERN(ted->te_color));
				xml_indent(level + 3); putprop("fillcolor", "%d", COLSPEC_GET_INTERIORCOL(ted->te_color));
				xml_indent(level + 2); outstr("</color>\n");
				xml_indent(level + 2); putprop("junk2", "%d", ted->te_junk2);
				xml_indent(level + 2); putprop("thickness", "%d", ted->te_thickness);
				xml_indent(level + 2); putprop("txtlen", "%d", ted->te_txtlen);
				xml_indent(level + 2); putprop("tmplen", "%d", ted->te_tmplen);
				xml_indent(level + 1); outstr("</tedinfo>\n");
			}
			break;
		case G_IMAGE:
			xml_indent(level + 1); outstr("<bitblk>\n");
			out_bitblk(tree[ob].ob_spec.bitblk, level + 2);
			xml_indent(level + 1); outstr("</bitblk>\n");
			break;
		case G_ICON:
			xml_indent(level + 1); outstr("<iconblk>\n");
			out_iconblk(tree[ob].ob_spec.iconblk, level + 2);
			xml_indent(level + 1); outstr("</iconblk>\n");
			break;
		case G_CICON:
			{
				CICON *cic;
				_ULONG size;
				CICONBLK *ciconblk = tree[ob].ob_spec.ciconblk;
				
				xml_indent(level + 1); outstr("<ciconblk>\n");
				out_iconblk(&ciconblk->monoblk, level + 2);
				size = iconblk_masksize(&ciconblk->monoblk);
				cic = ciconblk->mainlist;
				while (cic != NULL)
				{
					xml_indent(level + 2); output("<cicon planes=\"%d\">\n", cic->num_planes);
					if (cic->col_data)
					{
						xml_indent(level + 3); outstr("<colordata>\n");
						if (out_data((const unsigned char *)cic->col_data, size * cic->num_planes) == FALSE)
							return FALSE;
						xml_indent(level + 3); outstr("</colordata>\n");
					}
					if (cic->col_mask)
					{
						xml_indent(level + 3); outstr("<colormask>\n");
						if (out_data((const unsigned char *)cic->col_mask, size) == FALSE)
							return FALSE;
						xml_indent(level + 3); outstr("</colormask>\n");
					}
					if (cic->sel_data)
					{
						xml_indent(level + 3); outstr("<seldata>\n");
						if (out_data((const unsigned char *)cic->sel_data, size * cic->num_planes) == FALSE)
							return FALSE;
						xml_indent(level + 3); outstr("</seldata>\n");
					}
					if (cic->sel_mask)
					{
						xml_indent(level + 3); outstr("<selmask>\n");
						if (out_data((const unsigned char *)cic->sel_mask, size) == FALSE)
							return FALSE;
						xml_indent(level + 3); outstr("</selmask>\n");
					}
					xml_indent(level + 2); outstr("</cicon>\n");
					cic = cic->next_res;
				}
				xml_indent(level + 1); outstr("</ciconblk>\n");
			}
			break;
		case G_USERDEF:
			xml_indent(level + 1); outstr("<userdef>\n");
			xml_indent(level + 2); putprop("code", "%ld", (long)tree[ob].ob_spec.userblk->ub_code);
			xml_indent(level + 2); putprop("parm", "%ld", (long)tree[ob].ob_spec.userblk->ub_parm);
			xml_indent(level + 1); outstr("</userdef>\n");
			break;
		default:
			xml_indent(level + 1); outstr("<index>\n");
			xml_indent(level + 2); putprop("index", "%ld", (long)tree[ob].ob_spec.index);
			xml_indent(level + 1); outstr("</index>\n");
			break;
		}
		if (tree[ob].ob_head != NIL)
		{
			xml_indent(level + 1);
			outstr("<child>\n");
			if (xml_ob_object(file, xrsc_header, rsctree, ob, verbose, level + 2) == FALSE)
				return FALSE;
			xml_indent(level + 1);
			outstr("</child>\n");
		}
		xml_indent(level);
		outstr("</object>\n");
		ob = tree[ob].ob_next;
	} while (ob != parent);
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL xml_bghinfo(BGHINFO *bgh, int level)
{
	BGHENTRY *entry;
	
	if (bgh == NULL)
		return TRUE;
	FOR_ALL_BGH(bgh, entry)
	{
		xml_bgh(entry->cmnt, entry->idx, level);
	}
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL xml_trees(RSCFILE *file, XRS_HEADER *xrsc_header, rsc_counter *counter, _BOOL verbose)
{
	RSCTREE *tree;
	OBJECT *ob;
	char *str;
	
	FOR_ALL_RSC(file, tree)
	{
		switch (tree->rt_type)
		{
		case RT_DIALOG:
		case RT_FREE:
		case RT_UNKNOWN:
		case RT_MENU:
			if (tree->rt_type == RT_MENU)
				ob = tree->rt_objects.menu.mn_tree;
			else
				ob = tree->rt_objects.dial.di_tree;
			if (ob != NULL)
			{
				xml_indent(1); output3("<tree index=\"%ld\" name=\"%s\" type=\"%s\">\n", tree->rt_index, tree->rt_name, rtype_name_short(tree->rt_type));
				if (do_comment)
				{
					xml_comment(tree->rt_cmnt, 2);
				}
				if (xml_ob_object(file, xrsc_header, tree, NIL, verbose, 2) == FALSE)
					return FALSE;
				xml_indent(1); outstr("</tree>\n");
			}
			break;
		case RT_FRSTR:
		case RT_ALERT:
			str = tree->rt_type == RT_ALERT ? tree->rt_objects.alert.al_str : tree->rt_objects.str.fr_str;
			xml_indent(1); output3("<freestring index=\"%ld\" name=\"%s\" type=\"%s\">\n", tree->rt_index, tree->rt_name, rtype_name_short(tree->rt_type));
			if (do_comment)
			{
				xml_comment(tree->rt_cmnt, 2);
			}
			xml_indent(2); xml_quotestring("text", str, -1);
			xml_bghinfo(tree->rt_objects.alert.al_bgh, 2);
			xml_indent(1); outstr("</freestring>\n");
			break;
		case RT_FRIMG:
		case RT_MOUSE:
			xml_indent(1); output3("<freeimage index=\"%ld\" name=\"%s\" type=\"%s\">\n", tree->rt_index, tree->rt_name, rtype_name_short(tree->rt_type));
			if (do_comment)
			{
				xml_comment(tree->rt_cmnt, 2);
			}
			out_bitblk(tree->rt_objects.bit, 2);
			xml_indent(1); outstr("</freeimage>\n");
			break;
		case RT_BUBBLEMORE:
		case RT_BUBBLEUSER:
			xml_indent(1); output3("<bubble index=\"%ld\" name=\"%s\" type=\"%s\">\n", tree->rt_index, tree->rt_name, rtype_name_short(tree->rt_type));
			if (do_comment)
			{
				xml_comment(tree->rt_cmnt, 2);
			}
			xml_bghinfo(tree->rt_objects.bgh, 2);
			xml_indent(1); outstr("</bubble>\n");
			break;
		}
	}

	{
		_ULONG idx = xrsc_header->rsh_nstring;
		
		if (file->rsc_opts.crc_string)
		{
			--idx;
			str = counter->crc_string_buf;
			xml_indent(1); output3("<freestring index=\"%ld\" name=\"%s\" type=\"%s\">\n", idx, RSM_CRC_STRID, rtype_name_short(RT_FRSTR));
			xml_indent(2); xml_quotestring("text", str, -1);
			xml_indent(1); outstr("</freestring>\n");
		}
	}
	
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL output_xml_data(RSCFILE *file, XRS_HEADER *xrsc_header, rsc_counter *counter, char *buf)
{
	_BOOL verbose;
	_ULONG xrs_diff_size = xrsc_header->rsh_rssize <= (RS_THRESHOLD + XRS_DIFF_SIZE) ? XRS_DIFF_SIZE : 0;
	char strbuf[40];
	struct tm *tm;
	
	UNUSED(buf);
	verbose = file->rsc_flags & RF_VERBOSE ? TRUE : FALSE;
	
	xml_indent(1); outstr("<header>\n");
	xml_indent(2); putprop("endian", "%s", (HOST_BYTE_ORDER == BYTE_ORDER_BIG_ENDIAN) == file->rsc_swap_flag ? "little" : "big");
	xml_indent(2); putprop("version", "%u", xrsc_header->rsh_vrsn);
	xml_indent(2); putprop("extversion", "%u", xrsc_header->rsh_extvrsn);
	xml_indent(2); putprop("offset_objects", "%lu", xrsc_header->rsh_object - xrs_diff_size);
	xml_indent(2); putprop("offset_tedinfos", "%lu", xrsc_header->rsh_tedinfo - xrs_diff_size);
	xml_indent(2); putprop("offset_iconblks", "%lu", xrsc_header->rsh_iconblk - xrs_diff_size);
	xml_indent(2); putprop("offset_bitblks", "%lu", xrsc_header->rsh_bitblk - xrs_diff_size);
	xml_indent(2); putprop("offset_freestrings", "%lu", xrsc_header->rsh_frstr - xrs_diff_size);
	xml_indent(2); putprop("offset_strings", "%lu", xrsc_header->rsh_string - xrs_diff_size);
	xml_indent(2); putprop("offset_imagedata", "%lu", xrsc_header->rsh_imdata - xrs_diff_size);
	xml_indent(2); putprop("offset_freeimages", "%lu", xrsc_header->rsh_frimg - xrs_diff_size);
	xml_indent(2); putprop("offset_trees", "%lu", xrsc_header->rsh_trindex - xrs_diff_size);
	xml_indent(2); putprop("count_objects", "%lu", xrsc_header->rsh_nobs);
	xml_indent(2); putprop("count_trees", "%lu", xrsc_header->rsh_ntree);
	xml_indent(2); putprop("count_tedinfos", "%lu", xrsc_header->rsh_nted);
	xml_indent(2); putprop("count_iconblks", "%lu", xrsc_header->rsh_nib);
	xml_indent(2); putprop("count_bitblks", "%lu", xrsc_header->rsh_nbb);
	xml_indent(2); putprop("count_freestrings", "%lu", xrsc_header->rsh_nstring);
	xml_indent(2); putprop("count_freeimages", "%lu", xrsc_header->rsh_nimages);
	xml_indent(2); putprop("resource_size", "%lu", xrsc_header->rsh_rssize - xrs_diff_size);
	xml_indent(2); putprop("file_size", "%lu", xrsc_header->rsh_rssize - xrs_diff_size + counter->ctotal_size + counter->ext_size);
	xml_indent(2); putprop("count_strings", "%lu", file->rsc_nstrings);
	xml_indent(2); putprop("count_userblks", "%lu", file->rsc_nuserblks);
	xml_indent(2); putprop("count_ciconblks", "%lu", file->rsc_nciconblks);
	xml_indent(2); putprop("count_cicons", "%lu", file->rsc_ncicons);
	xml_indent(1); outstr("</header>\n");

	xml_indent(1); outstr("<options>\n");
	xml_indent(2); putprop("extended_objects", "%d", (int)file->rsc_extob.mode);
	xml_indent(2); putflag("out_cheader", file->rsc_flags & RF_C);
	xml_indent(2); putflag("out_pascal", file->rsc_flags & RF_PASCAL);
	xml_indent(2); putflag("out_modula", file->rsc_flags & RF_MODULA);
	xml_indent(2); putflag("out_fortran", file->rsc_flags & RF_FORTRAN);
	xml_indent(2); putflag("out_gfa", file->rsc_flags & RF_GFA);
	xml_indent(2); putflag("out_csource1", file->rsc_flags & RF_CSOURCE1);
	xml_indent(2); putflag("out_csource2", file->rsc_flags & RF_CSOURCE2);
	xml_indent(2); putflag("out_ass", file->rsc_flags & RF_ASS);
	xml_indent(2); putflag("out_asource", file->rsc_flags & RF_ASOURCE);
	xml_indent(2); putflag("out_basic", file->rsc_flags & RF_BASIC);
	xml_indent(2); putflag("out_forth", file->rsc_flags & RF_FORTH);
	xml_indent(2); putflag("out_mm2", file->rsc_flags & RF_MODULA_MM2);
	xml_indent(2); putflag("out_tdi", file->rsc_flags & RF_MODULA_TDI);
	xml_indent(2); putflag("out_nrsc", file->rsc_flags & RF_NRSC);
	xml_indent(2); putflag("verbose_out", file->rsc_flags & RF_VERBOSE);
	xml_indent(2); putflag("out_def", file->rsc_flags & RF_DEF);
	xml_indent(2); putflag("out_dfn", file->rsc_flags & RF_DFN);
	xml_indent(2); putflag("out_hrd", file->rsc_flags & RF_HRD);
	xml_indent(2); putflag("out_rso", file->rsc_flags & RF_RSO);
	xml_indent(2); putflag("out_lpr", file->rsc_flags & RF_MODULA_LPR);
	xml_indent(2); putflag("out_cdata", file->rsc_flags & RF_CDATA);
	xml_indent(2); putflag("out_psource", file->rsc_flags & RF_PSOURCE);
	xml_indent(2); putflag("out_marray", file->rsc_flags & RF_MODULA_ARRAY);
	xml_indent(2); putflag("out_rcsource", file->rsc_flags & RF_RCSOURCE);
	/* xml_indent(2); putflag("out_rsx", file->rsc_flags & RF_RSX); */
	xml_indent(2); putflag("out_rsm", file->rsc_flags & RF_RSM);
	xml_indent(2); putflag("out_sdl", file->rsc_flags2 & RF_SDLSOURCE);
	xml_indent(2); putflag("out_external", file->rsc_flags2 & RF_EXTERNAL);
	xml_indent(2); putflag("out_xml", file->rsc_flags2 & RF_XML);
	xml_indent(2); putflag("out_altorder", file->rsc_flags2 & RF_ALTORDER);
	xml_indent(2); putflag("out_nopvalopt", file->rsc_flags2 & RF_NOPVALOPT);
	xml_indent(2); putflag("out_static", file->rsc_flags2 & RF_STATIC);
	xml_indent(2); putflag("out_romable", file->rsc_flags2 & RF_ROMABLE);
	xml_indent(2); putflag("out_imagewords", file->rsc_flags2 & RF_IMAGEWORDS);
	xml_indent(2); putprop("namelen", "%d", file->rsc_namelen);
	xml_indent(2); outstr("<namerule type=\"first\">\n");
	xml_indent(3); putprop("upper", "%d", file->rsc_rule1.upper);
	xml_indent(3); putprop("lower", "%d", file->rsc_rule1.lower);
	xml_indent(3); putprop("alpha", "%d", file->rsc_rule1.alpha);
	xml_indent(3); putprop("alnum", "%d", file->rsc_rule1.alnum);
	xml_indent(3); xml_quotestring("add", file->rsc_rule1.add, -1);
	xml_indent(3); xml_quotestring("sub", file->rsc_rule1.sub, -1);
	xml_indent(2); outstr("</namerule>\n");
	xml_indent(2); outstr("<namerule type=\"other\">\n");
	xml_indent(3); putprop("upper", "%d", file->rsc_rule2.upper);
	xml_indent(3); putprop("lower", "%d", file->rsc_rule2.lower);
	xml_indent(3); putprop("alpha", "%d", file->rsc_rule2.alpha);
	xml_indent(3); putprop("alnum", "%d", file->rsc_rule2.alnum);
	xml_indent(3); xml_quotestring("add", file->rsc_rule2.add, -1);
	xml_indent(3); xml_quotestring("sub", file->rsc_rule2.sub, -1);
	xml_indent(2); outstr("</namerule>\n");
	xml_indent(2); putprop("fillchar", "%d", file->rsc_opts.ted_fillchar);
	xml_indent(2); putprop("menu_leftmargin", "%d", file->rsc_opts.menu_leftmargin);
	xml_indent(2); putprop("menu_rightmargin", "%d", file->rsc_opts.menu_rightmargin);
	xml_indent(2); putprop("menu_minspace", "%d", file->rsc_opts.menu_minspace);
	xml_indent(2); putprop("menu_fillspace", "%d", file->rsc_opts.menu_fillspace);
	tm = gmtime(&file->rsc_date_created);
	sprintf(strbuf, "%04d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	xml_indent(2); putprop("date_created", "%s", strbuf);
	tm = gmtime(&file->rsc_date_changed);
	sprintf(strbuf, "%04d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	xml_indent(2); putprop("date_changed", "%s", strbuf);
	xml_indent(2); putprop("edition", "%lu", file->rsc_edition);
	xml_indent(2); putprop("crc", "%u", file->rsc_rsm_crc);
	xml_indent(1); outstr("</options>\n");
	
#if 0
	/* NYI here */
	if (rsc_haspalette(file))
	{
		_LONG entries, i;
		_UWORD *data;
		
		xml_indent(1); outstr("<palette>\n");
		entries = file->rsc_extensions[RSC_EXT_PALETTE].ext_size / 8;
		data = (_UWORD *)file->rsc_extensions[RSC_EXT_PALETTE].ext_ptr;
		for (i = 0; i < entries; i++, data += 4)
		{
			xml_indent(2); outstr("<color>\n");
			xml_indent(3); putprop("red", "%d", data[0]);
			xml_indent(3); putprop("green", "%d", data[1]);
			xml_indent(3); putprop("blue", "%d", data[2]);
			xml_indent(3); putprop("index", "%d", data[3]);
			xml_indent(2); outstr("</color>\n");
		}
		xml_indent(1); outstr("</palette>\n");
	}
#endif

	if (xml_trees(file, xrsc_header, counter, verbose) == FALSE)
		return FALSE;
	
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static _BOOL rsc_xml_output_source_file(RSCFILE *file, XRS_HEADER *xrsc_header, rsc_counter *counter, char *buf, const char *h_ext, const char *default_file)
{
	_BOOL ret;
	
	UNUSED(h_ext);
	UNUSED(default_file);
	fprintf(ffp, default_header1, program_name, program_version);
	fprintf(ffp, default_header2, rsx_basename(file->rsc_rsxname), program_name, program_version);
	ret = output_xml_data(file, xrsc_header, counter, buf);
	fputs("</rscfile>\n\n", ffp);
	return ret;
}

/*** ---------------------------------------------------------------------- ***/

_BOOL rsc_xml_source(RSCFILE *file, rsc_counter *counter, char *filename, char *buf)
{
	XRS_HEADER xrsc_header;
	_BOOL ok;

	xrsc_header = file->header;
	count_trees(file, &xrsc_header, counter, TRUE);
	if (file_create(filename, "w") == FALSE)
	{
		return FALSE;
	}
	index_trees(file, &xrsc_header, buf);
	ok = rsc_xml_output_source_file(file, &file->header, counter, buf, NULL, "orcs_xml.tpl");
	return file_close(ok);
}
