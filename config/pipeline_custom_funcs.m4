AC_MSG_CHECKING(whether to build pipeline with custom functions)
disable_inlines="no"

AC_ARG_WITH([pipeline-custom-funcs],
	AS_HELP_STRING([--without-pipeline-custom-funcs], [Disable pipeline custom functions [default=no]]),
disable_custom_funcs="yes", [])

if test "$disable_custom_funcs" = "yes"; then
	AC_MSG_NOTICE([Not using any of the inlined function headers])
	AM_CONDITIONAL(LOCK_INLINE, false)
	AM_CONDITIONAL(ATOMIC_OPS_INLINE, false)
	AM_CONDITIONAL(LIKELY_CUSTOM, false)
	
else
	AC_MSG_NOTICE([Using the existing inlined function headers])
	
	lock_inline_file=0
	AC_CHECK_FILE([$srcdir/src/rofl/datapath/pipeline/platform/lock_inline.h], [lock_inline_file=1], [])
	AM_CONDITIONAL(LOCK_INLINE, test $lock_inline_file -eq 1)
	if test $lock_inline_file -eq 1 ; then
		AC_MSG_NOTICE([Compiling with locking functions inlined])
		AC_DEFINE(LOCK_INLINE_HDR)
	fi

	atomic_ops_inline_file=0
	AC_CHECK_FILE([$srcdir/src/rofl/datapath/pipeline/platform/atomic_ops_inline.h], [atomic_ops_inline_file=1], [])
	AM_CONDITIONAL(ATOMIC_OPS_INLINE, test $atomic_ops_inline_file -eq 1)
	if test $atomic_ops_inline_file -eq 1 ; then
		AC_MSG_NOTICE([Compiling with atomic operations inlined])
		AC_DEFINE(ATOMIC_OPS_INLINE_HDR)
	fi
	
	custom_likely_file=0
	AC_CHECK_FILE([$srcdir/src/rofl/datapath/pipeline/platform/likely_custom.h], [custom_likely_file=1], [])
	AM_CONDITIONAL(LIKELY_CUSTOM, test $custom_likely_file -eq 1)
	if test $custom_likely_file -eq 1 ; then
		AC_MSG_NOTICE([Compiling with custom likely file])
		AC_DEFINE(LIKELY_CUSTOM_HDR)
	fi
fi
