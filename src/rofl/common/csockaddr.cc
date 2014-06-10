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
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < csockaddr::length())
		throw eInval("csockaddr::pack() buflen too short");

	struct sockaddr* sa = (struct sockaddr*)buf;

	sa->sa_family = htobe16(sa_family);
}



void
csockaddr::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < csockaddr::length())
		throw eInval("csockaddr::unpack() buflen too short");

	struct sockaddr* sa = (struct sockaddr*)buf;

	sa_family = be16toh(sa->sa_family);
}



size_t
csockaddr_in4::length() const
{
	return sizeof(struct sockaddr_in);
}



void
csockaddr_in4::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < csockaddr::length())
		throw eInval("csockaddr::pack() buflen too short");

	struct sockaddr* sa = (struct sockaddr*)buf;

	sa->sa_family = htobe16(sa_family);
}



void
csockaddr_in4::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < csockaddr::length())
		throw eInval("csockaddr::unpack() buflen too short");

	struct sockaddr* sa = (struct sockaddr*)buf;

	sa_family = be16toh(sa->sa_family);
}

