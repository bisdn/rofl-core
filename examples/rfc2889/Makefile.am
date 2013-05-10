MAINTAINERCLEANFILES = Makefile.in

if !ONLY_C_CODE

SUBDIRS = 

sbin_PROGRAMS = rfc2889

rfc2889_SOURCES = \
	main.cc \
	rfc2889.cc \
	rfc2889.h
rfc2889_LDADD = ../../src/rofl/librofl.la 
	-lpthread \
	-lrt 

AM_LDFLAGS = -static
endif
