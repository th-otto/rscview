include $(top_srcdir)/config.mk

INCLUDES += -I$(srcdir)
AM_CPPFLAGS += $(GLIB_CFLAGS) $(CURL_CFLAGS)

GEMLIBS = ../aes/libaes.a ../vdi/libvdi.a ../gemlib/libgem.a ../util/libutil.a

bin_PROGRAMS = rscview
if HAVE_CURL
noinst_PROGRAMS = rscview.cgi
endif

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
	fio_err.c \
	pofile.c \
	glib.c \
	$(empty)

rscview_cgi_SOURCES = \
	fileio.h \
	rsccgi.c \
	rsrcload.c \
	xmlout.c \
	object.c \
	rsc.c \
	fileio.c \
	bghio.c \
	fio_err.c \
	pofile.c \
	glib.c \
	cgic.c cgic.h \
	html.c html.h \
	$(empty)

rscview_LDADD = -L../aes -L../vdi -L../gemlib -L../util -laes -lgem -lvdi -laes -lgem -lvdi -lutil
rscview_DEPENDENCIES = $(GEMLIBS)

rscview_cgi_LDADD = -L../aes -L../vdi -L../gemlib -L../util -laes -lgem -lvdi -laes -lgem -lvdi -lutil $(GLIB_LIBS) $(CURL_LIBS)
rscview_cgi_DEPENDENCIES = $(GEMLIBS)
