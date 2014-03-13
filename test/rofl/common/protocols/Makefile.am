MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

unittest_SOURCES= \
	unittest.cc \
	clldpattr_test.cc \
	clldpattr_test.h \
	clldpattrs_test.cc \
	clldpattrs_test.h \
	clldpmsg_test.cc \
	clldpmsg_test.h

unittest_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

check_PROGRAMS=unittest 

TESTS=unittest
