/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofbuckets.h"

using namespace rofl;

cofbuckets::cofbuckets(
		uint8_t ofp_version,
		int bcnum) :
				ofp_version(ofp_version)
{
	for (int i = 0; i < bcnum; ++i) {
		elems[i] = cofbucket(ofp_version);
	}
}



cofbuckets::~cofbuckets()
{

}



cofbuckets::cofbuckets(cofbuckets const& bclist)
{
	*this = bclist;
}



cofbuckets&
cofbuckets::operator= (cofbuckets const& bclist)
{
	if (this == &bclist)
		return *this;

	this->ofp_version = bclist.ofp_version;
	coflist<cofbucket>::operator= (bclist);

	return *this;
}



std::vector<cofbucket>&
cofbuckets::unpack(uint8_t* buf, size_t buflen)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: 	return unpack_of12(buf, buflen);
	case openflow13::OFP_VERSION:	return unpack_of13(buf, buflen);
	default:						throw eBadVersion();
	}
}




uint8_t*
cofbuckets::pack(uint8_t* buf, size_t buflen)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: 	return pack_of12(buf, buflen);
	case openflow13::OFP_VERSION:	return pack_of13(buf, buflen);
	default:						throw eBadVersion();
	}
}



std::vector<cofbucket>&
cofbuckets::unpack_of12(
		uint8_t* buf,
		size_t buflen)
{
	struct openflow12::ofp_bucket* buckets = (struct openflow12::ofp_bucket*)buf;

	clear(); // clears elems

	// sanity check: bclen must be of size at least of ofp_bucket
	if (buflen < (int)sizeof(struct openflow12::ofp_bucket))
		return elems;

	// first bucket
	struct openflow12::ofp_bucket *bchdr = buckets;

	while (buflen > 0)
	{
		if (be16toh(bchdr->len) < sizeof(struct openflow12::ofp_bucket))
			throw eBucketBadLen();

		cofbucket bucket(ofp_version, (uint8_t*)bchdr, be16toh(bchdr->len));

		next() = cofbucket(ofp_version, (uint8_t*)bchdr, be16toh(bchdr->len) );

		buflen -= be16toh(bchdr->len);
		bchdr = (struct openflow12::ofp_bucket*)(((uint8_t*)bchdr) + be16toh(bchdr->len));
	}

	return elems;
}


uint8_t*
cofbuckets::pack_of12(
	uint8_t* buf,
	size_t buflen)
{
	struct openflow12::ofp_bucket *buckets = (struct openflow12::ofp_bucket*)buf;
	size_t needed_bclen = length();

	if (buflen < needed_bclen)
		throw eBcListInval();

	struct openflow12::ofp_bucket *bchdr = buckets; // first bucket header

	for (cofbuckets::iterator
			it = elems.begin(); it != elems.end(); ++it) {
		cofbucket& bucket = (*it);
		bchdr = (struct openflow12::ofp_bucket*)
				((uint8_t*)(bucket.pack((uint8_t*)bchdr, bucket.length())) + bucket.length());
	}

	return buf;
}


std::vector<cofbucket>&
cofbuckets::unpack_of13(uint8_t* buf, size_t buflen)
{
	struct openflow13::ofp_bucket *buckets = (struct openflow13::ofp_bucket*)buf;

	clear(); // clears elems

	// sanity check: bclen must be of size at least of ofp_bucket
	if (buflen < (int)sizeof(struct openflow13::ofp_bucket))
		return elems;

	// first bucket
	struct openflow13::ofp_bucket *bchdr = buckets;


	while (buflen > 0)
	{
		if (be16toh(bchdr->len) < sizeof(struct openflow13::ofp_bucket))
			throw eBucketBadLen();

		cofbucket bucket(ofp_version, (uint8_t*)bchdr, be16toh(bchdr->len));

		next() = cofbucket(ofp_version, (uint8_t*)bchdr, be16toh(bchdr->len) );

		buflen -= be16toh(bchdr->len);
		bchdr = (struct openflow13::ofp_bucket*)(((uint8_t*)bchdr) + be16toh(bchdr->len));
	}

	return elems;
}


uint8_t*
cofbuckets::pack_of13(uint8_t* buf, size_t buflen)
{
	struct openflow13::ofp_bucket* buckets = (struct openflow13::ofp_bucket*)buf;
	size_t needed_bclen = length();

	if (buflen < needed_bclen)
		throw eBcListInval();

	struct openflow13::ofp_bucket *bchdr = buckets; // first bucket header

	for (cofbuckets::iterator
			it = elems.begin(); it != elems.end(); ++it) {
		cofbucket& bucket = (*it);
		bchdr = (struct openflow13::ofp_bucket*)
				((uint8_t*)(bucket.pack((uint8_t*)bchdr, bucket.length())) + bucket.length());
	}

	return buf;
}



size_t
cofbuckets::length() const
{
	size_t len = 0;
	cofbuckets::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		len += (*it).length();
	}
	return len;
}

