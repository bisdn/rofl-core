/*
 * cfib.cc
 *
 *  Created on: 15.07.2013
 *      Author: andreas
 */

#include <cfib.h>

using namespace etherswitch;

std::map<uint64_t, cfib*> cfib::fibs;


/*static*/
cfib&
cfib::get_fib(uint64_t dpid)
{
	if (cfib::fibs.find(dpid) == cfib::fibs.end()) {
		new cfib(dpid);
	}
	return *(cfib::fibs[dpid]);
}


cfib::cfib(uint64_t dpid) :
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
		fibtable[entry->get_lladdr()]->flow_mod_delete();
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
	std::cerr << "UPDATE: src: " << src << std::endl;

	// update cfibentry for src/inport
	if (fibtable.find(src) == fibtable.end()) {
		fibtable[src] = new cfibentry(this, rofbase, &dpt, src, in_port);
		fibtable[src]->flow_mod_add();

		std::cerr << "UPDATE[2.1]: " << *this << std::endl;

	} else {
		fibtable[src]->set_out_port_no(in_port);

		std::cerr << "UPDATE[3.1]: " << *this << std::endl;

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
	std::cerr << "LOOKUP: dst: " << dst << " src: " << src << std::endl;

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
	}

	return *(fibtable[dst]);
}





