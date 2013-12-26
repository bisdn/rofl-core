/*
 * cofmsg_async_config.h
 *
 *  Created on: 15.06.2013
 *      Author: andi
 */

#ifndef COFMSG_ASYNC_CONFIG_H_A
#define COFMSG_ASYNC_CONFIG_H_A 1

#include "cofmsg.h"

namespace rofl
{

/**
 *
 */
class cofmsg_get_async_config_request :
	public cofmsg
{
public:


	/** constructor
	 *
	 */
	cofmsg_get_async_config_request(
			uint8_t of_version = 0,
			uint32_t xid = 0);


	/**
	 *
	 */
	cofmsg_get_async_config_request(
			cofmsg_get_async_config_request const& get_async_config_request);


	/**
	 *
	 */
	cofmsg_get_async_config_request&
	operator= (
			cofmsg_get_async_config_request const& get_async_config_request);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_get_async_config_request();


	/**
	 *
	 */
	cofmsg_get_async_config_request(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length();


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

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_get_async_config_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_get_async_config_request >" << std::endl;
		return os;
	};
};


/**
 *
 */
class cofmsg_get_async_config_reply :
	public cofmsg
{
private:

	union {
		uint8_t*					ofhu_async_config;
		struct openflow13::ofp_async_config*	ofhu13_async_config;
	} ofhu;

#define ofh_async_config   ofhu.ofhu_async_config
#define ofh13_async_config ofhu.ofhu13_async_config

public:


	/** constructor
	 *
	 */
	cofmsg_get_async_config_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t packet_in_mask0 = 0,
			uint32_t packet_in_mask1 = 0,
			uint32_t port_status_mask0 = 0,
			uint32_t port_status_mask1 = 0,
			uint32_t flow_removed_mask0 = 0,
			uint32_t flow_removed_mask1 = 0);


	/**
	 *
	 */
	cofmsg_get_async_config_reply(
			cofmsg_get_async_config_reply const& config);


	/**
	 *
	 */
	cofmsg_get_async_config_reply&
	operator= (
			cofmsg_get_async_config_reply const& config);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_get_async_config_reply();


	/**
	 *
	 */
	cofmsg_get_async_config_reply(cmemory *memarea);


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
	get_packet_in_mask(unsigned int index) const;

	/**
	 *
	 */
	void
	set_packet_in_mask(unsigned int index, uint32_t mask);

	/**
	 *
	 */
	uint32_t
	get_port_status_mask(unsigned int index) const;

	/**
	 *
	 */
	void
	set_port_status_mask(unsigned int index, uint32_t mask);

	/**
	 *
	 */
	uint32_t
	get_flow_removed_mask(unsigned int index) const;

	/**
	 *
	 */
	void
	set_flow_removed_mask(unsigned int index, uint32_t mask);


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_get_async_config_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_get_async_config_reply >" << std::endl;
		os << indent(4) << "<packet-in-mask:0x" << std::hex << msg.get_packet_in_mask(0) << std::dec << " >" << std::endl;
		os << indent(4) << "<port-status-mask:0x" << std::hex << msg.get_port_status_mask(0) << std::dec << " >" << std::endl;
		os << indent(4) << "<flow-removed-mask:0x" << std::hex << msg.get_flow_removed_mask(0) << std::dec << " >" << std::endl;
		return os;
	};
};

/**
 *
 */
class cofmsg_set_async_config :
	public cofmsg
{
private:

	union {
		uint8_t*					ofhu_async_config;
		struct openflow13::ofp_async_config*	ofhu13_async_config;
	} ofhu;

#define ofh_async_config   ofhu.ofhu_async_config
#define ofh13_async_config ofhu.ofhu13_async_config

public:


	/** constructor
	 *
	 */
	cofmsg_set_async_config(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t packet_in_mask0 = 0,
			uint32_t packet_in_mask1 = 0,
			uint32_t port_status_mask0 = 0,
			uint32_t port_status_mask1 = 0,
			uint32_t flow_removed_mask0 = 0,
			uint32_t flow_removed_mask1 = 0);


	/**
	 *
	 */
	cofmsg_set_async_config(
			cofmsg_set_async_config const& config);


	/**
	 *
	 */
	cofmsg_set_async_config&
	operator= (
			cofmsg_set_async_config const& config);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_set_async_config();


	/**
	 *
	 */
	cofmsg_set_async_config(cmemory *memarea);


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
	get_packet_in_mask(unsigned int index) const;

	/**
	 *
	 */
	void
	set_packet_in_mask(unsigned int index, uint32_t mask);

	/**
	 *
	 */
	uint32_t
	get_port_status_mask(unsigned int index) const;

	/**
	 *
	 */
	void
	set_port_status_mask(unsigned int index, uint32_t mask);

	/**
	 *
	 */
	uint32_t
	get_flow_removed_mask(unsigned int index) const;

	/**
	 *
	 */
	void
	set_flow_removed_mask(unsigned int index, uint32_t mask);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_set_async_config const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_get_async_config_reply >" << std::endl;
		os << indent(4) << "<packet-in-mask:0x" << std::hex << msg.get_packet_in_mask(0) << std::dec << " >" << std::endl;
		os << indent(4) << "<port-status-mask:0x" << std::hex << msg.get_port_status_mask(0) << std::dec << " >" << std::endl;
		os << indent(4) << "<flow-removed-mask:0x" << std::hex << msg.get_flow_removed_mask(0) << std::dec << " >" << std::endl;
		return os;
	};
};



} // end of namespace rofl

#endif /* COFMSG_ASYNC_CONFIG_H_ */
