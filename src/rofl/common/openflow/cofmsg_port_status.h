/*
 * cofmsg_port_status.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PORT_STATUS_H_
#define COFMSG_PORT_STATUS_H_ 1

#include "cofmsg.h"
#include "cofport.h"

namespace rofl
{

/**
 *
 */
class cofmsg_port_status :
	public cofmsg
{
private:

	cofport				port;

	union {
		uint8_t*						ofhu_port_status;
		struct ofp10_port_status*		ofhu10_port_status;
		struct ofp12_port_status*		ofhu12_port_status;
		struct ofp13_port_status*		ofhu13_port_status;
	} ofhu;

#define ofh_port_status   ofhu.ofhu_port_status
#define ofh10_port_status ofhu.ofhu10_port_status
#define ofh12_port_status ofhu.ofhu12_port_status
#define ofh13_port_status ofhu.ofhu13_port_status

public:


	/** constructor
	 *
	 */
	cofmsg_port_status(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint8_t reason = 0,
			cofport const& port = cofport());


	/**
	 *
	 */
	cofmsg_port_status(
			cofmsg_port_status const& port_status);


	/**
	 *
	 */
	cofmsg_port_status&
	operator= (
			cofmsg_port_status const& port_status);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_port_status();


	/**
	 *
	 */
	cofmsg_port_status(cmemory *memarea);


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
	uint8_t
	get_reason() const;

	/**
	 *
	 */
	void
	set_reason(uint8_t reason);

	/**
	 *
	 */
	cofport&
	get_port();
};

} // end of namespace rofl

#endif /* COFMSG_PORT_STATUS_H_ */
