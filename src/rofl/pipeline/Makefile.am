MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = platform common util openflow

lib_LTLIBRARIES = librofl_pipeline.la

librofl_pipeline_ladir = $(includedir)/rofl/pipeline

librofl_pipeline_la_HEADERS = physical_switch.h \
	switch_port.h 

librofl_pipeline_la_SOURCES = physical_switch.h \
	physical_switch.c \
	switch_port.h

librofl_pipeline_la_LIBADD = \
	platform/librofl_pipeline_platform.la \
	common/librofl_pipeline_common.la \
	util/librofl_pipeline_util.la \
	openflow/librofl_pipeline_openflow.la
