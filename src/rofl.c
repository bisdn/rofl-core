/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl.h"

#ifndef ROFL_BUILD
	//No git detected
	const char ROFL_VERSION[]=ROFL_VERSION_;
	const char ROFL_BUILD_NUM[]="";
	const char ROFL_BUILD_BRANCH[]="";
	const char ROFL_BUILD_DESCRIBE[]="";
#else
	const char ROFL_VERSION[]=ROFL_VERSION_;
	const char ROFL_BUILD_NUM[]=ROFL_BUILD;
	const char ROFL_BUILD_BRANCH[]=ROFL_BRANCH;
	const char ROFL_BUILD_DESCRIBE[]=ROFL_DESCRIBE;

#endif

//C++ extern C
ROFL_BEGIN_DECLS

// autoconf AC_CHECK_LIB helper function as C-declaration
void librofl_is_present(void) {};

//C++ extern C
ROFL_END_DECLS


