AUTOMAKE_OPTIONS = subdir-objects

include $(top_srcdir)/config.mk

AM_CPPFLAGS += -I$(srcdir)

noinst_LIBRARIES = libutil.a

libutil_a_SOURCES = \
	debug.c \
	nls.c \
	$(empty)

if OS_UNIX
libutil_a_SOURCES += \
	sys/unix/gemdos.c \
	$(empty)
endif

if OS_WIN32
libutil_a_SOURCES += \
	sys/unix/gemdos.c \
	$(empty)
endif

if OS_TOS
libutil_a_SOURCES += \
	sys/tos/fclose.c \
	sys/tos/gemdos.c \
	$(empty)
endif
