include $(top_srcdir)/config.mk

AM_CPPFLAGS += -I$(srcdir)
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
	charset.c cp_atari.h cp_latin1.h cp_latin2.h cp_latin9.h cp_1250.h cp_1251.h cp_1252.h cp_737.h \
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
	charset.c cp_atari.h cp_latin1.h cp_latin2.h cp_latin9.h cp_1250.h cp_1251.h cp_1252.h cp_737.h \
	cgic.c cgic.h \
	html.c html.h htmljs.h \
	$(empty)

rscview_LDADD = -L../aes -L../vdi -L../gemlib -L../util -laes -lgem -lvdi -laes -lgem -lvdi -lutil
rscview_DEPENDENCIES = $(GEMLIBS)
if OS_UNIX
rscview_LDFLAGS=-Wl,-rpath,'$$ORIGIN:$$ORIGIN/../lib64:$$ORIGIN/../lib' -Wl,-z,origin
endif
rscview$(EXEEXT): $(rscview_OBJECTS) $(rscview_DEPENDENCIES) $(EXTRA_rscview_DEPENDENCIES) 
	@rm -f rscview$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(rscview_OBJECTS) $(rscview_LDFLAGS) $(rscview_LDADD) $(LIBS)
if !CROSS_COMPILING
	$(AM_V_at)if test -d "$(SRVDIR)"; then \
	   echo "installing $@ in $(SRVDIR)"; \
	   cp -a $@ "$(SRVDIR)/rscview"; \
	fi
endif


rscview_cgi_LDADD = -L../aes -L../vdi -L../gemlib -L../util -laes -lgem -lvdi -laes -lgem -lvdi -lutil $(GLIB_LIBS) $(CURL_LIBS)
rscview_cgi_DEPENDENCIES = $(GEMLIBS)
if OS_UNIX
rscview_cgi_LDFLAGS=-Wl,-rpath,'$$ORIGIN:$$ORIGIN/../lib64:$$ORIGIN/../lib' -Wl,-z,origin
endif
rscview.cgi$(EXEEXT): $(rscview_cgi_OBJECTS) $(rscview_cgi_DEPENDENCIES) $(EXTRA_rscview_cgi_DEPENDENCIES) 
	@rm -f rscview.cgi$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(rscview_cgi_OBJECTS) $(rscview_cgi_LDFLAGS) $(rscview_cgi_LDADD) $(LIBS)
if !CROSS_COMPILING
	$(AM_V_at)if test -d "$(SRVDIR)"; then \
	   echo "installing $@ in $(SRVDIR)"; \
	   cp -a $@ "$(SRVDIR)/rscview.cgi"; \
	fi
endif

