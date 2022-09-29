#ifndef __AESUTILS_H__
#define __AESUTILS_H__

#include <stdarg.h>
#include <nls.h>
#include "visibility.h"

#undef min
#undef max
#define max(x,y)	(((x) > (y)) ? (x) : (y))
#define min(x,y)	(((x) < (y)) ? (x) : (y))


EXTERN_C_BEG


/* 	Routine to set the variables x,y,w,h to the values found
 *	in an x,y,w,h block (grect)
 */
static INLINE void r_get(const GRECT *gr, _WORD *px, _WORD *py, _WORD *pw, _WORD *ph)
{
	*px = gr->g_x;
	*py = gr->g_y;
	*pw = gr->g_w;
	*ph = gr->g_h;
}


/* 	Routine to set a x,y,w,h block to the x,y,w,h  
 *	values passed in.
 */
static INLINE void r_set(GRECT *gr, _WORD x, _WORD y, _WORD w, _WORD h)
{
	gr->g_x = x;
	gr->g_y = y;
	gr->g_w = w;
	gr->g_h = h;
}

#define scasb        __hidden_aes_scasb
#define inside       __hidden_aes_inside
#define inside       __hidden_aes_inside
#define rc_constrain __hidden_aes_rc_constrain
#define strscn       __hidden_aes_strscn
#define strchk       __hidden_aes_strchk
#define fmt_str      __hidden_aes_fmt_str
#define unfmt_str    __hidden_aes_unfmt_str
#define fs_sset      __hidden_aes_fs_sset
#define inf_sset     __hidden_aes_inf_sset
#define fs_sget      __hidden_aes_fs_sget
#define inf_fldset   __hidden_aes_inf_fldset
#define merge_str    __hidden_aes_merge_str
#define wildcmp      __hidden_aes_wildcmp
#define strmaxcpy    __hidden_aes_strmaxcpy
#define reverse      __hidden_aes_reverse
#define streq        __hidden_aes_streq
#define inf_gindex   __hidden_aes_inf_gindex
#define inf_what     __hidden_aes_inf_what
#define aes_toupper  __hidden_aes_toupper

/*
 * Copy src xywh block to dest xywh block.
 */
#define rc_copy(src, dst) (*(dst) = *(src))


/*
 * aesutils.c
 */
VISIBILITY("hidden") const char *scasb(const char *p, char b);
VISIBILITY("hidden") _BOOL inside(_WORD x, _WORD y, const GRECT *pt);
VISIBILITY("hidden") void rc_constrain(const GRECT *pc, GRECT *pt);
VISIBILITY("hidden") char *strscn(const char *src, char *dst, char stp);
VISIBILITY("hidden") _WORD strchk(const char *s, const char *t);
VISIBILITY("hidden") void fmt_str(const char *instr, char *outstr);
VISIBILITY("hidden") void unfmt_str(const char *instr, char *outstr);
VISIBILITY("hidden") void fs_sset(OBJECT *tree, _WORD obj, const char *pstr, char **ptext, _WORD *ptxtlen);
VISIBILITY("hidden") void inf_sset(OBJECT *tree, _WORD obj, char *pstr);
VISIBILITY("hidden") void fs_sget(OBJECT *tree, _WORD obj, char *pstr);
VISIBILITY("hidden") void inf_fldset(OBJECT *tree, _WORD obj, _UWORD testfld, _UWORD testbit, _UWORD truestate, _UWORD falsestate);
VISIBILITY("hidden") void merge_str(char *pdst, const char *ptmp, va_list parms);
VISIBILITY("hidden") _WORD wildcmp(const char *pwild, const char *ptest);
VISIBILITY("hidden") size_t strmaxcpy(char *dst, size_t len, const char *src);
VISIBILITY("hidden") unsigned int reverse(unsigned int index);
VISIBILITY("hidden") _BOOL streq(const char *p1, const char *p2);
VISIBILITY("hidden") _WORD inf_gindex(OBJECT *tree, _UWORD baseobj, _UWORD numobj);
VISIBILITY("hidden") _WORD inf_what(OBJECT *tree, _WORD ok);

VISIBILITY("hidden") int aes_toupper(int ch);

/*
 * mul_div - signed integer multiply and divide
 *
 * mul_div (m1,m2,d1)
 *
 * ( ( m1 * m2 ) / d1 ) + 1/2
 *
 * m1 = signed 16 bit integer
 * m2 = unsigned 15 bit integer
 * d1 = signed 16 bit integer
 */

/*
 * mul_div - signed integer multiply and divide
 * return ( m1 * m2 ) / d1
 * While the operands are WORD, the intermediate result is LONG.
 */
static INLINE _WORD mul_div(_WORD m1, _WORD m2, _WORD d1)
{
#if defined(__mc68000__) && defined(__GNUC__)
    __asm__ (
      "muls %1,%0\n\t"
      "divs %2,%0"
    : "+d"(m1)
    : "idm"(m2), "idm"(d1)
    );
    return m1;
#else
	return ((_LONG) m1 * m2) / d1;
#endif
}

/*
 *  WORD mul_div_round(WORD mult1,WORD mult2,WORD divisor)
 *
 *  multiplies two WORDs together and divides by a WORD, returning a WORD.
 *  the result is rounded away from zero if the remainder is greater than
 *  or equal to half the divisor.
 *
 *  if you do not need rounding, use mul_div() instead!
 */
static INLINE _WORD mul_div_round(_WORD m1, _WORD m2, _WORD d1)
{
	_LONG inc = 1;
	_LONG l = (_LONG) m1 * m2;
	_WORD rem;

	if (l < 0)
		inc = -inc;
	rem = l % d1;
	l /= d1;
	if (d1 < 0)
	{
		inc = -inc;
		d1 = -d1;
	}
	if (rem < 0)
		rem = -rem;
	rem <<= 1;
	if (rem >= d1)
		l += inc;
	return l;
}

EXTERN_C_END

#endif /* __AESUTILS_H__ */
