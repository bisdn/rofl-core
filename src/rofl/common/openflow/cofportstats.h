/*
 * cofportstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFPORTSTATS_H_
#define COFPORTSTATS_H_ 1

#include "../cmemory.h"
#include "cofmatch.h"
#include "cofactions.h"
#include "cofinstructions.h"
#include "../../platform/unix/csyslog.h"
#include "openflow.h"
#include "openflow_rofl_exceptions.h"

namespace rofl
{

class cofport_stats_request
{
private: // data structures

	uint8_t 	of_version;
	uint32_t	port_no;

public: // data structures


public:
	/**
	 *
	 */
	cofport_stats_request(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);

	/**
	 *
	 */
	cofport_stats_request(
			uint8_t of_version,
			uint32_t port_no);


	/**
	 *
	 */
	virtual
	~cofport_stats_request();


	/**
	 *
	 */
	cofport_stats_request(
			cofport_stats_request const& port_stats_request);

	/**
	 *
	 */
	cofport_stats_request&
	operator= (
			cofport_stats_request const& port_stats_request);


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
	uint32_t
	get_portno() const;
};



class cofport_stats_reply
{
private: // data structures

	uint8_t 	of_version;
	uint32_t	port_no;
	uint64_t	rx_packets;
	uint64_t	tx_packets;
	uint64_t	rx_bytes;
	uint64_t	tx_bytes;
	uint64_t	rx_dropped;
	uint64_t 	tx_dropped;
	uint64_t	rx_errors;
	uint64_t	tx_errors;
	uint64_t	rx_frame_err;
	uint64_t	rx_over_err;
	uint64_t	rx_crc_err;
	uint64_t	collisions;


public: // data structures


public:
	/**
	 *
	 */
	cofport_stats_reply(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);


	/**
	 *
	 */
	cofport_stats_reply(
			uint8_t of_version,
			uint32_t port_no,
			uint64_t rx_packets,
			uint64_t tx_packets,
			uint64_t rx_bytes,
			uint64_t tx_bytes,
			uint64_t rx_dropped,
			uint64_t tx_dropped,
			uint64_t rx_errors,
			uint64_t tx_errors,
			uint64_t rx_frame_err,
			uint64_t rx_over_err,
			uint64_t rx_crc_err,
			uint64_t collisions);



	/**
	 *
	 */
	virtual
	~cofport_stats_reply();


	/**
	 *
	 */
	cofport_stats_reply(
			cofport_stats_reply const& port_stats_reply);

	/**
	 *
	 */
	cofport_stats_reply&
	operator= (
			cofport_stats_reply const& port_stats_reply);


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
	reset();


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
	uint32_t
	get_portno() const;

	/**
	 *
	 */
	uint64_t
	get_rx_packets() const;

	/**
	 *
	 */
	uint64_t
	get_tx_packets() const;

	/**
	 *
	 */
	uint64_t
	get_rx_bytes() const;

	/**
	 *
	 */
	uint64_t
	get_tx_bytes() const;

	/**
	 *
	 */
	uint64_t
	get_rx_dropped() const;

	/**
	 *
	 */
	uint64_t
	get_tx_dropped() const;

	/**
	 *
	 */
	uint64_t
	get_rx_errors() const;

	/**
	 *
	 */
	uint64_t
	get_tx_errors() const;

	/**
	 *
	 */
	uint64_t
	get_rx_frame_err() const;

	/**
	 *
	 */
	uint64_t
	get_rx_over_err() const;

	/**
	 *
	 */
	uint64_t
	get_rx_crc_err() const;

	/**
	 *
	 */
	uint64_t
	get_collisions() const;

	/**
	 *
	 */
	void
	set_portno(uint32_t port_no);

	/**
	 *
	 */
	void
	set_rx_packets(uint64_t rx_packets);

	/**
	 *
	 */
	void
	set_tx_packets(uint64_t tx_packets);

	/**
	 *
	 */
	void
	set_rx_bytes(uint64_t rx_bytes);

	/**
	 *
	 */
	void
	set_tx_bytes(uint64_t tx_bytes);

	/**
	 *
	 */
	void
	set_rx_dropped(uint64_t rx_dropped);

	/**
	 *
	 */
	void
	set_tx_dropped(uint64_t tx_dropped);

	/**
	 *
	 */
	void
	set_rx_errors(uint64_t rx_errors);

	/**
	 *
	 */
	void
	set_tx_errors(uint64_t tx_errors);

	/**
	 *
	 */
	void
	set_rx_frame_err(uint64_t rx_frame_err);

	/**
	 *
	 */
	void
	set_rx_over_err(uint64_t rx_over_err);

	/**
	 *
	 */
	void
	set_rx_crc_err(uint64_t rx_crc_err);

	/**
	 *
	 */
	void
	set_collisions(uint64_t collisions);
};


} /* end of namespace */

#endif /* COFFLOWSTATS_H_ */
