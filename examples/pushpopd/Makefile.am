MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = pushpopd

pushpopd_SOURCES = \
	pushpopd.cpp \
	ccontrol.hpp \
	ccontrol.cpp
	
pushpopd_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
