/*
 * cofmsg_queue_get_config.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_QUEUE_GET_CONFIG_H_
#define COFMSG_QUEUE_GET_CONFIG_H_ 1

#include <ostream>

#include "cofmsg.h"
#include "../cofpacketqueuelist.h"

namespace rofl
{

/**
 *
 */
class cofmsg_queue_get_config_request :
	public cofmsg
{
private:

	cofbclist			buckets;

	union {
		uint8_t*								ofhu_queue_get_config_request;
		struct ofp10_queue_get_config_request*	ofhu10_queue_get_config_request;
		struct ofp12_queue_get_config_request*	ofhu12_queue_get_config_request;
		struct ofp13_queue_get_config_request*	ofhu13_queue_get_config_request;
	} ofhu;

#define ofh_queue_get_config_request   ofhu.ofhu_queue_get_config_request
#define ofh10_queue_get_config_request ofhu.ofhu10_queue_get_config_request
#define ofh12_queue_get_config_request ofhu.ofhu12_queue_get_config_request
#define ofh13_queue_get_config_request ofhu.ofhu13_queue_get_config_request

public:


	/** constructor
	 *
	 */
	cofmsg_queue_get_config_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t port_no = 0);


	/**
	 *
	 */
	cofmsg_queue_get_config_request(
			cofmsg_queue_get_config_request const& queue_get_config);


	/**
	 *
	 */
	cofmsg_queue_get_config_request&
	operator= (
			cofmsg_queue_get_config_request const& queue_get_config);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_queue_get_config_request();


	/**
	 *
	 */
	cofmsg_queue_get_config_request(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual void
	resize(size_t len);


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();


public:


	/**
	 *
	 */
	uint32_t
	get_port_no() const;

	/**
	 *
	 */
	void
	set_port_no(uint32_t port_no);
};




/**
 *
 */
class cofmsg_queue_get_config_reply :
	public cofmsg
{
private:

	// TODO: cofqueuelist
	//cmemory				queues;
	cofpacket_queue_list	pql;

	union {
		uint8_t*								ofhu_queue_get_config_reply;
		struct ofp10_queue_get_config_reply*	ofhu10_queue_get_config_reply;
		struct ofp12_queue_get_config_reply*	ofhu12_queue_get_config_reply;
		struct ofp13_queue_get_config_reply*	ofhu13_queue_get_config_reply;
	} ofhu;

#define ofh_queue_get_config_reply   ofhu.ofhu_queue_get_config_reply
#define ofh10_queue_get_config_reply ofhu.ofhu10_queue_get_config_reply
#define ofh12_queue_get_config_reply ofhu.ofhu12_queue_get_config_reply
#define ofh13_queue_get_config_reply ofhu.ofhu13_queue_get_config_reply

public:


	/** constructor
	 *
	 */
	cofmsg_queue_get_config_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t port_no = 0,
			cofpacket_queue_list const &pql = cofpacket_queue_list(OFP12_VERSION));
#if 0
			uint8_t *data = (uint8_t*)0,
			size_t datalen = 0);
#endif

	/**
	 *
	 */
	cofmsg_queue_get_config_reply(
			cofmsg_queue_get_config_reply const& queue_get_config);


	/**
	 *
	 */
	cofmsg_queue_get_config_reply&
	operator= (
			cofmsg_queue_get_config_reply const& queue_get_config);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_queue_get_config_reply();


	/**
	 *
	 */
	cofmsg_queue_get_config_reply(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual void
	resize(size_t len);


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();


public:


	/**
	 *
	 */
	uint32_t
	get_port_no() const;

	/**
	 *
	 */
	void
	set_port_no(uint32_t port_no);

	/**
	 *
	 */
	cofpacket_queue_list&
	get_queues();


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_queue_get_config_reply const& msg)
	{
		os << "cofmsg_queue_get_config_reply[";
		os << "portno: " << msg.get_port_no() << " ";
		os << "queues: " << msg.pql << "]";
		return os;
	};
};



} // end of namespace rofl

#endif /* COFMSG_QUEUE_GET_CONFIG_H_ */
