/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFWDTABLE_H
#define CFWDTABLE_H

#include <map>
#include <set>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <inttypes.h>
#include "../openflow/openflow12.h"

#include <endian.h>
#ifndef htobe16
#include "endian_conversion.h"

#endif

#ifdef __cplusplus
}
#endif

#include "../cmacaddr.h"
#include "../cerror.h"
#include "cfwdtable.h"
#include "cfwdentry.h"
#include "../cvastring.h"

#include "../protocols/fetherframe.h"
#include "../protocols/fvlanframe.h"

#include "rofl/platform/unix/csyslog.h"


class cfwdentry; // forward declaration

/* error classes */
class eFwdTableNotFound : public cerror {}; // no entry for MAC address found


class cfwdtable : public csyslog {

	std::string info;

public:


	/** constructor
	 */
	cfwdtable();


	/** destructor
	 */
	virtual
	~cfwdtable();


	/** STP learning
	 */
	void
	mac_learning(
			cpacket &pack,
			uint64_t dpid,
			uint32_t port_no);


	/**
	 *
	 */
	bool
	mac_is_known(
			cpacket& pack);


	/** STP return next hop
	 * for vlan id and hwaddr
	 */
	uint32_t
	mac_next_hop(
			cpacket& pack) throw(eFwdTableNotFound);


	/**
	 *
	 */
	uint32_t
	next_hop_port_no(
			cpacket& pack) throw (eFwdTableNotFound);


	/**
	 *
	 */
	uint64_t
	next_hop_dpid(
			cpacket& pack) throw (eFwdTableNotFound);


	/** dump info
	 *
	 */
	const char*
	c_str();

	/**
	 *
	 */
	cfwdentry&
	get_egress_fwdentry(
			cmacaddr const& ma,
			uint16_t dl_vid = 0xffff) throw (eFwdTableNotFound);

	/* each VLAN gets its own map for hosting entries, so we have separated
	 * forwarding tables between vlans. 
	 */

	// forwarding entries table
	std::map<uint16_t, std::set<cfwdentry*> > fwdtable;

public: // auxiliary classes

};

#endif
