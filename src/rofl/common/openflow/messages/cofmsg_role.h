/*
 * cofmsg_role.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_ROLE_H
#define COFMSG_ROLE_H 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofrole.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_role_request :
	public cofmsg
{
	rofl::openflow::cofrole				role;

public:


	/** constructor
	 *
	 */
	cofmsg_role_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			rofl::openflow::cofrole const& role = rofl::openflow::cofrole());


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

public:

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
	rofl::openflow::cofrole&
	set_role() { return role; };

	/**
	 *
	 */
	rofl::openflow::cofrole const&
	get_role() const { return role; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_role_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(0) << "<cofmsg_role_request >" << std::endl;
		rofl::indent i(2);
		os << msg.role;
		return os;
	};
};



/**
 *
 */
class cofmsg_role_reply :
	public cofmsg
{
	rofl::openflow::cofrole				role;

public:


	/** constructor
	 *
	 */
	cofmsg_role_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			rofl::openflow::cofrole const& role = rofl::openflow::cofrole());


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

public:

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
	rofl::openflow::cofrole&
	set_role() { return role; };

	/**
	 *
	 */
	rofl::openflow::cofrole const&
	get_role() const { return role; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_role_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(0) << "<cofmsg_role_reply >" << std::endl;
		rofl::indent i(2);
		os << msg.role;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_ROLE_H_ */
