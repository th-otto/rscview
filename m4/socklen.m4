dnl ***************************************************************************
dnl AC_CHECK_SYSTYPE(type, default, includes)
dnl ***************************************************************************
AC_DEFUN([AC_CHECK_SYSTYPE],
[AC_MSG_CHECKING(for $1)
AC_CACHE_VAL(ac_cv_type_$1,
[AC_EGREP_CPP(dnl
changequote(<<<,>>>)dnl
<<<$1[^a-zA-Z_0-9]>>>dnl
changequote([,]), [#include <sys/types.h>
#if STDC_HEADERS
#include <stdlib.h>
#include <stddef.h>
#endif
$3], ac_cv_type_$1=yes, ac_cv_type_$1=no)])dnl
AC_MSG_RESULT($ac_cv_type_$1)
if test $ac_cv_type_$1 = no; then
  AC_DEFINE($1, $2)
fi
])
AH_TEMPLATE([socklen_t], [Define to `int' if <sys/socket.h> doesn't define.])
AC_DEFUN([AC_TYPE_SOCKLEN_T], [AC_CHECK_SYSTYPE(socklen_t, int, [#include <sys/socket.h>])])

