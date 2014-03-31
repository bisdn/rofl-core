#Check inline platform functions
if test "$PIPELINE_SUPPORT" = "yes"; then

	CFLAGS+=" -D__COMPILING_ROFL_PIPELINE__"
	CXXFLAGS+=" -D__COMPILING_ROFL_PIPELINE__"

	AC_MSG_CHECKING(whether to inline platform functions in ROFL-pipeline packet processing API)
enable_inline="no"

	#Detect option
	AC_ARG_WITH([pipeline-platform-funcs-inlined], AS_HELP_STRING([--with-pipeline-platform-funcs-inlined], [Inline platform functions in ROFL-pipeline packet processing API [default=no]]), with_pipeline_inline="yes", [])

	if test "$with_pipeline_inline" = "yes"; then
	
		#Set to inline pipeline funcs
#		echo "#define ROFL_PIPELINE_INLINE_PP_PLATFORM_FUNCS 1" >> $ROFL_PIPELINE_CONFIG
		
#		echo "" >> $ROFL_PIPELINE_CONFIG
#		echo "" >> $ROFL_PIPELINE_CONFIG
		
#		echo "#ifdef ROFL_PIPELINE_ABORT_IF_INLINED" >> $ROFL_PIPELINE_CONFIG
#		echo "    #error rofl-pipeline has been compiled with packet processing API functions inlined, but target does not support it(ROFL_PIPELINE_ABORT_IF_INLINED). Please recompile rofl-core without --with-pipeline-platform-funcs-inlined" >> $ROFL_PIPELINE_CONFIG
#		echo "#endif" >> $ROFL_PIPELINE_CONFIG
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
	fi
else
	#Set presence flag
#	echo "#define ROFL_PIPELINE_NOT_PRESENT 1" >> $ROFL_PIPELINE_CONFIG
	AC_MSG_RESULT(no)
fi

