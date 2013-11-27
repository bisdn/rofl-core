/*
 * cofpacketqueue.h
 *
 *  Created on: 29.04.2013
 *      Author: andi
 */

#ifndef COFPACKETQUEUE_H_
#define COFPACKETQUEUE_H_ 1

#include <ostream>

#include "../cmemory.h"
#include "../cerror.h"
#include "openflow.h"
#include "openflow_rofl_exceptions.h"

#include "cofqueueproplist.h"

namespace rofl
{


class ePacketQueueBase : public cerror {};
class ePacketQueueNotFound : public ePacketQueueBase {};


class cofpacket_queue :
		public cmemory
{
private:

	uint8_t				of_version;
	cofqueue_prop_list	qpl;	//< list of queue properties

	union {
		uint8_t						*ofpu_pqueue;
		struct openflow10::ofp_packet_queue	*ofpu10_pqueue;
		struct openflow12::ofp_packet_queue	*ofpu12_pqueue;
	} ofp_ofpu;

#define ofp_pqueue		ofp_ofpu.ofpu_pqueue
#define openflow10::ofp_pqueue	ofp_ofpu.ofpu10_pqueue
#define openflow12::ofp_pqueue	ofp_ofpu.ofpu12_pqueue


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofpacket_queue const& pq)
	{
		os 	<< "PacketQueue["
				<< "port:" 			<< pq.get_port() << " "
				<< "queueid: " 	<< pq.get_queue_id() << " "
				<< "properties: " 	<< pq.qpl << "]";
		return os;
	};


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
};

}

#endif /* COFPACKETQUEUE_H_ */
