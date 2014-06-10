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
	csockaddr(unsigned short int sa_family) :
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
			uint32_t addr_nbo = 0, uint16_t portno = 0) :
					csockaddr(AF_INET), saddr(4), portno(portno) {
		set_addr_nbo(addr_nbo);
	};

	/**
	 *
	 */
	csockaddr_in4(
			uint8_t* buf, size_t buflen, uint16_t portno = 0) :
					csockaddr(AF_INET), saddr(4), portno(portno) {
		if (buflen < saddr.length())
			throw eSockAddrInval("csockaddr_in4() buflen too short");
		saddr.assign(buf, buflen);
	};

	/**
	 *
	 */
	csockaddr_in4(
			const std::string& addr, uint16_t portno = 0) :
					csockaddr(AF_INET), saddr(4), portno(portno) {
		set_addr_nbo(str2addr_nbo(addr)),
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
	operator=(const csockaddr_in4& sockaddr) {
		if (this == &sockaddr)
			return *this;
		csockaddr::operator=(sockaddr);
		saddr 	= sockaddr.saddr;
		portno 	= sockaddr.portno;
		return *this;
	};

public:

	/**
	 *
	 */
	const rofl::cmemory&
	get_addr() const { return saddr; };

	/**
	 *
	 */
	rofl::cmemory&
	set_addr() { return saddr; };

	/**
	 *
	 */
	void
	set_addr(const rofl::cmemory& saddr) { this->saddr = saddr; };

	/**
	 *
	 */
	uint32_t
	get_addr_nbo() const {
		uint32_t addr;
		memcpy((uint8_t*)&addr, saddr.somem(), saddr.length());
		return addr;
	};

	/**
	 *
	 */
	void
	set_addr_nbo(uint32_t addr) {
		memcpy(saddr.somem(), (uint8_t*)&addr, saddr.length());
	}

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
			struct sockaddr* sa, size_t salen);

	/**
	 *
	 */
	virtual void
	unpack(
			struct sockaddr* sa, size_t salen);

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

	/**
	 * @brief	Maps a uint32_t in network byte order to an IP address string ddd.ddd.ddd.ddd
	 */
	std::string
	addr2str_nbo(uint32_t addr) const;

	/**
	 * @brief	Maps an IP address string ddd.ddd.ddd.ddd to a uint32-t in network byte order
	 */
	uint32_t
	str2addr_nbo(const std::string& addr) const;

private:

	rofl::cmemory	saddr; 		// network byte order
	uint16_t 		portno; 	// host byte order
};



class csockaddr_in6: public csockaddr {
public:

	/**
	 *
	 */
	csockaddr_in6(
			uint8_t* buf, size_t buflen, uint16_t portno = 0) :
					csockaddr(AF_INET6), saddr(16), portno(portno) {
		if (buflen < saddr.length())
			throw eSockAddrInval("csockaddr_in6() buflen too short");
		saddr.assign(buf, buflen);
	};

	/**
	 *
	 */
	csockaddr_in6(
			const std::string& addr, uint16_t portno = 0) :
					csockaddr(AF_INET6), saddr(16), portno(portno) {
		saddr = str2addr_nbo(addr);
	};

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
		csockaddr::operator=(sockaddr);
		saddr 	= sockaddr.saddr;
		portno 	= sockaddr.portno;
		return *this;
	};

public:

	/**
	 *
	 */
	const rofl::cmemory&
	get_addr() const { return saddr; };

	/**
	 *
	 */
	rofl::cmemory&
	set_addr() { return saddr; };

	/**
	 *
	 */
	void
	set_addr(const rofl::cmemory& saddr) { this->saddr = saddr; };

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
			struct sockaddr* sa, size_t salen);

	/**
	 *
	 */
	virtual void
	unpack(
			struct sockaddr* sa, size_t salen);

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

	/**
	 *
	 */
	std::string
	addr2str_nbo(const rofl::cmemory& addr) const;

	/**
	 *
	 */
	rofl::cmemory
	str2addr_nbo(const std::string& addr) const;

private:

	rofl::cmemory	saddr; 		// network byte order
	uint16_t 		portno; 	// host byte order
};

}; // end of namespace rofl

#endif /* CSOCKADDR_H_ */
