/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofflowstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFFLOWSTATS_H_
#define COFFLOWSTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/croflexception.h"

namespace rofl {
namespace openflow {

class eFlowStatsBase		: public RoflException {};
class eFlowStatsInval		: public eFlowStatsBase {};
class eFlowStatsNotFound	: public eFlowStatsBase {};

class cofflow_stats_request
{
private: // data structures

	uint8_t 	of_version;
	cofmatch 	match;
	uint8_t 	table_id;
	uint32_t	out_port;
	uint32_t	out_group;
	uint64_t	cookie;
	uint64_t	cookie_mask;

public: // data structures


public:
	/**
	 *
	 */
	cofflow_stats_request(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);

	/**
	 *
	 */
	cofflow_stats_request(
			uint8_t of_version,
			cofmatch const& match,
			uint8_t table_id,
			uint16_t out_port);


	/**
	 *
	 */
	cofflow_stats_request(
			uint8_t of_version,
			cofmatch const& match,
			uint8_t table_id,
			uint32_t out_port,
			uint32_t out_group,
			uint64_t cookie,
			uint64_t cookie_mask);


	/**
	 *
	 */
	virtual
	~cofflow_stats_request();


	/**
	 *
	 */
	cofflow_stats_request(
			cofflow_stats_request const& flowstatsrequest);

	/**
	 *
	 */
	cofflow_stats_request&
	operator= (
			cofflow_stats_request const& flowstatsrequest);


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


	/**
	 *
	 */
	void
	set_version(uint8_t of_version);

	/**
	 *
	 */
	uint8_t
	get_version() const;

	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id);

	/**
	 *
	 */
	uint8_t
	get_table_id() const;

	/**
	 *
	 */
	void
	set_out_port(uint32_t out_port);

	/**
	 *
	 */
	uint32_t
	get_out_port() const;

	/**
	 *
	 */
	void
	set_out_group(uint32_t out_group);

	/**
	 *
	 */
	uint32_t
	get_out_group() const;

	/**
	 *
	 */
	void
	set_cookie(uint64_t cookie);

	/**
	 *
	 */
	uint64_t
	get_cookie() const;

	/**
	 *
	 */
	void
	set_cookie_mask(uint64_t cookie_mask);

	/**
	 *
	 */
	uint64_t
	get_cookie_mask() const;

	/**
	 *
	 */
	void
	set_match(cofmatch const& match);

	/**
	 *
	 */
	cofmatch&
	set_match();

	/**
	 *
	 */
	cofmatch const&
	get_match() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofflow_stats_request const& flow_stats_request) {
		switch (flow_stats_request.of_version) {
		case openflow10::OFP_VERSION: {
			os << indent(0) << "<cofflow_stats_request >" << std::endl;
			os << indent(2) << "<table-id:" << (int)flow_stats_request.get_table_id() << " >" << std::endl;
			os << indent(2) << "<out-port:0x" << std::hex << (int)flow_stats_request.get_out_port() << std::dec << " >" << std::endl;
			indent i(2);
			os << flow_stats_request.match;

		} break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			os << indent(0) << "<cofflow_stats_request >" << std::endl;
			os << indent(2) << "<table-id:" << (int)flow_stats_request.get_table_id() << " >" << std::endl;
			os << indent(2) << "<out-port:0x" << std::hex << (int)flow_stats_request.get_out_port() << std::dec << " >" << std::endl;
			os << indent(2) << "<out-group:0x" << std::hex << (int)flow_stats_request.get_out_group() << std::dec << " >" << std::endl;
			os << indent(2) << "<cookie:0x" << std::hex << (unsigned long long)flow_stats_request.get_cookie() << std::dec << " >" << std::endl;
			os << indent(2) << "<cookie-mask:0x" << std::hex << (unsigned long long)flow_stats_request.get_cookie_mask() << std::dec << " >" << std::endl;
			indent i(2);
			os << flow_stats_request.match;

		} break;
		default: {
			os << "<cofflow_stats_request unknown OFP version >" << std::endl;
		};
		}
		return os;
	};
};



class cofflow_stats_reply
{
private: // data structures

	uint8_t 	of_version;
	uint8_t 	table_id;
	uint32_t 	duration_sec;
	uint32_t 	duration_nsec;
	uint16_t	priority;
	uint16_t 	idle_timeout;
	uint16_t	hard_timeout;
	uint16_t	flags; 				// since OF1.3
	uint64_t	cookie;
	uint64_t 	packet_count;
	uint64_t	byte_count;
	cofmatch 	match;
	cofactions	actions;			// for OF1.0
	cofinstructions	instructions;	// since OF1.2

#define OFP12_FLOW_STATS_REPLY_STATIC_HDR_LEN 		48 // bytes

public: // data structures


public:
	/**
	 *
	 */
	cofflow_stats_reply(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);


	/**
	 *
	 */
	cofflow_stats_reply(
			uint8_t of_version,
			uint8_t table_id,
			uint32_t duration_sec,
			uint32_t duration_nsec,
			uint16_t priority,
			uint16_t idle_timeout,
			uint16_t hard_timeout,
			uint64_t cookie,
			uint64_t packet_count,
			uint64_t byte_count,
			cofmatch const& match,
			cofactions const& actions);


	/**
	 *
	 */
	cofflow_stats_reply(
			uint8_t of_version,
			uint8_t table_id,
			uint32_t duration_sec,
			uint32_t duration_nsec,
			uint16_t priority,
			uint16_t idle_timeout,
			uint16_t hard_timeout,
			uint64_t cookie,
			uint64_t packet_count,
			uint64_t byte_count,
			cofmatch const& match,
			cofinstructions const& instructions);



	/**
	 *
	 */
	virtual
	~cofflow_stats_reply();


	/**
	 *
	 */
	cofflow_stats_reply(
			cofflow_stats_reply const& flowstats);

	/**
	 *
	 */
	cofflow_stats_reply&
	operator= (
			cofflow_stats_reply const& flowstats);

	/**
	 *
	 */
	bool
	operator== (
			cofflow_stats_reply const& flowstats);

public:

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


	/**
	 *
	 */
	void
	set_version(uint8_t of_version);


	/**
	 *
	 */
	uint8_t
	get_version() const;


	/**
	 *
	 */
	uint8_t
	get_table_id() const;

	/**
	 *
	 */
	uint32_t
	get_duration_sec() const;

	/**
	 *
	 */
	uint32_t
	get_duration_nsec() const;

	/**
	 *
	 */
	uint16_t
	get_priority() const;

	/**
	 *
	 */
	uint16_t
	get_idle_timeout() const;

	/**
	 *
	 */
	uint16_t
	get_hard_timeout() const;

	/**
	 *
	 */
	uint16_t
	get_flags() const;

	/**
	 *
	 */
	uint64_t
	get_cookie() const;

	/**
	 *
	 */
	uint64_t
	get_packet_count() const;

	/**
	 *
	 */
	uint64_t
	get_byte_count() const;







	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id);

	/**
	 *
	 */
	void
	set_duration_sec(uint32_t duration_sec);

	/**
	 *
	 */
	void
	set_duration_nsec(uint32_t duration_nsec);

	/**
	 *
	 */
	void
	set_priority(uint16_t priority);

	/**
	 *
	 */
	void
	set_idle_timeout(uint16_t idle_timeout);

	/**
	 *
	 */
	void
	set_hard_timeout(uint16_t hard_timeout);

	/**
	 *
	 */
	void
	set_flags(uint16_t flags);

	/**
	 *
	 */
	void
	set_cookie(uint64_t cookie);

	/**
	 *
	 */
	void
	set_packet_count(uint64_t packet_count);

	/**
	 *
	 */
	void
	set_byte_count(uint64_t byte_count);






	/**
	 *
	 */
	cofmatch&
	set_match() { return match; };

	/**
	 *
	 */
	cofactions&
	set_actions() { return actions; };

	/**
	 *
	 */
	cofinstructions&
	set_instructions() { return instructions; };


	/**
	 *
	 */
	cofmatch const&
	get_match() const { return match; };

	/**
	 *
	 */
	cofactions const&
	get_actions() const { return actions; };

	/**
	 *
	 */
	cofinstructions const&
	get_instructions() const { return instructions; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofflow_stats_reply const& flow_stats_reply) {
		switch (flow_stats_reply.of_version) {
		case rofl::openflow10::OFP_VERSION:
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << indent(0) << "<cofflow_stats_reply >" << std::endl;
			os << indent(2) << "<table-id: " << (int)flow_stats_reply.get_table_id() << " >" << std::endl;
			os << indent(2) << "<duration-sec: " << (int)flow_stats_reply.get_duration_sec() << " >" << std::endl;
			os << indent(2) << "<duration-nsec: " << (int)flow_stats_reply.get_duration_nsec() << " >" << std::endl;
			os << indent(2) << "<priority: 0x" << std::hex << (int)flow_stats_reply.get_priority() << std::dec << " >" << std::endl;
			os << indent(2) << "<idle-timeout: " << (int)flow_stats_reply.get_idle_timeout() << " >" << std::endl;
			os << indent(2) << "<hard-timeout: " << (int)flow_stats_reply.get_hard_timeout() << " >" << std::endl;
			os << indent(2) << "<cookie: 0x" << std::hex << (unsigned long long)flow_stats_reply.get_cookie() << std::dec << " >" << std::endl;
			os << indent(2) << "<packet-count: " << (int)flow_stats_reply.get_packet_count() << " >" << std::endl;
			os << indent(2) << "<byte-count: " << (int)flow_stats_reply.get_byte_count() << " >" << std::endl;
			indent i(2);
			os << flow_stats_reply.match;
		} break;
		default: {

		};
		}
		indent i(2);
		switch (flow_stats_reply.of_version) {
		case openflow10::OFP_VERSION: os << flow_stats_reply.actions; break;
		case openflow12::OFP_VERSION: os << flow_stats_reply.instructions; break;
		case openflow13::OFP_VERSION: os << flow_stats_reply.instructions; break;
		default: os << "<unknown OFP version >" << std::endl;
		}
		return os;
	};
};

} /* end of namespace */
} /* end of namespace */

#endif /* COFFLOWSTATS_H_ */
