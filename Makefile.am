ACLOCAL_AMFLAGS = -I m4

if BUILD_EXAMPLES
  MAYBE_EXAMPLES = examples
endif

if BUILD_BENCHMARK
  MAYBE_BENCHMARK = benchmark
endif

SUBDIRS = src/module src/lib src/util $(MAYBE_EXAMPLES) $(MAYBE_BENCHMARK)
EXTRA_DIST = autogen.sh LICENSE
