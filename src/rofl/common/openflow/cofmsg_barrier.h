/*
 * cofmsg_barrier.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_BARRIER_H_
#define COFMSG_BARRIER_H_ 1

#include "cofmsg.h"

namespace rofl
{

/**
 *
 */
class cofmsg_barrier :
	public cofmsg
{
private:

	cmemory			body;

public:


	/** constructor
	 *
	 */
	cofmsg_barrier(
			uint8_t of_version = 0,
			uint8_t type = OFPT_BARRIER_REQUEST,
			uint32_t xid = 0,
			uint8_t* data = 0,
			size_t datalen = 0);


	/**
	 *
	 */
	cofmsg_barrier(
			cofmsg_barrier const& barrier);


	/**
	 *
	 */
	cofmsg_barrier&
	operator= (
			cofmsg_barrier const& barrier);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_barrier() {};


	/**
	 *
	 */
	cofmsg_barrier(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


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
	cmemory&
	get_body();
};


typedef cofmsg_barrier cofmsg_barrier_request;
typedef cofmsg_barrier cofmsg_barrier_reply;


} // end of namespace rofl

#endif /* COFMSG_BARRIER_H_ */
