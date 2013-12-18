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


class cofbuckets : public coflist<cofbucket> {

	uint8_t ofp_version;

public: // static methods


public: // methods

	/** constructor
	 */
	cofbuckets(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
			int bcnum = 0);

	/** destructor
	 */
	virtual
	~cofbuckets();

	/**
	 */
	cofbuckets(
			cofbuckets const& bclist);

	/**
	 */
	cofbuckets&
	operator= (
			cofbuckets const& bclist);



	/**
	 *
	 * @param buckets
	 * @param bclen
	 * @return
	 */
	std::vector<cofbucket>&
	unpack(uint8_t* buckets, size_t bclen);


	/**
	 *
	 * @param buckets
	 * @param bclen
	 * @return
	 */
	uint8_t*
	pack(uint8_t* buckets, size_t bclen);



	/** returns required length for array of struct ofp_bucket
	 * for all buckets defined in this->bcvec
	 */
	size_t
	length() const;


private:

	/**
	 */
	std::vector<cofbucket>&
	unpack_of12(uint8_t* buf, size_t buflen);

	/**
	 */
	uint8_t*
	pack_of12(uint8_t* buf, size_t buflen);

	/**
	 */
	std::vector<cofbucket>&
	unpack_of13(uint8_t* buf, size_t buflen);

	/**
	 */
	uint8_t*
	pack_of13(uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofbuckets const& buckets) {
		os << indent(0) << "<cofbuckets ofp-version:" << (int)buckets.ofp_version << ">";
		for (coflist<cofbucket>::const_iterator
				it = buckets.elems.begin(); it != buckets.elems.end(); ++it) {
			os << indent(2) << (*it) << std::endl;
		}
		return os;
	};
};

}; // end of namespace

#endif
