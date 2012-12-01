/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFLOWENTRY_H
#define CFLOWENTRY_H 1

#include <string>
#include <vector>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

#include <endian.h>
#include <strings.h>

#ifdef __cplusplus
}
#endif


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


class eFlowEntryBase : public cerror {}; // error base class cflowentry
class eFlowEntryInvalid : public eFlowEntryBase {}; // invalid parameter
class eFlowEntryOutOfMem : public eFlowEntryBase {}; // out of memory


class cflowentry :
	public csyslog
{
public: // static methods

	static void test();

public: // data structures

	cofmatch match; 				// cofmatch class containing ofp_match structure

	cofinlist instructions; 		// list of instructions

	struct ofp_flow_mod *flow_mod; 	// ofp_flow_mod structure



public: // methods

	/** constructor
	 */
	cflowentry(uint16_t type = OFPMT_OXM) throw (eFlowEntryOutOfMem);

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

	/**
	 *
	 */
	bool
	operator< (cflowentry const& fe) const;

	/** reset flowentry
	 *
	 */
	void
	reset();

	/** dump flow entry info
	 */
	const char*
	c_str();

	/** pack flowentry, i.e. add actions to flow_mod structure ready for transmission
	 * @return length of flow_mod structure including actions
	 */
	size_t
	pack();

#if 0
public: // setter methods for ofp_match structure

	/**
	 */
	void match_set_type(const uint16_t& type = OFPMT_STANDARD);
	/**
	 */
	void match_set_in_port(const uint16_t& in_port);
	/**
	 */
	void match_set_dl_src(const cmemory& dl_src, const cmemory& dl_src_mask = cmemory(0));
	/**
	 */
	void match_set_dl_dst(const cmemory& dl_dst, const cmemory& dl_dst_mask = cmemory(0));
	/**
	 */
	void match_set_dl_type(const uint16_t& dl_type);
	/**
	 */
	void match_set_dl_vlan(const uint16_t& dl_vlan);
	/**
	 */
	void match_set_dl_vlan_pcp(const uint8_t& dl_vlan_pcp);
	/**
	 */
	void match_set_nw_tos(const uint8_t& nw_tos);
	/**
	 */
	void match_set_nw_proto(const uint8_t& nw_proto);
	/**
	 */
	void match_set_nw_src(const caddress& nw_src, uint8_t nw_mask = 0);
	/**
	 */
	void match_set_nw_dst(const caddress& nw_dst, uint8_t nw_mask = 0);
	/**
	 */
	void match_set_tp_src(const uint16_t& tp_src);
	/**
	 */
	void match_set_tp_dst(const uint16_t& tp_dst);
	/**
	 */
	void match_set_pppoe_code(uint8_t code);
	/**
	 */
	void match_set_pppoe_type(uint8_t type);
	/**
	 */
	void match_set_pppoe_sessid(uint16_t sessid);
#endif

public: // setter methods for ofp_flow_mod structure

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

	std::string info;				// info string
	cmemory flow_mod_area;			// flow mod memory area
};

#endif
