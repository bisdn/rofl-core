/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFBUCKETLIST_H
#define COFBUCKETLIST_H 1

#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

#include "openflow12.h"
#include <endian.h>

#ifndef htobe16
#include "../endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "../coflist.h"
#include "../cvastring.h"
#include "cofbucket.h"

class eBcListBase : public cerror {}; // base error class for cofbclist
class eBcListInval : public eBcListBase {};
class eBcListOutOfRange : public eBcListBase {};


class cofbclist : public coflist<cofbucket> {
public: // static methods


public: // methods

	/** constructor
	 */
	cofbclist(int bcnum = 0);

	/** destructor
	 */
	virtual
	~cofbclist();

	/**
	 */
	cofbclist(cofbclist const& bclist)
	{
		*this = bclist;
	};

	/**
	 */
	cofbclist& operator= (cofbclist const& bclist)
	{
		if (this == &bclist)
			return *this;
		coflist<cofbucket>::operator= (bclist);
		return *this;
	};

	/** stores cofbucket instances in this->bcvec from a packed array struct ofp_bucket (e.g. in struct ofp_group_mod)
	 */
	std::vector<cofbucket>&		// returns reference to this->bcvec
	unpack(
		struct ofp_bucket *buckets, // parses memory area buckets and creates cofbucket instance in this->bcvec
		size_t bclen) 					// length of memory area to be parsed
	throw (eBucketBadLen, eActionBadOutPort);

	/** builds an array of struct ofp_buckets from this->bcvec
	 */
	struct ofp_bucket*				// returns parameter "struct ofp_bucket *buckets"
	pack(
		struct ofp_bucket *buckets, // pointer to memory area for storing this->bcvec
		size_t bclen) 					// length of memory area
	throw (eBcListInval);

	/** returns required length for array of struct ofp_bucket
	 * for all buckets defined in this->bcvec
	 */
	size_t
	length();

	/** dump info string
	 */
	const char*
	c_str();


private:

	std::string info; // info string

};

#endif
