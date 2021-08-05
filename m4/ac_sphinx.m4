AC_DEFUN([AC_INIT_SPHINX], [
    AC_PATH_PROG([SPHINXBUILD], [sphinx-build], [false])
    AS_IF([test "x$SPHINXBUILD" = xfalse], [
        AC_MSG_ERROR(['sphinx-build' not found.])
    ])
])
