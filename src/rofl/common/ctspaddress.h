/*
 * ctspaddress.h
 *
 *  Created on: 24.10.2014
 *      Author: andreas
 */

#ifndef CTSPADDRESS_H_
#define CTSPADDRESS_H_

#include <inttypes.h>
#include <rofl/common/caddress.h>
#include <rofl/common/logging.h>

namespace rofl {
namespace common {

class ctspaddress {
public:

	/**
	 *
	 */
	ctspaddress(uint16_t port = 0) :
		port(port) {};

	/**
	 *
	 */
	virtual
	~ctspaddress() {};

	/**
	 *
	 */
	ctspaddress(const ctspaddress& addr) { *this = addr; };

	/**
	 *
	 */
	ctspaddress&
	operator= (const ctspaddress& addr) {
		if (this == &addr)
			return *this;
		port = addr.port;
		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (const ctspaddress& addr) const { return (port == addr.port); };

	/**
	 *
	 */
	bool
	operator< (const ctspaddress& addr) const { return (port < addr.port); };

public:

	/**
	 *
	 */
	uint16_t
	get_port() const { return port; };

	/**
	 *
	 */
	void
	set_port(uint16_t port) { this->port = port; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const ctspaddress& addr) {
		os << rofl::indent(0) << "<ctspaddress port:" << (int)addr.get_port() << " >" << std::endl;
		return os;
	};

protected:

	uint16_t port;
};

class ctspaddress_in4 : public ctspaddress {
public:

	/**
	 *
	 */
	ctspaddress_in4() {};

	/**
	 *
	 */
	ctspaddress_in4(const rofl::caddress_in4& addr, uint16_t port) :
		ctspaddress(port), addr(addr) {};

	/**
	 *
	 */
	virtual
	~ctspaddress_in4() {};

	/**
	 *
	 */
	ctspaddress_in4(const ctspaddress_in4& addr) { *this = addr; };

	/**
	 *
	 */
	ctspaddress_in4&
	operator= (const ctspaddress_in4& addr) {
		if (this == &addr)
			return *this;
		ctspaddress::operator= (addr);
		this->addr = addr.addr;
		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (const ctspaddress_in4& addr) const {
		return ((this->addr == addr.addr) && ctspaddress::operator== (addr));
	};

	/**
	 *
	 */
	bool
	operator< (const ctspaddress_in4& addr) const {
		return ((this->addr < addr.addr) || ctspaddress::operator< (addr));
	};

public:

	/**
	 *
	 */
	const rofl::caddress_in4&
	get_addr() const { return addr; };

	/**
	 *
	 */
	void
	set_addr(rofl::caddress_in4& addr) { this->addr = addr; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const ctspaddress_in4& addr) {
		os << rofl::indent(0) << "<ctspaddress_in4 addr:" << addr.get_addr().str()
				<< " port:" << (int)addr.get_port() << " >" << std::endl;
		return os;
	};

private:

	rofl::caddress_in4 addr;
};

class ctspaddress_in6 : public ctspaddress {
public:

	/**
	 *
	 */
	ctspaddress_in6() {};

	/**
	 *
	 */
	ctspaddress_in6(const rofl::caddress_in6& addr, uint16_t port) :
		ctspaddress(port), addr(addr) {};

	/**
	 *
	 */
	virtual
	~ctspaddress_in6() {};

	/**
	 *
	 */
	ctspaddress_in6(const ctspaddress_in6& addr) { *this = addr; };

	/**
	 *
	 */
	ctspaddress_in6&
	operator= (const ctspaddress_in6& addr) {
		if (this == &addr)
			return *this;
		ctspaddress::operator= (addr);
		this->addr = addr.addr;
		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (const ctspaddress_in6& addr) const {
		return ((this->addr == addr.addr) && ctspaddress::operator== (addr));
	};

	/**
	 *
	 */
	bool
	operator< (const ctspaddress_in6& addr) const {
		return ((this->addr < addr.addr) || ctspaddress::operator< (addr));
	};

public:

	/**
	 *
	 */
	const rofl::caddress_in6&
	get_addr() const { return addr; };

	/**
	 *
	 */
	void
	set_addr(rofl::caddress_in6& addr) { this->addr = addr; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const ctspaddress_in6& addr) {
		os << rofl::indent(0) << "<ctspaddress_in6 addr:" << addr.get_addr().str()
				<< " port:" << (int)addr.get_port() << " >" << std::endl;
		return os;
	};

private:

	rofl::caddress_in6 addr;
};

}; // end of namespace common
}; // end of namespace rofl

#endif /* CTSPADDRESS_H_ */
