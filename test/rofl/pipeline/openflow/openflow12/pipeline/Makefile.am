MAINTAINERCLEANFILES = Makefile.in

SUBDIRS=ma/loop

unit_test_SOURCES= unit_test.c \
	timers_hard_timeout.c \
	platform_empty_actions.c \
	platform_empty_packet_matches.c\
	platform_empty_hooks_of12.cc\
	memory.c \
	pthread_atomic_operations.c \
	pthread_lock.c \
	lib_random.c \
	$(top_srcdir)/src/rofl/pipeline/openflow/openflow12/pipeline/of12_group_table.c \
	$(top_srcdir)/src/rofl/pipeline/openflow/openflow12/pipeline/of12_action.c \
	$(top_srcdir)/src/rofl/pipeline/openflow/openflow12/pipeline/of12_packet_matches.c

#group_table.c
	
unit_test_LDADD= -lrofl -lcunit -lpthread -lrofl_pipeline
unit_test_LDFLAGS= -DTIMERS_FAKE_TIME

check_PROGRAMS= unit_test
TESTS = unit_test
