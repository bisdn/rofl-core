/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofbucket.h"


cofbucket::cofbucket(
		uint16_t weight,
		uint32_t watch_port,
		uint32_t watch_group) :
	packet_count(0),
	byte_count(0),
	weight(weight),
	watch_port(watch_port),
	watch_group(watch_group)
{
	WRITELOG(COFBUCKET, ROFL_DBG, "cofbucket(%p)::cofbucket()", this);
}


cofbucket::cofbucket(
		struct ofp_bucket *bucket,
		size_t bclen) :
	packet_count(0),
	byte_count(0),
	weight(0),
	watch_port(0),
	watch_group(0)
{
	WRITELOG(COFBUCKET, ROFL_DBG, "cofbucket(%p)::cofbucket()", this);
	unpack(bucket, bclen);
}


cofbucket::~cofbucket()
{
	WRITELOG(COFBUCKET, ROFL_DBG, "cofbucket(%p)::~cofbucket()", this);
}


cofbucket&
cofbucket::operator= (const cofbucket& b)
{
	if (this == &b)
		return *this;

	this->actions = b.actions;
	this->packet_count = b.packet_count;
	this->byte_count = b.byte_count;
	this->weight = b.weight;
	this->watch_group = b.watch_group;
	this->watch_port = b.watch_port;

	return *this;
};


const char*
cofbucket::c_str()
{
	cvastring vas(4096);
	info.assign(vas("cofbucket(%p) weight:%d watch-group:%d watch-port:%d length:%d actions: %s",
			this, weight, watch_group, watch_port, length(), actions.c_str()));
	return info.c_str();
}


struct ofp_bucket*
cofbucket::pack(
		struct ofp_bucket* bucket,
		size_t bclen)
throw (eBucketBadLen)
{
	size_t __bclen = sizeof(struct ofp_bucket) + actions.length();

	if (bclen < __bclen)
		throw eBucketBadLen();

	WRITELOG(COFBUCKET, ROFL_DBG, "cofbucket(%p)::pack() %s", this, c_str());

	bucket->len = htobe16(__bclen);
	bucket->weight = htobe16(weight);
	bucket->watch_port = htobe32(watch_port);
	bucket->watch_group = htobe32(watch_group);

	size_t aclen = bclen - sizeof(struct ofp_bucket);

	actions.pack(bucket->actions, aclen);

	return bucket;
}


void
cofbucket::unpack(
		struct ofp_bucket* bucket,
		size_t bclen)
throw (eBucketBadLen, eActionBadOutPort)
{
	if (bclen < sizeof(struct ofp_bucket))
		throw eBucketBadLen();

	weight = be16toh(bucket->weight);
	watch_port = be32toh(bucket->watch_port);
	watch_group = be32toh(bucket->watch_group);

	size_t aclen = bclen - sizeof(struct ofp_bucket);

	if (aclen >= sizeof(struct ofp_action_header))
	{
		actions.unpack(bucket->actions, aclen);
	}
}


size_t
cofbucket::length()
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
	cmemory bstats(sizeof(struct ofp_bucket_counter));
	struct ofp_bucket_counter* bucket_counter = (struct ofp_bucket_counter*)bstats.somem();

	bucket_counter->packet_count 	= htobe64(packet_count);
	bucket_counter->byte_count		= htobe64(byte_count);

	body += bstats;
}

template class coflist<cofbucket>;

