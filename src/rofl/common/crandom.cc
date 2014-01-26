/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crandom.h"

using namespace rofl;

crandom::crandom(size_t vallen)
{
	rand(vallen);
}

crandom::~crandom()
{

}

crandom&
crandom::rand(size_t length)
{
	int fd = 0;
	int rc = 0;
	try {
		resize(length);

		if ((fd = open(DEV_URANDOM, O_RDONLY, "r")) < 0)
			throw eRandomOpenFailed();

		if ((rc = read(fd, somem(), memlen())) < 0)
			throw eRandomReadFailed();

		close(fd);

	} catch (eRandomReadFailed& e) {
		close(fd);
		throw;
	}

	return *this;
}


size_t
crandom::randlen()
{
	return memlen();
}



uint8_t
crandom::uint8()
{
	if (memlen() < sizeof(uint8_t))
		rand(sizeof(uint8_t));
	uint8_t *n = (uint8_t*)somem();
	return *n;
}

uint16_t
crandom::uint16()
{
	if (memlen() < sizeof(uint16_t))
		rand(sizeof(uint16_t));
	uint16_t *n = (uint16_t*)somem();
	return *n;
}

uint32_t
crandom::uint32()
{
	if (memlen() < sizeof(uint32_t))
		rand(sizeof(uint32_t));
	uint32_t *n = (uint32_t*)somem();
	return *n;
}

uint64_t
crandom::uint64()
{
	if (memlen() < sizeof(uint64_t))
		rand(sizeof(uint64_t));
	uint64_t *n = (uint64_t*)somem();
	return *n;
}



