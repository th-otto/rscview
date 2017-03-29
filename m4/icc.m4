# _AC_LANG_COMPILER_ICC
# ---------------------
# Check whether the compiler for the current language is ICC.
m4_define([_AC_LANG_COMPILER_ICC],
[AC_CACHE_CHECK([whether we are using the ICC _AC_LANG compiler],
		[ac_cv_[]_AC_LANG_ABBREV[]_compiler_icc],
[_AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[#ifndef __ICC
       choke me
#endif
]])],
		   [ac_compiler_icc=yes],
		   [ac_compiler_icc=no])
ac_cv_[]_AC_LANG_ABBREV[]_compiler_icc=$ac_compiler_icc
])])# _AC_LANG_COMPILER_ICC

AC_DEFUN([AC_PROG_ICC],
[AC_REQUIRE([AC_PROG_CC])dnl
AC_LANG_PUSH(C)dnl

_AC_LANG_COMPILER_ICC
if test $ac_compiler_icc = yes; then
  ICC=yes
else
  ICC=
fi

AC_LANG_POP(C)dnl
])# AC_PROG_ICC

