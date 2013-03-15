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
#include "cofaclist.h"
#include "cofinlist.h"
#include "../../platform/unix/csyslog.h"
#include "openflow.h"

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
			uint8_t of_version,
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
	pack(uint8_t *buf, size_t buflen) const;


	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);


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
	get_out_port() const;

	/**
	 *
	 */
	uint32_t
	get_out_group() const;

	/**
	 *
	 */
	uint64_t
	get_cookie() const;

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
	cofaclist	actions;		// for OF1.0
	cofinlist	instructions;	// for OF1.2

public: // data structures


public:
	/**
	 *
	 */
	cofflow_stats_reply(
			uint8_t of_version,
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
			cofaclist const& actions);


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
			cofinlist const& instructions);



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
	cofaclist&
	get_actions();

	/**
	 *
	 */
	cofinlist&
	get_instructions();
};


} /* end of namespace */

#endif /* COFDESCSTATS_H_ */
