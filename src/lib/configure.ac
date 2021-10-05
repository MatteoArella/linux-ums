#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
m4_include([../../m4/version.m4])
AC_INIT([libums], [VERSION_NUMBER], [arella.matteo.95@gmail.com])
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

PKG_CHECK_MODULES([CHECK], [check >= 0.9.6], [], [have_check=no])
AM_CONDITIONAL([BUILD_TESTS], [test x$have_check != xno])

AC_PATH_PROG([CPPCHECK], [cppcheck], [true])
AS_IF([test "x$CPPCHECK" = xtrue], [
    AC_MSG_WARN(['cppcheck' is recommended for developing this package.])
])

# Use the C language and compiler for the following checks
AC_LANG([C])

AC_DOCS_ENABLE

# Checks for header files.
AC_CHECK_HEADERS_ONCE([ features.h
                        stddef.h
                        stdlib.h
                        errno.h
                        unistd.h
                        fcntl.h
                        sys/ioctl.h])

# Checks for pthread implementation.
AX_PTHREAD

# Check if linker supports -wrap
AX_CHECK_LINK_FLAG([-Wl,-wrap,open], [], [have_ld_wrap=no])
AM_CONDITIONAL([HAVE_LD_WRAP], [test x$have_ld_wrap != xno])

AC_CONFIG_FILES([libums.pc
                 Makefile
                 include/Makefile])
AS_IF([test x$enable_docs != xno ], [
    AC_CONFIG_FILES([Doxyfile])
])

AC_OUTPUT
