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
class cerror {
public:

	std::string 		desc;

public:
	cerror(std::string const& desc = std::string("")) :
		desc(desc)
	{};
	friend std::ostream& operator<< (std::ostream& os, cerror& e) {
		os << "<ROFL exception " << e.desc << ">";
		return os;
	};
};

class eSysCall : public cerror {
public:
	int			n_err;
	std::string	s_err;
public:
	eSysCall() :
		n_err(errno),
		s_err(strerror(errno))
	{};
public:
	friend std::ostream& operator<< (std::ostream& os, eSysCall const& e) {
		os << "<eSysCall errno: " << e.n_err << " (" << e.s_err << ") >";
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



