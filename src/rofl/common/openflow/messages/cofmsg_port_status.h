/*
 * cofmsg_port_status.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PORT_STATUS_H_
#define COFMSG_PORT_STATUS_H_ 1

#include "cofmsg.h"
#include "rofl/common/openflow/cofport.h"

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
		struct openflow10::ofp_port_status*		ofhu10_port_status;
		struct openflow12::ofp_port_status*		ofhu12_port_status;
		struct openflow13::ofp_port_status*		ofhu13_port_status;
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

	/**
	 *
	 */
	cofport const&
	get_port_const() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_port_status const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(0) << "<cofmsg_port_status >" << std::endl;
			switch (msg.get_reason()) {
			case openflow::OFPPR_ADD: {
				os << indent(2) << "<reason: -ADD- >" << std::endl;
			} break;
			case openflow::OFPPR_DELETE: {
				os << indent(2) << "<reason: -DELETE- >" << std::endl;
			} break;
			case openflow::OFPPR_MODIFY: {
				os << indent(2) << "<reason: -MODIFY- >" << std::endl;
			} break;
			default: {
				os << indent(2) << "<reason: -UNKNOWN- >" << std::endl;
			} break;
			}
			indent i(2);
			os << msg.port;
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_PORT_STATUS_H_ */
