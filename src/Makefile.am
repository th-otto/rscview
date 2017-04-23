include $(top_srcdir)/config.mk

INCLUDES += -I$(srcdir)
AM_CPPFLAGS += $(GLIB_CFLAGS) $(CURL_CFLAGS)

GEMLIBS = ../aes/libaes.a ../vdi/libvdi.a ../gemlib/libgem.a ../util/libutil.a

bin_PROGRAMS = rscview
if HAVE_CURL
noinst_PROGRAMS = rscview.cgi
endif

SRVDIR = /srv/www/htdocs/rscview

LIBS += -lpng -lz

rscview_SOURCES = \
	fileio.h rso.h \
	rscview.c \
	rsrcload.c \
	xmlout.c \
	object.c \
	rsc.c \
	fileio.c \
	bghio.c \
	fio_err.c \
	pofile.c pofile.h \
	glib.c \
	$(empty)

rscview_cgi_SOURCES = \
	fileio.h rso.h \
	rsccgi.c \
	rsrcload.c \
	xmlout.c \
	object.c \
	rsc.c \
	fileio.c \
	bghio.c \
	fio_err.c \
	pofile.c pofile.h \
	glib.c \
	cgic.c cgic.h \
	html.c html.h \
	$(empty)

rscview_LDADD = -L../aes -L../vdi -L../gemlib -L../util -laes -lgem -lvdi -laes -lgem -lvdi -lutil
rscview_DEPENDENCIES = $(GEMLIBS)
if OS_UNIX
rscview_LDFLAGS=-Wl,-rpath,'$$ORIGIN:$$ORIGIN/../lib64:$$ORIGIN/../lib'
endif
rscview$(EXEEXT): $(rscview_OBJECTS) $(rscview_DEPENDENCIES) $(EXTRA_rscview_DEPENDENCIES) 
	@rm -f rscview$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(rscview_OBJECTS) $(rscview_LDADD) $(LIBS)
if !CROSS_COMPILING
	$(AM_V_at)if test -d "$(SRVDIR)"; then \
	   echo "installing $@ in $(SRVDIR)"; \
	   cp -a $@ "$(SRVDIR)/rscview"; \
	fi
endif


rscview_cgi_LDADD = -L../aes -L../vdi -L../gemlib -L../util -laes -lgem -lvdi -laes -lgem -lvdi -lutil $(GLIB_LIBS) $(CURL_LIBS)
rscview_cgi_DEPENDENCIES = $(GEMLIBS)
if OS_UNIX
rscview_cgi_LDFLAGS=-Wl,-rpath,'$$ORIGIN:$$ORIGIN/../lib64:$$ORIGIN/../lib'
endif
rscview.cgi$(EXEEXT): $(rscview_cgi_OBJECTS) $(rscview_cgi_DEPENDENCIES) $(EXTRA_rscview_cgi_DEPENDENCIES) 
	@rm -f rscview.cgi$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(rscview_cgi_OBJECTS) $(rscview_cgi_LDADD) $(LIBS)
if !CROSS_COMPILING
	$(AM_V_at)if test -d "$(SRVDIR)"; then \
	   echo "installing $@ in $(SRVDIR)"; \
	   cp -a $@ "$(SRVDIR)/rscview.cgi"; \
	fi
endif

