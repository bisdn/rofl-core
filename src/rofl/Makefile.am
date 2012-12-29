MAINTAINERCLEANFILES = Makefile.in

export INCLUDES += -I$(abs_srcdir)/../ #-I$(abs_builddir)

if PIPELINE_SUPPORT
PIPELINE_DIRS=pipeline
endif

if EXPERIMENTAL 
EXPERIMENTAL_DIRS=experimental
EXPERIMENTAL_LIBS=experimental/libexperimental.la 
endif

SUBDIRS = common config platform control $(EXPERIMENTAL_DIRS) $(PIPELINE_DIRS)

library_includedir=$(includedir)/rofl 
include_HEADERS=rofl.h

lib_LTLIBRARIES = librofl.la
librofl_la_SOURCES = rofl.h 
librofl_la_LIBADD = common/libcommon.la config/libconfig.la platform/libplatform.la $(EXPERIMENTAL_LIBS)

AM_LDFLAGS= -static
