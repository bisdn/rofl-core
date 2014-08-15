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
	for (std::map<rofl::caddress_ll, cfibentry*>::iterator
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

	for (std::map<rofl::caddress_ll, cfibentry*>::iterator
			it = fibtable.begin(); it != fibtable.end(); ++it) {
		delete (it->second);
	}
	fibtable.clear();
}



void
cfib::fib_timer_expired(cfibentry *entry)
{
	if (fibtable.find(entry->get_lladdr()) != fibtable.end()) {
		fibtable.erase(entry->get_lladdr());
		delete entry;
	}

	std::cerr << "EXPIRED: " << *this << std::endl;
}



void
cfib::fib_update(
		rofl::crofbase *rofbase,
		rofl::crofdpt& dpt,
		rofl::caddress_ll const& src,
		uint32_t in_port)
{
	// update cfibentry for src/inport
	if (fibtable.find(src) == fibtable.end()) {
		fibtable[src] = new cfibentry(this, rofbase, &dpt, src, in_port);
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
		rofl::caddress_ll const& dst,
		rofl::caddress_ll const& src,
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

		rofl::logging::debug << "[ethswctld][ethsw][cfib][lookup] NOT FOUND" << std::endl;

		throw eFibNotFound();
	}

	return *(fibtable[dst]);
}





