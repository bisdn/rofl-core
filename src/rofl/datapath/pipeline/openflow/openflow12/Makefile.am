MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = pipeline

noinst_LTLIBRARIES = librofl_pipeline_openflow12.la

library_includedir = $(includedir)/rofl/datapath/pipeline/openflow/openflow12

library_include_HEADERS = of12_switch.h \
			of12_async_events_hooks.h \
			openflow12.h

librofl_pipeline_openflow12_la_SOURCES = of12_switch.h \
	of12_switch.c

librofl_pipeline_openflow12_la_LIBADD = pipeline/librofl_pipeline_openflow12_pipeline.la
