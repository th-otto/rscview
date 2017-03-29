/*****************************************************************************
*
* PORTAB.H
*
* Use of this file may make your code compatible with all C compilers
* listed.
*
*
* Folgende Konstanten werden z.Z. benutzt
*
* OS_WINDOWS		Windows 3.1/Win32s Version
* OS_ATARI          Atari (TOS) Version
* OS_UNIX			Unix/X Window System
*
* STDC_HEADERS
*	   wenn ANSI-Header-Dateien (stdlib.h, stddef.h, string.h und stdarg.h)
*	   verfuegbar sind.
* HAVE_xxxx_H
*	   wenn die entsprechende Header-Datei verfuegbar ist
* HAVE_xxxx
*	   wenn die entsprechende Funktion verfuegbar ist
* HAVE_DIRENT_H
*	   wenn dirent.h und opendir() etc. verfuegbar sind
*
* _HUGE
*	   Schluesselwort huge
* _CDECL
*	   Schluesselwort cdecl
* VOLATILE
*	   Schluesselwort volatile
*
* _BOOL
*	   boolean value (TRUE/FALSE)
* _BYTE
*	   signed byte (8 bits)
* _UBYTE
*	   unsigned byte (8 bits)
* _WORD
*	   signed word (16 bits)
* _UWORD
*	   unsigned word (16 bits)
* _LONG
*	   signed long (32 bits)
* _ULONG
*	   unsigned long (32 bits)
* _LONG64
*	   signed long long (64 bits, defined in longlong.h)
* _ULONG64
*	   unsigned long long (64 bits, defined int longlong.h)
* _FLOAT
*	   single precision floating point value
* _DOUBLE
*	   double precision floating point value
*
* PATH_MAX
*	   maximale Laenge von kompletten Pfadangaben
*****************************************************************************/

#ifndef __PORTAB_H__
#define __PORTAB_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>

/* ANSI Compiler ist schon Voraussetzung */
#ifndef __STDC__
#  ifndef _AIX /* complains about __STDC__ */
#    ifndef hpux /* does not compile with __STDC__ defined without -Ae */
#	    define __STDC__ 1
#    endif
#  endif
#else
#  if !__STDC__
you loose
#  endif
#endif


#ifndef __GNUC_PREREQ
# ifdef __GNUC__
#   define __GNUC_PREREQ(maj, min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
# else
#   define __GNUC_PREREQ(maj, min) 0
# endif
#endif

/*****************************************************************************/
/* Windows 3.1 / Win32s / Windows 95 / Windows NT / Windows 2000 / MSDOS     */
/*****************************************************************************/
#ifdef _MSC_VER
#ifndef __WIN32__
#define __WIN32__ 1
#endif
#ifndef __MSDOS__
#define OS_WINDOWS 1
#endif
#endif

#if defined(_WIN32) && !defined(__WIN32__)
#  define __WIN32__ 1
#endif
#ifdef __CYGWIN__
#  ifndef __MSDOS__
#    define __MSDOS__ 1
#  endif
#  ifndef __WIN32__
#    define __WIN32__ 1
#  endif
#endif
#ifdef __LCC__
#  ifndef __WIN32__
#    define __WIN32__ 1
#  endif
#endif
#ifdef __MINGW32__
#  ifndef __WIN32__
#    define __WIN32__ 1
#  endif
#endif
#ifdef __WIN32__
#  define OS_WINDOWS 1
#endif
#ifdef _Windows
#  define OS_WINDOWS 1
#endif
#ifdef __MSDOS__
#  define OS_MSDOS 1
#endif

#if defined(OS_WINDOWS) || defined(OS_MSDOS) || defined(__WIN32__)

#define HOST_BYTE_ORDER BYTE_ORDER_LITTLE_ENDIAN

#define HAVE_STRING_H 1
#define STDC_HEADERS 1
#define HAVE_DIRENT_H 1
#define __IEEE_LITTLE_ENDIAN

#define HAVE_DOS_BASED_FILE_SYSTEM 1

/*
 * Begin Borland-C
 */
#ifdef __BORLANDC__
#define HAVE_FCNTL_H 1
#define HAVE_IO_H 1
#define HAVE_ALLOC_H 1
#define HAVE_STRSTR 1
#define HAVE_LIMITS_H 1
#define HAVE_SHELLAPI_H 1
#define HAVE_MMSYSTEM_H 1
#define HAVE_VA_LIST_NULL 1
#ifdef OS_MSDOS
#define HAVE_DOS_H 1
#endif
#ifdef __WIN32__
#define HAVE_SHLOBJ_H 1
#define HAVE_WINNLS_H 1
#define HAVE_WINBASE_H 1
#endif
#if __BORLANDC__ >= 0x500
#define HAVE_ANONYMOUS_STRUCTS 1
#endif
#if __BORLANDC__ >= 0x520 && 0
#define HAVE_LONGLONG 1
#  define LONGLONG_TYPE __int64
#  define ULONGLONG_TYPE unsigned __int64
#endif
#undef UNUSED
#define UNUSED(x) (void)(x)
#if __BORLANDC__ >= 0x520
#define INLINE __inline
#endif
#define STAT_MACROS_BROKEN
#endif


/*
 * Begin Microsoft Visual C
 */
#ifdef _MSC_VER
#define HAVE_FCNTL_H 1
#define HAVE_IO_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_STRSTR 1
#define HAVE_SHELLAPI_H 1
#define HAVE_MMSYSTEM_H 1
#define HAVE_WINBASE_H 1
#define HAVE_WINNLS_H 1
#define HAVE_DOS_H 1
#define HAVE_VA_LIST_NULL 1
#ifdef __WIN32__
#if _MSC_VER >= 1000
#define HAVE_SHLOBJ_H 1
#endif
#define HAVE_WINNLS_H 1
#define HAVE_WINBASE_H 1
#endif
#define _CDECL
#define fileno _fileno
#define read   _read
#define open   _open
#define close  _close
#define write  _write
#define creat  _creat
#define lseek _lseek
#define isatty _isatty

#define HAVE_ANONYMOUS_STRUCTS 1

#pragma warning(disable:4018) /* signed/unsigned comparisons */
#pragma warning(disable:4133) /* signed/unsigned char */
#pragma warning(disable:4761) /* integral size mismatch */
#pragma warning(disable:4068) /* unknown pragma */
#pragma warning(disable:4244) /* possible loss of data */
#pragma warning(disable:4305) /* truncation from long to short */
#pragma warning(disable:4245) /* conversion from 'const int ' to 'unsigned int ', signed/unsigned mismatch */
#pragma warning(disable:4057) /* 'const unsigned char *' differs in indirection to slightly different base types from 'char []' */
#pragma warning(disable:4201) /* nonstandard extension used : nameless struct/union */
#pragma warning(disable:4214) /* nonstandard extension used : bit field types other than int */
#pragma warning(disable:4514) /* 'foo' : unreferenced inline function has been removed */

#undef UNUSED
#define UNUSED(x) (void)(x)

#endif /* _MSC_VER */


/*
 * Begin Gnu-C
 */
#ifdef __GNUC__
#define _CDECL
#ifdef __MINGW32__
#if (__MINGW32_MAJOR_VERSION > 0 || (__MINGW32_MAJOR_VERSION == 0 && __MINGW32_MINOR_VERSION >= 3))
#define HAVE_MMSYSTEM_H
#define HAVE_SHLOBJ_H
#define HAVE_SHELLAPI_H
#else
#define HAVE_ALLOC_H 1
#undef HAVE_SHELLAPI_H /* has shellapi.h, but is broken */
#endif
#endif
#ifndef HAVE_CONFIG_H
/*
 * minimum which is assumed, but use autoconf
 */
#define HAVE_STRSTR 1
#define HAVE_LIMITS_H 1
#define HAVE_WINBASE_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_DIRENT_H 1
#define HAVE_SYS_STAT_H 1
#ifdef __CYGWIN__
/* only for cygwin, not for mingw */
#define HAVE_LSTAT 1
#endif
#endif

#define HAVE_ANONYMOUS_STRUCTS 1

#undef UNUSED
#define UNUSED(x) (void)(x)

#endif


/*
 * Begin LCC-Win32
 */
#ifdef __LCC__
#define HAVE_FCNTL_H 1
#define HAVE_IO_H 1
#define HAVE_STRSTR 1
#define HAVE_LIMITS_H 1
#define HAVE_SHELLAPI_H 1
#define HAVE_MMSYSTEM_H 1
#define HAVE_SHLOBJ_H 1
#define HAVE_VA_LIST_NULL 1
/* #define HAVE_WINNLS_H 1 has winnls.h, but already defines its structs in win.h */
#define HAVE_ANONYMOUS_STRUCTS 1

#undef UNUSED
#define UNUSED(x) if(x){}

#endif



#ifdef __WIN32__

#ifndef __FLAT__
#  define __FLAT__ 1
#endif


#ifndef __GNUC__
#ifndef _CDECL
#define _CDECL _cdecl
#endif
#endif
/* __cdecl */
#define _HUGE

#ifdef _MSC_VER
#define EXPORT
#define IMPORT
#endif

#ifdef __BORLANDC__
#define EXPORT __stdcall _export
#define IMPORT __stdcall _import
#endif

#define _WORD signed short
#define _UWORD unsigned short

#ifdef __BORLANDC__
#pragma option -w-sig
#endif

#else /* !__WIN32__ */

#ifdef __BORLANDC__
#define _CDECL _cdecl
#define _HUGE huge
#define EXPORT _far _pascal _export
#define IMPORT _far _pascal

#define _WORD signed int
#define _UWORD unsigned int

#ifdef OS_WINDOWS
#  pragma option -zE_FARDATA
#endif
#undef PACKED
#define PACKED
#endif /* __BORLANDC__ */

#endif /* __WIN32__ */

/* #include <_defs.h> */
/* #include <_nfile.h> */
/* #include <locale.h> */

#ifdef __GNUC__
#include <path_max.h>
#endif
#define ALL_FILE_MASK "*.*"

#undef UNUSED
#define UNUSED(x) (void)(x)

#endif /* OS_WINDOWS || OS_MSDOS || __WIN32__ */


/*****************************************************************************/
/* OS/2 Warp																 */
/*****************************************************************************/

#ifdef __OS2__

#define OS_OS2 1

#if 0
#pragma info(all)
#endif
#pragma info(cmp)
#pragma info(cnd)
#pragma info(cns)
#pragma info(cnv)
#pragma info(cpy)
#pragma info(noeff)
#pragma info(enu)
#pragma info(noext)
#pragma info(gnr)
#pragma info(nogot)
#pragma info(noini)
#pragma info(lan)
#pragma info(obs)
#pragma info(par)
#pragma info(nopor)
#pragma info(noppc)
#pragma info(noppt)
#pragma info(pro)
#pragma info(rea)
#pragma info(ret)
#pragma info(notrd)
#pragma info(tru)
#pragma info(und)
#pragma info(nouni)
#pragma info(use)
#pragma info(vft)
#pragma info(dcl)
#pragma info(ord)


#define HOST_BYTE_ORDER BYTE_ORDER_LITTLE_ENDIAN

#define ALL_FILE_MASK "*.*"

#define HAVE_STRING_H 1
#define STDC_HEADERS 1
#define HAVE_FCNTL_H 1
#define HAVE_IO_H 1
#define HAVE_DIRENT_H 1
#define HAVE_STRSTR 1
#define HAVE_LIMITS_H 1
#define HAVE_VA_LIST_NULL 1

#ifndef _CDECL
/* #define _CDECL _Cdecl */
#endif

#define UNUSED(x) x = x

#endif /* __OS2__ */


/*****************************************************************************/
/* Atari TOS/GEM															 */
/*****************************************************************************/
#if defined(__TOS__) || defined(__atarist__)
#  define OS_ATARI 1
#endif

#ifdef OS_ATARI

#define HAVE_STRING_H 1
#define STDC_HEADERS 1
#define HAVE_STRSTR 1
#define HAVE_VA_LIST_NULL 1

#define HOST_BYTE_ORDER BYTE_ORDER_BIG_ENDIAN

#define ALL_FILE_MASK "*.*"

#ifdef __PUREC__
#define _WORD	  signed int	/* A machine dependent int	   */
#define _UWORD	  unsigned int	/* A machine dependent uint    */
#define __CDECL cdecl
#ifdef __NO_CDECL
#define _CDECL
#else
#define _CDECL	   __CDECL
#endif
#define HAVE_LIMITS_H 1
#define HAVE_STDINT_H 1
#endif

#ifdef __SOZOBONX__
#define CDECL
#define _CDECL
#define __CDECL
#endif

#ifdef __GNUC__
#define _WORD	  signed short
#define _UWORD	  unsigned short
#define HAVE_UNISTD_H 1
#define HAVE_FCNTL_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_DIRENT_H 1
#define HAVE_LIMITS_H 1
#define HAVE_STDINT_H 1
#endif

#if defined(__PUREC__) && defined(_PUREC_SOURCE)
#define fclose(file) purec_fclose(file)
void purec_fclose(FILE *fp);
#endif

#endif /* OS_ATARI */


/*****************************************************************************/
/* X Window System / Unix generell											 */
/*****************************************************************************/
#ifdef _AIX
#  define UNIX 1
#endif
#ifdef unix
#  define OS_UNIX 1
#endif
#ifdef __unix__
#  define OS_UNIX 1
#endif
#ifdef __unix
#  define OS_UNIX 1
#endif
#ifdef UNIX
#  define OS_UNIX 1
#endif
#ifdef hpux
#  define OS_UNIX 1
#endif
#ifdef OS_ATARI
#  undef OS_UNIX
#endif
#ifdef OS_UNIX
#ifndef __WIN32__

#define ALL_FILE_MASK "*"

#endif
#endif /* OS_UNIX */


#ifdef HAVE_LIMITS_H
#  include <limits.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

/*****************************************************************************/
/* DEFAULTS                                                                  */
/*****************************************************************************/

#ifndef RC_INVOKED
#ifdef STDC_HEADERS
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef STDC_HEADERS
#include <string.h>
#else
#include <strings.h>
#define strchr(s,c) index(s, c)
#define strrchr(s,c) rindex(s, c)
#endif
#endif
#ifdef STDC_HEADERS
#include <stdlib.h>
#include <stddef.h>
#endif
#endif /* RC_INVOKED */

#ifndef EXPORT
#define EXPORT
#endif

#ifndef _HUGE
#define _HUGE
#endif

#ifndef _CDECL
#define _CDECL /* */
#endif

#ifndef _VOID
#define _VOID void
#endif

#ifndef _LPVOID
#define _LPVOID void *
#endif

#ifndef _LPBYTE
#  define _LPBYTE char *
#endif

#ifndef _BOOL
#  define _BOOL int				/* 2 valued (true/false)	   */
#endif

#ifndef HAVE_GLIB
typedef int gboolean;
#endif

#if defined(ULONG_MAX) && ULONG_MAX > 0x7ffffffful
#  define _LONG_PTR  intptr_t
#else
#  define _LONG_PTR _LONG
#endif

#ifndef _BYTE
#define _BYTE	signed char
#endif
#ifndef _UBYTE
#define _UBYTE	unsigned char	/* Unsigned byte			   */
#endif

#ifndef _WORD
#define _WORD	signed short	/* signed word (exact 16 bits) */
#endif
#ifndef _UWORD
#define _UWORD	unsigned short	/* unsigned word (exact 16 bits) */
#endif

#ifndef _LONG
#define _LONG	signed long     /* signed long (at least 32 bits) */
#endif
#ifndef _ULONG
#define _ULONG	unsigned long	/* unsigned long (at least 32 bits) */
#endif

#ifndef _FLOAT
#define _FLOAT	float			/* Single precision float	   */
#endif
#ifndef _DOUBLE
#define _DOUBLE double			/* Double precision float	   */
#endif

#ifndef VOLATILE
#define VOLATILE volatile
#endif

#ifndef INLINE
#  ifdef __GNUC__
#	 define INLINE __inline__
#  endif
#endif
#ifndef INLINE
#  define INLINE /**/
#endif


#ifndef RSC_SUBMENUS
#  define RSC_SUBMENUS 1
#endif

#ifndef ALL_FILE_MASK
#  define ALL_FILE_MASK "*"
#endif

#ifndef ANONYMOUS_STRUCT_DUMMY
#  if defined(HAVE_ANONYMOUS_STRUCTS)
#    define ANONYMOUS_STRUCT_DUMMY(x)
#  else
#    define ANONYMOUS_STRUCT_DUMMY(x) struct x { int dummy; };
#  endif
#endif

/*****************************************************************************/
/* MISCELLANEOUS DEFINITIONS												 */
/*****************************************************************************/

#define OffsetOf(type,ident)	 ((_ULONG)&(((type *)0)->ident))
#define SizeOf(type,ident)		 (_UWORD)sizeof(((type *)0)->ident)
#define NelemOf(type)			 (sizeof(type)/sizeof(type[0]))

#undef FALSE
#undef TRUE
#define FALSE	0				/* Function FALSE value        */
#define TRUE	1				/* Function TRUE  value        */

#ifndef NULL
#define NULL	( ( void * ) 0L )		/* Null pointer value			  */
#endif

#ifndef UNUSED
#  define UNUSED(x) if(x!=0){}						 /* indicate unused variable */
#endif

#ifndef __GNUC__
#  define __extension__
#  define __attribute__(x)
#endif

#ifndef NO_CONST
#  ifdef __GNUC__
#	 define NO_CONST(p) __extension__({ union { const void *cs; void *s; } x; x.cs = p; x.s; })
#  else
#	 define NO_CONST(p) ((void *)(p))
#  endif
#endif

/* (void *)0 als Funktionspointer, hier ohne cast,
   da die meisten Compiler dann Warnungen generieren */
#define FUNK_NULL 0L

#ifndef EOF
#define EOF     (-1)			/* EOF value				   */
#endif

#ifdef c_plusplus
#  ifndef __cplusplus
#    define __cplusplus
#  endif
#endif

#if __GNUC_PREREQ(2, 0)
#  define __FORMAT_ATTRIBUTE__(x) __attribute__ (x)
#endif
#if __GNUC_PREREQ(2, 7)
#  define PACKED __attribute__((packed))
#endif
#ifndef __FORMAT_ATTRIBUTE__
#  define __FORMAT_ATTRIBUTE__(x) /**/
#endif

#ifndef __attribute_noinline__
# if __GNUC_PREREQ (3,1)
#  define __attribute_used__ __attribute__ ((__used__))
#  define __attribute_noinline__ __attribute__ ((__noinline__))
# else
#  define __attribute_used__ __attribute__ ((__unused__))
#  define __attribute_noinline__ /* Ignore */
# endif
#endif

#ifndef PACKED
#  define PACKED /**/
#endif

#ifndef RETSIGTYPE
#  define RETSIGTYPE _VOID
#endif
#ifndef SIGNAL_RETURN
#  define SIGNAL_RETURN
#endif

#ifdef __cplusplus
#  define EXTERN_C_BEG extern "C" {
#  define EXTERN_C_END }
#  define EXTERN_C extern "C"
#else
#  define EXTERN_C_BEG
#  define EXTERN_C_END
#  define EXTERN_C extern
#endif

/* temporary, for sources that are used in other projects, too */
#define IN_ORCS


#endif /* __PORTAB_H__ */
