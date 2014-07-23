/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_queue_get_config.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_QUEUE_GET_CONFIG_H_
#define COFMSG_QUEUE_GET_CONFIG_H_ 1

#include <ostream>

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofpacketqueues.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_queue_get_config_request :
	public cofmsg
{
private:

	union {
		uint8_t*											ofhu_queue_get_config_request;
		struct openflow10::ofp_queue_get_config_request*	ofhu10_queue_get_config_request;
		struct openflow12::ofp_queue_get_config_request*	ofhu12_queue_get_config_request;
		struct openflow13::ofp_queue_get_config_request*	ofhu13_queue_get_config_request;
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
	virtual uint8_t*
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

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_queue_get_config_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(0) << "<cofmsg_queue_get_config_request >" << std::endl;
			os << indent(2) << "<port-no:0x" << std::hex << (int)msg.get_port_no() << std::dec << " >" << std::endl;
		return os;
	};
};




/**
 *
 */
class cofmsg_queue_get_config_reply :
	public cofmsg
{
private:

	cofpacket_queues	pql;

	union {
		uint8_t*								ofhu_queue_get_config_reply;
		struct openflow10::ofp_queue_get_config_reply*	ofhu10_queue_get_config_reply;
		struct openflow12::ofp_queue_get_config_reply*	ofhu12_queue_get_config_reply;
		struct openflow13::ofp_queue_get_config_reply*	ofhu13_queue_get_config_reply;
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
			cofpacket_queues const &pql = cofpacket_queues(openflow12::OFP_VERSION));
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
	virtual uint8_t*
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
	cofpacket_queues&
	get_queues();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_queue_get_config_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(0) << "<cofmsg_queue_get_config_request >" << std::endl;
			os << indent(2) << "<port-no:0x" << std::hex << (int)msg.get_port_no() << std::dec << " >" << std::endl;
			indent i(4);
			os << msg.pql;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_QUEUE_GET_CONFIG_H_ */
