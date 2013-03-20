MAINTAINERCLEANFILES = Makefile.in

export INCLUDES += -I$(abs_srcdir)/../ #-I$(abs_builddir)

if EXPERIMENTAL 
EXPERIMENTAL_DIRS=experimental
EXPERIMENTAL_LIBS=experimental/libexperimental.la 
endif

SUBDIRS = config platform control $(EXPERIMENTAL_DIRS) $(PIPELINE_DIRS)

#library_includedir=$(includedir)/rofl 
#include_HEADERS=rofl.h

lib_LTLIBRARIES = librofl_controller.la
librofl_controller_la_SOURCES = #rofl.h 
librofl_controller_la_LIBADD = config/libconfig.la platform/libplatform.la $(EXPERIMENTAL_LIBS)

AM_LDFLAGS= -static
