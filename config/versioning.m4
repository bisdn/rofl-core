# Set application version based on the git version

#Default
ROFL_VERSION="$PACKAGE_VERSION" #Unknown (no GIT repository detected)"
FILE_VERSION=`cat ../VERSION`

#Since AC_INIT caches VERSION; force an autogen.sh
if test "$ROFL_VERSION" != "$FILE_VERSION"; 
then
	AC_MSG_ERROR("ROFL version file has been updated($ROFL_VERSION => $FILE_VERSION). Please regenerate Autoconf state by calling autogen.sh again.")
fi

AC_CHECK_PROG(ff_git,git,yes,no)

#Normalize
ROFL_VERSION_NORMALIZED=`echo $ROFL_VERSION | sed s/dev//g | sed s/RC.*//g | tr -d v`

#Substs
AC_SUBST([ROFL_VERSION], ["$ROFL_VERSION"])
AC_SUBST([ROFL_VERSION_NORMALIZED], ["$ROFL_VERSION_NORMALIZED"])

AC_MSG_CHECKING([the build version])
AC_MSG_RESULT([$ROFL_VERSION ($ROFL_VERSION_NORMALIZED)])

AC_MSG_CHECKING([the build number])
if test $ff_git = no
then
	AC_MSG_RESULT([git not found!])
else

	if test -d $srcdir/.git ; then
		#Try to retrieve the build number
		_ROFL_GIT_BUILD=`git log -1 --pretty=%H`
		_ROFL_GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`
		_ROFL_GIT_DESCRIBE=`git describe --abbrev=40`

		AC_SUBST([ROFL_BUILD], ["$_ROFL_GIT_BUILD"])
		AC_SUBST([ROFL_BRANCH], ["$_ROFL_GIT_BRANCH"])
		AC_SUBST([ROFL_DESCRIBE], ["$_ROFL_GIT_DESCRIBE"])

	fi

	AC_MSG_RESULT([$_ROFL_GIT_BUILD])
fi
