/*
 * cofmsg_features.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FEATURES_H_
#define COFMSG_FEATURES_H_ 1

#include "cofmsg.h"
#include "cofportlist.h"

namespace rofl
{

/**
 *
 */
class cofmsg_features_request :
	public cofmsg
{
public:


	/** constructor
	 *
	 */
	cofmsg_features_request(
			uint8_t of_version = 0,
			uint32_t xid = 0);


	/**
	 *
	 */
	cofmsg_features_request(
			cofmsg_features_request const& features_request);


	/**
	 *
	 */
	cofmsg_features_request&
	operator= (
			cofmsg_features_request const& features_request);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_features_request();


	/**
	 *
	 */
	cofmsg_features_request(cmemory *memarea);


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
class cofmsg_features_reply :
	public cofmsg
{
private:

	cofportlist			ports;

	union {
		uint8_t*						ofhu_switch_features;
		struct ofp10_switch_features*	ofhu10_switch_features;
		struct ofp12_switch_features*	ofhu12_switch_features;
		struct ofp13_switch_features*	ofhu13_switch_features;
	} ofhu;

#define ofh_switch_features   ofhu.ofhu_switch_features
#define ofh10_switch_features ofhu.ofhu10_switch_features
#define ofh12_switch_features ofhu.ofhu12_switch_features
#define ofh13_switch_features ofhu.ofhu13_switch_features

public:


	/** constructor
	 *
	 */
	cofmsg_features_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint64_t dpid = 0,
			uint32_t n_buffers = 0,
			uint8_t  n_tables = 0,
			uint32_t capabilities = 0,
			uint32_t of10_actions_bitmap = 0,
			uint8_t  of13_auxiliary_id = 0,
			cofportlist const& ports = cofportlist());


	/**
	 *
	 */
	cofmsg_features_reply(
			cofmsg_features_reply const& features_reply);


	/**
	 *
	 */
	cofmsg_features_reply&
	operator= (
			cofmsg_features_reply const& features_reply);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_features_reply();


	/**
	 *
	 */
	cofmsg_features_reply(cmemory *memarea);


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

#endif /* COFMSG_FEATURES_H_ */
