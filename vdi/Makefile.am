AUTOMAKE_OPTIONS = subdir-objects

include $(top_srcdir)/config.mk

AM_CPPFLAGS += -I$(srcdir)
AM_CPPFLAGS += $(PNG_CFLAGS)

noinst_LIBRARIES = libvdi.a

libvdi_a_SOURCES = \
	vdi.c vdi.h \
	vdi_bm.c \
	maptab.c maptab.h \
	\
	vdidefs.h vdimaps.h pattern.h \
	\
	writepng.c writepng.h \
	writebmp.c writebmp.h \
	$(empty)
