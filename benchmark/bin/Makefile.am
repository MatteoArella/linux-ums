ACLOCAL_AMFLAGS = -I ../../m4

bin_PROGRAMS = ums-benchmark

ums_benchmark_SOURCES = \
    include/task.h \
    include/ums/model.h \
    include/pthread/model.h \
    src/main.c \
    src/ums/model.c \
    src/pthread/model.c

ums_benchmark_CFLAGS = \
    -I$(top_srcdir)/lib/include \
    -I$(top_srcdir)/bin/include \
    -I$(top_srcdir)/../src/util \
    -I$(top_srcdir)/../src/lib/include

ums_benchmark_LDADD = \
    $(top_builddir)/../src/lib/libums.la \
    $(builddir)/../lib/libums-benchmark.la
