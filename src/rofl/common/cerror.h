/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CERROR_H
#define CERROR_H

#include <set>
#include <string>
#include <iostream>

#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <strings.h>
#include <stdarg.h>
#ifdef __cplusplus
}
#endif

// base class for entire error class hierarchy
class cerror {
public:
	cerror(std::string desc = std::string("")) :
		desc(desc) {};
	std::string desc;
};

class eOutOFMemory : public cerror {}; //< out of mem error
class eNotImplemented : public cerror {}; //< oops, fix me exception :D
class eInternalError : public cerror {}; //< some internal error occured
class eDebug : public cerror {};

#endif



