/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cfwdentry.h"

cfwdentry::cfwdentry(
		std::set<cfwdentry*> *__fwdtable,
		cmacaddr const& __maddr,
		uint64_t __dpid,
		uint32_t __port_no,
		uint16_t __vlan_id,
		int __timeout) :
	maddr(__maddr),
	dpid(__dpid),
	port_no(__port_no),
	vlan_id(__vlan_id),
	fwdtable(__fwdtable),
	fwdentry_timeout(__timeout)
{
	fwdtable->insert(this);

	register_timer(TIMER_CFWDENTRY_EXPIRED, fwdentry_timeout);

	WRITELOG(FTE, ROFL_DBG, "cfwdentry(%p)::cfwdentry() constructor %s", this, c_str());
}


cfwdentry::~cfwdentry()
{
	fwdtable->erase(this);
	WRITELOG(FTE, ROFL_DBG, "cfwdentry(%p)::~cfwdentry() destructor %s", this, c_str());
}


void
cfwdentry::handle_timeout(int opaque)
{
	switch (opaque) {
	case TIMER_CFWDENTRY_EXPIRED:
		WRITELOG(FTE, ROFL_DBG,
				"cfwdentry(%p)::handle_timeout() fwdentry expired %s", this, c_str());
		delete this;
		return;
	}

}


void
cfwdentry::refresh_entry(uint64_t dpid, uint32_t port_no)
{
	this->dpid = dpid;
	this->port_no = port_no;
	reset_timer(TIMER_CFWDENTRY_EXPIRED, fwdentry_timeout);
	WRITELOG(FTE, ROFL_DBG,
		"cfwdentry(%p)::refresh_entry() %s", this, c_str());
}


const char*
cfwdentry::c_str()
{
	cvastring vas;
	info.assign(vas("cfwdentry(%p): mac:%s dpid:0x%llx port:%d vid:%d expires:%u",
			this, maddr.c_str(), dpid, port_no, vlan_id, fwdentry_timeout));
	return info.c_str();
}







