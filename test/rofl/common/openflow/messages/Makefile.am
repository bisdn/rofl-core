MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

unittest_SOURCES= \
	unittest.cc \
	cofmsgtablefeatures_test.cc \
	cofmsgtablefeatures_test.h \
	cofmsgmetermod_test.cc \
	cofmsgmetermod_test.h

unittest_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

check_PROGRAMS=unittest 

TESTS=unittest
