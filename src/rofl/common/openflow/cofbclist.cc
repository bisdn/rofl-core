/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofbclist.h"

using namespace rofl;

cofbclist::cofbclist(int bcnum)
{
	for (int i = 0; i < bcnum; ++i)
	{
		elems[i] = cofbucket();
	}
}


cofbclist::~cofbclist()
{

}



const char*
cofbclist::c_str()
{
	cvastring vas(4096);
	info.assign(vas("cofbclist(%p) length:%d => %d bucket(s): ", this, length(), elems.size()));
	cofbclist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		info.append(vas("\n  %s ", (*it).c_str()));
	}
	return info.c_str();
}


std::vector<cofbucket>&
cofbclist::unpack(
		struct ofp_bucket *buckets,
		size_t bclen)
throw (eBucketBadLen, eActionBadOutPort)
{
	reset(); // clears elems

	WRITELOG(COFBUCKET, DBG, "cofbclist(%p)::unpack() bclen:%d", this, bclen);

	// sanity check: bclen must be of size at least of ofp_bucket
	if (bclen < (int)sizeof(struct ofp_bucket))
		return elems;

	// first bucket
	struct ofp_bucket *bchdr = buckets;


	while (bclen > 0)
	{
		if (be16toh(bchdr->len) < sizeof(struct ofp_bucket))
			throw eBucketBadLen();

		cofbucket bucket(bchdr, be16toh(bchdr->len));

		WRITELOG(COFBUCKET, DBG, "cofbclist(%p)::unpack() new bucket[1]: %s", this, bucket.c_str());

		next() = cofbucket(bchdr, be16toh(bchdr->len) );

		WRITELOG(COFBUCKET, DBG, "cofbclist(%p)::unpack() new bucket: %s", this, back().c_str());

		bclen -= be16toh(bchdr->len);
		bchdr = (struct ofp_bucket*)(((uint8_t*)bchdr) + be16toh(bchdr->len));
	}

	return elems;
}


struct ofp_bucket*
cofbclist::pack(
	struct ofp_bucket *buckets,
	size_t bclen) throw (eBcListInval)
{
	size_t needed_bclen = length();

	if (bclen < needed_bclen)
		throw eBcListInval();

	struct ofp_bucket *bchdr = buckets; // first bucket header

	WRITELOG(COFBUCKET, DBG, "cofbclist(%p)::pack() %s", this, c_str());

	cofbclist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cofbucket& bucket = (*it);

		bchdr = (struct ofp_bucket*)
				((uint8_t*)(bucket.pack(bchdr, bucket.length())) + bucket.length());
	}

	return buckets;
}


size_t
cofbclist::length()
{
	size_t len = 0;
	cofbclist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		len += (*it).length();
	}
	return len;
}

