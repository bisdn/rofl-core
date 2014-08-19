MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = matching_algorithms

noinst_LTLIBRARIES = librofl_pipeline_openflow1x_pipeline.la

librofl_pipeline_openflow1x_pipeline_ladir = $(includedir)/rofl/datapath/pipeline/openflow/openflow1x/pipeline

librofl_pipeline_openflow1x_pipeline_la_HEADERS = of1x_action.h \
	of1x_action_pp.h \
	of1x_flow_entry.h \
	of1x_flow_table.h \
	of1x_flow_table_pp.h \
	of1x_group_table.h \
	of1x_group_types.h \
	of1x_instruction.h \
	of1x_instruction_pp.h \
	of1x_match.h \
	of1x_match_pp.h \
	of1x_pipeline.h \
	of1x_pipeline_pp.h \
	of1x_timers.h \
	of1x_statistics.h\
	of1x_utils.h

librofl_pipeline_openflow1x_pipeline_la_SOURCES = of1x_action.h \
	of1x_flow_entry.h \
	of1x_flow_table.h \
	of1x_group_table.h \
	of1x_instruction.h \
	of1x_match.h \
	of1x_pipeline.h \
	of1x_timers.h \
	of1x_action.c \
	of1x_flow_entry.c \
	of1x_flow_table.c \
	of1x_group_table.c \
	of1x_instruction.c \
	of1x_match.c \
	of1x_pipeline.c \
	of1x_timers.c \
	of1x_statistics.c

librofl_pipeline_openflow1x_pipeline_la_LIBADD = matching_algorithms/librofl_pipeline_openflow1x_pipeline_matching_algorithms.la
