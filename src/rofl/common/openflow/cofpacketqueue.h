/*
 * cofpacketqueue.h
 *
 *  Created on: 29.04.2013
 *      Author: andi
 */

#ifndef COFPACKETQUEUE_H_
#define COFPACKETQUEUE_H_ 1

#include "../cmemory.h"
#include "openflow.h"
#include "openflow_rofl_exceptions.h"

#include "cofqueueproplist.h"

namespace rofl
{

class cofpacket_queue :
		public cmemory
{
private:

	uint8_t				of_version;
	cofqueueproplist	qpl;	//< list of queue properties

	union {
		uint8_t						*ofpu_pqueue;
		struct ofp10_packet_queue	*ofpu10_pqueue;
		struct ofp12_packet_queue	*ofpu12_pqueue;
	} ofp_ofpu;

#define ofp_pqueue		ofp_ofpu.ofpu_pqueue
#define ofp10_pqueue	ofp_ofpu.ofpu10_pqueue
#define ofp12_pqueue	ofp_ofpu.ofpu12_pqueue

public:

	/**
	 *
	 */
	cofpacket_queue(
			uint8_t of_version = OFP12_VERSION);


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
	cofqueueproplist&
	get_queue_prop_list();
};

}

#endif /* COFPACKETQUEUE_H_ */
