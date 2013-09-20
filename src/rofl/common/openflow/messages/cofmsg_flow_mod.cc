#include "cofmsg_flow_mod.h"

using namespace rofl;



cofmsg_flow_mod::cofmsg_flow_mod(
		uint8_t of_version,
		uint32_t xid,
		uint64_t cookie,
		uint8_t  command,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint16_t priority,
		uint32_t buffer_id,
		uint16_t out_port,
		uint16_t flags,
		cofaclist const& actions,
		cofmatch const& match) :
	cofmsg(sizeof(struct ofp_header)),
	instructions(),
	match(match)
{
	ofh_flow_mod = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		set_type(OFPT10_FLOW_MOD);
		resize(OFP10_FLOW_MOD_STATIC_HDR_LEN);

		ofh10_flow_mod->cookie			= htobe64(cookie);
		ofh10_flow_mod->command			= htobe16((uint16_t)command);
		ofh10_flow_mod->idle_timeout	= htobe16(idle_timeout);
		ofh10_flow_mod->hard_timeout	= htobe16(hard_timeout);
		ofh10_flow_mod->priority		= htobe16(priority);
		ofh10_flow_mod->buffer_id		= htobe32(buffer_id);
		ofh10_flow_mod->out_port		= htobe16(out_port);
		ofh10_flow_mod->flags			= htobe16(flags);
	} break;
	default:
		throw eBadVersion();
	}
}


cofmsg_flow_mod::cofmsg_flow_mod(
		uint8_t of_version,
		uint32_t xid,
		uint64_t cookie,
		uint64_t cookie_mask,
		uint8_t table_id,
		uint8_t command,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint16_t priority,
		uint32_t buffer_id,
		uint32_t out_port,
		uint32_t out_group,
		uint16_t flags,
		cofinlist const& _instructions,
		cofmatch const& match) :
	cofmsg(sizeof(struct ofp_header)),
	instructions(_instructions),
	match(match)
{
	ofh_flow_mod = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case OFP12_VERSION: {
		set_type(OFPT12_FLOW_MOD);
		resize(OFP12_FLOW_MOD_STATIC_HDR_LEN);

		ofh12_flow_mod->cookie			= htobe64(cookie);
		ofh12_flow_mod->cookie_mask		= htobe64(cookie_mask);
		ofh12_flow_mod->table_id		= table_id;
		ofh12_flow_mod->command			= command;
		ofh12_flow_mod->idle_timeout	= htobe16(idle_timeout);
		ofh12_flow_mod->hard_timeout	= htobe16(hard_timeout);
		ofh12_flow_mod->priority		= htobe16(priority);
		ofh12_flow_mod->buffer_id		= htobe32(buffer_id);
		ofh12_flow_mod->out_port		= htobe32(out_port);
		ofh12_flow_mod->out_group		= htobe32(out_group);
		ofh12_flow_mod->flags			= htobe16(flags);
	} break;
	case OFP13_VERSION: {
		set_type(OFPT13_FLOW_MOD);
		resize(OFP13_FLOW_MOD_STATIC_HDR_LEN);

		ofh13_flow_mod->cookie			= htobe64(cookie);
		ofh13_flow_mod->cookie_mask		= htobe64(cookie_mask);
		ofh13_flow_mod->table_id		= table_id;
		ofh13_flow_mod->command			= command;
		ofh13_flow_mod->idle_timeout	= htobe16(idle_timeout);
		ofh13_flow_mod->hard_timeout	= htobe16(hard_timeout);
		ofh13_flow_mod->priority		= htobe16(priority);
		ofh13_flow_mod->buffer_id		= htobe32(buffer_id);
		ofh13_flow_mod->out_port		= htobe32(out_port);
		ofh13_flow_mod->out_group		= htobe32(out_group);
		ofh13_flow_mod->flags			= htobe16(flags);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_flow_mod::cofmsg_flow_mod(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_flow_mod = soframe();
}



cofmsg_flow_mod::cofmsg_flow_mod(
		cofmsg_flow_mod const& flow_mod)
{
	*this = flow_mod;
}



cofmsg_flow_mod&
cofmsg_flow_mod::operator= (
		cofmsg_flow_mod const& flow_mod)
{
	if (this == &flow_mod)
		return *this;

	cofmsg::operator =(flow_mod);

	ofh_flow_mod = soframe();

	actions			= flow_mod.actions; // OF1.0 only
	instructions 	= flow_mod.instructions;
	match			= flow_mod.match;

	return *this;
}



cofmsg_flow_mod::~cofmsg_flow_mod()
{

}



void
cofmsg_flow_mod::reset()
{
	cofmsg::reset();
	actions.clear();
	instructions.clear();
	match.clear();
}



void
cofmsg_flow_mod::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_flow_mod = soframe();
}



size_t
cofmsg_flow_mod::length() const
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return (OFP10_FLOW_MOD_STATIC_HDR_LEN + match.length() + actions.length());
	} break;
	case OFP12_VERSION: {
		return (OFP12_FLOW_MOD_STATIC_HDR_LEN + match.length() + instructions.length());
	} break;
	case OFP13_VERSION: {
		return (OFP13_FLOW_MOD_STATIC_HDR_LEN + match.length() + instructions.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP10_VERSION: {
		memcpy(buf, soframe(), OFP10_FLOW_MOD_STATIC_HDR_LEN);
		match.pack((struct ofp10_match*)
				(struct ofp10_match*)(buf + sizeof(struct ofp_header)),
												sizeof(struct ofp10_match));
		actions.pack(OFP10_VERSION, (struct ofp_action_header*)
				(buf + OFP10_FLOW_MOD_STATIC_HDR_LEN), actions.length());
	} break;
	case OFP12_VERSION: {
		memcpy(buf, soframe(), OFP12_FLOW_MOD_STATIC_HDR_LEN);
		match.pack((struct ofp12_match*)
				(buf + OFP12_FLOW_MOD_STATIC_HDR_LEN), match.length());
		instructions.pack(OFP12_VERSION, (struct ofp_instruction*)
				(buf + OFP12_FLOW_MOD_STATIC_HDR_LEN + match.length()), instructions.length());
	} break;
	case OFP13_VERSION: {
		memcpy(buf, soframe(), OFP13_FLOW_MOD_STATIC_HDR_LEN);
		match.pack((struct ofp13_match*)
				(buf + OFP13_FLOW_MOD_STATIC_HDR_LEN), match.length());
		instructions.pack(OFP13_VERSION, (struct ofp_instruction*)
				(buf + OFP13_FLOW_MOD_STATIC_HDR_LEN + match.length()), instructions.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_flow_mod::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_flow_mod::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_flow_mod = soframe();

	actions.clear();
	instructions.clear();
	match.clear();


	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_flow_mod))
			throw eBadSyntaxTooShort();
		match.unpack(&(ofh10_flow_mod->match), sizeof(struct ofp10_match));
		actions.unpack(OFP10_VERSION, ofh10_flow_mod->actions, get_length() - sizeof(struct ofp10_flow_mod));
	} break;
	case OFP12_VERSION: {
		// struct ofp12_flow_mod includes static part of struct ofp12_match (i.e. type and length) !!
		if (get_length() < sizeof(struct ofp12_flow_mod))
			throw eBadSyntaxTooShort();

		/* OFP_FLOW_MOD_STATIC_HDR_LEN = length of generic flow-mod header
		 * according to OpenFlow-spec-1.2 is 48bytes
		 */

		// OFP12_FLOW_MOD_STATIC_HDR_LEN does NOT include static part of struct ofp12_match !!
		if ((be16toh(ofh12_flow_mod->match.length)) > (get_length() - OFP12_FLOW_MOD_STATIC_HDR_LEN))
			throw eBadSyntaxTooShort();

		// stored - OFP_FLOW_MOD_STATIC_HDR_LEN is #bytes for struct ofp_match and array of struct ofp_instructions

		/*
		 * unpack ofp_match structure
		 */
		match.unpack(&(ofh12_flow_mod->match), be16toh(ofh12_flow_mod->match.length));


		// match.length() returns length of struct ofp12_match including padding
		if (get_length() < (OFP12_FLOW_MOD_STATIC_HDR_LEN + match.length()))
			throw eBadSyntaxTooShort();

		/*
		 * unpack instructions list
		 */
		struct ofp_instruction *insts = (struct ofp_instruction*)((uint8_t*)&(ofh12_flow_mod->match) + match.length());
		size_t instslen = get_length() - (OFP12_FLOW_MOD_STATIC_HDR_LEN + match.length());
		instructions.unpack(OFP12_VERSION, insts, instslen);

	} break;
	case OFP13_VERSION: {
		// struct ofp13_flow_mod includes static part of struct ofp13_match (i.e. type and length) !!
		if (get_length() < sizeof(struct ofp13_flow_mod))
			throw eBadSyntaxTooShort();

		/* OFP_FLOW_MOD_STATIC_HDR_LEN = length of generic flow-mod header
		 * according to OpenFlow-spec-1.3 is 48bytes
		 */

		// OFP13_FLOW_MOD_STATIC_HDR_LEN does NOT include static part of struct ofp13_match !!
		if ((be16toh(ofh12_flow_mod->match.length)) > (get_length() - OFP12_FLOW_MOD_STATIC_HDR_LEN))
			throw eBadSyntaxTooShort();

		// stored - OFP_FLOW_MOD_STATIC_HDR_LEN is #bytes for struct ofp_match and array of struct ofp_instructions

		/*
		 * unpack ofp_match structure
		 */
		match.unpack(&(ofh13_flow_mod->match), be16toh(ofh13_flow_mod->match.length));


		// match.length() returns length of struct ofp13_match including padding
		if (get_length() < (OFP13_FLOW_MOD_STATIC_HDR_LEN + match.length()))
			throw eBadSyntaxTooShort();

		/*
		 * unpack instructions list
		 */
		struct ofp_instruction *insts = (struct ofp_instruction*)((uint8_t*)&(ofh13_flow_mod->match) + match.length());
		size_t instslen = get_length() - (OFP13_FLOW_MOD_STATIC_HDR_LEN + match.length());
		instructions.unpack(OFP13_VERSION, insts, instslen);

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint8_t
cofmsg_flow_mod::get_table_id() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return ofh12_flow_mod->table_id;
	} break;
	case OFP13_VERSION: {
		return ofh13_flow_mod->table_id;
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_table_id(uint8_t table_id)
{
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh12_flow_mod->table_id = table_id;
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->table_id = table_id;
	} break;
	default:
		throw eBadVersion();
	}
}



uint8_t
cofmsg_flow_mod::get_command() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (be16toh(ofh10_flow_mod->command) & 0x00ff);
	} break;
	case OFP12_VERSION: {
		return ofh12_flow_mod->command;
	} break;
	case OFP13_VERSION: {
		return ofh13_flow_mod->command;
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_command(uint8_t command)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_mod->command = htobe16((uint16_t)command);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_mod->command = command;
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->command = command;
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
cofmsg_flow_mod::get_cookie() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be64toh(ofh10_flow_mod->cookie);
	} break;
	case OFP12_VERSION: {
		return be64toh(ofh12_flow_mod->cookie);
	} break;
	case OFP13_VERSION: {
		return be64toh(ofh13_flow_mod->cookie);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_cookie(uint64_t cookie)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_mod->cookie = htobe64(cookie);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_mod->cookie = htobe64(cookie);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->cookie = htobe64(cookie);
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
cofmsg_flow_mod::get_cookie_mask() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return be64toh(ofh12_flow_mod->cookie_mask);
	} break;
	case OFP13_VERSION: {
		return be64toh(ofh13_flow_mod->cookie_mask);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_cookie_mask(uint64_t cookie_mask)
{
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh12_flow_mod->cookie_mask = htobe64(cookie_mask);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->cookie_mask = htobe64(cookie_mask);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_flow_mod::get_idle_timeout() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_flow_mod->idle_timeout);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_flow_mod->idle_timeout);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_flow_mod->idle_timeout);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_idle_timeout(uint16_t idle_timeout)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_mod->idle_timeout = htobe16(idle_timeout);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_mod->idle_timeout = htobe16(idle_timeout);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->idle_timeout = htobe16(idle_timeout);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_flow_mod::get_hard_timeout() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_flow_mod->hard_timeout);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_flow_mod->hard_timeout);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_flow_mod->hard_timeout);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_hard_timeout(uint16_t hard_timeout)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_mod->hard_timeout = htobe16(hard_timeout);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_mod->hard_timeout = htobe16(hard_timeout);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->hard_timeout = htobe16(hard_timeout);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_flow_mod::get_priority() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_flow_mod->priority);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_flow_mod->priority);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_flow_mod->priority);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_priority(uint16_t priority)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_mod->priority = htobe16(priority);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_mod->priority = htobe16(priority);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->priority = htobe16(priority);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_flow_mod::get_buffer_id() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be32toh(ofh10_flow_mod->buffer_id);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofh12_flow_mod->buffer_id);
	} break;
	case OFP13_VERSION: {
		return be32toh(ofh13_flow_mod->buffer_id);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_buffer_id(uint32_t buffer_id)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_mod->buffer_id = htobe32(buffer_id);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_mod->buffer_id = htobe32(buffer_id);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->buffer_id = htobe32(buffer_id);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_flow_mod::get_out_port() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (uint32_t)be16toh(ofh10_flow_mod->out_port);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofh12_flow_mod->out_port);
	} break;
	case OFP13_VERSION: {
		return be32toh(ofh13_flow_mod->out_port);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_out_port(uint32_t out_port)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_mod->out_port = htobe16((uint16_t)(out_port & 0x0000ffff));
	} break;
	case OFP12_VERSION: {
		ofh12_flow_mod->out_port = htobe32(out_port);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->out_port = htobe32(out_port);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_flow_mod::get_out_group() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return be32toh(ofh12_flow_mod->out_group);
	} break;
	case OFP13_VERSION: {
		return be32toh(ofh13_flow_mod->out_group);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_out_group(uint32_t out_group)
{
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh12_flow_mod->out_group = htobe32(out_group);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->out_group = htobe32(out_group);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_flow_mod::get_flags() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_flow_mod->flags);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_flow_mod->flags);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_flow_mod->flags);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::set_flags(uint16_t flags)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_mod->flags = htobe16(flags);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_mod->flags = htobe16(flags);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->flags = htobe16(flags);
	} break;
	default:
		throw eBadVersion();
	}
}



cofaclist&
cofmsg_flow_mod::get_actions()
{
	return actions;
}



cofinlist&
cofmsg_flow_mod::get_instructions()
{
	switch (get_version()) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return instructions;
	} break;
	case OFP10_VERSION:
	default:
		throw eBadVersion();
	}
}



cofmatch&
cofmsg_flow_mod::get_match()
{
	return match;
}


