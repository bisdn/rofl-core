AC_INIT(ROFL, 0.2.22, rofl@bisdn.de, rofl)
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
	CFLAGS="$CFLAGS -pg"
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

# build with POSIX malloc
AC_MSG_CHECKING(build with posix malloc)
posix_malloc_default="no"
AC_ARG_ENABLE(posix_malloc,
	AS_HELP_STRING([--enable-posix-malloc], [Build with posix malloc [default=no]])
		, , enable_posix_malloc=$posix_malloc_default)

if test "$enable_posix_malloc" = "yes"; then
	AM_CONDITIONAL(POSIX_MALLOC, [true])
	AC_DEFINE(POSIX_MALLOC)
	AC_MSG_RESULT(yes)
else
	AM_CONDITIONAL(POSIX_MALLOC, [false])
	AC_MSG_RESULT(no)
fi

#Check if C++ should not be compiled
AC_MSG_CHECKING(whether to build C code only)
AC_ARG_WITH([cplusplus],
	AS_HELP_STRING([--without-cplusplus], [Do not compile C++ code (handy for compiling datapath C-only code) [default=no]]),
without_cplusplus="yes", without_cplusplus="no")
if test "$without_cplusplus" = "yes"; then
	AM_CONDITIONAL(ONLY_C_CODE, [true])
	AC_MSG_RESULT(yes)
else
	AM_CONDITIONAL(ONLY_C_CODE, [false])
	AC_MSG_RESULT(no)
fi

#Pipeline support
AC_MSG_CHECKING(whether to build pipeline library)
AC_ARG_WITH([pipeline],
	AS_HELP_STRING([--without-pipeline], [Disable pipeline library compilation [default=no]]),
without_pipeline="yes", [])

if test "$without_pipeline" = "yes"; then
	if test "$without_cplusplus" = "yes"; then
		#Compile nothing???
		AC_MSG_ERROR(Invalid combination of flags --without-cplusplus and --without-pipeline)
	fi
	PIPELINE_SUPPORT="no"
else
	PIPELINE_SUPPORT="yes"
	AC_DEFINE(PIPELINE_SUPPORT)
fi

#Fancy message
AC_MSG_RESULT($PIPELINE_SUPPORT)

AM_CONDITIONAL(PIPELINE_SUPPORT, test $PIPELINE_SUPPORT = yes)
AM_COND_IF([PIPELINE_SUPPORT],[AC_CONFIG_FILES([
        src/rofl/datapath/pipeline/Makefile
        src/rofl/datapath/pipeline/common/Makefile
        src/rofl/datapath/pipeline/platform/Makefile
        src/rofl/datapath/pipeline/openflow/Makefile
        src/rofl/datapath/pipeline/openflow/openflow12/Makefile
        src/rofl/datapath/pipeline/openflow/openflow12/pipeline/Makefile
        src/rofl/datapath/pipeline/openflow/openflow12/pipeline/matching_algorithms/Makefile
        src/rofl/datapath/pipeline/util/Makefile

	test/rofl/datapath/pipeline/openflow/openflow12/pipeline/Makefile
	test/rofl/datapath/pipeline/openflow/openflow12/pipeline/ma/loop/Makefile
	test/rofl/datapath/pipeline/openflow/openflow12/pipeline/dynamic/Makefile
	test/rofl/datapath/pipeline/openflow/openflow12/pipeline/static/Makefile
])])

## pipeline
MATCHING_ALGORITHMS_DIR="src/rofl/datapath/pipeline/openflow/openflow12/pipeline/matching_algorithms"
AC_SUBST(MATCHING_ALGORITHMS_DIR)
MATCHING_ALGORITHMS="loop"
MATCHING_ALGORITHM_LIBS=""
MATCHING_ALGORITHM_LIBADD=""

# pipeline matching algorithms
AC_ARG_ENABLE(matching-algorithms,
	AS_HELP_STRING([--enable-matching-algorithms="list of matching algorithms"],
  		[Build support for the list of matching algorithms. The 
  		default is to build the loop matching algorithm.]),
[ case $enableval in
  yes)
  		# add all algorithms in $MATCHING_ALGORITHMS_DIR
        for dir in $srcdir/$MATCHING_ALGORITHMS_DIR/*; do
            algorithm="`basename $dir`"
            if test -d "$dir"; then
                MATCHING_ALGORITHMS="$MATCHING_ALGORITHMS $algorithm"
            fi
        done
        ;;
  no)
        ;;
  *)
        MATCHING_ALGORITHMS="`echo $enableval| sed -e 's/,/ /g;s/  */ /g'`"
        ;;
  esac
],[])
if test -n "$MATCHING_ALGORITHMS"; then
    # ensure that all algorithms a) exist and b) only include once.
    MATCHING_ALGORITHMS_FULL=$MATCHING_ALGORITHMS
    MATCHING_ALGORITHMS=
    for algorithm in $MATCHING_ALGORITHMS_FULL; do
        have_alg=`echo "$MATCHING_ALGORITHMS" | grep "$algorithm"`
        if test "$have_alg" != ""; then
            AC_MSG_NOTICE([Removing duplicate $algorithm from matching algorithms])
        elif test -d "$srcdir/$MATCHING_ALGORITHMS_DIR/$algorithm"; then
            MATCHING_ALGORITHMS="$MATCHING_ALGORITHMS $algorithm"
            MATCHING_ALGORITHM_LIBADD="$MATCHING_ALGORITHM_LIBADD librofl_pipeline_openflow12_pipeline_matching_algorithms_$algorithm.la"
        else
            MATCHING_ALGORITHMS="$MATCHING_ALGORITHMS $algorithm"
        	MATCHING_ALGORITHM_AS_LIBS="$MATCHING_ALGORITHM_AS_LIBS $algorithm"
            MATCHING_ALGORITHM_LIBS="MATCHING_ALGORITHM_LIBS -lrofl_pipeline_openflow12_pipeline_matching_algorithms_$algorithm"
        fi
    done
    AC_MSG_NOTICE([Matching algorithms built: $MATCHING_ALGORITHMS])
    AC_MSG_NOTICE([Matching algorithms as libs: $MATCHING_ALGORITHM_AS_LIBS])
fi

for algorithm in $MATCHING_ALGORITHMS; do
    HAVE_MATHING_ALGORITHM=HAVE_MA_`echo $algorithm | sed 's/\(.*\)/\U\1/'`
    AC_DEFINE_UNQUOTED($HAVE_MATHING_ALGORITHM, 1)
done

AC_SUBST(MATCHING_ALGORITHM_LIBADD)
AC_SUBST(MATCHING_ALGORITHM_LIBS)
AC_SUBST(MATCHING_ALGORITHMS)


#Check libs used (only in C++)
if test "$without_cplusplus" = "no"; then
	AC_CHECK_LIB(pthread, pthread_kill,,AC_MSG_ERROR([pthread library not found])) 
	AC_CHECK_LIB(rt,clock_gettime,,[AC_MSG_ERROR([rt library not found])]) 
	AC_CHECK_LIB(cli,cli_telnet_protocol,,[AC_MSG_ERROR([cli library not found])]) 
fi

#CHECK for doxygen
AC_CHECK_PROGS([DOXYGEN], [doxygen],)
if test -z "$DOXYGEN";then
	AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
	AM_CONDITIONAL(doc, [false])
else
	AC_MSG_CHECKING(whether to compile documentation)
	doc="no"
	AC_ARG_ENABLE(doc,
		AS_HELP_STRING([--doc], [compile documentation if doxygen is available [default=yes]])
			, doc="yes", )

	if test "$doc" = "yes"; then
		AM_CONDITIONAL(doc, [true])
		AC_CONFIG_FILES([doc/doxyfile.conf])	
		AC_CONFIG_FILES([doc/doxyfile-pipeline.conf])
		AC_CONFIG_FILES([doc/doxyfile-afa.conf])
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
		AM_CONDITIONAL(doc, [false])
	fi
fi

AC_CONFIG_FILES([

	Makefile
	src/Makefile
	src/rofl/common/config/Makefile
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
        src/rofl/datapath/afa/openflow/openflow12/Makefile
	
	examples/Makefile
	examples/etherswitch/Makefile
	examples/queuetest/Makefile
	examples/rfc2889/Makefile
	examples/ofperftest/Makefile


	test/Makefile
	test/unit/Makefile
	test/unit/test1/Makefile
	test/unit/cclock/Makefile
	test/unit/cmemory/Makefile
	test/unit/cpacket/Makefile
	test/unit/croflexp/Makefile
	test/unit/frames/Makefile

	doc/Makefile
	])
AC_OUTPUT

