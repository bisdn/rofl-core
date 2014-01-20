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

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofbucket.h"

namespace rofl
{

class eBucketsBase 			: public RoflException {};
class eBucketsInval 		: public eBucketsBase {};
class eBucketsOutOfRange 	: public eBucketsBase {};


class cofbuckets : public std::list<cofbucket*>
{
	uint8_t ofp_version;

public: // iterators

	typedef typename std::list<cofbucket*>::iterator iterator;
	typedef typename std::list<cofbucket*>::const_iterator const_iterator;

	typedef typename std::list<cofbucket*>::reverse_iterator reverse_iterator;
	typedef typename std::list<cofbucket*>::const_reverse_iterator const_reverse_iterator;

public: // methods

	/** constructor
	 */
	cofbuckets(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN);

	/** destructor
	 */
	virtual
	~cofbuckets();

	/**
	 */
	cofbuckets(
			cofbuckets const& buckets);

	/**
	 */
	cofbuckets&
	operator= (
			cofbuckets const& buckets);


	/**
	 *
	 */
	void
	pop_front();


	/**
	 *
	 */
	void
	pop_back();


	/**
	 *
	 */
	cofbucket&
	front();


	/**
	 *
	 */
	cofbucket&
	back();


	/**
	 *
	 */
	void
	clear();


	/**
	 *
	 * @return
	 */
	void
	unpack(uint8_t* buf, size_t buflen);


	/**
	 *
	 * @param buckets
	 * @param bclen
	 * @return
	 */
	uint8_t*
	pack(uint8_t* buf, size_t buflen);



	/** returns required length for array of struct ofp_bucket
	 * for all buckets defined in this->bcvec
	 */
	size_t
	length() const;


	/**
	 *
	 */
	void
	append_bucket(cofbucket const& bucket);


	/**
	 *
	 */
	void
	prepend_bucket(cofbucket const& bucket);


private:

	/**
	 */
	void
	unpack_of13(uint8_t* buf, size_t buflen);

	/**
	 */
	uint8_t*
	pack_of13(uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofbuckets const& buckets) {
		os << indent(0) << "<cofbuckets ";
		os << "ofp-version:" << (int)buckets.ofp_version << " ";
		os << "#buckets:" << buckets.size() << " >" << std::endl;
		indent i(2);
		for (cofbuckets::const_iterator
				it = buckets.begin(); it != buckets.end(); ++it) {
			os << *(*it);
		}
		return os;
	};
};

}; // end of namespace

#endif
