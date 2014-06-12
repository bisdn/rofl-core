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

class csockaddr {
public:

	/**
	 *
	 */
	csockaddr(unsigned short int sa_family = 0) :
			sa_family(sa_family) {
	};

	/**
	 *
	 */
	virtual ~csockaddr() {
	};

	/**
	 *
	 */
	csockaddr(const csockaddr& sockaddr) {
		*this = sockaddr;
	};

	/**
	 *
	 */
	csockaddr&
	operator=(const csockaddr& sockaddr) {
		if (this == &sockaddr)
			return *this;
		sa_family = sockaddr.sa_family;
		return *this;
	};

public:

	/**
	 *
	 */
	unsigned short int
	get_sa_family() const { return sa_family; };

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
		os << rofl::indent(0) << "<csockaddr sa-family:"
				<< (unsigned int) sockaddr.get_sa_family() << " >" << std::endl;
		return os;
	};

private:

	/**
	 *
	 */
	void
	set_sa_family(unsigned short int sa_family) { this->sa_family = sa_family; };

private:

	unsigned short int sa_family;
};



class csockaddr_in4: public csockaddr {
public:

	/**
	 *
	 */
	csockaddr_in4(
			rofl::caddress_in4 saddr = rofl::caddress_in4(), uint16_t portno = 0) :
					csockaddr(AF_INET), saddr(saddr), portno(portno) {};

	/**
	 *
	 */
	csockaddr_in4(
			const std::string& addr, uint16_t portno = 0) :
					csockaddr(AF_INET), saddr(addr), portno(portno) {};

	/**
	 *
	 */
	virtual
	~csockaddr_in4() {};

	/**
	 *
	 */
	csockaddr_in4(
			const csockaddr_in4& sockaddr) { *this = sockaddr; };

	/**
	 *
	 */
	csockaddr_in4&
	operator=(const csockaddr_in4& sockaddr) {
		if (this == &sockaddr)
			return *this;
		csockaddr::operator= (sockaddr);
		saddr 	= sockaddr.saddr;
		portno 	= sockaddr.portno;
		return *this;
	};

public:

	/**
	 *
	 */
	const rofl::caddress_in4&
	get_addr() const { return saddr; };

	/**
	 *
	 */
	rofl::caddress_in4&
	set_addr() { return saddr; };

	/**
	 *
	 */
	void
	set_addr(const rofl::caddress_in4& saddr) { this->saddr = saddr; };

	/**
	 *
	 */
	uint16_t
	get_port_no() const { return portno; };

	/**
	 *
	 */
	void
	set_port_no(uint16_t portno) { this->portno = portno; };

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
			struct sockaddr_in* sa, size_t salen);

	/**
	 *
	 */
	virtual void
	unpack(
			struct sockaddr_in* sa, size_t salen);

public:

	friend std::ostream&
	operator<<(std::ostream& os, const csockaddr_in4& sockaddr) {
		os << rofl::indent(0) << "<csockaddr_in4 "
				<< "saddr:" << sockaddr.get_addr() << " "
				<< "portno:" << sockaddr.get_port_no() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const csockaddr&>(sockaddr);
		return os;
	};

private:

	caddress_in4	saddr;
	uint16_t 		portno; 	// host byte order
};




class csockaddr_in6: public csockaddr {
public:

	/**
	 *
	 */
	csockaddr_in6(
			rofl::caddress_in6 saddr = rofl::caddress_in6(), uint16_t portno = 0) :
					csockaddr(AF_INET6), saddr(saddr), portno(portno) {};

	/**
	 *
	 */
	csockaddr_in6(
			const std::string& addr, uint16_t portno = 0) :
					csockaddr(AF_INET6), saddr(addr), portno(portno) {};

	/**
	 *
	 */
	virtual
	~csockaddr_in6() {};

	/**
	 *
	 */
	csockaddr_in6(
			const csockaddr_in6& sockaddr) { *this = sockaddr; };

	/**
	 *
	 */
	csockaddr_in6&
	operator=(const csockaddr_in6& sockaddr) {
		if (this == &sockaddr)
			return *this;
		csockaddr::operator= (sockaddr);
		saddr 	= sockaddr.saddr;
		portno 	= sockaddr.portno;
		return *this;
	};

public:

	/**
	 *
	 */
	const rofl::caddress_in6&
	get_addr() const { return saddr; };

	/**
	 *
	 */
	rofl::caddress_in6&
	set_addr() { return saddr; };

	/**
	 *
	 */
	void
	set_addr(const rofl::caddress_in6& saddr) { this->saddr = saddr; };

	/**
	 *
	 */
	uint16_t
	get_port_no() const { return portno; };

	/**
	 *
	 */
	void
	set_port_no(uint16_t portno) { this->portno = portno; };

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
			struct sockaddr_in6* sa, size_t salen);

	/**
	 *
	 */
	virtual void
	unpack(
			struct sockaddr_in6* sa, size_t salen);

public:

	friend std::ostream&
	operator<<(std::ostream& os, const csockaddr_in6& sockaddr) {
		os << rofl::indent(0) << "<csockaddr_in6 "
				<< "saddr:" << sockaddr.get_addr() << " "
				<< "portno:" << sockaddr.get_port_no() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const csockaddr&>(sockaddr);
		return os;
	};

private:

	caddress_in6	saddr;
	uint16_t 		portno; 	// host byte order
};




}; // end of namespace rofl

#endif /* CSOCKADDR_H_ */
