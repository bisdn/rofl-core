MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = #test1 cmemory cpacket croflexp frames 

unittest_SOURCES= \
	unittest.cc \
	cmemorytest.cc \
	cmemorytest.h

#fipv6frametest.cc \
#fipv6frametest.h \
#ficmpv6frametest.cc \
#ficmpv6frametest.h \
#fl2tpv3frametest.cc \
#fl2tpv3frametest.h \
#coxmatchtest.cc \
#coxmatchtest.h \
#crofbasetest.cc \
#crofbasetest.h \
#coxmlisttest.cc \
#coxmlisttest.h \
#crofconntest.h crofconntest.cc

unittest_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

check_PROGRAMS=unittest #$(TEST_PROG)

TESTS=unittest
