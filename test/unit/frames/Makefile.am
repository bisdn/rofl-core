MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS = -std=gnu++11
frames_SOURCES= frames.cc
frames_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

checksums_SOURCES= checksums.cc 
checksums_LDADD=$(top_builddir)/src/rofl/librofl.la -lcppunit

check_PROGRAMS=frames checksums#$(TEST_PROG)

TESTS=frames checksums
