/*
 * cofmsg_echo.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_ECHO_H_
#define COFMSG_ECHO_H_ 1

#include "cofmsg.h"

namespace rofl
{

/**
 *
 */
class cofmsg_echo :
	public cofmsg
{
private:

	cmemory			body;

public:


	/** constructor
	 *
	 */
	cofmsg_echo(
			uint8_t of_version = 0,
			uint8_t type = 0,
			uint32_t xid = 0,
			uint8_t* data = 0,
			size_t datalen = 0);


	/**
	 *
	 */
	cofmsg_echo(
			cofmsg_echo const& echo);


	/**
	 *
	 */
	cofmsg_echo&
	operator= (
			cofmsg_echo const& echo);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_echo();


	/**
	 *
	 */
	cofmsg_echo(cmemory *memarea);


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


typedef cofmsg_echo cofmsg_echo_request;
typedef cofmsg_echo cofmsg_echo_reply;


} // end of namespace rofl

#endif /* COFMSG_ECHO_H_ */
