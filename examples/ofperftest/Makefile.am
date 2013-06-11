MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = ofperftest

ofperftest_SOURCES = \
	main.cc \
	ofperftest.cc \
	ofperftest.h
ofperftest_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
