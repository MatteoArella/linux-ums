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

AC_ARG_VAR([KERNELDIR], ["linux kernel dir"])

AS_IF([test "x$KERNELDIR" = x], [
    [KERNELDIR=/lib/modules/$with_kernel/build]
    [moduledir=/lib/modules/$with_kernel/extra]

    # check linux headers
    AC_CHECK_HEADERS([linux/ioctl.h], [], [
        AC_MSG_ERROR([You must install 'linux-headers-$with_kernel'])
    ])
], [])
AC_SUBST([KERNELDIR])
AC_SUBST([moduledir])

# check if 'checkpatch.pl' script exists in PATH or in KERNELDIR
AC_PATH_PROG([CHECKPATCH], [checkpatch.pl], [true])
AS_IF([test "x$CHECKPATCH" = xtrue], [
    AS_IF([test -f $KERNELDIR/scripts/checkpatch.pl], [
        AC_SUBST([CHECKPATCH], [$KERNELDIR/scripts/checkpatch.pl])
    ], [])
])

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

AC_CONFIG_FILES([Makefile
                 src/uapi/Makefile])

AC_OUTPUT
