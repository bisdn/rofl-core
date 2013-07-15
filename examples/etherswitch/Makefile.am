MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = etherswitch

etherswitch_SOURCES = \
	main.cc \
	etherswitch.cc \
	etherswitch.h \
	cfib.cc \
	cfib.h \
	cfibentry.h \
	cfibentry.cc
etherswitch_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
