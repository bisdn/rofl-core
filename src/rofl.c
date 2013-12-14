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
