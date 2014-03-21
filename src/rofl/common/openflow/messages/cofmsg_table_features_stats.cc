#include "rofl/common/openflow/messages/cofmsg_table_features_stats.h"

using namespace rofl::openflow;

cofmsg_table_features_stats_request::cofmsg_table_features_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_flags,
		rofl::openflow::coftables const& tables) :
	cofmsg_stats_request(of_version, xid, 0, stats_flags),
	tables(tables)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow13::OFP_VERSION: {
		this->tables.set_version(of_version);
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		set_stats_type(rofl::openflow13::OFPMP_TABLE_FEATURES);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_table_features_stats_request::cofmsg_table_features_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea),
	tables(get_version())
{

}



cofmsg_table_features_stats_request::cofmsg_table_features_stats_request(
		cofmsg_table_features_stats_request const& msg)
{
	*this = msg;
}



cofmsg_table_features_stats_request&
cofmsg_table_features_stats_request::operator= (
		cofmsg_table_features_stats_request const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg_stats::operator =(msg);
	tables = msg.tables;

	return *this;
}



cofmsg_table_features_stats_request::~cofmsg_table_features_stats_request()
{

}



void
cofmsg_table_features_stats_request::reset()
{
	cofmsg_stats::reset();
	tables.clear();
}



uint8_t*
cofmsg_table_features_stats_request::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		ofh_ofhu.ofhu_generic = soframe() + sizeof(struct rofl::openflow13::ofp_table_features);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_table_features_stats_request::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request) + tables.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_table_features_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length()) {
		throw eInval();
	}

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_multipart_request));
		buf += sizeof(struct rofl::openflow13::ofp_multipart_request);
		buflen -= sizeof(struct rofl::openflow13::ofp_multipart_request);
		tables.pack(buf, buflen);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_table_features_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_features_stats_request::validate()
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		if (cofmsg::get_length() < sizeof(struct rofl::openflow13::ofp_multipart_request)) {
			return;
		}
		tables.unpack(body.somem(), body.memlen());
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}











cofmsg_table_features_stats_reply::cofmsg_table_features_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_flags,
		rofl::openflow::coftables const tables) :
	cofmsg_stats_reply(of_version, xid, 0, stats_flags),
	tables(tables)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow13::OFP_VERSION: {
		this->tables.set_version(of_version);
		set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);
		set_stats_type(rofl::openflow13::OFPMP_TABLE_FEATURES);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_table_features_stats_reply::cofmsg_table_features_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	tables(get_version())
{

}



cofmsg_table_features_stats_reply::cofmsg_table_features_stats_reply(
		cofmsg_table_features_stats_reply const& msg)
{
	*this = msg;
}



cofmsg_table_features_stats_reply&
cofmsg_table_features_stats_reply::operator= (
		cofmsg_table_features_stats_reply const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg_stats::operator =(msg);
	tables = msg.tables;

	return *this;
}



cofmsg_table_features_stats_reply::~cofmsg_table_features_stats_reply()
{

}



void
cofmsg_table_features_stats_reply::reset()
{
	cofmsg_stats::reset();
	tables.clear();
}



uint8_t*
cofmsg_table_features_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		ofh_ofhu.ofhu_generic = soframe() + sizeof(struct rofl::openflow13::ofp_table_features);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_table_features_stats_reply::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request) + tables.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_table_features_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length()) {
		throw eInval();
	}

	set_length(length());

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_multipart_request));
		buf += sizeof(struct rofl::openflow13::ofp_multipart_request);
		buflen -= sizeof(struct rofl::openflow13::ofp_multipart_request);
		tables.pack(buf, buflen);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_table_features_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_features_stats_reply::validate()
{
	tables.clear();
	tables.set_version(get_version());

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		if (cofmsg::get_length() < sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			return;
		}
		tables.unpack(body.somem(), body.memlen());
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}


