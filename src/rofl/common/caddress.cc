/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "caddress.h"
#include "rofl/datapath/pipeline/common/large_types.h"

using namespace rofl;



caddress::caddress(
		std::string const& node,
		std::string const& service,
		int ai_flags,
		int preferred_family,
		int preferred_socktype,
		int preferred_protocol) :
				salen(0),
				node(node),
				service(service)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_flags		= ai_flags;
	hints.ai_family 	= preferred_family;
	hints.ai_socktype 	= preferred_socktype;
	hints.ai_protocol	= preferred_protocol;

	struct addrinfo *res = (struct addrinfo*)0;

	int rc = getaddrinfo((node.empty()) ? NULL : node.c_str(), (service.empty()) ? NULL : service.c_str(), &hints, &res);
	if (rc != 0) {
		switch (rc) {
		case EAI_ADDRFAMILY:
		case EAI_AGAIN:
		case EAI_BADFLAGS:
		case EAI_FAIL:
		case EAI_FAMILY:
		case EAI_MEMORY:
		case EAI_NODATA:
		case EAI_NONAME:
		case EAI_SERVICE:
		case EAI_SOCKTYPE: {
			rofl::logging::error << "[rofl][caddress] name resolution failed: " << std::string(gai_strerror(rc)) << std::endl;
		} break;
		case EAI_SYSTEM:
		default: {
			rofl::logging::error << "[rofl][caddress] name resolution failed: unknown error occured " << std::endl;
		};
		}
		throw eSysCall("getaddrinfo() "+std::string(gai_strerror(rc)));
	}

	// for now: we take the first entry returned from getaddrinfo

	domain 		= res->ai_family;
	protocol 	= res->ai_protocol;
	sock_type 	= res->ai_socktype;
	ca_saddr 	= (struct sockaddr*)rofl::cmemory::resize(res->ai_addrlen);
	memcpy((uint8_t*)ca_saddr, (uint8_t*)(res->ai_addr), res->ai_addrlen);
	salen		= res->ai_addrlen;
}



caddress::caddress(int af)
{
	switch (af) {
	case AF_UNSPEC: {
		salen = sizeof(struct sockaddr);
	} break;
	case AF_INET: {
		salen = sizeof(struct sockaddr_in);
	} break;
	case AF_INET6: {
		salen = sizeof(struct sockaddr_in6);
	} break;
	case AF_UNIX: {
		salen = sizeof(struct sockaddr_un);
	} break;
	case AF_PACKET: {
		salen = sizeof(struct sockaddr_ll);
	} break;
	default:
		throw eNotImplemented();
	}

	resize(salen);
	ca_saddr->sa_family = af;
}



caddress::caddress(
		u_int16_t protocol,
		std::string devname,
		u_int16_t hatype,
		u_int8_t pkttype,
		const char* addr,
		size_t halen) :
	cmemory(sizeof(struct sockaddr_ll))
{
	ca_saddr = (struct sockaddr*)somem();

	struct ifreq ifr;
		int rc, sd;
	if ((sd = socket(AF_PACKET, protocol, 0)) < 0) {
		switch (errno) {
			case EACCES:
				throw eAddressSocketFailedNoPermission();
				break;
			default:
				throw eAddressSocketFailed();
				break;
		}
	}
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, devname.c_str(), IFNAMSIZ);
	if ((rc = ioctl(sd, SIOCGIFINDEX, &ifr)) < 0)
		throw eAddressIoctlFailed();
	close(sd);

	ca_sladdr->sll_family 		= AF_PACKET;
	ca_sladdr->sll_protocol 	= htons(protocol);
	ca_sladdr->sll_ifindex 		= ifr.ifr_ifindex;
	ca_sladdr->sll_hatype 		= hatype;
	ca_sladdr->sll_pkttype 		= pkttype;
	ca_sladdr->sll_halen 		= halen;
	{
		int n = (halen < 8) ? halen : 8;
		memcpy(&(ca_sladdr->sll_addr[0]), addr, n);
	}
	salen = sizeof(struct sockaddr_ll);
}



caddress::caddress(
		int af,
		const char* astr,
		u_int16_t port) throw (eAddressInval) :
	cmemory(sizeof(struct sockaddr_un))
{
	switch (af) {
	case AF_INET: {
		resize(sizeof(struct sockaddr_in));
		pton(af, astr);
		ca_s4addr->sin_family 	= AF_INET;
		ca_s4addr->sin_port 	= htons(port);
		salen = sizeof(struct sockaddr_in);
	} break;
	case AF_INET6: {
		resize(sizeof(struct sockaddr_in6));
		pton(af, astr);
		ca_s6addr->sin6_family 	= AF_INET6;
		ca_s6addr->sin6_port 	= htons(port);
		salen = sizeof(struct sockaddr_in6);
	} break;
	case AF_UNIX: {
		resize(sizeof(struct sockaddr_un));
		ca_suaddr->sun_family 	= AF_UNIX;
		int n = (strlen(astr) < 108) ? strlen(astr) : 108;
		strncpy(&(ca_suaddr->sun_path[0]), astr, n);
		salen = sizeof(struct sockaddr_un);
	} break;
	default:
		throw eAddressInval();
	}
}



caddress::caddress(
		struct sockaddr_in *sa,
		size_t salen) throw (eAddressInval) :
		cmemory(sizeof(struct sockaddr_in))
{
	if (salen < sizeof(struct sockaddr_in))
	{
		throw eAddressInval();
	}

	ca_saddr = (struct sockaddr*)somem();
	this->salen = sizeof(struct sockaddr_in);

	memcpy((void*)ca_s4addr, (void*)sa, salen);
}



caddress::caddress(
		struct sockaddr_in6 *sa,
		size_t salen) throw (eAddressInval) :
		cmemory(sizeof(struct sockaddr_in6))
{
	if (salen < sizeof(struct sockaddr_in6))
	{
		throw eAddressInval();
	}

	ca_s6addr = (struct sockaddr_in6*)somem();
	this->salen = sizeof(struct sockaddr_in6);

	memcpy((void*)ca_s6addr, (void*)sa, salen);
}



caddress::caddress(caddress const& ca)
{
	*this = ca;
}



caddress::~caddress()
{
}


size_t
caddress::length() const
{
	switch (ca_saddr->sa_family) {
	case AF_INET: 	return sizeof(uint32_t);
	case AF_INET6: 	return 16*sizeof(uint8_t);
	case AF_UNIX:	return strlen(ca_suaddr->sun_path);
	default: return 0;
	}
}



void
caddress::pack(uint8_t* buf, size_t buflen)
{
	if (buflen < length()) {
		throw eInval();
	}

	switch (ca_saddr->sa_family) {
	case AF_INET: {
		memcpy(buf, (uint8_t*)&(ca_s4addr->sin_addr.s_addr), 4);
	} break;
	case AF_INET6: {
		memcpy(buf, (uint8_t*)ca_s6addr->sin6_addr.s6_addr, 16);
	} break;
	case AF_UNIX: {
		memcpy(buf, (uint8_t*)ca_suaddr->sun_path, strlen(ca_suaddr->sun_path));
	} break;
	default:
		return;
	}
}



void
caddress::unpack(uint8_t* buf, size_t buflen)
{
	unsigned int sa_family = ca_saddr->sa_family;

	if (buflen < length()) {
		resize(buflen); // FIXME: sizeof(sa_family) must be added
	}

	ca_saddr->sa_family = sa_family;

	switch (ca_saddr->sa_family) {
	case AF_INET: {
		memcpy((uint8_t*)&(ca_s4addr->sin_addr.s_addr), buf, 4);
	} break;
	case AF_INET6: {
		memcpy((uint8_t*)ca_s6addr->sin6_addr.s6_addr, buf, 16);
	} break;
	case AF_UNIX: {
		memcpy((uint8_t*)ca_suaddr->sun_path, buf, (buflen > 108) ? 108 : buflen);
	} break;
	default:
		return;
	}
}



uint8_t*
caddress::resize(size_t len)
{
	cmemory::resize(len);
	ca_saddr = (struct sockaddr*)somem();
	return somem();
}


void
caddress::pton(int af, const char* astr)
{
	switch (af) {
	case AF_INET:
		inet_pton(af, astr, (struct in_addr*)&(ca_s4addr->sin_addr));
		break;
	case AF_INET6:
		inet_pton(af, astr, (struct in6_addr*)&(ca_s6addr->sin6_addr));
		break;
	}
}



int
caddress::get_family() const
{
	return ca_saddr->sa_family;
}



caddress&
caddress::operator=(const caddress& ca)
{
	if (this == &ca)
		return *this;

	cmemory::operator= (ca);

	ca_saddr = (struct sockaddr*)somem();
	salen = ca.salen;

	return *this;
}


bool
caddress::operator< (const caddress& ca) const
{
	return cmemory::operator< (ca);
}


bool
caddress::operator> (const caddress& ca) const
{
	return ((not cmemory::operator== (ca)) && (not cmemory::operator< (ca)));
}


caddress
caddress::operator& (caddress const& mask) const
{
	caddress a(*this);

	if (mask.ca_saddr->sa_family != ca_saddr->sa_family)
	{
		return *this; // invalid address family! simply return *this, better throw exception?
	}

	switch (ca_saddr->sa_family) {
	case AF_INET:
		a.ca_s4addr->sin_addr.s_addr &= mask.ca_s4addr->sin_addr.s_addr;
		break;
	case AF_INET6:
		for (int i = 0; i < 16; i++)
		{
			a.ca_s6addr->sin6_addr.s6_addr[i] &= mask.ca_s6addr->sin6_addr.s6_addr[i];
		}
		break;
	default:
		throw eNotImplemented();
	}

	return a;
}


bool
caddress::operator== (caddress const& ca) const
{
	if (ca.ca_saddr->sa_family != ca_saddr->sa_family) {
		return false;
	}

	switch (ca_saddr->sa_family) {
	case AF_INET: {
		if (ca_s4addr->sin_addr.s_addr == ca.ca_s4addr->sin_addr.s_addr) {
			return true;
		}
	} break;
	case AF_INET6: {
		if (not memcmp(ca_s6addr->sin6_addr.s6_addr, ca.ca_s6addr->sin6_addr.s6_addr,
				16)) {
			return true;
		}
	} break;
	default:
		throw eNotImplemented();
	}
	return false;
}


bool
caddress::operator!= (caddress const& ca) const
{
	return (not operator== (ca));
}


uint8_t&
caddress::operator[] (
		unsigned int index)
{
	switch (ca_saddr->sa_family) {
	case AF_INET: {
		if ((index >= AF_INET4_SIZE))
			throw eInval();
		return ((uint8_t*)&(ca_s4addr->sin_addr.s_addr))[index];
	} break;
	case AF_INET6: {
		if ((index >= AF_INET6_SIZE))
			throw eInval();
		return ((uint8_t*)&(ca_s6addr->sin6_addr.s6_addr))[index];
	} break;
	default: {
		throw eInval();
	}
	}
}


const char*
caddress::c_str()
{
	cvastring vas;
	cmemory mem(256);

	switch (ca_saddr->sa_family) {
	case 0:
		info.assign(vas("[caddress(%p) EMPTY]", this));
		break;
	case AF_INET:
		inet_ntop(AF_INET, &(ca_s4addr->sin_addr), (char*)mem.somem(), mem.memlen()-1);
		info.assign(vas("[AF_INET/ip:%s port:%d salen:%d]",
				mem.somem(),
				be16toh(ca_s4addr->sin_port),
				salen));
		break;
	case AF_INET6:
		inet_ntop(AF_INET6, &(ca_s6addr->sin6_addr), (char*)mem.somem(), mem.memlen()-1);
		info.assign(vas("[caddress(%p) AF_INET6/%s port:%d salen:%d]",
				this,
				mem.somem(),
				be16toh(ca_s6addr->sin6_port),
				salen));
		break;
	case AF_UNIX:
		info.assign(vas("[caddress(%p) AF_UNIX/%s/%d]",
				this,
				ca_suaddr->sun_path,
				salen));
		break;
	case AF_PACKET:
		info.assign(vas("[caddress(%p) AF_PACKET/TODO: implement!]", this));
		break;
	}

	return info.c_str();
};


const char*
caddress::addr_c_str()
{
	cvastring vas;
	cmemory mem(256);

	switch (ca_saddr->sa_family) {
	case AF_INET:
		inet_ntop(AF_INET, &(ca_s4addr->sin_addr), (char*)mem.somem(), mem.memlen()-1);
		info.assign(vas("%s:%d", mem.somem(), be16toh(ca_s4addr->sin_port)));
		break;
	case AF_INET6:
		inet_ntop(AF_INET6, &(ca_s6addr->sin6_addr), (char*)mem.somem(), mem.memlen()-1);
		info.assign(vas("[%s]:%d", mem.somem(), be16toh(ca_s6addr->sin6_port)));
		break;
	case AF_UNIX:
		info.assign(vas("%s", ca_suaddr->sun_path));
		break;
	case AF_PACKET:
		info.assign(vas("[caddress(%p) AF_PACKET/TODO: implement!]", this));
		break;
	}

	return info.c_str();
}


bool
caddress::is_af_inet() const
{
	return (AF_INET == ca_saddr->sa_family);
}


bool
caddress::is_af_inet6() const
{
	return (AF_INET6 == ca_saddr->sa_family);
}


bool
caddress::is_af_unix() const
{
	return (AF_UNIX == ca_saddr->sa_family);
}


bool
caddress::is_af_packet() const
{
	return (AF_PACKET == ca_saddr->sa_family);
}


bool
caddress::is_multicast() const
{
	switch (ca_saddr->sa_family) {
	case AF_INET: {
		caddress multicast(AF_INET, "224.0.0.0"); // [1110]0000 ...
		return (((*this) & multicast) == multicast);
	} break;
	case AF_INET6: {
		caddress multicast(AF_INET6, "ff00::");
		return (((*this) & multicast) == multicast);
	} break;
	default:
		throw eInval();
	}
}


bool
caddress::is_broadcast() const
{
	switch (ca_saddr->sa_family) {
	case AF_INET: {
		caddress broadcast(AF_INET, "225.255.255.255");
		return (((*this) & broadcast) == broadcast);
	} break;
	case AF_INET6: {
		caddress broadcast(AF_INET6, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
		return (((*this) & broadcast) == broadcast);
	} break;
	default:
		throw eInval();
	}

}


void
caddress::test()
{
	caddress a(AF_INET);
	caddress b(AF_INET, "1.2.3.4");
	caddress c(AF_UNIX, "/blub/blub/blub/blub/blub/blub");

	fprintf(stderr, "[1] a: %s b: %s\n", a.c_str(), b.c_str());

	a = b;

	fprintf(stderr, "[2] a: %s b: %s\n", a.c_str(), b.c_str());

	a = c;

	fprintf(stderr, "[3] a: %s c: %s\n", a.c_str(), c.c_str());
}

/**
 * This function takes the address from the sin_addr structure and puts it into a uint32_t
 * in HOST BYTE ORDER
 */
uint32_t
caddress::get_ipv4_addr(){
	
	switch (ca_saddr->sa_family){
		case AF_INET: {
			return be32toh(this->ca_s4addr->sin_addr.s_addr);
		}break;
		default:
			throw eInval();
	}
}

void
caddress::set_ipv4_addr(uint32_t addr){
	this->ca_s4addr->sin_addr.s_addr = htobe32(addr);
}

/**
 * This function takes the address from the sin6_addr structure and puts it into a uint128__t
 * in HOST BYTE ORDER
 */
uint128__t
caddress::get_ipv6_addr(){
	uint128__t addr;
	
	switch (ca_saddr->sa_family){
		case AF_INET6:{
			memcpy(&addr.val,this->ca_s6addr->sin6_addr.__in6_u.__u6_addr8,sizeof(addr));
			//Convert to host byte order
			NTOHB128(addr);
		}break;
		default:
			throw eInval();
	}
	return addr;
}

void
caddress::set_ipv6_addr(uint128__t addr){
	//Convert to network byte order
	HTONB128(addr);
	memcpy(this->ca_s6addr->sin6_addr.__in6_u.__u6_addr8,&addr.val,sizeof(addr));
}
