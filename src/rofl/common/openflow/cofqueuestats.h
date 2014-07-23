/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofqueuestats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFQUEUESTATS_H_
#define COFQUEUESTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class eQueueStatsBase			: public RoflException {};
class eQueueStatsInval			: public eQueueStatsBase {};
class eQueueStatsNotFound		: public eQueueStatsBase {};

class cofqueue_stats_request
{
private: // data structures

	uint8_t 		of_version;
	uint32_t 		port_no;
	uint32_t		queue_id;

public: // data structures


public:
	/**
	 *
	 */
	cofqueue_stats_request(
			uint8_t of_version = 0);

	/**
	 *
	 */
	cofqueue_stats_request(
			uint8_t of_version,
			uint32_t port_no,
			uint32_t queue_id);

	/**
	 *
	 */
	virtual
	~cofqueue_stats_request();


	/**
	 *
	 */
	cofqueue_stats_request(
			cofqueue_stats_request const& stats);

	/**
	 *
	 */
	cofqueue_stats_request&
	operator= (
			cofqueue_stats_request const& stats);


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
	get_port_no() const { return port_no; };


	/**
	 *
	 */
	uint32_t
	get_queue_id() const { return queue_id; };


	/**
	 *
	 */
	void
	set_port_no(uint32_t port_no) { this->port_no = port_no; };


	/**
	 *
	 */
	void
	set_queue_id(uint32_t queue_id) { this->queue_id = queue_id; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofqueue_stats_request const& r) {
		os << indent(0) << "<cofqueue_stats_request >" << std::endl;
		os << indent(2) << "<port-no: 0x" << std::hex << (int)r.get_port_no() << std::dec << " >" << std::endl;
		os << indent(2) << "<queue-id: " << (int)r.get_queue_id() << " >" << std::endl;
		return os;
	};
};



class cofqueue_stats_reply
{
private: // data structures

	uint8_t 		of_version;
	uint32_t		port_no;
	uint32_t		queue_id;
	uint64_t		tx_bytes;
	uint64_t		tx_packets;
	uint64_t		tx_errors;
	uint32_t	 	duration_sec;
	uint32_t	 	duration_nsec;

public: // data structures


public:
	/**
	 *
	 */
	cofqueue_stats_reply(
			uint8_t of_version = 0);

	/**
	 *
	 */
	cofqueue_stats_reply(
			uint8_t of_version,
			uint32_t port_no,
			uint32_t queue_id,
			uint64_t tx_bytes,
			uint64_t tx_packets,
			uint64_t tx_errors,
			uint32_t duration_sec,
			uint32_t duration_nsec);

	/**
	 *
	 */
	virtual
	~cofqueue_stats_reply();


	/**
	 *
	 */
	cofqueue_stats_reply(
			cofqueue_stats_reply const& stats);

	/**
	 *
	 */
	cofqueue_stats_reply&
	operator= (
			cofqueue_stats_reply const& stats);


	/**
	 *
	 */
	bool
	operator== (
			cofqueue_stats_reply const& stats);

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
	get_port_no() const { return port_no; };

	/**
	 *
	 */
	uint32_t
	get_queue_id() const { return queue_id; };

	/**
	 *
	 */
	uint64_t
	get_tx_bytes() const { return tx_bytes; };

	/**
	 *
	 */
	uint64_t
	get_tx_packets() const { return tx_packets; };

	/**
	 *
	 */
	uint64_t
	get_tx_errors() const { return tx_errors; };

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
	void
	set_port_no(uint32_t port_no) { this->port_no = port_no; };

	/**
	 *
	 */
	void
	set_queue_id(uint32_t queue_id) { this->queue_id = queue_id; };

	/**
	 *
	 */
	void
	set_tx_bytes(uint64_t tx_bytes) { this->tx_bytes = tx_bytes; };

	/**
	 *
	 */
	void
	set_tx_packets(uint64_t tx_packets) { this->tx_packets = tx_packets; };

	/**
	 *
	 */
	void
	set_tx_errors(uint64_t tx_errors) { this->tx_errors = tx_errors; };

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
	operator<< (std::ostream& os, cofqueue_stats_reply const& r) {
		os << indent(0) << "<cofqueue_stats_reply >" << std::endl;
		os << std::hex;
		os << indent(2) << "<port-no: 0x" << (int)r.get_port_no() << " >" << std::endl;
		os << indent(2) << "<queue-id: 0x" << (int)r.get_queue_id() << " >" << std::endl;
		os << indent(2) << "<tx-packets: 0x" 	<< (unsigned long long)r.get_tx_packets() 	<< " >" << std::endl;
		os << indent(2) << "<tx-bytes: 0x" 	<< (unsigned long long)r.get_tx_bytes() 	<< " >" << std::endl;
		os << indent(2) << "<tx-errors: 0x" 	<< (unsigned long long)r.get_tx_errors() 	<< " >" << std::endl;
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

}
}

#endif /* COFQUEUESTATS_H_ */
