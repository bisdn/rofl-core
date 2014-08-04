MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = testomatd

testomatd_SOURCES = \
	testomatd.cpp \
	ccontrol.hpp \
	ccontrol.cpp
	
testomatd_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
