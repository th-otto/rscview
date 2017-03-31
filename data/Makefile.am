include $(top_srcdir)/config.mk

INCLUDES += -I$(srcdir)

noinst_PROGRAMS = fntdump

CC = $(BUILD_CC)
EXEEXT = $(BUILD_EXEEXTT)

fntdump_SOURCES = \
	fntdump.c \
	$(empty)

BUILT_SOURCES = font_8x16.c font_8x8.c font_6x8.c

font_8x16.c: $(srcdir)/system10.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_st_8x16 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

font_8x8.c: $(srcdir)/system09.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_st_8x8 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

font_6x8.c: $(srcdir)/system08.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_st_6x8 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

EXTRA_DIST += system08.fnt system09.fnt system10.fnt

CLEANFILES += $(BUILT_SOURCES)
