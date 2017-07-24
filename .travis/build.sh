#!/bin/sh

TMP="$1"
OUT="$2"

./autogen.sh --enable-warnings
make
make DESTDIR="$TMP" install
VERSION=$(grep PACKAGE_VERSION config.h | sed -e 's/^.*\"\(.*\)\".*/\1/')
ATAG=$VERSION
isrelease=false

mkdir -p "${OUT}"

tag=`git tag --points-at ${TRAVIS_COMMIT}`
case $tag in
	VERSION*)
		isrelease=true
		;;
	*)
		ATAG=$SHORT_ID
		;;
esac

(
cd "${TMP}"
tar cvfj "${OUT}/${PROJECT}-${ATAG}-linux.tar.bz2" .
tar cvfJ "${OUT}/${PROJECT}-${ATAG}-linux.tar.xz" .
)

if $isrelease; then
	make dist
	mv "${PROJECT}-${VERSION}.tar.xz" "$OUT"
fi
