include $(top_srcdir)/config.mk

INCLUDES += -I$(srcdir)

GEMLIBS = ../aes/libaes.a ../vdi/libvdi.a ../gemlib/libgem.a ../util/libutil.a

bin_PROGRAMS = rscview

LIBS += -lpng -lz

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
	writepng.c writepng.h \
	$(empty)

rscview_LDADD = -L../aes -L../vdi -L../gemlib -L../util -laes -lgem -lvdi -laes -lgem -lvdi -lutil
rscview_DEPENDENCIES = $(GEMLIBS)
