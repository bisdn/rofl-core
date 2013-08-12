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
#include <stdio.h>
#include <strings.h>
#include <stdarg.h>

namespace rofl
{

// base class for entire error class hierarchy
class cerror
{
public:


	int 				n_errno;
	std::string 		desc;


public:
	cerror() :
		n_errno(0)
	{
		n_errno = errno;
		desc = std::string(strerror(errno));
	};
	cerror(std::string const& desc) :
		n_errno(0),
		desc(desc)
	{};
	friend std::ostream& operator<< (std::ostream& os, cerror& e)
	{
		os << "exception errno:" << e.n_errno << " (" << strerror(e.n_errno) << ")";
		return os;
	};
};

class eOutOFMemory 			: public cerror {}; //< out of mem error
class eNotImplemented 		: public cerror {
public:
	eNotImplemented(std::string desc = std::string("")) : cerror(desc) {};
}; //< oops, fix me exception :D
class eInternalError 		: public cerror {}; //< some internal error occured
class eDebug 				: public cerror {};
class eInval				: public cerror {};
class eTooShort				: public cerror {};

}; // end of namespace

#endif



