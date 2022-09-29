#include "aes.h"
#include "nls.h"
#include "gem_rsc.h"
#define rs_fstr rs_frstr
#include "gem_rsc.rsh"
#include "s_endian.h"

OBJECT **aes_rsc_tree = (OBJECT **)rs_trindex;
const BITBLK *const *aes_rsc_bitblk = rs_frimg;

/* Strings for the alert box */
static char msg_str[MAX_LINENUM][MAX_LINELEN+1];
static char msg_but[MAX_BUTNUM][MAX_BUTLEN+1];

static nls_domain aes_domain = { "aes", NULL, CHARSET_ST, NULL };


/*** ---------------------------------------------------------------------- ***/

/*
 *  trims trailing spaces from string
 */
static void trim_spaces(char *string)
{
	char *p;

	for (p = string + strlen(string) - 1; p >= string; p--)
		if (*p != ' ')
			break;
	p[1] = '\0';
}

/*** ---------------------------------------------------------------------- ***/

static void xlate_obj_array(nls_domain *domain, OBJECT *obj_array, _LONG nobs, _BOOL trim_strings)
{
	OBJECT *obj;
	
	for (obj = obj_array; --nobs >= 0; obj++)
	{
		_WORD type = obj->ob_type & OBTYPEMASK;
		switch (type)
		{
		case G_TEXT:
		case G_FTEXT:
			obj->ob_spec.tedinfo->te_ptext = nls_dgettext(domain, obj->ob_spec.tedinfo->te_ptext);
			break;
		case G_BOXTEXT:
		case G_FBOXTEXT:
			obj->ob_spec.tedinfo->te_ptmplt = nls_dgettext(domain, obj->ob_spec.tedinfo->te_ptmplt);
			break;
		case G_STRING:
		case G_BUTTON:
		case G_TITLE:
			if (type == G_STRING && trim_strings)
				trim_spaces(obj->ob_spec.free_string);
			obj->ob_spec.free_string = nls_dgettext(domain, obj->ob_spec.free_string);
			break;
		default:
			break;
		}
	}
}

_BOOL rsc_read(void)
{
	OBJECT *tree;
	int i;
	
	/* Copy data from ROM to RAM: */
	memcpy(rs_object, rs_object_rom, sizeof(rs_object));
	memcpy(rs_tedinfo, rs_tedinfo_rom, sizeof(rs_tedinfo));

	/* translate strings in objects */
	xlate_obj_array(&aes_domain, rs_object, NUM_OBS, FALSE);

	/*
	 * Set up message & button buffers for form_alert().
	 */
	tree = rs_trindex[DIALERT];
	for (i = 0; i < MAX_LINENUM; i++)
		tree[MSGOFF + i].ob_spec.free_string = msg_str[i];
	for (i = 0; i < MAX_BUTNUM; i++)
		tree[BUTOFF + i].ob_spec.free_string = msg_but[i];
	return TRUE;
}


void rsc_free(void)
{
}


/* Get a string from the GEM-RSC */
char *rs_str(_UWORD stnum)
{
	return nls_dgettext(&aes_domain, rs_frstr[stnum]);
}
