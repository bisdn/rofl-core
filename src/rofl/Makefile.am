MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = common config platform experimental  

export INCLUDES += -I$(abs_srcdir) #-I$(abs_builddir)

lib_LTLIBRARIES = librofl.la
librofl_la_SOURCES = rofl.h 
librofl_la_LIBADD = common/libcommon.la config/libconfig.la platform/libplatform.la experimental/libexperimental.la 

