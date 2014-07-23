/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofbucketcounter.h
 *
 *  Created on: 16.03.2014
 *      Author: andreas
 */

#ifndef COFBUCKETCOUNTER_H_
#define COFBUCKETCOUNTER_H_

#include <inttypes.h>

#include <iostream>

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/logging.h"

namespace rofl {
namespace openflow {

class eBucketCounterBase			: public RoflException {};
class eBucketCounterInval			: public eBucketCounterBase {};
class eBucketCounterNotFound		: public eBucketCounterBase {};

class cofbucket_counter
{
	uint8_t		ofp_version;
	uint64_t	packet_count;
	uint64_t	byte_count;

public:

	/**
	 *
	 */
	cofbucket_counter(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofbucket_counter();

	/**
	 *
	 */
	cofbucket_counter(
			cofbucket_counter const& bc);

	/**
	 *
	 */
	cofbucket_counter&
	operator= (
			cofbucket_counter const& bc);

	/**
	 *
	 */
	bool
	operator== (
			cofbucket_counter const& bc);

public:

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

	/**
	 *
	 */
	uint64_t
	get_packet_count() const { return packet_count; };

	/**
	 *
	 */
	void
	set_packet_count(uint64_t packet_count) { this->packet_count = packet_count; };

	/**
	 *
	 */
	uint64_t
	get_byte_count() const { return byte_count; };

	/**
	 *
	 */
	void
	set_byte_count(uint64_t byte_count) { this->byte_count = byte_count; };

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

	friend std::ostream&
	operator<< (std::ostream& os, cofbucket_counter const& bc) {
		os << rofl::indent(0) << "<cofbucket_counter ";
		os << "ofp-version: " << (int)bc.ofp_version << " ";
		os << std::hex;
		os << "packet-count: 0x" << (unsigned long long)bc.packet_count << " ";
		os << "byte-count: 0x" << (unsigned long long)bc.byte_count << " ";
		os << std::dec;
		os << ">" << std::endl;
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFBUCKETCOUNTER_H_ */
