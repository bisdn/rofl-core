#include "cofmsg_table_features.h"

using namespace rofl;




cofmsg_table_features::cofmsg_table_features(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_flags) :
	cofmsg_stats(of_version, xid, 0, stats_flags),
	tables(of_version)
{
}



cofmsg_table_features::cofmsg_table_features(
		cmemory *memarea) :
	cofmsg_stats(memarea),
	tables(get_version())
{

}



cofmsg_table_features::cofmsg_table_features(
		cofmsg_table_features const& msg)
{
	*this = msg;
}



cofmsg_table_features&
cofmsg_table_features::operator= (
		cofmsg_table_features const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg_stats::operator =(msg);
	tables = msg.tables;

	return *this;
}



cofmsg_table_features::~cofmsg_table_features()
{

}



void
cofmsg_table_features::reset()
{
	cofmsg_stats::reset();
	tables.clear();
}



uint8_t*
cofmsg_table_features::resize(size_t len)
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
cofmsg_table_features::length() const
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
cofmsg_table_features::pack(uint8_t *buf, size_t buflen)
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
cofmsg_table_features::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_features::validate()
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





