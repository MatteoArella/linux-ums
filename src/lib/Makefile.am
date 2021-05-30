ACLOCAL_AMFLAGS = -I ../../m4

lib_LTLIBRARIES = libums.la

libums_la_SOURCES = \
	src/complist.c \
	src/context.c \
    src/scheduler.c \
    src/worker.c

libums_la_CPPFLAGS = -I$(srcdir)/include -I$(srcdir)/../module/src/uapi

libums_la_LDFLAGS = -version-info 0:0:0

SUBDIRS = include

pkgconfig_DATA = libums.pc
