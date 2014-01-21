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
#include "rofl/common/openflow/cofflowmod.h"

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
	cofinstructions		instructions; 	// since OF1.1
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
			uint8_t of_version,
			uint32_t xid,
			cflowentry const& fe);


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
		switch (msg.get_version()) {
		case OFP10_VERSION: {
			os << indent(0) << dynamic_cast<cofmsg const&>( msg );
			os << indent(2) << "<cofmsg_flow_mod >" << std::endl;
			switch (msg.get_command()) {
			case rofl::openflow10::OFPFC_ADD: {
				os << indent(4) << "<command: -ADD- >" << std::endl;
			} break;
			case rofl::openflow10::OFPFC_MODIFY: {
				os << indent(4) << "<command: -MODIFY- >" << std::endl;
			} break;
			case rofl::openflow10::OFPFC_MODIFY_STRICT: {
				os << indent(4) << "<command: -MODIFY-STRICT- >" << std::endl;
			} break;
			case rofl::openflow10::OFPFC_DELETE: {
				os << indent(4) << "<command: -DELETE- >" << std::endl;
			} break;
			case rofl::openflow10::OFPFC_DELETE_STRICT: {
				os << indent(4) << "<command: -DELETE-STRICT- >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<command: -UNKNOWN- >" << std::endl;
			};
			}
			os << indent(4) << "<table-id:" 		<< (int)msg.get_table_id() << " >" << std::endl;
			os << indent(4) << "<cookie:0x" 		<< std::hex << (unsigned long long)msg.get_cookie() << std::dec << " >" << std::endl;
			os << indent(4) << "<idle-timeout:" 	<< (int)msg.get_idle_timeout() << " >" << std::endl;;
			os << indent(4) << "<hard-timeout:" 	<< (int)msg.get_hard_timeout() << " >" << std::endl;
			os << indent(4) << "<priority:0x" 		<< std::hex << (int)msg.get_priority() << std::dec << " >" << std::endl;
			os << indent(4) << "<buffer-id:0x" 		<< std::hex << (int)msg.get_buffer_id() << std::dec << " >" << std::endl;
			os << indent(4) << "<flags:0x" 			<< std::hex << (int)msg.get_flags() << std::dec << " >" << std::endl;
			os << indent(4) << "<out-port:0x" 		<< std::hex << (int)msg.get_out_port() << std::dec << " >" << std::endl;
			{
				os << indent(4) << "<matches: >" << std::endl;
				indent i(6);
				os << msg.match;
			}
			{
				os << indent(4) << "<actions: >" << std::endl;
				indent i(6);
				os << msg.actions;
			}
		} break;
		case OFP12_VERSION:
		case OFP13_VERSION: {
			os << indent(0) << dynamic_cast<cofmsg const&>( msg );
			os << indent(2) << "<cofmsg_flow_mod >" << std::endl;
			switch (msg.get_command()) {
			case rofl::openflow10::OFPFC_ADD: {
				os << indent(4) << "<command: -ADD- >" << std::endl;
			} break;
			case rofl::openflow10::OFPFC_MODIFY: {
				os << indent(4) << "<command: -MODIFY- >" << std::endl;
			} break;
			case rofl::openflow10::OFPFC_MODIFY_STRICT: {
				os << indent(4) << "<command: -MODIFY-STRICT- >" << std::endl;
			} break;
			case rofl::openflow10::OFPFC_DELETE: {
				os << indent(4) << "<command: -DELETE- >" << std::endl;
			} break;
			case rofl::openflow10::OFPFC_DELETE_STRICT: {
				os << indent(4) << "<command: -DELETE-STRICT- >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<command: -UNKNOWN- >" << std::endl;
			};
			}
			os << indent(4) << "<table-id:" 		<< (int)msg.get_table_id() << " >" << std::endl;
			os << indent(4) << "<cookie:0x" 		<< std::hex << (unsigned long long)msg.get_cookie() << std::dec << " >" << std::endl;
			os << indent(4) << "<cookie-mask:0x" 	<< std::hex << (unsigned long long)msg.get_cookie_mask() << std::dec << " >" << std::endl;
			os << indent(4) << "<idle-timeout:" 	<< (int)msg.get_idle_timeout() << " >" << std::endl;;
			os << indent(4) << "<hard-timeout:" 	<< (int)msg.get_hard_timeout() << " >" << std::endl;
			os << indent(4) << "<priority:0x" 		<< std::hex << (int)msg.get_priority() << std::dec << " >" << std::endl;
			os << indent(4) << "<buffer-id:0x" 		<< std::hex << (int)msg.get_buffer_id() << std::dec << " >" << std::endl;
			os << indent(4) << "<flags:0x" 			<< std::hex << (int)msg.get_flags() << std::dec << " >" << std::endl;
			os << indent(4) << "<out-port:0x" 		<< std::hex << (int)msg.get_out_port() << std::dec << " >" << std::endl;
			os << indent(4) << "<out-group:0x" 		<< std::hex << (int)msg.get_out_group() << std::dec << " >" << std::endl;
			{
				os << indent(4) << "<matches: >" << std::endl;
				indent i(6);
				os << msg.match;
			}
			{
				os << indent(4) << "<instructions: >" << std::endl;
				indent i(6);
				os << msg.instructions;
			}
		} break;
		}
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_FLOW_MOD_H_ */
