MAINTAINERCLEANFILES = Makefile.in

export INCLUDES += -I$(abs_srcdir)/../ #-I$(abs_builddir)

SUBDIRS = common config platform experimental  

library_includedir=$(includedir)/rofl 
include_HEADERS=rofl.h

lib_LTLIBRARIES = librofl.la
librofl_la_SOURCES = rofl.h 
librofl_la_LIBADD = common/libcommon.la config/libconfig.la platform/libplatform.la experimental/libexperimental.la 


