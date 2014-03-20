/*
 * cofbucketcounters.h
 *
 *  Created on: 16.03.2014
 *      Author: andi
 */

#ifndef COFBUCKETCOUNTERS_H_
#define COFBUCKETCOUNTERS_H_

#include <inttypes.h>

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofbucketcounter.h"
#include "rofl/common/logging.h"

namespace rofl {
namespace openflow {

class cofbucket_counters
{
	uint8_t 								ofp_version;
	std::map<uint32_t, cofbucket_counter>	bucketcounters;

public:

	/**
	 *
	 */
	cofbucket_counters(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofbucket_counters();

	/**
	 *
	 */
	cofbucket_counters(
			cofbucket_counters const& bcs);

	/**
	 *
	 */
	cofbucket_counters&
	operator= (
			cofbucket_counters const& bcs);

	/**
	 *
	 */
	bool
	operator== (
			cofbucket_counters const& bcs);

public:

	/**
	 *
	 */
	void
	clear() { return bucketcounters.clear(); };

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
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t* buf, size_t buflen);

public:

	/**
	 *
	 */
	cofbucket_counter&
	add_bucket_counter(uint32_t bucket_counter_id);

	/**
	 *
	 */
	void
	drop_bucket_counter(uint32_t bucket_counter_id);

	/**
	 *
	 */
	cofbucket_counter&
	set_bucket_counter(uint32_t bucket_counter_id);

	/**
	 *
	 */
	cofbucket_counter const&
	get_bucket_counter(uint32_t bucket_counter_id) const;

	/**
	 *
	 */
	bool
	has_bucket_counter(uint32_t bucket_counter_id);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofbucket_counters const& bcs) {
		os << rofl::indent(0) << "<cofbucket_counters #bucket-counters:" << bcs.bucketcounters.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, cofbucket_counter>::const_iterator
				it = bcs.bucketcounters.begin(); it != bcs.bucketcounters.end(); ++it) {
			os << it->second;
		}
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl



#endif /* COFBUCKETCOUNTERS_H_ */
