
##
## Pipeline optimizations
## * Check inline platform functions
## * Check ROFL_PIPELINE_MAX_TIDS
##
##
#if test "$PIPELINE_SUPPORT" = "yes"; then
AM_COND_IF([PIPELINE_SUPPORT],[
	CFLAGS+=" -D__COMPILING_ROFL_PIPELINE__"
	CXXFLAGS+=" -D__COMPILING_ROFL_PIPELINE__"

	AC_MSG_CHECKING(whether to inline platform functions in ROFL-pipeline packet processing API)
enable_inline="no"

	#Detect option
	AC_ARG_WITH([pipeline-platform-funcs-inlined], AS_HELP_STRING([--with-pipeline-platform-funcs-inlined], [Inline platform functions in ROFL-pipeline packet processing API [default=no]]), with_pipeline_inline="yes", [])

	if test "$with_pipeline_inline" = "yes"; then
	
		AC_SUBST([ROFL_PIPELINE_INLINE_PP_PLATFORM_FUNCS], ["#define ROFL_PIPELINE_INLINE_PP_PLATFORM_FUNCS 1"])
		
		AC_SUBST([ROFL_PIPELINE_ABORT_IF_INLINED], ["#ifdef ROFL_PIPELINE_ABORT_IF_INLINED
	#error rofl-pipeline has been compiled with packet processing API functions inlined, but target does not support it(ROFL_PIPELINE_ABORT_IF_INLINED). Please recompile rofl-core without --with-pipeline-platform-funcs-inlined
#endif]")
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi

	#Pipeline thread IDs
	AC_MSG_CHECKING(the maximum number of threads/cpus that ROFL-pipeline packet processing API) 
	AC_ARG_WITH([pipeline-max-tids], AS_HELP_STRING([--with-pipeline-max-tids=num], [maximum number of threads/cpus that ROFL-pipeline packet processing API supports concurrently without locking. Supported values {2,4,8,16,32,64,128} [default=16]]), with_pipeline_max_tids="yes", [])
	
	MAX_TIDS=16

	#TODO: There is probably a better way to do this and that is still portable
	if test "$with_pipeline_max_tids" = "yes"; then
		if test "$withval" = "2"; then
			MAX_TIDS=$withval
		elif test "$withval" = "4"; then
			MAX_TIDS=$withval
		elif test "$withval" = "8"; then
			MAX_TIDS=$withval
		elif test "$withval" = "16"; then
			MAX_TIDS=$withval
		elif test "$withval" = "32"; then
			MAX_TIDS=$withval
		elif test "$withval" = "64"; then
			MAX_TIDS=$withval
		elif test "$withval" = "128"; then
			MAX_TIDS=$withval
		else
			AC_MSG_RESULT(ERROR)
		  	AC_ERROR([Invalid value for --with-pipeline-max-tids of '$withval'; supported values {2,4,8,16,32,64,128}])
		fi	
	fi
	AC_MSG_RESULT($MAX_TIDS)

	AC_SUBST([ROFL_PIPELINE_MAX_TIDS], ["#define ROFL_PIPELINE_MAX_TIDS $MAX_TIDS"])
	AC_SUBST([ROFL_PIPELINE_LOCKED_TID], ["#define ROFL_PIPELINE_LOCKED_TID 0"])
])
