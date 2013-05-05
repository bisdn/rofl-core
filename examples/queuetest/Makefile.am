MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = queuetest

queuetest_SOURCES = \
	main.cc \
	queuetest.cc \
	queuetest.h
queuetest_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
