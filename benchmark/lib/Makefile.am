ACLOCAL_AMFLAGS = -I ../../m4

AM_CFLAGS = -Wall -Werror -Wextra

lib_LTLIBRARIES = libums-benchmark.la

libums_benchmark_la_SOURCES = \
	src/suite.c \
	src/suite.h \
	src/timer.c \
	src/timer.h \
	src/ums/scheduler.c \
	src/ums/scheduler.h \
	src/ums/worker.c \
	src/ums/worker.h \
	src/ums/suite.c \
	src/pthread/worker.c \
	src/pthread/worker.h \
	src/pthread/suite.c

libums_benchmark_la_CFLAGS = \
    -I$(srcdir)/include \
    -I$(top_srcdir)/../src/lib/include

libums_benchmark_la_LDFLAGS = -version-info 0:0:0

SUBDIRS = include

# add static C code analysis
static-check:
	-@$(CPPCHECK) -I $(srcdir)/include $(srcdir)

check-local: static-check

.PHONY: static-check
