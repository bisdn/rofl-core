/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_packet_out.h"

using namespace rofl::openflow;

cofmsg_packet_out::cofmsg_packet_out(
		uint8_t of_version,
		uint32_t xid,
		uint32_t buffer_id,
		uint32_t in_port,
		cofactions const& actions,
		uint8_t *data,
		size_t datalen) :
	cofmsg(sizeof(struct rofl::openflow::ofp_header)),
	actions(actions),
	packet(of_version, data, datalen, in_port, false)
{
	ofh_packet_out = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		set_type(rofl::openflow10::OFPT_PACKET_OUT);
		resize(sizeof(struct rofl::openflow10::ofp_packet_out));
		set_length(length());

		ofh10_packet_out->buffer_id		= htobe32(buffer_id);
		ofh10_packet_out->in_port		= htobe16((uint16_t)(in_port & 0x0000ffff));
		ofh10_packet_out->actions_len	= htobe16(actions.length()); // filled in when method pack() is called

	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_PACKET_OUT);
		resize(sizeof(struct rofl::openflow12::ofp_packet_out));
		set_length(length());

		ofh12_packet_out->buffer_id		= htobe32(buffer_id);
		ofh12_packet_out->in_port		= htobe32(in_port);
		ofh12_packet_out->actions_len	= htobe16(actions.length()); // filled in when method pack() is called

	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_PACKET_OUT);
		resize(sizeof(struct rofl::openflow13::ofp_packet_out));
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
	actions(get_version()),
	packet(get_version(), (size_t)0)
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



uint8_t*
cofmsg_packet_out::resize(size_t len)
{
	return (ofh_packet_out = cofmsg::resize(len));
}



size_t
cofmsg_packet_out::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_packet_out) + actions.length() + packet.framelen());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_packet_out) + actions.length() + packet.framelen());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_packet_out) + actions.length() + packet.framelen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_packet_out::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		ofh10_packet_out->actions_len 	= htobe16(actions.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh12_packet_out->actions_len 	= htobe16(actions.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
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
	case rofl::openflow10::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct rofl::openflow10::ofp_packet_out));
		actions.pack(buf + sizeof(struct rofl::openflow10::ofp_packet_out), actions.length());
		memcpy(buf + sizeof(struct rofl::openflow10::ofp_packet_out) + actions.length(), packet.soframe(), packet.framelen());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct rofl::openflow12::ofp_packet_out));
		actions.pack(buf + sizeof(struct rofl::openflow12::ofp_packet_out), actions.length());
		memcpy(buf + sizeof(struct rofl::openflow12::ofp_packet_out) + actions.length(), packet.soframe(), packet.framelen());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_packet_out));
		actions.pack(buf + sizeof(struct rofl::openflow13::ofp_packet_out), actions.length());
		memcpy(buf + sizeof(struct rofl::openflow13::ofp_packet_out) + actions.length(), packet.soframe(), packet.framelen());
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
	case rofl::openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow10::ofp_packet_out))
			throw eBadSyntaxTooShort();

		if (get_length() < (sizeof(struct rofl::openflow10::ofp_packet_out) + be16toh(ofh10_packet_out->actions_len)))
			throw eBadSyntaxTooShort();

		actions.unpack((uint8_t*)ofh10_packet_out->actions,
						be16toh(ofh10_packet_out->actions_len));

		if (rofl::openflow10::OFP_NO_BUFFER == get_buffer_id()) {
			packet.unpack(get_in_port(),
						((uint8_t*)ofh10_packet_out) +
							sizeof(struct rofl::openflow10::ofp_packet_out) +
								be16toh(ofh10_packet_out->actions_len),
								be16toh(ofh10_packet_out->header.length) -
													sizeof(struct rofl::openflow10::ofp_packet_out) -
														be16toh(ofh10_packet_out->actions_len));
		}
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (framelen() < sizeof(struct rofl::openflow12::ofp_packet_out))
			throw eBadSyntaxTooShort();

		if (get_length() < (sizeof(struct rofl::openflow12::ofp_packet_out) + be16toh(ofh12_packet_out->actions_len)))
			throw eBadSyntaxTooShort();

		actions.unpack((uint8_t*)ofh12_packet_out->actions,
						be16toh(ofh12_packet_out->actions_len));

		if (rofl::openflow12::OFP_NO_BUFFER == get_buffer_id()) {
			packet.unpack(get_in_port(),
						((uint8_t*)ofh12_packet_out) +
							sizeof(struct rofl::openflow12::ofp_packet_out) +
								be16toh(ofh12_packet_out->actions_len),
								get_length() - sizeof(struct rofl::openflow12::ofp_packet_out) -
											be16toh(ofh12_packet_out->actions_len));
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (framelen() < sizeof(struct rofl::openflow13::ofp_packet_out))
			throw eBadSyntaxTooShort();

		if (get_length() < (sizeof(struct rofl::openflow13::ofp_packet_out) + be16toh(ofh13_packet_out->actions_len)))
			throw eBadSyntaxTooShort();

		actions.unpack((uint8_t*)ofh13_packet_out->actions,
						be16toh(ofh13_packet_out->actions_len));

		if (rofl::openflow13::OFP_NO_BUFFER == get_buffer_id()) {
			packet.unpack(get_in_port(),
						((uint8_t*)ofh13_packet_out) +
							sizeof(struct rofl::openflow13::ofp_packet_out) +
								be16toh(ofh13_packet_out->actions_len),
								be16toh(ofh13_packet_out->header.length) -
													sizeof(struct rofl::openflow13::ofp_packet_out) -
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
	case rofl::openflow10::OFP_VERSION: {
		return be32toh(ofh10_packet_out->buffer_id);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return be32toh(ofh12_packet_out->buffer_id);
	} break;
	case rofl::openflow13::OFP_VERSION: {
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
	case rofl::openflow10::OFP_VERSION: {
		ofh10_packet_out->buffer_id = htobe32(buffer_id);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh12_packet_out->buffer_id = htobe32(buffer_id);
	} break;
	case rofl::openflow13::OFP_VERSION: {
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
	case rofl::openflow10::OFP_VERSION: {
		return (uint32_t)be16toh(ofh10_packet_out->in_port);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return be32toh(ofh12_packet_out->in_port);
	} break;
	case rofl::openflow13::OFP_VERSION: {
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
	case rofl::openflow10::OFP_VERSION: {
		ofh10_packet_out->in_port = htobe16((uint16_t)(in_port & 0x0000ffff));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh12_packet_out->in_port = htobe32(in_port);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh13_packet_out->in_port = htobe32(in_port);
	} break;
	default:
		throw eBadVersion();
	}
}



cofactions&
cofmsg_packet_out::set_actions()
{
	return actions;
}



cofactions const&
cofmsg_packet_out::get_actions() const
{
	return actions;
}



rofl::cpacket&
cofmsg_packet_out::set_packet()
{
	return packet;
}



rofl::cpacket const&
cofmsg_packet_out::get_packet() const
{
	return packet;
}


