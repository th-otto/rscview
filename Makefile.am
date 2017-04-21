AUTOMAKE_OPTIONS = foreign dist-xz
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = data gemlib po aes vdi util src

EXTRA_DIST = include

CROSS_WIN32_HOST = i686-pc-mingw32
CROSS_WIN32_PREFIX = $(CROSS_WIN32_HOST)-
CROSS_MINT_HOST = m68k-atari-mint
CROSS_MINT_PREFIX = $(CROSS_MINT_HOST)-
CONFIGURE_ARGS = --enable-warnings --enable-fatal-warnings
TMPDIR=$(HOME)/tmp/rscview

DOWNLOADS=$(HOME)/webgo24/home/www/download

bindist:
	$(MAKE) clean
	./configure $(CONFIGURE_ARGS) --host=$(CROSS_WIN32_HOST)
	$(MAKE) -j8
	-mkdir $(TMPDIR)
	$(CROSS_WIN32_PREFIX)strip src/rscview.exe
	cp -a src/rscview.exe $(TMPDIR)
	cp -a /usr/$(CROSS_WIN32_HOST)/sys-root/mingw/bin/zlib1.dll $(TMPDIR)
	cp -a /usr/$(CROSS_WIN32_HOST)/sys-root/mingw/bin/libpng16-16.dll $(TMPDIR)
	cp -a /usr/lib/gcc/$(CROSS_WIN32_HOST)/libgcc_s_dw2-1.dll $(TMPDIR)
	$(RM) $(DOWNLOADS)/rscview-$(VERSION)-win32.zip
	(cd $(TMPDIR)/..; zip -r $(DOWNLOADS)/rscview-$(VERSION)-win32.zip rscview)
	$(MAKE) clean
	./configure $(CONFIGURE_ARGS) --host=$(CROSS_MINT_HOST)
	$(MAKE) -j8
	$(CROSS_MINT_PREFIX)strip src/rscview
	(cd src; mv rscview rscview.ttp; zip -r $(DOWNLOADS)/rscview-$(VERSION)-mint.zip rscview.ttp; rm -f rscview.ttp)
	$(MAKE) clean
	./configure $(CONFIGURE_ARGS)
	$(MAKE) -j8
	strip src/rscview
	(cd src; \
	 set -- `ldd rscview | sed -n 's/.*\(libpng.*\) => \([^ ]*\).*/\1 \2/p'`; \
	 so=$$1; \
	 f=`readlink -f $$2`; \
	 cp -a $$f $$so; tar cvfj $(DOWNLOADS)/rscview-$(VERSION)-linux.tar.bz2 rscview $$so; rm $$so)

bd:
	strip src/rscview
	(cd src; \
	 set -- `ldd rscview | sed -n 's/.*\(libpng.*\) => \([^ ]*\).*/\1 \2/p'`; \
	 so=$$1; \
	 f=`readlink -f $$2`; \
	 cp -a $$f $$so; tar cvfj $(DOWNLOADS)/rscview-$(VERSION)-linux.tar.bz2 rscview $$so; rm $$so)
