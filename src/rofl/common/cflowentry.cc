/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cflowentry.h"

using namespace rofl;

cflowentry::cflowentry(uint8_t of_version, uint16_t __type) :
		of_version(of_version),
		match(of_version, __type),
		instructions(of_version)
{
	switch (of_version) {
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


	WRITELOG(CFTTABLE, DBG, "cflowentry(%p)::cflowentry()", this);

}


cflowentry::~cflowentry()
{
	WRITELOG(CFTTABLE, DBG, "cflowentry(%p)::~cflowentry()", this);
}


cflowentry::cflowentry(cflowentry const& fe)
{
	WRITELOG(CFTTABLE, DBG, "cflowentry(%p)::cflowentry()", this);
	*this = fe;
}


cflowentry&
cflowentry::operator= (const cflowentry& fe)
{
	if (this == &fe)
		return *this;

	this->match 		= fe.match;
	this->instructions 	= fe.instructions;
	this->flow_mod_area = fe.flow_mod_area;
	this->of_version	= fe.of_version;

	switch (of_version) {
	case openflow12::OFP_VERSION: {
		of12m_flow_mod = (struct openflow12::ofp_flow_mod*)(flow_mod_area.somem());
	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod = (struct openflow13::ofp_flow_mod*)(flow_mod_area.somem());
	} break;
	default: {
		// do nothing
	} break;
	}

	return *this;
}


#if 0
bool
cflowentry::operator< (cflowentry const& fe) const
{
	if (get_command() != fe.get_command())
		return (get_command() < fe.get_command());
	if (get_table_id() != fe.get_table_id())
		return (get_table_id() < fe.get_table_id());
	if (get_idle_timeout() != fe.get_idle_timeout())
		return (get_idle_timeout() < fe.get_idle_timeout());
	if (get_hard_timeout() != fe.get_hard_timeout())
		return (get_hard_timeout() < fe.get_hard_timeout());
	if (get_cookie() != fe.get_cookie())
		return (get_cookie() < fe.get_cookie());
	if (get_cookie_mask() != fe.get_cookie_mask())
		return (get_cookie_mask() < fe.get_cookie_mask());
	if (get_priority() != fe.get_priority())
		return (get_priority() < fe.get_priority());
	if (get_buffer_id() != fe.get_buffer_id())
		return (get_buffer_id() < fe.get_buffer_id());
	if (get_out_port() != fe.get_out_port())
		return (get_out_port() < fe.get_out_port());
	if (get_out_group() != fe.get_out_group())
		return (get_out_group() < fe.get_out_group());
	if (get_flags() != fe.get_flags())
		return (get_flags() < fe.get_flags());
	if ((match < fe.match) || (fe.match < match))
		return (match < fe.match);
	// TODO: add operator< to class cofinlist
#if 0
	if ((instructions < fe.instructions) || (fe.instructions < instructions))
		return (instructions < fe.instructions);
#endif
	return false;
}
#endif


void
cflowentry::reset()
{
	WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::reset()", this);

	match.clear();

	instructions.clear();

	flow_mod_area.clear();

	switch (of_version) {
	case openflow12::OFP_VERSION: {
		of12m_flow_mod = (struct openflow12::ofp_flow_mod*)(flow_mod_area.somem());

		of12m_flow_mod->table_id 		= 0;
		of12m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of12m_flow_mod->buffer_id 		= htobe32(openflow12::OFP_NO_BUFFER);	// default: buffer_id = -1
		of12m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of12m_flow_mod->idle_timeout 	= htobe16(5);				// default: idle_timeout = 5 secs
		of12m_flow_mod->hard_timeout 	= htobe16(0);				// default: hard_timeout = 0 secs (not used)
		of12m_flow_mod->command 		= openflow12::OFPFC_ADD;				// default: add flow-mod entry
		of12m_flow_mod->out_port 		= htobe32(openflow12::OFPP_ANY);
		of12m_flow_mod->out_group 		= htobe32(openflow12::OFPG_ANY);

	} break;
	case openflow13::OFP_VERSION: {
		of13m_flow_mod = (struct openflow13::ofp_flow_mod*)(flow_mod_area.somem());

		of13m_flow_mod->table_id 		= 0;
		of13m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of13m_flow_mod->buffer_id 		= htobe32(openflow13::OFP_NO_BUFFER);	// default: buffer_id = -1
		of13m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of13m_flow_mod->idle_timeout 	= htobe16(5);				// default: idle_timeout = 5 secs
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
cflowentry::set_table_id(uint8_t table_id)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->table_id = table_id;
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_command(uint8_t command)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->command = command;
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_idle_timeout(const uint16_t& idle_timeout)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->idle_timeout = htobe16(idle_timeout);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_hard_timeout(const uint16_t& hard_timeout)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->hard_timeout = htobe16(hard_timeout);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_cookie(const uint64_t& cookie)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->cookie = htobe64(cookie);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_cookie_mask(const uint64_t& cookie_mask)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->cookie_mask = htobe64(cookie_mask);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_priority(const uint16_t& priority)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->priority = htobe16(priority);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_buffer_id(const uint32_t& buffer_id)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->buffer_id = htobe32(buffer_id);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_out_port(const uint32_t& out_port)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->out_port = htobe32(out_port);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_out_group(const uint32_t& out_group)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->out_group = htobe32(out_group);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


void
cflowentry::set_flags(const uint16_t& flags)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		of13m_flow_mod->flags = htobe16(flags);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}


uint8_t
cflowentry::get_command() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return of13m_flow_mod->command;
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint8_t
cflowentry::get_table_id() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return of13m_flow_mod->table_id;
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint16_t
cflowentry::get_idle_timeout() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return be16toh(of13m_flow_mod->idle_timeout);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint16_t
cflowentry::get_hard_timeout() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return be16toh(of13m_flow_mod->hard_timeout);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint64_t
cflowentry::get_cookie() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return be64toh(of13m_flow_mod->cookie);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint64_t
cflowentry::get_cookie_mask() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return be64toh(of13m_flow_mod->cookie_mask);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint16_t
cflowentry::get_priority() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return be16toh(of13m_flow_mod->priority);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint32_t
cflowentry::get_buffer_id() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return be32toh(of13m_flow_mod->buffer_id);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint32_t
cflowentry::get_out_port() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return be32toh(of13m_flow_mod->out_port);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint32_t
cflowentry::get_out_group() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return be32toh(of13m_flow_mod->out_group);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}


uint16_t
cflowentry::get_flags() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
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
cflowentry::pack()
{
	size_t instslen = 0;
	size_t ofmatch_len = 0;
	size_t fm_len = 0;
	size_t packlen = 0;

	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		instslen = instructions.length();
		ofmatch_len = match.length();
		// length of generic ofp_flow_mod header without ofp_match
		fm_len = sizeof(struct openflow12::ofp_flow_mod) - sizeof(openflow12::ofp_match);

		packlen = fm_len + ofmatch_len + instslen;

		WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [-] instslen:%d matchlen:%d fmgenlen:%d packlen:%d",
				this,
				instslen,
				ofmatch_len,
				fm_len,
				packlen);

		//WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() instructions length: %d *this: %s", this, instslen, c_str());

		//WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [0] flow_mod_area: %s", this, flow_mod_area.c_str());


		if (packlen > flow_mod_area.memlen()) // not enough space, resize memory area for flow_mod
		{
			WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() resizing flow_mod_area: from %d to %d",
					this, flow_mod_area.memlen(), packlen);
			flow_mod_area.resize(packlen);
		}

		//WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [1] flow_mod_area: %s", this, flow_mod_area.c_str());


		//WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [1] flow_mod_area: %s", this, flow_mod_area.c_str());

		struct openflow12::ofp_match* m = (struct openflow12::ofp_match*)(flow_mod_area.somem() + fm_len);
		match.pack((uint8_t*)m, ofmatch_len);

		//WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [2] flow_mod_area: %s", this, flow_mod_area.c_str());

		uint8_t* insts = (flow_mod_area.somem() + fm_len + ofmatch_len);
		instructions.pack(insts, instslen);

		//WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [3] flow_mod_area: %s", this, flow_mod_area.c_str());

	} break;
	default: {
		throw eBadVersion();
	} break;
	}

	return packlen;
}


void
cflowentry::test()
{
	cmacaddr dl_mask("ff:ff:ff:ff:ff:ff");
	cmacaddr dl_src("00:01:01:01:01:01");
	cmacaddr dl_dst("00:02:02:02:02:02");

	cflowentry fe(openflow12::OFP_VERSION);

	fe.set_command((uint16_t)openflow12::OFPFC_ADD);
	fe.set_idle_timeout(5);
	fe.set_hard_timeout(0);

	fe.match.set_in_port(1);
	fe.match.set_eth_type(0x0800);
	fe.match.set_eth_src(dl_src);
	fe.match.set_eth_dst(dl_dst);

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

