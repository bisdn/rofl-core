/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cofactions.h"

using namespace rofl::openflow;

cofactions::cofactions(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}



cofactions::~cofactions()
{
	clear();
}



cofactions::cofactions(
		const cofactions& actions)
{
	*this = actions;
}



cofactions&
cofactions::operator= (
		const cofactions& actions)
{
	if (this == &actions)
		return *this;

	ofp_version = actions.ofp_version;

	clear();

	for (std::map<cindex, unsigned int>::const_iterator
			it = actions.actions_index.begin(); it != actions.actions_index.end(); ++it) {

		const cindex& index 		= it->first;
		const unsigned int& type 	= it->second;

		switch (type) {
		case rofl::openflow::OFPAT_OUTPUT: {
			add_action_output(index) = actions.get_action_output(index);
		} break;
		case rofl::openflow::OFPAT_SET_VLAN_VID: {
			add_action_set_vlan_vid(index) = actions.get_action_set_vlan_vid(index);
		} break;
		case rofl::openflow::OFPAT_SET_VLAN_PCP: {
			add_action_set_vlan_pcp(index) = actions.get_action_set_vlan_pcp(index);
		} break;
		case rofl::openflow::OFPAT_STRIP_VLAN: {
			add_action_strip_vlan(index) = actions.get_action_strip_vlan(index);
		} break;
		case rofl::openflow::OFPAT_SET_DL_SRC: {
			add_action_set_dl_src(index) = actions.get_action_set_dl_src(index);
		} break;
		case rofl::openflow::OFPAT_SET_DL_DST: {
			add_action_set_dl_dst(index) = actions.get_action_set_dl_dst(index);
		} break;
		case rofl::openflow::OFPAT_SET_NW_SRC: {
			add_action_set_nw_src(index) = actions.get_action_set_nw_src(index);
		} break;
		case rofl::openflow::OFPAT_SET_NW_DST: {
			add_action_set_nw_dst(index) = actions.get_action_set_nw_dst(index);
		} break;
		case rofl::openflow::OFPAT_SET_NW_TOS: {
			add_action_set_nw_tos(index) = actions.get_action_set_nw_tos(index);
		} break;
		case rofl::openflow::OFPAT_SET_TP_SRC: {
			add_action_set_tp_src(index) = actions.get_action_set_tp_src(index);
		} break;
		case rofl::openflow::OFPAT_SET_TP_DST: {
			add_action_set_tp_dst(index) = actions.get_action_set_tp_dst(index);
		} break;
		case rofl::openflow::OFPAT_COPY_TTL_OUT: {
			switch (get_version()) {
			case rofl::openflow10::OFP_VERSION: {
				add_action_enqueue(index) = actions.get_action_enqueue(index);
			} break;
			default: {
				add_action_copy_ttl_out(index) = actions.get_action_copy_ttl_out(index);
			}
			}
		} break;
		case rofl::openflow::OFPAT_COPY_TTL_IN: {
			add_action_copy_ttl_in(index) = actions.get_action_copy_ttl_in(index);
		} break;
		case rofl::openflow::OFPAT_SET_MPLS_TTL: {
			add_action_set_mpls_ttl(index) = actions.get_action_set_mpls_ttl(index);
		} break;
		case rofl::openflow::OFPAT_DEC_MPLS_TTL: {
			add_action_dec_mpls_ttl(index) = actions.get_action_dec_mpls_ttl(index);
		} break;
		case rofl::openflow::OFPAT_PUSH_VLAN: {
			add_action_push_vlan(index) = actions.get_action_push_vlan(index);
		} break;
		case rofl::openflow::OFPAT_POP_VLAN: {
			add_action_pop_vlan(index) = actions.get_action_pop_vlan(index);
		} break;
		case rofl::openflow::OFPAT_PUSH_MPLS: {
			add_action_push_mpls(index) = actions.get_action_push_mpls(index);
		} break;
		case rofl::openflow::OFPAT_POP_MPLS: {
			add_action_pop_mpls(index) = actions.get_action_pop_mpls(index);
		} break;
		case rofl::openflow::OFPAT_SET_QUEUE: {
			add_action_set_queue(index) = actions.get_action_set_queue(index);
		} break;
		case rofl::openflow::OFPAT_GROUP: {
			add_action_group(index) = actions.get_action_group(index);
		} break;
		case rofl::openflow::OFPAT_SET_NW_TTL: {
			add_action_set_nw_ttl(index) = actions.get_action_set_nw_ttl(index);
		} break;
		case rofl::openflow::OFPAT_DEC_NW_TTL: {
			add_action_dec_nw_ttl(index) = actions.get_action_dec_nw_ttl(index);
		} break;
		case rofl::openflow::OFPAT_SET_FIELD: {
			add_action_set_field(index) = actions.get_action_set_field(index);
		} break;
		case rofl::openflow::OFPAT_PUSH_PBB: {
			add_action_push_pbb(index) = actions.get_action_push_pbb(index);
		} break;
		case rofl::openflow::OFPAT_POP_PBB: {
			add_action_pop_pbb(index) = actions.get_action_pop_pbb(index);
		} break;
		case rofl::openflow::OFPAT_EXPERIMENTER: {
			switch (get_version()) {
			case rofl::openflow10::OFP_VERSION: {
				add_action_vendor(index) = actions.get_action_vendor(index);
			} break;
			default: {
				add_action_experimenter(index) = actions.get_action_experimenter(index);
			}
			}
		} break;
		default: {
			LOGGING_WARN << "[rofl][cofactions][unpack] unknown action type:" << (unsigned int)type << std::endl;
		}
		}
	}

	return *this;
}



bool
cofactions::operator== (
		cofactions const& actions)
{
#if 0
	if (ofp_version != actions.ofp_version) {
		//FIXME: std::cerr << "PUNKT 1" << std::endl;
		return false;
	}

	if (std::list<cofaction*>::size() != actions.size()) {
		// FIXME: std::cerr << "PUNKT 2" << std::endl;
		return false;
	}

	for (std::list<cofaction*>::const_iterator
			it = actions.begin(), jt = this->begin(); it != actions.end(), jt != this->end(); ++it, ++jt) {
		if (not (**it == **jt)) {
			//FIXME: std::cerr << "PUNKT 3" << std::endl;
			//FIXME: std::cerr << "it:" << std::endl << **it;
			//FIXME: std::cerr << "jt:" << std::endl << **jt;
			return false;
		}
	}

	//FIXME: std::cerr << "PUNKT 4" << std::endl;
#endif
	return true;
}



void
cofactions::clear()
{
	for (std::map<cindex, cofaction*>::iterator
			it = actions.begin(); it != actions.end(); ++it) {
		delete (it->second);
	}
	actions.clear();
	actions_index.clear();
}



size_t
cofactions::length() const
{
	size_t len = 0;
	for (std::map<cindex, cofaction*>::const_iterator
			it = actions.begin(); it != actions.end(); ++it) {
		len += it->second->length();
	}
	return len;
}



void
cofactions::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval("cofactions::pack() buflen too short");

	for (std::map<cindex, cofaction*>::iterator
			it = actions.begin(); it != actions.end(); ++it) {
		cofaction& action = *(it->second);

		action.pack(buf, action.length());
		buf += action.length();
		buflen -= action.length();
	}
}



void
cofactions::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	cofactions::clear();

	if (buflen < sizeof(struct rofl::openflow::ofp_action_header))
		throw eInval("cofactions::unpack() buflen too short");

	cindex index;

	while (buflen >= sizeof(struct rofl::openflow::ofp_action_header)) {

		struct rofl::openflow::ofp_action_header* hdr =
				(struct rofl::openflow::ofp_action_header*)buf;

		uint16_t type = be16toh(hdr->type);
		uint16_t len  = be16toh(hdr->len);

		if (len > buflen)
			throw eBadActionBadLen("cofactions::unpack() invalid length field in action");

		switch (type) {
		case rofl::openflow::OFPAT_OUTPUT: {
			add_action_output(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_VLAN_VID: {
			add_action_set_vlan_vid(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_VLAN_PCP: {
			add_action_set_vlan_pcp(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_STRIP_VLAN: {
			add_action_strip_vlan(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_DL_SRC: {
			add_action_set_dl_src(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_DL_DST: {
			add_action_set_dl_dst(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_NW_SRC: {
			add_action_set_nw_src(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_NW_DST: {
			add_action_set_nw_dst(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_NW_TOS: {
			add_action_set_nw_tos(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_TP_SRC: {
			add_action_set_tp_src(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_TP_DST: {
			add_action_set_tp_dst(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_COPY_TTL_OUT: {
			switch (get_version()) {
			case rofl::openflow10::OFP_VERSION: {
				add_action_enqueue(index++).unpack(buf, len);
			} break;
			default: {
				add_action_copy_ttl_out(index++).unpack(buf, len);			}
			}
		} break;
		case rofl::openflow::OFPAT_COPY_TTL_IN: {
			add_action_copy_ttl_in(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_MPLS_TTL: {
			add_action_set_mpls_ttl(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_DEC_MPLS_TTL: {
			add_action_dec_mpls_ttl(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_PUSH_VLAN: {
			add_action_push_vlan(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_POP_VLAN: {
			add_action_pop_vlan(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_PUSH_MPLS: {
			add_action_push_mpls(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_POP_MPLS: {
			add_action_pop_mpls(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_QUEUE: {
			add_action_set_queue(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_GROUP: {
			add_action_group(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_NW_TTL: {
			add_action_set_nw_ttl(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_DEC_NW_TTL: {
			add_action_dec_nw_ttl(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_SET_FIELD: {
			add_action_set_field(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_PUSH_PBB: {
			add_action_push_pbb(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_POP_PBB: {
			add_action_pop_pbb(index++).unpack(buf, len);
		} break;
		case rofl::openflow::OFPAT_EXPERIMENTER: {
			switch (get_version()) {
			case rofl::openflow10::OFP_VERSION: {
				add_action_vendor(index++).unpack(buf, len);
			} break;
			default: {
				add_action_experimenter(index++).unpack(buf, len);
			}
			}
		} break;
		default: {
			LOGGING_WARN << "[rofl][cofactions][unpack] unknown action type:" << (unsigned int)type << std::endl;
		}
		}

		buf += len;
		buflen -= len;
	}
}



int
cofactions::count_action_type(
		uint16_t type) const
{
	return count_if(actions.begin(), actions.end(), cofaction::cofaction_find_by_type(type));
}



int
cofactions::count_action_output(
		uint32_t port_no) const
{
	int action_cnt = 0;
	for (std::map<cindex, cofaction*>::const_iterator
			it = actions.begin(); it != actions.end(); ++it) {
		const cofaction& action = *(it->second);

		if (rofl::openflow::OFPAT_OUTPUT != action.get_type())
			continue;

		try {
			const cofaction_output& aoutput = dynamic_cast<const cofaction_output&>( action );
			if ((0 == port_no) || (aoutput.get_port_no() == port_no)) {
				action_cnt++;
			}
		} catch (...) {}
	}
	return action_cnt;
}



std::list<uint32_t>
cofactions::actions_output_ports() const
{
	std::list<uint32_t> outports;

	for (std::map<cindex, cofaction*>::const_iterator
			it = actions.begin(); it != actions.end(); ++it) {
		const cofaction& action = *(it->second);

		if (rofl::openflow::OFPAT_OUTPUT != action.get_type())
			continue;

		try {
			const cofaction_output& aoutput = dynamic_cast<const cofaction_output&>( action );
			outports.push_back(aoutput.get_port_no());
		} catch (...) {}
	}
	return outports;
}



void
cofactions::check_prerequisites() const
{
	for (std::map<cindex, cofaction*>::const_iterator
			it = actions.begin(); it != actions.end(); ++it) {
		it->second->check_prerequisites();
	}
}



void
cofactions::drop_action(const cindex& index)
{
	if (actions_index.find(index) == actions_index.end()) {
		return;
	}
	delete actions[index];
	actions.erase(index);
	actions_index.erase(index);
}



bool
cofactions::has_action(const cindex& index) const
{
	return (not (actions_index.find(index) == actions_index.end()));
}



cofaction_output&
cofactions::add_action_output(const cindex& index)
{
	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_output(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_OUTPUT;
	return dynamic_cast<cofaction_output&>(*(actions[index]));
}



cofaction_output&
cofactions::set_action_output(const cindex& index)
{
	if ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_OUTPUT != actions[index]->get_type())) {
		throw eInval("cofactions::set_action_output() invalid action type");
	}
	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_output(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_OUTPUT;
	}
	return dynamic_cast<cofaction_output&>(*(actions[index]));
}



const cofaction_output&
cofactions::get_action_output(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_OUTPUT != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_output&>(*(actions.at(index)));
}



void
cofactions::drop_action_output(const cindex& index)
{
	if (rofl::openflow::OFPAT_OUTPUT != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_output(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_OUTPUT == actions_index.at(index)));
}



cofaction_set_vlan_vid&
cofactions::add_action_set_vlan_vid(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_vlan_vid() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_vlan_vid(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_VLAN_VID;
	return dynamic_cast<cofaction_set_vlan_vid&>( *(actions[index]) );
}



cofaction_set_vlan_vid&
cofactions::set_action_set_vlan_vid(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_vlan_vid() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_vlan_vid(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_VLAN_VID;
	}
	return dynamic_cast<cofaction_set_vlan_vid&>( *(actions[index]) );
}



const cofaction_set_vlan_vid&
cofactions::get_action_set_vlan_vid(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_VLAN_VID != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_vlan_vid&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_vlan_vid(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_VLAN_VID != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_vlan_vid(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_VLAN_VID == actions_index.at(index)));
}



cofaction_set_vlan_pcp&
cofactions::add_action_set_vlan_pcp(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_vlan_pcp() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_vlan_pcp(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_VLAN_PCP;
	return dynamic_cast<cofaction_set_vlan_pcp&>( *(actions[index]) );
}



cofaction_set_vlan_pcp&
cofactions::set_action_set_vlan_pcp(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_vlan_pcp() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_vlan_pcp(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_VLAN_PCP;
	}
	return dynamic_cast<cofaction_set_vlan_pcp&>( *(actions[index]) );
}



const cofaction_set_vlan_pcp&
cofactions::get_action_set_vlan_pcp(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_VLAN_PCP != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_vlan_pcp&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_vlan_pcp(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_VLAN_PCP != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_vlan_pcp(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_VLAN_PCP == actions_index.at(index)));
}



cofaction_strip_vlan&
cofactions::add_action_strip_vlan(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_strip_vlan() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_strip_vlan(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_STRIP_VLAN;
	return dynamic_cast<cofaction_strip_vlan&>( *(actions[index]) );
}



cofaction_strip_vlan&
cofactions::set_action_strip_vlan(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_strip_vlan() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_strip_vlan(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_STRIP_VLAN;
	}
	return dynamic_cast<cofaction_strip_vlan&>( *(actions[index]) );
}



const cofaction_strip_vlan&
cofactions::get_action_strip_vlan(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_STRIP_VLAN != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_strip_vlan&>( *(actions.at(index)) );
}



void
cofactions::drop_action_strip_vlan(const cindex& index)
{
	if (rofl::openflow::OFPAT_STRIP_VLAN != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_strip_vlan(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_STRIP_VLAN == actions_index.at(index)));
}



cofaction_set_dl_src&
cofactions::add_action_set_dl_src(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_dl_src() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_dl_src(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_DL_SRC;
	return dynamic_cast<cofaction_set_dl_src&>( *(actions[index]) );
}



cofaction_set_dl_src&
cofactions::set_action_set_dl_src(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_dl_src() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_dl_src(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_DL_SRC;
	}
	return dynamic_cast<cofaction_set_dl_src&>( *(actions[index]) );
}



const cofaction_set_dl_src&
cofactions::get_action_set_dl_src(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_DL_SRC != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_dl_src&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_dl_src(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_DL_SRC != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_dl_src(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_DL_SRC == actions_index.at(index)));
}



cofaction_set_dl_dst&
cofactions::add_action_set_dl_dst(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_dl_dst() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_dl_dst(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_DL_DST;
	return dynamic_cast<cofaction_set_dl_dst&>( *(actions[index]) );
}



cofaction_set_dl_dst&
cofactions::set_action_set_dl_dst(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_dl_dst() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_dl_dst(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_DL_DST;
	}
	return dynamic_cast<cofaction_set_dl_dst&>( *(actions[index]) );
}



const cofaction_set_dl_dst&
cofactions::get_action_set_dl_dst(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_DL_DST != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_dl_dst&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_dl_dst(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_DL_DST != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_dl_dst(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_DL_DST == actions_index.at(index)));
}



cofaction_set_nw_src&
cofactions::add_action_set_nw_src(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_nw_src() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_nw_src(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_NW_SRC;
	return dynamic_cast<cofaction_set_nw_src&>( *(actions[index]) );
}



cofaction_set_nw_src&
cofactions::set_action_set_nw_src(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_nw_src() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_nw_src(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_NW_SRC;
	}
	return dynamic_cast<cofaction_set_nw_src&>( *(actions[index]) );
}



const cofaction_set_nw_src&
cofactions::get_action_set_nw_src(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_NW_SRC != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_nw_src&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_nw_src(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_NW_SRC != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_nw_src(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_NW_SRC == actions_index.at(index)));
}



cofaction_set_nw_dst&
cofactions::add_action_set_nw_dst(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_nw_dst() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_nw_dst(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_NW_DST;
	return dynamic_cast<cofaction_set_nw_dst&>( *(actions[index]) );
}



cofaction_set_nw_dst&
cofactions::set_action_set_nw_dst(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_nw_dst() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_nw_dst(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_NW_DST;
	}
	return dynamic_cast<cofaction_set_nw_dst&>( *(actions[index]) );
}



const cofaction_set_nw_dst&
cofactions::get_action_set_nw_dst(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_NW_DST != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_nw_dst&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_nw_dst(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_NW_DST != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_nw_dst(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_NW_DST == actions_index.at(index)));
}



cofaction_set_nw_tos&
cofactions::add_action_set_nw_tos(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_nw_tos() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_nw_tos(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_NW_TOS;
	return dynamic_cast<cofaction_set_nw_tos&>( *(actions[index]) );
}



cofaction_set_nw_tos&
cofactions::set_action_set_nw_tos(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_nw_tos() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_nw_tos(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_NW_TOS;
	}
	return dynamic_cast<cofaction_set_nw_tos&>( *(actions[index]) );
}



const cofaction_set_nw_tos&
cofactions::get_action_set_nw_tos(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_NW_TOS != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_nw_tos&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_nw_tos(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_NW_TOS != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_nw_tos(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_NW_TOS == actions_index.at(index)));
}



cofaction_set_tp_src&
cofactions::add_action_set_tp_src(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_tp_src() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_tp_src(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_TP_SRC;
	return dynamic_cast<cofaction_set_tp_src&>( *(actions[index]) );
}



cofaction_set_tp_src&
cofactions::set_action_set_tp_src(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_tp_src() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_tp_src(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_TP_SRC;
	}
	return dynamic_cast<cofaction_set_tp_src&>( *(actions[index]) );
}



const cofaction_set_tp_src&
cofactions::get_action_set_tp_src(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_TP_SRC != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_tp_src&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_tp_src(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_TP_SRC != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_tp_src(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_TP_SRC == actions_index.at(index)));
}



cofaction_set_tp_dst&
cofactions::add_action_set_tp_dst(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_tp_dst() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_tp_dst(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_TP_DST;
	return dynamic_cast<cofaction_set_tp_dst&>( *(actions[index]) );
}



cofaction_set_tp_dst&
cofactions::set_action_set_tp_dst(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_tp_dst() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_tp_dst(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_TP_DST;
	}
	return dynamic_cast<cofaction_set_tp_dst&>( *(actions[index]) );
}



const cofaction_set_tp_dst&
cofactions::get_action_set_tp_dst(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_TP_DST != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_tp_dst&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_tp_dst(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_TP_DST != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_tp_dst(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_TP_DST == actions_index.at(index)));
}



cofaction_enqueue&
cofactions::add_action_enqueue(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_enqueue() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_enqueue(ofp_version);
	actions_index[index] = rofl::openflow10::OFPAT_ENQUEUE;
	return dynamic_cast<cofaction_enqueue&>( *(actions[index]) );
}



cofaction_enqueue&
cofactions::set_action_enqueue(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_enqueue() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_enqueue(ofp_version);
		actions_index[index] = rofl::openflow10::OFPAT_ENQUEUE;
	}
	return dynamic_cast<cofaction_enqueue&>( *(actions[index]) );
}



const cofaction_enqueue&
cofactions::get_action_enqueue(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow10::OFPAT_ENQUEUE != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_enqueue&>( *(actions.at(index)) );
}



void
cofactions::drop_action_enqueue(const cindex& index)
{
	if (rofl::openflow10::OFPAT_ENQUEUE != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_enqueue(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow10::OFPAT_ENQUEUE == actions_index.at(index)));
}



cofaction_vendor&
cofactions::add_action_vendor(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_vendor() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_vendor(ofp_version);
	actions_index[index] = rofl::openflow10::OFPAT_VENDOR;
	return dynamic_cast<cofaction_vendor&>( *(actions[index]) );
}



cofaction_vendor&
cofactions::set_action_vendor(const cindex& index)
{
	if (get_version() != rofl::openflow10::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_vendor() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_vendor(ofp_version);
		actions_index[index] = rofl::openflow10::OFPAT_VENDOR;
	}
	return dynamic_cast<cofaction_vendor&>( *(actions[index]) );
}



const cofaction_vendor&
cofactions::get_action_vendor(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow10::OFPAT_VENDOR != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_vendor&>( *(actions.at(index)) );
}



void
cofactions::drop_action_vendor(const cindex& index)
{
	if (rofl::openflow10::OFPAT_VENDOR != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_vendor(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow10::OFPAT_VENDOR == actions_index.at(index)));
}



cofaction_copy_ttl_out&
cofactions::add_action_copy_ttl_out(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_copy_ttl_out() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_copy_ttl_out(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_COPY_TTL_OUT;
	return dynamic_cast<cofaction_copy_ttl_out&>( *(actions[index]) );
}



cofaction_copy_ttl_out&
cofactions::set_action_copy_ttl_out(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_copy_ttl_out() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_copy_ttl_out(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_COPY_TTL_OUT;
	}
	return dynamic_cast<cofaction_copy_ttl_out&>( *(actions[index]) );
}



const cofaction_copy_ttl_out&
cofactions::get_action_copy_ttl_out(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_COPY_TTL_OUT != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_copy_ttl_out&>( *(actions.at(index)) );
}



void
cofactions::drop_action_copy_ttl_out(const cindex& index)
{
	if (rofl::openflow::OFPAT_COPY_TTL_OUT != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_copy_ttl_out(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_COPY_TTL_OUT == actions_index.at(index)));
}



cofaction_copy_ttl_in&
cofactions::add_action_copy_ttl_in(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_copy_ttl_in() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_copy_ttl_in(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_COPY_TTL_IN;
	return dynamic_cast<cofaction_copy_ttl_in&>( *(actions[index]) );
}



cofaction_copy_ttl_in&
cofactions::set_action_copy_ttl_in(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_copy_ttl_in() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_copy_ttl_in(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_COPY_TTL_IN;
	}
	return dynamic_cast<cofaction_copy_ttl_in&>( *(actions[index]) );
}



const cofaction_copy_ttl_in&
cofactions::get_action_copy_ttl_in(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_COPY_TTL_IN != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_copy_ttl_in&>( *(actions.at(index)) );
}



void
cofactions::drop_action_copy_ttl_in(const cindex& index)
{
	if (rofl::openflow::OFPAT_COPY_TTL_IN != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_copy_ttl_in(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_COPY_TTL_IN == actions_index.at(index)));
}



cofaction_set_mpls_ttl&
cofactions::add_action_set_mpls_ttl(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_mpls_ttl() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_mpls_ttl(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_MPLS_TTL;
	return dynamic_cast<cofaction_set_mpls_ttl&>( *(actions[index]) );
}



cofaction_set_mpls_ttl&
cofactions::set_action_set_mpls_ttl(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_mpls_ttl() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_mpls_ttl(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_MPLS_TTL;
	}
	return dynamic_cast<cofaction_set_mpls_ttl&>( *(actions[index]) );
}



const cofaction_set_mpls_ttl&
cofactions::get_action_set_mpls_ttl(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_MPLS_TTL != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_mpls_ttl&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_mpls_ttl(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_MPLS_TTL != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_mpls_ttl(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_MPLS_TTL == actions_index.at(index)));
}



cofaction_dec_mpls_ttl&
cofactions::add_action_dec_mpls_ttl(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_dec_mpls_ttl() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_dec_mpls_ttl(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_DEC_MPLS_TTL;
	return dynamic_cast<cofaction_dec_mpls_ttl&>( *(actions[index]) );
}



cofaction_dec_mpls_ttl&
cofactions::set_action_dec_mpls_ttl(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_dec_mpls_ttl() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_dec_mpls_ttl(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_DEC_MPLS_TTL;
	}
	return dynamic_cast<cofaction_dec_mpls_ttl&>( *(actions[index]) );
}



const cofaction_dec_mpls_ttl&
cofactions::get_action_dec_mpls_ttl(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_DEC_MPLS_TTL != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_dec_mpls_ttl&>( *(actions.at(index)) );
}



void
cofactions::drop_action_dec_mpls_ttl(const cindex& index)
{
	if (rofl::openflow::OFPAT_DEC_MPLS_TTL != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_dec_mpls_ttl(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_DEC_MPLS_TTL == actions_index.at(index)));
}



cofaction_push_vlan&
cofactions::add_action_push_vlan(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_push_vlan() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_push_vlan(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_PUSH_VLAN;
	return dynamic_cast<cofaction_push_vlan&>( *(actions[index]) );
}



cofaction_push_vlan&
cofactions::set_action_push_vlan(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_push_vlan() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_push_vlan(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_PUSH_VLAN;
	}
	return dynamic_cast<cofaction_push_vlan&>( *(actions[index]) );
}



const cofaction_push_vlan&
cofactions::get_action_push_vlan(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_PUSH_VLAN != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_push_vlan&>( *(actions.at(index)) );
}



void
cofactions::drop_action_push_vlan(const cindex& index)
{
	if (rofl::openflow::OFPAT_PUSH_VLAN != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_push_vlan(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_PUSH_VLAN == actions_index.at(index)));
}



cofaction_pop_vlan&
cofactions::add_action_pop_vlan(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_pop_vlan() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_pop_vlan(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_POP_VLAN;
	return dynamic_cast<cofaction_pop_vlan&>( *(actions[index]) );
}



cofaction_pop_vlan&
cofactions::set_action_pop_vlan(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_pop_vlan() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_pop_vlan(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_POP_VLAN;
	}
	return dynamic_cast<cofaction_pop_vlan&>( *(actions[index]) );
}



const cofaction_pop_vlan&
cofactions::get_action_pop_vlan(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_POP_VLAN != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_pop_vlan&>( *(actions.at(index)) );
}



void
cofactions::drop_action_pop_vlan(const cindex& index)
{
	if (rofl::openflow::OFPAT_POP_VLAN != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_pop_vlan(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_POP_VLAN == actions_index.at(index)));
}



cofaction_push_mpls&
cofactions::add_action_push_mpls(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_push_mpls() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_push_mpls(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_PUSH_MPLS;
	return dynamic_cast<cofaction_push_mpls&>( *(actions[index]) );
}



cofaction_push_mpls&
cofactions::set_action_push_mpls(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_push_mpls() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_push_mpls(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_PUSH_MPLS;
	}
	return dynamic_cast<cofaction_push_mpls&>( *(actions[index]) );
}



const cofaction_push_mpls&
cofactions::get_action_push_mpls(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_PUSH_MPLS != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_push_mpls&>( *(actions.at(index)) );
}



void
cofactions::drop_action_push_mpls(const cindex& index)
{
	if (rofl::openflow::OFPAT_PUSH_MPLS != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_push_mpls(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_PUSH_MPLS == actions_index.at(index)));
}



cofaction_pop_mpls&
cofactions::add_action_pop_mpls(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_pop_mpls() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_pop_mpls(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_POP_MPLS;
	return dynamic_cast<cofaction_pop_mpls&>( *(actions[index]) );
}



cofaction_pop_mpls&
cofactions::set_action_pop_mpls(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_pop_mpls() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_pop_mpls(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_POP_MPLS;
	}
	return dynamic_cast<cofaction_pop_mpls&>( *(actions[index]) );
}



const cofaction_pop_mpls&
cofactions::get_action_pop_mpls(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_POP_MPLS != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_pop_mpls&>( *(actions.at(index)) );
}



void
cofactions::drop_action_pop_mpls(const cindex& index)
{
	if (rofl::openflow::OFPAT_POP_MPLS != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_pop_mpls(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_POP_MPLS == actions_index.at(index)));
}



cofaction_group&
cofactions::add_action_group(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_group() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_group(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_GROUP;
	return dynamic_cast<cofaction_group&>( *(actions[index]) );
}



cofaction_group&
cofactions::set_action_group(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_group() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_group(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_GROUP;
	}
	return dynamic_cast<cofaction_group&>( *(actions[index]) );
}



const cofaction_group&
cofactions::get_action_group(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_GROUP != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_group&>( *(actions.at(index)) );
}



void
cofactions::drop_action_group(const cindex& index)
{
	if (rofl::openflow::OFPAT_GROUP != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_group(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_GROUP == actions_index.at(index)));
}



cofaction_set_nw_ttl&
cofactions::add_action_set_nw_ttl(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_nw_ttl() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_nw_ttl(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_NW_TTL;
	return dynamic_cast<cofaction_set_nw_ttl&>( *(actions[index]) );
}



cofaction_set_nw_ttl&
cofactions::set_action_set_nw_ttl(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_nw_ttl() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_nw_ttl(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_NW_TTL;
	}
	return dynamic_cast<cofaction_set_nw_ttl&>( *(actions[index]) );
}



const cofaction_set_nw_ttl&
cofactions::get_action_set_nw_ttl(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_NW_TTL != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_nw_ttl&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_nw_ttl(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_NW_TTL != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_nw_ttl(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_NW_TTL == actions_index.at(index)));
}



cofaction_dec_nw_ttl&
cofactions::add_action_dec_nw_ttl(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_dec_nw_ttl() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_dec_nw_ttl(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_DEC_NW_TTL;
	return dynamic_cast<cofaction_dec_nw_ttl&>( *(actions[index]) );
}



cofaction_dec_nw_ttl&
cofactions::set_action_dec_nw_ttl(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_dec_nw_ttl() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_dec_nw_ttl(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_DEC_NW_TTL;
	}
	return dynamic_cast<cofaction_dec_nw_ttl&>( *(actions[index]) );
}



const cofaction_dec_nw_ttl&
cofactions::get_action_dec_nw_ttl(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_DEC_NW_TTL != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_dec_nw_ttl&>( *(actions.at(index)) );
}



void
cofactions::drop_action_dec_nw_ttl(const cindex& index)
{
	if (rofl::openflow::OFPAT_DEC_NW_TTL != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_dec_nw_ttl(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_DEC_NW_TTL == actions_index.at(index)));
}



cofaction_set_queue&
cofactions::add_action_set_queue(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_queue() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_queue(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_QUEUE;
	return dynamic_cast<cofaction_set_queue&>( *(actions[index]) );
}



cofaction_set_queue&
cofactions::set_action_set_queue(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_queue() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_queue(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_QUEUE;
	}
	return dynamic_cast<cofaction_set_queue&>( *(actions[index]) );
}



const cofaction_set_queue&
cofactions::get_action_set_queue(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_QUEUE != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_queue&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_queue(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_QUEUE != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_queue(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_QUEUE == actions_index.at(index)));
}



cofaction_set_field&
cofactions::add_action_set_field(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_set_field() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_field(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_SET_FIELD;
	return dynamic_cast<cofaction_set_field&>( *(actions[index]) );
}



cofaction_set_field&
cofactions::set_action_set_field(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_set_field() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_set_field(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_SET_FIELD;
	}
	return dynamic_cast<cofaction_set_field&>( *(actions[index]) );
}



const cofaction_set_field&
cofactions::get_action_set_field(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_SET_FIELD != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_field&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_field(const cindex& index)
{
	if (rofl::openflow::OFPAT_SET_FIELD != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_field(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_SET_FIELD == actions_index.at(index)));
}



cofaction_experimenter&
cofactions::add_action_experimenter(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_experimenter() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_experimenter(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_EXPERIMENTER;
	return dynamic_cast<cofaction_experimenter&>( *(actions[index]) );
}



cofaction_experimenter&
cofactions::set_action_experimenter(const cindex& index)
{
	if (get_version() < rofl::openflow12::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_experimenter() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_experimenter(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_EXPERIMENTER;
	}
	return dynamic_cast<cofaction_experimenter&>( *(actions[index]) );
}



const cofaction_experimenter&
cofactions::get_action_experimenter(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_EXPERIMENTER != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_experimenter&>( *(actions.at(index)) );
}



void
cofactions::drop_action_experimenter(const cindex& index)
{
	if (rofl::openflow::OFPAT_EXPERIMENTER != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_experimenter(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_EXPERIMENTER == actions_index.at(index)));
}



cofaction_push_pbb&
cofactions::add_action_push_pbb(const cindex& index)
{
	if (get_version() < rofl::openflow13::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_push_pbb() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_push_pbb(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_PUSH_PBB;
	return dynamic_cast<cofaction_push_pbb&>( *(actions[index]) );
}



cofaction_push_pbb&
cofactions::set_action_push_pbb(const cindex& index)
{
	if (get_version() < rofl::openflow13::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_push_pbb() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_push_pbb(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_PUSH_PBB;
	}
	return dynamic_cast<cofaction_push_pbb&>( *(actions[index]) );
}



const cofaction_push_pbb&
cofactions::get_action_push_pbb(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_PUSH_PBB != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_push_pbb&>( *(actions.at(index)) );
}



void
cofactions::drop_action_push_pbb(const cindex& index)
{
	if (rofl::openflow::OFPAT_PUSH_PBB != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_push_pbb(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_PUSH_PBB == actions_index.at(index)));
}



cofaction_pop_pbb&
cofactions::add_action_pop_pbb(const cindex& index)
{
	if (get_version() < rofl::openflow13::OFP_VERSION)
		throw eBadVersion("cofactions::add_action_pop_pbb() invalid version");

	if (actions_index.find(index) != actions_index.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_pop_pbb(ofp_version);
	actions_index[index] = rofl::openflow::OFPAT_POP_PBB;
	return dynamic_cast<cofaction_pop_pbb&>( *(actions[index]) );
}



cofaction_pop_pbb&
cofactions::set_action_pop_pbb(const cindex& index)
{
	if (get_version() < rofl::openflow13::OFP_VERSION)
		throw eBadVersion("cofactions::set_action_pop_pbb() invalid version");

	if (actions_index.find(index) == actions_index.end()) {
		actions[index] = new cofaction_pop_pbb(ofp_version);
		actions_index[index] = rofl::openflow::OFPAT_POP_PBB;
	}
	return dynamic_cast<cofaction_pop_pbb&>( *(actions[index]) );
}



const cofaction_pop_pbb&
cofactions::get_action_pop_pbb(const cindex& index) const
{
	if ((actions_index.find(index) == actions_index.end()) ||
			(rofl::openflow::OFPAT_POP_PBB != actions_index.at(index))) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_pop_pbb&>( *(actions.at(index)) );
}



void
cofactions::drop_action_pop_pbb(const cindex& index)
{
	if (rofl::openflow::OFPAT_POP_PBB != actions_index[index]) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_pop_pbb(const cindex& index) const
{
	return ((actions_index.find(index) != actions_index.end()) &&
			(rofl::openflow::OFPAT_POP_PBB == actions_index.at(index)));
}


