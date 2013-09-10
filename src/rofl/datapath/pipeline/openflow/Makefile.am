MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow1x

noinst_LTLIBRARIES = librofl_pipeline_openflow.la

library_includedir = $(includedir)/rofl/datapath/pipeline/openflow

library_include_HEADERS = of_switch.h

librofl_pipeline_openflow_la_SOURCES = of_switch.h \
	of_switch.c

librofl_pipeline_openflow_la_LIBADD = openflow1x/librofl_pipeline_openflow1x.la
