MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = ethswctld

ethswctld_SOURCES = \
	ethswctld.cc \
	etherswitch.cc \
	etherswitch.h \
	cfibtable.cc \
	cfibtable.h \
	cfibentry.h \
	cfibentry.cc \
	cflowtable.h \
	cflowtable.cc \
	cflowentry.h \
	cflowentry.cc

ethswctld_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
