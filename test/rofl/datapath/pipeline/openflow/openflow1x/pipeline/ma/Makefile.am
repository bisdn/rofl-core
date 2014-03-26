MAINTAINERCLEANFILES = Makefile.in

SUBDIRS=loop

SHARED_SRC= $(top_srcdir)/src/rofl/datapath/pipeline/physical_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/monitoring.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/switch_port.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/port_queue.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/util/logging.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/common/ternary_fields.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/common/packet_matches.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/of_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_action.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_match.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_instruction.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_timers.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_statistics.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_group_table.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/loop/of1x_loop_ma.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms_available.c \
	../memory.c \
	../empty_packet.c\
	../platform_empty_hooks_of12.cc\
	../pthread_atomic_operations.c\
	../pthread_lock.c \
	../timing.c

pipe_profile_SOURCES= $(SHARED_SRC)\
			profiling_tests.c\
			pipeline_profile.c
		

pipe_profile_LDADD=$(top_builddir)/src/rofl/librofl.la -lcunit -lpthread

pipe_profile_CFLAGS = -O3 -pg 
pipe_profile_CXXFLAGS = -O3 -pg 
pipe_profile_CPPFLAGS = -UDEBUG -DNDEBUG=1 

check_PROGRAMS= pipe_profile
TESTS = pipe_profile
