#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libums], [1.0.0], [arella.matteo.95@gmail.com])
AC_SUBST(PACKAGE_DESCRIPTION, ["User Mode Scheduling library."])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([../../m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign 1.13 subdir-objects])
AM_SILENT_RULES([yes])

PKG_PROG_PKG_CONFIG([0.25]) # check and set $PKG_CONFIG

# Checks for programs.
AC_PROG_CC
LT_PREREQ([2.4])
LT_INIT
PKG_INSTALLDIR

# Use the C language and compiler for the following checks
AC_LANG([C])

# Checks for header files.
AC_CHECK_HEADERS_ONCE([ features.h
                        pthread.h
                        stddef.h
                        errno.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_SEARCH_LIBS([pthread_create], [pthread])

AC_CONFIG_FILES([libums.pc
                 Makefile
                 include/Makefile])

AC_OUTPUT
