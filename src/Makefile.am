include $(top_srcdir)/config.mk

AM_CPPFLAGS += -I$(srcdir)
AM_CPPFLAGS += $(GLIB_CFLAGS) $(CURL_CFLAGS)
AM_CPPFLAGS += $(PNG_CFLAGS)
AM_CPPFLAGS += $(Z_CFLAGS)

AM_CPPFLAGS += -Wno-deprecated-declarations

GEMLIBS = ../aes/libaes.a ../vdi/libvdi.a ../gemlib/libgem.a ../util/libutil.a

bin_PROGRAMS = rscview
if HAVE_CURL
noinst_PROGRAMS = rscview.cgi
endif

SRVDIR = /srv/www/htdocs/rscview

LIBS += $(PNG_LIBS) $(Z_LIBS) -lm

rscview_SOURCES = \
	fileio.h rso.h \
	rscview.c \
	rsrcload.c \
	xmlout.c \
	object.c \
	rsc.c \
	rsc_lang.c rsc_lang.h langtab.h \
	fileio.c \
	bghio.c \
	fio_err.c \
	or_draw.c \
	or_drawo.c \
	or_sgem.c \
	fo_alert.c \
	w_draw.c \
	pofile.c pofile.h \
	glib.c \
	charset.c cp_atari.h cp_iso1.h cp_st_l2.h cp_st_l9.h cp_1250.h cp_1252.h cp_st_gr.h cp_st_ru.h cp_st_l5.h \
	$(empty)

rscview_cgi_SOURCES = \
	fileio.h rso.h \
	rsccgi.c \
	rsrcload.c \
	xmlout.c \
	object.c \
	rsc.c \
	rsc_lang.c rsc_lang.h langtab.h \
	fileio.c \
	bghio.c \
	or_draw.c \
	or_drawo.c \
	or_sgem.c \
	fo_alert.c \
	w_draw.c \
	fio_err.c \
	pofile.c pofile.h \
	glib.c \
	charset.c cp_atari.h cp_iso1.h cp_st_l2.h cp_st_l9.h cp_1250.h cp_1252.h cp_st_gr.h cp_st_ru.h cp_st_l5.h \
	cgic.c cgic.h \
	html.c html.h \
	$(empty)

rscview_LDADD = -L../aes -L../vdi -L../gemlib -L../util -laes -lgem -lvdi -laes -lgem -lvdi -lutil @GLIBC_SO_SCRIPT@
rscview_DEPENDENCIES = $(GEMLIBS)
rscview$(EXEEXT): $(rscview_OBJECTS) $(rscview_DEPENDENCIES) $(EXTRA_rscview_DEPENDENCIES) 
	@rm -f rscview$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(rscview_OBJECTS) $(rscview_LDFLAGS) $(rscview_LDADD) $(LIBS)
if !CROSS_COMPILING
	$(AM_V_at)if test -d "$(SRVDIR)"; then \
	   echo "installing $@ in $(SRVDIR)"; \
	   cp $@ "$(SRVDIR)/rscview"; \
	fi
endif


rscview_cgi_LDADD = -L../aes -L../vdi -L../gemlib -L../util -laes -lgem -lvdi -laes -lgem -lvdi -lutil $(GLIB_LIBS) $(CURL_LIBS) @GLIBC_SO_SCRIPT@
rscview_cgi_DEPENDENCIES = $(GEMLIBS)
if !WITH_SYSTEM_LIBPNG
rscview_cgi_DEPENDENCIES += $(PNG_LIBS)
endif
if !WITH_SYSTEM_ZLIB
rscview_cgi_DEPENDENCIES += $(Z_LIBS)
endif
rscview.cgi$(EXEEXT): $(rscview_cgi_OBJECTS) $(rscview_cgi_DEPENDENCIES) $(EXTRA_rscview_cgi_DEPENDENCIES) 
	@rm -f rscview.cgi$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(rscview_cgi_OBJECTS) $(rscview_cgi_LDFLAGS) $(rscview_cgi_LDADD) $(LIBS)
if !CROSS_COMPILING
	$(AM_V_at)if test -d "$(SRVDIR)"; then \
	   echo "installing $@ in $(SRVDIR)"; \
	   cp $@ "$(SRVDIR)/rscview.cgi"; \
	fi
endif

