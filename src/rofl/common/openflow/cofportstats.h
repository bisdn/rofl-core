/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofportstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFPORTSTATS_H_
#define COFPORTSTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class ePortStatsBase			: public RoflException {};
class ePortStatsInval			: public ePortStatsBase {};
class ePortStatsNotFound		: public ePortStatsBase {};

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

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofport_stats_request const& r) {
		os << indent(0) << "<cofport_stats_request >" << std::endl;
		os << indent(2) << "<port-no: 0x" << std::hex << (int)r.get_portno() << std::dec << " >" << std::endl;
		return os;
	};
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
	uint32_t	duration_sec;	// since OF1.3
	uint32_t	duration_nsec;	// since OF1.3


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
			uint64_t collisions,
			uint32_t durarion_sec,
			uint32_t duration_nsec);



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
	bool
	operator== (
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
	get_port_no() const;

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
	void
	set_port_no(uint32_t port_no);

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

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofport_stats_reply const& r) {
		os << indent(0) << "<cofport_stats_reply >" << std::endl;
		os << std::hex;
		os << indent(2) << "<port-no: 0x" 		<< (int)r.get_port_no() << " >" << std::endl;
		os << indent(2) << "<rx-packets: 0x" 	<< (unsigned long long)r.get_rx_packets() 	<< " >" << std::endl;
		os << indent(2) << "<tx-packets: 0x" 	<< (unsigned long long)r.get_tx_packets() 	<< " >" << std::endl;
		os << indent(2) << "<rx-bytes: 0x" 		<< (unsigned long long)r.get_rx_bytes() 	<< " >" << std::endl;
		os << indent(2) << "<tx-bytes: 0x" 		<< (unsigned long long)r.get_tx_bytes() 	<< " >" << std::endl;
		os << indent(2) << "<rx-dropped: 0x" 	<< (unsigned long long)r.get_rx_dropped() 	<< " >" << std::endl;
		os << indent(2) << "<tx-dropped: 0x" 	<< (unsigned long long)r.get_tx_dropped() 	<< " >" << std::endl;
		os << indent(2) << "<rx-errors: 0x" 	<< (unsigned long long)r.get_rx_errors() 	<< " >" << std::endl;
		os << indent(2) << "<tx-errors: 0x" 	<< (unsigned long long)r.get_tx_errors() 	<< " >" << std::endl;
		os << indent(2) << "<rx-frame-err: 0x"	<< (unsigned long long)r.get_rx_frame_err()	<< " >" << std::endl;
		os << indent(2) << "<rx-over-err: 0x" 	<< (unsigned long long)r.get_rx_over_err() 	<< " >" << std::endl;
		os << indent(2) << "<rx-crc-err: 0x" 	<< (unsigned long long)r.get_rx_crc_err() 	<< " >" << std::endl;
		os << indent(2) << "<collisions: 0x" 	<< (unsigned long long)r.get_collisions() 	<< " >" << std::endl;
		os << std::dec;
		switch (r.get_version()) {
		case rofl::openflow13::OFP_VERSION: {
			os << indent(2) << "<duration-sec: " 	<< (unsigned long long)r.get_duration_sec() 	<< " >" << std::endl;
			os << indent(2) << "<duration-nsec: " 	<< (unsigned long long)r.get_duration_nsec() 	<< " >" << std::endl;
		} break;
		default: {
		};
		}
		return os;
	};
};

} /* end of namespace */
} /* end of namespace */

#endif /* COFFLOWSTATS_H_ */
