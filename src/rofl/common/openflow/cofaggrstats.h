/*
 * cofflowstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFAGGRSTATS_H_
#define COFAGGRSTATS_H_ 1

#include "../cmemory.h"
#include "cofmatch.h"
#include "cofaclist.h"
#include "cofinlist.h"
#include "../../platform/unix/csyslog.h"
#include "openflow.h"
#include "openflow_rofl_exceptions.h"

namespace rofl
{

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
			uint8_t of_version = 0,
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
	virtual const char*
	c_str();


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
};


} /* end of namespace */

#endif /* COFAGGRSTATS_H_ */
