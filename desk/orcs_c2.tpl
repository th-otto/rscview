/*
 * Template file for C-sourcecode-output
 * (new format with initialized structures)
 * 
 * ORCS version 2.14 08.02.2015
 * 
 * %N will be replaced by the RSC filename (without path)
 * %n will be replaced by the RSC filename
 *    (in lowercase and without .rsc extension)
 * %R will be replaced by the RSC filename
 *    (according to namerules and without .rsc extension)
 * %h will be replaced by the extension of include files
 * %d marks the place where the actual data is written
 * %D will output the definitons of the object names
 * 
 * %s: Number of all strings
 * %S: Number of free strings
 * %U: Number of USERDEFs
 * %i: Number of all images
 * %B: Number of BITBLKs
 * %F: Number of free images
 * %I: Number of ICONBLKs
 * %C: Number of CICONBLKs
 * %c: Number of CICONs
 * %t: Number of TEDINFOs
 * %O: Number of OBJECTs
 * %T: Number of TREEs
 * %%: the '%'-character
 *
 * Macros and types used:
 *
 * _WORD or WORD:
 *    a 16 bit integer type
 * _LONG or LONG:
 *    a 32 bit integer type
 * _UBYTE:
 *    a 8 bit type
 *
 * SHORT:
 *    the base type the bi_pdata, ib_pdata etc fields
 *    point to. Depending on header used, this might
 *    be different from WORD declared above
 *    (sometimes int, sometimes short, sometimes even char)
 * CP:
 *    used to cast an address to a SHORT *
 * CW:
 *    used to cast an address to a WORD *
 *
 * C_UNION(x): to declare the OB_SPEC field of an OBJECT.
 *    Depending on headers used, this should be either
 *    (LONG)(x) (when the ob_spec is declared as plain long) or
 *    { x } (when ob_spec is a union).
 *
 * The types are usually already be declared in portab.h.
 * The macros are not essential, but without them
 * you might get lots of warnings.
 */

%BEGIN%
/*
 * %nc.c
 *
 * Generated from %n.rsc and %n.rso by ORCS %V
 *
 * Copyright 2013-2016 The EmuTOS development team
 *
 * This software is licenced under the GNU General Public License.
 * Please see LICENSE.TXT for further information.
 */

#include "config.h"
#include "string.h"
#include "portab.h"
#include "obdefs.h"
#include "../desk/deskmain.h"
#include "gemrslib.h"
#include "%nc.h"
#include "nls.h"

#ifndef OS_NORMAL
#  define OS_NORMAL 0x0000
#endif
#ifndef OS_SELECTED
#  define OS_SELECTED 0x0001
#endif
#ifndef OS_CROSSED
#  define OS_CROSSED 0x0002
#endif
#ifndef OS_CHECKED
#  define OS_CHECKED 0x0004
#endif
#ifndef OS_DISABLED
#  define OS_DISABLED 0x0008
#endif
#ifndef OS_OUTLINED
#  define OS_OUTLINED 0x0010
#endif
#ifndef OS_SHADOWED
#  define OS_SHADOWED 0x0020
#endif
#ifndef OS_WHITEBAK
#  define OS_WHITEBAK 0x0040
#endif
#ifndef OS_DRAW3D
#  define OS_DRAW3D 0x0080
#endif

#ifndef OF_NONE
#  define OF_NONE 0x0000
#endif
#ifndef OF_SELECTABLE
#  define OF_SELECTABLE 0x0001
#endif
#ifndef OF_DEFAULT
#  define OF_DEFAULT 0x0002
#endif
#ifndef OF_EXIT
#  define OF_EXIT 0x0004
#endif
#ifndef OF_EDITABLE
#  define OF_EDITABLE 0x0008
#endif
#ifndef OF_RBUTTON
#  define OF_RBUTTON 0x0010
#endif
#ifndef OF_LASTOB
#  define OF_LASTOB 0x0020
#endif
#ifndef OF_TOUCHEXIT
#  define OF_TOUCHEXIT 0x0040
#endif
#ifndef OF_HIDETREE
#  define OF_HIDETREE 0x0080
#endif
#ifndef OF_INDIRECT
#  define OF_INDIRECT 0x0100
#endif
#ifndef OF_FL3DIND
#  define OF_FL3DIND 0x0200
#endif
#ifndef OF_FL3DBAK
#  define OF_FL3DBAK 0x0400
#endif
#ifndef OF_FL3DACT
#  define OF_FL3DACT 0x0600
#endif
#ifndef OF_MOVEABLE
#  define OF_MOVEABLE 0x0800
#endif
#ifndef OF_POPUP
#  define OF_POPUP 0x1000
#endif

#ifndef R_CICONBLK
#  define R_CICONBLK 17
#endif
#ifndef R_CICON
#  define R_CICON 18
#endif

#ifndef G_SWBUTTON
#  define G_SWBUTTON 34
#endif
#ifndef G_POPUP
#  define G_POPUP 35
#endif
#ifndef G_EDIT
#  define G_EDIT 37
#endif
#ifndef G_SHORTCUT
#  define G_SHORTCUT 38
#endif
#ifndef G_SLIST
#  define G_SLIST 39
#endif
#ifndef G_EXTBOX
#  define G_EXTBOX 40
#endif
#ifndef G_OBLINK
#  define G_OBLINK 41
#endif

#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif

#ifndef _UBYTE
#  define _UBYTE char
#endif

#ifndef _LONG
#  ifdef LONG
#    define _LONG LONG
#  else
#    define _LONG long
#  endif
#endif

#ifndef C_UNION
#  define C_UNION(x) (_LONG)(x)
#endif

#ifndef SHORT
#  define SHORT short
#endif

#ifndef CP
#  define CP (SHORT *)
#endif

#ifndef CW
#  define CW (_WORD *)
#endif


#include "%nc.%h"

#ifndef TARGET_192
#define RS_NOBS %Oc
#define RS_NTREE %Tc
#define RS_NTED %tc
#define RS_NIB %Ic
#define RS_NBB %Bc
#else
#define RS_NOBS %O
#define RS_NTREE %T
#define RS_NTED %t
#define RS_NIB %I
#define RS_NBB %B
#endif
#define NUM_TI   RS_NTED
#define NUM_OBS  RS_NOBS
#define NUM_BB   RS_NBB
#define NUM_TREE RS_NTREE

%d


TEDINFO desk_rs_tedinfo[RS_NTED];
OBJECT desk_rs_obj[RS_NOBS];

void %n_rs_init(void)
{
#if RS_NOBS != 0
    /* Copy data from ROM to RAM: */
    memcpy(desk_rs_obj, rs_object_rom, RS_NOBS*sizeof(OBJECT));
    memcpy(desk_rs_tedinfo, rs_tedinfo_rom, RS_NTED*sizeof(TEDINFO));
#endid
}
