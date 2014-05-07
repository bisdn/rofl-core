MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = tranny morpheus

morpheus_SOURCES = \
	morpheus_main.cc \
	morpheus.cc \
	cportvlan_mapper.cc
	
morpheus_LDADD = ../../src/rofl/librofl.la \
	-lpthread \
	-lrt 

tranny_SOURCES = \
	tranny.cc \
	tranny.h \
	main.cc
tranny_LDADD = ../../src/rofl/librofl.la \
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
