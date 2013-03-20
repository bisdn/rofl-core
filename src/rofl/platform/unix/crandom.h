/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CRANDOM_H
#define CRANDOM_H 1

#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#ifdef __cplusplus
}
#endif

#include "rofl/common/cerror.h"
#include "rofl/common/cmemory.h"

namespace rofl
{

class eRandomBase : public eMemBase {}; // error base class crandom
class eRandomOpenFailed : public eRandomBase {}; // open system-call failed
class eRandomReadFailed : public eRandomBase {}; // read system-call failed

class crandom : public cmemory {
#define DEV_URANDOM "/dev/urandom"
public:
	// constructor with default random number length of 4 bytes
	crandom(size_t vallen = sizeof(uint32_t));
	// destructor
	virtual
	~crandom();
	// copy constructor
	crandom(crandom &r)
	{
		*this = r;
	};
	// assignment operator
	crandom&
	operator=(const crandom &r)
	{
		if (this == &r)
			return *this;
		cmemory::operator= (r);
		return *this;
	};

	/** return random number of length "length"
	 */
	crandom& rand(size_t length);
	/** return length of random number
	 */
	size_t randlen();
	/** dump random number as hex string
	 */
	const char* c_str();
	/** convenience method: return uint8_t
	 */
	uint8_t uint8();
	/** convenience method: return uint16_t
	 */
	uint16_t uint16();
	/** convenience method: return uint32_t
	 */
	uint32_t uint32();
	/** convenience method: return uint64_t
	 */
	uint64_t uint64();
};

}; // end of namespace

#endif
