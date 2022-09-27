dnl ***************************************************************************
dnl AC_FILESYS_LIST
dnl ***************************************************************************
AH_TEMPLATE([MOUNTED_FREAD], [Define if there is no specific function for reading the list of
mounted filesystems.  fread will be used to read /etc/mnttab.  (SVR2)])
AH_TEMPLATE([MOUNTED_FREAD_FSTYP], [Define if (like SVR2) there is no specific function for reading the
list of mounted filesystems, and your system has these header files:
<sys/fstyp.h> and <sys/statfs.h>.  (SVR3)])
AH_TEMPLATE([MOUNTED_GETFSSTAT], [Define if there is a function named getfsstat for reading the list
of mounted filesystems.  (DEC Alpha running OSF/1)])
AH_TEMPLATE([MOUNTED_GETMNT], [Define if there is a function named getmnt for reading the list of
mounted filesystems.  (Ultrix)])
AH_TEMPLATE([MOUNTED_GETMNTENT1], [Define if there is a function named getmntent for reading the list
of mounted filesystems, and that function takes a single argument.
(4.3BSD, SunOS, HP-UX, Dynix, Irix)])
AH_TEMPLATE([MOUNTED_GETMNTENT2], [Define if there is a function named getmntent for reading the list of
mounted filesystems, and that function takes two arguments.  (SVR4)])
AH_TEMPLATE([MOUNTED_GETMNTINFO], [Define if there is a function named getmntinfo for reading the list
of mounted filesystems.  (4.4BSD)])
AH_TEMPLATE([MOUNTED_VMOUNT], [Define if there is a function named mntctl that can be used to read
the list of mounted filesystems, and there is a system header file
that declares `struct vmount.'  (AIX)])

AC_DEFUN([AC_FILESYS_LIST], [
AC_MSG_CHECKING(how to get the list of mounted filesystems)

# If the getmntent function is available but not in the standard library,
# make sure LIBS contains -lsun (on Irix4) or -lseq (on PTX).
AC_FUNC_GETMNTENT

AC_CACHE_VAL(ac_cv_filesys,[
ac_cv_filesys=none

if test $ac_cv_func_getmntent = yes; then

  # This system has the getmntent function.
  # Determine whether it's the one-argument variant or the two-argument one.

  if test "$ac_cv_filesys" = "none"; then
    # 4.3BSD, SunOS, HP-UX, Dynix, Irix
    AC_MSG_CHECKING([for one-argument getmntent function])
    AC_CACHE_VAL(fu_cv_sys_mounted_getmntent1,
		 [test $ac_cv_header_mntent_h = yes \
		   && fu_cv_sys_mounted_getmntent1=yes \
		   || fu_cv_sys_mounted_getmntent1=no])
    AC_MSG_RESULT($fu_cv_sys_mounted_getmntent1)
    if test $fu_cv_sys_mounted_getmntent1 = yes; then
      ac_cv_filesys=getmntent1
    fi
  fi

  if test "$ac_cv_filesys" = "none"; then
    # SVR4
    AC_MSG_CHECKING([for two-argument getmntent function])
    AC_CACHE_VAL(fu_cv_sys_mounted_getmntent2,
    [AC_EGREP_HEADER(getmntent, sys/mnttab.h,
      fu_cv_sys_mounted_getmntent2=yes,
      fu_cv_sys_mounted_getmntent2=no)])
    AC_MSG_RESULT($fu_cv_sys_mounted_getmntent2)
    if test $fu_cv_sys_mounted_getmntent2 = yes; then
      ac_cv_filesys=getmntent2
    fi
  fi

fi

if test "$ac_cv_filesys" = "none"; then
  # DEC Alpha running OSF/1.
  AC_MSG_CHECKING([for getfsstat function])
  AC_CACHE_VAL(fu_cv_sys_mounted_getsstat,
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/fs_types.h>]],
  [[struct statfs *stats;
  numsys = getfsstat ((struct statfs *)0, 0L, MNT_WAIT); ]])],
    [fu_cv_sys_mounted_getsstat=yes],
    [fu_cv_sys_mounted_getsstat=no])])
  AC_MSG_RESULT($fu_cv_sys_mounted_getsstat)
  if test $fu_cv_sys_mounted_getsstat = yes; then
    ac_cv_filesys=getfsstat
  fi
fi

if test "$ac_cv_filesys" = "none"; then
  # AIX.
  AC_MSG_CHECKING([for mntctl function and struct vmount])
  AC_CACHE_VAL(fu_cv_sys_mounted_vmount,
  [AC_PREPROC_IFELSE([AC_LANG_SOURCE([[#include <fshelp.h>]])],
    [fu_cv_sys_mounted_vmount=yes],
    [fu_cv_sys_mounted_vmount=no])])
  AC_MSG_RESULT($fu_cv_sys_mounted_vmount)
  if test $fu_cv_sys_mounted_vmount = yes; then
    ac_cv_filesys=vmount
  fi
fi

if test "$ac_cv_filesys" = "none"; then
  # SVR3
  AC_MSG_CHECKING([for FIXME existence of three headers])
  AC_CACHE_VAL(fu_cv_sys_mounted_fread_fstyp,
    [AC_PREPROC_IFELSE([AC_LANG_SOURCE([[
#include <sys/statfs.h>
#include <sys/fstyp.h>
#include <mnttab.h>]])],
		[fu_cv_sys_mounted_fread_fstyp=yes],
		[fu_cv_sys_mounted_fread_fstyp=no])])
  AC_MSG_RESULT($fu_cv_sys_mounted_fread_fstyp)
  if test $fu_cv_sys_mounted_fread_fstyp = yes; then
    ac_cv_filesys=fread_fstype
  fi
fi

if test "$ac_cv_filesys" = "none"; then
  # 4.4BSD and DEC OSF/1.
  AC_MSG_CHECKING([for getmntinfo function])
  AC_CACHE_VAL(fu_cv_sys_mounted_getmntinfo,
    [
      ok=
      if test $ac_cv_func_getmntinfo = yes; then
	AC_EGREP_HEADER(f_type;, sys/mount.h,
			ok=yes)
      fi
      test -n "$ok" \
	  && fu_cv_sys_mounted_getmntinfo=yes \
	  || fu_cv_sys_mounted_getmntinfo=no
    ])
  AC_MSG_RESULT($fu_cv_sys_mounted_getmntinfo)
  if test $fu_cv_sys_mounted_getmntinfo = yes; then
    ac_cv_filesys=getmntinfo
  fi
fi

# FIXME: add a test for netbsd-1.1 here

if test "$ac_cv_filesys" = "none"; then
  # Ultrix
  AC_MSG_CHECKING([for getmnt function])
  AC_CACHE_VAL(fu_cv_sys_mounted_getmnt,
    [AC_PREPROC_IFELSE([AC_LANG_SOURCE([[
#include <sys/fs_types.h>
#include <sys/mount.h>]])],
		[fu_cv_sys_mounted_getmnt=yes],
		[fu_cv_sys_mounted_getmnt=no])])
  AC_MSG_RESULT($fu_cv_sys_mounted_getmnt)
  if test $fu_cv_sys_mounted_getmnt = yes; then
    ac_cv_filesys=getmnt
  fi
fi

if test "$ac_cv_filesys" = "none"; then
  # SVR2
  AC_MSG_CHECKING([whether it is possible to resort to fread on /etc/mnttab])
  AC_CACHE_VAL(fu_cv_sys_mounted_fread,
    [AC_PREPROC_IFELSE([AC_LANG_SOURCE([[#include <mnttab.h>]])],
		[fu_cv_sys_mounted_fread=yes],
		[fu_cv_sys_mounted_fread=no])])
  AC_MSG_RESULT($fu_cv_sys_mounted_fread)
  if test $fu_cv_sys_mounted_fread = yes; then
    ac_cv_filesys=fread
  fi
fi

if test "$ac_cv_filesys" = "none"; then
  case $TARGET_OS in
    tos | os2 | win32)
       ac_cv_filesys=native
       ;;
  esac
fi
])

case "$ac_cv_filesys" in
  getmntent1)  AC_DEFINE(MOUNTED_GETMNTENT1) ;;
  getmntent2)  AC_DEFINE(MOUNTED_GETMNTENT2) ;;
  getfsstat)   AC_DEFINE(MOUNTED_GETFSSTAT) ;;
  vmount)      AC_DEFINE(MOUNTED_VMOUNT) ;;
  fread_fstyp) AC_DEFINE(MOUNTED_FREAD_FSTYP) ;;
  getmntinfo)  AC_DEFINE(MOUNTED_GETMNTINFO) ;;
  getmnt)      AC_DEFINE(MOUNTED_GETMNT) ;;
  fread)       AC_DEFINE(MOUNTED_FREAD) ;;
  native)      ;;
  *) AC_MSG_ERROR([could not determine how to read list of mounted filesystems]) ;;
esac

])
