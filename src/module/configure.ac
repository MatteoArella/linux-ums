#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ums-kmod], [1.0.0], [arella.matteo.95@gmail.com])
AC_SUBST(PACKAGE_DESCRIPTION, ["User Mode Scheduling linux kernel module."])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([../../m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign 1.13 subdir-objects])
AM_SILENT_RULES([yes])

AC_SUBST([with_kernel], [`uname -r`])
AC_SUBST([kerneldir], [/lib/modules/$with_kernel/build])
AC_SUBST([moduledir], [/lib/modules/$with_kernel/extra])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PROG_LN_S
LT_PREREQ([2.4])
LT_INIT

# Use the C language and compiler for the following checks
AC_LANG([C])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
