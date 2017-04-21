include $(top_srcdir)/config.mk

INCLUDES += -I$(srcdir)

noinst_PROGRAMS = fntdump bug

CC = $(BUILD_CC)
EXEEXT = $(BUILD_EXEEXTT)

fntdump_SOURCES = \
	fntdump.c \
	$(empty)

bug_SOURCES = \
	bug.c \
	$(empty)

BUILT_SOURCES = \
	fnt_st_8x16.c fnt_st_8x8.c fnt_st_6x8.c \
	fnt_l9_8x16.c fnt_l9_8x8.c fnt_l9_6x8.c \
	fnt_l2_8x16.c fnt_l2_8x8.c fnt_l2_6x8.c \
	fnt_gr_8x16.c fnt_gr_8x8.c fnt_gr_6x8.c \
	fnt_ru_8x16.c fnt_ru_8x8.c fnt_ru_6x8.c \
	$(empty)

fnt_st_8x16.c: $(srcdir)/system10.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_st_8x16 -I 1 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_st_8x8.c: $(srcdir)/system09.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_st_8x8 -I 1 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_st_6x8.c: $(srcdir)/system08.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_st_6x8 -I 1 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_l9_8x16.c: $(srcdir)/fntl910.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_l9_8x16 -I 2 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_l9_8x8.c: $(srcdir)/fntl909.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_l9_8x8 -I 2 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_l9_6x8.c: $(srcdir)/fntl908.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_l9_6x8 -I 2 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_l2_8x16.c: $(srcdir)/fntl210.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_l2_8x16 -I 3 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_l2_8x8.c: $(srcdir)/fntl209.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_l2_8x8 -I 3 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_l2_6x8.c: $(srcdir)/fntl208.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_l2_6x8 -I 3 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_gr_8x16.c: $(srcdir)/fntgr10.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_gr_8x16 -I 4 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_gr_8x8.c: $(srcdir)/fntgr09.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_gr_8x8 -I 4 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_gr_6x8.c: $(srcdir)/fntgr08.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_gr_6x8 -I 4 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_ru_8x16.c: $(srcdir)/fntru10.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_ru_8x16 -I 5 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_ru_8x8.c: $(srcdir)/fntru09.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_ru_8x8 -I 5 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

fnt_ru_6x8.c: $(srcdir)/fntru08.fnt fntdump$(EXEEXT)
	$(AM_V_GEN)./fntdump$(EXEEXT) -n fnt_ru_6x8 -I 5 $< > $@.$$$$ && mv $@.$$$$ $@ || { $(RM) $@.$$$$; false; }

EXTRA_DIST += \
	system08.fnt system09.fnt system10.fnt system20.fnt \
	fntl908.fnt fntl909.fnt fntl910.fnt \
	fntl208.fnt fntl209.fnt fntl210.fnt \
	fntgr08.fnt fntgr09.fnt fntgr10.fnt \
	fntru08.fnt fntru09.fnt fntru10.fnt \
	$(empty)

CLEANFILES += $(BUILT_SOURCES)
