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
	coftables_test.cc \
	coftables_test.h \
	cofhelloelemstest.cc \
	cofhelloelemstest.h \
	cofhelloelemversionbitmaptest.cc \
	cofhelloelemversionbitmaptest.h \
	cofflowstatsarray_test.cc \
	cofflowstatsarray_test.h \
	cofgroupdescstatsarray_test.cc \
	cofgroupdescstatsarray_test.h \
	cofgroupstatsarray_test.cc \
	cofgroupstatsarray_test.h \
	cofbucketcounter_test.cc \
	cofbucketcounter_test.h \
	cofbucketcounters_test.cc \
	cofbucketcounters_test.h \
	cofportstatsarray_test.cc \
	cofportstatsarray_test.h \
	cofqueuestatsarray_test.cc \
	cofqueuestatsarray_test.h \
	coftablestatsarray_test.cc \
	coftablestatsarray_test.h

unittest_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

check_PROGRAMS=unittest 

TESTS=unittest
