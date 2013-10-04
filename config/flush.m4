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

