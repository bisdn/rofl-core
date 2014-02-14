MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

unittest_SOURCES= \
	unittest.cc \
	coftablefeatureprop_test.cc \
	coftablefeatureprop_test.h

unittest_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

check_PROGRAMS=unittest 

TESTS=unittest
