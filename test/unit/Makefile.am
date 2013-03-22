MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = test1 cclock cmemory cpacket croflexp frames 

unittest_SOURCES= unittest.cc fipv6frametest.cc fipv6frametest.h ficmpv6frametest.cc ficmpv6frametest.h coxmatchtest.cc coxmatchtest.h crofbasetest.cc crofbasetest.h
unittest_LDADD= -lrofl -lcppunit

check_PROGRAMS=unittest #$(TEST_PROG)

TESTS=unittest
