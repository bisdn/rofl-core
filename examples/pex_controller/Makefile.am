MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = pex_controller

pex_controller_SOURCES = \
	ethswctld.cc \
	etherswitch.cc \
	etherswitch.h \
	cfib.cc \
	cfib.h \
	cfibentry.h \
	cfibentry.cc
pex_controller_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
