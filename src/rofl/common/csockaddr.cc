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
		struct sockaddr_in* sin, size_t sinlen)
{
	if ((0 == sin) || (0 == sinlen))
		return;

	if (sinlen < csockaddr_in4::length())
		throw eInval("csockaddr_in4::pack() sinlen too short");

	csockaddr::pack((struct sockaddr*)sin, sinlen);

	sin->sin_addr.s_addr = saddr.get_addr_nbo();
	sin->sin_port = htobe16(portno);
}



void
csockaddr_in4::unpack(
		struct sockaddr_in* sin, size_t sinlen)
{
	if ((0 == sin) || (0 == sinlen))
		return;

	if (sinlen < csockaddr_in4::length())
		throw eInval("csockaddr_in6::unpack() sinlen too short");

	csockaddr::unpack((struct sockaddr*)sin, sinlen);

	saddr.set_addr_nbo(sin->sin_addr.s_addr);
	portno = be16toh(sin->sin_port);
}



size_t
csockaddr_in6::length() const
{
	return sizeof(struct sockaddr_in6);
}



void
csockaddr_in6::pack(
		struct sockaddr_in6* sin6, size_t sin6len)
{
	if ((0 == sin6) || (0 == sin6len))
		return;

	if (sin6len < csockaddr_in6::length())
		throw eInval("csockaddr_in6::pack() sin6len too short");

	csockaddr::pack((struct sockaddr*)sin6, sin6len);

	saddr.pack((uint8_t*)&(sin6->sin6_addr.s6_addr), 16);
	sin6->sin6_port = htobe16(portno);
}



void
csockaddr_in6::unpack(
		struct sockaddr_in6* sin6, size_t sin6len)
{
	if ((0 == sin6) || (0 == sin6len))
		return;

	if (sin6len < csockaddr_in6::length())
		throw eInval("csockaddr_in6::unpack() sin6len too short");

	csockaddr::unpack((struct sockaddr*)sin6, sin6len);

	portno = be16toh(sin6->sin6_port);
	saddr.unpack((uint8_t*)&(sin6->sin6_addr.s6_addr), 16);
}







