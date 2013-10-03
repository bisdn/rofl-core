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
