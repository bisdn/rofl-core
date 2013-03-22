MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

sbin_PROGRAMS = etherswitch

etherswitch_SOURCES = \
	main.cc \
	etherswitch.cc \
	etherswitch.h
etherswitch_LDADD = -lpthread \
	-lrofl_control \
	-lrofl \
	-lrt 

AM_LDFLAGS = -static
