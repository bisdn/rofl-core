/*
 * cofflowstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFAGGRSTATS_H_
#define COFAGGRSTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class cofaggr_stats_request
{
private: // data structures

	uint8_t 	of_version;
	cofmatch 	match;
	uint8_t 	table_id;
	uint32_t	out_port;
	uint32_t	out_group;
	uint64_t	cookie;
	uint64_t	cookie_mask;

	std::string info;

public: // data structures


public:
	/**
	 *
	 */
	cofaggr_stats_request(
			uint8_t of_version = openflow::OFP_VERSION_UNKNOWN,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);

	/**
	 *
	 */
	cofaggr_stats_request(
			uint8_t of_version,
			cofmatch const& match,
			uint8_t table_id,
			uint16_t out_port);


	/**
	 *
	 */
	cofaggr_stats_request(
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
	~cofaggr_stats_request();


	/**
	 *
	 */
	cofaggr_stats_request(
			cofaggr_stats_request const& flowstatsrequest);

	/**
	 *
	 */
	cofaggr_stats_request&
	operator= (
			cofaggr_stats_request const& flowstatsrequest);


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
	cofmatch&
	get_match();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaggr_stats_request const& aggr_stats_request) {
		switch (aggr_stats_request.get_version()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			os << indent(0) << "<cofaggr_stats_request >" << std::endl;
		} break;
		case rofl::openflow10::OFP_VERSION: {
			os << indent(0) << "<cofaggr_stats_request >" << std::endl;
			os << indent(2) << "<table-id:" << (int)aggr_stats_request.get_table_id() << " >" << std::endl;
			os << indent(2) << "<out-port:0x" << std::hex << (int)aggr_stats_request.get_out_port() << std::dec << " >" << std::endl;
			indent i(2);
			os << aggr_stats_request.match;

		} break;
		case rofl::openflow12::OFP_VERSION: {
			os << indent(0) << "<cofaggr_stats_request >" << std::endl;
			os << indent(2) << "<table-id:" << (int)aggr_stats_request.get_table_id() << " >" << std::endl;
			os << indent(2) << "<out-port:0x" << std::hex << (int)aggr_stats_request.get_out_port() << std::dec << " >" << std::endl;
			os << indent(2) << "<out-group:0x" << std::hex << (int)aggr_stats_request.get_out_group() << std::dec << " >" << std::endl;
			os << indent(2) << "<cookie:0x" << std::hex << (unsigned long long)aggr_stats_request.get_cookie() << std::dec << " >" << std::endl;
			os << indent(2) << "<cookie-mask:0x" << std::hex << (unsigned long long)aggr_stats_request.get_cookie_mask() << std::dec << " >" << std::endl;
			indent i(2);
			os << aggr_stats_request.match;

		} break;
		case rofl::openflow13::OFP_VERSION: {
			os << indent(0) << "<cofaggr_stats_request >" << std::endl;
			os << indent(2) << "<table-id:" << (int)aggr_stats_request.get_table_id() << " >" << std::endl;
			os << indent(2) << "<out-port:0x" << std::hex << (int)aggr_stats_request.get_out_port() << std::dec << " >" << std::endl;
			os << indent(2) << "<out-group:0x" << std::hex << (int)aggr_stats_request.get_out_group() << std::dec << " >" << std::endl;
			os << indent(2) << "<cookie:0x" << std::hex << (unsigned long long)aggr_stats_request.get_cookie() << std::dec << " >" << std::endl;
			os << indent(2) << "<cookie-mask:0x" << std::hex << (unsigned long long)aggr_stats_request.get_cookie_mask() << std::dec << " >" << std::endl;
			indent i(2);
			os << aggr_stats_request.match;

		} break;
		default: {
			throw eBadVersion();
		};
		}
		return os;
	};
};



class cofaggr_stats_reply
{
private: // data structures

	uint8_t 	of_version;
	uint64_t	packet_count;
	uint64_t	byte_count;
	uint32_t	flow_count;

public: // data structures


public:
	/**
	 *
	 */
	cofaggr_stats_reply(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);


	/**
	 *
	 */
	cofaggr_stats_reply(
			uint8_t of_version,
			uint64_t packet_count,
			uint64_t byte_count,
			uint32_t flow_count);


	/**
	 *
	 */
	virtual
	~cofaggr_stats_reply();


	/**
	 *
	 */
	cofaggr_stats_reply(
			cofaggr_stats_reply const& aggrstats);

	/**
	 *
	 */
	cofaggr_stats_reply&
	operator= (
			cofaggr_stats_reply const& aggrstats);


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
	uint32_t
	get_flow_count() const;


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
	void
	set_flow_count(uint32_t flow_count);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaggr_stats_reply const& reply) {
		switch (reply.get_version()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			os << indent(0) << "<cofaggr_stats_reply >" << std::endl;
		} break;
		case rofl::openflow10::OFP_VERSION:
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << indent(0) << "<cofaggr_stats_reply >" << std::endl;
			os << indent(2) << "<packet-count:" << (int)reply.get_packet_count() << " >" << std::endl;
			os << indent(2) << "<byte-count:" << (int)reply.get_byte_count() << " >" << std::endl;
			os << indent(2) << "<flow-count:" << (int)reply.get_flow_count() << " >" << std::endl;
		} break;
		default: {
			throw eBadVersion();
		};
		}
		return os;
	};
};


} /* end of namespace openflow */
} /* end of namespace rofl */

#endif /* COFAGGRSTATS_H_ */
