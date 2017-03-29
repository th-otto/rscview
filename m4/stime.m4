dnl ***************************************************************************
dnl AC_CHECK_STIME_ARG(type, default, includes)
dnl ***************************************************************************
AH_TEMPLATE([STIME_ARG_TYPE], [Define to the argument type of the stime() function])
AC_DEFUN([AC_CHECK_STIME_ARG],
[AC_MSG_CHECKING(for stime argument type)
AC_CACHE_VAL(ac_cv_argtype_stime,
[AC_EGREP_CPP(dnl
changequote(<<<,>>>)dnl
<<<stime.*\(.*long>>>dnl
changequote([,]), [#include <sys/time.h>], ac_cv_argtype_stime=long, ac_cv_argtype_stime=time_t)])dnl
AC_MSG_RESULT($ac_cv_argtype_stime)
if test $ac_cv_argtype_stime = long; then
  AC_DEFINE(STIME_ARG_TYPE, long)
else
  AC_DEFINE(STIME_ARG_TYPE, time_t)
fi
])
