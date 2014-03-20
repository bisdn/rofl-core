MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = openflow protocols

export INCLUDES += -I$(abs_srcdir)/../src/

unittest_SOURCES= \
        unittest.cc \
        csegmsg_test.cc \
        csegmsg_test.h 

unittest_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

check_PROGRAMS=unittest 

TESTS=unittest

