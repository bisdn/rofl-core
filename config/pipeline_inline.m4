#Check inline platform functions
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
])
