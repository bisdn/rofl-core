/*
 * csockaddr.cc
 *
 *  Created on: 10.06.2014
 *      Author: andreas
 */

#include "rofl/common/csockaddr.h"

using namespace rofl;

size_t
csockaddr::length() const
{
	return sizeof(sa_family_t);
}



void
csockaddr::pack(
		struct sockaddr* sa, size_t salen)
{
	if ((0 == sa) || (0 == salen))
		return;

	if (salen < csockaddr::length())
		throw eInval("csockaddr::pack() buflen too short");

	sa->sa_family = htobe16(sa_family);
}



void
csockaddr::unpack(
		struct sockaddr* sa, size_t salen)
{
	if ((0 == sa) || (0 == salen))
		return;

	if (salen < csockaddr::length())
		throw eInval("csockaddr::unpack() salen too short");

	sa_family = be16toh(sa->sa_family);
}



size_t
csockaddr_in4::length() const
{
	return sizeof(struct sockaddr_in);
}



void
csockaddr_in4::pack(
		struct sockaddr* sa, size_t salen)
{
	if ((0 == sa) || (0 == salen))
		return;

	if (salen < csockaddr_in4::length())
		throw eInval("csockaddr_in4::pack() salen too short");

	csockaddr::pack(sa, salen);

	struct sockaddr_in* sin = (struct sockaddr_in*)sa;

	memcpy((uint8_t*)&(sin->sin_addr.s_addr), saddr.somem(), saddr.length());
	sin->sin_port 			= htobe16(portno);
}



void
csockaddr_in4::unpack(
		struct sockaddr* sa, size_t salen)
{
	if ((0 == sa) || (0 == salen))
		return;

	if (salen < csockaddr_in4::length())
		throw eInval("csockaddr::unpack() salen too short");

	struct sockaddr_in* sin = (struct sockaddr_in*)sa;

	portno = be16toh(sin->sin_port);
	memcpy(saddr.somem(), (uint8_t*)&(sin->sin_addr.s_addr), saddr.length());
}



std::string
csockaddr_in4::addr2str_nbo(uint32_t addr) const
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = addr;
	rofl::cmemory mem(16);
	char* ptr = inet_ntop(AF_INET, &sin, (char*)mem.somem(), mem.length());
	if (NULL == ptr) {
		throw eSysCall("inet_ntop()");
	}
	return std::string(mem.somem(), strnlen((char*)mem.somem(), mem.length()));
}



uint32_t
csockaddr_in4::str2addr_nbo(const std::string& addr) const
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	int rc = inet_pton(AF_INET, addr.c_str(), &sin);

	if (rc == 1) { /* success */
		// yippieh
	} else
	if (rc <= 0) {
		throw eSysCall("inet_pton());
	}
	return sin.sin_addr.s_addr;
}



size_t
csockaddr_in6::length() const
{
	return sizeof(struct sockaddr_in6);
}



void
csockaddr_in6::pack(
		struct sockaddr* sa, size_t salen)
{
	if ((0 == sa) || (0 == salen))
		return;

	if (salen < csockaddr_in6::length())
		throw eInval("csockaddr_in6::pack() salen too short");

	csockaddr::pack(sa, salen);

	struct sockaddr_in6* sin6 = (struct sockaddr_in6*)sa;

	memcpy((uint8_t*)&(sin6->sin6_addr.s6_addr), saddr.somem(), saddr.length());
	sin6->sin6_port = htobe16(portno);
}



void
csockaddr_in6::unpack(
		struct sockaddr* sa, size_t salen)
{
	if ((0 == sa) || (0 == salen))
		return;

	if (salen < csockaddr_in6::length())
		throw eInval("csockaddr::unpack() salen too short");

	struct sockaddr_in6* sin6 = (struct sockaddr_in6*)sa;

	portno = be16toh(sin6->sin6_port);
	memcpy(saddr.somem(), (uint8_t*)&(sin6->sin6_addr.s6_addr), saddr.length());
}



std::string
csockaddr_in6::addr2str_nbo(const rofl::cmemory& addr) const
{
	if (addr.length() < 16)
		throw eSockAddrInval("csockaddr_in6::addr2str_nbo() addr to short");

	struct sockaddr_in6 sin6;
	memset(&sin6, 0, sizeof(sin6));
	sin6.sin6_family = AF_INET6;
	memcpy(sin6.sin6_addr.s6_addr, addr.somem(), 16);
	rofl::cmemory mem(40);
	char* ptr = inet_ntop(AF_INET6, &sin6, (char*)mem.somem(), mem.length());
	if (NULL == ptr) {
		throw eSysCall("inet_ntop()");
	}
	return std::string(mem.somem(), strnlen((char*)mem.somem(), mem.length()));
}



rofl::cmemory
csockaddr_in6::str2addr_nbo(const std::string& addr) const
{
	struct sockaddr_in6 sin6;
	memset(&sin6, 0, sizeof(sin6));
	sin6.sin6_family = AF_INET6;
	int rc = inet_pton(AF_INET6, addr.c_str(), &sin6);

	if (rc == 1) { /* success */
		// yippieh
	} else
	if (rc <= 0) {
		throw eSysCall("inet_pton());
	}
	return rofl::cmemory(sin6.sin6_addr.s6_addr, 16);
}







