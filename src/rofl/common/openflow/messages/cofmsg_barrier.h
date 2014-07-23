/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_barrier.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_BARRIER_H_
#define COFMSG_BARRIER_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_barrier_request :
	public cofmsg
{
private:

	cmemory			body;

public:


	/** constructor
	 *
	 */
	cofmsg_barrier_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint8_t* data = 0,
			size_t datalen = 0);


	/**
	 *
	 */
	cofmsg_barrier_request(
			cofmsg_barrier_request const& barrier);


	/**
	 *
	 */
	cofmsg_barrier_request&
	operator= (
			cofmsg_barrier_request const& barrier);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_barrier_request();


	/**
	 *
	 */
	cofmsg_barrier_request(cmemory *memarea);


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

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_barrier_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_barrier_request >" << std::endl;
		indent i(4);
		os << msg.body;
		return os;
	};
};



/**
 *
 */
class cofmsg_barrier_reply :
	public cofmsg
{
private:

	cmemory			body;

public:


	/** constructor
	 *
	 */
	cofmsg_barrier_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint8_t* data = 0,
			size_t datalen = 0);


	/**
	 *
	 */
	cofmsg_barrier_reply(
			cofmsg_barrier_reply const& barrier);


	/**
	 *
	 */
	cofmsg_barrier_reply&
	operator= (
			cofmsg_barrier_reply const& barrier);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_barrier_reply();


	/**
	 *
	 */
	cofmsg_barrier_reply(cmemory *memarea);


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

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_barrier_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_barrier_reply >" << std::endl;
		indent i(4);
		os << msg.body;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_BARRIER_H_ */
