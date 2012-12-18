/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFWDENTRY_H
#define CFWDENTRY_H

#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include <endian.h>
#ifndef htobe16
#include "../common/endian_conversion.h"
#endif

#include "../common/openflow/openflow12.h"
#ifdef __cplusplus
}
#endif

#include "../common/ciosrv.h"
#include "../common/cmacaddr.h"
#include "../common/cvastring.h"

//#include "openflow/cofdpath.h"

namespace rofl
{

class eFwdEntryBase : public cerror {};
class eFwdEntrySearchInvalidHWAddr : public eFwdEntryBase {}; // no
class eFwdEntryInvalidHWAddr : public eFwdEntryBase {}; // no


/** 
 * cfwdentry contains MAC address entries for cfwdtables
 * it is used for providing legacy L2 switching functionality
 * idea: 
 * - an entry generated sets a timer 
 * - when this timer expires, it removes itself from its fwdtable
 */
class cfwdentry : public ciosrv {

	enum cfwdentry_timer_t {
		TIMER_CFWDENTRY_BASE = 0x0016,
		TIMER_CFWDENTRY_EXPIRED = ((TIMER_CFWDENTRY_BASE << 16) | 0x0001),
	};

public: // data structures

	// the mac address associated to this fwdentry: copied in constructor
	cmacaddr maddr;
	// datapath this port is connected to
	uint64_t dpid;
	// port_no
	uint32_t port_no;
	// the vlan identifier associated to this fwdentry
	uint16_t vlan_id;

public:
	/** constructor
	 */
	cfwdentry(
			std::set<cfwdentry*> *fwdtable,
			cmacaddr const& maddr,
			uint64_t dpid,
			uint32_t port_no,
			uint16_t vlan_id = 0xffff, // 0xffff: untagged frame
			int timeout = 15);

	/** destructor
	 */
	virtual
	~cfwdentry();

	/* equality operator */
	bool
	operator==(
			const cfwdentry &param) const;

	/* inequality operator */
	bool
	operator!=(
			const cfwdentry &param) const;

	/** dump info for this cfwdentry
	 */
	const char*
	c_str();

	/** refresh an entry
	 */
	void
	refresh_entry(
			uint64_t dpid,
			uint32_t port_no);

protected: // methods overwritten from ciosrv

	/** 
	 * handle_timeout
	 */
	void
	handle_timeout(
			int opaque);

public: // auxiliary classes

	class cfwdentry_search {
		cmacaddr maddr;
		uint16_t vlan_id;
		public:
			cfwdentry_search(
					cmacaddr const& __maddr,
					uint16_t __vlan_id) :
				maddr(__maddr),
				vlan_id(__vlan_id) {};
			bool operator() (const cfwdentry* fwdentry) {
				return ((fwdentry->vlan_id == vlan_id) && (fwdentry->maddr == maddr));
			};
	};

protected: // data structures

	// the fwd table this entry belongs to
	std::set<cfwdentry*> *fwdtable;
	// info string
	std::string info;
	// expiration timer	
	time_t fwdentry_timeout;

};

}; // end of namespace

#endif
