/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFBUCKETLIST_H
#define COFBUCKETLIST_H 1

#include <string>
#include <vector>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "openflow.h"
#include "../coflist.h"
#include "../cvastring.h"
#include "cofbucket.h"

namespace rofl
{

class eBcListBase : public cerror {}; // base error class for cofbclist
class eBcListInval : public eBcListBase {};
class eBcListOutOfRange : public eBcListBase {};


class cofbclist : public coflist<cofbucket> {

	uint8_t ofp_version;

public: // static methods


public: // methods

	/** constructor
	 */
	cofbclist(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
			int bcnum = 0);

	/** destructor
	 */
	virtual
	~cofbclist();

	/**
	 */
	cofbclist(
			cofbclist const& bclist);

	/**
	 */
	cofbclist&
	operator= (
			cofbclist const& bclist);



	/**
	 *
	 * @param buckets
	 * @param bclen
	 * @return
	 */
	std::vector<cofbucket>&		// returns reference to this->bcvec
	unpack(
		uint8_t* buckets,
		size_t bclen)
	throw (eBucketBadLen, eBadActionBadOutPort);


	/**
	 *
	 * @param buckets
	 * @param bclen
	 * @return
	 */
	uint8_t*				// returns parameter "struct ofp_bucket *buckets"
	pack(
		uint8_t* buckets,
		size_t bclen) const
	throw (eBcListInval);



	/** returns required length for array of struct ofp_bucket
	 * for all buckets defined in this->bcvec
	 */
	size_t
	length() const;


private:

	/** stores cofbucket instances in this->bcvec from a packed array struct ofp_bucket (e.g. in struct ofp_group_mod)
	 */
	std::vector<cofbucket>&		// returns reference to this->bcvec
	unpack(
		struct ofp12_bucket *buckets, // parses memory area buckets and creates cofbucket instance in this->bcvec
		size_t bclen) 					// length of memory area to be parsed
	throw (eBucketBadLen, eBadActionBadOutPort);

	/** builds an array of struct ofp_buckets from this->bcvec
	 */
	struct ofp12_bucket*				// returns parameter "struct ofp_bucket *buckets"
	pack(
		struct ofp12_bucket *buckets, // pointer to memory area for storing this->bcvec
		size_t bclen) const			// length of memory area
	throw (eBcListInval);

	/** stores cofbucket instances in this->bcvec from a packed array struct ofp_bucket (e.g. in struct ofp_group_mod)
	 */
	std::vector<cofbucket>&		// returns reference to this->bcvec
	unpack(
		struct ofp13_bucket *buckets, // parses memory area buckets and creates cofbucket instance in this->bcvec
		size_t bclen) 					// length of memory area to be parsed
	throw (eBucketBadLen, eBadActionBadOutPort);

	/** builds an array of struct ofp_buckets from this->bcvec
	 */
	struct ofp13_bucket*				// returns parameter "struct ofp_bucket *buckets"
	pack(
		struct ofp13_bucket *buckets, // pointer to memory area for storing this->bcvec
		size_t bclen) const			// length of memory area
	throw (eBcListInval);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofbclist const& buckets) {
		os << "<cofbclist ";
			os << dynamic_cast<coflist const&>( buckets ) << " ";
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif
