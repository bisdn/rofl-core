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

	for (std::map<unsigned int, cofaction*>::const_iterator
			it = actions.begin(); it != actions.end(); ++it) {
		//map_and_insert(*(it->second));
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
	for (std::map<unsigned int, cofaction*>::iterator
			it = actions.begin(); it != actions.end(); ++it) {
		delete (it->second);
	}
	actions.clear();
}



void
cofactions::drop_action(unsigned int index)
{
	if (actions.find(index) == actions.end()) {
		return;
	}
	delete actions[index];
	actions.erase(index);
}



cofaction_output&
cofactions::add_action_output(unsigned int index)
{
	if (actions.find(index) != actions.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_output(ofp_version);
	return dynamic_cast<cofaction_output&>(*(actions[index]));
}



cofaction_output&
cofactions::set_action_output(unsigned int index)
{
	if ((actions.find(index) != actions.end()) &&
			(rofl::openflow::OFPAT_OUTPUT != actions[index]->get_type())) {
		throw eInval("cofactions::set_action_output() invalid action type");
	}
	if (actions.find(index) == actions.end()) {
		actions[index] = new cofaction_output(ofp_version);
	}
	return dynamic_cast<cofaction_output&>(*(actions[index]));
}



const cofaction_output&
cofactions::get_action_output(unsigned int index) const
{
	if ((actions.find(index) == actions.end()) ||
			(rofl::openflow::OFPAT_OUTPUT != actions.at(index)->get_type())) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_output&>(*(actions.at(index)));
}



void
cofactions::drop_action_output(unsigned int index)
{
	if (rofl::openflow::OFPAT_OUTPUT != actions[index]->get_type()) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_output(unsigned int index) const
{
	return ((not (actions.find(index) == actions.end())) &&
			(rofl::openflow::OFPAT_OUTPUT == actions.at(index)->get_type()));
}



cofaction_set_vlan_vid&
cofactions::add_action_set_vlan_vid(unsigned int index)
{
	if (rofl::openflow10::OFP_VERSION != get_version())
		throw eBadVersion("cofactions::add_action_set_vlan_vid() invalid version");

	if (actions.find(index) != actions.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_vlan_vid(ofp_version);
	return dynamic_cast<cofaction_set_vlan_vid&>( *(actions[index]) );
}



cofaction_set_vlan_vid&
cofactions::set_action_set_vlan_vid(unsigned int index)
{
	if (actions.find(index) == actions.end()) {
		actions[index] = new cofaction_set_vlan_vid(ofp_version);
	}
	return dynamic_cast<cofaction_set_vlan_vid&>( *(actions[index]) );
}



const cofaction_set_vlan_vid&
cofactions::get_action_set_vlan_vid(unsigned int index) const
{
	if (actions.find(index) == actions.end()) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_vlan_vid&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_vlan_vid(unsigned int index)
{
	if (rofl::openflow::OFPAT_SET_VLAN_VID != actions[index]->get_type()) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_vlan_vid(unsigned int index) const
{
	return ((not (actions.find(index) == actions.end())) &&
			(rofl::openflow::OFPAT_SET_VLAN_VID == actions.at(index)->get_type()));
}




cofaction_set_vlan_pcp&
cofactions::add_action_set_vlan_pcp(unsigned int index)
{
	if (rofl::openflow10::OFP_VERSION != get_version())
		throw eBadVersion("cofactions::add_action_set_vlan_pcp() invalid version");

	if (actions.find(index) != actions.end()) {
		delete actions[index];
	}
	actions[index] = new cofaction_set_vlan_pcp(ofp_version);
	return dynamic_cast<cofaction_set_vlan_pcp&>( *(actions[index]) );
}



cofaction_set_vlan_pcp&
cofactions::set_action_set_vlan_pcp(unsigned int index)
{
	if (actions.find(index) == actions.end()) {
		actions[index] = new cofaction_set_vlan_pcp(ofp_version);
	}
	return dynamic_cast<cofaction_set_vlan_pcp&>( *(actions[index]) );
}



const cofaction_set_vlan_pcp&
cofactions::get_action_set_vlan_pcp(unsigned int index) const
{
	if (actions.find(index) == actions.end()) {
		throw eActionNotFound();
	}
	return dynamic_cast<const cofaction_set_vlan_pcp&>( *(actions.at(index)) );
}



void
cofactions::drop_action_set_vlan_pcp(unsigned int index)
{
	if (rofl::openflow::OFPAT_SET_VLAN_PCP != actions[index]->get_type()) {
		throw eActionInvalType();
	}
	drop_action(index);
}



bool
cofactions::has_action_set_vlan_pcp(unsigned int index) const
{
	return ((not (actions.find(index) == actions.end())) &&
			(rofl::openflow::OFPAT_SET_VLAN_PCP == actions.at(index)->get_type()));
}


































size_t
cofactions::length() const
{
	size_t len = 0;
	for (std::map<unsigned int, cofaction*>::const_iterator
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

	for (std::map<unsigned int, cofaction*>::iterator
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

	unsigned int index = 0;

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

		// TODO

		default: {
			add_action(index++).unpack(buf, len);
		}
		}

		buf += len;
		buflen -= len;
	}
}






int
cofactions::count_action_type(
		uint16_t type)
{
	return count_if(actions.begin(), actions.end(), cofaction::cofaction_find_by_type(type));
}


int
cofactions::count_action_output(
		uint32_t port_no) const
{
	int action_cnt = 0;
	for (std::map<unsigned int, cofaction*>::const_iterator
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
cofactions::actions_output_ports()
{
	std::list<uint32_t> outports;

	for (std::map<unsigned int, cofaction*>::iterator
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
	for (std::map<unsigned int, cofaction*>::const_iterator
			it = actions.begin(); it != actions.end(); ++it) {
		it->second->check_prerequisites();
	}
}



