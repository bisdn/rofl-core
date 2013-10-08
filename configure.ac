AC_INIT(ROFL, 0.3.0, rofl@bisdn.de, rofl)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_GNU_SOURCE

AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LD
AM_PROG_CC_C_O
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AUTOCONF_ENV="SHELL=/bin/sh"

LT_INIT
AC_ENABLE_STATIC

# default flags
CFLAGS="-Werror -Wall $CFLAGS"
CXXFLAGS="-Werror -Wall $CXXFLAGS"
AC_DEFINE(__STDC_FORMAT_MACROS)

# Debug
m4_include([config/debug.m4])

# Application verbosity 
m4_include([config/verbose.m4])

# Check for profiling mode
m4_include([config/profiling.m4])

# Experimental
m4_include([config/experimental.m4])

# Flush
m4_include([config/flush.m4])

# No C++ (cross compilation in C enviornments)
m4_include([config/no_cpp.m4])

# Pipeline support
m4_include([config/pipeline.m4])

# Checking libs
#m4_include([config/libs.m4])

# Output files
AC_CONFIG_FILES([

	Makefile
	src/Makefile
	src/rofl/common/protocols/Makefile
	src/rofl/common/openflow/messages/Makefile
	src/rofl/common/openflow/extensions/Makefile
	src/rofl/common/openflow/experimental/Makefile
	src/rofl/common/openflow/experimental/actions/Makefile
	src/rofl/common/openflow/experimental/matches/Makefile
	src/rofl/common/openflow/Makefile
	src/rofl/common/utils/Makefile
	src/rofl/common/Makefile
	
	src/rofl/platform/Makefile
	src/rofl/platform/unix/Makefile
	
	src/rofl/controller/experimental/Makefile
	src/rofl/controller/control/Makefile
	src/rofl/controller/Makefile

	src/rofl/Makefile

        src/rofl/datapath/afa/Makefile
        src/rofl/datapath/afa/openflow/openflow1x/Makefile
	
	examples/Makefile
	examples/etherswitch/Makefile
	examples/queuetest/Makefile
	examples/rfc2889/Makefile
	examples/ofperftest/Makefile

	tools/Makefile
	tools/spray/Makefile


	test/Makefile
	test/unit/Makefile
	test/unit/test1/Makefile
	test/unit/cclock/Makefile
	test/unit/cmemory/Makefile
	test/unit/cpacket/Makefile
	test/unit/croflexp/Makefile
	test/unit/frames/Makefile
	])

# Doxygen (here to be the last Makefile) 
m4_include([config/doxygen.m4])

AC_OUTPUT

