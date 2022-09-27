#!/bin/sh

./autogen.sh
./configure --enable-warnings
make || exit 1
make DESTDIR="$TMP" install || exit 1
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
cp "${OUT}/${PROJECT}-${ATAG}-linux.tar.xz" "${OUT}/latest-snapshot.tar.xz"
)

if $isrelease; then
	make dist
	mv "${PROJECT}-${VERSION}.tar.xz" "$OUT"
	cp "${OUT}/${PROJECT}-${VERSION}.tar.xz" "${OUT}/latest-release.tar.xz"
fi
ls -l "${OUT}"
