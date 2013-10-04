#Check libs used (only in C++)
if test "$without_cplusplus" = "no"; then
	AC_CHECK_LIB(pthread, pthread_kill,,AC_MSG_ERROR([pthread library not found])) 
	AC_CHECK_LIB(rt,clock_gettime,,[AC_MSG_ERROR([rt library not found])]) 
	AC_CHECK_LIB(cli,cli_telnet_protocol,,[AC_MSG_ERROR([cli library not found])]) 
fi
