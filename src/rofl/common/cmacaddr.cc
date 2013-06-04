/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cmacaddr.h"

using namespace rofl;

cmacaddr::cmacaddr() :
	cmemory(OFP_ETH_ALEN)
{
	cvastring vas(32);

	info.assign(vas("%02x:%02x:%02x:%02x:%02x:%02x",
			(*this)[0],
			(*this)[1],
			(*this)[2],
			(*this)[3],
			(*this)[4],
			(*this)[5]));
}



cmacaddr::cmacaddr(cmacaddr const& ma)
{
	*this = ma;
}



cmacaddr::cmacaddr(cmemory const& mem) :
	cmemory(OFP_ETH_ALEN)
{
	size_t len = mem.memlen() < OFP_ETH_ALEN? mem.memlen() : OFP_ETH_ALEN;

	memcpy(somem(), mem.somem(), len);

	cvastring vas(32);

	info.assign(vas("%02x:%02x:%02x:%02x:%02x:%02x",
			(*this)[0],
			(*this)[1],
			(*this)[2],
			(*this)[3],
			(*this)[4],
			(*this)[5]));
}


cmacaddr::cmacaddr(uint8_t *data, size_t datalen) :
		cmemory(data, datalen)
{
	size_t len = (datalen < OFP_ETH_ALEN) ? datalen : OFP_ETH_ALEN;

	memcpy(somem(), data, len);

	cvastring vas(32);

	info.assign(vas("%02x:%02x:%02x:%02x:%02x:%02x",
			(*this)[0],
			(*this)[1],
			(*this)[2],
			(*this)[3],
			(*this)[4],
			(*this)[5]));
}


/*
 * cmacaddr ma(std::string("00:01:01:01:01:01"));
 */
cmacaddr::cmacaddr(const std::string& s_addr) :
	cmemory(OFP_ETH_ALEN)
{
	sscanf(s_addr.c_str(), "%x:%x:%x:%x:%x:%x",
			(int*)&((*this)[0]),
			(int*)&((*this)[1]),
			(int*)&((*this)[2]),
			(int*)&((*this)[3]),
			(int*)&((*this)[4]),
			(int*)&((*this)[5]));

	cvastring vas(32);

	info.assign(vas("%02x:%02x:%02x:%02x:%02x:%02x",
			(*this)[0],
			(*this)[1],
			(*this)[2],
			(*this)[3],
			(*this)[4],
			(*this)[5]));
}


/*
 * cmacaddr ma("00:01:01:01:01:01");
 */
cmacaddr::cmacaddr(const char* s_addr) :
	cmemory(OFP_ETH_ALEN)
{
	unsigned int vec[OFP_ETH_ALEN];

	sscanf(s_addr, "%02x:%02x:%02x:%02x:%02x:%02x",
			&vec[0],
			&vec[1],
			&vec[2],
			&vec[3],
			&vec[4],
			&vec[5]);

	for (int i = 0; i < OFP_ETH_ALEN; ++i)
	{
		(*this)[i] = vec[i];
	}

	cvastring vas(32);

	info.assign(vas("%02x:%02x:%02x:%02x:%02x:%02x",
			(*this)[0],
			(*this)[1],
			(*this)[2],
			(*this)[3],
			(*this)[4],
			(*this)[5]));
}

cmacaddr::cmacaddr(const uint64_t mac) :
		cmemory(OFP_ETH_ALEN)
{
	uint64_t _mac = htobe64(mac);

	(*this)[0] = ((uint8_t*)&_mac)[2];
	(*this)[1] = ((uint8_t*)&_mac)[3];
	(*this)[2] = ((uint8_t*)&_mac)[4];
	(*this)[3] = ((uint8_t*)&_mac)[5];
	(*this)[4] = ((uint8_t*)&_mac)[6];
	(*this)[5] = ((uint8_t*)&_mac)[7];

	cvastring vas(32);

	info.assign(vas("%02x:%02x:%02x:%02x:%02x:%02x",
			(*this)[0],
			(*this)[1],
			(*this)[2],
			(*this)[3],
			(*this)[4],
			(*this)[5]));
}


cmacaddr&
cmacaddr::operator= (cmacaddr const& ma)
{
	if (this == &ma)
		return *this;

	cmemory::operator= (ma);

	cvastring vas(32);

	info.assign(vas("%02x:%02x:%02x:%02x:%02x:%02x",
			(*this)[0],
			(*this)[1],
			(*this)[2],
			(*this)[3],
			(*this)[4],
			(*this)[5]));

	return *this;
}


bool
cmacaddr::operator== (cmacaddr const& ma) const
{
	return (((*this)[0] == ma[0]) &&
			((*this)[1] == ma[1]) &&
			((*this)[2] == ma[2]) &&
			((*this)[3] == ma[3]) &&
			((*this)[4] == ma[4]) &&
			((*this)[5] == ma[5]));
}


bool
cmacaddr::operator!= (cmacaddr const& ma) const
{
	return (not operator== (ma));
}


cmacaddr&
cmacaddr::operator& (
		cmacaddr const& ma)
{
	for (size_t i = 0; i < OFP_ETH_ALEN; ++i)
	{
		(*this)[i] &= ma[i];
	}
	return *this;
}


bool
cmacaddr::is_multicast() const
{
	if ((*this)[0] & 0x01)
		return true;
	return false;
}


bool
cmacaddr::is_broadcast() const
{
	if (*this == cmacaddr("ff:ff:ff:ff:ff:ff"))
		return true;
	return false;
}


bool
cmacaddr::is_null() const
{
	if (*this == cmacaddr("00:00:00:00:00:00"))
		return true;
	return false;
}


const char*
cmacaddr::c_str() const
{
	return info.c_str();
}

uint64_t
cmacaddr::get_mac() const
{
	uint64_t mac = 0;

	((uint8_t*)&mac)[2] = (*this)[0];
	((uint8_t*)&mac)[3] = (*this)[1];
	((uint8_t*)&mac)[4] = (*this)[2];
	((uint8_t*)&mac)[5] = (*this)[3];
	((uint8_t*)&mac)[6] = (*this)[4];
	((uint8_t*)&mac)[7] = (*this)[5];

	return be64toh(mac);
}
