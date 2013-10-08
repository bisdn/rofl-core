MAINTAINERCLEANFILES = Makefile.in

export INCLUDES += -I$(abs_srcdir)/../ #-I$(abs_builddir)

if PIPELINE_SUPPORT
PIPELINE_DIRS=datapath/pipeline
endif

if ONLY_C_CODE
SUBDIRS = datapath/afa $(PIPELINE_DIRS)
else
SUBDIRS = common datapath/afa $(PIPELINE_DIRS) platform
LIBS_ROFL=common/librofl_common.la platform/libplatform.la
endif

#library_includedir=$(includedir)/rofl 
#include_HEADERS=rofl.h

lib_LTLIBRARIES = librofl.la
librofl_la_SOURCES = #rofl.h 
librofl_la_LIBADD = $(LIBS_ROFL) -lpthread

#AM_LDFLAGS= -static
AM_CPPFLAGS= -fPIC
