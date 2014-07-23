/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * caddrinfo.h
 *
 *  Created on: 12.06.2014
 *      Author: andreas
 */

#ifndef CADDRINFO_H_
#define CADDRINFO_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <string>

#include "rofl/common/csockaddr.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class eAddrInfoBase	: public RoflException {
public:
	eAddrInfoBase(const std::string& __arg) : RoflException(__arg) {};
};

class eAddrInfoNotFound	: public eAddrInfoBase {
public:
	eAddrInfoNotFound(const std::string& __arg) : eAddrInfoBase(__arg) {};
};


class caddrinfo {
public:

	/**
	 *
	 */
	caddrinfo();

	/**
	 *
	 */
	caddrinfo(
			int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const csockaddr& ai_addr);

	/**
	 *
	 */
	~caddrinfo() {};

	/**
	 *
	 */
	caddrinfo(
			const caddrinfo& ai) { *this = ai; };

	/**
	 *
	 */
	caddrinfo&
	operator= (
			const caddrinfo& ai) {
		if (this == &ai)
			return *this;
		ai_flags	= ai.ai_flags;
		ai_family	= ai.ai_family;
		ai_socktype	= ai.ai_socktype;
		ai_protocol	= ai.ai_protocol;
		ai_addr		= ai.ai_addr;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_ai_flags(int ai_flags) { this->ai_flags = ai_flags; };

	/**
	 *
	 */
	int
	get_ai_flags() const { return ai_flags; };

	/**
	 *
	 */
	void
	set_ai_family(int ai_family) { this->ai_family = ai_family; };

	/**
	 *
	 */
	int
	get_ai_family() const { return ai_family; };

	/**
	 *
	 */
	void
	set_ai_socktype(int ai_socktype) { this->ai_socktype = ai_socktype; };

	/**
	 *
	 */
	int
	get_ai_socktype() const { return ai_socktype; };

	/**
	 *
	 */
	void
	set_ai_protocol(int ai_protocol) { this->ai_protocol = ai_protocol; };

	/**
	 *
	 */
	int
	get_ai_protocol() const { return ai_protocol; };

	/**
	 *
	 */
	csockaddr&
	set_ai_addr() { return ai_addr; };

	/**
	 *
	 */
	const csockaddr&
	get_ai_addr() const { return ai_addr; };

public:

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	pack(struct addrinfo* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(struct addrinfo* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const caddrinfo& ai) {
		os << rofl::indent(0) << "<caddrinfo >" << std::endl;
		os << rofl::indent(2) << "<ai_flags: " 		<< ai.get_ai_flags() 	<< " >" << std::endl;
		os << rofl::indent(2) << "<ai_family: " 	<< ai.get_ai_family() 	<< " >" << std::endl;
		os << rofl::indent(2) << "<ai_socktype: " 	<< ai.get_ai_socktype() << " >" << std::endl;
		os << rofl::indent(2) << "<ai_protocol: " 	<< ai.get_ai_protocol() << " >" << std::endl;
		os << rofl::indent(2) << "<ai_flags: " 		<< ai.get_ai_flags() 	<< " >" << std::endl;
		os << rofl::indent(2) << "<ai_addr: >"		<< std::endl;
		rofl::indent i(4);
		os << ai.get_ai_addr();
		return os;
	};

private:

	int			ai_flags;
	int			ai_family;
	int			ai_socktype;
	int			ai_protocol;
	csockaddr	ai_addr;
};

}; // end of namespace rofl

#endif /* CADDRINFO_H_ */
