ACLOCAL_AMFLAGS = -I ../../m4

AM_CFLAGS = -Wall -Werror -Wextra

lib_LTLIBRARIES = libums.la

libums_la_SOURCES = \
	src/complist.c \
	src/hooks.c \
	src/scheduler.c \
	src/worker.c

libums_la_CFLAGS = -I$(srcdir)/include -I$(srcdir)/../module/src/uapi

libums_la_LDFLAGS = -version-info 0:0:0

SUBDIRS = include

pkgconfig_DATA = libums.pc

# add static C code analysis
static-check:
	@$(CPPCHECK) -I $(srcdir)/include -I $(srcdir)/../module/src/uapi $(srcdir)

check-local: static-check

.PHONY: static-check
