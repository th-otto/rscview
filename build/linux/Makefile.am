EXTRA_DIST = \
	AppRun \
	LibcWrapGenerator.vala \
	LibcWrapGenerator.pl \
	PrepareAppDir.sh \
	README \
	jhbuildrc \
	modulesets \
	pangorc \
	triggers \
	$(empty)

#$(srcdir)/libcwrap.h: $(srcdir)/LibcWrapGenerator.vala
#	valac --pkg gee-0.8 --pkg posix --pkg glib-2.0 --pkg gio-2.0 $<
#	./LibcWrapGenerator --output=$@ --libdir=/lib64

libcwrap.h: $(srcdir)/LibcWrapGenerator.pl
	$(srcdir)/LibcWrapGenerator.pl --output=$@ --libdir=/lib64
