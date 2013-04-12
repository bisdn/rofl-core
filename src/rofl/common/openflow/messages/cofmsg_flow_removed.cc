#include "cofmsg_flow_removed.h"

using namespace rofl;

cofmsg_flow_removed::cofmsg_flow_removed(
		uint8_t of_version,
		uint32_t xid,
		uint64_t cookie,
		uint16_t priority,
		uint8_t  reason,
		uint8_t  table_id,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint64_t packet_count,
		uint64_t byte_count,
		cofmatch const& match) :
	cofmsg(sizeof(struct ofp_header)),
	match(match)
{
	ofh_flow_removed = soframe();

	set_version(of_version);
	set_type(OFPT_FLOW_REMOVED);
	set_xid(xid);

	switch (get_version()) {
	case OFP10_VERSION: {
		resize(OFP10_FLOW_REMOVED_STATIC_HDR_LEN);
		set_length(OFP10_FLOW_REMOVED_STATIC_HDR_LEN);

		ofh10_flow_removed->cookie				= htobe64(cookie);
		ofh10_flow_removed->priority			= htobe16(priority);
		ofh10_flow_removed->reason				= reason;
		ofh10_flow_removed->duration_sec		= htobe32(duration_sec);
		ofh10_flow_removed->duration_nsec		= htobe32(duration_nsec);
		ofh10_flow_removed->idle_timeout		= htobe16(idle_timeout);
		ofh10_flow_removed->packet_count		= htobe64(packet_count);
		ofh10_flow_removed->byte_count			= htobe64(byte_count);
	} break;
	case OFP12_VERSION: {
		resize(OFP12_FLOW_REMOVED_STATIC_HDR_LEN);
		set_length(OFP12_FLOW_REMOVED_STATIC_HDR_LEN);

		ofh12_flow_removed->cookie				= htobe64(cookie);
		ofh12_flow_removed->priority			= htobe16(priority);
		ofh12_flow_removed->reason				= reason;
		ofh12_flow_removed->table_id			= table_id;
		ofh12_flow_removed->duration_sec		= htobe32(duration_sec);
		ofh12_flow_removed->duration_nsec		= htobe32(duration_nsec);
		ofh12_flow_removed->idle_timeout		= htobe16(idle_timeout);
		ofh12_flow_removed->hard_timeout		= htobe16(hard_timeout);
		ofh12_flow_removed->packet_count		= htobe64(packet_count);
		ofh12_flow_removed->byte_count			= htobe64(byte_count);
	} break;
	case OFP13_VERSION: {
		resize(OFP13_FLOW_REMOVED_STATIC_HDR_LEN);
		set_length(OFP13_FLOW_REMOVED_STATIC_HDR_LEN);

		ofh13_flow_removed->cookie				= htobe64(cookie);
		ofh13_flow_removed->priority			= htobe16(priority);
		ofh13_flow_removed->reason				= reason;
		ofh13_flow_removed->table_id			= table_id;
		ofh13_flow_removed->duration_sec		= htobe32(duration_sec);
		ofh13_flow_removed->duration_nsec		= htobe32(duration_nsec);
		ofh13_flow_removed->idle_timeout		= htobe16(idle_timeout);
		ofh13_flow_removed->hard_timeout		= htobe16(hard_timeout);
		ofh13_flow_removed->packet_count		= htobe64(packet_count);
		ofh13_flow_removed->byte_count			= htobe64(byte_count);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_flow_removed::cofmsg_flow_removed(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_flow_removed = soframe();
}



cofmsg_flow_removed::cofmsg_flow_removed(
		cofmsg_flow_removed const& flow_removed)
{
	*this = flow_removed;
}



cofmsg_flow_removed&
cofmsg_flow_removed::operator= (
		cofmsg_flow_removed const& flow_removed)
{
	if (this == &flow_removed)
		return *this;

	cofmsg::operator =(flow_removed);

	ofh_flow_removed = soframe();

	match 	= flow_removed.match;

	return *this;
}



cofmsg_flow_removed::~cofmsg_flow_removed()
{

}



void
cofmsg_flow_removed::reset()
{
	cofmsg::reset();
	match.clear();
}



void
cofmsg_flow_removed::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_flow_removed = soframe();
}



size_t
cofmsg_flow_removed::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return OFP10_FLOW_REMOVED_STATIC_HDR_LEN;
	} break;
	case OFP12_VERSION: {
		return (OFP12_FLOW_REMOVED_STATIC_HDR_LEN + match.length());
	} break;
	case OFP13_VERSION: {
		return (OFP13_FLOW_REMOVED_STATIC_HDR_LEN + match.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofh_header->version) {
	case OFP10_VERSION: {
		memcpy(buf, soframe(), OFP10_FLOW_REMOVED_STATIC_HDR_LEN);

		match.pack((struct ofp10_match*)
				(buf + sizeof(struct ofp_header)), sizeof(struct ofp10_match));
	} break;
	case OFP12_VERSION: {
		memcpy(buf, soframe(), OFP12_FLOW_REMOVED_STATIC_HDR_LEN);

		match.pack((struct ofp12_match*)
				(buf + OFP12_FLOW_REMOVED_STATIC_HDR_LEN), match.length());
	} break;
	case OFP13_VERSION: {
		memcpy(buf, soframe(), OFP13_FLOW_REMOVED_STATIC_HDR_LEN);

		match.pack((struct ofp13_match*)
				(buf + OFP13_FLOW_REMOVED_STATIC_HDR_LEN), match.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_flow_removed::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_flow_removed::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_flow_removed = soframe();

	match.clear();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_flow_removed))
			throw eBadSyntaxTooShort();

		try {
			match.unpack(&(ofh10_flow_removed->match), sizeof(struct ofp10_match));
		} catch (eOFmatchInval& e) {
			throw eBadSyntax();
		}

	} break;
	case OFP12_VERSION: {
		size_t frgenlen = sizeof(struct ofp12_flow_removed) - sizeof(struct ofp12_match);

		if (get_length() < frgenlen)
			throw eBadSyntaxTooShort();

		size_t matchlen = be16toh(ofh12_flow_removed->header.length);

		try  {
			match.unpack(&(ofh12_flow_removed->match), matchlen);
		} catch (eOFmatchInval& e) {
			throw eBadSyntax();
		}

	} break;
	case OFP13_VERSION: {
		size_t frgenlen = sizeof(struct ofp13_flow_removed) - sizeof(struct ofp13_match);

		if (get_length() < frgenlen)
			throw eBadSyntaxTooShort();

		size_t matchlen = be16toh(ofh13_flow_removed->header.length);

		try  {
			match.unpack(&(ofh13_flow_removed->match), matchlen);
		} catch (eOFmatchInval& e) {
			throw eBadSyntax();
		}

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint64_t
cofmsg_flow_removed::get_cookie() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be64toh(ofh10_flow_removed->cookie);
	} break;
	case OFP12_VERSION: {
		return be64toh(ofh12_flow_removed->cookie);
	} break;
	case OFP13_VERSION: {
		return be64toh(ofh13_flow_removed->cookie);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_cookie(uint64_t cookie)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_removed->cookie = htobe64(cookie);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_removed->cookie = htobe64(cookie);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->cookie = htobe64(cookie);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_flow_removed::get_priority() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_flow_removed->priority);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_flow_removed->priority);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_flow_removed->priority);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_priority(uint64_t priority)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_removed->priority = htobe16(priority);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_removed->priority = htobe16(priority);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->priority = htobe16(priority);
	} break;
	default:
		throw eBadVersion();
	}
}



uint8_t
cofmsg_flow_removed::get_reason() const
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return ofh10_flow_removed->reason;
	} break;
	case OFP12_VERSION: {
		return ofh12_flow_removed->reason;
	} break;
	case OFP13_VERSION: {
		return ofh13_flow_removed->reason;
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_reason(uint8_t reason)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_removed->reason = (reason);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_removed->reason = (reason);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->reason = (reason);
	} break;
	default:
		throw eBadVersion();
	}
}



uint8_t
cofmsg_flow_removed::get_table_id() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return ofh12_flow_removed->table_id;
	} break;
	case OFP13_VERSION: {
		return ofh13_flow_removed->table_id;
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_table_id(uint8_t table_id)
{
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh12_flow_removed->table_id = (table_id);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->table_id = (table_id);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_flow_removed::get_duration_sec() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be32toh(ofh10_flow_removed->duration_sec);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofh12_flow_removed->duration_sec);
	} break;
	case OFP13_VERSION: {
		return be32toh(ofh13_flow_removed->duration_sec);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_duration_sec(uint32_t duration_sec)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_removed->duration_sec = htobe32(duration_sec);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_removed->duration_sec = htobe32(duration_sec);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->duration_sec = htobe32(duration_sec);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_flow_removed::get_duration_nsec() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be32toh(ofh10_flow_removed->duration_nsec);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofh12_flow_removed->duration_nsec);
	} break;
	case OFP13_VERSION: {
		return be32toh(ofh13_flow_removed->duration_nsec);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_duration_nsec(uint32_t duration_nsec)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_removed->duration_nsec = htobe32(duration_nsec);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_removed->duration_nsec = htobe32(duration_nsec);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->duration_nsec = htobe32(duration_nsec);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_flow_removed::get_idle_timeout() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_flow_removed->idle_timeout);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_flow_removed->idle_timeout);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_flow_removed->idle_timeout);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_idle_timeout(uint16_t idle_timeout)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_removed->idle_timeout = htobe16(idle_timeout);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_removed->idle_timeout = htobe16(idle_timeout);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->idle_timeout = htobe16(idle_timeout);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_flow_removed::get_hard_timeout() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return be16toh(ofh12_flow_removed->hard_timeout);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_flow_removed->hard_timeout);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_hard_timeout(uint16_t hard_timeout)
{
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh12_flow_removed->hard_timeout = htobe16(hard_timeout);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->hard_timeout = htobe16(hard_timeout);
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
cofmsg_flow_removed::get_packet_count() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be64toh(ofh10_flow_removed->packet_count);
	} break;
	case OFP12_VERSION: {
		return be64toh(ofh12_flow_removed->packet_count);
	} break;
	case OFP13_VERSION: {
		return be64toh(ofh13_flow_removed->packet_count);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_packet_count(uint64_t packet_count)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_removed->packet_count = htobe64(packet_count);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_removed->packet_count = htobe64(packet_count);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->packet_count = htobe64(packet_count);
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
cofmsg_flow_removed::get_byte_count() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be64toh(ofh10_flow_removed->byte_count);
	} break;
	case OFP12_VERSION: {
		return be64toh(ofh12_flow_removed->byte_count);
	} break;
	case OFP13_VERSION: {
		return be64toh(ofh13_flow_removed->byte_count);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_removed::set_byte_count(uint64_t byte_count)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_flow_removed->byte_count = htobe64(byte_count);
	} break;
	case OFP12_VERSION: {
		ofh12_flow_removed->byte_count = htobe64(byte_count);
	} break;
	case OFP13_VERSION: {
		ofh13_flow_removed->byte_count = htobe64(byte_count);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmatch&
cofmsg_flow_removed::get_match()
{
	return match;
}




