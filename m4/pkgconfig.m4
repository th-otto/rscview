AC_DEFUN([AC_PROG_PKGCONFIG],[
# Find pkg-config... (need this for both gtk and gdk_pixbuf.)
# if the user specified --with-gtk=/foo/ then look there.
#
gtk_path="$PATH"
if test ! -z "$gtk_dir"; then
  # canonicalize slashes.
  foo=`echo "${gtk_dir}/bin" | sed 's@//*@/@g'`
  gtk_path="$foo:$gtk_path"
fi

AC_PATH_PROGS(pkg_config, pkg-config,, $gtk_path)

if test -z "$pkg_config" ; then
  AC_MSG_WARN([pkg-config not found!])
  pkg_config="false"
fi
#
# do not trust pkg-config when cross-compiling,
# unless PKG_CONFIG_LIBDIR was set up to
# point to the correct path
if test "$cross_compiling" != no -a "$PKG_CONFIG_LIBDIR" = ""; then
  pkg_config=false
fi
])
