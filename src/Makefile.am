include $(top_srcdir)/config.mk

INCLUDES += -I$(srcdir)

GEMLIBS = ../aes/libaes.a ../gemlib/libgem.a

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

rscview_LDADD = -L../aes -L../gemlib -L../util -laes -lgem -laes -lgem -lutil
rscview_DEPENDENCIES = $(GEMLIBS)
