/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cofactions.h"

using namespace rofl;

cofactions::cofactions(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}


cofactions::cofactions(
		uint8_t ofp_version, uint8_t *achdr, size_t aclen) :
				ofp_version(ofp_version)
{
	unpack(achdr, aclen);
}



cofactions::cofactions(
		cofactions const& aclist)
{
	*this = aclist;
}



cofactions&
cofactions::operator= (
		cofactions const& actions)
{
	if (this == &actions)
		return *this;

	this->ofp_version = actions.ofp_version;

	clear();

	for (std::list<cofaction*>::const_iterator
			it = actions.begin(); it != actions.end(); ++it) {
		map_and_insert(*(*it));
	}

	return *this;
}



cofactions::~cofactions()
{
	clear();
}



void
cofactions::pop_front()
{
	if (empty())
		return;
	delete std::list<cofaction*>::front();
	std::list<cofaction*>::pop_front();
}



void
cofactions::pop_back()
{
	if (empty())
		return;
	delete std::list<cofaction*>::back();
	std::list<cofaction*>::pop_back();
}



cofaction&
cofactions::front()
{
	if (empty())
		throw eActionsOutOfRange();
	return *(std::list<cofaction*>::front());
}



cofaction&
cofactions::back()
{
	if (empty())
		throw eActionsOutOfRange();
	return *(std::list<cofaction*>::back());
}



void
cofactions::clear()
{
	for (cofactions::iterator it = begin(); it != end(); ++it) {
		delete (*it);
	}
	std::list<cofaction*>::clear();
}



void
cofactions::unpack(uint8_t* buf, size_t buflen)
{
	struct openflow::ofp_action_header *achdr = (struct openflow::ofp_action_header*)buf;

	clear();

	if (buflen < (int)sizeof(struct openflow::ofp_action_header))
		return;

	while (buflen > 0) {
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			if (be16toh(achdr->len) < sizeof(struct openflow10::ofp_action_header))
				throw eBadActionBadLen();
		} break;
		case openflow12::OFP_VERSION: {
			if (be16toh(achdr->len) < sizeof(struct openflow12::ofp_action_header))
				throw eBadActionBadLen();
		} break;
		case openflow13::OFP_VERSION: {
			if (be16toh(achdr->len) < sizeof(struct openflow13::ofp_action_header))
				throw eBadActionBadLen();
		} break;
		default:
			logging::warn << "[rofl][cofactions] method unpack() failed, bad ofp version" << std::endl;
			throw eBadVersion();
		}

		cofaction action(ofp_version, achdr, be16toh(achdr->len));

		map_and_insert(action);

		buflen -= be16toh(achdr->len);
		achdr = (struct openflow::ofp_action_header*)(((uint8_t*)achdr) + be16toh(achdr->len));
	}
}



uint8_t*
cofactions::pack(uint8_t* achdr, size_t aclen)
{
	if (aclen < length())
		throw eActionsInval();

	cofactions::iterator it;
	for (std::list<cofaction*>::iterator it = begin(); it != end(); ++it) {
		cofaction& action = *(*it);
		achdr = ((uint8_t*)(action.pack((uint8_t*)achdr, action.length())) + action.length());
	}
	return achdr;
}



size_t
cofactions::length() const
{
	size_t len = 0;
	for (std::list<cofaction*>::const_iterator it = begin(); it != end(); ++it) {
		cofaction const& action = *(*it);
		len += action.length();
	}
	return len;
}



void
cofactions::map_and_insert(cofaction const& action)
{
	switch (action.get_type()) {
	case openflow::OFPAT_OUTPUT:
		push_back(new cofaction_output(action)); break;
	case openflow::OFPAT_SET_VLAN_VID:
		push_back(new cofaction_set_vlan_vid(action)); break;
	case openflow::OFPAT_SET_VLAN_PCP:
		push_back(new cofaction_set_vlan_pcp(action)); break;
	case openflow::OFPAT_STRIP_VLAN:
		push_back(new cofaction_strip_vlan(action)); break;
	case openflow::OFPAT_SET_DL_SRC:
		push_back(new cofaction_set_dl_src(action)); break;
	case openflow::OFPAT_SET_DL_DST:
		push_back(new cofaction_set_dl_dst(action)); break;
	case openflow::OFPAT_SET_NW_SRC:
		push_back(new cofaction_set_nw_src(action)); break;
	case openflow::OFPAT_SET_NW_DST:
		push_back(new cofaction_set_nw_dst(action)); break;
	case openflow::OFPAT_SET_NW_TOS:
		push_back(new cofaction_set_nw_tos(action)); break;
	case openflow::OFPAT_SET_TP_SRC:
		push_back(new cofaction_set_tp_src(action)); break;
	case openflow::OFPAT_SET_TP_DST:
		push_back(new cofaction_set_tp_dst(action)); break;
	case openflow::OFPAT_COPY_TTL_OUT: // = openflow10::OFPAT_ENQUEUE
		switch (ofp_version) {
		case openflow10::OFP_VERSION:
			push_back(new cofaction_enqueue(action)); break;
		default:
			push_back(new cofaction_copy_ttl_out(action)); break;
		} break;
	case openflow::OFPAT_COPY_TTL_IN:
		push_back(new cofaction_copy_ttl_in(action)); break;
	case openflow::OFPAT_SET_MPLS_TTL:
		push_back(new cofaction_set_mpls_ttl(action)); break;
	case openflow::OFPAT_DEC_MPLS_TTL:
		push_back(new cofaction_dec_mpls_ttl(action)); break;
	case openflow::OFPAT_PUSH_VLAN:
		push_back(new cofaction_push_vlan(action)); break;
	case openflow::OFPAT_POP_VLAN:
		push_back(new cofaction_pop_vlan(action)); break;
	case openflow::OFPAT_PUSH_MPLS:
		push_back(new cofaction_push_mpls(action)); break;
	case openflow::OFPAT_POP_MPLS:
		push_back(new cofaction_pop_mpls(action)); break;
	case openflow::OFPAT_SET_QUEUE:
		push_back(new cofaction_set_queue(action)); break;
	case openflow::OFPAT_GROUP:
		push_back(new cofaction_group(action)); break;
	case openflow::OFPAT_SET_NW_TTL:
		push_back(new cofaction_set_nw_ttl(action)); break;
	case openflow::OFPAT_DEC_NW_TTL:
		push_back(new cofaction_dec_nw_ttl(action)); break;
	case openflow::OFPAT_SET_FIELD:
		push_back(new cofaction_set_field(action)); break;
	case openflow::OFPAT_EXPERIMENTER:
		switch (ofp_version) {
		case openflow10::OFP_VERSION:
			push_back(new cofaction_vendor(action)); break;
		default:
			push_back(new cofaction_experimenter(action)); break;
		} break;
	default:
		push_back(new cofaction(action)); break;
	}
}




int
cofactions::count_action_type(
		uint16_t type)
{
	return count_if(begin(), end(), cofaction_find_type(type));
}


int
cofactions::count_action_output(
		uint32_t port_no) const
{
	int action_cnt = 0;

	for (cofactions::const_iterator it = begin(); it != end(); ++it) {
		cofaction action(*(*it));
		uint32_t out_port = 0;

		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			if (openflow10::OFPAT_OUTPUT != action.get_type()) {
				continue;
			}
			out_port = be16toh(action.oac_10output->port);
		} break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			if (openflow12::OFPAT_OUTPUT != action.get_type()) {
				continue;
			}
			out_port = be32toh(action.oac_12output->port);
		} break;
		default:
			throw eBadVersion();
		}
		if ((0 == port_no) || (out_port == port_no)) {
			action_cnt++;
		}
	}
	return action_cnt;
}


std::list<uint32_t>
cofactions::actions_output_ports()
{
	std::list<uint32_t> outports;

	for (cofactions::iterator it = begin(); it != end(); ++it) {
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			if ((*it)->get_type() != openflow10::OFPAT_OUTPUT) {
				continue;
			}
			outports.push_back(be16toh((*it)->oac_10output->port));
		} break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			if ((*it)->get_type() != openflow12::OFPAT_OUTPUT) {
				continue;
			}
			outports.push_back(be32toh((*it)->oac_12output->port));
		} break;
		default:
			throw eBadVersion();
		}
	}
	return outports;
}



cofaction&
cofactions::append_action(cofaction const action)
{
	cofaction *n_action = new cofaction(action);

	push_back(n_action);

	return *n_action;
}



cofaction_output&
cofactions::append_action_output(uint32_t port_no)
{
	if (openflow10::OFP_VERSION == ofp_version) {
		if ((port_no & 0x0000ffff) != port_no)
			throw eActionsInval();
	}

	cofaction_output* output = new cofaction_output(ofp_version, port_no);

	push_back(output);

	return *output;
}



cofaction_set_vlan_vid&
cofactions::append_action_set_vlan_vid(uint16_t vid)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_set_vlan_vid* set_vlan_vid = new cofaction_set_vlan_vid(ofp_version, vid);

	push_back(set_vlan_vid);

	return *set_vlan_vid;
}



cofaction_set_vlan_pcp&
cofactions::append_action_set_vlan_pcp(uint8_t pcp)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_set_vlan_pcp* set_vlan_pcp = new cofaction_set_vlan_pcp(ofp_version, pcp);

	push_back(set_vlan_pcp);

	return *set_vlan_pcp;
}



cofaction_strip_vlan&
cofactions::append_action_strip_vlan()
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_strip_vlan* strip_vlan = new cofaction_strip_vlan(ofp_version);

	push_back(strip_vlan);

	return *strip_vlan;
}



cofaction_set_dl_src&
cofactions::append_action_set_dl_src(cmacaddr const& maddr)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_set_dl_src* set_dl_src = new cofaction_set_dl_src(ofp_version, maddr);

	push_back(set_dl_src);

	return *set_dl_src;
}



cofaction_set_dl_dst&
cofactions::append_action_set_dl_dst(cmacaddr const& maddr)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_set_dl_dst* set_dl_dst = new cofaction_set_dl_dst(ofp_version, maddr);

	push_back(set_dl_dst);

	return *set_dl_dst;
}



cofaction_set_nw_src&
cofactions::append_action_set_nw_src(caddress const& addr)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_set_nw_src* set_nw_src = new cofaction_set_nw_src(ofp_version, addr);

	push_back(set_nw_src);

	return *set_nw_src;
}



cofaction_set_nw_dst&
cofactions::append_action_set_nw_dst(caddress const& addr)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_set_nw_dst* set_nw_dst = new cofaction_set_nw_dst(ofp_version, addr);

	push_back(set_nw_dst);

	return *set_nw_dst;
}



cofaction_set_nw_tos&
cofactions::append_action_set_nw_tos(uint8_t tos)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_set_nw_tos* set_nw_tos = new cofaction_set_nw_tos(ofp_version, tos);

	push_back(set_nw_tos);

	return *set_nw_tos;
}



cofaction_set_tp_src&
cofactions::append_action_set_tp_src(uint16_t tp_src)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_set_tp_src* set_tp_src = new cofaction_set_tp_src(ofp_version, tp_src);

	push_back(set_tp_src);

	return *set_tp_src;
}



cofaction_set_tp_dst&
cofactions::append_action_set_tp_dst(uint16_t tp_dst)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_set_tp_dst* set_tp_dst = new cofaction_set_tp_dst(ofp_version, tp_dst);

	push_back(set_tp_dst);

	return *set_tp_dst;
}



cofaction_enqueue&
cofactions::append_action_enqueue(uint16_t port_no, uint32_t queue_id)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_enqueue* enqueue = new cofaction_enqueue(ofp_version, port_no, queue_id);

	push_back(enqueue);

	return *enqueue;
}



cofaction_copy_ttl_out&
cofactions::append_action_copy_ttl_out()
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_copy_ttl_out* copy_ttl_out = new cofaction_copy_ttl_out(ofp_version);

	push_back(copy_ttl_out);

	return *copy_ttl_out;
}



cofaction_copy_ttl_in&
cofactions::append_action_copy_ttl_in()
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_copy_ttl_in* copy_ttl_in = new cofaction_copy_ttl_in(ofp_version);

	push_back(copy_ttl_in);

	return *copy_ttl_in;
}



cofaction_set_mpls_ttl&
cofactions::append_action_set_mpls_ttl(uint8_t ttl)
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_set_mpls_ttl* set_mpls_ttl = new cofaction_set_mpls_ttl(ofp_version, ttl);

	push_back(set_mpls_ttl);

	return *set_mpls_ttl;
}



cofaction_dec_mpls_ttl&
cofactions::append_action_dec_mpls_ttl()
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_dec_mpls_ttl* dec_mpls_ttl = new cofaction_dec_mpls_ttl(ofp_version);

	push_back(dec_mpls_ttl);

	return *dec_mpls_ttl;
}



cofaction_push_vlan&
cofactions::append_action_push_vlan(uint16_t eth_type)
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_push_vlan* push_vlan = new cofaction_push_vlan(ofp_version, eth_type);

	push_back(push_vlan);

	return *push_vlan;
}



cofaction_pop_vlan&
cofactions::append_action_pop_vlan()
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_pop_vlan* pop_vlan = new cofaction_pop_vlan(ofp_version);

	push_back(pop_vlan);

	return *pop_vlan;
}



cofaction_push_mpls&
cofactions::append_action_push_mpls(uint16_t eth_type)
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_push_mpls* push_mpls = new cofaction_push_mpls(ofp_version, eth_type);

	push_back(push_mpls);

	return *push_mpls;
}



cofaction_pop_mpls&
cofactions::append_action_pop_mpls(uint16_t eth_type)
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_pop_mpls* pop_mpls = new cofaction_pop_mpls(ofp_version, eth_type);

	push_back(pop_mpls);

	return *pop_mpls;
}



cofaction_set_queue&
cofactions::append_action_set_queue(uint32_t queue_id)
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_set_queue* set_queue = new cofaction_set_queue(ofp_version, queue_id);

	push_back(set_queue);

	return *set_queue;
}



cofaction_group&
cofactions::append_action_group(uint32_t group_id)
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_group* group = new cofaction_group(ofp_version, group_id);

	push_back(group);

	return *group;
}



cofaction_set_nw_ttl&
cofactions::append_action_set_nw_ttl(uint8_t ttl)
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_set_nw_ttl* set_nw_ttl = new cofaction_set_nw_ttl(ofp_version, ttl);

	push_back(set_nw_ttl);

	return *set_nw_ttl;
}



cofaction_dec_nw_ttl&
cofactions::append_action_dec_nw_ttl()
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_dec_nw_ttl* dec_nw_ttl = new cofaction_dec_nw_ttl(ofp_version);

	push_back(dec_nw_ttl);

	return *dec_nw_ttl;
}



cofaction_set_field&
cofactions::append_action_set_field(coxmatch const& oxm)
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_set_field* set_field = new cofaction_set_field(ofp_version, oxm);

	push_back(set_field);

	return *set_field;
}



cofaction_experimenter&
cofactions::append_action_experimenter(cofaction const& action)
{
	// OpenFlow 1.2 and beyond
	if (openflow12::OFP_VERSION > ofp_version)
		throw eBadVersion();

	cofaction_experimenter* set_experimenter = new cofaction_experimenter(action);

	push_back(set_experimenter);

	return *set_experimenter;
}



cofaction_vendor&
cofactions::append_action_vendor(cofaction const& action)
{
	// OpenFlow 1.0 only
	if (openflow10::OFP_VERSION != ofp_version)
		throw eBadVersion();

	cofaction_vendor* set_vendor = new cofaction_vendor(action);

	push_back(set_vendor);

	return *set_vendor;
}






