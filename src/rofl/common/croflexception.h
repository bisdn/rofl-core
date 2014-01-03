/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CROFLEXCEPTION_H
#define CROFLEXCEPTION_H

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
class RoflException {
public:

	std::string 		desc;

public:
	RoflException(std::string const& desc = std::string("")) :
		desc(desc)
	{};
	friend std::ostream& operator<< (std::ostream& os, RoflException& e) {
		os << "<ROFL exception " << e.desc << ">";
		return os;
	};
};

class eSysCall : public RoflException {
public:
	std::string	syscall;
	int			n_err;
	std::string	s_err;
public:
	eSysCall(std::string const& syscall = std::string("")) :
		syscall(syscall),
		n_err(errno),
		s_err(strerror(errno))
	{};
public:
	friend std::ostream& operator<< (std::ostream& os, eSysCall const& e) {
		os << "<eSysCall syscall:" << e.syscall << " errno: " << e.n_err << " (" << e.s_err << ") >";
		return os;
	};
};

class eOutOFMemory 			: public RoflException {}; //< out of mem error
class eNotImplemented 		: public RoflException {
public:
	eNotImplemented(std::string desc = std::string("")) : RoflException(desc) {};
}; //< oops, fix me exception :D
class eInternalError 		: public RoflException {}; //< some internal error occured
class eDebug 				: public RoflException {};
class eInval				: public RoflException {};
class eTooShort				: public RoflException {};

}; // end of namespace

#endif



