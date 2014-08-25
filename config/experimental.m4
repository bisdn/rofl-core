#Check for compilation of experimental code
AC_MSG_CHECKING(whether to compile experimental code)
experimental_default="no"
AC_ARG_ENABLE(experimental,
	AS_HELP_STRING([--enable-experimental], [Compile experimental code [default=no]])
		, , enable_experimental=$experimental_default)

if test "$enable_experimental" = "yes"; then
	AC_SUBST([ROFL_EXPERIMENTAL], ["#define ROFL_EXPERIMENTAL 1"])
	AC_MSG_RESULT(yes)
else
	AC_SUBST([ROFL_EXPERIMENTAL], ["//Compiled without experimental support"])
	AC_MSG_RESULT(no)
fi

#Set automake conditional
AM_CONDITIONAL(EXPERIMENTAL, test "$enable_experimental" = yes)
