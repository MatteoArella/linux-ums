AC_DEFUN([AC_INIT_LATEX], [
    AC_CHECK_PROGS([LATEX], [latex elatex], [false])
    AS_IF([test "x$LATEX" = xfalse], [
        AC_MSG_WARN(['latex' not found.])
    ])
    AC_CHECK_PROGS([LATEXMK], [latexmk], [false])
    AS_IF([test "x$LATEXMK" = xfalse], [
        AC_MSG_WARN(['latexmk' not found.])
    ])
])
