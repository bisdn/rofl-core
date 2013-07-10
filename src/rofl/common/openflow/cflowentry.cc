/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cflowentry.h"

using namespace rofl;

cflowentry::cflowentry(uint8_t of_version, uint16_t __type) throw (eFlowEntryOutOfMem) :
		of_version(of_version),
		match(of_version, __type)
{
	switch (of_version) {
	case OFP12_VERSION: {
		flow_mod_area.resize(sizeof(struct ofp12_flow_mod));
		of12m_flow_mod = (struct ofp12_flow_mod*)flow_mod_area.somem();

		of12m_flow_mod->table_id 		= 0;
		of12m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of12m_flow_mod->buffer_id 		= htobe32(OFP_NO_BUFFER);	// default: buffer_id = -1
		of12m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of12m_flow_mod->idle_timeout 	= htobe16(5);				// default: idle_timeout = 5 secs
		of12m_flow_mod->hard_timeout 	= htobe16(0);				// default: hard_timeout = 0 secs (not used)
		of12m_flow_mod->command 		= OFPFC_ADD;				// default: add flow-mod entry
		of12m_flow_mod->out_port 		= htobe32(OFPP_ANY);
		of12m_flow_mod->out_group 		= htobe32(OFPG_ANY);

	} break;
	case OFP13_VERSION: {
		flow_mod_area.resize(sizeof(struct ofp13_flow_mod));
		of13m_flow_mod = (struct ofp13_flow_mod*)flow_mod_area.somem();

		of13m_flow_mod->table_id 		= 0;
		of13m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of13m_flow_mod->buffer_id 		= htobe32(OFP_NO_BUFFER);	// default: buffer_id = -1
		of13m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of13m_flow_mod->idle_timeout 	= htobe16(5);				// default: idle_timeout = 5 secs
		of13m_flow_mod->hard_timeout 	= htobe16(0);				// default: hard_timeout = 0 secs (not used)
		of13m_flow_mod->command 		= OFPFC_ADD;				// default: add flow-mod entry
		of13m_flow_mod->out_port 		= htobe32(OFPP_ANY);
		of13m_flow_mod->out_group 		= htobe32(OFPG_ANY);

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
	case OFP12_VERSION: {
		of12m_flow_mod = (struct ofp12_flow_mod*)(flow_mod_area.somem());
	} break;
	case OFP13_VERSION: {
		of13m_flow_mod = (struct ofp13_flow_mod*)(flow_mod_area.somem());
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
	case OFP12_VERSION: {
		of12m_flow_mod = (struct ofp12_flow_mod*)(flow_mod_area.somem());

		of12m_flow_mod->table_id 		= 0;
		of12m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of12m_flow_mod->buffer_id 		= htobe32(OFP_NO_BUFFER);	// default: buffer_id = -1
		of12m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of12m_flow_mod->idle_timeout 	= htobe16(5);				// default: idle_timeout = 5 secs
		of12m_flow_mod->hard_timeout 	= htobe16(0);				// default: hard_timeout = 0 secs (not used)
		of12m_flow_mod->command 		= OFPFC_ADD;				// default: add flow-mod entry
		of12m_flow_mod->out_port 		= htobe32(OFPP_ANY);
		of12m_flow_mod->out_group 		= htobe32(OFPG_ANY);

	} break;
	case OFP13_VERSION: {
		of13m_flow_mod = (struct ofp13_flow_mod*)(flow_mod_area.somem());

		of13m_flow_mod->table_id 		= 0;
		of13m_flow_mod->cookie_mask 	= htobe64(0xffffffffffffffffULL);
		of13m_flow_mod->buffer_id 		= htobe32(OFP_NO_BUFFER);	// default: buffer_id = -1
		of13m_flow_mod->priority 		= htobe16(0x0800);			// default: priority = 0x0800
		of13m_flow_mod->idle_timeout 	= htobe16(5);				// default: idle_timeout = 5 secs
		of13m_flow_mod->hard_timeout 	= htobe16(0);				// default: hard_timeout = 0 secs (not used)
		of13m_flow_mod->command 		= OFPFC_ADD;				// default: add flow-mod entry
		of13m_flow_mod->out_port 		= htobe32(OFPP_ANY);
		of13m_flow_mod->out_group 		= htobe32(OFPG_ANY);

	} break;
	default: {
		// do nothing
	} break;
	}
}




const char*
cflowentry::c_str()
{
	std::string s_cmd;

	switch (of_version) {
	case OFP12_VERSION: {
		switch (of12m_flow_mod->command) {
		case OFPFC_ADD:
			s_cmd.assign("OFPFC_ADD");
			break;
		case OFPFC_DELETE:
			s_cmd.assign("OFPFC_DELETE");
			break;
		case OFPFC_DELETE_STRICT:
			s_cmd.assign("OFPFC_DELETE_STRICT");
			break;
		case OFPFC_MODIFY:
			s_cmd.assign("OFPFC_MODIFY");
			break;
		case OFPFC_MODIFY_STRICT:
			s_cmd.assign("OFPFC_MODIFY_STRICT");
			break;
		default:
			s_cmd.assign("OFPFC_UNKNOWN");
			break;
		}

		cvastring vas(2048);
		info.assign(vas("[cflowentry(%p) "
				"%s table-id[%d] buffer-id[0x%x] idle_timeout[%d] hard_timeout[%d] priority[%d] "
				"\n%s\n%s]",
				this,
				s_cmd.c_str(),
				of12m_flow_mod->table_id,
				be32toh(of12m_flow_mod->buffer_id),
				be16toh(of12m_flow_mod->idle_timeout),
				be16toh(of12m_flow_mod->hard_timeout),
				be16toh(of12m_flow_mod->priority),
				match.c_str(),
				instructions.c_str()));

	} break;
	case OFP13_VERSION: {
		switch (of13m_flow_mod->command) {
		case OFPFC_ADD:
			s_cmd.assign("OFPFC_ADD");
			break;
		case OFPFC_DELETE:
			s_cmd.assign("OFPFC_DELETE");
			break;
		case OFPFC_DELETE_STRICT:
			s_cmd.assign("OFPFC_DELETE_STRICT");
			break;
		case OFPFC_MODIFY:
			s_cmd.assign("OFPFC_MODIFY");
			break;
		case OFPFC_MODIFY_STRICT:
			s_cmd.assign("OFPFC_MODIFY_STRICT");
			break;
		default:
			s_cmd.assign("OFPFC_UNKNOWN");
			break;
		}

		cvastring vas(2048);
		info.assign(vas("[cflowentry(%p) "
				"%s table-id[%d] buffer-id[0x%x] idle_timeout[%d] hard_timeout[%d] priority[%d] "
				"\n%s\n%s]",
				this,
				s_cmd.c_str(),
				of13m_flow_mod->table_id,
				be32toh(of13m_flow_mod->buffer_id),
				be16toh(of13m_flow_mod->idle_timeout),
				be16toh(of13m_flow_mod->hard_timeout),
				be16toh(of13m_flow_mod->priority),
				match.c_str(),
				instructions.c_str()));

	} break;
	default: {
		throw eBadVersion();
	} break;
	}

	return info.c_str();
}


void
cflowentry::set_table_id(uint8_t table_id)
{
	switch (of_version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
	case OFP12_VERSION:
	case OFP13_VERSION: {
		instslen = instructions.length();
		ofmatch_len = match.length();
		// length of generic ofp_flow_mod header without ofp_match
		fm_len = sizeof(struct ofp12_flow_mod) - sizeof(ofp12_match);

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


		WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [1] flow_mod_area: %s", this, flow_mod_area.c_str());

		struct ofp12_match* m = (struct ofp12_match*)(flow_mod_area.somem() + fm_len);
		match.pack(m, ofmatch_len);

		WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [2] flow_mod_area: %s", this, flow_mod_area.c_str());

		struct ofp_instruction *insts = (struct ofp_instruction*)(flow_mod_area.somem() + fm_len + ofmatch_len);
		instructions.pack(insts, instslen);

		WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [3] flow_mod_area: %s", this, flow_mod_area.c_str());

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

	cflowentry fe(OFP12_VERSION);

	fe.set_command((uint16_t)OFPFC_ADD);
	fe.set_idle_timeout(5);
	fe.set_hard_timeout(0);

	fe.match.set_in_port(1);
	fe.match.set_eth_type(0x0800);
	fe.match.set_eth_src(dl_src);
	fe.match.set_eth_dst(dl_dst);

	fe.instructions[0] = cofinst_clear_actions();

	fe.instructions[1] = cofinst_apply_actions();
	fe.instructions[1].actions[0] = cofaction_output(2);
	fe.instructions[1].actions[1] = cofaction_set_field(coxmatch_ofb_vlan_vid(3));
	fe.instructions[1].actions[2] = cofaction_set_field(coxmatch_ofb_ip_dscp(4));
	fe.instructions[1].actions[3] = cofaction_push_mpls(50);
	fe.instructions[1].actions[4] = cofaction_push_mpls(100);
	fe.instructions[1].actions[5] = cofaction_set_field(coxmatch_ofb_tcp_dst(8080));

	fe.instructions[2] = cofinst_write_actions();
	fe.instructions[2].actions[0] = cofaction_pop_mpls(1000);
	fe.instructions[2].actions[1] = cofaction_group(32);

	fe.instructions[3] = cofinst_goto_table(5);

	fe.instructions[4] = cofinst_write_metadata(1, 2);

	//fe.reset();

	fprintf(stderr, "XXXXX ==>: %s\n", fe.c_str());
}

