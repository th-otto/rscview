AUTOMAKE_OPTIONS = subdir-objects

include $(top_srcdir)/config.mk

INCLUDES += -I$(srcdir)

noinst_LIBRARIES = libutil.a

libutil_a_SOURCES =

if OS_UNIX
libutil_a_SOURCES += \
	sys/unix/gemdos.c \
	$(empty)
endif
