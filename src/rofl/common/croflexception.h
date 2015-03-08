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
	RoflException(const std::string& __arg = std::string("")) :
		std::runtime_error(__arg) {};
	virtual ~RoflException() throw() {};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const RoflException& e) {
		os << "<RoflException: " << e.what() << " >";
		return os;
	};
};

class eSysCall : public RoflException {
public:
	eSysCall(std::string const& syscall = std::string("unknown")) :
		RoflException("syscall: "+syscall+
				" error: "+std::string(strerror(errno))+" "+
				std::string(__FILE__)+std::string(":")+std::string(__func__)),
		n_err(errno), s_err(strerror(errno)) {};
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
	eNotImplemented(
			const std::string& __arg = std::string("eNotImplemented")) :
				RoflException(__arg) {};
	virtual ~eNotImplemented() throw() {};
};

class eInval : public RoflException {
public:
	eInval(
			const std::string& __arg = std::string("eInval")) :
				RoflException(__arg) {};
	virtual ~eInval() throw() {};
};

class eBadVersion : public RoflException {
public:
	eBadVersion(
			const std::string& __arg = std::string("eBadVersion")) :
				RoflException(__arg) {
#ifndef NDEBUG
		std::cerr << "BAD-WOLF" << std::endl;
//		assert(0 == 1);
#endif
	};
};

class eBadSyntax					: public RoflException {
public:
	eBadSyntax(
			const std::string& __arg = std::string("eBadSyntax")) :
				RoflException(__arg) {};
};

class eBadSyntaxTooShort			: public eBadSyntax {
public:
	eBadSyntaxTooShort(
			const std::string& __arg = std::string("eBadSyntaxTooShort")) :
				eBadSyntax(__arg) {};
};

}; // end of namespace

#endif



