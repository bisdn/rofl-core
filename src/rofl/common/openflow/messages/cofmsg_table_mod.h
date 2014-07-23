/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_table_mod.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_TABLE_MOD_H_
#define COFMSG_TABLE_MOD_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_table_mod :
	public cofmsg
{
private:

	union {
		uint8_t*					ofhu_table_mod;
		struct openflow12::ofp_table_mod*		ofhu12_table_mod;
		struct openflow13::ofp_table_mod*		ofhu13_table_mod;
	} ofhu;

#define ofh_table_mod   ofhu.ofhu_table_mod
#define ofh12_table_mod ofhu.ofhu12_table_mod
#define ofh13_table_mod ofhu.ofhu13_table_mod

public:


	/** constructor
	 *
	 */
	cofmsg_table_mod(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint8_t  table_id = 0,
			uint32_t config = 0);


	/**
	 *
	 */
	cofmsg_table_mod(
			cofmsg_table_mod const& table_mod);


	/**
	 *
	 */
	cofmsg_table_mod&
	operator= (
			cofmsg_table_mod const& table_mod);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_table_mod();


	/**
	 *
	 */
	cofmsg_table_mod(cmemory *memarea);


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
	get_table_id() const;

	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id);

	/**
	 *
	 */
	uint32_t
	get_config() const;

	/**
	 *
	 */
	void
	set_config(uint32_t config);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_table_mod const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(0) << "<cofmsg_table_mod >" << std::endl;
			os << indent(2) << "<table-id:" << (int)msg.get_table_id() << " >" << std::endl;
			os << indent(2) << "<config:0x" << std::hex << (int)msg.get_config() << std::dec << " >" << std::endl;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_TABLE_MOD_H_ */
