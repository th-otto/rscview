AC_PREREQ(2.59)
m4_define(RSCVIEW_MAJOR_VERSION, 1)
m4_define(RSCVIEW_MINOR_VERSION, 03)
m4_define(RELEASE_YEAR, 2017)
m4_define(RSCVIEW_DATE, 25.05.RELEASE_YEAR)
m4_define(RSCVIEW_VERSION, RSCVIEW_MAJOR_VERSION.RSCVIEW_MINOR_VERSION)

AC_INIT(rscview, RSCVIEW_VERSION, [http://www.tho-otto.de/bugs/],,[http://www.tho-otto.de/])
AC_CONFIG_SRCDIR(src/rscview.c)

AUTHOR="Thorsten Otto"

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([support])

AC_CANONICAL_HOST

AC_CONFIG_HEADER(config.h)

AH_TEMPLATE([[RSCVIEW_VERSION]], [Define release version.])
AC_DEFINE_UNQUOTED([[RSCVIEW_VERSION]], "[RSCVIEW_VERSION]")
AH_TEMPLATE([[RELEASE_YEAR]], [Define release year.])
AC_DEFINE_UNQUOTED([[RELEASE_YEAR]], "[RELEASE_YEAR]")
AH_TEMPLATE([[RSCVIEW_DATE]], [Define release date.])
AC_DEFINE_UNQUOTED([[RSCVIEW_DATE]], "[RSCVIEW_DATE]")

AM_INIT_AUTOMAKE

AC_DEFUN([_AX_COMPILE_IFELSE],
[AC_REQUIRE_SHELL_FN([ac_fn_]_AC_LANG_ABBREV[_try_compile],
  [AS_FUNCTION_DESCRIBE([ac_fn_]_AC_LANG_ABBREV[_try_compile], [LINENO],
    [Try to compile conftest.$ac_ext, and return whether this succeeded.])],
  [$0_BODY])]dnl
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])]dnl
[AS_IF([ac_fn_[]_AC_LANG_ABBREV[]_try_compile "$LINENO"], [$2], [$3])
rm -f core conftest.$ac_objext[]m4_ifval([$1], [ conftest.$ac_ext])[]dnl
])# _AX_COMPILE_IFELSE

AC_DEFUN([AX_COMPILE_IFELSE],
[AC_LANG_COMPILER_REQUIRE()dnl
_AX_COMPILE_IFELSE($@)])

AU_DEFUN([AX_TRY_COMPILE],
[AX_COMPILE_IFELSE([AC_LANG_PROGRAM([[$1]], [[$2]])], [$3], [$4])])

m4_define([AC_ADD_CFLAG], [
for flag in $2; do
case " [$]$1 " in
"* $flag *") ;;
*) AC_MSG_CHECKING([whether the compiler understands] $flag)
   saved_$1="[$]$1"
   $1="[$]$1 $flag"
   AX_TRY_COMPILE(, , result=yes, result=no)
   if grep "unrecognized command line option" conftest.err >/dev/null 2>&1; then
     result=no
   fi
   AC_MSG_RESULT([$result])
   if test $result = yes; then
     $1="$saved_$1 $flag"
   else
     $1="$saved_$1"
   fi
   rm -f core conftest.err
   ;;
esac
done
])

###############################################################################
#
#       Function to figure out how to run the compiler.
#
###############################################################################

AC_DEFUN([AC_PROG_CC_ANSI],
 [AC_REQUIRE([AC_PROG_CC])

  if test -z "$GCC"; then
    # not using GCC
    AC_MSG_CHECKING(how to request ANSI compilation)
    case "$host" in
      *-hpux* )
        AC_MSG_RESULT([HPUX: adding -Ae])
        AC_ADD_CFLAG(CC, -Ae)
      ;;
      *-aix* )
        AC_MSG_RESULT([AIX: adding -qlanglvl=ansi -qhalt=e])
        AC_ADD_CFLAG(CC, -qlanglvl=ansi -qhalt=e)
      ;;
      *-dec-* )
        AC_MSG_RESULT([DEC: adding -std1 -ieee])
        AC_ADD_CFLAG(CC, -std1)
      ;;
      *)
        AC_MSG_RESULT([no idea])
      ;;
    esac
  else
    # using GCC
    case "$host" in
      *-solaris*)
        AC_MSG_RESULT([Solaris: adding -D__EXTENSIONS__])
        CC="$CC -D__EXTENSIONS__"
      ;;
    *-*-mingw*|*-*-msys*|*-*-MINGW*|*-*-MSYS*)
      WIN32_CONSOLE_FLAGS=-mconsole
      WIN32_WINDOWS_FLAGS=-mwindows
      ;;
    esac
  fi

  AC_SUBST(WIN32_CONSOLE_FLAGS)
  AC_SUBST(WIN32_WINDOWS_FLAGS)

  AC_MSG_CHECKING([whether the compiler works on ANSI C])
  AC_TRY_RUN([ main(int ac, char **av) { return 0; } ],
     AC_MSG_RESULT([yes]),
     AC_MSG_RESULT([no])
     AC_MSG_ERROR(Couldn't build even a trivial ANSI C program: check CC.),
     AC_MSG_RESULT([assuming yes]))

  AC_SUBST(WARN_NO_UNUSED)
])


AC_DEFUN([AC_PROG_CC_WARNINGS], [
AC_REQUIRE([AC_PROG_ICC])
if test -n "$GCC"; then
    AC_MSG_RESULT([Turning on gcc compiler warnings.])
    case $host in
      i686*) math_opts="-msse -mfpmath=sse" ;;
    esac
    if test "$enable_warnings" = "yes"; then
      AC_ADD_CFLAG(CC, -pedantic -Wno-overlength-strings -Wall -W -Wunused -Wundef -Wstrict-prototypes -Wmissing-prototypes -Wc++-compat -Wwrite-strings -Wcast-qual -Wno-pointer-sign -Wno-variadic-macros -Wno-misleading-indentation -Wno-nonnull-compare)
    fi
    AC_ADD_CFLAG(CC,  $math_opts -ftrack-macro-expansion=0 -funsigned-char)
    if test "$ICC" != yes; then
     if test "$enable_warnings" = "yes"; then
       AC_ADD_CFLAG(CC, -Wnested-externs)
     fi
     AC_ADD_CFLAG(CC, -ffast-math)
    else
     AC_ADD_CFLAG(CC, "-fp-model fast")
    fi
    # newer g++ versions complain about -Wstrict-prototypes being invalid for C++
    # g++ 4.6.x complains about 'long long' even when declared with __extension__ when using -pedantic
    # AC_ADD_CFLAG(CXX, -pedantic)
    AC_ADD_CFLAG(CXX, -Wall -W -Wunused -Wundef $math_opts)
    if test "$ICC" != yes; then
      AC_ADD_CFLAG(CXX, -ffast-math)
    else
      AC_ADD_CFLAG(CXX, "-fp-model fast")
    fi
    OBJC="$CC -xobjective-c"
    #CC="$CC -Waggregate-return"
    # supposedly gcc 3.4 will have "-Wdeclaration-after-statement"
    # and then perhaps we can do without -pedantic?
    WARN_NO_UNUSED="-Wno-unused-parameter"
  else
    case "$host" in
      *-irix5* |*-irix6.[0-3]* )
        AC_MSG_RESULT(Turning on SGI compiler warnings.)
        if test "$enable_warnings" = "yes"; then
          CC="$CC -fullwarn -use_readonly_const -rdata_shared -g3"
        fi
      ;;
#     *-dec-osf* )
#       if test -z "$GCC"; then
#         AC_MSG_RESULT(Turning on DEC C compiler warnings.)
#         CC="$CC -migrate -w0 -verbose -warnprotos"
#       fi
#     ;;
    esac
    OBJC="$CC"
  fi
  # passing "-xobjective-c" when linking makes gcc think it should compile the object files...
  OBJCLD='$(CCLD)'
])


AC_DEFUN([AC_GCC_ACCEPTS_STD],
 [if test -n "$GCC"; then
   AC_CACHE_CHECK([whether gcc accepts -std],
     ac_cv_gcc_accepts_std,
    [if ( ( gcc -E -std=c89 - </dev/null >/dev/null ) 2>&1 | \
          grep unrecognized >/dev/null ); then
       ac_cv_gcc_accepts_std=no
     else
       ac_cv_gcc_accepts_std=yes
     fi])
   ac_gcc_accepts_std="$ac_cv_gcc_accepts_std"
  fi
])

AC_DEFUN([AC_NO_CPLUSPLUS_COMMENTS_IN_C_CODE],
 [if test -n "$GCC"; then
   AC_GCC_ACCEPTS_STD
   AC_MSG_RESULT([Disabling C++ comments in ANSI C code.])
   #
   # The reason that // comments are banned is that gcc is
   # basically the only compiler in the world that supports them in C code.
   # All other vendors support them only in their C++ compilers, not in their
   # ANSI C compilers.  This means that it's a portability problem: every time
   # these comments have snuck into the source code, I've gotten
   # complaints about it the next day.  So we turn off support for them in gcc
   # as well to prevent them from accidentially slipping in.
   #
   if test "$ac_gcc_accepts_std" = yes ; then
     #
     # -std=c89 defines __STRICT_ANSI__, which we don't want.
     # (That appears to be the only additional preprocessor symbol
     # it defines, in addition to the syntax changes it makes.)
     #
     # -std=gnu89 is no good, because // comments were a GNU extension
     # before they were in the ANSI C 99 spec...  (gcc 2.96 permits //
     # with -std=gnu89 but not with -std=c89.)
     #
     # -std=c89 is no good, because the float versions of the math
     # functions are not available
     #CC="$CC -std=c89"
     CC="$CC -std=gnu89"
   else
     # The old way:
     CC="$CC -Wp,-lang-c89"
   fi
  fi
])


# Like AC_CHECK_FUNC, but uses additional header files for compiling
# (needed for the math functions, which are only correctly identified
# when including math.h)
# AC_CHECK_X_FUNCS(INCLUDES, FUNCTION..., [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
AC_DEFUN([AC_CHECK_X_FUNC],
[AS_VAR_PUSHDEF([ac_var], [ac_cv_func_$2])dnl
AC_CACHE_CHECK([for $2], ac_var,
[AC_LINK_IFELSE([AC_LANG_PROGRAM([[$1]], [[$2]])],
		[AS_VAR_SET(ac_var, yes)],
		[AS_VAR_SET(ac_var, no)])])
AS_IF([test AS_VAR_GET(ac_var) = yes], [$3], [$4])
AS_VAR_POPDEF([ac_var])dnl
])# AC_CHECK_X_FUNC


# Like AC_CHECK_X_FUNCS, but uses additional header files for compiling
# AC_CHECK_X_FUNCS(INCLUDES, FUNCTION..., [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
m4_define([_AH_CHECK_X_FUNCS],
[m4_foreach_w([AC_Func], [$1],
   [AH_TEMPLATE(AS_TR_CPP([HAVE_]m4_defn([AC_Func])),
      [Define to 1 if you have the ']m4_defn([AC_Func])[' function.])])])
AC_DEFUN([AC_CHECK_X_FUNCS],
[_AH_CHECK_X_FUNCS([$2])dnl
for ac_func in $2
do
AC_CHECK_X_FUNC([$1], $ac_func,
	      [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_$ac_func])) $3],
	      [$4], [$5])dnl
done
])


###############################################################################
#
#       Function to check whether gettimeofday() exists, and how to call it.
#	This may define HAVE_GETTIMEOFDAY and GETTIMEOFDAY_TWO_ARGS.
#
###############################################################################

AH_TEMPLATE([HAVE_GETTIMEOFDAY],
	    [Define this if you have the gettimeofday function.])

AH_TEMPLATE([GETTIMEOFDAY_TWO_ARGS],
	    [Define this if gettimeofday() takes two arguments.])

AC_DEFUN([AC_GETTIMEOFDAY_ARGS],
 [AC_MSG_CHECKING(how to call gettimeofday)
  AC_CACHE_VAL(ac_cv_gettimeofday_args,
   [AC_TRY_COMPILE([#include <stdlib.h>
                    #include <sys/time.h>],
                   [struct timeval tv; struct timezone tzp;
                    gettimeofday(&tv, &tzp);],
                   [ac_gettimeofday_args=2],
                   [AC_TRY_COMPILE([#include <stdlib.h>
                                    #include <sys/time.h>],
                                   [struct timeval tv; gettimeofday(&tv);],
                                   [ac_gettimeofday_args=1],
                                   [ac_gettimeofday_args=0])])
    ac_cv_gettimeofday_args=$ac_gettimeofday_args])
  ac_gettimeofday_args=$ac_cv_gettimeofday_args
  if test "$ac_gettimeofday_args" = 1 ; then
    AC_DEFINE(HAVE_GETTIMEOFDAY)
    AC_MSG_RESULT([one argument])
  elif test "$ac_gettimeofday_args" = 2 ; then
    AC_DEFINE(HAVE_GETTIMEOFDAY)
    AC_DEFINE(GETTIMEOFDAY_TWO_ARGS)
    AC_MSG_RESULT([two arguments])
  else
    AC_MSG_RESULT([unknown])
  fi
])


###############################################################################
###############################################################################
#
#       End of function definitions.  Now start actually executing stuff.
#
###############################################################################
###############################################################################

# Support silent build rules, requires at least automake-1.11. Disable
# by either passing --disable-silent-rules to configure or passing V=1
# to make
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AH_TOP([
#ifndef __CONFIG_H__
#define __CONFIG_H__ 1

#include "linux/libcwrap.h"

])
dnl append that also to confdefs.h
echo "#include \"${srcdir}/include/linux/libcwrap.h\"" >> confdefs.h

# random compiler setup
AC_PROG_CC_ANSI
AC_PROG_CXX
AM_PROG_AS
AC_CHECK_TOOL(STRIP, strip, :)

AC_PROG_CC_WARNINGS
AC_NO_CPLUSPLUS_COMMENTS_IN_C_CODE
AC_PROG_CPP
AM_PROG_CC_C_O
AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE
AC_EXEEXT
AC_PROG_YACC

AH_TEMPLATE([CONST], [Define to empty if the keyword const does not work.])
if test $ac_cv_c_const = no; then
  AC_DEFINE(CONST, )
else
  AC_DEFINE(CONST, const)
fi

ac_macosx=no
case $host in
  m68*-*-mint*)
     test "$CC" = "" && CC=gcc
     test "$CXX" = "" && CXX="g++"
     TARGET_OS=tos
     X_LIBS=-lgem
     ;;
  *-*-mingw*|*-*-msys*|*-*-MINGW*|*-*-MSYS*)
     test "$CC" = "" && CC=gcc
     test "$CXX" = "" && CXX="g++"
     for arg in -mms-bitfields -mno-cygwin -mwin32 -mthreads; do
        case $CC in
        *$arg*) ;;
        *) if $CC $arg -E - < /dev/null >/dev/null 2>&1; then
             echo adding $arg to $CC
             CC="$CC $arg"
             CXX="$CXX $arg"
           fi
        esac
     done
     LTCC="$CC"
     TARGET_OS=win32
     # can be built for X on windows too (using cygwin)
     # but native built is default
     X_EXTRA_LIBS=-lws2_32
     X_LIBS="-lole32 -ladvapi32 -lrpcrt4 -lcomdlg32 -luser32 -lgdi32 -lwinmm"
     CPPFLAGS="$CPPFLAGS -D_WIN32"
     case $host in
     x86_64-*-*) ARCH=x64 ;;
     *) ARCH=x86 ;;
     esac
     ;;
  *-apple-darwin* )
     ac_macosx=yes
     TARGET_OS=unix
       with_quartz=yes
       X_LIBS="-framework CoreFoundation -framework Cocoa"
     ;;
  *-*-os2)
     test "$CC" = "" && CC=gcc
     test "$CXX" = "" && CXX="g++"
     TARGET_OS=os2
     X_EXTRA_LIBS=
     X_LIBS=
     ;;
  *) TARGET_OS=unix
     ;;
esac

AM_CONDITIONAL(OS_TOS, test "$TARGET_OS" = "tos")
AM_CONDITIONAL(OS_WIN32, test "$TARGET_OS" = "win32")
AM_CONDITIONAL(OS_UNIX, test "$TARGET_OS" = "unix")
AM_CONDITIONAL(OS_OS2, test "$TARGET_OS" = "os2")

if test "`echo -e 2>/dev/null`" != ""; then
  ECHO_E="echo"
else
  ECHO_E="echo -e"
fi
AC_SUBST(ECHO_E)

dnl ***************************************************************************
dnl AC_PROG_TAR
dnl ***************************************************************************
AC_DEFUN([AC_PROG_TAR],
[AC_CHECK_PROGS(TAR, gtar tar, )])

dnl
dnl ***************************************************************************
dnl AC_PROG_UDO
dnl ***************************************************************************
AC_DEFUN([AC_PROG_UDO],
[AC_CHECK_PROGS(UDO, udo udo7 udo6, )])

###############################################################################

AC_DEFUN([AC_MAN_GZIP], [
MAN_GZIP=cat
MAN_GZ_EXT=
AC_PATH_PROGS(GZIP, [gzip],,)
if test -n "$GZIP"; then
	MAN_GZ_EXT=`rpm --eval '%{ext_man}' 2>/dev/null`
	if test "$MAN_GZ_EXT" = .gz; then
		MAN_GZIP='$(GZIP)'
	else
		MAN_GZ_EXT=
	fi
fi
AC_SUBST(MAN_GZIP)
AC_SUBST(MAN_GZ_EXT)
AC_SUBST(GZIP)
])

###############################################################################

# stuff for Makefiles
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_MAKE_SET
AC_PROG_TAR
AC_PROG_UDO
AC_PATH_PROGS(UNZIPSFX, unzipsfx, )
AC_MAN_GZIP


if test "$cross_compiling" = no; then
  # do not consider this as a native build only because we can exec build programs;
  # it might just be a binfmt handler that makes this possible
  case $build in *-*-linux*)
     case $host in
     *-*-linux*) ;;
     *) cross_compiling=yes ;;
     esac
  esac
fi

AC_CHECK_TOOL(RC, windres, :)

if test "$RC" = "" -a "$TARGET_OS" = "win32"; then
  AC_MSG_ERROR([windres tool for compiling resource files not found])
fi

if test "$RC" = "" -a "$TARGET_OS" = "os2"; then
  RC=rc
fi

###############################################################################

# random libc stuff
AC_HEADER_STDC
AC_CHECK_HEADERS([argz.h limits.h locale.h nl_types.h malloc.h string.h strings.h unistd.h process.h sys/param.h endian.h \
        unistd.h string.h memory.h sys/types.h limits.h fcntl.h sys/time.h sys/timeb.h utime.h \
	sys/param.h sys/statfs.h sys/fstyp.h mnttab.h mntent.h sys/statvfs.h sys/vfs.h byteswap.h \
	sys/mount.h sys/filsys.h sys/fs_types.h sys/dustat.h sys/times.h sys/select.h stdarg.h sys/utsname.h \
        math.h crypt.h sys/select.h io.h fcntl.h pwd.h dlfcn.h dos.h direct.h])
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T
AC_TYPE_SOCKLEN_T
AC_C_CHAR_UNSIGNED
AC_STRUCT_TM
AC_TYPE_SIGNAL
AC_CHECK_STIME_ARG
AC_VA_LIST_NULL
AH_TEMPLATE([SIGNAL_RETURN], [define to a statement to return from signal handlers])
if test "$ac_cv_type_signal" = "void"; then
   AC_DEFINE(SIGNAL_RETURN, [])
else
   AC_DEFINE(SIGNAL_RETURN, [return 0])
fi

AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT
AC_FUNC_CLOSEDIR_VOID
AC_GETTIMEOFDAY_ARGS
AC_SYS_LARGEFILE
AC_HEADER_STAT
AC_CHECK_FUNCS([select fcntl uname gethostname nice setpriority getcwd getwd putenv sbrk sync \
     getcwd munmap putenv setenv setlocale strchr strcasecmp strdup __argz_count __argz_stringify __argz_next \
     sigaction syslog realpath setrlimit lstat mktemp mkstemp mkstemps \
     snprintf vasprintf vsnprintf stpcpy mempcpy strtoul wcsnlen mbrtowc wcrtomb strsignal getrlimit setrlimit \
     select usleep nap getcwd getwd strstr gettimeofday strerror ftime getmntinfo vfork uname gethostname XExtendedMaxRequestSize])

AC_CHECK_X_FUNCS([#undef __STRICT_ANSI__
#define _GNU_SOURCE
#define _XOPEN_SOURCE
#include <math.h>
], sinf cosf tanf asinf acosf atanf atan2f ceilf floorf fabsf sqrtf fmodf powf expf sincosf roundf lrintf lrint rintf rint finite _finite finitef _finitef isnan _isnan)
AC_CHECK_X_FUNCS([#include <time.h>], localtime_r gmtime_r ctime_r mktime tzset)
AC_CHECK_X_FUNCS([#include <stdlib.h>], random_r)

AC_CHECK_LIB(c_s, open)
AC_CHECK_LIB(m, pow)

###############################################################################

AC_DEFUN([AC_TRANSLATE_DEFINE], [
    case $2 in
       *yes)
        AC_DEFINE([$1], 1, [$3])
        ;;
    esac
])

###############################################################################
#
#       various --enable-xxx options that are tested later,
#       moved here to get the help output right
#
###############################################################################

AH_TEMPLATE([SPEC_DEBUG], [define to 1 to get debugging code])
AC_ARG_ENABLE(debug,
[AC_HELP_STRING(--enable-debug, [Enable debugging [default=disabled]])],
enable_debug="$enableval", enable_debug=no)
if test "$enable_debug" = "yes"; then
  LOCAL_CFLAGS="-g"
  AC_DEFINE(SPEC_DEBUG)
else
  LOCAL_CFLAGS=""
fi
AC_SUBST(LOCAL_CFLAGS)
AC_SUBST(LOCAL_LDFLAGS)

if test "x${GCC}" = "xyes"; then
  if test "$enable_debug" = "yes"; then
    CFLAGS="$CFLAGS -fno-omit-frame-pointer"
  else
    CFLAGS="$CFLAGS -fomit-frame-pointer"
  fi
fi

AC_ARG_ENABLE(warnings,
[AC_HELP_STRING(--enable-warnings, [enable compiler warnings])],
[case $enableval in
yes | no) ;;
*)   AC_MSG_ERROR([must be yes or no: --enable-warnings=$enableval]) ;;
esac
], enable_warnings=no)

AC_ARG_ENABLE(fatal-warnings,
[AC_HELP_STRING(--enable-fatal-warnings, [Turn compiler warnings into errors [default=disabled]])],
enable_fatal_warnings="$enableval", enable_fatal_warnings=no)

AC_CONFIG_COMMANDS_PRE([
if test "$GCC" = "yes" -a "$enable_fatal_warnings" = "yes"; then
	CFLAGS="$CFLAGS -Werror"
	CXXFLAGS="$CXXFLAGS -Werror"
fi
])

###############################################################################
#
#       Curl support
#
###############################################################################

AM_PATH_CURL
if test "$ac_cv_header_curl_curl_h" != yes -a "$ac_cv_header_curl_h" != yes; then
   no_curl=yes
fi
AM_CONDITIONAL(HAVE_CURL, test "$no_curl" = "")

# compute the final value of X_LIBS
#
if test "$TARGET_OS" = win32; then
   X_LIBS="$X_LIBS $X_EXTRA_LIBS"
fi
if test "$TARGET_OS" = tos; then
   X_LIBS="$X_LIBS $X_EXTRA_LIBS"
fi
AC_SUBST(X_EXTRA_LIBS)

BUILD_PREFIX="$build_alias"
if test "$BUILD_PREFIX" = ""; then
  BUILD_PREFIX="$ac_build_alias"
fi
BUILD_PREFIX="${BUILD_PREFIX}-"
AC_SUBST([BUILD_PREFIX])

AH_BOTTOM([
#endif /* __CONFIG_H__ */
])

###############################################################################
#
#       spit out Makefiles
#
###############################################################################

AC_CONFIG_FILES([
Makefile
build/linux/Makefile
data/Makefile
gemlib/Makefile
po/Makefile
aes/Makefile
vdi/Makefile
util/Makefile
src/Makefile
])

[RSCVIEW_MAJOR_VERSION]=RSCVIEW_MAJOR_VERSION
[RSCVIEW_MINOR_VERSION]=RSCVIEW_MINOR_VERSION
[RSCVIEW_VERSION]=RSCVIEW_VERSION
[RSCVIEW_DATE]=RSCVIEW_DATE
[RELEASE_YEAR]=RELEASE_YEAR
AC_SUBST([RSCVIEW_MAJOR_VERSION])
AC_SUBST([RSCVIEW_MINOR_VERSION])
AC_SUBST([RSCVIEW_VERSION])
AC_SUBST([RSCVIEW_DATE])
AC_SUBST([RELEASE_YEAR])
AC_SUBST([AUTHOR])

AC_SUBST([ac_aux_dir])

AM_CONDITIONAL([CROSS_COMPILING], test "$cross_compiling" = yes)
CROSS_COMPILING="$cross_compiling"
AC_SUBST([CROSS_COMPILING])

if test "$cross_compiling" = yes; then
	BUILD_CC=gcc
	case $build in
	*-*-mingw*|*-*-msys*|*-*-MINGW*|*-*-MSYS*|*-*-cygwin*|*-*-CYGWIN*)
	  BUILD_EXEEXT=.exe
	  ;;
        *) 
	  BUILD_EXEEXT=
	  ;;
        esac
else
	BUILD_CC="$CC"
	BUILD_EXEEXT="$EXEEXT"
fi
AC_SUBST(BUILD_CC)
AC_SUBST(BUILD_EXEEXT)


# do not let config.status generate the dependency files,
# it does not work because we use subdir-objects and
# Makefile variables in the directory names, which
# config.status does not know yet
# AC_DEFUN([_AM_OUTPUT_DEPENDENCY_COMMANDS], [:])

AC_OUTPUT
