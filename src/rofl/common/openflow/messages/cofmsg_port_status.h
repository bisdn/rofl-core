/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_port_status.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PORT_STATUS_H_
#define COFMSG_PORT_STATUS_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofport.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_port_status :
	public cofmsg
{
private:

	rofl::openflow::cofport				port;

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
			rofl::openflow::cofport const& port = rofl::openflow::cofport());


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
	virtual uint8_t*
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
	rofl::openflow::cofport&
	set_port() { return port; };

	/**
	 *
	 */
	rofl::openflow::cofport const&
	get_port() const { return port; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_port_status const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_port_status >" << std::endl;
			switch (msg.get_reason()) {
			case openflow::OFPPR_ADD: {
				os << indent(4) << "<reason: -ADD- >" << std::endl;
			} break;
			case openflow::OFPPR_DELETE: {
				os << indent(4) << "<reason: -DELETE- >" << std::endl;
			} break;
			case openflow::OFPPR_MODIFY: {
				os << indent(4) << "<reason: -MODIFY- >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<reason: -UNKNOWN- >" << std::endl;
			} break;
			}
			indent i(4);
			os << msg.port;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_PORT_STATUS_H_ */
