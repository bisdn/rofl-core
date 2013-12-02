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
cofbuckets::unpack(
	uint8_t* buckets,
	size_t bclen)
throw (eBucketBadLen, eBadActionBadOutPort)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: {
		return unpack((struct openflow12::ofp_bucket*)buckets, bclen);
	} break;
	case openflow13::OFP_VERSION: {
		return unpack((struct openflow13::ofp_bucket*)buckets, bclen);
	} break;
	default:
		throw eBadVersion();
	}
}




uint8_t*
cofbuckets::pack(
	uint8_t* buckets,
	size_t bclen) const
throw (eBcListInval)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: {
		return (uint8_t*)pack((struct openflow12::ofp_bucket*)buckets, bclen);
	} break;
	case openflow13::OFP_VERSION: {
		return (uint8_t*)pack((struct openflow13::ofp_bucket*)buckets, bclen);
	} break;
	default:
		throw eBadVersion();
	}
}



std::vector<cofbucket>&
cofbuckets::unpack(
		struct openflow12::ofp_bucket *buckets,
		size_t bclen)
throw (eBucketBadLen, eBadActionBadOutPort)
{
	clear(); // clears elems

	// sanity check: bclen must be of size at least of ofp_bucket
	if (bclen < (int)sizeof(struct openflow12::ofp_bucket))
		return elems;

	// first bucket
	struct openflow12::ofp_bucket *bchdr = buckets;


	while (bclen > 0)
	{
		if (be16toh(bchdr->len) < sizeof(struct openflow12::ofp_bucket))
			throw eBucketBadLen();

		cofbucket bucket(ofp_version, (uint8_t*)bchdr, be16toh(bchdr->len));

		next() = cofbucket(ofp_version, (uint8_t*)bchdr, be16toh(bchdr->len) );

		bclen -= be16toh(bchdr->len);
		bchdr = (struct openflow12::ofp_bucket*)(((uint8_t*)bchdr) + be16toh(bchdr->len));
	}

	return elems;
}


struct openflow12::ofp_bucket*
cofbuckets::pack(
	struct openflow12::ofp_bucket *buckets,
	size_t bclen) const throw (eBcListInval)
{
	size_t needed_bclen = length();

	if (bclen < needed_bclen)
		throw eBcListInval();

	struct openflow12::ofp_bucket *bchdr = buckets; // first bucket header

	cofbuckets::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cofbucket const& bucket = (*it);

		bchdr = (struct openflow12::ofp_bucket*)
				((uint8_t*)(bucket.pack((uint8_t*)bchdr, bucket.length())) + bucket.length());
	}

	return buckets;
}



std::vector<cofbucket>&
cofbuckets::unpack(
		struct openflow13::ofp_bucket *buckets,
		size_t bclen)
throw (eBucketBadLen, eBadActionBadOutPort)
{
	clear(); // clears elems

	// sanity check: bclen must be of size at least of ofp_bucket
	if (bclen < (int)sizeof(struct openflow13::ofp_bucket))
		return elems;

	// first bucket
	struct openflow13::ofp_bucket *bchdr = buckets;


	while (bclen > 0)
	{
		if (be16toh(bchdr->len) < sizeof(struct openflow13::ofp_bucket))
			throw eBucketBadLen();

		cofbucket bucket(ofp_version, (uint8_t*)bchdr, be16toh(bchdr->len));

		next() = cofbucket(ofp_version, (uint8_t*)bchdr, be16toh(bchdr->len) );

		bclen -= be16toh(bchdr->len);
		bchdr = (struct openflow13::ofp_bucket*)(((uint8_t*)bchdr) + be16toh(bchdr->len));
	}

	return elems;
}


struct openflow13::ofp_bucket*
cofbuckets::pack(
	struct openflow13::ofp_bucket *buckets,
	size_t bclen) const throw (eBcListInval)
{
	size_t needed_bclen = length();

	if (bclen < needed_bclen)
		throw eBcListInval();

	struct openflow13::ofp_bucket *bchdr = buckets; // first bucket header

	cofbuckets::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cofbucket const& bucket = (*it);

		bchdr = (struct openflow13::ofp_bucket*)
				((uint8_t*)(bucket.pack((uint8_t*)bchdr, bucket.length())) + bucket.length());
	}

	return buckets;
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

