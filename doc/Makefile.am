# HTML target
HTML_OUTPUT = $(builddir)/html
html-local:
	@$(SPHINXBUILD) -b html -c $(builddir) $(srcdir) $(HTML_OUTPUT)

# PDF target
if HAVE_LATEX

LATEX_OUTPUT = $(builddir)/latex

pdf-local:
	@$(SPHINXBUILD) -b latex -c $(builddir) $(srcdir) $(LATEX_OUTPUT)
	@$(MAKE) -C latex

endif

MOSTLYCLEANFILES = -r $(HTML_OUTPUT) $(LATEX_OUTPUT)
EXTRA_DIST = \
	$(srcdir)/_static \
	$(srcdir)/_templates \
	$(srcdir)/images \
	$(srcdir)/kernelspace \
	$(srcdir)/userspace \
	$(srcdir)/benchmarks.rst \
	$(srcdir)/index.rst \
	$(srcdir)/install.rst \
	$(srcdir)/overview.rst \
	$(srcdir)/tools.rst
