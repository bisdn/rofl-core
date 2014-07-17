/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CADDRESS_H
#define CADDRESS_H 1

#include <arpa/inet.h>

#include <string>

#include "rofl/common/croflexception.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"

namespace rofl {

class eAddress 							: public RoflException {
public:
	eAddress(const std::string& __arg) : RoflException(__arg) {};
};

class eAddressInval 					: public eAddress {
public:
	eAddressInval(const std::string& __arg) : eAddress(__arg) {};
};


class caddress : public cmemory {
public:

	/**
	 *
	 */
	caddress(
			size_t size = 0) :
				rofl::cmemory(size) {};

	/**
	 *
	 */
	caddress(
			const rofl::cmemory& addr) :
				rofl::cmemory(addr) {};

	/**
	 *
	 */
	caddress(
			const caddress& addr) { *this = addr; };

	/**
	 *
	 */
	virtual
	~caddress() {};

	/**
	 *
	 */
	caddress&
	operator= (
			const caddress& addr) {
		if (this == &addr)
			return *this;
		cmemory::operator= (addr);
		return *this;
	};

	/**
	 *
	 */
	bool
	operator< (
			const caddress& addr) const { return cmemory::operator< (addr); };

	/**
	 *
	 */
	bool
	operator> (
			const caddress& addr) const { return cmemory::operator> (addr); };

	/**
	 *
	 */
	caddress
	operator& (
			const caddress& addr) const { return caddress(cmemory::operator& (addr)); };

	/**
	 *
	 */
	bool
	operator== (
			const caddress& addr) const { return cmemory::operator== (addr); };

	/**
	 *
	 */
	bool
	operator!= (
			 const caddress& addr) const { return cmemory::operator!= (addr); };

private:

	/**
	 * @brief	Shadow rofl::cmemory::resize()
	 */
	virtual uint8_t*
	resize(size_t size) { return cmemory::resize(size); };

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

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os,  const caddress& addr) {
		os << rofl::indent(0) << "<caddress >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( addr );
		return os;
	};
};



class caddress_ll : public caddress {
public:

	/**
	 *
	 */
	caddress_ll() :
		caddress(ETH_ADDR_LEN) {};

	/**
	 *
	 */
	caddress_ll(uint8_t* buf, size_t buflen) :
		caddress(ETH_ADDR_LEN) {
		if (buflen < ETH_ADDR_LEN)
			throw eAddressInval("caddress_ll::caddress_ll() buflen too short");
		cmemory::unpack(buf, buflen);
	};

	/**
	 *
	 */
	caddress_ll(
			const std::string& addr) :
					caddress(ETH_ADDR_LEN) {
		str2addr(addr);
	};

	/**
	 *
	 */
	caddress_ll(
			uint64_t mac) :
					caddress(ETH_ADDR_LEN) {
		set_mac(mac);
	};

	/**
	 *
	 */
	virtual
	~caddress_ll() {};

	/**
	 *
	 */
	caddress_ll(
			const caddress_ll& addr) { *this = addr; };

	/**
	 *
	 */
	caddress_ll&
	operator= (
			const caddress_ll& addr) {
		if (this == &addr)
			return *this;
		caddress::operator= (addr);
		return *this;
	};

	/**
	 *
	 */
	bool
	operator< (
			const caddress_ll& addr) const { return caddress::operator< (addr); };

	/**
	 *
	 */
	bool
	operator> (
			const caddress_ll& addr) const { return caddress::operator> (addr); };

	/**
	 *
	 */
	caddress_ll
	operator& (
			const caddress_ll& addr) const {
		caddress_ll addr_ll;
		addr_ll[0] = (*this)[0] & addr[0];
		addr_ll[1] = (*this)[1] & addr[1];
		addr_ll[2] = (*this)[2] & addr[2];
		addr_ll[3] = (*this)[3] & addr[3];
		addr_ll[4] = (*this)[4] & addr[4];
		addr_ll[5] = (*this)[5] & addr[5];
		return addr_ll;
	};

	/**
	 *
	 */
	bool
	operator== (
			const caddress_ll& addr) const { return caddress::operator== (addr); };

	/**
	 *
	 */
	bool
	operator!= (
			 const caddress_ll& addr) const { return caddress::operator!= (addr); };

public:

	/**
	 * @brief	Check for multicast bit in hardware address.
	 *
	 * @return true: hardware address has multicast bit set, false otherwise
	 */
	bool
	is_multicast() const;

	/**
	 * @brief	Check for broadcast hardware address.
	 *
	 * @return true: hardware address equals "ff:ff:ff:ff:ff:ff", false otherwise
	 */
	bool
	is_broadcast() const;

	/**
	 * @brief	Check for null hardware address.
	 *
	 * @return true: hardware address equals "00:00:00:00:00:00", false otherwise
	 */
	bool
	is_null() const;

	/**
	 * @brief	Return the MAC address as a uint64_t value. The MAC address will be transformed to host byte order and resides in the lower bytes.
	 *
	 * @return uint64_t mac
	 */
	uint64_t
	get_mac() const;

	/**
	 *
	 */
	void
	set_mac(uint64_t mac);

	/**
	 *
	 */
	std::string
	str() { return addr2str(); };

private:

	/**
	 *
	 */
	void
	str2addr(
			const std::string& addr);

	/**
	 *
	 */
	std::string
	addr2str() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, const caddress_ll& addr) {
		os << rofl::indent(0) << "<caddress_ll " << addr.addr2str() << " >" << std::endl;
#if 0
		rofl::indent i(2);
		os << dynamic_cast<const caddress&>( addr );
#endif
		return os;
	};

private:

	static const size_t ETH_ADDR_LEN = 6;
};

typedef caddress_ll cmacaddr;




class caddress_in : public caddress {
public:

	/**
	 *
	 */
	caddress_in(
			size_t size = 0) :
					caddress(size) {};

	/**
	 *
	 */
	virtual
	~caddress_in() {};

	/**
	 *
	 */
	caddress_in(
			const caddress_in& addr) { *this = addr; };

	/**
	 *
	 */
	caddress_in&
	operator= (
			const caddress_in& addr) {
		if (this == &addr)
			return *this;
		caddress::operator= (addr);
		return *this;
	};

	/**
	 *
	 */
	bool
	operator< (
			const caddress_in& addr) const { return caddress::operator< (addr); };

	/**
	 *
	 */
	bool
	operator> (
			const caddress_in& addr) const { return caddress::operator> (addr); };

	/**
	 *
	 */
	bool
	operator== (
			const caddress_in& addr) const { return caddress::operator== (addr); };

	/**
	 *
	 */
	bool
	operator!= (
			 const caddress_in& addr) const { return caddress::operator!= (addr); };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const caddress_in& addr) {
		os << rofl::indent(0) << "<caddress_in >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const caddress&>( addr );
		return os;
	};
};



class caddress_in4 : public caddress_in {
public:

	/**
	 *
	 */
	caddress_in4() :
		caddress_in(INET4_ADDR_LEN) {};

	/**
	 *
	 */
	caddress_in4(
			const std::string& addr) :
					caddress_in(INET4_ADDR_LEN) {
		str2addr(addr);
	};

	/**
	 *
	 */
	virtual
	~caddress_in4() {};

	/**
	 *
	 */
	caddress_in4(
			const caddress_in4& addr) { *this = addr; };

	/**
	 *
	 */
	caddress_in4&
	operator= (
			const caddress_in4& addr) {
		if (this == &addr)
			return *this;
		caddress_in::operator= (addr);
		return *this;
	};

	/**
	 *
	 */
	bool
	operator< (
			const caddress_in4& addr) const { return caddress_in::operator< (addr); };

	/**
	 *
	 */
	bool
	operator> (
			const caddress_in4& addr) const { return caddress_in::operator> (addr); };

	/**
	 *
	 */
	caddress_in4
	operator& (
			const caddress_in4& addr) const {
		caddress_in4 addr_in4;
		for (unsigned int i = 0; i < INET4_ADDR_LEN; i++) {
			addr_in4[i] = (*this)[i] & addr[i];
		}
		return addr_in4;
	};

	/**
	 *
	 */
	bool
	operator== (
			const caddress_in4& addr) const { return caddress_in::operator== (addr); };

	/**
	 *
	 */
	bool
	operator!= (
			 const caddress_in4& addr) const { return caddress_in::operator!= (addr); };

public:

	/**
	 *
	 */
	uint32_t
	get_addr_nbo() const {
		uint8_t* ptr = somem();
		return *((uint32_t*)ptr);
	};

	/**
	 *
	 */
	void
	set_addr_nbo(uint32_t addr) {
		uint8_t* ptr = somem();
		*ptr = addr;
	};


	/**
	 *
	 */
	uint32_t
	get_addr_hbo() const {
		uint8_t* ptr = somem();
		return be32toh(*((uint32_t*)ptr));
	};

	/**
	 *
	 */
	void
	set_addr_hbo(uint32_t addr) {
		uint8_t* ptr = somem();
		*ptr = htobe32(addr);
	};

	/**
	 *
	 */
	std::string
	str() { return addr2str(); };

private:

	/**
	 *
	 */
	void
	str2addr(
			const std::string& addr);

	/**
	 *
	 */
	std::string
	addr2str() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, const caddress_in4& addr) {
		os << rofl::indent(0) << "<caddress_in4 " << addr.addr2str() << " >" << std::endl;
#if 0
		rofl::indent i(2);
		os << dynamic_cast<const caddress&>( addr );
#endif
		return os;
	};

private:

	static const size_t INET4_ADDR_LEN = 4;
};



class caddress_in6 : public caddress_in {
public:

	/**
	 *
	 */
	caddress_in6() :
		caddress_in(INET6_ADDR_LEN) {};

	/**
	 *
	 */
	caddress_in6(
			const std::string& addr) :
					caddress_in(INET6_ADDR_LEN) {
		str2addr(addr);
	};

	/**
	 *
	 */
	virtual
	~caddress_in6() {};

	/**
	 *
	 */
	caddress_in6(
			const caddress_in6& addr) { *this = addr; };

	/**
	 *
	 */
	caddress_in6&
	operator= (
			const caddress_in6& addr) {
		if (this == &addr)
			return *this;
		caddress_in::operator= (addr);
		return *this;
	};

	/**
	 *
	 */
	bool
	operator< (
			const caddress_in6& addr) const { return caddress_in::operator< (addr); };

	/**
	 *
	 */
	bool
	operator> (
			const caddress_in6& addr) const { return caddress_in::operator> (addr); };

	/**
	 *
	 */
	caddress_in6
	operator& (
			const caddress_in6& addr) const {
		caddress_in6 addr_in6;
		for (unsigned int i = 0; i < INET6_ADDR_LEN; i++) {
			addr_in6[i] = (*this)[i] & addr[i];
		}
		return addr_in6;
	};

	/**
	 *
	 */
	bool
	operator== (
			const caddress_in6& addr) const { return caddress_in::operator== (addr); };

	/**
	 *
	 */
	bool
	operator!= (
			 const caddress_in6& addr) const { return caddress_in::operator!= (addr); };

	/**
	 *
	 */
	std::string
	str() { return addr2str(); };

private:

	/**
	 *
	 */
	void
	str2addr(
			const std::string& addr);

	/**
	 *
	 */
	std::string
	addr2str() const;

private:

	friend std::ostream&
	operator<< (std::ostream& os, const caddress_in6& addr) {
		os << rofl::indent(0) << "<caddress_in6 " << addr.addr2str() << " >" << std::endl;
#if 0
		rofl::indent i(2);
		os << dynamic_cast<const caddress&>( addr );
#endif
		return os;
	};

private:

	static const size_t INET6_ADDR_LEN = 16;
};


}; // end of namespace rofl

#endif
