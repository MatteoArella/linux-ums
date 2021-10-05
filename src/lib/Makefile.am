ACLOCAL_AMFLAGS = -I ../../m4

AM_CFLAGS = @PTHREAD_CFLAGS@ -Wall -Werror -Wextra

lib_LTLIBRARIES = libums.la

libums_la_SOURCES = \
	src/complist.c \
	src/hooks.c \
	src/scheduler.c \
	src/worker.c

EXTRA_DIST = \
	src/private.h

libums_la_CFLAGS = -I$(srcdir)/include -I$(srcdir)/../module/src/uapi
libums_la_LDFLAGS = -version-info 0:0:0
libums_la_LIBADD = @PTHREAD_LIBS@

SUBDIRS = include

pkgconfig_DATA = libums.pc

if BUILD_TESTS

if HAVE_LD_WRAP
lib_LTLIBRARIES += libums_mock.la
libums_mock_la_SOURCES = $(libums_la_SOURCES) tests/mock_ums_syscalls.c
libums_mock_la_CFLAGS = $(libums_la_CFLAGS)
libums_mock_la_LIBADD = $(libums_la_LIBADD)
libums_mock_la_LDFLAGS = $(libums_la_LDFLAGS) -Wl,-wrap,open -Wl,-wrap,close -Wl,-wrap,ioctl

TESTS = \
  check_ums_completion_list \
  check_ums_worker \
  check_ums_scheduler

check_PROGRAMS = \
  check_ums_completion_list \
  check_ums_worker \
  check_ums_scheduler

check_ums_completion_list_SOURCES = \
	tests/check_ums.h \
	tests/check_ums_completion_list.c
check_ums_completion_list_CFLAGS = -I$(srcdir)/include @CHECK_CFLAGS@
check_ums_completion_list_LDADD = libums_mock.la @CHECK_LIBS@

check_ums_worker_SOURCES = \
	tests/check_ums.h \
	tests/check_ums_worker.c
check_ums_worker_CFLAGS = -I$(srcdir)/include @CHECK_CFLAGS@
check_ums_worker_LDADD = libums_mock.la @CHECK_LIBS@

check_ums_scheduler_SOURCES = \
	tests/check_ums.h \
	tests/check_ums_scheduler.c
check_ums_scheduler_CFLAGS = -I$(srcdir)/include @CHECK_CFLAGS@
check_ums_scheduler_LDADD = libums_mock.la @CHECK_LIBS@

installcheck-local: $(check_PROGRAMS)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(check_PROGRAMS)'; d=".libs/"; for p in $$list; do \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(bindir)/$$p; \
	  chmod +x $(DESTDIR)$(bindir)/$$p; \
	done
uninstall-hook:
	@list='$(check_PROGRAMS)'; d=".libs/"; for p in $$list; do \
	  rm -rf $(DESTDIR)$(bindir)/$$p; \
	done
endif

endif

# add static C code analysis
static-check:
	-@$(CPPCHECK) -I $(srcdir)/include -I $(srcdir)/../module/src/uapi $(srcdir)

check-local: static-check

.PHONY: static-check

if BUILD_DOCS
include ../../aminclude.am

html-local: doxygen-doc

if HAVE_LATEX
pdf-local: doxygen-doc
endif

endif
