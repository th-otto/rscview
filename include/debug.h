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

#if !defined(OS_ATARI) && defined(SPEC_DEBUG)
void nf_debugprintf(const char *format, ...) __attribute__((format(printf, 1, 2)));
#endif

#define KINFO(args) nf_debugprintf args

#ifdef ENABLE_KDEBUG
#define KDEBUG(args) KINFO(args)
#else
#define KDEBUG(args)
#endif

EXTERN_C_END

#endif /* __DEBUG_H__ */
