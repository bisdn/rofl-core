#include "rofl/common/openflow/messages/cofmsg_table_stats.h"

using namespace rofl;




cofmsg_table_stats_request::cofmsg_table_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags) :
	cofmsg_stats_request(of_version, xid, 0, flags)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow10::OFP_VERSION: {
		set_stats_type(rofl::openflow10::OFPST_TABLE);
		set_type(rofl::openflow10::OFPT_STATS_REQUEST);
		resize(sizeof(struct rofl::openflow10::ofp_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_stats_type(rofl::openflow12::OFPST_TABLE);
		set_type(rofl::openflow12::OFPT_STATS_REQUEST);
		resize(sizeof(struct rofl::openflow12::ofp_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_stats_type(rofl::openflow13::OFPMP_TABLE);
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		resize(sizeof(struct rofl::openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_table_stats_request::cofmsg_table_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{

}



cofmsg_table_stats_request::cofmsg_table_stats_request(
		cofmsg_table_stats_request const& stats)
{
	*this = stats;
}



cofmsg_table_stats_request&
cofmsg_table_stats_request::operator= (
		cofmsg_table_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	return *this;
}



cofmsg_table_stats_request::~cofmsg_table_stats_request()
{

}



void
cofmsg_table_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_table_stats_request::resize(size_t len)
{
	return cofmsg_stats::resize(len);
}



size_t
cofmsg_table_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_table_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_table_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow10::ofp_stats_request))
			throw eBadSyntaxTooShort();
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow12::ofp_stats_request))
			throw eBadSyntaxTooShort();
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow13::ofp_multipart_request))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}





cofmsg_table_stats_reply::cofmsg_table_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		rofl::openflow::coftablestatsarray const& tablestatsarray) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	tablestatsarray(tablestatsarray)
{
	this->tablestatsarray.set_version(of_version);

	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow10::OFP_VERSION: {
		set_stats_type(rofl::openflow10::OFPST_TABLE);
		set_type(rofl::openflow10::OFPT_STATS_REPLY);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_stats_type(rofl::openflow12::OFPST_TABLE);
		set_type(rofl::openflow12::OFPT_STATS_REPLY);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_stats_type(rofl::openflow13::OFPMP_TABLE);
		set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);

	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_table_stats_reply::cofmsg_table_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	tablestatsarray(get_version())
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct rofl::openflow10::ofp_stats_reply);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_table_stats_reply::cofmsg_table_stats_reply(
		cofmsg_table_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_table_stats_reply&
cofmsg_table_stats_reply::operator= (
		cofmsg_table_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_table_stats = soframe();

	tablestatsarray = stats.tablestatsarray;

	return *this;
}



cofmsg_table_stats_reply::~cofmsg_table_stats_reply()
{

}



void
cofmsg_table_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_table_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct rofl::openflow10::ofp_stats_reply);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_table_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_reply) + tablestatsarray.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_reply) + tablestatsarray.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + tablestatsarray.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_table_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		tablestatsarray.pack(buf + sizeof(struct rofl::openflow10::ofp_stats_reply), buflen - sizeof(struct rofl::openflow10::ofp_stats_reply));

	} break;
	case rofl::openflow12::OFP_VERSION: {

		tablestatsarray.pack(buf + sizeof(struct rofl::openflow12::ofp_stats_reply), buflen - sizeof(struct rofl::openflow12::ofp_stats_reply));

	} break;
	case rofl::openflow13::OFP_VERSION: {

		tablestatsarray.pack(buf + sizeof(struct rofl::openflow13::ofp_multipart_reply), buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_table_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	tablestatsarray.clear();
	tablestatsarray.set_version(get_version());

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow10::ofp_stats_reply))
			throw eBadSyntaxTooShort();

		tablestatsarray.unpack(soframe() + sizeof(struct rofl::openflow10::ofp_stats_reply), framelen() - sizeof(struct rofl::openflow10::ofp_stats_reply));

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow12::ofp_stats_reply))
			throw eBadSyntaxTooShort();

		tablestatsarray.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply), framelen() - sizeof(struct rofl::openflow12::ofp_stats_reply));

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow13::ofp_multipart_reply))
			throw eBadSyntaxTooShort();

		tablestatsarray.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply), framelen() - sizeof(struct rofl::openflow13::ofp_multipart_reply));

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}




