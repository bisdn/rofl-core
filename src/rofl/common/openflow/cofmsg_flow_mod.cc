#include "cofmsg_flow_mod.h"

using namespace rofl;

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
		uint16_t flags) :
	cofmsg(sizeof(struct ofp_header)),
	actions(actions),
	packet(data, datalen)
{
	ofh_flow_mod = soframe();

	set_version(of_version);
	set_type(OFPT_FLOW_MOD);
	set_xid(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		cofmsg::resize(sizeof(struct ofp10_flow_mod));
		set_length(length());

		ofh10_flow_mod->buffer_id		= htobe32(buffer_id);
		ofh10_flow_mod->in_port		= htobe16((uint16_t)(in_port & 0x0000ffff));
		ofh10_flow_mod->actions_len	= htobe16(actions.length()); // filled in when method pack() is called

	} break;
	case OFP12_VERSION: {
		cofmsg::resize(sizeof(struct ofp12_flow_mod));
		set_length(length());

		ofh12_flow_mod->buffer_id		= htobe32(buffer_id);
		ofh12_flow_mod->in_port		= htobe32(in_port);
		ofh12_flow_mod->actions_len	= htobe16(actions.length()); // filled in when method pack() is called

	} break;
	case OFP13_VERSION: {
		cofmsg::resize(sizeof(struct ofp12_flow_mod));
		set_length(length());

		ofh13_flow_mod->buffer_id		= htobe32(buffer_id);
		ofh13_flow_mod->in_port		= htobe32(in_port);
		ofh13_flow_mod->actions_len	= htobe16(actions.length()); // filled in when method pack() is called

	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_flow_mod::cofmsg_flow_mod(
		cmemory *memarea) :
	cofmsg(memarea)
{
	validate();
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

	actions	= flow_mod.actions;
	packet 	= flow_mod.packet;

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
	packet.reset();
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
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_flow_mod) + actions.length() + packet.framelen());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_flow_mod) + actions.length() + packet.framelen());
	} break;
	case OFP13_VERSION: {
		return (sizeof(struct ofp13_flow_mod) + actions.length() + packet.framelen());
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

	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_mod->actions_len 	= htobe16(actions.length());
	} break;
	case OFP12_VERSION: {
		ofh12_flow_mod->actions_len 	= htobe16(actions.length());
	} break;
	case OFP13_VERSION: {
		ofh13_flow_mod->actions_len 	= htobe16(actions.length());
	} break;
	default:
		throw eBadVersion();
	}

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP10_VERSION: {
		memcpy(buf, soframe(), sizeof(struct ofp10_flow_mod));
		actions.pack((struct ofp_action_header*)(buf + sizeof(struct ofp10_flow_mod)), actions.length());
		memcpy(buf + sizeof(struct ofp10_flow_mod) + actions.length(), packet.soframe(), packet.framelen());
	} break;
	case OFP12_VERSION: {
		memcpy(buf, soframe(), sizeof(struct ofp12_flow_mod));
		actions.pack((struct ofp_action_header*)(buf + sizeof(struct ofp12_flow_mod)), actions.length());
		memcpy(buf + sizeof(struct ofp12_flow_mod) + actions.length(), packet.soframe(), packet.framelen());
	} break;
	case OFP13_VERSION: {
		memcpy(buf, soframe(), sizeof(struct ofp13_flow_mod));
		actions.pack((struct ofp_action_header*)(buf + sizeof(struct ofp13_flow_mod)), actions.length());
		memcpy(buf + sizeof(struct ofp13_flow_mod) + actions.length(), packet.soframe(), packet.framelen());
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
	packet.reset();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_flow_mod))
			throw eBadSyntaxTooShort();

		if (get_length() < (sizeof(struct ofp10_flow_mod) + be16toh(ofh10_flow_mod->actions_len)))
			throw eBadSyntaxTooShort();

		actions.unpack(ofh10_flow_mod->actions,
						be16toh(ofh10_flow_mod->actions_len));

		if (OFP_NO_BUFFER != be16toh(ofh10_flow_mod->buffer_id)) {
			packet.unpack((uint32_t)be16toh(ofh10_flow_mod->in_port),
						((uint8_t*)ofh10_flow_mod) +
							sizeof(struct ofp10_flow_mod) +
								be16toh(ofh10_flow_mod->actions_len),
								be16toh(ofh10_flow_mod->header.length) -
													sizeof(struct ofp10_flow_mod) -
														be16toh(ofh10_flow_mod->actions_len));
		}
	} break;
	case OFP12_VERSION: {
		if (framelen() < sizeof(struct ofp12_flow_mod))
			throw eBadSyntaxTooShort();

		if (get_length() < (sizeof(struct ofp12_flow_mod) + be16toh(ofh12_flow_mod->actions_len)))
			throw eBadSyntaxTooShort();

		actions.unpack(ofh12_flow_mod->actions,
						be16toh(ofh12_flow_mod->actions_len));

		if (OFP_NO_BUFFER != be16toh(ofh12_flow_mod->buffer_id)) {
			packet.unpack(be32toh(ofh12_flow_mod->in_port),
						((uint8_t*)ofh12_flow_mod) +
							sizeof(struct ofp12_flow_mod) +
								be16toh(ofh12_flow_mod->actions_len),
								be16toh(ofh12_flow_mod->header.length) -
													sizeof(struct ofp12_flow_mod) -
														be16toh(ofh12_flow_mod->actions_len));
		}
	} break;
	case OFP13_VERSION: {
		if (framelen() < sizeof(struct ofp13_flow_mod))
			throw eBadSyntaxTooShort();

		if (get_length() < (sizeof(struct ofp13_flow_mod) + be16toh(ofh13_flow_mod->actions_len)))
			throw eBadSyntaxTooShort();

		actions.unpack(ofh13_flow_mod->actions,
						be16toh(ofh13_flow_mod->actions_len));

		if (OFP_NO_BUFFER != be16toh(ofh13_flow_mod->buffer_id)) {
			packet.unpack(be32toh(ofh13_flow_mod->in_port),
						((uint8_t*)ofh13_flow_mod) +
							sizeof(struct ofp13_flow_mod) +
								be16toh(ofh13_flow_mod->actions_len),
								be16toh(ofh13_flow_mod->header.length) -
													sizeof(struct ofp13_flow_mod) -
														be16toh(ofh13_flow_mod->actions_len));
		}
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



cofinlist&
cofmsg_flow_mod::get_instructions()
{
	return instructions;
}



cofmatch&
cofmsg_flow_mod::get_match()
{
	return match;
}


