/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofpacketqueuelist.h
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */

#ifndef COFPACKETQUEUELIST_H_
#define COFPACKETQUEUELIST_H_ 1

#include <map>
#include <ostream>

#include "rofl/common/openflow/cofpacketqueue.h"

namespace rofl {
namespace openflow {

class cofpacket_queues {
public:

	/**
	 *
	 */
	cofpacket_queues(
		uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofpacket_queues();

	/**
	 *
	 */
	cofpacket_queues(
		const cofpacket_queues& queues);

	/**
	 */
	cofpacket_queues&
	operator= (
		const cofpacket_queues& queues);

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
	const std::map<uint32_t, std::map<uint32_t, cofpacket_queue> >&
	get_packet_queues() const { return pqueues; };

	/**
	 *
	 */
	std::map<uint32_t, std::map<uint32_t, cofpacket_queue> >&
	set_packet_queues() { return pqueues; };

public:

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	cofpacket_queue&
	add_pqueue(uint32_t port_no, uint32_t queue_id);

	/**
	 *
	 */
	cofpacket_queue&
	set_pqueue(uint32_t port_no, uint32_t queue_id);

	/**
	 *
	 */
	const cofpacket_queue&
	get_pqueue(uint32_t port_no, uint32_t queue_id) const;

	/**
	 *
	 */
	void
	drop_pqueue(uint32_t port_no, uint32_t queue_id);

	/**
	 *
	 */
	bool
	has_pqueue(uint32_t port_no, uint32_t queue_id) const;

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
	unpack(
		uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	pack(
		uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofpacket_queues const& pql) {
		unsigned int count = 0;
		for (std::map<uint32_t, std::map<uint32_t, cofpacket_queue> >::const_iterator
				it = pql.pqueues.begin(); it != pql.pqueues.end(); ++it) {
			count += it->second.size();
		}
		os << rofl::indent(0) << "<cofpacket_queues #queues: " << count << " >" << std::endl;
		for (std::map<uint32_t, std::map<uint32_t, cofpacket_queue> >::const_iterator
				it = pql.pqueues.begin(); it != pql.pqueues.end(); ++it) {
			for (std::map<uint32_t, cofpacket_queue>::const_iterator
					jt = it->second.begin(); jt != it->second.end(); ++jt) {
				rofl::indent i(2); os << (jt->second);
			}
		}
		return os;
	};

private:

	uint8_t 													ofp_version;
	std::map<uint32_t, std::map<uint32_t, cofpacket_queue> > 	pqueues; // port_no/queue_id => cofpacket_queue
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFPACKETQUEUELIST_H_ */
