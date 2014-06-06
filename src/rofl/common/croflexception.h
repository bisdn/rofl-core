/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CROFLEXCEPTION_H
#define CROFLEXCEPTION_H

#include <assert.h>

#include <set>
#include <string>
#include <iostream>

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <stdarg.h>
#include <stdexcept>

namespace rofl
{

/*
 *  base class for entire error class hierarchy in rofl
 */
class RoflException : public std::runtime_error {
public:
	RoflException(const std::string& __arg) :
		std::runtime_error(__arg) {};
	virtual ~RoflException() throw() {};
};



class eSysCall : public RoflException {
public:
	eSysCall(std::string const& syscall = std::string("")) :
		RoflException(syscall), n_err(errno), s_err(strerror(errno)) {};
	virtual ~eSysCall() throw() {};
public:
	friend std::ostream& operator<< (std::ostream& os, eSysCall const& e) {
		os << "<eSysCall syscall:" << e.what() << " errno: " << e.n_err << " (" << e.s_err << ") >";
		return os;
	};

private:
	int			n_err;
	std::string	s_err;
};



class eNotImplemented : public RoflException {
public:
	eNotImplemented(const std::string& __arg = std::string("")) :
		RoflException(__arg) {};
	virtual ~eNotImplemented() throw() {};
};



class eInval : public RoflException {
public:
	eInval(const std::string& __arg) : RoflException(__arg) {};
	virtual ~eInval() throw() {};
};



class eTooShort				: public RoflException {};



class eBadVersion : public RoflException {
public:
	eBadVersion(const std::string& __arg) : RoflException(__arg) {
		std::cerr << "BAD-WOLF" << std::endl;
		assert(0 == 1);
	};
};

class eBadSyntax					: public RoflException {};
class eBadSyntaxTooShort			: public eBadSyntax {};


//class eOutOFMemory 			: public RoflException {}; //< out of mem error
//class eInternalError 		: public RoflException {}; //< some internal error occured
//class eDebug 				: public RoflException {};
//class eNotConnected			: public RoflException {};

}; // end of namespace

#endif



