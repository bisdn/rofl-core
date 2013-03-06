MAINTAINERCLEANFILES = Makefile.in

unit_test_SOURCES= unit_test.c \
	group_table.c \
	platform_empty_actions.c \
	$(top_srcdir)/src/rofl/pipeline/openflow/openflow12/pipeline/of12_group_table.c \
	$(top_srcdir)/src/rofl/pipeline/openflow/openflow12/pipeline/of12_action.c \
	$(top_srcdir)/test/rofl/pipeline/openflow/openflow12/pipeline/memory.c \
	$(top_srcdir)/test/rofl/pipeline/openflow/openflow12/pipeline/pthread_lock.c

unit_test_LDADD= -lrofl -lcunit -lpthread

check_PROGRAMS= unit_test
TESTS = unit_test
