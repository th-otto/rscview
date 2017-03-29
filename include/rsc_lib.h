/*****************************************************************************
 * RSC_LIB.H
 *****************************************************************************/

#ifndef __RSC_LIB_H__
#define __RSC_LIB_H__

#ifndef __PORTAES_H__
#  include <portaes.h>
#endif

EXTERN_C_BEG


#define SYM_SHIFT 0x1
#define SYM_ALT   0x7
#define SYM_CNTL  0x5e

#define MAX_KEYSTR_LEN 9

#undef max
#define max(a, b) ((a) > (b) ? (a) : (b))

_BOOL menu_string_find_keycode(_WORD *keycode, const char *str);
_BOOL menu_string_extract_keystring(char buf[10], const char *str, _WORD *pos);

char *Obj_MakePtmpltStr(char *toStr, _WORD toSize, const OBJECT *ob);

char *Obj_ValidPtr(OBJECT *tree, _WORD obj);
_LONG  *Obj_LongPtr(OBJECT *objPtr);
_BOOL *Obj_CheckXPtr(OBJECT *objPtr);
_VOID **Obj_BufPtrPtr(OBJECT *objPtr);

_WORD get_popup_selected(OBJECT *tree, _WORD head, _WORD *valp);
_VOID set_popup_selected(OBJECT *tree, _WORD head, _WORD val);
_VOID set_popup_checked(OBJECT *tree, _WORD head, _WORD val, _BOOL checked);
_BOOL is_popup_checked(OBJECT *tree, _WORD head, _WORD val);

_VOID Obj_StrToObj(OBJECT *objPtr, const char *str);
_WORD Obj_StrMaxLen(OBJECT *ptr, _WORD idx);
_VOID Obj_StrSet(OBJECT *ptr, _WORD idx, const char *str);
_VOID Obj_StrGet(OBJECT *ptr, _WORD idx, char *buf);
char *Obj_ToStr(char *strTo, _UWORD size, const OBJECT *obj);

_WORD   Obj_LengText(const OBJECT *tree, _WORD obj);
_BOOL   Obj_StrAndMask(char *strTo, _UWORD size, const OBJECT *tree, _WORD obj);
_BOOL   Obj_StrAndAll(char *strTo, _UWORD size, const OBJECT *tree, _WORD obj);

_VOID   set_txt_thickness(OBJECT *tree, _WORD obj, _WORD thickness);
_VOID   set_txtlen(OBJECT *tree, _WORD obj, _WORD length);
_VOID   set_ptext(OBJECT *tree, _WORD obj, char *name);
_VOID   set_button(OBJECT *tree, _WORD obj, char *name);
_VOID   SetTxtDialog(OBJECT *tree, _WORD obj, char *name, _UWORD length, ...);

char *MsgString(_WORD Nr);

_VOID bit_free(BITBLK *bit);
_VOID icon_free(ICONBLK *icon);
TEDINFO *ted_dup(TEDINFO *ted, _BOOL formatted);
BITBLK *bit_dup(BITBLK *bit);
ICONBLK *icon_dup(ICONBLK *icon);
_VOID user_free(USERBLK *);
_BOOL obspec_dup(_UWORD type, OBSPEC *to, const OBSPEC *from);
_VOID obspec_free(_UWORD type, OBSPEC *obspec);
CICONBLK *cicon_dup(CICONBLK *cicon);
USERBLK *user_dup(USERBLK *blk);
CICON *cicon_plane_dup(CICON *old, _ULONG monosize);
CICON *cicon_new_plane(_ULONG monosize, _WORD planes, _BOOL selected);

_LONG bitblk_datasize(BITBLK *bit);
_LONG iconblk_masksize(ICONBLK *icon);


_VOID cicon_free(CICONBLK *cicon);
_VOID cicon_list_free(CICON *list);

EXTERN_C_END

#endif /* __RSC_LIB_H__ */
