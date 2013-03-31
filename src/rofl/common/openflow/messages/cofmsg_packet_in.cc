#include "cofmsg_packet_in.h"

using namespace rofl;

cofmsg_packet_in::cofmsg_packet_in(
		uint8_t of_version,
		uint32_t xid,
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t  reason,
		uint8_t  table_id,
		uint64_t cookie,
		uint16_t in_port, /*OF1.0*/
		cofmatch const& match,
		uint8_t *data,
		size_t datalen) :
	cofmsg(sizeof(struct ofp_header)),
	match(match),
	packet(data, datalen, match.get_in_port())
{
	ofh_packet_in = soframe();

	set_version(of_version);
	set_type(OFPT_PACKET_IN);
	set_xid(xid);

	switch (get_version()) {
	case OFP10_VERSION: {
		resize(OFP10_PACKET_IN_STATIC_HDR_LEN);
		set_length(OFP10_PACKET_IN_STATIC_HDR_LEN);

		ofh10_packet_in->buffer_id				= htobe32(buffer_id);
		ofh10_packet_in->total_len				= htobe16(total_len);
		ofh10_packet_in->reason					= reason;
		ofh10_packet_in->in_port				= htobe16(in_port);
	} break;
	case OFP12_VERSION: {
		resize(OFP12_PACKET_IN_STATIC_HDR_LEN);
		set_length(OFP12_PACKET_IN_STATIC_HDR_LEN);

		ofh12_packet_in->buffer_id				= htobe32(buffer_id);
		ofh12_packet_in->total_len				= htobe16(total_len);
		ofh12_packet_in->reason					= reason;
		ofh12_packet_in->table_id				= table_id;
	} break;
	case OFP13_VERSION: {
		resize(OFP13_PACKET_IN_STATIC_HDR_LEN);
		set_length(OFP13_PACKET_IN_STATIC_HDR_LEN);

		ofh13_packet_in->buffer_id				= htobe32(buffer_id);
		ofh13_packet_in->total_len				= htobe16(total_len);
		ofh13_packet_in->reason					= reason;
		ofh13_packet_in->table_id				= table_id;
		ofh13_packet_in->cookie					= htobe64(cookie);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_packet_in::cofmsg_packet_in(
		cmemory *memarea) :
	cofmsg(memarea)
{
	validate();
}



cofmsg_packet_in::cofmsg_packet_in(
		cofmsg_packet_in const& packet_in)
{
	*this = packet_in;
}



cofmsg_packet_in&
cofmsg_packet_in::operator= (
		cofmsg_packet_in const& packet_in)
{
	if (this == &packet_in)
		return *this;

	cofmsg::operator =(packet_in);

	ofh_packet_in = soframe();

	match 	= packet_in.match;
	packet 	= packet_in.packet;

	return *this;
}



cofmsg_packet_in::~cofmsg_packet_in()
{

}



void
cofmsg_packet_in::reset()
{
	cofmsg::reset();
	match.clear();
	packet.clear();
}



void
cofmsg_packet_in::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_packet_in = soframe();
}



size_t
cofmsg_packet_in::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (OFP10_PACKET_IN_STATIC_HDR_LEN + 2 + packet.framelen());
	} break;
	case OFP12_VERSION: {
		return (OFP12_PACKET_IN_STATIC_HDR_LEN + match.length() + 2 + packet.framelen());
	} break;
	case OFP13_VERSION: {
		return (OFP13_PACKET_IN_STATIC_HDR_LEN + match.length() + 2 + packet.framelen());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_in::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	/*
	 * Please note: +2 magic => provides proper alignment of IPv4 addresses in pin_data as defined by OF spec
	 */
	switch (get_version()) {
	case OFP10_VERSION: {
		memcpy(buf, soframe(), OFP10_PACKET_IN_STATIC_HDR_LEN);
		memcpy(buf + OFP10_PACKET_IN_STATIC_HDR_LEN + 2, packet.soframe(), packet.framelen());
	} break;
	case OFP12_VERSION: {
		memcpy(buf, soframe(), OFP12_PACKET_IN_STATIC_HDR_LEN);
		match.pack((struct ofp12_match*)
				(buf + OFP12_PACKET_IN_STATIC_HDR_LEN), match.length());
		memcpy(buf + OFP12_PACKET_IN_STATIC_HDR_LEN + match.length() + 2, packet.soframe(), packet.framelen());
	} break;
	case OFP13_VERSION: {
		memcpy(buf, soframe(), OFP13_PACKET_IN_STATIC_HDR_LEN);
		match.pack((struct ofp13_match*)
				(buf + OFP13_PACKET_IN_STATIC_HDR_LEN), match.length());
		memcpy(buf + OFP13_PACKET_IN_STATIC_HDR_LEN + match.length() + 2, packet.soframe(), packet.framelen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_packet_in::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_packet_in::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_packet_in = soframe();

	match.clear();
	packet.clear();

	switch (get_version()) {
	case OFP10_VERSION: {
		/*
		 * static part of struct ofp_packet_in also contains static fields from struct ofp_match (i.e. type and length)
		 */
		if (get_length() < OFP10_PACKET_IN_STATIC_HDR_LEN)
			throw eBadSyntaxTooShort();

		/*
		 * set data and datalen variables
		 */
		uint16_t offset = OFP10_PACKET_IN_STATIC_HDR_LEN + 2;

		uint16_t in_port = be16toh(ofh10_packet_in->in_port);

		packet.unpack(in_port, (uint8_t*)(soframe() + offset), framelen() - (offset)); // +2: magic :)

	} break;
	case OFP12_VERSION: {
		/*
		 * static part of struct ofp_packet_in also contains static fields from struct ofp_match (i.e. type and length)
		 */
		if (get_length() < (OFP12_PACKET_IN_STATIC_HDR_LEN + sizeof(struct ofp12_match)))
			throw eBadSyntaxTooShort();

		/*
		 * get variable length struct ofp_match
		 */
		if (be16toh(ofh12_packet_in->match.type) != OFPMT_OXM) // must be extensible match
			throw eBadSyntax();

		/* fixed part outside of struct ofp_match is 16bytes */
		if (be16toh(ofh12_packet_in->match.length) > (framelen() - OFP12_PACKET_IN_STATIC_HDR_LEN))
			throw eBadSyntaxTooShort();

		match.unpack(&(ofh12_packet_in->match), be16toh(ofh12_packet_in->match.length));

		/*
		 * set data and datalen variables
		 */
		uint16_t offset = OFP12_PACKET_IN_STATIC_HDR_LEN + match.length() + 2;

		uint32_t in_port = 0;

		try {
			in_port = match.get_in_port();
		} catch (eOFmatchNotFound& e) {
			in_port = 0;
		}

		packet.unpack(in_port, (uint8_t*)(soframe() + offset), framelen() - (offset)); // +2: magic :)

	} break;
	case OFP13_VERSION: {
		/*
		 * static part of struct ofp_packet_in also contains static fields from struct ofp_match (i.e. type and length)
		 */
		if (get_length() < (OFP13_PACKET_IN_STATIC_HDR_LEN + sizeof(struct ofp13_match)))
			throw eBadSyntaxTooShort();

		/*
		 * get variable length struct ofp_match
		 */
		if (be16toh(ofh13_packet_in->match.type) != OFPMT_OXM) // must be extensible match
			throw eBadSyntax();

		/* fixed part outside of struct ofp_match is 16bytes */
		if (be16toh(ofh13_packet_in->match.length) > (framelen() - OFP13_PACKET_IN_STATIC_HDR_LEN))
			throw eBadSyntaxTooShort();

		match.unpack(&(ofh13_packet_in->match), be16toh(ofh13_packet_in->match.length));

		/*
		 * set data and datalen variables
		 */
		uint16_t offset = OFP13_PACKET_IN_STATIC_HDR_LEN + match.length() + 2;

		uint32_t in_port = 0;

		try {
			in_port = match.get_in_port();
		} catch (eOFmatchNotFound& e) {
			in_port = 0;
		}

		packet.unpack(in_port, (uint8_t*)(soframe() + offset), framelen() - (offset)); // +2: magic :)

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}




uint32_t
cofmsg_packet_in::get_buffer_id() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be32toh(ofh10_packet_in->buffer_id);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofh12_packet_in->buffer_id);
	} break;
	case OFP13_VERSION: {
		return be32toh(ofh13_packet_in->buffer_id);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_in::set_buffer_id(uint32_t buffer_id)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_packet_in->buffer_id = htobe32(buffer_id);
	} break;
	case OFP12_VERSION: {
		ofh12_packet_in->buffer_id = htobe32(buffer_id);
	} break;
	case OFP13_VERSION: {
		ofh13_packet_in->buffer_id = htobe32(buffer_id);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_packet_in::get_total_len() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_packet_in->total_len);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_packet_in->total_len);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_packet_in->total_len);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_in::set_total_len(uint16_t total_len)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_packet_in->total_len = htobe16(total_len);
	} break;
	case OFP12_VERSION: {
		ofh12_packet_in->total_len = htobe16(total_len);
	} break;
	case OFP13_VERSION: {
		ofh13_packet_in->total_len = htobe16(total_len);
	} break;
	default:
		throw eBadVersion();
	}
}



uint8_t
cofmsg_packet_in::get_reason() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (ofh10_packet_in->reason);
	} break;
	case OFP12_VERSION: {
		return (ofh12_packet_in->reason);
	} break;
	case OFP13_VERSION: {
		return (ofh13_packet_in->reason);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_in::set_reason(uint8_t reason)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_packet_in->reason = (reason);
	} break;
	case OFP12_VERSION: {
		ofh12_packet_in->reason = (reason);
	} break;
	case OFP13_VERSION: {
		ofh13_packet_in->reason = (reason);
	} break;
	default:
		throw eBadVersion();
	}
}



uint8_t
cofmsg_packet_in::get_table_id() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return (ofh12_packet_in->table_id);
	} break;
	case OFP13_VERSION: {
		return (ofh13_packet_in->table_id);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_in::set_table_id(uint8_t table_id)
{
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh12_packet_in->table_id = (table_id);
	} break;
	case OFP13_VERSION: {
		ofh13_packet_in->table_id = (table_id);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_packet_in::get_in_port() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_packet_in->in_port);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_in::set_in_port(uint16_t port_no)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_packet_in->in_port = htobe16(port_no);
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
cofmsg_packet_in::get_cookie() const
{
	switch (get_version()) {
	case OFP13_VERSION: {
		return be64toh(ofh13_packet_in->cookie);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_packet_in::set_cookie(uint64_t cookie)
{
	switch (get_version()) {
	case OFP13_VERSION: {
		ofh13_packet_in->cookie = htobe64(cookie);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmatch&
cofmsg_packet_in::get_match()
{
	return match;
}



cpacket&
cofmsg_packet_in::get_packet()
{
	return packet;
}


