AH_TEMPLATE([HAVE_VA_LIST_NULL], [define if va_list can be null])
AC_DEFUN([AC_VA_LIST_NULL],
 [AC_MSG_CHECKING(wether va_list can be 0)
  AC_CACHE_VAL(ac_cv_va_list_null,
   [AC_TRY_COMPILE([#include <stdarg.h>
                    #include <stdlib.h>
                    va_list args;],
                   [int x = args == 0;],
                   [ac_cv_va_list_null=yes],
                   [ac_cv_va_list_null=no])
    ])
  ac_va_list_null=$ac_cv_va_list_null
  if test "$ac_va_list_null" = yes ; then
    AC_DEFINE(HAVE_VA_LIST_NULL)
  fi
  AC_MSG_RESULT([$ac_va_list_null])
])


