/*
 * clldpmsg.h
 *
 *  Created on: 04.03.2014
 *      Author: andreas
 */

#ifndef CLLDPMSG_H_
#define CLLDPMSG_H_

#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"
#include "rofl/common/cmacaddr.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/protocols/clldpattr.h"
#include "rofl/common/protocols/clldpattrs.h"

namespace rofl {
namespace protocol {
namespace lldp {

class eLLDPMsgBase		: public RoflException {};
class eLLDPMsgInval		: public eLLDPMsgBase {};
class eLLDPMsgNotFound	: public eLLDPMsgBase {};

class clldpmsg
{
	clldpattrs			attrs;
	cmacaddr			eth_dst;
	cmacaddr			eth_src;
	uint16_t			eth_type;

	static unsigned int const LLDP_ETHER_TYPE = 0x88cc;

public:

	/**
	 *
	 */
	clldpmsg();

	/**
	 *
	 */
	virtual
	~clldpmsg();

	/**
	 *
	 */
	clldpmsg(clldpmsg const& msg);

	/**
	 *
	 */
	clldpmsg&
	operator= (clldpmsg const& msg);

public:

	/**
	 *
	 */
	size_t
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

public:

	/**
	 *
	 */
	clldpattrs&
	set_attrs() { return attrs; };

	/**
	 *
	 */
	clldpattrs const&
	get_attrs() const { return attrs; };

	/**
	 *
	 */
	cmacaddr&
	set_eth_dst() { return eth_dst; };

	/**
	 *
	 */
	cmacaddr const&
	get_eth_dst() const { return eth_dst; };

	/**
	 *
	 */
	cmacaddr&
	set_eth_src() { return eth_src; };

	/**
	 *
	 */
	cmacaddr const&
	get_eth_src() const { return eth_src; };

	/**
	 *
	 */
	uint16_t const&
	get_eth_type() const { return eth_type; };

	/**
	 *
	 */
	uint16_t&
	set_eth_type() { return eth_type; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, clldpmsg const& msg) {
		os << rofl::indent(0) << "<clldpmsg length:" << msg.length() << " >" << std::endl;
		os << rofl::indent(2) << "<eth-dst: " << msg.get_eth_dst() << " >" << std::endl;
		os << rofl::indent(2) << "<eth-src: " << msg.get_eth_src() << " >" << std::endl;
		os << rofl::indent(2) << "<eth-type: 0x" << std::hex << (int)msg.get_eth_type() << std::dec << " >" << std::endl;
		rofl::indent i(2);
		os << msg.get_attrs();
		return os;
	};
};


}; // end of namespace lldp
}; // end of namespace protocol
}; // end of namespace rofl



#endif /* CLLDPMSG_H_ */
