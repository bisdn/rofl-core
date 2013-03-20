/*
 * cofmsg_role.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_ROLE_H_
#define COFMSG_ROLE_H_ 1

#include "cofmsg.h"

namespace rofl
{

/**
 *
 */
class cofmsg_role :
	public cofmsg
{
private:

	union {
		uint8_t*						ofhu_role_request;
		struct ofp12_role_request*		ofhu12_role_request;
		struct ofp13_role_request*		ofhu13_role_request;
		struct ofp12_role_request*		ofhu12_role_reply;
		struct ofp13_role_request*		ofhu13_role_reply;
	} ofhu;

#define ofh_role_request   	ofhu.ofhu_role_request
#define ofh12_role_request 	ofhu.ofhu12_role_request
#define ofh13_role_request 	ofhu.ofhu13_role_request
#define ofh12_role_reply 	ofhu.ofhu12_role_reply
#define ofh13_role_reply 	ofhu.ofhu13_role_reply


public:


	/** constructor
	 *
	 */
	cofmsg_role(
			uint8_t of_version = 0,
			uint8_t  type = OFPT_ROLE_REQUEST,
			uint32_t xid = 0,
			uint32_t role = 0,
			uint64_t generation_id = 0);


	/**
	 *
	 */
	cofmsg_role(
			cofmsg_role const& role);


	/**
	 *
	 */
	cofmsg_role&
	operator= (
			cofmsg_role const& role);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_role() {};


	/**
	 *
	 */
	cofmsg_role(cmemory *memarea);


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
