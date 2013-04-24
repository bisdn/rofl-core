MAINTAINERCLEANFILES = Makefile.in

static_unit_test_CFLAGS= -DTIMERS_FAKE_TIME

static_unit_test_SOURCES=../unit_test.c \
	../output_actions.c \
	../timers_hard_timeout.c \
	../platform_empty_actions.c \
	../platform_empty_packet_matches.c\
	../platform_empty_hooks_of12.cc\
	../memory.c \
	../pthread_atomic_operations.c \
	../pthread_lock.c \
	../lib_random.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/physical_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/switch_port.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/port_queue.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/of_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/matching_algorithms/matching_algorithms.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/matching_algorithms/loop/of12_loop_match.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/of12_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_action.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_flow_entry.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_flow_table.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_group_table.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_instruction.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_match.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_packet_matches.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_pipeline.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_statistics.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_timers.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/common/ternary_fields.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/util/logging.c

static_unit_test_LDADD= -lcunit -lpthread

#-lrofl_pipeline -lrofl
# we need to link to the local libraries, not the installed ones
# and if we want to apply special flags, we need to compile again.

check_PROGRAMS= static_unit_test
TESTS = static_unit_test
