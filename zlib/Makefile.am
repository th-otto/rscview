AUTOMAKE_OPTIONS = subdir-objects

include $(top_srcdir)/config.mk

AM_CPPFLAGS += -I$(srcdir)
AM_CPPFLAGS += $(Z_CFLAGS) -DZLIB_STATIC

if !WITH_SYSTEM_ZLIB
noinst_LIBRARIES = libz.a
endif

libz_a_SOURCES = \
	adler32.c \
	crc32.c \
	deflate.c \
	infback.c \
	inffast.c \
	inflate.c \
	inftrees.c \
	trees.c \
	zutil.c \
	compress.c \
	uncompr.c \
	gzclose.c \
	gzlib.c \
	gzread.c \
	gzwrite.o \
	contrib/minizip/ioapi.c \
	contrib/minizip/zip.c \
	contrib/minizip/unzip.c \
	contrib/minizip/iowin32.c

match.o: match.S
	$(CPP) match.S > _match.s
	$(CC) -c _match.s
	mv _match.o match.o
	rm -f _match.s

