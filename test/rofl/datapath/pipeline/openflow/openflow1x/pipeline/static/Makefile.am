MAINTAINERCLEANFILES = Makefile.in

static_unit_test_CFLAGS= -DTIMERS_FAKE_TIME 
static_unit_test_CPPFLAGS= -I$(top_srcdir)/src/

#FIXME add group table tests!
static_unit_test_SOURCES=../unit_test.c \
	../output_actions.c \
	../timers_hard_timeout.c \
	../empty_packet.c \
	../platform_empty_hooks_of12.cc\
	../memory.c \
	../pthread_atomic_operations.c \
	../pthread_lock.c \
	../lib_random.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/physical_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/monitoring.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/switch_port.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/port_queue.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/of_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms_available.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/loop/of1x_loop_match.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_action.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_group_table.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_instruction.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_match.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_packet_matches.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_statistics.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_timers.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/common/ternary_fields.c \
	$(top_srcdir)/src/rofl/datapath/pipeline/util/logging.c

static_unit_test_LDADD= -lcunit -lpthread

#-lrofl_pipeline -lrofl
# we need to link to the local libraries, not the installed ones
# and if we want to apply special flags, we need to compile again.

check_PROGRAMS= static_unit_test
TESTS = static_unit_test
