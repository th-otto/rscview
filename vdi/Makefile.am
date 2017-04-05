AUTOMAKE_OPTIONS = subdir-objects

include $(top_srcdir)/config.mk

INCLUDES += -I$(srcdir)

noinst_LIBRARIES = libvdi.a

libvdi_a_SOURCES = \
	vdi.c vdi.h \
	vdi_bm.c \
	\
	vdidefs.h vdimaps.h pattern.h \
	$(empty)
