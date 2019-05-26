AUTOMAKE_OPTIONS = subdir-objects

include $(top_srcdir)/config.mk

AM_CPPFLAGS += -I$(srcdir)
AM_CPPFLAGS += $(Z_CFLAGS) -DZLIB_STATIC

if !WITH_SYSTEM_ZLIB
noinst_LIBRARIES = libz.a
endif

libz_a_SOURCES = \
	zlib.h zconf.h \
	adler32.c \
	crc32.c crc32.h \
	deflate.c deflate.h \
	infback.c \
	inffast.c inffast.h inffixed.h \
	inflate.c inflate.h \
	inftrees.c inftrees.h \
	trees.c trees.h \
	zutil.c zutil.h \
	dbgmem.h \
	compress.c \
	uncompr.c \
	gzclose.c gzguts.h \
	gzlib.c \
	gzread.c \
	gzwrite.c \
	contrib/minizip/ioapi.c contrib/minizip/ioapi.h contrib/minizip/crypt.h \
	contrib/minizip/zip.c contrib/minizip/zip.h \
	contrib/minizip/unzip.c contrib/minizip/unzip.h \
	contrib/minizip/iowin32.c contrib/minizip/iowin32.h

match.o: match.S
	$(CPP) match.S > _match.s
	$(CC) -c _match.s
	mv _match.o match.o
	rm -f _match.s

