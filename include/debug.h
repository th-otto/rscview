/*****************************************************************************
 * DEBUG.H
 *****************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifndef __PORTAB_H__
#  include <portab.h>
#endif

#ifdef OS_ATARI
#include <mint/arch/nf_ops.h>
#else
#ifndef SPEC_DEBUG
#define nf_debugprintf(format...)
#endif
#endif

EXTERN_C_BEG

typedef enum
{
	EO_INFO,
	EO_TRACE,
	EO_ERROR,
	EO_FATAL,
	EO_DEBUG,
	EO_INIT,
	EO_EXIT
} EO_MODUS;

typedef _BOOL (*ERROROUT_FUNC)(
	const char *prgName,
	EO_MODUS modus,
	_WORD nr,
	const char *modeStr,
	const char *errStr
);

_VOID ErrorOut(EO_MODUS modus, const char *str, ...) __attribute__((format(printf, 2, 3)));

ERROROUT_FUNC ErrorOut_SetFunk(ERROROUT_FUNC UseFunk);
void ErrorStrOut(const char *str);
_BOOL error_std_out(EO_MODUS modus, const char *ptrPrgName, const char *str);

_BOOL _assert_ ( const char *expr, const char *file, _LONG line );
void set_assert ( _VOID (*p_assert) ( const char *expr, const char *fname, _LONG line ) );

#if !defined(OS_ATARI) && defined(SPEC_DEBUG)
void nf_debugprintf(const char *format, ...) __attribute__((format(printf, 1, 2)));
#endif

EXTERN_C_END

#endif /* __DEBUG_H__ */
