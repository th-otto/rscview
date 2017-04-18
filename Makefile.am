AUTOMAKE_OPTIONS = foreign dist-xz
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = data gemlib po aes vdi util src

DIST_SUBDIRS = $(SUBDIRS) include www

CROSS_HOST = i686-pc-mingw32
CROSS_PREFIX = $(CROSS_HOST)-
CONFIGURE_ARGS = --enable-warnings --enable-fatal-warnings
TMPDIR=$(HOME)/tmp/rscview

DOWNLOADS=$(HOME)/webgo24/home/www/download

bindist:
	$(MAKE) clean
	./configure $(CONFIGURE_ARGS) --host=$(CROSS_HOST)
	$(MAKE)
	-mkdir $(TMPDIR)
	$(CROSS_PREFIX)strip src/rscview.exe
	cp -a src/rscview.exe $(TMPDIR)
	cp -a /usr/$(CROSS_HOST)/sys-root/mingw/bin/zlib1.dll $(TMPDIR)
	cp -a /usr/$(CROSS_HOST)/sys-root/mingw/bin/libpng16-16.dll $(TMPDIR)
	cp -a /usr/lib/gcc/$(CROSS_HOST)/libgcc_s_dw2-1.dll $(TMPDIR)
	$(RM) $(DOWNLOADS)/rscview-$(VERSION)-win32.zip
	(cd $(TMPDIR)/..; zip -r $(DOWNLOADS)/rscview-$(VERSION)-win32.zip rscview)
	$(MAKE) clean
	./configure $(CONFIGURE_ARGS)
	$(MAKE)
	strip src/rscview
	(cd src; tar cvfj $(DOWNLOADS)/rscview-$(VERSION)-linux.tar.bz2 rscview)
