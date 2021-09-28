ACLOCAL_AMFLAGS = -I m4

if BUILD_EXAMPLES
  MAYBE_EXAMPLES = examples
endif

if BUILD_BENCHMARK
  MAYBE_BENCHMARK = benchmark
endif

if BUILD_DOCS
  MAYBE_DOCS = doc
endif

SUBDIRS = src/module src/lib src/util $(MAYBE_EXAMPLES) $(MAYBE_BENCHMARK) $(MAYBE_DOCS)
EXTRA_DIST = autogen.sh LICENSE m4/version.m4
