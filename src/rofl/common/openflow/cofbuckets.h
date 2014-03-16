/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFBUCKETLIST_H
#define COFBUCKETLIST_H 1

#include <map>
#include <string>
#include <vector>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofbucket.h"

namespace rofl {
namespace openflow {

class eBucketsBase 			: public RoflException {};
class eBucketsInval 		: public eBucketsBase {};
class eBucketsNotFound 		: public eBucketsBase {};
class eBucketsOutOfRange 	: public eBucketsBase {};


class cofbuckets
{
	uint8_t 							ofp_version;
	std::map<uint32_t, cofbucket>		buckets;

public: // methods

	/**
	 *
	 */
	cofbuckets(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofbuckets();

	/**
	 *
	 */
	cofbuckets(
			cofbuckets const& buckets);

	/**
	 *
	 */
	cofbuckets&
	operator= (
			cofbuckets const& buckets);

	/**
	 *
	 */
	bool
	operator== (
			cofbuckets const& buckets);

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

public:

	/**
	 *
	 */
	cofbucket&
	add_bucket(uint32_t bucket_id);

	/**
	 *
	 */
	void
	drop_bucket(uint32_t bucket_id);

	/**
	 *
	 */
	cofbucket&
	set_bucket(uint32_t bucket_id);

	/**
	 *
	 */
	cofbucket const&
	get_bucket(uint32_t bucket_id) const;

	/**
	 *
	 */
	bool
	has_bucket(uint32_t bucket_id);

public:

	/**
	 *
	 */
	void
	clear() { buckets.clear(); };

	/**
	 *
	 */
	void
	check_prerequisites() const;

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	uint8_t*
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(uint8_t* buf, size_t buflen);


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
		switch (buckets.get_version()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			os << indent(0) << "<cofbuckets ";
					os << "ofp-version:" << (int)buckets.ofp_version << " >" << std::endl;

		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << indent(0) << "<cofbuckets ";
			os << "ofp-version:" << (int)buckets.ofp_version << " ";
			os << "#buckets:" << buckets.buckets.size() << " >" << std::endl;
			indent i(2);
			for (std::map<uint32_t, cofbucket>::const_iterator
					it = buckets.buckets.begin(); it != buckets.buckets.end(); ++it) {
				os << it->second;
			}

		} break;
		default:
			throw eBadVersion();
		}
		return os;
	};
};

}; // end of namespace
}; // end of namespace

#endif
