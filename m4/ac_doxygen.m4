AC_DEFUN([AC_INIT_DOXYGEN], [
    AC_PATH_PROG([DOXYGEN], [doxygen], [false])
    AS_IF([test "x$DOXYGEN" = xfalse], [
        AC_MSG_ERROR(['doxygen' not found.])
    ])
])
