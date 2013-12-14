/*
 * cofmsg_flow_mod.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FLOW_MOD_H_
#define COFMSG_FLOW_MOD_H_ 1

#include "cofmsg.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/cofmatch.h"

namespace rofl
{

/**
 *
 */
class cofmsg_flow_mod :
	public cofmsg
{
private:

	cofactions			actions; 		// for OF1.0
	cofinstructions			instructions; 	// since OF1.1
	cofmatch			match;

	union {
		uint8_t*								ofhu_flow_mod;
		struct openflow10::ofp_flow_mod*		ofhu10_flow_mod;
		struct openflow12::ofp_flow_mod*		ofhu12_flow_mod;
		struct openflow13::ofp_flow_mod*		ofhu13_flow_mod;
	} ofhu;

#define ofh_flow_mod   ofhu.ofhu_flow_mod
#define ofh10_flow_mod ofhu.ofhu10_flow_mod
#define ofh12_flow_mod ofhu.ofhu12_flow_mod
#define ofh13_flow_mod ofhu.ofhu13_flow_mod


public:


	/** constructor
	 *
	 */
	cofmsg_flow_mod(
			uint8_t of_version = 0, // must be openflow10::OFP_VERSION
			uint32_t xid = 0,
			uint64_t cookie = 0,
			uint8_t  command = 0,
			uint16_t idle_timeout = 0,
			uint16_t hard_timeout = 0,
			uint16_t priority = 0,
			uint32_t buffer_id = 0,
			uint16_t out_port = 0,
			uint16_t flags = 0,
			cofactions const& actions = cofactions(),
			cofmatch const& match = cofmatch(openflow10::OFP_VERSION));


	/** constructor
	 *
	 */
	cofmsg_flow_mod(
			uint8_t of_version = 0,  // openflow12::OFP_VERSION, openflow13::OFP_VERSION, and beyond
			uint32_t xid = 0,
			uint64_t cookie = 0,
			uint64_t cookie_mask = 0,
			uint8_t  table_id = 0,
			uint8_t  command = 0,
			uint16_t idle_timeout = 0,
			uint16_t hard_timeout = 0,
			uint16_t priority = 0,
			uint32_t buffer_id = 0,
			uint32_t out_port = 0,
			uint32_t out_group = 0,
			uint16_t flags = 0,
			cofinstructions const& instructions = cofinstructions(),
			cofmatch const& match = cofmatch(openflow12::OFP_VERSION));


	/**
	 *
	 */
	cofmsg_flow_mod(
			cofmsg_flow_mod const& flow_mod);


	/**
	 *
	 */
	cofmsg_flow_mod&
	operator= (
			cofmsg_flow_mod const& flow_mod);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_flow_mod();


	/**
	 *
	 */
	cofmsg_flow_mod(cmemory *memarea);


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
	uint64_t
	get_cookie() const;

	/**
	 *
	 */
	void
	set_cookie(uint64_t cookie);

	/**
	 *
	 */
	uint64_t
	get_cookie_mask() const;

	/**
	 *
	 */
	void
	set_cookie_mask(uint64_t cookie_mask);

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
	uint8_t
	get_command() const;

	/**
	 *
	 */
	void
	set_command(uint8_t command);

	/**
	 *
	 */
	uint16_t
	get_idle_timeout() const;

	/**
	 *
	 */
	void
	set_idle_timeout(uint16_t idle_timeout);

	/**
	 *
	 */
	uint16_t
	get_hard_timeout() const;

	/**
	 *
	 */
	void
	set_hard_timeout(uint16_t hard_timeout);

	/**
	 *
	 */
	uint16_t
	get_priority() const;

	/**
	 *
	 */
	void
	set_priority(uint16_t priority);

	/**
	 *
	 */
	uint32_t
	get_buffer_id() const;

	/**
	 *
	 */
	void
	set_buffer_id(uint32_t buffer_id);

	/**
	 *
	 */
	uint32_t
	get_out_port() const;

	/**
	 *
	 */
	void
	set_out_port(uint32_t out_port);

	/**
	 *
	 */
	uint32_t
	get_out_group() const;

	/**
	 *
	 */
	void
	set_out_group(uint32_t out_group);

	/**
	 *
	 */
	uint16_t
	get_flags() const;

	/**
	 *
	 */
	void
	set_flags(uint16_t flags);

	/**
	 *
	 */
	cofactions&
	get_actions();

	/**
	 *
	 */
	cofinstructions&
	get_instructions();

	/**
	 *
	 */
	cofmatch&
	get_match();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_flow_mod const& msg) {
		os << "cofmsg_flow_mod ";
			os << dynamic_cast<cofmsg const&>( msg ) << std::endl;
			os << "command:" << (int)msg.get_command() << " ";
			os << "cookie:" << (unsigned long long)msg.get_cookie() << " ";
			os << "cookie-mask:" << (unsigned long long)msg.get_cookie_mask() << " " << std::endl;
			os << "table-id:" << (int)msg.get_table_id() << " ";
			os << "idle-timeout:" << (int)msg.get_idle_timeout() << " ";
			os << "hard-timeout:" << (int)msg.get_hard_timeout() << " " << std::endl;
			os << "priority:" << (int)msg.get_priority() << " ";
			os << "buffer-id:" << (int)msg.get_buffer_id() << " ";
			os << "flags:" << (int)msg.get_flags() << " " << std::endl;
			os << "match:" << msg.match << std::endl;
			switch (msg.get_version()) {
			case OFP10_VERSION: {
				os << "actions:" << msg.actions << " ";
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				os << "instructions:" << msg.instructions << " ";
			} break;
			}
		os << ">";
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_FLOW_MOD_H_ */
