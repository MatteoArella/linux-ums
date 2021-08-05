AC_DEFUN([AC_DOCS_ENABLE], [
    AC_ARG_ENABLE([docs],
    [AS_HELP_STRING([--enable-docs], [Build documentation])],
    [
        enable_docs=yes
        AC_INIT_LATEX
        AC_INIT_DOXYGEN
        AC_INIT_SPHINX
    ],
    [enable_docs=no])
    AM_CONDITIONAL([BUILD_DOCS], [test x$enable_docs == xyes])

    AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
    AM_CONDITIONAL([HAVE_DOT], [test -n "$DOT"])
    AM_CONDITIONAL([HAVE_SPHINX], [test -n "$SPHINXBUILD"])
    AM_CONDITIONAL([HAVE_LATEX], [test -n "$LATEX"])
])
