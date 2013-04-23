MAINTAINERCLEANFILES = Makefile.in

SHARED_SRC= $(top_srcdir)/src/rofl/datapath/pipeline/physical_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/switch_port.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/port_queue.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/util/logging.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/common/ternary_fields.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/of_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/of12_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_action.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_match.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_instruction.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_flow_entry.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_flow_table.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_pipeline.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_packet_matches.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_timers.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_statistics.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_group_table.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/matching_algorithms/loop/of12_loop_match.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/matching_algorithms/matching_algorithms.c \
	../../memory.c \
	../../platform_empty_actions.c\
	../../platform_empty_packet_matches.c\
	../../platform_empty_hooks_of12.cc\
	../../pthread_atomic_operations.c\
	../../pthread_lock.c

unit_test_SOURCES= $(SHARED_SRC)\
		   	matching_test.c\
			unit_test.c
		

unit_test_LDADD=$(top_builddir)/src/rofl/librofl.la -lcunit -lpthread

check_PROGRAMS= unit_test
TESTS = unit_test
