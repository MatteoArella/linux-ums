ACLOCAL_AMFLAGS = -I ../../m4

bin_PROGRAMS = simple-example

simple_example_SOURCES = \
    main.c

simple_example_CPPFLAGS = -I$(top_srcdir)/../src/lib/include
simple_example_LDADD = $(top_builddir)/../src/lib/libums.la -lpthread
