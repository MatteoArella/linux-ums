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
	src/worker.h

automake_dummy_SOURCES = \
	$(lint_dummy_SOURCES) \
	src/uapi/ums_ioctl.h

module_DATA = ums.o
module_objs = ums_mod.o complist.o context.o device.o enter_ums.o scheduler.o worker.o

MODULE_EXTRA_CFLAGS = $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -Wall -Werror

export MODULE_EXTRA_CFLAGS module_DATA module_objs

.PHONY: FORCE lint

$(automake_dummy_SOURCES): FORCE
	@test -f $(builddir)/$@ || \
		($(MKDIR_P) $(@D) && $(LN_S) $(abs_top_srcdir)/$@ $@)

# Add FORCE in case the kernel has changed.
$(module_DATA): $(automake_dummy_SOURCES) FORCE
	$(MAKE) -C $(KERNELDIR) M=$(abs_builddir)/src modules

install-moduleDATA: $(module_DATA)
	@test -z "$(DESTDIR)$(moduledir)" ||\
	    $(MKDIR_P) "$(DESTDIR)$(moduledir)"
	$(INSTALL_DATA) $(abs_builddir)/src/ums.ko $(DESTDIR)$(moduledir)/ums.ko
	depmod -a
	modprobe ums

uninstall-moduleDATA:
	$(RM) $(DESTDIR)$(moduledir)/ums.ko
	modprobe -r ums

clean-local:
	$(MAKE) -C $(KERNELDIR) M=$(abs_builddir)/src clean

lint: $(lint_dummy_SOURCES)
	-@$(CHECKPATCH) --no-tree --ignore LINUX_VERSION_CODE --file $^

check-local: lint

FORCE:
