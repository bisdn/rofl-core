/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofbuckets.h"

using namespace rofl::openflow;

cofbuckets::cofbuckets(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}



cofbuckets::~cofbuckets()
{
	clear();
}



cofbuckets::cofbuckets(cofbuckets const& buckets)
{
	*this = buckets;
}



cofbuckets&
cofbuckets::operator= (cofbuckets const& bc)
{
	if (this == &bc)
		return *this;

	this->ofp_version = bc.ofp_version;

	clear();

	for (std::map<uint32_t, cofbucket>::const_iterator
			it = bc.buckets.begin(); it != bc.buckets.end(); ++it) {
		buckets[it->first] = it->second;
	}

	return *this;
}



bool
cofbuckets::operator== (
		cofbuckets const& bc)
{
	if (ofp_version != bc.ofp_version)
		return false;

	if (buckets.size() != bc.buckets.size())
		return false;

	for (std::map<uint32_t, cofbucket>::const_iterator
			it = bc.buckets.begin(); it != bc.buckets.end(); ++it) {
		if (not (buckets[it->first] == it->second))
			return false;
	}

	return true;
}



void
cofbuckets::check_prerequisites() const
{
	for (std::map<uint32_t, cofbucket>::const_iterator
			it = buckets.begin(); it != buckets.end(); ++it) {
		it->second.check_prerequisites();
	}
}



size_t
cofbuckets::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofbucket>::const_iterator
			it = buckets.begin(); it != buckets.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



uint8_t*
cofbuckets::pack(uint8_t* buf, size_t buflen)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION:	return pack_of13(buf, buflen);
	default:						throw eBadVersion();
	}
}



void
cofbuckets::unpack(uint8_t* buf, size_t buflen)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION:	unpack_of13(buf, buflen); break;
	default:						throw eBadVersion();
	}
}



uint8_t*
cofbuckets::pack_of13(
	uint8_t* buf,
	size_t buflen)
{
	if (buflen < length())
		throw eBucketsInval();

	struct openflow::ofp_bucket *bchdr = (struct openflow::ofp_bucket*)buf;

	for (std::map<uint32_t, cofbucket>::iterator
			it = buckets.begin(); it != buckets.end(); ++it) {
		cofbucket& bucket = it->second;
		bchdr = (struct openflow::ofp_bucket*)
				((uint8_t*)(bucket.pack((uint8_t*)bchdr, bucket.length())) + bucket.length());
	}

	return buf;
}



void
cofbuckets::unpack_of13(
		uint8_t* buf,
		size_t buflen)
{
	clear(); // delete all previously assigned buckets in this bucket list

	// sanity check: buflen must be of size at least of ofp_bucket
	if (buflen < (int)sizeof(struct openflow::ofp_bucket))
		return;

	// first bucket
	struct openflow::ofp_bucket *bchdr = (struct openflow::ofp_bucket*)buf;

	uint32_t bucket_id = 0;

	while (buflen > 0) {

		if (be16toh(bchdr->len) < sizeof(struct openflow::ofp_bucket))
			throw eBucketBadLen();

		add_bucket(bucket_id++).unpack((uint8_t*)bchdr, be16toh(bchdr->len));

		buflen -= be16toh(bchdr->len);
		bchdr = (struct openflow::ofp_bucket*)(((uint8_t*)bchdr) + be16toh(bchdr->len));
	}
}



cofbucket&
cofbuckets::add_bucket(uint32_t bucket_id)
{
	if (buckets.find(bucket_id) != buckets.end()) {
		buckets.erase(bucket_id);
	}
	return (buckets[bucket_id] = cofbucket(ofp_version));
}



void
cofbuckets::drop_bucket(uint32_t bucket_id)
{
	if (buckets.find(bucket_id) == buckets.end()) {
		return;
	}
	buckets.erase(bucket_id);
}



cofbucket&
cofbuckets::set_bucket(uint32_t bucket_id)
{
	if (buckets.find(bucket_id) == buckets.end()) {
		buckets[bucket_id] = cofbucket(ofp_version);
	}
	return buckets[bucket_id];
}



cofbucket const&
cofbuckets::get_bucket(uint32_t bucket_id) const
{
	if (buckets.find(bucket_id) == buckets.end()) {
		throw eBucketsNotFound();
	}
	return buckets.at(bucket_id);
}



bool
cofbuckets::has_bucket(uint32_t bucket_id)
{
	return (not (buckets.find(bucket_id) == buckets.end()));
}


