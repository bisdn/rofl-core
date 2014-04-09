#Detecting crypto and ssl
ssl_detected="yes"
 
AC_ARG_WITH(ssl,
	AS_HELP_STRING([--without-ssl], [Turn off tls (openssl) even if detected [default=no]])
		, ssl_detected="no",)
     
if test "$ssl_detected" = "yes"; then
	AC_CHECK_LIB(ssl, SSL_library_init, , ssl_detected="no")
	AC_CHECK_LIB(crypto, ERR_get_error, , ssl_detected="no")

	AC_MSG_CHECKING(for availabilty of openssl and crypto libraries(SSL/TLS))
	if test "$ssl_detected" = "yes"; then
		AC_MSG_RESULT(found)
		AC_SUBST([ROFL_HAVE_OPENSSL], ["#define ROFL_HAVE_OPENSSL 1"])
	else
		AC_MSG_RESULT(not found)
	fi
	AC_MSG_CHECKING(whether to compile SSL/TLS support)
	AC_MSG_RESULT(yes)
else
	AC_MSG_CHECKING(whether to compile SSL/TLS support)
	#SSL explictely disabled 
	AC_MSG_RESULT(SSL explictely disabled!)
fi 

AM_CONDITIONAL([HAVE_OPENSSL],  [test "$ssl_detected" = yes])
AM_CONDITIONAL([ROFL_HAVE_OPENSSL],  [test "$ssl_detected" = yes])
