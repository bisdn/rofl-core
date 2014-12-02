/*
 * cofmsg_hello.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_HELLO_H_
#define COFMSG_HELLO_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_hello :
	public cofmsg
{
private:

	cmemory			body;

public:


	/** constructor
	 *
	 */
	cofmsg_hello(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint8_t* data = 0,
			size_t datalen = 0);


	/**
	 *
	 */
	cofmsg_hello(
			cofmsg_hello const& hello);


	/**
	 *
	 */
	cofmsg_hello&
	operator= (
			cofmsg_hello const& hello);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_hello();


	/**
	 *
	 */
	cofmsg_hello(cmemory *memarea);


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
	operator<< (std::ostream& os, cofmsg_hello const& msg) {
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_hello >" << std::endl;
		os << indent(2) << msg.body;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "-Hello- " << cofmsg::str();
		return ss.str();
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_HELLO_H_ */
