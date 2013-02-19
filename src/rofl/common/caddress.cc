/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "caddress.h"

using namespace rofl;

caddress::caddress(size_t size) :
	cmemory(size),
	salen(size)
{
	ca_saddr = (struct sockaddr*)somem();
}


caddress::caddress(
		int af,
		u_int16_t protocol,
		std::string devname,
		u_int16_t hatype,
		u_int8_t pkttype,
		const char* addr,
		size_t halen) throw (eAddressInval) :
	cmemory(sizeof(struct sockaddr_ll))
{
	ca_saddr = (struct sockaddr*)somem();

	switch (af) {
	case PF_PACKET:
	{
		struct ifreq ifr;
			int rc, sd;
		if ((sd = socket(af, protocol, 0)) < 0) {
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

		ca_sladdr->sll_family 		= af;
		ca_sladdr->sll_protocol 	= htons(protocol);
		ca_sladdr->sll_ifindex 	= ifr.ifr_ifindex;
		ca_sladdr->sll_hatype 		= hatype;
		ca_sladdr->sll_pkttype 	= pkttype;
		ca_sladdr->sll_halen 		= halen;
		{
			int n = (halen < 8) ? halen : 8;
			memcpy(&(ca_sladdr->sll_addr[0]), addr, n);
		}
		salen = sizeof(struct sockaddr_ll);
		break;
	}
	default:
		throw eAddressInval();
	}
}


caddress::caddress(
		int af,
		const char* astr,
		u_int16_t port) throw (eAddressInval) :
	cmemory(sizeof(struct sockaddr_un))
{
	ca_saddr = (struct sockaddr*)somem();

	switch (af) {
	case AF_INET:
	{
		pton(af, astr);
		ca_s4addr->sin_family 	= AF_INET;
		ca_s4addr->sin_port 	= htons(port);
		salen = sizeof(struct sockaddr_in);
		break;
	}
	case AF_INET6:
	{
		pton(af, astr);
		ca_s6addr->sin6_family = AF_INET6;
		ca_s6addr->sin6_port 	= htons(port);
		salen = sizeof(struct sockaddr_in6);
		break;
	}
	case AF_UNIX:
	{
		ca_suaddr->sun_family 	= AF_UNIX;
		int n = (strlen(astr) < 108) ? strlen(astr) : 108;
		strncpy(&(ca_suaddr->sun_path[0]), astr, n);
		salen = sizeof(struct sockaddr_un);
		break;
	}
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

	memcpy((void*)ca_s6addr, (void*)sa, salen);
}


caddress::caddress(caddress const& ca)
{
	*this = ca;
}

caddress::~caddress()
{
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


caddress&
caddress::operator=(const caddress& ca)
{
	if (this == &ca)
		return *this;

	cmemory::operator= (ca);

	ca_saddr = (struct sockaddr*)somem();
	salen = memlen();

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
	if (ca.ca_saddr->sa_family != ca_saddr->sa_family)
	{
		return false;
	}

	switch (ca_saddr->sa_family) {
	case AF_INET:
		if (ca_s4addr->sin_addr.s_addr == ca.ca_s4addr->sin_addr.s_addr)
		{
			return true;
		}
		break;
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
		inet_ntop(AF_INET, &(ca_s6addr->sin6_addr), (char*)mem.somem(), mem.memlen()-1);
		info.assign(vas("[caddress(%p) AF_INET6/%s:%d/%d]",
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
		info.assign(vas("%s", mem.somem()));
		break;
	case AF_INET6:
		inet_ntop(AF_INET, &(ca_s6addr->sin6_addr), (char*)mem.somem(), mem.memlen()-1);
		info.assign(vas("%s", mem.somem()));
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


void
caddress::test()
{
	caddress a;
	caddress b(AF_INET, "1.2.3.4");
	caddress c(AF_UNIX, "/blub/blub/blub/blub/blub/blub");

	fprintf(stderr, "[1] a: %s b: %s\n", a.c_str(), b.c_str());

	a = b;

	fprintf(stderr, "[2] a: %s b: %s\n", a.c_str(), b.c_str());

	a = c;

	fprintf(stderr, "[3] a: %s c: %s\n", a.c_str(), c.c_str());
}


