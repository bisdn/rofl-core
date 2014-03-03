MAINTAINERCLEANFILES = Makefile.in

.NOTPARALLEL:

# build this before anything else
BUILT_SOURCES = matching_algorithms_available.h matching_algorithms_available.c

EXTRA_LTLIBRARIES = \
	librofl_pipeline_openflow1x_pipeline_matching_algorithms_loop.la

noinst_LTLIBRARIES = $(MATCHING_ALGORITHM_LIBADD) librofl_pipeline_openflow1x_pipeline_matching_algorithms.la

library_includedir = $(includedir)/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms

library_include_HEADERS = matching_algorithms.h \
	matching_algorithms_available.h

# loop matching
librofl_pipeline_openflow1x_pipeline_matching_algorithms_loop_ladir = \
	$(library_includedir)/loop
librofl_pipeline_openflow1x_pipeline_matching_algorithms_loop_la_HEADERS = \
	loop/of1x_loop_match.h
librofl_pipeline_openflow1x_pipeline_matching_algorithms_loop_la_SOURCES = \
	loop/of1x_loop_match.c \
	loop/of1x_loop_match.h

# combined library
librofl_pipeline_openflow1x_pipeline_matching_algorithms_la_SOURCES = \
	matching_algorithms.h

nodist_librofl_pipeline_openflow1x_pipeline_matching_algorithms_la_SOURCES = \
	matching_algorithms_available.c \
	matching_algorithms_available.h

librofl_pipeline_openflow1x_pipeline_matching_algorithms_la_LIBADD = \
	$(MATCHING_ALGORITHM_LIBADD) \
	$(MATCHING_ALGORITHM_LIBS)

librofl_pipeline_openflow1x_pipeline_matching_algorithms_la_DEPENDENCIES = \
	$(MATCHING_ALGORITHM_LIBADD)

matching_algorithms_available.h: matching_algorithms_available.sh Makefile
	$(SHELL) $(srcdir)/matching_algorithms_available.sh $(MATCHING_ALGORITHMS) > $(srcdir)/matching_algorithms_available.h

matching_algorithms_available.c: matching_algorithms_available_c.sh Makefile matching_algorithms_available.h matching_algorithms.h
	$(SHELL) $(srcdir)/matching_algorithms_available_c.sh $(srcdir) $(MATCHING_ALGORITHMS) > $(srcdir)/matching_algorithms_available.c

CLEANFILES = $(srcdir)/matching_algorithms_available.c \
	$(srcdir)/matching_algorithms_available.h
