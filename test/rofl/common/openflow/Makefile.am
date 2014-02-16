MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = messages

unittest_SOURCES= \
	unittest.cc \
	coftablefeatureprop_test.cc \
	coftablefeatureprop_test.h \
	coftablefeatureprops_test.cc \
	coftablefeatureprops_test.h \
	coftablefeatures_test.cc \
	coftablefeatures_test.h \
	cofhelloelemstest.cc \
	cofhelloelemstest.h \
	cofhelloelemversionbitmaptest.cc \
	cofhelloelemversionbitmaptest.h 

unittest_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

check_PROGRAMS=unittest 

TESTS=unittest
