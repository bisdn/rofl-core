MAINTAINERCLEANFILES = Makefile.in

SHARED_SRC= ../../../../../../../../../src/rofl/datapath/pipeline/physical_switch.c \
	../../../../../../../../../src/rofl/datapath/pipeline/switch_port.c \
	../../../../../../../../../src/rofl/datapath/pipeline/port_queue.c \
	../../../../../../../../../src/rofl/datapath/pipeline/util/logging.c \
	../../../../../../../../../src/rofl/datapath/pipeline/common/ternary_fields.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/of_switch.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_action.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_match.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_instruction.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_packet_matches.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_timers.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_statistics.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_group_table.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/loop/of1x_loop_match.c \
	../../../../../../../../../src/rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms_available.c \
	../memory.c \
	../empty_packet.c\
	../platform_empty_hooks_of12.cc\
	../pthread_atomic_operations.c\
	../pthread_lock.c

unit_test_SOURCES= $(SHARED_SRC)\
			unit_test.c\
			reset_pipeline_test.c
		

unit_test_LDADD=$(top_builddir)/src/rofl/librofl.la -lcunit -lpthread

check_PROGRAMS= unit_test
TESTS = unit_test
