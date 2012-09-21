MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libconfig.la

library_includedir=$(includedir)/rofl/config

library_include_HEADERS = ccli.h \
	cconfigsyslog.h 

libconfig_la_SOURCES = ccli.cc \
	ccli.h \
	cconfigsyslog.cc \
	cconfigsyslog.h
	
libconfig_la_LIBADD = -lcli
