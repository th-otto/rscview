#!/bin/sh
# Run this to generate all the initial makefiles, etc.

test -n "$srcdir" || srcdir="${0%/*}"
test -n "$srcdir" || srcdir=.

set -e

cd "$srcdir"
rm -rf autom4te.cache

# get local m4 stuff
aclocal -I m4
# produce config.h.in
autoheader
# get missing system files & produce Makefile.in
automake --copy --add-missing
# produce configure
autoconf

rm -rf autom4te.cache
