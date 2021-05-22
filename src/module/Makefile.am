ACLOCAL_AMFLAGS = -I ../../m4

EXTRA_PROGRAMS = automake_dummy

automake_dummy_SOURCES = \
	src/Makefile \
	src/ums.c

generated_sources =

module_DATA = ums.o

KERNEL_LOCATION=@kerneldir@

export KERNEL_LOCATION module_DATA

.PHONY: FORCE

$(automake_dummy_SOURCES): FORCE
	test -f $(builddir)/$@ || \
		$(LN_S) $(abs_top_srcdir)/$@ $(builddir)/src/

# Add FORCE in case the kernel has changed.
$(module_DATA): $(generated_sources) $(automake_dummy_SOURCES) FORCE
	$(MAKE) -C $(KERNEL_LOCATION) M=$(abs_builddir)/src modules EXTRA_CFLAGS="-Wall"

all-local: src/ums.ko

install-exec-hook: src/ums.ko
	test -z "$(DESTDIR)$(moduledir)" ||\
	    $(MKDIR_P) "$(DESTDIR)$(moduledir)"
	$(INSTALL) $(builddir)/src/ums.ko $(DESTDIR)$(moduledir)/ums.ko

clean-local:
	$(MAKE) -C $(KERNEL_LOCATION) M=$(abs_builddir)/src clean

FORCE:
