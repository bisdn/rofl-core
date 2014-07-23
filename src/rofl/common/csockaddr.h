/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * csockaddr.h
 *
 *  Created on: 10.06.2014
 *      Author: andreas
 */

#ifndef CSOCKADDR_H_
#define CSOCKADDR_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>

#include "rofl/common/caddress.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class eSockAddrBase		: public RoflException {
public:
	eSockAddrBase(const std::string& __arg) : RoflException(__arg) {};
};

class eSockAddrInval	: public eSockAddrBase {
public:
	eSockAddrInval(const std::string& __arg) : eSockAddrBase(__arg) {};
};




class csockaddr : public rofl::cmemory {
public:

    union {
    		uint8_t*				addru_mem;
            struct sockaddr*        addru_saddr;
            struct sockaddr_in*     addru_s4addr;
            struct sockaddr_in6*    addru_s6addr;
    } addr_addru;

#define ca_mem					addr_addru.addru_mem
#define ca_saddr                addr_addru.addru_saddr          // generic sockaddr
#define ca_s4addr               addr_addru.addru_s4addr         // sockaddr_in
#define ca_s6addr               addr_addru.addru_s6addr         // sockaddr_in6

    socklen_t salen;

public:

    /**
     *
     */
    csockaddr(
    		int family, const std::string& addr, uint16_t port_no);

    /**
     *
     */
    csockaddr(
    		const caddress_in4& addr, uint16_t port_no);

    /**
     *
     */
    csockaddr(
    		const caddress_in6& addr, uint16_t port_no);

	/**
	 *
	 */
	csockaddr() :
			rofl::cmemory(sizeof(struct sockaddr_in6)),
			salen(0) {
		ca_mem = cmemory::somem();
	};

	/**
	 *
	 */
	virtual
	~csockaddr() {};

	/**
	 *
	 */
	csockaddr(
			const csockaddr& sockaddr) { *this = sockaddr; };

	/**
	 *
	 */
	csockaddr&
	operator= (
			const csockaddr& sockaddr) {
		if (this == &sockaddr)
			return *this;
		cmemory::operator= (sockaddr);
		ca_mem = cmemory::somem();
		salen = sockaddr.salen;
		return *this;
	};

public:

	/**
	 *
	 */
	int
	get_family() const;

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			struct sockaddr* sa, size_t salen);

	/**
	 *
	 */
	virtual void
	unpack(
			struct sockaddr* sa, size_t salen);

public:

	friend std::ostream&
	operator<<(std::ostream& os, const csockaddr& sockaddr) {
		os << rofl::indent(0) << "<csockaddr >" << std::endl;
		switch (sockaddr.ca_saddr->sa_family) {
		case AF_INET: {
			caddress_in4 addr; addr.set_addr_nbo(sockaddr.ca_s4addr->sin_addr.s_addr);
			rofl::indent i(2);
			os << addr;
			os << rofl::indent(0) << "<portno: " << (unsigned int)be16toh(sockaddr.ca_s4addr->sin_port) << " >" << std::endl;
		} break;
		case AF_INET6: {
			caddress_in6 addr; addr.unpack(sockaddr.ca_s6addr->sin6_addr.s6_addr, 16);
			rofl::indent i(2);
			os << addr;
			os << rofl::indent(0) << "<portno: " << (unsigned int)be16toh(sockaddr.ca_s6addr->sin6_port) << " >" << std::endl;
		} break;
		}
		return os;
	};

};


}; // end of namespace rofl

#endif /* CSOCKADDR_H_ */
