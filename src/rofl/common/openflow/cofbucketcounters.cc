/*
 * cofbucketcounters.cc
 *
 *  Created on: 16.03.2014
 *      Author: andi
 */

#include "rofl/common/openflow/cofbucketcounters.h"

using namespace rofl::openflow;


cofbucket_counters::cofbucket_counters(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}


cofbucket_counters::~cofbucket_counters()
{

}


cofbucket_counters::cofbucket_counters(
		cofbucket_counters const& bcs)
{
	*this = bcs;
}


cofbucket_counters&
cofbucket_counters::operator= (
		cofbucket_counters const& bcs)
{
	if (this == &bcs)
		return *this;

	clear();

	ofp_version = bcs.ofp_version;

	for (std::map<uint32_t, cofbucket_counter>::const_iterator
			it = bcs.bucketcounters.begin(); it != bcs.bucketcounters.end(); ++it) {
		add_bucket_counter(it->first) = it->second;
	}

	return *this;
}


bool
cofbucket_counters::operator== (
		cofbucket_counters const& bcs)
{
	if (bucketcounters.size() != bcs.bucketcounters.size())
		return false;

	for (std::map<uint32_t, cofbucket_counter>::const_iterator
			it = bcs.bucketcounters.begin(); it != bcs.bucketcounters.end(); ++it) {
		if (not (bucketcounters[it->first] == it->second))
			return false;
	}

	return true;
}


size_t
cofbucket_counters::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofbucket_counter>::const_iterator
			it = bucketcounters.begin(); it != bucketcounters.end(); ++it) {
		len += it->second.length();
	}
	return len;
}


void
cofbucket_counters::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		for (std::map<uint32_t, cofbucket_counter>::iterator
				it = bucketcounters.begin(); it != bucketcounters.end(); ++it) {
			it->second.pack(buf, it->second.length());
			buf += it->second.length();
			buflen -= it->second.length();
		}
	} break;
	default: {
		throw eBadVersion();
	};
	}
}


void
cofbucket_counters::unpack(uint8_t* buf, size_t buflen)
{
	clear();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION: {

		uint32_t bucket_counter_id = 0;

		while (buflen >= sizeof(struct rofl::openflow12::ofp_bucket_counter)) {
			add_bucket_counter(bucket_counter_id++).unpack(buf, sizeof(struct rofl::openflow12::ofp_bucket_counter));
			buf += sizeof(struct rofl::openflow12::ofp_bucket_counter);
			buflen -= sizeof(struct rofl::openflow12::ofp_bucket_counter);
		}

	} break;
	case rofl::openflow13::OFP_VERSION: {

		uint32_t bucket_counter_id = 0;

		while (buflen >= sizeof(struct rofl::openflow13::ofp_bucket_counter)) {
			add_bucket_counter(bucket_counter_id++).unpack(buf, sizeof(struct rofl::openflow13::ofp_bucket_counter));
			buf += sizeof(struct rofl::openflow13::ofp_bucket_counter);
			buflen -= sizeof(struct rofl::openflow13::ofp_bucket_counter);
		}

	} break;
	default: {
		throw eBadVersion();
	};
	}


}


cofbucket_counter&
cofbucket_counters::add_bucket_counter(uint32_t bucket_counter_id)
{
	if (bucketcounters.find(bucket_counter_id) != bucketcounters.end()) {
		bucketcounters.erase(bucket_counter_id);
	}
	return (bucketcounters[bucket_counter_id] = cofbucket_counter(ofp_version));
}


void
cofbucket_counters::drop_bucket_counter(uint32_t bucket_counter_id)
{
	if (bucketcounters.find(bucket_counter_id) == bucketcounters.end()) {
		return;
	}
	bucketcounters.erase(bucket_counter_id);
}


cofbucket_counter&
cofbucket_counters::set_bucket_counter(uint32_t bucket_counter_id)
{
	if (bucketcounters.find(bucket_counter_id) == bucketcounters.end()) {
		bucketcounters[bucket_counter_id] = cofbucket_counter(ofp_version);
	}
	return bucketcounters[bucket_counter_id];
}


cofbucket_counter const&
cofbucket_counters::get_bucket_counter(uint32_t bucket_counter_id) const
{
	if (bucketcounters.find(bucket_counter_id) == bucketcounters.end()) {
		throw eBucketCounterNotFound();
	}
	return bucketcounters.at(bucket_counter_id);
}


bool
cofbucket_counters::has_bucket_counter(uint32_t bucket_counter_id)
{
	return (not (bucketcounters.find(bucket_counter_id) == bucketcounters.end()));
}



