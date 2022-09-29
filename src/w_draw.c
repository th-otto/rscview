#include "config.h"
#include <gem.h>
#include "portvdi.h"
#include "or_draw.h"
#include "w_draw.h"

/******************************************************************************/
/* W_Pixel()                                                                  */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Zeichnet einzelne Punkte.                                                  */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> pxy          = Kordinaten der Punkte (jeweils x,y)                      */
/* -> numpoints    = Anzahl der Koordinatenpaare in pxy                       */
/* -> color        = Farbnummer                                               */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Pixel(_WORD handle, const W_POINTS pxy[], _WORD numpoints, _WORD color)
{
	_WORD pxyar[MAX_POINTS * 2];
	_WORD i;
	
	if (numpoints > MAX_POINTS)
	{
		return;
	}
	for (i = 0; i < numpoints; i++)
	{
		pxyar[2*i] = pxy[i].xx;
		pxyar[2*i+1] = pxy[i].yy;
	}
	vsm_color(handle, color);
	vsm_height(handle, 1);
	vsm_type(handle, 1);
	v_pmarker(handle, numpoints, pxyar);
}

/******************************************************************************/
/* W_Clip_Rect()                                                              */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Beschraenkt die Ausgabe aller nachfolgenden Zeichenoperationen             */
/* auf den angegebenen Bereich.                                               */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> on           = Schalter fuer Ein- oder Ausschalten                      */
/* -> gr           = Rechteck fuer Koordinaten                                */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Clip_Rect(_WORD handle, _BOOL on, const GRECT *gr)
{
	_WORD clip[4];
	
	clip[0] = gr->g_x;
	clip[1] = gr->g_y;
	clip[2] = clip[0] + gr->g_w - 1;
	clip[3] = clip[1] + gr->g_h - 1;
	vs_clip(handle, on, clip);
}

/******************************************************************************/
/* W_Lines()                                                                  */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Zeichnet zusammenhaengende Linien. Der Linienstil muss vorher              */
/* mit W_PenCreate() eingestellt worden sein.                                 */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> points       = Koordinatenpaare der Eckpunkte                           */
/* -> numpoints    = Anzahl der Koordinatenpaare in points[]                  */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Lines(_WORD handle, const W_POINTS points[], _WORD npoints)
{
	_WORD ar[MAX_POINTS * 2], *toAr;
	_WORD ii;

	if (npoints > MAX_POINTS)
	{
		return;
	}
	toAr = ar;
	for (ii = 0; ii < npoints; ii++)
	{
		*toAr++ = points[ii].xx;
		*toAr++ = points[ii].yy;
	}
	v_pline(handle, npoints, ar);
}

/******************************************************************************/
/* W_Clear_Rect()                                                             */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Fuellt einen Rechteck WEISS.                                               */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> gr           = Ausdehnung des Rechtecks                                 */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

static void set_fill_style(_WORD handle, _WORD style, _WORD color)
{
	_WORD interior;
	
	if (style == IP_HOLLOW)
	{
		interior = FIS_SOLID;
		style = 0;
		color = W_PAL_WHITE;
	} else
	{
		if (style >= 25)
		{
			style -= 24;
			interior = FIS_HATCH;
		} else
		{
			switch (style)
			{
			case IP_SOLID:
				style = 8;
				break;
			case 8:
				style = 7;
				break;
			default:
				break;
			}
			interior = FIS_PATTERN;
		}
	}
	vsf_interior(handle, interior);
	vsf_style(handle, style);
	vsf_color(handle, color);
}

void W_Clear_Rect(_WORD handle, const GRECT *gr)
{
	_WORD pxyar[4];

	pxyar[0] = gr->g_x;
	pxyar[1] = gr->g_y;
	pxyar[2] = pxyar[0] + gr->g_w - 1;
	pxyar[3] = pxyar[1] + gr->g_h - 1;
	set_fill_style(handle, IP_HOLLOW, W_PAL_WHITE);
	vr_recfl(handle, pxyar);
}

/******************************************************************************/
/* W_Fill_Rect()                                                              */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Fuellt einen Rechteck in einer beliebigen Farbe.                           */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> gr           = Ausdehnung des Rechtecks                                 */
/* -> style        = Fuellmodus fuer die Flaeche                              */
/*   moegliche Werte:                                                         */
/*      IP_HOLLOW    Die Flaeche wird weiss ausgefuellt                       */
/*      IP_1PATT                                                              */
/*      IP_2PATT     .                                                        */
/*      IP_3PATT     .                                                        */
/*      IP_4PATT     .                                                        */
/*      IP_5PATT                                                              */
/*      IP_6PATT                                                              */
/*      IP_SOLID     Die Flaeche wird komplett ausgefuellt                    */
/* -> color        = Farbnummer                                               */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Fill_Rect(_WORD handle, const GRECT *gr, _WORD style, _WORD color, _BOOL perimeter)
{
	_WORD pxyar[4];

	pxyar[0] = gr->g_x;
	pxyar[1] = gr->g_y;
	pxyar[2] = pxyar[0] + gr->g_w - 1;
	pxyar[3] = pxyar[1] + gr->g_h - 1;
	vsf_perimeter(handle, perimeter);
	set_fill_style(handle, style, color);
	v_bar(handle, pxyar);
	vsf_perimeter(handle, FALSE);
}

/******************************************************************************/
/* W_Invert_Rect()                                                            */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Invertiert die Farben eines Rechtecks.                                     */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> gr           = Ausdehnung des Rechtecks                                 */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Invert_Rect(_WORD handle, const GRECT *gr)
{
	_WORD pxyar[4];
	
	pxyar[0] = gr->g_x;
	pxyar[1] = gr->g_y;
	pxyar[2] = pxyar[0] + gr->g_w - 1;
	pxyar[3] = pxyar[1] + gr->g_h - 1;

	vswr_mode(handle, MD_XOR);
	set_fill_style(handle, IP_SOLID, W_PAL_BLACK);
	vr_recfl(handle, pxyar);
	vswr_mode(handle, MD_REPLACE);
}

/******************************************************************************/
/* W_Disable_Rect()                                                           */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Stellt den Inhalt eines Rechtecks hell oder grau dar.                      */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> gr           = Ausdehnung des Rechtecks                                 */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Disable_Rect(_WORD handle, const GRECT *gr)
{
	_WORD pxyar[4];
	
	pxyar[0] = gr->g_x;
	pxyar[1] = gr->g_y;
	pxyar[2] = pxyar[0] + gr->g_w - 1;
	pxyar[3] = pxyar[1] + gr->g_h - 1;

	vswr_mode(handle, MD_TRANS);
	set_fill_style(handle, IP_4PATT, W_PAL_WHITE);
	v_bar(handle, pxyar);
	vswr_mode(handle, MD_REPLACE);
}

/******************************************************************************/
/* W_Rectangle()                                                              */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Zeichnet die Umrandung einen Rechtecks                                     */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> gr           = Ausdehnung des Rechtecks                                 */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Rectangle(_WORD handle, const GRECT *gr)
{
	_WORD pxy[10];

	/* top left */
	pxy[0] = gr->g_x;
	pxy[1] = gr->g_y;
	/* bottom left */
	pxy[2] = pxy[0];
	pxy[3] = pxy[1] + gr->g_h - 1;
	/* bottom right */
	pxy[4] = pxy[0] + gr->g_w - 1;
	pxy[5] = pxy[3];
	/* top right */
	pxy[6] = pxy[4];
	pxy[7] = pxy[1];
	/* top left, again */
	pxy[8] = pxy[0];
	pxy[9] = pxy[1];
#if 0
	/* avoid drawing the same pixel twice in XOR */
	if (os->dc->wmode == MD_XOR)
		pxy[8]++;
#endif
	
	v_pline(handle, 5, pxy);
}

/******************************************************************************/
/* W_Ellipse()                                                                */
/* -------------------------------------------------------------------------- */
/* Entwickler......:   Th.Otto                                                */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Zeichnet eine Ellipse                                                      */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> gr           = Zeiger auf Rechteck, das die Ellipse umschliesst         */
/* -> fill         = Wenn TRUE, wird die Ellipse (mit color) gefuellt         */
/* -> color        = Fuellfarbe, wenn fill==TRUE                              */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Ellipse(_WORD handle, const GRECT *gr, _BOOL fill, _WORD style, _WORD color, _BOOL perimeter)
{
	_WORD xradius, yradius;
	_WORD x, y;
	
	xradius = gr->g_w / 2;
	yradius = gr->g_h / 2;
	x = gr->g_x + xradius;
	y = gr->g_y + yradius;
	if (fill)
	{
		vsf_perimeter(handle, perimeter);
		set_fill_style(handle, style, color);
		v_ellpie(handle, x, y, xradius, yradius, 0, 3600);
		vsf_perimeter(handle, FALSE);
	} else
	{
		v_ellarc(handle, x, y, xradius, yradius, 0, 3600);
	}
}

/* ------------------------------------------------------------------- */

#undef xywh2rect
void xywh2rect(_WORD x, _WORD y, _WORD w, _WORD h, GRECT * gr)
{
	gr->g_x = x;
	gr->g_y = y;
	gr->g_w = w;
	gr->g_h = h;
}

/*** ---------------------------------------------------------------------- ***/

CICON *cicon_best_match(CICONBLK *cib, _WORD planes)
{
	_WORD best_match = 0;
	CICON *best = NULL;
	CICON *list;
	
	for (list = cib->mainlist; list != NULL; list = list->next_res)
	{
		if (list->num_planes > best_match && list->num_planes <= planes)
		{
			best_match = list->num_planes;
			best = list;
		}
	}
	return best;
}


CICON *cicon_find_plane(CICON *list, _WORD planes)
{
	while (list != NULL)
	{
		if (list->num_planes == planes)
			return list;
		list = list->next_res;
	}
	return NULL;
}

/*** ---------------------------------------------------------------------- ***/

static void scrfdb(MFDB *fdb)
{
	fdb->fd_addr = (void *) 0;
	GetScreenSize(&fdb->fd_w, &fdb->fd_h);
	fdb->fd_wdwidth = (fdb->fd_w >> 4);
	fdb->fd_stand = FALSE;
	fdb->fd_nplanes = GetNumPlanes();
	fdb->fd_r1 = fdb->fd_r2 = fdb->fd_r3 = 0;
}

/*** ---------------------------------------------------------------------- ***/

void W_Draw_Icon(_WORD handle, _WORD x, _WORD y, _WORD w, _WORD h, void *data, void *mask, _WORD datacol, _WORD maskcol, _BOOL selected)
{
	MFDB src, dst;
	_WORD pxy[8];
	_WORD colors[2];
	
	scrfdb(&dst);

	src.fd_w = w;
	src.fd_h = h;
	src.fd_wdwidth = (w + 15) >> 4;
	src.fd_stand = TRUE;
	src.fd_nplanes = 1;
	src.fd_r1 = 0;
	src.fd_r2 = 0;
	src.fd_r3 = 0;
	
	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = w - 1;
	pxy[3] = h - 1;
	pxy[4] = x;
	pxy[5] = y;
	pxy[6] = pxy[4] + w - 1;
	pxy[7] = pxy[5] + h - 1;

	src.fd_addr = mask;
	colors[0] = selected ? datacol : maskcol;
	colors[1] = G_WHITE;
	vrt_cpyfm(handle, MD_TRANS, pxy, &src, &dst, colors);
	colors[0] = selected ? maskcol : datacol;
	colors[1] = G_WHITE;
	src.fd_addr = data;
	vrt_cpyfm(handle, MD_TRANS, pxy, &src, &dst, colors);
}

/******************************************************************************/
/* W_Draw_Image()                                                             */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Zeichnet ein Bild von einer Plane auf dem Bildschirm.                      */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> x,y,w,h      = Position an der das Bild gezeichnet werden soll          */
/* -> data         = Daten des Bildes                                         */
/* -> color        = Farbe fuer die gesetzten Bits des Bildes                 */
/* -> mode         = Modus in dem gezeichnet wird (siehe W_SetBkMode)         */
/* -> selected     = TRUE wenn das Bild ivertiert gezeichnet werden soll      */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Draw_Image(_WORD handle, _WORD x, _WORD y, _WORD w, _WORD h, void *data, _WORD fg, _WORD bg, _WORD mode)
{
	_WORD pxy[8], colors[2];
	MFDB src, dst;

	if (data == NULL)
		return;
	scrfdb(&dst);	/* screen */
	src.fd_addr = data;
	src.fd_wdwidth = (w + 15) >> 4;
	src.fd_w = w;
	src.fd_h = h;
	src.fd_stand = FALSE;
	src.fd_nplanes = 1;
	src.fd_r1 = src.fd_r2 = src.fd_r3 = 0;
	
	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = src.fd_w - 1;
	pxy[3] = src.fd_h - 1;
	pxy[4] = x;
	pxy[5] = y;
	pxy[6] = pxy[4] + pxy[2];
	pxy[7] = pxy[5] + pxy[3];

	colors[0] = fg;
	colors[1] = bg;
	vrt_cpyfm(handle, mode, pxy, &src, &dst, colors);
}

/******************************************************************************/
/* W_Draw_Cicon()                                                             */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Zeichnet ein mehrfarbiges Icon auf dem Bildschirm.                         */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> x,y,w,h      = Position an der das Icon gezeichnet werden soll          */
/* -> CICON        = Daten des Bildes                                         */
/* -> datacol      = Farbe fuer die gesetzten Bits des Icons                  */
/* -> maskcol      = Farbe fuer die gesetzten Bits der Maske                  */
/* -> selected     = TRUE wenn das Icon invertiert gezeichnet werden soll     */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Draw_Cicon(_WORD handle, _WORD x, _WORD y, _WORD w, _WORD h, CICON *color, _WORD fgcol, _WORD bgcol, _WORD state)
{
	/*
	 * this routine was intended to just draw the icon image.
	 * Fall back to the AES version, by constructing an CICON with no text.
	 */
	CICONBLK icon;
	char c = '\0';
	OBJECT ob;
	
	UNUSED(handle);
	icon.monoblk.ib_pmask = NULL;
	icon.monoblk.ib_pdata = NULL;
	icon.monoblk.ib_ptext = &c;
	icon.monoblk.ib_char = (fgcol << 12) | (bgcol << 8);
	icon.monoblk.ib_xchar = 0;
	icon.monoblk.ib_ychar = 0;
	icon.monoblk.ib_xicon = 0;
	icon.monoblk.ib_yicon = 0;
	icon.monoblk.ib_wicon = w;
	icon.monoblk.ib_hicon = h;
	icon.monoblk.ib_xtext = 0;
	icon.monoblk.ib_ytext = 0;
	icon.monoblk.ib_wtext = 0;
	icon.monoblk.ib_htext = 0;
	icon.mainlist = color;
	ob.ob_next = NIL;
	ob.ob_head = NIL;
	ob.ob_tail = NIL;
	ob.ob_type = G_CICON;
	ob.ob_flags = OF_LASTOB;
	ob.ob_state = state;
	ob.ob_spec.ciconblk = &icon;
	ob.ob_x = x;
	ob.ob_y = y;
	ob.ob_width = w;
	ob.ob_height = h;
	objc_draw(&ob, ROOT, 1, od_parm.pb_xc, od_parm.pb_yc, od_parm.pb_wc, od_parm.pb_hc);
}

/******************************************************************************/
/* W_Text()                                                                   */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Gibt einen Text im momentan eingestellten Zeichensatz aus.                 */
/* Die Koordinaten beziehen sich auf Basis-Linie des Zeichensatzes.           */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> xx           = X-Koordinate                                             */
/* -> yy           = Y-Koordinate                                             */
/* -> str          = Auszugebender Text                                       */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_Text(_WORD handle, _WORD xx, _WORD yy, const char *str)
{
	v_gtext(handle, xx, yy, str);
}

/******************************************************************************/
/* W_ClipText()                                                               */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Erlaubt die Ausgabe von Text innerhalb der Grenzen eines                   */
/* Rechtecks, wobei die Position des Textes innerhalb des Rechtecks           */
/* bestimmt werden kann. Text wird an den Grenzen des Rechtecks               */
/* abgeschnitten                                                              */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> gr           = Zeiger auf Rechteck, welches den Text                    */
/*                   umschliesst                                              */
/* -> str          = Zeiger auf Text, der ausgegeben werden soll              */
/* -> hdir         = Bestimmt, wie der Text horizontal innerhalb des          */
/*                   Rechtecks plaziert werden soll                           */
/*   moegliche Werte:                                                         */
/*      -1 : linksbuendig                                                     */
/*       0 : mitte                                                            */
/*       1 : rechtsbuendig                                                    */
/* -> hdir         = Bestimmt, wie der Text vertikal innerhalb des            */
/*                   Rechtecks plaziert werden soll                           */
/*   moegliche Werte:                                                         */
/*      -1 : oben                                                             */
/*       0 : mitte                                                            */
/*       1 : unten                                                            */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   keine                                                    */
/******************************************************************************/

void W_ClipText(_WORD handle, GRECT *gr, const char *str, _WORD hdir, _WORD vdir)
{
	_WORD x, y, w, h;
	_WORD extend[8];
	
	vqt_extent(handle, str, extend);
	w = extend[2] - extend[0];
	h = extend[5] - extend[1];
	switch (vdir)
	{
	case -1:
	default:
		y = 0;
		break;
	case 0:
		y = (gr->g_h - h) / 2;
		break;
	case 1:
		y = gr->g_h - h;
		break;
	}
	switch (hdir)
	{
	case -1:
	default:
		x = 0;
		break;
	case 0:
		x = (gr->g_w - w) / 2;
		break;
	case 1:
		x = gr->g_w - w;
		break;
	}

	W_Text(handle, gr->g_x + x, gr->g_y + y, str);
}

/******************************************************************************/
/* W_TextWidth()                                                              */
/* -------------------------------------------------------------------------- */
/* Entwickler......: Th.Otto                                                  */
/* 1.Version.......:                                                          */
/* letzte Aenderung:                                                          */
/* -------------------------------------------------------------------------- */
/* Gibt die Breite des Textes (text) in Bildpunkten zurueck. Die              */
/* Funktion ist notwendig, seit in  Metabildern und auf Drucker               */
/* TRUE-TYPE-Fonts benutzt werden.                                            */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> os           = Zeiger auf Ausgabegeraete/Geraetekontexte                */
/* -> text         = Zeiger auf Text, der zu untersuchen ist                  */
/* -------------------------------------------------------------------------- */
/* Rueckgabe:                                                                 */
/*                   Breite des Textes in Bildpunkten                         */
/******************************************************************************/

_WORD W_TextWidth(_WORD handle, const char *text)
{
	_WORD w;
	_WORD extend[8];
	
	vqt_extent(handle, text, extend);
	w = extend[2] - extend[0];
	return w;
}
