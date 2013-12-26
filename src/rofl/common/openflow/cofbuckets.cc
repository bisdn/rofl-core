/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofbuckets.h"

using namespace rofl;

cofbuckets::cofbuckets(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}



cofbuckets::~cofbuckets()
{
	clear();
}



void
cofbuckets::pop_front()
{
	if (empty())
		return;
	delete std::list<cofbucket*>::front();
	std::list<cofbucket*>::pop_front();
}



void
cofbuckets::pop_back()
{
	if (empty())
		return;
	delete std::list<cofbucket*>::back();
	std::list<cofbucket*>::pop_back();
}



cofbucket&
cofbuckets::front()
{
	if (empty())
		throw eBucketsOutOfRange();
	return *(std::list<cofbucket*>::front());
}



cofbucket&
cofbuckets::back()
{
	if (empty())
		throw eBucketsOutOfRange();
	return *(std::list<cofbucket*>::back());
}



void
cofbuckets::clear()
{
	for (cofbuckets::iterator it = begin(); it != end(); ++it) {
		delete (*it);
	}
	std::list<cofbucket*>::clear();
}



cofbuckets::cofbuckets(cofbuckets const& buckets)
{
	*this = buckets;
}



cofbuckets&
cofbuckets::operator= (cofbuckets const& buckets)
{
	if (this == &buckets)
		return *this;

	this->ofp_version = buckets.ofp_version;

	clear();

	for (cofbuckets::const_iterator
			it = buckets.begin(); it != buckets.end(); ++it) {
		append_bucket(*(*it));
	}

	return *this;
}



void
cofbuckets::append_bucket(cofbucket const& bucket)
{
	push_back(new cofbucket(bucket));
}



void
cofbuckets::prepend_bucket(cofbucket const& bucket)
{
	push_front(new cofbucket(bucket));
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
cofbuckets::pack(uint8_t* buf, size_t buflen)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION:	return pack_of13(buf, buflen);
	default:						throw eBadVersion();
	}
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

	while (buflen > 0) {
		if (be16toh(bchdr->len) < sizeof(struct openflow::ofp_bucket))
			throw eBucketBadLen();

		append_bucket(cofbucket(ofp_version, (uint8_t*)bchdr, be16toh(bchdr->len)));

		buflen -= be16toh(bchdr->len);
		bchdr = (struct openflow::ofp_bucket*)(((uint8_t*)bchdr) + be16toh(bchdr->len));
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

	for (cofbuckets::iterator it = begin(); it != end(); ++it) {
		cofbucket& bucket = *(*it);
		bchdr = (struct openflow::ofp_bucket*)
				((uint8_t*)(bucket.pack((uint8_t*)bchdr, bucket.length())) + bucket.length());
	}

	return buf;
}



size_t
cofbuckets::length() const
{
	size_t len = 0;
	for (cofbuckets::const_iterator
			it = begin(); it != end(); ++it) {
		len += (*it)->length();
	}
	return len;
}



