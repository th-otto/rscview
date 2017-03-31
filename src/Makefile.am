include $(top_srcdir)/config.mk

INCLUDES += -I$(srcdir)

GEMLIBS = ../aeslib/libaes.a ../aes/libgem.a ../vdilib/libvdi.a

bin_PROGRAMS = rscview

rscview_SOURCES = \
	fileio.h \
	rscview.c \
	rsrcload.c \
	xmlout.c \
	object.c \
	rsc.c \
	fileio.c \
	bghio.c \
	w_draw.c \
	w_font.c \
	fio_err.c \
	$(empty)

rscview_LDADD = $(GEMLIBS)
rscview_DEPENDENCIES = $(GEMLIBS)
