ACLOCAL_AMFLAGS = -I ../../m4

AM_CFLAGS = @PTHREAD_CFLAGS@

bin_PROGRAMS = simple-example

simple_example_SOURCES = \
    global.h \
    main.c

simple_example_CFLAGS = \
    -I$(top_srcdir)/../src/util \
    -I$(top_srcdir)/../src/lib/include

simple_example_LDADD = $(top_builddir)/../src/lib/libums.la @PTHREAD_LIBS@
