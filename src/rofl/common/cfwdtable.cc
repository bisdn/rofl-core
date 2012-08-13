/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cfwdtable.h"

cfwdtable::cfwdtable()
{
	WRITELOG(FWDTABLE, DBG, "cfwdtable::cfwdtable() ");
}


cfwdtable::~cfwdtable()
{
	std::map<uint16_t, std::set<cfwdentry*> >::iterator it;
	for (it = fwdtable.begin(); it != fwdtable.end(); ++it)
	{
		std::set<cfwdentry*>::iterator itf;
		while ((itf = it->second.begin()) != it->second.end())
		{
			delete (*itf);
		}
	}
	fwdtable.clear();
	WRITELOG(FWDTABLE, DBG, "cfwdtable::~cfwdtable() ");
}


/* checks in Ethernet frame src MAC address and VLAN and updates
 * table accordingly
 */
void
cfwdtable::mac_learning(
		fetherframe& ether,
		uint64_t dpid,
		uint32_t port_no) // ingress port
{
	uint16_t vid = 0xffff;
	cmacaddr dl_src = ether.get_dl_src();

	// multicast address as source??? => ignore
	if (dl_src.is_multicast())
		return;

	// get vlan tag control information (TCI)
	if ((fvlanframe::VLAN_ETHER == be16toh(ether.eth_hdr->dl_type)) ||
			(fvlanframe::QINQ_ETHER == be16toh(ether.eth_hdr->dl_type)))
	{
		fvlanframe vlan(ether.payload(), ether.payloadlen(), ether.payloadlen());
		vid = vlan.get_dl_vlan_id();
	}


	std::set<cfwdentry*>::iterator it;
	if ((it = find_if(fwdtable[vid].begin(), fwdtable[vid].end(),
				cfwdentry::cfwdentry_search(dl_src, vid))) == fwdtable[vid].end())
	{
#ifdef NDEBUG
		new cfwdentry(
				&fwdtable[vid],
				dl_src,
				dpid,
				port_no,
				vid);
#else
		cfwdentry* fwdentry = new cfwdentry(
				&fwdtable[vid],
				dl_src,
				dpid,
				port_no,
				vid);

		WRITELOG(UNKNOWN, DBG, "cfwdtable(%p)::mac_learning() "
						"new cfwdentry: %s", this, fwdentry->c_str());
#endif
	}
	else
	{
		// update timer for entry and adjust port_no if necessary
		(*it)->refresh_entry(dpid, port_no);
	}
}


bool
cfwdtable::mac_is_known(
		fetherframe& ether)
{
	//struct eth_hdr_t *eth_hdr = (struct eth_hdr_t*)data;
	uint16_t vid = 0xffff;

	// multicast destinations are "NotFound"
	if (ether.get_dl_dst().is_multicast())
	{
		return false;
	}

	// get vlan tag control information (TCI)
	if ((fvlanframe::VLAN_ETHER == ether.get_dl_type()) ||
			(fvlanframe::QINQ_ETHER == ether.get_dl_type()))
	{
		fvlanframe vlan(ether.payload(), ether.payloadlen(), ether.payloadlen());
		vid = vlan.get_dl_vlan_id();
	}

	// check for existing vlan identifier
	if (fwdtable.find(vid) == fwdtable.end())
	{
		return false;
	}

	// check for existing entry
	std::set<cfwdentry*>::iterator it;
	if ((it = find_if(fwdtable[vid].begin(), fwdtable[vid].end(),
						cfwdentry::cfwdentry_search(ether.get_dl_dst(), vid))) == fwdtable[vid].end())
	{
		return false;
	}

	return true;
}


uint32_t
cfwdtable::mac_next_hop(
		fetherframe& ether)
throw(eFwdTableNotFound)
{
	//struct eth_hdr_t *eth_hdr = (struct eth_hdr_t*)data;
	uint16_t vid = 0xffff;

	// broadcast address is OFPP_FLOOD
	if (ether.get_dl_dst().is_broadcast())
	{
		return OFPP_FLOOD;
	}

	// multicast destinations are "NotFound"
	if (ether.get_dl_dst().is_multicast())
	{
		throw eFwdTableNotFound();
	}

	// get vlan tag control information (TCI)
	if ((fvlanframe::VLAN_ETHER == be16toh(ether.eth_hdr->dl_type)) ||
			(fvlanframe::QINQ_ETHER == be16toh(ether.eth_hdr->dl_type)))
	{
		fvlanframe vlan(ether.payload(), ether.payloadlen(), ether.payloadlen());
		vid = vlan.get_dl_vlan_id();
	}

	// check for existing vlan identifier
	if (fwdtable.find(vid) == fwdtable.end())
	{
		throw eFwdTableNotFound();
	}

	// check for existing entry
	std::set<cfwdentry*>::iterator it;
	if ((it = find_if(fwdtable[vid].begin(), fwdtable[vid].end(),
						cfwdentry::cfwdentry_search(ether.get_dl_dst(), vid))) == fwdtable[vid].end())
	{
		throw eFwdTableNotFound();
	}

	return (*it)->port_no;
}


uint32_t
cfwdtable::next_hop_port_no(
		fetherframe& ether)
throw (eFwdTableNotFound)
{
	//struct eth_hdr_t *eth_hdr = (struct eth_hdr_t*)data;
	uint16_t vid = 0xffff;

	// broadcast address is OFPP_FLOOD
	if (ether.get_dl_dst().is_broadcast())
	{
		return OFPP_FLOOD;
	}

	// multicast destinations are "NotFound"
	if (ether.get_dl_dst().is_multicast())
	{
		throw eFwdTableNotFound();
	}

	// get vlan tag control information (TCI)
	if ((fvlanframe::VLAN_ETHER == be16toh(ether.eth_hdr->dl_type)) ||
			(fvlanframe::QINQ_ETHER == be16toh(ether.eth_hdr->dl_type)))
	{
		fvlanframe vlan(ether.payload(), ether.payloadlen(), ether.payloadlen());
		vid = vlan.get_dl_vlan_id();
	}

	// check for existing vlan identifier
	if (fwdtable.find(vid) == fwdtable.end())
	{
		throw eFwdTableNotFound();
	}

	// check for existing entry
	std::set<cfwdentry*>::iterator it;
	if ((it = find_if(fwdtable[vid].begin(), fwdtable[vid].end(),
						cfwdentry::cfwdentry_search(ether.get_dl_dst(), vid))) == fwdtable[vid].end())
	{
		throw eFwdTableNotFound();
	}

	return (*it)->port_no;
}


uint64_t
cfwdtable::next_hop_dpid(
		fetherframe& ether)
throw (eFwdTableNotFound)
{
	//struct eth_hdr_t *eth_hdr = (struct eth_hdr_t*)data;
	uint16_t vid = 0xffff;

	// multicast destinations are "NotFound"
	if (ether.get_dl_dst().is_multicast())
	{
		throw eFwdTableNotFound();
	}

	// get vlan tag control information (TCI)
	if ((fvlanframe::VLAN_ETHER == be16toh(ether.eth_hdr->dl_type)) ||
			(fvlanframe::QINQ_ETHER == be16toh(ether.eth_hdr->dl_type)))
	{
		fvlanframe vlan(ether.payload(), ether.payloadlen(), ether.payloadlen());
		vid = vlan.get_dl_vlan_id();
	}

	// check for existing vlan identifier
	if (fwdtable.find(vid) == fwdtable.end())
	{
		throw eFwdTableNotFound();
	}

	// check for existing entry
	std::set<cfwdentry*>::iterator it;
	if ((it = find_if(fwdtable[vid].begin(), fwdtable[vid].end(),
						cfwdentry::cfwdentry_search(ether.get_dl_dst(), vid))) == fwdtable[vid].end())
	{
		throw eFwdTableNotFound();
	}

	return (*it)->dpid;
}


const char*
cfwdtable::c_str()
{
	cvastring vas;
	info.assign(vas("cfwdtable(%p):\n", this));
	std::map<uint16_t, std::set<cfwdentry*> >::iterator it;
	for (it = fwdtable.begin(); it != fwdtable.end(); ++it)
	{
		info.append(vas("vid:%d => \n", it->first));
		std::set<cfwdentry*>::iterator jt;
		for (jt = it->second.begin(); jt != it->second.end(); ++jt)
		{
			info.append(vas("%s\n", (*jt)->c_str()));
		}
	}
	return info.c_str();
}


cfwdentry&
cfwdtable::get_egress_fwdentry(
		cmacaddr const& ma,
		uint16_t dl_vid)
throw (eFwdTableNotFound)
{
	if (fwdtable.find(dl_vid) == fwdtable.end())
	{
		throw eFwdTableNotFound();
	}

	std::set<cfwdentry*>::iterator it;
	if ((it = find_if(fwdtable[dl_vid].begin(), fwdtable[dl_vid].end(),
			cfwdentry::cfwdentry_search(ma, dl_vid))) == fwdtable[dl_vid].end())
	{
		throw eFwdTableNotFound();
	}
	return *(*it);
}


