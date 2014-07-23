/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofasyncconfig.h
 *
 *  Created on: 18.03.2014
 *      Author: andreas
 */

#ifndef COFASYNCCONFIG_H_
#define COFASYNCCONFIG_H_

#include <inttypes.h>

#include <map>
#include <iostream>

#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class cofasync_config {

	uint8_t							ofp_version;
	std::map<uint8_t, uint32_t>		packet_in_mask;
	std::map<uint8_t, uint32_t>		port_status_mask;
	std::map<uint8_t, uint32_t>		flow_removed_mask;

public:

	struct async_config_t {
		uint32_t packet_in_mask[2];		/* Bitmasks of OFPR_* values. */
		uint32_t port_status_mask[2]; 	/* Bitmasks of OFPPR_* values. */
		uint32_t flow_removed_mask[2];	/* Bitmasks of OFPRR_* values. */
	};

public:

	/**
	 *
	 */
	cofasync_config(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofasync_config();

	/**
	 *
	 */
	cofasync_config(
			cofasync_config const& async_config);

	/**
	 *
	 */
	cofasync_config&
	operator= (
			cofasync_config const& async_config);


public:

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t* buf, size_t buflen);

public:

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

	/**
	 *
	 */
	uint32_t const&
	get_packet_in_mask_master() const;

	/**
	 *
	 */
	void
	set_packet_in_mask_master(uint32_t packet_in_mask_master);

	/**
	 *
	 */
	uint32_t&
	set_packet_in_mask_master();

	/**
	 *
	 */
	uint32_t const&
	get_packet_in_mask_slave() const;

	/**
	 *
	 */
	void
	set_packet_in_mask_slave(uint32_t packet_in_mask_slave);

	/**
	 *
	 */
	uint32_t&
	set_packet_in_mask_slave();


	/**
	 *
	 */
	uint32_t const&
	get_port_status_mask_master() const;

	/**
	 *
	 */
	void
	set_port_status_mask_master(uint32_t port_status_mask_master);

	/**
	 *
	 */
	uint32_t&
	set_port_status_mask_master();

	/**
	 *
	 */
	uint32_t const&
	get_port_status_mask_slave() const;

	/**
	 *
	 */
	void
	set_port_status_mask_slave(uint32_t port_status_mask_slave);

	/**
	 *
	 */
	uint32_t&
	set_port_status_mask_slave();


	/**
	 *
	 */
	uint32_t const&
	get_flow_removed_mask_master() const;

	/**
	 *
	 */
	void
	set_flow_removed_mask_master(uint32_t flow_removed_mask_master);

	/**
	 *
	 */
	uint32_t&
	set_flow_removed_mask_master();

	/**
	 *
	 */
	uint32_t const&
	get_flow_removed_mask_slave() const;

	/**
	 *
	 */
	void
	set_flow_removed_mask_slave(uint32_t flow_removed_mask_slave);

	/**
	 *
	 */
	uint32_t&
	set_flow_removed_mask_slave();


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofasync_config const& async_config) {
		os << rofl::indent(0) << "<cofasync_config ofp-version:" << (int)async_config.get_version() << " >" << std::endl;
		os << std::hex;
		os << rofl::indent(2) << "<packet-in-mask[EQUAL|MASTER]: 0x" <<
				(int)async_config.get_packet_in_mask_master() << " >" << std::endl;
		os << rofl::indent(2) << "<packet-in-mask[SLAVE]: 0x" <<
				(int)async_config.get_packet_in_mask_slave() << " >" << std::endl;
		os << rofl::indent(2) << "<port-status-mask[EQUAL|MASTER]: 0x" <<
				(int)async_config.get_port_status_mask_master() << " >" << std::endl;
		os << rofl::indent(2) << "<port-status-mask[SLAVE]: 0x" <<
				(int)async_config.get_port_status_mask_slave() << " >" << std::endl;
		os << rofl::indent(2) << "<flow-removed-mask[EQUAL|MASTER]: 0x" <<
				(int)async_config.get_flow_removed_mask_master() << " >" << std::endl;
		os << rofl::indent(2) << "<flow-removed-mask[SLAVE]: 0x" <<
				(int)async_config.get_flow_removed_mask_slave() << " >" << std::endl;
		os << std::dec;
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl




#endif /* COFASYNCCONFIG_H_ */
