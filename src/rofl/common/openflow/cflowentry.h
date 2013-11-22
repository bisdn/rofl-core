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

	/** dump flow entry info
	 */
	const char*
	c_str();

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

	std::string info;				// info string
	cmemory flow_mod_area;			// flow mod memory area

public:

	friend std::ostream&
	operator<< (std::ostream& os, cflowentry const& fe) {
		// TODO
		return os;
	};
};

}; // end of namespace

#endif
