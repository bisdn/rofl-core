AC_INIT(ROFL, 0.2.15, rofl@bisdn.de, rofl)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_GNU_SOURCE

AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LD
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AUTOCONF_ENV="SHELL=/bin/sh"

LT_INIT

AC_ENABLE_STATIC

# default flags
CFLAGS="-Werror -Wall $CFLAGS"
CXXFLAGS="-Werror -Wall $CXXFLAGS"

# Check for debug mode - MUST BE THE FIRST CHECK
AC_MSG_CHECKING(whether to enable debug mode)
debug_default="no"
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug], [turn on debug mode [default=no]])
		, , enable_debug=$debug_default)

if test "$enable_debug" = "yes"; then
	CFLAGS="$CFLAGS -g -O0"
	CXXFLAGS="$CXXFLAGS -g -O0 -fno-inline"
	AC_DEFINE(DEBUG)
	AC_MSG_RESULT(yes)
else
	CFLAGS="$CFLAGS -O3" #--compiler-options -fno-strict-aliasing --compiler-options -fno-inline"
	CXXFLAGS="$CXXFLAGS -O3" #-fomit-frame-pointer"
	AC_DEFINE(NDEBUG) 
	AC_MSG_RESULT(no)
fi

# Check for profiling mode
AC_MSG_CHECKING(whether to enable profiling mode)
profile_default="no"
AC_ARG_ENABLE(profile,
	AS_HELP_STRING([--enable-profile], [turn on profile mode [default=no]])
		, , enable_profile=$profile_default)
if test "$enable_profile" = "yes"; then
	CFLAGS="$( echo $CFLAGS | sed s/-fomit-frame-pointer//g )"
	CXXFLAGS="$( echo $CXXFLAGS | sed s/-fomit-frame-pointer//g )"
	CXXFLAGS="$CXXFLAGS -pg"
	LDFLAGS="$LDFLAGS -pg"
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

#Check for compilation of experimental code
AC_MSG_CHECKING(whether to compile experimental code)
experimental_default="no"
AC_ARG_ENABLE(experimental,
	AS_HELP_STRING([--enable-experimental], [Compile experimental code [default=no]])
		, , enable_experimental=$experimental_default)

if test "$enable_experimental" = "yes"; then
	AM_CONDITIONAL(EXPERIMENTAL, [true])
	AC_MSG_RESULT(yes)
else
	AM_CONDITIONAL(EXPERIMENTAL, [false])
	AC_MSG_RESULT(no)
	
fi

# Check for flush_always for logging
AC_MSG_CHECKING(whether to always flush logging messages)
flush_always_default="no"
AC_ARG_ENABLE(flush_always,
	AS_HELP_STRING([--enable-flush-always], [Always flush log entries [default=no]])
		, , enable_flush_always=$flush_always_default)

if test "$enable_flush_always" = "yes"; then
	AM_CONDITIONAL(FLUSH_ALWAYS, [true])
	AC_DEFINE(FLUSH_ALWAYS)
	AC_MSG_RESULT(yes)
else
	AM_CONDITIONAL(FLUSH_ALWAYS, [false])
	AC_MSG_RESULT(no)
fi

AC_DEFINE(__STDC_FORMAT_MACROS)

#Check libs used
AC_CHECK_LIB(pthread, pthread_kill,,AC_MSG_ERROR([pthread library not found])) 
AC_CHECK_LIB(rt,clock_gettime,,[AC_MSG_ERROR([rt library not found])]) 
AC_CHECK_LIB(cli,cli_telnet_protocol,,[AC_MSG_ERROR([cli library not found])]) 

#CHECK for doxygen
AC_CHECK_PROGS([DOXYGEN], [doxygen],)
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AC_CONFIG_FILES([

	Makefile
	src/Makefile
	src/rofl/platform/Makefile
	src/rofl/platform/unix/Makefile
	src/rofl/experimental/Makefile
	src/rofl/common/protocols/Makefile
	src/rofl/common/controller/Makefile
	src/rofl/common/openflow/extensions/Makefile
	src/rofl/common/openflow/Makefile
	src/rofl/common/Makefile
	src/rofl/Makefile
	src/rofl/config/Makefile

	test/Makefile
	test/unit/Makefile
	test/unit/test1/Makefile
	test/unit/cclock/Makefile
	test/unit/cmemory/Makefile
	test/unit/cpacket/Makefile
	test/unit/croflexp/Makefile

	doc/Makefile
	])
AC_OUTPUT
