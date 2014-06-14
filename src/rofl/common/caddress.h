/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CADDRESS_H
#define CADDRESS_H

#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <endian.h>
#include <sys/socket.h>
#include <netdb.h>
#ifndef htobe16
	#include "endian_conversion.h"
#endif

#include "cvastring.h"
#include "croflexception.h"
#include "cmemory.h"
#include "rofl/datapath/pipeline/common/large_types.h"

namespace rofl
{

/* error classes */
class eAddress 							: public RoflException {}; // base class caddress related errors
class eAddressIoctlFailed 				: public eAddress {};
class eAddressSocketFailed 				: public eAddress {};
class eAddressSocketFailedNoPermission 	: public eAddressSocketFailed {};
class eAddressInval 					: public eAddress {};


/**
 * @class	caddress
 * @brief	Auxiliary class for managing sockaddr structures for various address families.
 *
 * This class encapsulates Unix socket address structures. Currently, it
 * supports IPv4, IPv6, LinkLayer, and Unix.
 *
 */
class caddress :
	public cmemory
{
#define AF_INET4_SIZE	4
#define AF_INET6_SIZE	16
public:

		/**
		 * @union 	addr_addru
		 * @brief	A union containing a pointer to the sockaddr structure.
		 *
		 * Supports various pointer types (generic, AF_INET, AF_INET6, AF_UNIX, AF_PACKET).
		 */
		union {
			struct sockaddr*		addru_saddr;
			struct sockaddr_in* 	addru_s4addr;
			struct sockaddr_in6	*	addru_s6addr;
			struct sockaddr_un*		addru_suaddr;
			struct sockaddr_ll*		addru_sladdr;
		} addr_addru;

#define ca_saddr		addr_addru.addru_saddr		// generic sockaddr
#define ca_s4addr		addr_addru.addru_s4addr		// sockaddr_in
#define ca_s6addr		addr_addru.addru_s6addr		// sockaddr_in6
#define ca_suaddr		addr_addru.addru_suaddr		// sockaddr_un
#define ca_sladdr		addr_addru.addru_sladdr		// sockaddr_ll

		socklen_t salen; /**< actual length of struct sockaddr managed by this caddress instance */

private:

		/*
		 * this needs a refactoring in the next round: use struct addrinfo for storing all parameters
		 */
		int 				domain;
		int					sock_type;
		int					protocol;
		std::string 		node;		// for getaddrinfo
		std::string			service;	// for getaddrinfo

	std::string info;	// info string

public:


	/**
	 *
	 * @param node
	 * @param service
	 * @param preferred_family
	 * @param preferred_socktype
	 * @param preferred_protocol
	 */
	caddress(
			std::string const& node,
			std::string const& service,
			int ai_flags = 0,
			int preferred_family = 0,
			int preferred_socktype = 0,
			int preferred_protocol = 0);


	/**
	 * @brief	Constructor for creating an empty address. Allocates maximum memory required for any struct sockaddr (currently sockaddr_un).
	 *
	 * @param size size of memory area for storing struct sockaddr
	 */
	caddress(int af = AF_UNSPEC);



	/**
	 * @brief	Constructor for caddress instances of address family AF_PACKET.
	 *
	 * Allocates sufficient space for hosting a struct sockaddr_ll and fills the structure.
	 * See the man page packet(7) for details.
	 *
	 * @param af Address family for this socket address (here: PF_PACKET)
	 * @param protocol Protocol type in use (default: ETH_P_ALL)
	 * @param devname Device name for this address (default "eth0")
	 * @param hatype Hardware address type (default: ARPHRD_ETHER)
	 * @param pkttype Packet type (default: PACKET_HOST)
	 * @param addr link layer address memory area
	 * @param halen length of address memory area
	 */
	caddress(
			u_int16_t protocol /*= ETH_P_ALL*/,
			std::string devname /*= std::string("eth0")*/,
			u_int16_t hatype = ARPHRD_ETHER,
			u_int8_t pkttype = PACKET_HOST,
			const char* addr = NULL,
			size_t halen = 0);



	/**
	 * @brief	Constructor for caddress instances of address family AF_INET, AF_INET6, and AF_UNIX.
	 *
	 * Allocates sufficient space for hosting a struct sockaddr_in/sockaddr_in6 and
	 * fills the structure with the specified address in ASCII form "127.0.0.1".
	 *
	 * @param af Address family for this socket address (here: AF_INET, AF_INET6, AF_UNIX)
	 * @param astr ASCII-encoded address string, e.g. "127.0.0.1" or a FQDN
	 * @param port port value for a transport address
	 * @exception eAddressInval is thrown, when an unsupported address family is specified
	 */
	caddress(
			int af,
			const char* astr,
			u_int16_t port = 0) throw (eAddressInval);




	/**
	 * @brief	Creates a caddress instance from a struct sockaddr_in6.
	 *
	 * @param sa pointer to struct sockaddr_in6
	 * @param salen length of buffer
	 */
	caddress(
			struct sockaddr_in6 *sa,
			size_t salen) throw (eAddressInval);


	/**
	 * @brief	Creates a caddress instance from a struct sockaddr_in.
	 *
	 * @param sa pointer to struct sockaddr_in
	 * @param salen length of buffer	 */
	caddress(
			struct sockaddr_in *sa,
			size_t salen) throw (eAddressInval);



	/**
	 * @brief	Copy constructor.
	 *
	 * @param ca reference to the caddress instance to be copied.
	 */
	caddress(
			caddress const& ca);



	/**
	 * @brief	Destructor.
	 *
	 */
	~caddress();


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
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t* buf, size_t buflen);


public:

	/**
	 *
	 */
	std::string&
	set_node() { return node; };

	/**
	 *
	 */
	std::string const&
	get_node() const { return node; };

	/**
	 *
	 */
	std::string&
	set_service() { return service; };

	/**
	 *
	 */
	std::string const&
	get_service() const { return service; };

	/**
	 *
	 */
	void
	set_domain(int domain) { this->domain = domain; };

	/**
	 *
	 */
	int
	get_domain() { return domain; };

	/**
	 *
	 */
	void
	set_sock_type(int sock_type) { this->sock_type = sock_type; };

	/**
	 *
	 */
	int
	get_sock_type() { return sock_type; };

	/**
	 *
	 */
	void
	set_protocol(int protocol) { this->protocol = protocol; };

	/**
	 *
	 */
	int
	get_protocol() { return protocol; };


public:


	/**
	 * @brief	Returns a C-string with the address in ASCII representation ("AF_INET/127.0.0.1:6633").
	 *
	 * @return C-string
	 */
	const char*
	c_str();



	/**
	 * @brief	Returns a C-string with the address in ASCII representation ("127.0.0.1").
	 *
	 * @return C-string
	 */
	const char*
	addr_c_str();


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, caddress const& addr)
	{
		uint16_t port = 0;

		switch (addr.ca_saddr->sa_family) {
		case AF_INET: {
			port = be16toh(addr.ca_s4addr->sin_port);
		} break;
		case AF_INET6: {
			port = be16toh(addr.ca_s6addr->sin6_port);
		} break;
		}

		caddress t_addr(addr);
		os << std::string(t_addr.addr_c_str()) << " port:" << (unsigned int)port << " ";
#if 0
		caddress t_addr(addr);
		os << "caddress{"
				<< "af=" << t_addr.c_str() << " "
				<< "}";
#endif
		return os;
	};


public:


	/**
	 * @name Operators
	 */

	/**@{*/

	/**
	 * @brief	Returns address family of this instance.
	 *
	 * @return AF_INET, AF_INET6, AF_UNSPEC, ...
	 */
	int
	get_family() const;


	/**
	 * @brief	Assignment operator.
	 *
	 * @param ca reference to caddress instance to be assigned to *this
	 */
	caddress&
	operator=(
			caddress const& ca);



	/**
	 * @brief	Less than operator.
	 *
	 * This compares the two memory regions.
	 *
	 * @param ca reference to caddress instance for comparison
	 */
	bool
	operator<(
			caddress const& ca) const;



	/**
	 * @brief	Greater than operator.
	 *
	 * This compares the two memory regions.
	 *
	 * @param ca reference to caddress instance for comparison
	 */
	bool
	operator>(
			caddress const& ca) const;



	/**
	 * @brief	AND operator.
	 *
	 * @param mask reference to caddress instance to be used for ANDing with this caddress
	 */
	caddress
	operator& (
			caddress const& mask) const;



	/**
	 * @brief	Equals operator.
	 *
	 * @param ca reference to caddress instance for comparison
	 */
	bool
	operator== (
			caddress const& ca) const;



	/**
	 * @brief	Unequals operator.
	 *
	 * @param ca reference to caddress instance for comparison
	 */
	bool
	operator!= (
			caddress const& ca) const;


	/**
	 * @brief	Index operator granting direct access to address bytes
	 *
	 * @param index index defines the byte to be returned as reference
	 */
	uint8_t&
	operator[] (
			unsigned int index);

	/**@}*/


public:

	/**
	 * @name Helper methods
	 */

	/**@{*/

	/**
	 * @brief	Checks for an address family AF_INET.
	 *
	 * @return true: address family is AF_INET, false otherwise
	 */
	bool
	is_af_inet() const;



	/**
	 * @brief	Checks for an address family AF_INET6.
	 *
	 * @return true: address family is AF_INET6, false otherwise
	 */
	bool
	is_af_inet6() const;



	/**
	 * @brief	Checks for an address family AF_UNIX.
	 *
	 * @return true: address family is AF_UNIX, false otherwise
	 */
	bool
	is_af_unix() const;



	/**
	 * @brief	Checks for an address family AF_PACKET.
	 *
	 * @return true: address family is AF_PACKET, false otherwise
	 */
	bool
	is_af_packet() const;


	/**
	 * @brief	Checks for multicast address.
	 */
	bool
	is_multicast() const;


	/**
	 * @brief	Checks for broadcast address.
	 */
	bool
	is_broadcast() const;


	/**@}*/

#if 0
	/**
	 * Global method as output operator for class caddress().
	 * Puts the caddress 'address' into an output stream identified
	 * by 'os'.
	 * @param os Output stream to put the address to.
	 * @param ca The caddress instance to dump its content to the output stream.
	 * @return The output stream for chaining operators<<()
	 */
	friend std::ostream &
	operator<<(std::ostream& os, caddress& ca)
	{
		char address[256];
		bzero(address, sizeof(address));
		switch (ca.ca_saddr->sa_family) {
		case 0:
			os << "caddress(EMPTY)";
			break;
		case AF_INET:
			os << "caddress(AF_INET/" << inet_ntop(AF_INET, &(ca.s4addr->sin_addr), address, 256)
			<< ":" << ntohs(ca.s4addr->sin_port) << "/" << ca.salen << ")";
			break;
		case AF_INET6:
			os << "caddress(AF_INET6/" << inet_ntop(AF_INET6, &(ca.ca_s6addr->sin6_addr), address, 256)
			<< ":" << ntohs(ca.ca_s6addr->sin6_port) << "/" << ca.salen << ")";
			break;
		case AF_UNIX:
			os << "caddress(AF_UNIX/" << ca.ca_suaddr->sun_path << "/" << ca.salen << ")";
			break;
		case AF_PACKET:
			os << "caddress(AF_PACKET/TODO: implement!)";
			break;
		}
		return os;
	};
#endif


protected:

	/**
	 * Helper method: gethostbyname() replacement.
	 * Retrieves for an ASCII description the network address in network
	 * byte order. Used by the class constructors. Supports IPv6.
	 * @param af The address family
	 * @param astr The ASCII encoded string containing an address or FQDN
	 */
	void pton(int af, const char* astr);

public: // static

	static void
	test();
	
	uint32_t
	get_ipv4_addr();
	
	void
	set_ipv4_addr(uint32_t addr);
	
	uint128__t
	get_ipv6_addr();
	
	void
	set_ipv6_addr(uint128__t addr);

private:

	/**
	 *
	 */
	virtual uint8_t*
	resize(size_t len);

};

}; // end of namespace

#endif
