/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofflowmod.h"

using namespace rofl::openflow;

cofflowmod::cofflowmod(uint8_t of_version, uint16_t __type) :
		of_version(of_version),
		match(of_version, __type),
		instructions(of_version)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case openflow10::OFP_VERSION: {
		flow_mod_area.resize(sizeof(struct openflow10::ofp_flow_mod));
		of10m_flow_mod = (struct openflow10::ofp_flow_mod*)flow_mod_area.somem();

	} break;
	case openflow12::OFP_VERSION: {
		flow_mod_area.resize(sizeof(struct openflow12::ofp_flow_mod));
		of12m_flow_mod = (struct openflow12::ofp_flow_mod*)flow_mod_area.somem();

		of12m_flow_mod->table_id 		= 0;
		of12m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of12m_flow_mod->buffer_id 		= htobe32(openflow12::OFP_NO_BUFFER);	// default: buffer_id = -1
		of12m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of12m_flow_mod->idle_timeout 	= htobe16(0);				// default: idle_timeout = 5 secs
		of12m_flow_mod->hard_timeout 	= htobe16(0);				// default: hard_timeout = 0 secs (not used)
		of12m_flow_mod->command 		= openflow12::OFPFC_ADD;	// default: add flow-mod entry
		of12m_flow_mod->out_port 		= htobe32(openflow12::OFPP_ANY);
		of12m_flow_mod->out_group 		= htobe32(openflow12::OFPG_ANY);

	} break;
	case openflow13::OFP_VERSION: {
		flow_mod_area.resize(sizeof(struct openflow13::ofp_flow_mod));
		of13m_flow_mod = (struct openflow13::ofp_flow_mod*)flow_mod_area.somem();

		of13m_flow_mod->table_id 		= 0;
		of13m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of13m_flow_mod->buffer_id 		= htobe32(openflow13::OFP_NO_BUFFER);	// default: buffer_id = -1
		of13m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of13m_flow_mod->idle_timeout 	= htobe16(0);				// default: idle_timeout = 5 secs
		of13m_flow_mod->hard_timeout 	= htobe16(0);				// default: hard_timeout = 0 secs (not used)
		of13m_flow_mod->command 		= openflow13::OFPFC_ADD;	// default: add flow-mod entry
		of13m_flow_mod->out_port 		= htobe32(openflow13::OFPP_ANY);
		of13m_flow_mod->out_group 		= htobe32(openflow13::OFPG_ANY);

	} break;
	default: {
		throw eBadVersion();
	}
	}
}


cofflowmod::~cofflowmod()
{

}


cofflowmod::cofflowmod(cofflowmod const& fe)
{
	*this = fe;
}


cofflowmod&
cofflowmod::operator= (const cofflowmod& fe)
{
	if (this == &fe)
		return *this;

	this->match 		= fe.match;
	this->actions		= fe.actions;
	this->instructions 	= fe.instructions;
	this->flow_mod_area = fe.flow_mod_area;
	this->of_version	= fe.of_version;

	ofm_generic = flow_mod_area.somem();

	return *this;
}



void
cofflowmod::reset()
{
	match.clear();
	actions.clear();
	instructions.clear();
	flow_mod_area.clear();


	switch (of_version) {
	case openflow10::OFP_VERSION: {
		of10m_flow_mod = (struct openflow10::ofp_flow_mod*)(flow_mod_area.somem());

		of10m_flow_mod->cookie		 	= htobe64(0xffffffffffffffffULL);
		of10m_flow_mod->command 		= openflow10::OFPFC_ADD;				// default: add flow-mod entry
		of10m_flow_mod->idle_timeout 	= htobe16(0);							// default: idle_timeout = 0 secs (not used)
		of10m_flow_mod->hard_timeout 	= htobe16(0);							// default: hard_timeout = 0 secs (not used)
		of10m_flow_mod->priority	 	= htobe16(0);
		of10m_flow_mod->buffer_id 		= htobe32(openflow10::OFP_NO_BUFFER);	// default: buffer_id = -1
		of10m_flow_mod->out_port 		= htobe16(openflow10::OFPP_ALL);
		of10m_flow_mod->flags	 		= htobe16(0);

	} break;
	case openflow12::OFP_VERSION: {
		of12m_flow_mod = (struct openflow12::ofp_flow_mod*)(flow_mod_area.somem());

		of12m_flow_mod->table_id 		= 0;
		of12m_flow_mod->cookie		 	= htobe64(0xffffffffffffffffULL);
		of12m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of12m_flow_mod->buffer_id 		= htobe32(openflow12::OFP_NO_BUFFER);	// default: buffer_id = -1
		of12m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of12m_flow_mod->idle_timeout 	= htobe16(0);				// default: idle_timeout = 0 secs (not used)
		of12m_flow_mod->hard_timeout 	= htobe16(0);				// default: hard_timeout = 0 secs (not used)
		of12m_flow_mod->command 		= openflow12::OFPFC_ADD;				// default: add flow-mod entry
		of12m_flow_mod->out_port 		= htobe32(openflow12::OFPP_ANY);
		of12m_flow_mod->out_group 		= htobe32(openflow12::OFPG_ANY);

	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod = (struct openflow13::ofp_flow_mod*)(flow_mod_area.somem());

		of13m_flow_mod->table_id 		= 0;
		of13m_flow_mod->cookie		 	= htobe64(0xffffffffffffffffULL);
		of13m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of13m_flow_mod->buffer_id 		= htobe32(openflow13::OFP_NO_BUFFER);	// default: buffer_id = -1
		of13m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of13m_flow_mod->idle_timeout 	= htobe16(0);				// default: idle_timeout = 0 secs (not used)
		of13m_flow_mod->hard_timeout 	= htobe16(0);				// default: hard_timeout = 0 secs (not used)
		of13m_flow_mod->command 		= openflow13::OFPFC_ADD;	// default: add flow-mod entry
		of13m_flow_mod->out_port 		= htobe32(openflow13::OFPP_ANY);
		of13m_flow_mod->out_group 		= htobe32(openflow13::OFPG_ANY);

	} break;
	default: {
		// do nothing
	} break;
	}
}



void
cofflowmod::set_table_id(uint8_t table_id)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->table_id = table_id;
	} break;
	case openflow10::OFP_VERSION:
	default: {
		throw eBadVersion();
	} break;
	}
}


uint8_t
cofflowmod::get_table_id() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		return of12m_flow_mod->table_id;
	} break;
	case openflow13::OFP_VERSION: {
		return of13m_flow_mod->table_id;
	} break;
	case openflow10::OFP_VERSION:
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_command(uint8_t command)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		of10m_flow_mod->command = htobe16((uint16_t)command);
	} break;
	case openflow12::OFP_VERSION: {
		of12m_flow_mod->command = command;
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->command = command;
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


uint8_t
cofflowmod::get_command() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return (uint8_t)be16toh(of10m_flow_mod->command);
	} break;
	case openflow12::OFP_VERSION: {
		return of12m_flow_mod->command;
	} break;
	case openflow13::OFP_VERSION: {
		return of13m_flow_mod->command;
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_idle_timeout(const uint16_t& idle_timeout)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		of10m_flow_mod->idle_timeout = htobe16(idle_timeout);
	} break;
	case openflow12::OFP_VERSION: {
		of12m_flow_mod->idle_timeout = htobe16(idle_timeout);
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->idle_timeout = htobe16(idle_timeout);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


uint16_t
cofflowmod::get_idle_timeout() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be16toh(of10m_flow_mod->idle_timeout);
	} break;
	case openflow12::OFP_VERSION: {
		return be16toh(of12m_flow_mod->idle_timeout);
	} break;
	case openflow13::OFP_VERSION: {
		return be16toh(of13m_flow_mod->idle_timeout);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_hard_timeout(const uint16_t& hard_timeout)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		of10m_flow_mod->hard_timeout = htobe16(hard_timeout);
	} break;
	case openflow12::OFP_VERSION: {
		of12m_flow_mod->hard_timeout = htobe16(hard_timeout);
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->hard_timeout = htobe16(hard_timeout);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


uint16_t
cofflowmod::get_hard_timeout() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be16toh(of10m_flow_mod->hard_timeout);
	} break;
	case openflow12::OFP_VERSION: {
		return be16toh(of12m_flow_mod->hard_timeout);
	} break;
	case openflow13::OFP_VERSION: {
		return be16toh(of13m_flow_mod->hard_timeout);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_cookie(const uint64_t& cookie)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		of10m_flow_mod->cookie = htobe64(cookie);
	} break;
	case openflow12::OFP_VERSION: {
		of12m_flow_mod->cookie = htobe64(cookie);
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->cookie = htobe64(cookie);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


uint64_t
cofflowmod::get_cookie() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be64toh(of10m_flow_mod->cookie);
	} break;
	case openflow12::OFP_VERSION: {
		return be64toh(of12m_flow_mod->cookie);
	} break;
	case openflow13::OFP_VERSION: {
		return be64toh(of13m_flow_mod->cookie);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_cookie_mask(const uint64_t& cookie_mask)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->cookie_mask = htobe64(cookie_mask);
	} break;
	case openflow10::OFP_VERSION:
	default: {
		throw eBadVersion();
	} break;
	}
}


uint64_t
cofflowmod::get_cookie_mask() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		return be64toh(of12m_flow_mod->cookie_mask);
	} break;
	case openflow13::OFP_VERSION: {
		return be64toh(of13m_flow_mod->cookie_mask);
	} break;
	case openflow10::OFP_VERSION:
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_priority(const uint16_t& priority)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		of10m_flow_mod->priority = htobe16(priority);
	} break;
	case openflow12::OFP_VERSION: {
		of12m_flow_mod->priority = htobe16(priority);
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->priority = htobe16(priority);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


uint16_t
cofflowmod::get_priority() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be16toh(of10m_flow_mod->priority);
	} break;
	case openflow12::OFP_VERSION: {
		return be16toh(of12m_flow_mod->priority);
	} break;
	case openflow13::OFP_VERSION: {
		return be16toh(of13m_flow_mod->priority);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_buffer_id(const uint32_t& buffer_id)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		of10m_flow_mod->buffer_id = htobe32(buffer_id);
	} break;
	case openflow12::OFP_VERSION: {
		of12m_flow_mod->buffer_id = htobe32(buffer_id);
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->buffer_id = htobe32(buffer_id);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


uint32_t
cofflowmod::get_buffer_id() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be32toh(of10m_flow_mod->buffer_id);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(of12m_flow_mod->buffer_id);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(of13m_flow_mod->buffer_id);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_out_port(const uint32_t& out_port)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		of10m_flow_mod->out_port = htobe16(out_port);
	} break;
	case openflow12::OFP_VERSION: {
		of12m_flow_mod->out_port = htobe32(out_port);
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->out_port = htobe32(out_port);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


uint32_t
cofflowmod::get_out_port() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be16toh(of10m_flow_mod->out_port);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(of12m_flow_mod->out_port);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(of13m_flow_mod->out_port);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_out_group(const uint32_t& out_group)
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		of12m_flow_mod->out_group = htobe32(out_group);
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->out_group = htobe32(out_group);
	} break;
	case openflow10::OFP_VERSION:
	default: {
		throw eBadVersion();
	} break;
	}
}


uint32_t
cofflowmod::get_out_group() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		return be32toh(of12m_flow_mod->out_group);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(of13m_flow_mod->out_group);
	} break;
	case openflow10::OFP_VERSION:
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


void
cofflowmod::set_flags(const uint16_t& flags)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		of10m_flow_mod->flags = htobe16(flags);
	} break;
	case openflow12::OFP_VERSION: {
		of12m_flow_mod->flags = htobe16(flags);
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->flags = htobe16(flags);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


uint16_t
cofflowmod::get_flags() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be16toh(of10m_flow_mod->flags);
	} break;
	case openflow12::OFP_VERSION: {
		return be16toh(of12m_flow_mod->flags);
	} break;
	case openflow13::OFP_VERSION: {
		return be16toh(of13m_flow_mod->flags);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


size_t
cofflowmod::length() const
{
	switch(of_version) {
	case openflow10::OFP_VERSION: {
		return sizeof(struct openflow10::ofp_flow_mod) + actions.length();
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_flow_mod) - sizeof(openflow12::ofp_match) + match.length() + instructions.length());
	} break;
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_flow_mod) - sizeof(openflow13::ofp_match) + match.length() + instructions.length());
	} break;
	default:
		throw eBadVersion();
	}
}


size_t
cofflowmod::pack()
{
	size_t instslen = 0;
	size_t ofmatch_len = 0;
	size_t fm_len = 0;
	size_t packlen = length();

	switch (of_version) {
	case openflow10::OFP_VERSION: {

		if (length() > flow_mod_area.memlen()) {// not enough space, resize memory area for flow_mod
			ofm_generic = flow_mod_area.resize(packlen);
		}

		match.pack((uint8_t*)&(of10m_flow_mod->match), sizeof(struct openflow10::ofp_match));
		actions.pack((uint8_t*)of10m_flow_mod->actions, actions.length());

	} break;
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		instslen = instructions.length();
		ofmatch_len = match.length();
		// length of generic ofp_flow_mod header without ofp_match
		fm_len = sizeof(struct openflow12::ofp_flow_mod) - sizeof(openflow12::ofp_match);

		packlen = fm_len + ofmatch_len + instslen;

		if (length() > flow_mod_area.memlen()) {// not enough space, resize memory area for flow_mod
			ofm_generic = flow_mod_area.resize(packlen);
		}

		struct openflow12::ofp_match* m = (struct openflow12::ofp_match*)(flow_mod_area.somem() + fm_len);
		match.pack((uint8_t*)m, ofmatch_len);

		uint8_t* insts = (flow_mod_area.somem() + fm_len + ofmatch_len);
		instructions.pack(insts, instslen);

	} break;
	default: {
		throw eBadVersion();
	} break;
	}

	return packlen;
}


void
cofflowmod::example()
{
	cmacaddr dl_mask("ff:ff:ff:ff:ff:ff");
	cmacaddr dl_src("00:01:01:01:01:01");
	cmacaddr dl_dst("00:02:02:02:02:02");

	cofflowmod fe(openflow12::OFP_VERSION);

	fe.set_command((uint16_t)openflow12::OFPFC_ADD);
	fe.set_idle_timeout(5);
	fe.set_hard_timeout(0);

	fe.match.set_matches().add_match(coxmatch_ofb_in_port(1));
	fe.match.set_matches().add_match(coxmatch_ofb_eth_type(0x0800));
	fe.match.set_matches().add_match(coxmatch_ofb_eth_src(dl_src));
	fe.match.set_matches().add_match(coxmatch_ofb_eth_dst(dl_dst));

	fe.instructions.add_inst_clear_actions();

	fe.instructions.add_inst_apply_actions();
	fe.instructions.get_inst_apply_actions().get_actions().append_action_output(2);
	fe.instructions.get_inst_apply_actions().get_actions().append_action_set_field(coxmatch_ofb_vlan_vid(3));
	fe.instructions.get_inst_apply_actions().get_actions().append_action_set_field(coxmatch_ofb_ip_dscp(4));
	fe.instructions.get_inst_apply_actions().get_actions().append_action_push_mpls(50);
	fe.instructions.get_inst_apply_actions().get_actions().append_action_push_mpls(100);
	fe.instructions.get_inst_apply_actions().get_actions().append_action_set_field(coxmatch_ofb_tcp_dst(8080));

	fe.instructions.add_inst_write_actions();
	fe.instructions.set_inst_write_actions().get_actions().append_action_pop_mpls(1000);
	fe.instructions.set_inst_write_actions().get_actions().append_action_group(32);

	fe.instructions.add_inst_goto_table().set_table_id(5);

	fe.instructions.add_inst_write_metadata();
	fe.instructions.get_inst_write_metadata().set_metadata(1);
	fe.instructions.get_inst_write_metadata().set_metadata_mask(2);

	//fe.reset();

	std::cerr << "XXXXX => " << fe << std::endl;
}

