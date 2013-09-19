#include "cofmsg_packet_out.h"

using namespace rofl;

cofmsg_packet_out::cofmsg_packet_out(
		uint8_t of_version,
		uint32_t xid,
		uint32_t buffer_id,
		uint32_t in_port,
		cofaclist const& actions,
		uint8_t *data,
		size_t datalen) :
	cofmsg(sizeof(struct ofp_header)),
	actions(actions),
	packet(data, datalen)
{
	ofh_packet_out = soframe();

	set_version(of_version);
	set_type(OFPT_PACKET_OUT);
	set_xid(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		resize(sizeof(struct ofp10_packet_out));
		set_length(length());

		ofh10_packet_out->buffer_id		= htobe32(buffer_id);
		ofh10_packet_out->in_port		= htobe16((uint16_t)(in_port & 0x0000ffff));
		ofh10_packet_out->actions_len	= htobe16(actions.length()); // filled in when method pack() is called

	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_packet_out));
		set_length(length());

		ofh12_packet_out->buffer_id		= htobe32(buffer_id);
		ofh12_packet_out->in_port		= htobe32(in_port);
		ofh12_packet_out->actions_len	= htobe16(actions.length()); // filled in when method pack() is called

	} break;
	case OFP13_VERSION: {
		cofmsg::resize(sizeof(struct ofp12_packet_out));
		set_length(length());

		ofh13_packet_out->buffer_id		= htobe32(buffer_id);
		ofh13_packet_out->in_port		= htobe32(in_port);
		ofh13_packet_out->actions_len	= htobe16(actions.length()); // filled in when method pack() is called

	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_packet_out::cofmsg_packet_out(
		cmemory *memarea) :
	cofmsg(memarea),
	packet((size_t)0)
{
	ofh_packet_out = soframe();
}



cofmsg_packet_out::cofmsg_packet_out(
		cofmsg_packet_out const& packet_out)
{
	*this = packet_out;
}



cofmsg_packet_out&
cofmsg_packet_out::operator= (
		cofmsg_packet_out const& packet_out)
{
	if (this == &packet_out)
		return *this;

	cofmsg::operator =(packet_out);

	ofh_packet_out = soframe();

	actions	= packet_out.actions;
	packet 	= packet_out.packet;

	return *this;
}



cofmsg_packet_out::~cofmsg_packet_out()
{

}



void
cofmsg_packet_out::reset()
{
	cofmsg::reset();
	actions.clear();
	packet.clear();
}



void
cofmsg_packet_out::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_packet_out = soframe();
}



size_t
cofmsg_packet_out::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_packet_out) + actions.length() + packet.framelen());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_packet_out) + actions.length() + packet.framelen());
	} break;
	case OFP13_VERSION: {
		return (sizeof(struct ofp13_packet_out) + actions.length() + packet.framelen());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_out::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_packet_out->actions_len 	= htobe16(actions.length());
	} break;
	case OFP12_VERSION: {
		ofh12_packet_out->actions_len 	= htobe16(actions.length());
	} break;
	case OFP13_VERSION: {
		ofh13_packet_out->actions_len 	= htobe16(actions.length());
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
		memcpy(buf, soframe(), sizeof(struct ofp10_packet_out));
		actions.pack(OFP10_VERSION, (struct ofp_action_header*)(buf + sizeof(struct ofp10_packet_out)), actions.length());
		memcpy(buf + sizeof(struct ofp10_packet_out) + actions.length(), packet.soframe(), packet.framelen());
	} break;
	case OFP12_VERSION: {
		memcpy(buf, soframe(), sizeof(struct ofp12_packet_out));
		actions.pack(OFP10_VERSION, (struct ofp_action_header*)(buf + sizeof(struct ofp12_packet_out)), actions.length());
		memcpy(buf + sizeof(struct ofp12_packet_out) + actions.length(), packet.soframe(), packet.framelen());
	} break;
	case OFP13_VERSION: {
		memcpy(buf, soframe(), sizeof(struct ofp13_packet_out));
		actions.pack(OFP10_VERSION, (struct ofp_action_header*)(buf + sizeof(struct ofp13_packet_out)), actions.length());
		memcpy(buf + sizeof(struct ofp13_packet_out) + actions.length(), packet.soframe(), packet.framelen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_packet_out::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_packet_out::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_packet_out = soframe();

	actions.clear();
	packet.clear();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_packet_out))
			throw eBadSyntaxTooShort();

		if (get_length() < (sizeof(struct ofp10_packet_out) + be16toh(ofh10_packet_out->actions_len)))
			throw eBadSyntaxTooShort();

		actions.unpack(OFP10_VERSION, ofh10_packet_out->actions,
						be16toh(ofh10_packet_out->actions_len));

		if (OFP_NO_BUFFER == get_buffer_id()) {
			packet.unpack(get_in_port(),
						((uint8_t*)ofh10_packet_out) +
							sizeof(struct ofp10_packet_out) +
								be16toh(ofh10_packet_out->actions_len),
								be16toh(ofh10_packet_out->header.length) -
													sizeof(struct ofp10_packet_out) -
														be16toh(ofh10_packet_out->actions_len));
		}
	} break;
	case OFP12_VERSION: {
		if (framelen() < sizeof(struct ofp12_packet_out))
			throw eBadSyntaxTooShort();

		if (get_length() < (sizeof(struct ofp12_packet_out) + be16toh(ofh12_packet_out->actions_len)))
			throw eBadSyntaxTooShort();

		actions.unpack(OFP12_VERSION, ofh12_packet_out->actions,
						be16toh(ofh12_packet_out->actions_len));

		if (OFP_NO_BUFFER == get_buffer_id()) {
			packet.unpack(get_in_port(),
						((uint8_t*)ofh12_packet_out) +
							sizeof(struct ofp12_packet_out) +
								be16toh(ofh12_packet_out->actions_len),
								get_length() - sizeof(struct ofp12_packet_out) -
											be16toh(ofh12_packet_out->actions_len));
		}
	} break;
	case OFP13_VERSION: {
		if (framelen() < sizeof(struct ofp13_packet_out))
			throw eBadSyntaxTooShort();

		if (get_length() < (sizeof(struct ofp13_packet_out) + be16toh(ofh13_packet_out->actions_len)))
			throw eBadSyntaxTooShort();

		actions.unpack(OFP13_VERSION, ofh13_packet_out->actions,
						be16toh(ofh13_packet_out->actions_len));

		if (OFP_NO_BUFFER != get_buffer_id()) {
			packet.unpack(get_in_port(),
						((uint8_t*)ofh13_packet_out) +
							sizeof(struct ofp13_packet_out) +
								be16toh(ofh13_packet_out->actions_len),
								be16toh(ofh13_packet_out->header.length) -
													sizeof(struct ofp13_packet_out) -
														be16toh(ofh13_packet_out->actions_len));
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint32_t
cofmsg_packet_out::get_buffer_id() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be32toh(ofh10_packet_out->buffer_id);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofh12_packet_out->buffer_id);
	} break;
	case OFP13_VERSION: {
		return be32toh(ofh13_packet_out->buffer_id);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_out::set_buffer_id(uint32_t buffer_id)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_packet_out->buffer_id = htobe32(buffer_id);
	} break;
	case OFP12_VERSION: {
		ofh12_packet_out->buffer_id = htobe32(buffer_id);
	} break;
	case OFP13_VERSION: {
		ofh13_packet_out->buffer_id = htobe32(buffer_id);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_packet_out::get_in_port() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (uint32_t)be16toh(ofh10_packet_out->in_port);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofh12_packet_out->in_port);
	} break;
	case OFP13_VERSION: {
		return be32toh(ofh13_packet_out->in_port);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_out::set_in_port(uint32_t in_port)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_packet_out->in_port = htobe16((uint16_t)(in_port & 0x0000ffff));
	} break;
	case OFP12_VERSION: {
		ofh12_packet_out->in_port = htobe32(in_port);
	} break;
	case OFP13_VERSION: {
		ofh13_packet_out->in_port = htobe32(in_port);
	} break;
	default:
		throw eBadVersion();
	}
}



cofaclist&
cofmsg_packet_out::get_actions()
{
	return actions;
}



cpacket&
cofmsg_packet_out::get_packet()
{
	return packet;
}


