/*
 * cofmsg_role.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_ROLE_H
#define COFMSG_ROLE_H 1

#include "cofmsg.h"

namespace rofl
{

/**
 *
 */
class cofmsg_role_request :
	public cofmsg
{
private:

	union {
		uint8_t*						ofhu_role_request;
		struct ofp12_role_request*		ofhu12_role_request;
		struct ofp13_role_request*		ofhu13_role_request;
	} ofhu;

#define ofh_role_request	ofhu.ofhu_role_request
#define ofh12_role_request 	ofhu.ofhu12_role_request
#define ofh13_role_request 	ofhu.ofhu13_role_request


public:


	/** constructor
	 *
	 */
	cofmsg_role_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t role = 0,
			uint64_t generation_id = 0);


	/**
	 *
	 */
	cofmsg_role_request(
			cofmsg_role_request const& role);


	/**
	 *
	 */
	cofmsg_role_request&
	operator= (
			cofmsg_role_request const& role);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_role_request();


	/**
	 *
	 */
	cofmsg_role_request(cmemory *memarea);


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
	get_role() const;

	/**
	 *
	 */
	void
	set_role(uint32_t port_no);

	/**
	 *
	 */
	uint64_t
	get_generation_id() const;

	/**
	 *
	 */
	void
	set_generation_id(uint64_t generation_id);
};



/**
 *
 */
class cofmsg_role_reply :
	public cofmsg
{
private:

	union {
		uint8_t*						ofhu_role_reply;
		struct ofp12_role_request*		ofhu12_role_reply;
		struct ofp13_role_request*		ofhu13_role_reply;
	} ofhu;

#define ofh_role_reply		ofhu.ofhu_role_reply
#define ofh12_role_reply 	ofhu.ofhu12_role_reply
#define ofh13_role_reply 	ofhu.ofhu13_role_reply


public:


	/** constructor
	 *
	 */
	cofmsg_role_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t role = 0,
			uint64_t generation_id = 0);


	/**
	 *
	 */
	cofmsg_role_reply(
			cofmsg_role_reply const& role);


	/**
	 *
	 */
	cofmsg_role_reply&
	operator= (
			cofmsg_role_reply const& role);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_role_reply();


	/**
	 *
	 */
	cofmsg_role_reply(cmemory *memarea);


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
	get_role() const;

	/**
	 *
	 */
	void
	set_role(uint32_t port_no);

	/**
	 *
	 */
	uint64_t
	get_generation_id() const;

	/**
	 *
	 */
	void
	set_generation_id(uint64_t generation_id);
};




} // end of namespace rofl

#endif /* COFMSG_ROLE_H_ */
