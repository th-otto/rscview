# AC_C_VOLATILE
# -------------
# Note that, unlike const, #defining volatile to be the empty string can
# actually turn a correct program into an incorrect one, since removing
# uses of volatile actually grants the compiler permission to perform
# optimizations that could break the user's code.  So, do not #define
# volatile away unless it is really necessary to allow the user's code
# to compile cleanly.  Benign compiler failures should be tolerated.
AC_DEFUN([AC_C_VOLATILE],
[AC_CACHE_CHECK([for working volatile], ac_cv_c_volatile,
[ac_cv_volatile=no
for ac_kw in __volatile__ __volatile volatile; do
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [
$ac_kw int x;
int * $ac_kw y = (int *) 0;
return !x && !y;])],
		   [ac_cv_c_volatile=yes])
  test "$ac_cv_c_volatile" != no && break
done
  ])
if test $ac_cv_c_volatile = no; then
  AC_DEFINE(__volatile__,,
	    [Define to empty if the keyword `__volatile__' does not work,
	     or to `__volatile' or `volatile' if that's what the C compiler calls it.
	     Warning: valid code using `volatile' can become incorrect
	     without.  Disable with care.])
fi
])# AC_C_VOLATILE

