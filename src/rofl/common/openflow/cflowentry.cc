/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cflowentry.h"

cflowentry::cflowentry(uint16_t __type) throw (eFlowEntryOutOfMem) :
		match(__type),
		flow_mod(NULL),
		flow_mod_area(sizeof(struct ofp_flow_mod) + 128/*space for instructions*/)
{
	WRITELOG(CFTTABLE, ROFL_DBG, "cflowentry(%p)::cflowentry()", this);

	flow_mod = (struct ofp_flow_mod*)flow_mod_area.somem();

	flow_mod->table_id = OFPTT_ALL;
	flow_mod->cookie_mask = htobe64(0xffffffffffffffffULL);
	flow_mod->buffer_id = htobe32(OFP_NO_BUFFER);	// default: buffer_id = -1
	flow_mod->priority = htobe16(0x0800);			// default: priority = 0x0800
	flow_mod->idle_timeout = htobe16(5);			// default: idle_timeout = 5 secs
	flow_mod->hard_timeout = htobe16(0);			// default: hard_timeout = 0 secs (not used)
	flow_mod->command = OFPFC_ADD;					// default: add flow-mod entry
	flow_mod->out_port = htobe32(OFPP_ANY);
	flow_mod->out_group = htobe32(OFPG_ANY);
}


cflowentry::~cflowentry()
{
	WRITELOG(CFTTABLE, ROFL_DBG, "cflowentry(%p)::~cflowentry()", this);
}


cflowentry::cflowentry(cflowentry const& fe)
{
	WRITELOG(CFTTABLE, ROFL_DBG, "cflowentry(%p)::cflowentry()", this);
	*this = fe;
}


cflowentry&
cflowentry::operator= (const cflowentry& fe)
{
	if (this == &fe)
		return *this;

	this->match = fe.match;
	this->instructions = fe.instructions;
	this->flow_mod_area = fe.flow_mod_area;

	this->flow_mod = (struct ofp_flow_mod*)this->flow_mod_area.somem();

	return *this;
}


void
cflowentry::reset()
{
	WRITELOG(UNKNOWN, ROFL_DBG, "cflowentry(%p)::reset()", this);

	match.reset();

	instructions.reset();

	flow_mod_area.clear();
	flow_mod = (struct ofp_flow_mod*)flow_mod_area.somem();

	flow_mod->buffer_id = htobe32(0xffffffff); 		// default: buffer_id = -1
	flow_mod->priority = htobe16(0x0800);			// default: priority = 0x0800
	flow_mod->idle_timeout = htobe16(5);			// default: idle_timeout = 5 secs
	flow_mod->hard_timeout = htobe16(0);			// default: hard_timeout = 0 secs (not used)
	flow_mod->command = OFPFC_ADD;					// default: add flow-mod entry
}


#if 0
void
cflowentry::match_set_type(const uint16_t& type)
{
	match.set_type(type);
}


void
cflowentry::match_set_in_port(const uint16_t& in_port)
{
	match.set_in_port(in_port);
}


void
cflowentry::match_set_dl_src(const cmemory& dl_src, const cmemory& dl_src_mask)
{
	match.set_dl_src(dl_src, dl_src_mask);
}


void
cflowentry::match_set_dl_dst(const cmemory& dl_dst, const cmemory& dl_dst_mask)
{
	match.set_dl_dst(dl_dst, dl_dst_mask);
}


void
cflowentry::match_set_dl_type(const uint16_t& dl_type)
{
	match.set_dl_type(dl_type);
}


void
cflowentry::match_set_dl_vlan(const uint16_t& dl_vlan)
{
	match.set_dl_vlan(dl_vlan);
}


void
cflowentry::match_set_dl_vlan_pcp(const uint8_t& dl_vlan_pcp)
{
	match.set_dl_vlan_pcp(dl_vlan_pcp);
}


void
cflowentry::match_set_nw_tos(const uint8_t& nw_tos)
{
	match.set_nw_tos(nw_tos);
}


void
cflowentry::match_set_nw_proto(const uint8_t& nw_proto)
{
	match.set_nw_proto(nw_proto);
}


void
cflowentry::match_set_nw_src(const caddress& nw_src, uint8_t nw_mask)
{
	match.set_nw_src(nw_src, nw_mask);
}


void
cflowentry::match_set_nw_dst(const caddress& nw_dst, uint8_t nw_mask)
{
	match.set_nw_dst(nw_dst, nw_mask);
}


void
cflowentry::match_set_tp_src(const uint16_t& tp_src)
{
	match.set_tp_src(tp_src);
}


void
cflowentry::match_set_tp_dst(const uint16_t& tp_dst)
{
	match.set_tp_dst(tp_dst);
}


void
cflowentry::match_set_pppoe_code(uint8_t code)
{
	match.set_pppoe_code(code);
}


void
cflowentry::match_set_pppoe_type(uint8_t type)
{
	match.set_pppoe_type(type);
}


void
cflowentry::match_set_pppoe_sessid(uint16_t sessid)
{
	match.set_pppoe_sessid(sessid);
}
#endif


const char*
cflowentry::c_str()
{
	std::string s_cmd;

	switch (flow_mod->command) {
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
			flow_mod->table_id,
			be32toh(flow_mod->buffer_id),
			be16toh(flow_mod->idle_timeout),
			be16toh(flow_mod->hard_timeout),
			be16toh(flow_mod->priority),
			match.c_str(),
			instructions.c_str()));

	return info.c_str();
}


void
cflowentry::set_table_id(uint8_t table_id)
{
	flow_mod->table_id = table_id;
}


void
cflowentry::set_command(uint8_t command)
{
	flow_mod->command = command;
}


void
cflowentry::set_idle_timeout(const uint16_t& idle_timeout)
{
	flow_mod->idle_timeout = htobe16(idle_timeout);
}


void
cflowentry::set_hard_timeout(const uint16_t& hard_timeout)
{
	flow_mod->hard_timeout = htobe16(hard_timeout);
}


void
cflowentry::set_cookie(const uint64_t& cookie)
{
	flow_mod->cookie = htobe64(cookie);
}


void
cflowentry::set_cookie_mask(const uint64_t& cookie_mask)
{
	flow_mod->cookie_mask = htobe64(cookie_mask);
}


void
cflowentry::set_priority(const uint16_t& priority)
{
	flow_mod->priority = htobe16(priority);
}


void
cflowentry::set_buffer_id(const uint32_t& buffer_id)
{
	flow_mod->buffer_id = htobe32(buffer_id);
}


void
cflowentry::set_out_port(const uint32_t& out_port)
{
	flow_mod->out_port = htobe32(out_port);
}


void
cflowentry::set_out_group(const uint32_t& out_group)
{
	flow_mod->out_group = htobe32(out_group);
}

void
cflowentry::set_flags(const uint16_t& flags)
{
	flow_mod->flags = htobe16(flags);
}


uint8_t
cflowentry::get_command()
{
	return flow_mod->command;
}


uint8_t
cflowentry::get_table_id()
{
	return flow_mod->table_id;
}


uint16_t
cflowentry::get_idle_timeout()
{
	return be16toh(flow_mod->idle_timeout);
}


uint16_t
cflowentry::get_hard_timeout()
{
	return be16toh(flow_mod->hard_timeout);
}


uint64_t
cflowentry::get_cookie()
{
	return be64toh(flow_mod->cookie);
}


uint64_t
cflowentry::get_cookie_mask()
{
	return be64toh(flow_mod->cookie_mask);
}


uint16_t
cflowentry::get_priority()
{
	return be16toh(flow_mod->priority);
}


uint32_t
cflowentry::get_buffer_id()
{
	return be32toh(flow_mod->buffer_id);
}


uint32_t
cflowentry::get_out_port()
{
	return be32toh(flow_mod->out_port);
}


uint32_t
cflowentry::get_out_group()
{
	return be32toh(flow_mod->out_group);
}


uint16_t
cflowentry::get_flags()
{
	return be16toh(flow_mod->flags);
}


size_t
cflowentry::pack()
{
	size_t instslen = instructions.length();

	size_t ofmatch_len = match.length();

	size_t fm_len = sizeof(struct ofp_flow_mod) - sizeof(ofp_match); // length of generic ofp_flow_mod header without ofp_match

	size_t packlen = fm_len + ofmatch_len + instslen;

	WRITELOG(UNKNOWN, ROFL_DBG, "cflowentry(%p)::pack() [-] instslen:%d matchlen:%d fmgenlen:%d packlen:%d",
			this,
			instslen,
			ofmatch_len,
			fm_len,
			packlen);

	//WRITELOG(UNKNOWN, ROFL_DBG, "cflowentry(%p)::pack() instructions length: %d *this: %s", this, instslen, c_str());

	//WRITELOG(UNKNOWN, ROFL_DBG, "cflowentry(%p)::pack() [0] flow_mod_area: %s", this, flow_mod_area.c_str());


	if (packlen > flow_mod_area.memlen()) // not enough space, resize memory area for flow_mod
	{
		WRITELOG(UNKNOWN, ROFL_DBG, "cflowentry(%p)::pack() resizing flow_mod_area: from %d to %d",
				this, flow_mod_area.memlen(), packlen);
		flow_mod_area.resize(packlen);
	}

	flow_mod = (struct ofp_flow_mod*)flow_mod_area.somem();

	//WRITELOG(UNKNOWN, ROFL_DBG, "cflowentry(%p)::pack() [1] flow_mod_area: %s", this, flow_mod_area.c_str());


	WRITELOG(UNKNOWN, ROFL_DBG, "cflowentry(%p)::pack() [1] flow_mod_area: %s", this, flow_mod_area.c_str());

	struct ofp_match* m = (struct ofp_match*)(flow_mod_area.somem() + fm_len);
	match.pack(m, ofmatch_len);

	WRITELOG(UNKNOWN, ROFL_DBG, "cflowentry(%p)::pack() [2] flow_mod_area: %s", this, flow_mod_area.c_str());

	struct ofp_instruction *insts = (struct ofp_instruction*)(flow_mod_area.somem() + fm_len + ofmatch_len);
	instructions.pack(insts, instslen);

	WRITELOG(UNKNOWN, ROFL_DBG, "cflowentry(%p)::pack() [3] flow_mod_area: %s", this, flow_mod_area.c_str());

	return packlen;
}


void
cflowentry::test()
{
	cmacaddr dl_mask("ff:ff:ff:ff:ff:ff");
	cmacaddr dl_src("00:01:01:01:01:01");
	cmacaddr dl_dst("00:02:02:02:02:02");

	cflowentry fe;

	fe.set_command((uint16_t)OFPFC_ADD);
	fe.set_idle_timeout(5);
	fe.set_hard_timeout(0);

	fe.match.oxmlist[OFPXMT_OFB_IN_PORT] 	= coxmatch_ofb_in_port(1);
	fe.match.oxmlist[OFPXMT_OFB_ETH_TYPE] 	= coxmatch_ofb_eth_type(0x0800);
	fe.match.oxmlist[OFPXMT_OFB_ETH_SRC] 	= coxmatch_ofb_eth_src(dl_src);
	fe.match.oxmlist[OFPXMT_OFB_ETH_DST]	= coxmatch_ofb_eth_dst(dl_dst);

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

