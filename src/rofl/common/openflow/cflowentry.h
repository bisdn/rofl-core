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

#include "../cerror.h"
#include "../cmemory.h"
#include "../caddress.h"
#include "../cvastring.h"
#include "rofl/platform/unix/csyslog.h"

#include "cofmatch.h"
#include "cofaction.h"
#include "cofinst.h"
#include "cofinlist.h"
#include "coxmatch.h"

namespace rofl
{

class eFlowEntryBase : public cerror {}; // error base class cflowentry
class eFlowEntryInvalid : public eFlowEntryBase {}; // invalid parameter
class eFlowEntryOutOfMem : public eFlowEntryBase {}; // out of memory


class cflowentry :
	public csyslog
{
private: // data structures

	uint8_t 		of_version;		// OpenFlow version in use

public: // data structures

	cofmatch 		match; 			// cofmatch class containing ofp_match structure

	cofinlist 		instructions; 	// list of instructions

	union {
		struct ofp12_flow_mod 		*ofmu12_flow_mod;
		struct ofp13_flow_mod		*ofmu13_flow_mod;
	} ofm_ofmu;

#define of12m_flow_mod		ofm_ofmu.ofmu12_flow_mod
#define of13m_flow_mod		ofm_ofmu.ofmu13_flow_mod


public: // static methods

	static void test();



public: // methods

	/** constructor
	 */
	cflowentry(uint8_t of_version, uint16_t type = OFPMT_OXM) throw (eFlowEntryOutOfMem);

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

#if 0
	/**
	 *
	 */
	bool
	operator< (cflowentry const& fe) const;
#endif

	/** reset flowentry
	 *
	 */
	void
	reset();

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
		os << "<cflowentry ";
		switch (fe.of_version) {
		case OFP12_VERSION: {
			switch (fe.get_command()) {
			case OFPFC_ADD: 			os << "OFPFC-ADD "; 			break;
			case OFPFC_DELETE:			os << "OFPFC-DELETE "; 			break;
			case OFPFC_DELETE_STRICT:	os << "OFPFC-DELETE-STRICT "; 	break;
			case OFPFC_MODIFY:			os << "OFPFC-MODIFY "; 			break;
			case OFPFC_MODIFY_STRICT:	os << "OFPFC-MODIFY-STRICT ";	break;
			default:					os << "UNKNOWN ";				break;
			}
			os << "table-id:" 		<< (int)fe.get_table_id() << " ";
			os << "buffer-id:" 		<< (int)fe.get_buffer_id() << " ";
			os << "idle-timeout:" 	<< (int)fe.get_idle_timeout() << " ";
			os << "hard-timeout:" 	<< (int)fe.get_hard_timeout() << " ";
			os << "priority:" 		<< (int)fe.get_priority() << " ";
			os << std::endl;
			os << "match:" 			<< fe.match << std::endl;
			os << "instructions:" 	<< fe.instructions << std::endl;
		} break;
		case OFP13_VERSION: {
			switch (fe.get_command()) {
			case OFPFC_ADD: 			os << "OFPFC-ADD "; 			break;
			case OFPFC_DELETE:			os << "OFPFC-DELETE "; 			break;
			case OFPFC_DELETE_STRICT:	os << "OFPFC-DELETE-STRICT "; 	break;
			case OFPFC_MODIFY:			os << "OFPFC-MODIFY "; 			break;
			case OFPFC_MODIFY_STRICT:	os << "OFPFC-MODIFY-STRICT ";	break;
			default:					os << "UNKNOWN ";				break;
			}
			os << "table-id:" 		<< (int)fe.get_table_id() << " ";
			os << "buffer-id:" 		<< (int)fe.get_buffer_id() << " ";
			os << "idle-timeout:" 	<< (int)fe.get_idle_timeout() << " ";
			os << "hard-timeout:" 	<< (int)fe.get_hard_timeout() << " ";
			os << "priority:" 		<< (int)fe.get_priority() << " ";
			os << std::endl;
			os << "match:" 			<< fe.match << std::endl;
			os << "instructions:" 	<< fe.instructions << std::endl;
		} break;
		default: {
			os << "unknown";
		} break;
		}
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif
