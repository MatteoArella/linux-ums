ACLOCAL_AMFLAGS = -I ../../m4

bin_PROGRAMS = simple-example

simple_example_SOURCES = \
    main.c

simple_example_CFLAGS = \
    -I$(top_srcdir)/../src/util \
    -I$(top_srcdir)/../src/lib/include

simple_example_LDADD = $(top_builddir)/../src/lib/libums.la
