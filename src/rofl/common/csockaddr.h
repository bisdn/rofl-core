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

#include <iostream>

#include "rofl/common/logging.h"

namespace rofl {

class csockaddr {
public:

	/**
	 *
	 */
	csockaddr(
			unsigned short int sa_family) :
					sa_family(sa_family) {};

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
		sa_family = sockaddr.sa_family;
		return *this;
	};

public:

	/**
	 *
	 */
	unsigned short int
	get_sa_family() const { return sa_family; };

	/**
	 *
	 */
	void
	set_sa_family(unsigned short int sa_family) { this->sa_family = sa_family; };

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
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const csockaddr& sockaddr) {
		os << rofl::indent(0) << "<csockaddr sa-family:"
				<< (unsigned int)sockaddr.get_sa_family() << " >" << std::endl;
		return os;
	};

private:

	unsigned short int sa_family;
};

class csockaddr_in4 : public csockaddr {
public:

	/**
	 *
	 */
	csockaddr_in4(uint32_t addr = 0) :
		csockaddr(AF_INET) {
		this->addr.sin_addr.s_addr = htobe32(addr);
	};

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
	operator= (
			const csockaddr_in4& sockaddr) {
		if (this == &sockaddr)
			return *this;
		csockaddr::operator= (sockaddr);
		addr = sockaddr.addr;
		return *this;
	};

public:

	/**
	 *
	 */
	uint32_t
	get_addr_hbo() const { return addr; };

	/**
	 *
	 */
	void
	set_addr_hbo(uint32_t addr) { this->addr = addr; };

	/**
	 *
	 */
	uint32_t
	get_addr_nbo() const { return htobe32(addr); };

	/**
	 *
	 */
	void
	set_addr_nbo(uint32_t addr) { this->addr = be32toh(addr); };

	/**
	 *
	 */
	std::string
	get_addr_s() const;

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
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const csockaddr_in4& sockaddr) {
		os << rofl::indent(0) << "<csockaddr_in4 addr:" << sockaddr.get_addr_s() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const csockaddr&>( sockaddr );
		return os;
	};

private:

	struct sockaddr_in addr;
};

}; // end of namespace rofl

#endif /* CSOCKADDR_H_ */
