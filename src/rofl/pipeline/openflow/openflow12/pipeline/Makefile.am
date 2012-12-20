MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = matching_algorithms

noinst_LTLIBRARIES = librofl_pipeline_openflow12_pipeline.la

librofl_pipeline_openflow12_pipeline_ladir = $(includedir)/rofl/pipeline/openflow/openflow12/pipeline

librofl_pipeline_openflow12_pipeline_la_HEADERS = of12_action.h \
	of12_flow_entry.h \
	of12_flow_table.h \
	of12_instruction.h \
	of12_match.h \
	of12_packet_matches.h \
	of12_pipeline.h \
	of12_timers.h \
	of12_statistics.h

librofl_pipeline_openflow12_pipeline_la_SOURCES = of12_action.h \
	of12_flow_entry.h \
	of12_flow_table.h \
	of12_instruction.h \
	of12_match.h \
	of12_packet_matches.h \
	of12_pipeline.h \
	of12_timers.h \
	of12_action.c \
	of12_flow_entry.c \
	of12_flow_table.c \
	of12_instruction.c \
	of12_match.c \
	of12_packet_matches.c \
	of12_pipeline.c \
	of12_timers.c \
	of12_statistics.c

librofl_pipeline_openflow12_pipeline_la_LIBADD = matching_algorithms/librofl_pipeline_openflow12_pipeline_matching_algorithms.la
