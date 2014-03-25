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

#include "rofl/common/openflow/cofqueueproplist.h"

namespace rofl {
namespace openflow {


class ePacketQueueBase 		: public RoflException {};
class ePacketQueueNotFound 	: public ePacketQueueBase {};


class cofpacket_queue :
		public cmemory
{
private:

	uint8_t				of_version;
	cofqueue_prop_list	qpl;	//< list of queue properties

	union {
		uint8_t								*ofpu_pqueue;
		struct openflow10::ofp_packet_queue	*ofpu10_pqueue;
		struct openflow12::ofp_packet_queue	*ofpu12_pqueue;
	} ofp_ofpu;

#define ofp_pqueue		ofp_ofpu.ofpu_pqueue
#define ofp10_pqueue	ofp_ofpu.ofpu10_pqueue
#define ofp12_pqueue	ofp_ofpu.ofpu12_pqueue




public:

	/**
	 *
	 */
	cofpacket_queue(
			uint8_t of_version = openflow12::OFP_VERSION);


	/**
	 *
	 */
	virtual
	~cofpacket_queue();


	/**
	 *
	 */
	cofpacket_queue(
			cofpacket_queue const& pq);


	/**
	 *
	 */
	cofpacket_queue&
	operator= (
			cofpacket_queue const& qp);


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
			uint8_t *buf, size_t buflen) const;


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
	uint32_t
	get_queue_id() const;


	/**
	 *
	 */
	void
	set_queue_id(
			uint32_t queue_id);


	/**
	 *
	 */
	uint32_t
	get_port() const;


	/**
	 *
	 */
	void
	set_port(
			uint32_t port_no);


	/**
	 *
	 */
	cofqueue_prop_list&
	get_queue_prop_list();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofpacket_queue const& pq) {
		os 	<< indent(0) << "<PacketQueue >" << std::endl;
			switch (pq.of_version) {
			case rofl::openflow10::OFP_VERSION: {
				// no port field in 1.0
			} break;
			default: {
				os << indent(2) << "<port:0x" << std::hex << (int)pq.get_port() << std::dec << " >" << std::endl;
			};
			}
			os << indent(2) << "<queueid:" << (int)pq.get_queue_id() << " >" << std::endl;
			indent i(2);
			os << pq.qpl;
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFPACKETQUEUE_H_ */
