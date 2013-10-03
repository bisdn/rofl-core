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
