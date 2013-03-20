MAINTAINERCLEANFILES = Makefile.in

export INCLUDES += -I$(abs_srcdir)/../ #-I$(abs_builddir)

if PIPELINE_SUPPORT
PIPELINE_DIRS=datapath/pipeline
endif

SUBDIRS = common controller $(PIPELINE_DIRS)

#library_includedir=$(includedir)/rofl 
#include_HEADERS=rofl.h

lib_LTLIBRARIES = librofl.la
librofl_la_SOURCES = #rofl.h 
librofl_la_LIBADD = common/librofl_common.la controller/librofl_controller.la #datapath/libdatapath.la

AM_LDFLAGS= -static
