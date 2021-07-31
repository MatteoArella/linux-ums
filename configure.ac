#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([linux-ums], [1.0.0], [arella.matteo.95@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign 1.13])

AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC
LT_PREREQ([2.4])
LT_INIT

# Use the C language and compiler for the following checks
AC_LANG([C])

AC_ARG_ENABLE([examples],
    [AS_HELP_STRING([--enable-examples], [Build examples])],
    [],
    [enable_examples=no])
AM_CONDITIONAL([BUILD_EXAMPLES], [test x$enable_examples != xno])

AC_ARG_ENABLE([benchmark],
    [AS_HELP_STRING([--enable-benchmark], [Build benchmark])],
    [],
    [enable_benchmark=no])
AM_CONDITIONAL([BUILD_BENCHMARK], [test x$enable_benchmark != xno])

AC_CONFIG_FILES([Makefile src/util/Makefile])

# Call configure scripts recursively
AC_CONFIG_SUBDIRS([src/module src/lib examples])
AS_IF([test x$enable_benchmark != xno ], [
    AC_CONFIG_SUBDIRS([benchmark])
])

AC_OUTPUT
