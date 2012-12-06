MAINTAINERCLEANFILES = Makefile.in

# build this before anything else
BUILT_SOURCES = matching_algorithms_available.h matching_algorithms.c

EXTRA_LTLIBRARIES = \
	librofl_pipeline_openflow12_pipeline_matching_algorithms_loop.la
# 	librofl_pipeline_openflow12_pipeline_matching_algorithms_ALGORITHM_NAME.la

noinst_LTLIBRARIES = $(MATCHING_ALGORITHM_LIBADD) librofl_pipeline_openflow12_pipeline_matching_algorithms.la

library_includedir = $(includedir)/rofl/pipeline/openflow/openflow12/pipeline/matching_algorithms

library_include_HEADERS = matching_algorithms.h \
	matching_algorithms_available.h

# loop matching
librofl_pipeline_openflow12_pipeline_matching_algorithms_loop_la_SOURCES = \
	loop/of12_loop_match.c \
	loop/of12_loop_match.h

# new algorithm stub:
# librofl_pipeline_openflow12_pipeline_matching_algorithms_ALGORITHM_NAME_la_SOURCES = \
# 	ALGORITHM_NAME/of12_loop_match.c \
# 	ALGORITHM_NAME/of12_loop_match.h \
#	ALGORITHM_NAME/further_sources

# combined library
librofl_pipeline_openflow12_pipeline_matching_algorithms_la_SOURCES = \
	matching_algorithms.h

nodist_librofl_pipeline_openflow12_pipeline_matching_algorithms_la_SOURCES = \
	matching_algorithms.c \
	matching_algorithms_available.h

librofl_pipeline_openflow12_pipeline_matching_algorithms_la_LIBADD = \
	$(MATCHING_ALGORITHM_LIBADD) \
	$(MATCHING_ALGORITHM_LIBS)

matching_algorithms_available.h: matching_algorithms_available.sh Makefile
	$(SHELL) $(srcdir)/matching_algorithms_available.sh $(MATCHING_ALGORITHMS) > $(srcdir)/matching_algorithms_available.h

matching_algorithms.c: matching_algorithms.sh Makefile matching_algorithms_available.h matching_algorithms.h
	$(SHELL) $(srcdir)/matching_algorithms.sh $(srcdir) $(MATCHING_ALGORITHMS) > $(srcdir)/matching_algorithms.c

CLEANFILES = $(srcdir)/matching_algorithms.c \
	$(srcdir)/matching_algorithms_available.h
