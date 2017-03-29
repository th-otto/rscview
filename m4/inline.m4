# AC_C_INLINE
# -----------
# Do nothing if the compiler accepts the inline keyword.
# Otherwise define inline to __inline__ or __inline if one of those work,
# otherwise define inline to be empty.
#
# HP C version B.11.11.04 doesn't allow a typedef as the return value for an
# inline function, only builtin types.
#
AN_IDENTIFIER([inline], [AC_C_INLINE])
AC_DEFUN([AC_C_INLINE],
[AC_CACHE_CHECK([for inline], ac_cv_c_inline,
[ac_cv_c_inline=no
for ac_kw in __inline__ __inline inline; do
  AC_COMPILE_IFELSE([AC_LANG_SOURCE(
[#ifndef __cplusplus
typedef int foo_t;
static $ac_kw foo_t static_foo () {return 0; }
$ac_kw foo_t foo () {return 0; }
#endif
])],
		    [ac_cv_c_inline=$ac_kw])
  test "$ac_cv_c_inline" != no && break
done
])
AH_VERBATIM([__inline__],
[/* Define to `__inline or `inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#undef __inline__])
case $ac_cv_c_inline in
  __inline__ | yes) ;;
  *)
    case $ac_cv_c_inline in
      no) ac_val=;;
      *) ac_val=$ac_cv_c_inline;;
    esac
    cat >>confdefs.h <<_ACEOF
#ifndef __cplusplus
#define __inline__ $ac_val
#endif
_ACEOF
    ;;
esac
])# AC_C_INLINE



