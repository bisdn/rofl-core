/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFBUCKET_H
#define COFBUCKET_H 1

#include <string>
#include <vector>
#include <list>

#ifdef __cplusplus
extern "C" {
#endif
#include "openflow.h"
#include <endian.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif
#ifdef __cplusplus
}
#endif

#include "../cerror.h"
#include "../cmemory.h"
#include "../cvastring.h"
#include "../coflist.h"

#include "rofl/platform/unix/csyslog.h"

#include "cofaclist.h"

namespace rofl
{

/* error classes */
class eBucketBase : public cerror {}; // error base class for class cofbucket
class eBucketInval : public eBucketBase {}; // parameter is invalid
class eBucketBadLen : public eBucketBase {}; // invalid length



class cofbucket : public csyslog {
public: // static methods and data structures


public: // data structures

	uint64_t packet_count; // packet count for this bucket
	uint64_t byte_count; // byte count for this bucket
    uint16_t weight;
    uint32_t watch_port;
    uint32_t watch_group;

	cofaclist actions; // list of OpenFlow actions

public: // per instance methods

	/** constructor
	 */
	cofbucket(
			uint16_t weigth = 0,
			uint32_t watch_port = 0,
			uint32_t watch_group = 0);

	/** constructor
	 */
	cofbucket(
			struct ofp12_bucket *bucket,
			size_t bclen);


	/** constructor
	 */
	cofbucket(
			struct ofp13_bucket *bucket,
			size_t bclen);


	/** destructor
	 */
	virtual
	~cofbucket();

	/** assignment operator
	 */
	cofbucket& operator= (const cofbucket& b);

	/** dump info string
	 */
	const char*
	c_str();

	/** pack bucket
	 */
	struct ofp12_bucket*
	pack(struct ofp12_bucket* bucket, size_t bclen) const
		throw (eBucketBadLen);

	/** unpack bucket
	 */
	void
	unpack(struct ofp12_bucket* bucket, size_t bclen)
		throw (eBucketBadLen, eBadActionBadOutPort);

	/** pack bucket
	 */
	struct ofp13_bucket*
	pack(struct ofp13_bucket* bucket, size_t bclen) const
		throw (eBucketBadLen);

	/** unpack bucket
	 */
	void
	unpack(struct ofp13_bucket* bucket, size_t bclen)
		throw (eBucketBadLen, eBadActionBadOutPort);

	/** bucket length
	 */
	size_t
	length() const;


	/**
	 *
	 */
	void
	get_bucket_stats(
			cmemory& body);


private:

	std::string info; // info string

};

}; // end of namespace

#endif
