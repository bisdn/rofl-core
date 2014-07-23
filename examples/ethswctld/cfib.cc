/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cfib.cc
 *
 *  Created on: 15.07.2013
 *      Author: andreas
 */

#include <cfib.h>

using namespace etherswitch;

std::map<rofl::cdptid, cfib*> cfib::fibs;


/*static*/
cfib&
cfib::get_fib(const rofl::cdptid& dpid)
{
	if (cfib::fibs.find(dpid) == cfib::fibs.end()) {
		new cfib(dpid);
	}
	return *(cfib::fibs[dpid]);
}


cfib::cfib(const rofl::cdptid& dpid) :
		dpid(dpid),
		rofbase(0),
		dpt(0)
{
	if (cfib::fibs.find(dpid) != cfib::fibs.end()) {
		throw eFibExists();
	}
	cfib::fibs[dpid] = this;
}



cfib::~cfib()
{
	cfib::fibs.erase(dpid);
}


void
cfib::clear()
{
	for (std::map<rofl::cmacaddr, cfibentry*>::iterator
			it = fibtable.begin(); it != fibtable.end(); ++it) {
		delete it->second;
	}
	fibtable.clear();
}


void
cfib::dpt_bind(rofl::crofbase *rofbase, rofl::crofdpt *dpt)
{
	if (((0 != this->rofbase) && (rofbase != this->rofbase)) ||
			((0 != this->dpt) && (dpt != this->dpt))) {
		throw eFibBusy();
	}
	this->rofbase 	= rofbase;
	this->dpt 		= dpt;
}



void
cfib::dpt_release(rofl::crofbase *rofbase, rofl::crofdpt *dpt)
{
	if (((0 != this->rofbase) && (rofbase != this->rofbase)) ||
			((0 != this->dpt) && (dpt != this->dpt))) {
		throw eFibInval();
	}
	this->rofbase	= 0;
	this->dpt		= 0;

	for (std::map<rofl::cmacaddr, cfibentry*>::iterator
			it = fibtable.begin(); it != fibtable.end(); ++it) {
		delete (it->second);
	}
	fibtable.clear();
}



void
cfib::fib_timer_expired(cfibentry *entry)
{
	if (fibtable.find(entry->get_lladdr()) != fibtable.end()) {
#if 0
		entry->set_out_port_no(OFPP12_FLOOD);
#else
#if 0
		fibtable[entry->get_lladdr()]->flow_mod_delete();
#endif
		fibtable.erase(entry->get_lladdr());
		delete entry;
#endif
	}

	std::cerr << "EXPIRED: " << *this << std::endl;
}



void
cfib::fib_update(
		rofl::crofbase *rofbase,
		rofl::crofdpt& dpt,
		rofl::cmacaddr const& src,
		uint32_t in_port)
{
	// update cfibentry for src/inport
	if (fibtable.find(src) == fibtable.end()) {
		fibtable[src] = new cfibentry(this, rofbase, &dpt, src, in_port);
#if 0
		fibtable[src]->flow_mod_add();
#endif

		std::cerr << "UPDATE[NEW-ENTRY]: src:" << src << " " << *this << std::endl;

	} else {
		fibtable[src]->set_out_port_no(in_port);

		std::cerr << "UPDATE[UPDATE-ENTRY]: src:" << src << " " << *this << std::endl;

	}
}



cfibentry&
cfib::fib_lookup(
		rofl::crofbase *rofbase,
		rofl::crofdpt& dpt,
		rofl::cmacaddr const& dst,
		rofl::cmacaddr const& src,
		uint32_t in_port)
{
	//std::cerr << "LOOKUP: dst: " << dst << " src: " << src << std::endl;

	// sanity checks
	if (src.is_multicast()) {
		throw eFibInval();
	}

	fib_update(rofbase, dpt, src, in_port);

	if (dst.is_multicast()) {
		throw eFibInval();
	}

	// find out-port for dst
	if (fibtable.find(dst) == fibtable.end()) {

		//rofl::logging::debug << "[ethswctld][ethsw][cfib][lookup] NOT FOUND" << std::endl;

		throw eFibNotFound();

#if 0
		switch (dpt.get_version()) {
		case rofl::openflow10::OFP_VERSION:
			fibtable[dst] = new cfibentry(this, rofbase, &dpt, dst, rofl::openflow10::OFPP_FLOOD); break;
		case rofl::openflow12::OFP_VERSION:
			fibtable[dst] = new cfibentry(this, rofbase, &dpt, dst, rofl::openflow12::OFPP_FLOOD); break;
		case rofl::openflow13::OFP_VERSION:
			fibtable[dst] = new cfibentry(this, rofbase, &dpt, dst, rofl::openflow13::OFPP_FLOOD); break;
		default:
			throw rofl::eBadVersion();
		}

		fibtable[dst]->flow_mod_add();

		std::cerr << "LOOKUP[1]: " << *this << std::endl;
#endif
	}

	return *(fibtable[dst]);
}





