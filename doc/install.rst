.. SPDX-License-Identifier: AGPL-3.0-only

Building
********

Minimal Requirements
====================

- GNU C             >= 5.1
- GNU make          >= 3.81
- libtool
- pkg-config
- linux-headers
- check\ [#f1]_
- sphinx\ [#f2]_
- doxygen\ [#f2]_
- breathe\ [#f2]_
- sphinx_rtd_theme\ [#f2]_
- latex\ [#f3]_

.. [#f1] Optional: needed only for userspace library tests
.. [#f2] Optional: needed only to build the documentation
.. [#f3] Optional: needed only to build the pdf documentation

Development Requirements
========================

The following dependencies are needed in case of development build:

- autotools-dev
- autoconf
- automake
- git
- cppcheck\ [#f4]_

.. [#f4] Optional: needed only for userspace library static code analysis

Build
=====

.. code::

    $ ./configure [OPTION]... [VAR=VALUE]... && make [TARGET]...

To assign environment variables (e.g., ``CC``, ``CFLAGS``...), specify them as
``VAR=VALUE``.  See below for descriptions of some of the useful variables.

Options
-------

.. tabularcolumns:: |l|L|

========================== ====================================================
``--enable-examples``       Enable examples build
``--enable-benchmark``      Enable benchmark build
``--enable-docs``           Enable documentation build
``--prefix=PREFIX``         Install architecture-independent files in
                            ``PREFIX``; default to :file:`/usr/local`
``--exec-prefix=EPREFIX``   Install architecture-dependent files in
                            ``EPREFIX``; default to ``PREFIX``
``--bindir=DIR``            User executables; default to :file:`$EPREFIX/bin`
``--libdir=DIR``            Object code libraries; default to
                            :file:`$EPREFIX/lib`
``--includedir=DIR``        C header files; default to :file:`$PREFIX/include`
``--datarootdir=DIR``       Read-only architecture-independent data root;
                            default to :file:`$PREFIX/share`
``--datadir=DIR``           Read-only architecture-independent data; default to
                            :file:`$DATAROOTDIR`
``--docdir=DIR``            Documentation root; default to
                            :file:`$DATAROOTDIR/doc/linux-ums`
``--htmldir=DIR``           Html documentation; default to :file:`$DOCDIR`
``--pdfdir=DIR``            Pdf documentation; default to :file:`$DOCDIR`
``--build=BUILD``           Configure for building on ``BUILD``; guessed by
                            default
``--host=HOST``             Cross-compile to build programs to run on ``HOST``;
                            default to ``$BUILD``
``--enable-silent-rules``   Less verbose build output (undo: ``make V=1``)
``--disable-silent-rules``  Verbose build output (undo: ``make V=0``)
========================== ====================================================

Variables
---------

Some influential environment variables:

.. tabularcolumns:: |l|L|

========================== ====================================================
``CC``                      C compiler command
``CFLAGS``                  C compiler flags
``LDFLAGS``                 Linker flags
``LIBS``                    Libraries to pass to the linker
``CPPFLAGS``                C preprocessor flags
``LT_SYS_LIBRARY_PATH``     User-defined run-time library search path.
``CPP``                     C preprocessor
========================== ====================================================

Targets
-------

.. tabularcolumns:: |l|L|

========================== ====================================================
``check``                   Run code check
``html``                    Build the html documentation
``pdf``                     Build the pdf documentation
``install``                 Install into the system
``installcheck``            Install tests into the system
``uninstall``               Uninstall from the system
``clean``                   Delete all files in the current directory that are
                            normally created by building the program
``distclean``               Delete all files in the current directory that are
                            created by configuring or building the program
========================== ====================================================

Minimal installation steps
==========================

.. code::

    $ ./configure
    $ make
    $ make check
    $ sudo make install
    $ sudo ldconfig
    $ sudo modprobe ums
