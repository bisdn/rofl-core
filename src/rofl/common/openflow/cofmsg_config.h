/*
 * cofmsg_config.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_CONFIG_H_
#define COFMSG_CONFIG_H_ 1

#include "cofmsg.h"
#include "cofportlist.h"

namespace rofl
{

/**
 *
 */
class cofmsg_get_config_request :
	public cofmsg
{
public:


	/** constructor
	 *
	 */
	cofmsg_get_config_request(
			uint8_t of_version = 0,
			uint32_t xid = 0);


	/**
	 *
	 */
	cofmsg_get_config_request(
			cofmsg_get_config_request const& get_config_request);


	/**
	 *
	 */
	cofmsg_get_config_request&
	operator= (
			cofmsg_get_config_request const& get_config_request);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_get_config_request() {};


	/**
	 *
	 */
	cofmsg_get_config_request(cmemory *memarea);


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
};


/**
 *
 */
class cofmsg_config :
	public cofmsg
{
private:

	union {
		uint8_t*					ofhu_switch_config;
		struct ofp10_switch_config*	ofhu10_switch_config;
		struct ofp12_switch_config*	ofhu12_switch_config;
		struct ofp13_switch_config*	ofhu13_switch_config;
	} ofhu;

#define ofh_switch_config   ofhu.ofhu_switch_config
#define ofh10_switch_config ofhu.ofhu10_switch_config
#define ofh12_switch_config ofhu.ofhu12_switch_config
#define ofh13_switch_config ofhu.ofhu13_switch_config

public:


	/** constructor
	 *
	 */
	cofmsg_config(
			uint8_t of_version = 0,
			uint8_t  type = OFPT_GET_CONFIG_REPLY,
			uint32_t xid = 0,
			uint64_t dpid = 0,
			uint16_t flags = 0,
			uint16_t miss_send_len = 0);


	/**
	 *
	 */
	cofmsg_config(
			cofmsg_config const& config);


	/**
	 *
	 */
	cofmsg_config&
	operator= (
			cofmsg_config const& config);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_config() {};


	/**
	 *
	 */
	cofmsg_config(cmemory *memarea);


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
	uint64_t
	get_dpid() const;

	/**
	 *
	 */
	void
	set_dpid(uint64_t dpid);

	/**
	 *
	 */
	uint32_t
	get_n_buffers() const;

	/**
	 *
	 */
	void
	set_n_buffers(uint32_t n_buffers);

	/**
	 *
	 */
	uint8_t
	get_n_tables() const;

	/**
	 *
	 */
	void
	set_n_tables(uint8_t n_tables);

	/** OF1.3
	 *
	 */
	uint8_t
	get_auxiliary_id() const;

	/**
	 *
	 */
	void
	set_auxiliary_id(uint8_t auxiliary_id);

	/**
	 *
	 */
	uint32_t
	get_capabilities() const;

	/**
	 *
	 */
	void
	set_capabilities(uint32_t capabilities);

	/** OF1.0 only
	 *
	 */
	uint32_t
	get_actions_bitmap() const;

	/**
	 *
	 */
	void
	set_actions_bitmap(uint32_t actions_bitmap);

	/**
	 *
	 */
	cofportlist&
	get_ports();
};

} // end of namespace rofl

#endif /* COFMSG_CONFIG_H_ */
