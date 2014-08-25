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
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/available_ma.c \
	../memory.c \
	../platform_empty_hooks_of12.cc\
	../pthread_atomic_operations.c\
	../pthread_lock.c \
	../timing.c

pipe_profile_SOURCES= $(SHARED_SRC)\
			../empty_packet.c\
			profiling_tests.c
pipe_profile_LDADD=$(top_builddir)/src/rofl/librofl.la -lcunit -lpthread
pipe_profile_CFLAGS = -O3 -pg 
pipe_profile_CXXFLAGS = -O3 -pg 
pipe_profile_CPPFLAGS = -UDEBUG 


lockless_SOURCES= $(SHARED_SRC)\
			../empty_packet.c\
			lockless_tests.c
lockless_LDADD=$(top_builddir)/src/rofl/librofl.la -lcunit -lpthread
lockless_CFLAGS = -O3 -pg 
lockless_CXXFLAGS = -O3 -pg 
lockless_CPPFLAGS = -UDEBUG 


check_PROGRAMS = lockless pipe_profile
TESTS = lockless pipe_profile 
