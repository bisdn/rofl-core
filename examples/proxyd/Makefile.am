MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = proxyd

proxyd_SOURCES = \
	proxyd.cpp \
	ccontrol.hpp \
	ccontrol.cpp
	
proxyd_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
