ACLOCAL_AMFLAGS = -I ../../m4

EXTRA_PROGRAMS = automake_dummy

automake_dummy_SOURCES = \
	src/Makefile \
	src/ums_mod.c \
	src/device.c \
	src/device.h \
	src/log.h \
	src/uapi/ums_ioctl.h

generated_sources =

module_DATA = ums.o
module_objs = ums_mod.o device.o

KERNEL_LOCATION=@kerneldir@

MODULE_EXTRA_CFLAGS = $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -Wall -Werror

export MODULE_EXTRA_CFLAGS KERNEL_LOCATION module_DATA module_objs

.PHONY: FORCE

$(automake_dummy_SOURCES): FORCE
	test -f $(builddir)/$@ || \
		($(MKDIR_P) $(@D) && $(LN_S) $(abs_top_srcdir)/$@ $@)

# Add FORCE in case the kernel has changed.
$(module_DATA): $(generated_sources) $(automake_dummy_SOURCES) FORCE
	$(MAKE) -C $(KERNEL_LOCATION) M=$(abs_builddir)/src modules

install-moduleDATA: $(module_DATA)
	test -z "$(DESTDIR)$(moduledir)" ||\
	    $(MKDIR_P) "$(DESTDIR)$(moduledir)"
	$(INSTALL_DATA) $(abs_builddir)/src/ums.ko $(DESTDIR)$(moduledir)/ums.ko
	depmod -a
	modprobe ums

uninstall-moduleDATA:
	$(RM) $(DESTDIR)$(moduledir)/ums.ko
	modprobe -r ums

clean-local:
	$(MAKE) -C $(KERNEL_LOCATION) M=$(abs_builddir)/src clean

FORCE:
