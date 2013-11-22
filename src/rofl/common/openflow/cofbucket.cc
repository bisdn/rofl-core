/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofbucket.h"

using namespace rofl;

cofbucket::cofbucket(
		uint8_t ofp_version,
		uint16_t weight,
		uint32_t watch_port,
		uint32_t watch_group) :
				ofp_version(ofp_version),
				packet_count(0),
				byte_count(0),
				weight(weight),
				watch_port(watch_port),
				watch_group(watch_group),
				actions(ofp_version)
{
	WRITELOG(COFBUCKET, DBG, "cofbucket(%p)::cofbucket()", this);
}


cofbucket::cofbucket(
		uint8_t ofp_version,
		uint8_t* bucket,
		size_t bclen) :
				ofp_version(ofp_version),
				packet_count(0),
				byte_count(0),
				weight(0),
				watch_port(0),
				watch_group(0),
				actions(ofp_version)
{
	WRITELOG(COFBUCKET, DBG, "cofbucket(%p)::cofbucket()", this);
	unpack(bucket, bclen);
}




cofbucket::~cofbucket()
{
	WRITELOG(COFBUCKET, DBG, "cofbucket(%p)::~cofbucket()", this);
}


cofbucket&
cofbucket::operator= (const cofbucket& b)
{
	if (this == &b)
		return *this;

	this->ofp_version	= b.ofp_version;
	this->actions 		= b.actions;
	this->packet_count 	= b.packet_count;
	this->byte_count 	= b.byte_count;
	this->weight 		= b.weight;
	this->watch_group 	= b.watch_group;
	this->watch_port 	= b.watch_port;

	return *this;
};



uint8_t*
cofbucket::pack(
		uint8_t* bucket,
		size_t bclen) const
throw (eBucketBadLen)
{
	switch (ofp_version) {
	case OFP12_VERSION: {
		return (uint8_t*)pack((struct ofp12_bucket*)bucket, bclen);
	} break;
	case OFP13_VERSION: {
		return (uint8_t*)pack((struct ofp13_bucket*)bucket, bclen);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofbucket::unpack(
		uint8_t* bucket,
		size_t bclen)
throw (eBucketBadLen, eBadActionBadOutPort)
{
	switch (ofp_version) {
	case OFP12_VERSION: {
		unpack((struct ofp12_bucket*)bucket, bclen);
	} return;
	case OFP13_VERSION: {
		unpack((struct ofp13_bucket*)bucket, bclen);
	} return;
	default:
		throw eBadVersion();
	}
}




struct ofp12_bucket*
cofbucket::pack(
		struct ofp12_bucket* bucket,
		size_t bclen) const
throw (eBucketBadLen)
{
	size_t __bclen = sizeof(struct ofp12_bucket) + actions.length();

	if (bclen < __bclen)
		throw eBucketBadLen();

	WRITELOG(COFBUCKET, DBG, "cofbucket(%p)::pack()", this);

	bucket->len = htobe16(__bclen);
	bucket->weight = htobe16(weight);
	bucket->watch_port = htobe32(watch_port);
	bucket->watch_group = htobe32(watch_group);

	size_t aclen = bclen - sizeof(struct ofp12_bucket);

	actions.pack(bucket->actions, aclen);

	return bucket;
}


void
cofbucket::unpack(
		struct ofp12_bucket* bucket,
		size_t bclen)
throw (eBucketBadLen, eBadActionBadOutPort)
{
	if (bclen < sizeof(struct ofp12_bucket))
		throw eBucketBadLen();

	weight = be16toh(bucket->weight);
	watch_port = be32toh(bucket->watch_port);
	watch_group = be32toh(bucket->watch_group);

	size_t aclen = bclen - sizeof(struct ofp12_bucket);

	if (aclen >= sizeof(struct ofp_action_header)) {
		actions.unpack(bucket->actions, aclen);
	}
}


struct ofp13_bucket*
cofbucket::pack(
		struct ofp13_bucket* bucket,
		size_t bclen) const
throw (eBucketBadLen)
{
	size_t __bclen = sizeof(struct ofp13_bucket) + actions.length();

	if (bclen < __bclen)
		throw eBucketBadLen();

	WRITELOG(COFBUCKET, DBG, "cofbucket(%p)::pack()", this);

	bucket->len = htobe16(__bclen);
	bucket->weight = htobe16(weight);
	bucket->watch_port = htobe32(watch_port);
	bucket->watch_group = htobe32(watch_group);

	size_t aclen = bclen - sizeof(struct ofp13_bucket);

	actions.pack(bucket->actions, aclen);

	return bucket;
}


void
cofbucket::unpack(
		struct ofp13_bucket* bucket,
		size_t bclen)
throw (eBucketBadLen, eBadActionBadOutPort)
{
	if (bclen < sizeof(struct ofp13_bucket))
		throw eBucketBadLen();

	weight = be16toh(bucket->weight);
	watch_port = be32toh(bucket->watch_port);
	watch_group = be32toh(bucket->watch_group);

	size_t aclen = bclen - sizeof(struct ofp13_bucket);

	if (aclen >= sizeof(struct ofp_action_header)) {
		actions.unpack(bucket->actions, aclen);
	}
}


size_t
cofbucket::length() const
{
	size_t total_length = 16 * sizeof(uint8_t) + actions.length();

	size_t pad = (0x7 & total_length);

	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}

	return (total_length);
}


void
cofbucket::get_bucket_stats(
		cmemory& body)
{
	cmemory bstats(sizeof(struct ofp12_bucket_counter));
	struct ofp12_bucket_counter* bucket_counter = (struct ofp12_bucket_counter*)bstats.somem();

	bucket_counter->packet_count 	= htobe64(packet_count);
	bucket_counter->byte_count		= htobe64(byte_count);

	body += bstats;
}

template class coflist<cofbucket>;

