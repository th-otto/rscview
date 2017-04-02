/*	OPTIMIZE.C	1/25/84 - 06/05/85	Lee Jay Lorenzen	*/
/*	merge High C vers. w. 2.2 		8/25/87		mdf	*/
/*	modify fs_sset				10/30/87	mdf	*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 3.0
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "aes.h"
#include "aesutils.h"
#include <stdarg.h>


/*	Returns a byte pointer pointing to the matched byte or
 *	the end of the string.
 */
const char *scasb(const char *p, char b)
{
	for (; *p && *p != b; p++)
		;
	return p;
}



/*
 * Copy src xywh block to dest xywh block.
 */
void rc_copy(const GRECT *src, GRECT *dst)
{
	*dst = *src;
}


/*
 * Return true if the x,y position is within the grect.
 */
_BOOL inside(_WORD x, _WORD y, const GRECT *pt)
{
	if ((x >= pt->g_x) && (y >= pt->g_y) && (x < pt->g_x + pt->g_w) && (y < pt->g_y + pt->g_h))
		return TRUE;
	return FALSE;
}


/*	Routine to constrain a box within another box.	This is done by
 *	seting the x,y of the inner box to remain within the
 *	constraining box.
 */
void rc_constrain(const GRECT *pc, GRECT *pt)
{
	if (pt->g_x < pc->g_x)
		pt->g_x = pc->g_x;
	if (pt->g_y < pc->g_y)
		pt->g_y = pc->g_y;
	if ((pt->g_x + pt->g_w) > (pc->g_x + pc->g_w))
		pt->g_x = (pc->g_x + pc->g_w) - pt->g_w;
	if ((pt->g_y + pt->g_h) > (pc->g_y + pc->g_h))
		pt->g_y = (pc->g_y + pc->g_h) - pt->g_h;
}


/*
 * Return upper case value.
 */
int toupper(int ch)
{
	if (ch >= 'a' && ch <= 'z')
		ch -= 0x20;
	return ch;
}


/*
 * Length of a string
 */
size_t strlen(const char *p1)
{
	register _WORD len;

	len = 0;
	while (*p1++)
		len++;

	return len;
}


_BOOL streq(const char *p1, const char *p2)
{
	while (*p1)
	{
		if (*p1++ != *p2++)
			return FALSE;
	}
	if (*p2)
		return FALSE;
	return TRUE;
}


char *xstrpcpy(const char *ps, char *pd)
{
	while ((*pd++ = *ps++) != '\0')
		;
	return pd;
}


/*	copy the src to destination until we are out of characters
 *	or we get a char match.
 */
char *strscn(const char *ps, char *pd, char stop)
{
	while ((*ps) && (*ps != stop))
		*pd++ = *ps++;
	return pd;
}


char *xstrpcat(const char *ps, char *pd)
{
	while (*pd)
		pd++;
	while ((*pd++ = *ps++) != '\0')
		;
	return (pd);
}


/*	This is the true version of strcmp. Shall we remove the
 *	other -we shall see!!!
 *	Returns - <0 if(str1<str2), 0 if(str1=str2), >0 if(str1>str2)
 */
_WORD strchk(const char *s, const char *t)
{
	register _WORD i;

	i = 0;
	while (s[i] == t[i])
		if (s[i++] == 0)
			return 0;
	return (s[i] - t[i]);
}



/*
 *	Strip out period and turn into raw data.
 */
void fmt_str(const char *in_str, char *out_str)
{
	_WORD i;

	for (i = 0; *in_str; in_str++)
	{
		if (*in_str == '.')
		{
			in_str++;
			break;
		}
		if (i < 8)
		{
			*out_str++ = *in_str;
			i++;
		}
	}

	/* if any extension present, fill out with spaces, then copy extension */
	if (*in_str)
	{
		while (i < 8)
		{
			*out_str++ = ' ';
			i++;
		}
		while ((i < 11) && *in_str)
		{
			*out_str++ = *in_str++;
			i++;
		}
	}

	*out_str = '\0';
}


/*
 *	Insert in period and make into true data.
 */
void unfmt_str(const char *in_str, char *out_str)
{
	_WORD i;
	char temp;

	for (i = 0; i < 8 && *in_str; i++)
	{
		temp = *in_str++;
		if (temp != ' ')
			*out_str++ = temp;
	}

	if (*in_str)							/* any extension ? */
	{
		*out_str++ = '.';
		while (*in_str)
			*out_str++ = *in_str++;
	}
	*out_str = '\0';
}


/*	Copy the long in the ob_spec field to the callers variable
 *	ptext.	Next copy the string located at the ob_spec long to the
 *	callers pstr.  Finally copy the length of the tedinfo string
 *	to the callers ptxtlen.
 *	obj must reference a TEDINFO object.
 */
void fs_sset(OBJECT *tree, _WORD obj, char *pstr, char **ptext, _WORD *ptxtlen)
{
	TEDINFO *ted;

	ted = tree[obj].ob_spec.tedinfo;
	*ptext = ted->te_ptext;
	strcpy(*ptext, pstr);
	*ptxtlen = ted->te_txtlen;
}


void inf_sset(OBJECT *tree, _WORD obj, char *pstr)
{
	char *text;
	_WORD txtlen;

	fs_sset(tree, obj, pstr, &text, &txtlen);
}


/*	fs_sget
 *	This routine copies the tedinfo string to the dst pointer.
 *	The function inf_sget was the same as fs_sget.
 */
void fs_sget(OBJECT *tree, _WORD obj, char *pstr)
{
	char *ptext;

	ptext = tree[obj].ob_spec.tedinfo->te_ptext;

	strcpy(pstr, ptext);
}



/*	This routine is used to set an objects flags based on 
 *	the outcome of a 'and' operation.  The word is set to
 *	the 'truestate' if the operation is true else set to
 *	'falsestate'
 */
void inf_fldset(OBJECT *tree, _WORD obj, _UWORD testfld, _UWORD testbit, _UWORD truestate, _UWORD falsestate)
{
	tree[obj].ob_state = (testfld & testbit) ? truestate : falsestate;
}


/* inf_gindex	for each object from baseobj for N objects return the object
 *		that is selected or -1 if no objects are selected.
 */
_WORD inf_gindex(OBJECT *tree, _UWORD baseobj, _UWORD numobj)
{
	_WORD retobj;

	tree += baseobj;
	for (retobj = 0; retobj < numobj; retobj++)
	{
		if (tree[retobj].ob_state & SELECTED)
			return retobj;
	}
	return -1;
}


/*
 *	Return 0 if cancel was selected, 1 if okay was selected, -1 if
 *	nothing was selected.
 */
_WORD inf_what(OBJECT *tree, _WORD ok)
{
	register _WORD field;

	field = inf_gindex(tree, ok, 2);

	if (field >= 0)
	{
		tree[ok + field].ob_state = NORMAL;
		field = field == 0;
	}
	return field;
}



void merge_str(char *pdst, const char *ptmp, va_list parms)
{
	_WORD do_value;
	char lholder[12];
	register char *pnum;
	char *psrc;
	register int32_t lvalue, divten;
	_WORD digit;
	
	while (*ptmp)
	{
		if (*ptmp != '%')
		{
			*pdst++ = *ptmp++;
		} else
		{
			ptmp++;
			do_value = FALSE;
			switch (*ptmp++)
			{
			case '%':
				*pdst++ = '%';
				break;
			case 'L':
				lvalue = va_arg(parms, _LONG);
				do_value = TRUE;
				break;
			case 'W':
				lvalue = va_arg(parms, int);
				do_value = TRUE;
				break;
			case 'S':
				psrc = va_arg(parms, char *);
				while (*psrc)
					*pdst++ = *psrc++;
				break;
			}
			if (do_value)
			{
				pnum = &lholder[0];
				while (lvalue)
				{
					divten = lvalue / 10;
					digit = (_WORD) lvalue - (divten * 10);
					*pnum++ = '0' + digit;
					lvalue = divten;
				}
				if (pnum == &lholder[0])
				{
					*pdst++ = '0';
				} else
				{
					while (pnum != &lholder[0])
						*pdst++ = *--pnum;
				}
			}
		}
	}
	*pdst = '\0';
}



/*
 *	Routine to see if the test filename matches a standard TOS
 *	wildcard string.  For example:
 *		pattern = "*.BAT"
 *		filename = "MYFILE.BAT"
 */
_WORD wildcmp(const char *pwild, const char *ptest)
{
	while (*ptest && *pwild)
	{
		if (*pwild == '?')
		{
			pwild++;
			if (*ptest != '.')
				ptest++;
		} else
		{
			if (*pwild == '*')
			{
				if (*ptest != '.')
					ptest++;
				else
					pwild++;
			} else
			{
				if (*ptest == *pwild)
				{
					pwild++;
					ptest++;
				} else
				{
					return FALSE;
				}
			}
		}
	}
	/* eat up remaining wildcard chars */
	while (*pwild == '*' || *pwild == '?' || *pwild == '.')
		pwild++;
	/* if any part of wildcard or test is left then no match */
	if (*pwild || *ptest)
		return FALSE;
	return TRUE;
}



/*
 * strlcpy: a better strcnpy()
 *
 * strlcpy() was introduced in OpenBSD in the 90s as a
 * replacement for strncpy().  it has three improvements
 * compared to strncpy():
 *    . it always nul-terminates the destination (as long
 *      as the length is > 0)
 *    . it does not nul-fill the destination (a performance
 *      benefit when the source is short and the destination
 *      is large)
 *    . the return value allows a simple test for truncation
 *
 * this implementation of strlcpy() was written from scratch
 * by roger burrows.  it is based on the description of the
 * function in the paper "strlcpy and strlcat - consistent,
 * safe string copy and concatenation", by todd c. miller
 * and theo de raadt.
 *
 * description: strlcpy() copies bytes from src to dest,
 * stopping when the last (pre-NUL) byte of src is reached
 * OR count-1 bytes have been copied, whichever comes first.
 * as long as count is not zero, dest is then NUL-terminated.
 *
 * strlcpy() returns the length of src (excluding the
 * terminating NUL).  this allows a simple test for string
 * truncation: if the return value is greater than or equal
 * to the specified length, then truncation has occurred.
 */
size_t strlcpy(char *dest, size_t count, const char *src)
{
char *d = dest;
const char *s = src;
size_t n;

    if (count > 0) {
        for (n = count-1; *s && n; n--)
            *d++ = *s++;
        *d = '\0';
    }

    while (*s++)
        ;

    return s-src-1;
}
