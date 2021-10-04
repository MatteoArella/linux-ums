#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
m4_include([../m4/version.m4])
AC_INIT([libums-examples], [VERSION_NUMBER], [arella.matteo.95@gmail.com])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign 1.13 subdir-objects])
AM_SILENT_RULES([yes])

# Define _GNU_SOURCE appropriately
AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC
LT_PREREQ([2.4])
LT_INIT

# Use the C language and compiler for the following checks
AC_LANG([C])

# Checks for header files.
AC_CHECK_HEADERS_ONCE([ stdio.h
                        stdlib.h
                        stddef.h
                        errno.h
                        unistd.h
                        sched.h])

# Checks for pthread implementation.
AX_PTHREAD([ax_pthread_ok = yes])

# Checks for library functions.
AS_IF([test x$ax_pthread_ok = xyes], [
        AC_CHECK_DECLS([pthread_attr_setaffinity_np], [], [], [#include <pthread.h>])
])
AC_CHECK_FUNCS([sched_setaffinity])

AC_CONFIG_FILES([Makefile
                00-simple-example/Makefile])

AC_OUTPUT
