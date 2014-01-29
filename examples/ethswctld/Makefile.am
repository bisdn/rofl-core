MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = ethswctld

ethswctld_SOURCES = \
	ethswctld.cc \
	etherswitch.cc \
	etherswitch.h \
	cfib.cc \
	cfib.h \
	cfibentry.h \
	cfibentry.cc
ethswctld_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
