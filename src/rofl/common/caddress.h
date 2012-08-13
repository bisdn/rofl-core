/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CADDRESS_H
#define CADDRESS_H

#include <list>
#include <string>
#include <iostream>
#include <sstream>

#ifdef __cplusplus

extern "C" {
#endif
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

#ifndef htobe16
#include "endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "cvastring.h"
#include "cerror.h"
#include "cmemory.h"
#include "rofl/platform/unix/csyslog.h"


/* error classes */
class eAddress : public cerror {}; // base class caddress related errors
class eAddressIoctlFailed : public eAddress {};
class eAddressSocketFailed : public eAddress {};
class eAddressSocketFailedNoPermission : public eAddressSocketFailed {};
class eAddressInval : public eAddress {};


/**
 * Auxiliary class for encapsulating address structs.
 *
 * This class encapsulates socket address structures. Currently
 * supports IPv4, IPv6, LinkLayer, Unix
 */
class caddress :
	public cmemory
{
public:

		union {
			struct sockaddr*		addru_saddr;
			struct sockaddr_in* 	addru_s4addr;
			struct sockaddr_in6	*	addru_s6addr;
			struct sockaddr_un*		addru_suaddr;
			struct sockaddr_ll*		addru_sladdr;
		} addr_addru;

#define saddr		addr_addru.addru_saddr		// generic sockaddr
#define s4addr		addr_addru.addru_s4addr		// sockaddr_in
#define s6addr		addr_addru.addru_s6addr		// sockaddr_in6
#define suaddr		addr_addru.addru_suaddr		// sockaddr_un
#define sladdr		addr_addru.addru_sladdr		// sockaddr_ll

		socklen_t salen; //< maximum length of allocated memory area hosting saddr

private:

	std::string info;	// info string

public:

	/**
	 * Constructor.
	 * Allocates sufficient space for hosting any struct sockaddr_XX.
	 */
	caddress(
			size_t size = sizeof(struct sockaddr_un));

	/**
	 * Constructor for PF_PACKET struct sockaddr_ll.
	 * Allocates sufficient space for hosting a struct sockaddr_ll.
	 * See the man page packet(7) for details.
	 * @param af Address family for this socket address (here: PF_PACKET)
	 * @param protocol Protocol type in use (default: ETH_P_ALL)
	 * @param devname Device name for this address (default "eth0")
	 * @param hatype Hardware address type (default: ARPHRD_ETHER)
	 * @param pkttype Packet type (default: PACKET_HOST)
	 * @param addr link layer address memory area
	 * @param halen length of address memory area
	 */
	caddress(
			int af,
			u_int16_t protocol = ETH_P_ALL,
			std::string devname = std::string("eth0"),
			u_int16_t hatype = ARPHRD_ETHER,
			u_int8_t pkttype = PACKET_HOST,
			const char* addr = NULL,
			size_t halen = 0) throw (eAddressInval);

	/**
	 * Constructor for PF_INET/PF_INET6.
	 * Allocates sufficient space for hosting a struct sockaddr_in/sockaddr_in6.
	 * @param af Address family for this socket address (here: PF_INET, PF_INET6, PF_UNIX)
	 * @param astr ASCII-encoded address string, e.g. "10.0.0.1" or a FQDN
	 * @param port Port value for a transport address
	 */
	caddress(
			int af,
			const char* astr,
			u_int16_t port = 0) throw (eAddressInval);

	/**
	 *
	 */
	caddress(
			struct sockaddr_in *sa,
			size_t salen) throw (eAddressInval);

	/**
	 * Copy constructor.
	 * @param ca Reference to the caddress instance to be copied.
	 */
	caddress(
			caddress const& ca);

	/**
	 * Destructor.
	 * Deallocates the allocated memory area during construction.
	 */
	~caddress();

	/**
	 * Assignment operator.
	 * @param ca Reference to the caddress instance to be assigned to *this
	 */
	caddress&
	operator=(
			caddress const& ca);

	/**
	 * less operator.
	 * @param ca Reference to the caddress instance to be compared against *this
	 */
	bool
	operator<(
			caddress const& ca) const;

	/**
	 * greater operator.
	 * @param ca Reference to the caddress instance to be compared against *this
	 */
	bool
	operator>(
			caddress const& ca) const;

	/**
	 *
	 */
	caddress
	operator& (
			caddress const& mask) const;

	/**
	 *
	 */
	bool
	operator== (
			caddress const& ca) const;

	/**
	 *
	 */
	bool
	operator!= (
			caddress const& ca) const;

	/**
	 *
	 */
	bool
	is_af_inet() const;

	/**
	 *
	 */
	bool
	is_af_inet6() const;

	/**
	 *
	 */
	bool
	is_af_unix() const;

	/**
	 *
	 */
	bool
	is_af_packet() const;

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
		switch (ca.saddr->sa_family) {
		case 0:
			os << "caddress(EMPTY)";
			break;
		case AF_INET:
			os << "caddress(AF_INET/" << inet_ntop(AF_INET, &(ca.s4addr->sin_addr), address, 256)
			<< ":" << ntohs(ca.s4addr->sin_port) << "/" << ca.salen << ")";
			break;
		case AF_INET6:
			os << "caddress(AF_INET6/" << inet_ntop(AF_INET6, &(ca.s6addr->sin6_addr), address, 256)
			<< ":" << ntohs(ca.s6addr->sin6_port) << "/" << ca.salen << ")";
			break;
		case AF_UNIX:
			os << "caddress(AF_UNIX/" << ca.suaddr->sun_path << "/" << ca.salen << ")";
			break;
		case AF_PACKET:
			os << "caddress(AF_PACKET/TODO: implement!)";
			break;
		}
		return os;
	};
#endif
	/**
	 * Return a description string.
	 * Returns a description string for this caddress instance.
	 */
	const char*
	c_str();

	/**
	 * Returns the address as string (e.g. "1.2.3.4")
	 */
	const char*
	addr_c_str();


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

};

#endif
