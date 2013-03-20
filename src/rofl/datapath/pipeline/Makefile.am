MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = common util openflow platform

lib_LTLIBRARIES = librofl_pipeline.la

librofl_pipeline_ladir = $(includedir)/rofl/datapath/pipeline

librofl_pipeline_la_HEADERS = physical_switch.h \
	switch_port.h 

librofl_pipeline_la_SOURCES = physical_switch.h \
	physical_switch.c \
	switch_port.c \
	switch_port.h

librofl_pipeline_la_LIBADD = \
	common/librofl_pipeline_common.la \
	util/librofl_pipeline_util.la \
	openflow/librofl_pipeline_openflow.la
