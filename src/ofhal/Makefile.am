MAINTAINERCLEANFILES = Makefile.in

export INCLUDES += -I$(abs_srcdir)/../ #-I$(abs_builddir)

SUBDIRS = 

library_includedir=$(includedir)
include_HEADERS=ofhal.h

lib_LTLIBRARIES = libofhal.la
libofhal_la_SOURCES = ofhal.h ofhal.c
libofhal_la_LIBADD = -lpthread


