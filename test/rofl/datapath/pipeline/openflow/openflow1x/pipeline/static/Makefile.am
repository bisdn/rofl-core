MAINTAINERCLEANFILES = Makefile.in

static_unit_test_CFLAGS= -DTIMERS_FAKE_TIME 
static_unit_test_CPPFLAGS= -I../../../../../../../../../src/

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
	../../../../../../../../../src/rofl/datapath/pipeline/physical_switch.c \
	../../../../../../../../../src/rofl/datapath/pipeline/switch_port.c \
	../../../../../../../../../src/rofl/datapath/pipeline/port_queue.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/of_switch.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms_available.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/loop/of1x_loop_match.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_action.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_group_table.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_instruction.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_match.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_packet_matches.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_statistics.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_timers.c \
	../../../../../../../../../src/rofl/datapath/pipeline/common/ternary_fields.c \
	../../../../../../../../../src/rofl/datapath/pipeline/util/logging.c

static_unit_test_LDADD= -lcunit -lpthread

#-lrofl_pipeline -lrofl
# we need to link to the local libraries, not the installed ones
# and if we want to apply special flags, we need to compile again.

check_PROGRAMS= static_unit_test
TESTS = static_unit_test
