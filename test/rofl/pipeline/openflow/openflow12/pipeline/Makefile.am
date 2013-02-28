MAINTAINERCLEANFILES = Makefile.in

test_group_table_SOURCES= group_table.c \
	$(top_srcdir)/src/rofl/pipeline/openflow/openflow12/pipeline/of12_group_table.c \
	$(top_srcdir)/test/rofl/pipeline/openflow/openflow12/pipeline/memory.c

test_group_table_LDADD= -lrofl

check_PROGRAMS= test_group_table
TESTS = test_group_table
