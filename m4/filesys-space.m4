dnl ***************************************************************************
dnl AC_FILESYS_SPACE
dnl ***************************************************************************
AH_TEMPLATE([STATFS_OSF1], [Define if  statfs takes 3 args.  [DEC Alpha running OSF/1]])
AH_TEMPLATE([STATFS_STATVFS], [Define if there is a function named statvfs.  [SVR4]])
AH_TEMPLATE([STATFS_STATFS4], [Define if statfs takes 4 args.  [SVR3, Dynix, Irix, Dolphin]])
AH_TEMPLATE([STATFS_BSIZE], [Define if statfs takes 2 args and struct statfs has a field named f_bsize.
[4.3BSD, SunOS 4, HP-UX, AIX PS/2]])
AH_TEMPLATE([STATFS_FSIZE], [Define if statfs takes 2 args and struct statfs has a field named f_fsize.
[4.4BSD, NetBSD]])
AH_TEMPLATE([STATFS_FS_DATA], [Define if statfs takes 2 args and the second argument has
type struct fs_data.  [Ultrix]])
AH_TEMPLATE([STATFS_READ], [Define if there is no specific function for reading filesystems usage
information and you have the <sys/filsys.h> header file.  [SVR2]])

AC_DEFUN([AC_FILESYS_SPACE],[
AC_STRUCT_ST_BLOCKS
AC_STRUCT_ST_BLKSIZE
AC_STRUCT_ST_RDEV

AC_CHECKING(how to get filesystem space usage)
AC_CACHE_VAL(ac_cv_space,
[ac_cv_space=none

if test "$ac_cv_space" = none; then
	# DEC Alpha running OSF/1
	AC_MSG_CHECKING([for 3-argument statfs function (DEC/OSF1)])
        ac_res=no
	AC_TRY_RUN([
#include <sys/param.h>
#include <sys/types.h>
#include <sys/mount.h>
	main ()
	{
	struct statfs fsd;
	fsd.f_fsize = 0;
	exit (statfs (".", &fsd, sizeof (struct statfs)));
	}],
	ac_cv_space=osf1 ac_res=yes, ac_res=no, ac_res=no)
	AC_MSG_RESULT($ac_res)
fi
if test "$ac_cv_space" = none; then
	# SVR4
	AC_CHECK_FUNCS(statvfs)
	if test "$ac_cv_func_statvfs" = yes; then
		AC_TRY_CPP([
#include <sys/statvfs.h>
#include <sys/fstyp.h>],
		ac_cv_space=statvfs)
	fi
fi

if test "$ac_cv_space" = none; then
  AC_MSG_CHECKING([for two-argument statfs with statfs.bsize dnl
member (AIX, 4.3BSD)])
  ac_res=no
  AC_TRY_RUN([
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
#ifdef HAVE_SYS_VFS_H
#include <sys/vfs.h>
#endif
  main ()
  {
  struct statfs fsd;
  fsd.f_bsize = 0;
  exit (statfs (".", &fsd));
  }],
  ac_cv_space=bsize ac_res=yes, ac_res=no, ac_res=no)
  AC_MSG_RESULT($ac_res)
fi
if test "$ac_cv_space" = none; then
# SVR3
  AC_MSG_CHECKING([for four-argument statfs (AIX-3.2.5, SVR3)])
  ac_res=no
  AC_TRY_RUN([#include <sys/types.h>
#include <sys/statfs.h>
  main ()
  {
  struct statfs fsd;
  exit (statfs (".", &fsd, sizeof fsd, 0));
  }],
  ac_cv_space=statfs4 ac_res=yes, ac_res=no, ac_res=no)
  AC_MSG_RESULT($ac_res)
fi

if test "$ac_cv_space" = none; then
# 4.4BSD and NetBSD
  AC_MSG_CHECKING([for two-argument statfs with statfs.fsize dnl
member (4.4BSD and NetBSD)])
  ac_res=no
  AC_TRY_RUN([#include <sys/types.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
  main ()
  {
  struct statfs fsd;
  fsd.f_fsize = 0;
  exit (statfs (".", &fsd));
  }],
  ac_cv_space=fsize ac_res=yes, ac_res=no, ac_res=no)
  AC_MSG_RESULT($ac_res)
fi
if test "$ac_cv_space" = none; then
  # Ultrix
  AC_MSG_CHECKING([for two-argument statfs with struct fs_data (Ultrix)])
  ac_res=no
  AC_TRY_RUN([#include <sys/types.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
#ifdef HAVE_SYS_FS_TYPES_H
#include <sys/fs_types.h>
#endif
  main ()
  {
  struct fs_data fsd;
  /* Ultrix's statfs returns 1 for success,
     0 for not mounted, -1 for failure.  */
  exit (statfs (".", &fsd) != 1);
  }],
  ac_sv_space=fs_data ac_res=yes, ac_res=no, ac_res=no)
  AC_MSG_RESULT($ac_res)
fi

if test "$ac_cv_space" = none; then
AC_TRY_CPP([#include <sys/filsys.h>],
ac_cv_space=read)
fi

if test "$ac_cv_space" = "none"; then
  case $TARGET_OS in
    tos | os2 | win32)
       ac_cv_space=native
       ;;
  esac
fi
])dnl
case "$ac_cv_space" in
  osf1) AC_DEFINE(STATFS_OSF1) ;;
  statvfs) AC_DEFINE(STATFS_STATVFS) ;;
  bsize) AC_DEFINE(STATFS_BSIZE) ;;
  statfs4) AC_DEFINE(STATFS_STATFS4) ;;
  fsize) AC_DEFINE(STATFS_FSIZE) ;;
  fs_data) AC_DEFINE(STATFS_FS_DATA) ;;
  read) AC_DEFINE(STATFS_READ) ;;
  native)      ;;
  *)
AC_MSG_WARN([Sorry, this doesn't seem to work. Please let me know what system you are
using, and I'll try to fix this problem.])
   ;;
esac
])
