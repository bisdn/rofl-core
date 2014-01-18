/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFLOWENTRY_H
#define CFLOWENTRY_H 1

#include <string>
#include <vector>
#include <algorithm>
#include <endian.h>
#include <strings.h>

#include "rofl/common/croflexception.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/cvastring.h"
#include "rofl/platform/unix/csyslog.h"

#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofaction.h"
#include "rofl/common/openflow/cofinstruction.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/coxmatch.h"

namespace rofl
{

class eFlowEntryBase : public RoflException {}; // error base class cflowentry
class eFlowEntryInvalid : public eFlowEntryBase {}; // invalid parameter
class eFlowEntryOutOfMem : public eFlowEntryBase {}; // out of memory


class cflowentry :
	public csyslog
{
private: // data structures

	uint8_t 		of_version;		// OpenFlow version in use

public: // data structures

	cofmatch 		match; 			// cofmatch class containing ofp_match structure
	cofactions		actions;		// list of actions for OF 1.0
	cofinstructions instructions; 	// list of instructions since OF1.2

	union {
		uint8_t								*ofmu_generic;
		struct openflow10::ofp_flow_mod		*ofmu10_flow_mod;
		struct openflow12::ofp_flow_mod 	*ofmu12_flow_mod;
		struct openflow13::ofp_flow_mod		*ofmu13_flow_mod;
	} ofm_ofmu;

#define ofm_generic			ofm_ofmu.ofmu_generic
#define of10m_flow_mod		ofm_ofmu.ofmu10_flow_mod
#define of12m_flow_mod		ofm_ofmu.ofmu12_flow_mod
#define of13m_flow_mod		ofm_ofmu.ofmu13_flow_mod


public: // static methods

	static void example();



public: // methods

	/** constructor
	 */
	cflowentry(uint8_t of_version, uint16_t type = openflow::OFPMT_OXM);

	/** destructor
	 */
	virtual
	~cflowentry();

	/** copy constructor
	 */
	cflowentry(cflowentry const& fe);

	/** assignment operator
	 */
	cflowentry& operator= (const cflowentry& fe);

	/** reset flowentry
	 *
	 */
	void
	reset();

	/**
	 * @brief
	 */
	size_t
	length() const;

	/** pack flowentry, i.e. add actions to flow_mod structure ready for transmission
	 * @return length of flow_mod structure including actions
	 */
	size_t
	pack();


public: // setter methods for ofp_flow_mod structure

	/**
	 */
	void
	set_version(uint8_t of_version) { this->of_version = of_version; };
	/**
	 */
	void
	set_command(uint8_t command);
	/**
	 */
	void
	set_table_id(uint8_t table_id);
	/**
	 */
	void
	set_idle_timeout(const uint16_t& idle_timeout);
	/**
	 */
	void
	set_hard_timeout(const uint16_t& hard_timeout);
	/**
	 */
	void
	set_cookie(const uint64_t& cookie);
	/**
	 */
	void
	set_cookie_mask(const uint64_t& cookie_mask);
	/**
	 */
	void
	set_priority(const uint16_t& priority);
	/**
	 */
	void
	set_buffer_id(const uint32_t& buffer_id);
	/**
	 */
	void
	set_out_port(const uint32_t& out_port);
	/**
	 */
	void
	set_out_group(const uint32_t& out_group);
	/**
	 */
	void
	set_flags(const uint16_t& flags);


public: // getter methods for ofp_flow_mod structure

	/**
	 */
	uint8_t
	get_version() const { return of_version; };
	/**
	 */
	uint8_t
	get_command() const;
	/**
	 */
	uint8_t
	get_table_id() const;
	/**
	 */
	uint16_t
	get_idle_timeout() const;
	/**
	 */
	uint16_t
	get_hard_timeout() const;
	/**
	 */
	uint64_t
	get_cookie() const;
	/**
	 */
	uint64_t
	get_cookie_mask() const;
	/**
	 */
	uint16_t
	get_priority() const;
	/**
	 */
	uint32_t
	get_buffer_id() const;
	/**
	 */
	uint32_t
	get_out_port() const;
	/**
	 */
	uint32_t
	get_out_group() const;
	/**
	 */
	uint16_t
	get_flags() const;




private: // data structures

	cmemory flow_mod_area;			// flow mod memory area

public:

	friend std::ostream&
	operator<< (std::ostream& os, cflowentry const& fe) {
		os << indent(0) << "<cflowentry >";
		switch (fe.of_version) {
		case openflow10::OFP_VERSION: {
			os << indent(2) << "<cookie:0x" << std::hex << (int)fe.get_cookie() 	<< std::dec << " >" << std::endl;
			switch (fe.get_command()) {
			case openflow10::OFPFC_ADD:
				os << indent(2) << "<command:ADD >" 			<< std::endl; break;
			case openflow10::OFPFC_DELETE:
				os << indent(2) << "<command:DELETE >" 			<< std::endl; break;
			case openflow10::OFPFC_DELETE_STRICT:
				os << indent(2) << "<command:DELETE-STRICT >" 	<< std::endl; break;
			case openflow10::OFPFC_MODIFY:
				os << indent(2) << "<command:MODIFY >" 			<< std::endl; break;
			case openflow10::OFPFC_MODIFY_STRICT:
				os << indent(2) << "<command:MODIFY-STRICT >" 	<< std::endl; break;
			default:
				os << indent(2) << "<command:UNKNOWN >" 		<< std::endl; break;
			}
			os << indent(2) << "<idle-timeout:" 	<< (int)fe.get_idle_timeout() 	<< " >" << std::endl;
			os << indent(2) << "<hard-timeout:" 	<< (int)fe.get_hard_timeout() 	<< " >" << std::endl;
			os << indent(2) << "<priority:" 		<< (int)fe.get_priority() 		<< " >" << std::endl;
			os << indent(2) << "<buffer-id:" 		<< (int)fe.get_buffer_id() 		<< " >" << std::endl;
			os << indent(2) << "<out-port:0x" << std::hex << (int)fe.get_out_port() 	<< std::dec << " >" << std::endl;
			os << indent(2) << "<flags:" 	<< std::hex << (int)fe.get_flags() 		<< std::dec << " >" << std::endl;
			indent i(4);
			os << fe.match;
			os << fe.actions;
		} break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			os << indent(2) << "<cookie:0x" << std::hex << (int)fe.get_cookie() 	<< std::dec << " >" << std::endl;
			os << indent(2) << "<cookie-mask:0x" << std::hex << (int)fe.get_cookie_mask() << std::dec << " >" << std::endl;
			os << indent(2) << "<table-id:" 		<< (int)fe.get_table_id() 		<< " >" << std::endl;
			switch (fe.get_command()) {
			case openflow12::OFPFC_ADD:
				os << indent(2) << "<command:ADD >" 			<< std::endl; break;
			case openflow12::OFPFC_DELETE:
				os << indent(2) << "<command:DELETE >" 			<< std::endl; break;
			case openflow12::OFPFC_DELETE_STRICT:
				os << indent(2) << "<command:DELETE-STRICT >" 	<< std::endl; break;
			case openflow12::OFPFC_MODIFY:
				os << indent(2) << "<command:MODIFY >" 			<< std::endl; break;
			case openflow12::OFPFC_MODIFY_STRICT:
				os << indent(2) << "<command:MODIFY-STRICT >" 	<< std::endl; break;
			default:
				os << indent(2) << "<command:UNKNOWN >" 		<< std::endl; break;
			}
			os << indent(2) << "<idle-timeout:" 	<< (int)fe.get_idle_timeout() 	<< " >" << std::endl;
			os << indent(2) << "<hard-timeout:" 	<< (int)fe.get_hard_timeout() 	<< " >" << std::endl;
			os << indent(2) << "<priority:" 		<< (int)fe.get_priority() 		<< " >" << std::endl;
			os << indent(2) << "<buffer-id:" 		<< (int)fe.get_buffer_id() 		<< " >" << std::endl;
			os << indent(2) << "<out-port:0x" << std::hex << (int)fe.get_out_port() 	<< std::dec << " >" << std::endl;
			os << indent(2) << "<out-group:0x" << std::hex << (int)fe.get_out_group() 	<< std::dec << " >" << std::endl;
			os << indent(2) << "<flags:" 	<< std::hex << (int)fe.get_flags() 		<< std::dec << " >" << std::endl;
			indent i(4);
			os << fe.match;
			os << fe.instructions;
		} break;
		default: {
			os << indent(2) << "<unknown OFP version >" << std::endl;
		} break;
		}
		return os;
	};
};

}; // end of namespace

#endif
