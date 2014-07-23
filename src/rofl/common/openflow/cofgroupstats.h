/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofgroupstats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFGROUPSTATS_H_
#define COFGROUPSTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/cofbucketcounters.h"

namespace rofl {
namespace openflow {

class eGroupStatsBase 			: public RoflException {};
class eGroupStatsInval			: public eGroupStatsBase {};
class eGroupStatsNotFound		: public eGroupStatsBase {};

class cofgroup_stats_request
{
private: // data structures

	uint8_t 		of_version;
	uint32_t		group_id;

public: // data structures


public:
	/**
	 *
	 */
	cofgroup_stats_request(
			uint8_t of_version = 0);

	/**
	 *
	 */
	cofgroup_stats_request(
			uint8_t of_version,
			uint32_t group_id);

	/**
	 *
	 */
	virtual
	~cofgroup_stats_request();


	/**
	 *
	 */
	cofgroup_stats_request(
			cofgroup_stats_request const& stats);

	/**
	 *
	 */
	cofgroup_stats_request&
	operator= (
			cofgroup_stats_request const& stats);


	/**
	 *
	 */
	void
	pack(uint8_t *buf, size_t buflen) const;

	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	size_t
	length() const;


public:


	/**
	 *
	 */
	void
	set_version(uint8_t of_version) { this->of_version = of_version; };


	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };


	/**
	 *
	 */
	uint32_t
	get_group_id() const { return group_id; };


	/**
	 *
	 */
	void
	set_group_id(uint32_t group_id) { this->group_id = group_id; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroup_stats_request const& r) {
		os << indent(0) << "<cofgroup_stats_request >" << std::endl;
		os << indent(2) << "<group-id: " << (int)r.get_group_id() << " >" << std::endl;
		return os;
	};
};



class cofgroup_stats_reply
{
private: // data structures

	uint8_t 				of_version;
	uint32_t				group_id;
	uint32_t				ref_count;
	uint64_t 				packet_count;
	uint64_t				byte_count;
	uint32_t				duration_sec;
	uint32_t				duration_nsec;
	cofbucket_counters		bucket_counters;


public: // data structures


public:
	/**
	 *
	 */
	cofgroup_stats_reply(
			uint8_t of_version = 0);

	/**
	 *
	 */
	cofgroup_stats_reply(
			uint8_t of_version,
			uint32_t group_id,
			uint32_t ref_count,
			uint64_t packet_count,
			uint64_t byte_count,
			uint32_t duration_sec,
			uint32_t duration_nsec);

	/**
	 *
	 */
	virtual
	~cofgroup_stats_reply();


	/**
	 *
	 */
	cofgroup_stats_reply(
			cofgroup_stats_reply const& stats);

	/**
	 *
	 */
	cofgroup_stats_reply&
	operator= (
			cofgroup_stats_reply const& stats);

	/**
	 *
	 */
	bool
	operator== (
			cofgroup_stats_reply const& stats);

	/**
	 *
	 */
	void
	pack(uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	size_t
	length() const;


public:

	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };

	/**
	 *
	 */
	uint32_t
	get_group_id() const { return group_id; };

	/**
	 *
	 */
	uint32_t
	get_ref_count() const { return ref_count; };

	/**
	 *
	 */
	uint64_t
	get_packet_count() const { return packet_count; };

	/**
	 *
	 */
	uint64_t
	get_byte_count() const { return byte_count; };

	/**
	 *
	 */
	uint32_t
	get_duration_sec() const { return duration_sec; };

	/**
	 *
	 */
	uint32_t
	get_duration_nsec() const { return duration_nsec; };

	/**
	 *
	 */
	cofbucket_counters&
	set_bucket_counters() { return bucket_counters; };

	/**
	 *
	 */
	cofbucket_counters const&
	get_bucket_counters() const { return bucket_counters; };

	/**
	 *
	 */
	void
	set_version(uint8_t of_version) { this->of_version = of_version; };

	/**
	 *
	 */
	void
	set_group_id(uint32_t group_id) { this->group_id = group_id; };

	/**
	 *
	 */
	void
	set_ref_count(uint32_t ref_count) { this->ref_count = ref_count; };

	/**
	 *
	 */
	void
	set_packet_count(uint64_t packet_count) { this->packet_count = packet_count; };

	/**
	 *
	 */
	void
	set_byte_count(uint64_t byte_count) { this->byte_count = byte_count; };

	/**
	 *
	 */
	void
	set_duration_sec(uint32_t duration_sec) { this->duration_sec = duration_sec; };

	/**
	 *
	 */
	void
	set_duration_nsec(uint32_t duration_nsec) { this->duration_nsec = duration_nsec; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroup_stats_reply const& r) {
		os << indent(0) << "<cofgroup_stats_reply >" << std::endl;
		os << std::hex;
		os << indent(2) << "<group-id: 0x" << (int)r.get_group_id() << " >" << std::endl;
		os << indent(2) << "<ref-count: 0x" << (int)r.get_ref_count() << " >" << std::endl;
		os << indent(2) << "<packet-count: 0x" << (int)r.get_packet_count() << " >" << std::endl;
		os << indent(2) << "<byte-count: 0x" << (int)r.get_byte_count() << " >" << std::endl;
		os << std::dec;
		switch (r.get_version()) {
		case rofl::openflow13::OFP_VERSION: {
			os << std::hex;
			os << indent(2) << "<duration-sec: 0x" << (int)r.get_duration_sec() << " >" << std::endl;
			os << indent(2) << "<duration-nsec: 0x" << (int)r.get_duration_nsec() << " >" << std::endl;
			os << std::dec;
		} break;
		default: {
		};
		}
		indent i(2);
		os << r.bucket_counters;
		return os;
	};
};

}
}

#endif /* COFGROUPSTATS_H_ */
