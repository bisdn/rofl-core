MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow protocols

export INCLUDES += -I$(abs_srcdir)/../src/

unittest_SOURCES= \
        unittest.cc \
        csegmsg_test.cc \
        csegmsg_test.h \
	cparam_test.cc \
	cparam_test.h \
	cparams_test.cc \
	cparams_test.h \
	caddress_test.cc \
	caddress_test.h \
	csocket_test.cc \
	csocket_test.h \
	ctimerid_test.cc \
	ctimerid_test.h \
	ctimespec_test.cc \
	ctimespec_test.h

unittest_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

check_PROGRAMS=unittest 

TESTS=unittest

