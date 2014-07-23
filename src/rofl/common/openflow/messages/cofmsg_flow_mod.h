/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_flow_mod.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FLOW_MOD_H_
#define COFMSG_FLOW_MOD_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofflowmod.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_flow_mod : public cofmsg {
public:

	/**
	 *
	 */
	cofmsg_flow_mod(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN, uint32_t xid = 0, const cofflowmod& flowmod = cofflowmod());

	/**
	 *
	 */
	cofmsg_flow_mod(
			const cofmsg_flow_mod& msg);

	/**
	 *
	 */
	cofmsg_flow_mod&
	operator= (
			const cofmsg_flow_mod& msg);

	/**
	 *
	 */
	virtual
	~cofmsg_flow_mod();

	/**
	 *
	 */
	cofmsg_flow_mod(
			cmemory *memarea);

public:

	/** reset packet content
	 *
	 */
	virtual void
	reset();

	/** parse packet and validate it
	 */
	virtual void
	validate();

	/**
	 *
	 */
	void
	check_prerequisites() const;

public:

	/**
	 *
	 */
	virtual void
	set_version(uint8_t ofp_version) {
		cofmsg::set_version(ofp_version);
		flowmod.set_version(ofp_version);
	};

	/**
	 *
	 */
	rofl::openflow::cofflowmod&
	set_flowmod() { return flowmod; };

	/**
	 *
	 */
	const rofl::openflow::cofflowmod&
	get_flowmod() const { return flowmod; };

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
	pack(
			uint8_t *buf = (uint8_t*)0, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_flow_mod& msg) {
		os << rofl::indent(0) << "<cofmsg_flow_mod >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofmsg&>( msg ); };
		{ rofl::indent i(4); os << msg.get_flowmod(); };
		return os;
	};

private:

	rofl::openflow::cofflowmod	flowmod;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_FLOW_MOD_H_ */
