/*
 * cofpacketqueue.h
 *
 *  Created on: 29.04.2013
 *      Author: andi
 */

#ifndef COFPACKETQUEUE_H_
#define COFPACKETQUEUE_H_ 1

#include <ostream>

#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/cofqueueprops.h"

namespace rofl {
namespace openflow {


class ePacketQueueBase 		: public RoflException {};
class ePacketQueueNotFound 	: public ePacketQueueBase {};


class cofpacket_queue {
public:

	/**
	 *
	 */
	cofpacket_queue(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t port_no = 0,
			uint32_t queue_id = 0,
			const cofqueue_props& props = cofqueue_props());


	/**
	 *
	 */
	virtual
	~cofpacket_queue();


	/**
	 *
	 */
	cofpacket_queue(
			const cofpacket_queue& pq);


	/**
	 *
	 */
	cofpacket_queue&
	operator= (
			const cofpacket_queue& pq);


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
	pack(
			uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);


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
	set_version(uint8_t ofp_version) {
		this->ofp_version = ofp_version;
		queue_props.set_version(ofp_version);
	};

	/**
	 *
	 */
	uint32_t
	get_queue_id() const { return queue_id; };


	/**
	 *
	 */
	void
	set_queue_id(
			uint32_t queue_id) { this->queue_id = queue_id; };


	/**
	 *
	 */
	uint32_t
	get_port_no() const { return port_no; };


	/**
	 *
	 */
	void
	set_port_no(
			uint32_t port_no) { this->port_no = port_no; };

	/**
	 *
	 */
	const cofqueue_props&
	get_queue_props() const { return queue_props; };

	/**
	 *
	 */
	cofqueue_props&
	set_queue_props() { return queue_props; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofpacket_queue const& pq) {
		os << rofl::indent(0) << "<cofpacket_queue version: " << (int)pq.get_version() << " >" << std::endl;
		os << std::hex;
		os << rofl::indent(2) << "<port-no: 0x" << (unsigned int)pq.get_port_no() << " >" << std::endl;
		os << rofl::indent(2) << "<queue-id: 0x" << (unsigned int)pq.get_queue_id() << " >" << std::endl;
		os << std::dec;
			indent i(2);
			os << pq.queue_props;
		return os;
	};

private:

	uint8_t				ofp_version;
	uint32_t			port_no;
	uint32_t			queue_id;
	mutable uint16_t	len;
	cofqueue_props		queue_props;

};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFPACKETQUEUE_H_ */
