MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = pipeline

noinst_LTLIBRARIES = librofl_pipeline_openflow1x.la

library_includedir =  $(includedir)/rofl/datapath/pipeline/openflow/openflow1x

library_include_HEADERS = of1x_switch.h \
			of1x_async_events_hooks.h 

librofl_pipeline_openflow1x_la_SOURCES = of1x_switch.h \
	of1x_switch.c

librofl_pipeline_openflow1x_la_LIBADD = pipeline/librofl_pipeline_openflow1x_pipeline.la
