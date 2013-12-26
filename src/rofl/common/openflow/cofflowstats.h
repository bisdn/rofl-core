/*
 * cofflowstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFFLOWSTATS_H_
#define COFFLOWSTATS_H_ 1

#include "../cmemory.h"
#include "cofmatch.h"
#include "cofactions.h"
#include "cofinstructions.h"
#include "../../platform/unix/csyslog.h"
#include "openflow.h"
#include "openflow_rofl_exceptions.h"

namespace rofl
{

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
	get_match();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofflow_stats_request const& flow_stats_request) {
		os << indent(0) << "<cofflow_stats_request >" << std::endl;
		os << indent(2) << "<table-id:" << (int)flow_stats_request.get_table_id() << " >" << std::endl;
		os << indent(2) << "<out-port:0x" << std::hex << (int)flow_stats_request.get_out_port() << std::dec << " >" << std::endl;
		os << indent(2) << "<out-group:0x" << std::hex << (int)flow_stats_request.get_out_group() << std::dec << " >" << std::endl;
		os << indent(2) << "<cookie:0x" << std::hex << (unsigned long long)flow_stats_request.get_cookie() << std::dec << " >" << std::endl;
		os << indent(2) << "<cookie-mask:0x" << std::hex << (unsigned long long)flow_stats_request.get_cookie_mask() << std::dec << " >" << std::endl;
		indent i(2);
		os << flow_stats_request.match;
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
	uint64_t	cookie;
	uint64_t 	packet_count;
	uint64_t	byte_count;
	cofmatch 	match;
	cofactions	actions;		// for OF1.0
	cofinstructions	instructions;	// for OF1.2

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
	cofmatch&
	get_match();

	/**
	 *
	 */
	cofactions&
	get_actions();

	/**
	 *
	 */
	cofinstructions&
	get_instructions();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofflow_stats_reply const& flow_stats_reply) {
		os << indent(0) << "<cofflow_stats_reply >" << std::endl;
		os << indent(2) << "<table-id:" << (int)flow_stats_reply.get_table_id() << " >" << std::endl;
		os << indent(2) << "<duration-sec:" << (int)flow_stats_reply.get_duration_sec() << " >" << std::endl;
		os << indent(2) << "<duration-nsec:" << (int)flow_stats_reply.get_duration_nsec() << " >" << std::endl;
		os << indent(2) << "<priority:" << (int)flow_stats_reply.get_priority() << " >" << std::endl;
		os << indent(2) << "<idle-timeout:" << (int)flow_stats_reply.get_idle_timeout() << " >" << std::endl;
		os << indent(2) << "<hard-timeout:" << (int)flow_stats_reply.get_hard_timeout() << " >" << std::endl;
		os << indent(2) << "<cookie:0x" << std::hex << (unsigned long long)flow_stats_reply.get_cookie() << std::dec << " >" << std::endl;
		os << indent(2) << "<packet-count:" << (int)flow_stats_reply.get_packet_count() << " >" << std::endl;
		os << indent(2) << "<byte-count:" << (int)flow_stats_reply.get_byte_count() << " >" << std::endl;
		indent i(2);
		os << flow_stats_reply.match;
		switch (flow_stats_reply.of_version) {
		case openflow10::OFP_VERSION: os << flow_stats_reply.actions; break;
		default: os << flow_stats_reply.instructions; break;
		}
		return os;
	};
};


} /* end of namespace */

#endif /* COFFLOWSTATS_H_ */
