ACLOCAL_AMFLAGS = -I ../../m4

EXTRA_PROGRAMS = lint_dummy automake_dummy

lint_dummy_SOURCES = \
	src/Makefile \
	src/complist.c \
	src/complist.h \
	src/context.c \
	src/context.h \
	src/enter_ums.c \
	src/ums.h \
	src/ums_mod.c \
	src/device.c \
	src/device.h \
	src/log.h \
	src/scheduler.c \
	src/scheduler.h \
	src/worker.c \
	src/worker.h \
	src/idr_l.h \
	src/proc/base.c \
	src/proc/base.h \
	src/proc/context.h \
	src/proc/context.c \
	src/proc/scheduler.h \
	src/proc/scheduler.c \
	src/proc/worker.h \
	src/proc/worker.c

automake_dummy_SOURCES = \
	$(lint_dummy_SOURCES) \
	src/uapi/ums/ums_ioctl.h

module_DATA = ums.o
module_objs = \
	ums_mod.o \
	complist.o \
	context.o \
	device.o \
	enter_ums.o \
	proc/base.o \
	proc/context.o \
	proc/scheduler.o \
	proc/worker.o \
	scheduler.o \
	worker.o

MODULE_EXTRA_CFLAGS = $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -Wall -Werror

SUBDIRS = src/uapi

export MODULE_EXTRA_CFLAGS module_DATA module_objs

.PHONY: FORCE lint

$(automake_dummy_SOURCES): FORCE
	@test -f $(builddir)/$@ || \
		($(MKDIR_P) $(@D) && $(LN_S) $(abs_top_srcdir)/$@ $@)

# Add FORCE in case the kernel has changed.
$(module_DATA): $(automake_dummy_SOURCES) FORCE
	$(MAKE) -C $(KERNELDIR) M=$(abs_builddir)/src modules

install-moduleDATA: $(module_DATA)
	$(mkinstalldirs) $(DESTDIR)$(moduledir)
	$(INSTALL_DATA) $(abs_builddir)/src/ums.ko $(DESTDIR)$(moduledir)/ums.ko

uninstall-moduleDATA:
	$(RM) $(DESTDIR)$(moduledir)/ums.ko

if SANDBOXED
else
install-data-local: install-moduleDATA
	$(DEPMOD) -a
endif

uninstall-local: uninstall-moduleDATA

clean-local:
	$(MAKE) -C $(KERNELDIR) M=$(abs_builddir)/src clean

lint: $(lint_dummy_SOURCES)
	-@$(CHECKPATCH) --no-tree --ignore LINUX_VERSION_CODE --file $^

check-local: lint

FORCE:

if BUILD_DOCS
include ../../aminclude.am

html-local: doxygen-doc

if HAVE_LATEX
pdf-local: doxygen-doc
endif

endif
