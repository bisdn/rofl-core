MAINTAINERCLEANFILES = Makefile.in

export INCLUDES += -I$(abs_srcdir)/../ #-I$(abs_builddir)

if PIPELINE_SUPPORT
PIPELINE_DIRS=datapath/pipeline
endif

SUBDIRS = datapath/hal $(PIPELINE_DIRS)
if !ONLY_C_CODE
SUBDIRS += common datapath/hal $(PIPELINE_DIRS) platform
LIBS_ROFL=common/librofl_common.la platform/libplatform.la
endif

lib_LTLIBRARIES = librofl.la
librofl_la_SOURCES = ../rofl.c
librofl_la_LIBADD = $(LIBS_ROFL) -lpthread

ROFL_LIBRARY_VERSION = 1:1:1

librofl_la_LDFLAGS = -version-info $(ROFL_LIBRARY_VERSION)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = rofl.pc

AM_CPPFLAGS= -fPIC

