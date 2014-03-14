#include "cofmsg_group_stats.h"

using namespace rofl;




cofmsg_group_stats_request::cofmsg_group_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofgroup_stats_request const& group_stats) :
	cofmsg_stats_request(of_version, xid, 0, flags),
	group_stats(group_stats)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REQUEST);
		set_stats_type(rofl::openflow12::OFPST_GROUP);
		resize(sizeof(struct rofl::openflow12::ofp_stats_request) + sizeof(struct rofl::openflow12::ofp_group_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		set_stats_type(rofl::openflow13::OFPMP_GROUP);
		resize(sizeof(struct rofl::openflow13::ofp_multipart_request) + sizeof(struct rofl::openflow13::ofp_group_stats_request));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_stats_request::cofmsg_group_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_request);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_request);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_stats_request::cofmsg_group_stats_request(
		cofmsg_group_stats_request const& stats)
{
	*this = stats;
}



cofmsg_group_stats_request&
cofmsg_group_stats_request::operator= (
		cofmsg_group_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_group_stats = soframe();

	group_stats = stats.group_stats;

	return *this;
}



cofmsg_group_stats_request::~cofmsg_group_stats_request()
{

}



void
cofmsg_group_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_group_stats_request::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_request);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_request);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_group_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_request) + sizeof(struct rofl::openflow12::ofp_group_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request) + sizeof(struct rofl::openflow13::ofp_group_stats_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();


	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		group_stats.pack(buf + sizeof(struct rofl::openflow12::ofp_stats_request), sizeof(struct rofl::openflow12::ofp_group_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		group_stats.pack(buf + sizeof(struct rofl::openflow13::ofp_multipart_request), sizeof(struct rofl::openflow13::ofp_group_stats_request));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_group_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow12::ofp_stats_request) + sizeof(struct rofl::openflow12::ofp_group_stats_request)))
			throw eBadSyntaxTooShort();
		group_stats.set_version(rofl::openflow12::OFP_VERSION);
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_request);
		group_stats.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_request), sizeof(struct rofl::openflow12::ofp_group_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow13::ofp_multipart_request) + sizeof(struct rofl::openflow13::ofp_group_stats_request)))
			throw eBadSyntaxTooShort();
		group_stats.set_version(rofl::openflow13::OFP_VERSION);
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_request);
		group_stats.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_multipart_request), sizeof(struct rofl::openflow13::ofp_group_stats_request));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofgroup_stats_request&
cofmsg_group_stats_request::set_group_stats()
{
	return group_stats;
}



cofgroup_stats_request const&
cofmsg_group_stats_request::get_group_stats() const
{
	return group_stats;
}





cofmsg_group_stats_reply::cofmsg_group_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		rofl::openflow::cofgroups const& groups) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	groups(groups)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REPLY);
		set_stats_type(rofl::openflow12::OFPST_GROUP);
		resize(length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);
		set_stats_type(rofl::openflow13::OFPMP_GROUP);
		resize(length());
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_stats_reply::cofmsg_group_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea)
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_stats_reply::cofmsg_group_stats_reply(
		cofmsg_group_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_group_stats_reply&
cofmsg_group_stats_reply::operator= (
		cofmsg_group_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_group_stats = soframe();

	return *this;
}



cofmsg_group_stats_reply::~cofmsg_group_stats_reply()
{

}



void
cofmsg_group_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_group_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_group_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_reply) + groups.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + groups.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {

		groups.pack(buf + sizeof(struct rofl::openflow12::ofp_stats_reply), buflen - sizeof(struct rofl::openflow12::ofp_stats_reply));

	} break;
	case rofl::openflow13::OFP_VERSION: {

		groups.pack(buf + sizeof(struct rofl::openflow13::ofp_multipart_reply), buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_group_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	groups.clear();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow12::ofp_stats_reply)))
			throw eBadSyntaxTooShort();

		groups.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply), framelen() - sizeof(struct rofl::openflow12::ofp_stats_reply));

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow13::ofp_multipart_reply) + sizeof(struct rofl::openflow13::ofp_group_stats)))
			throw eBadSyntaxTooShort();

		groups.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply), framelen() - sizeof(struct rofl::openflow13::ofp_multipart_reply));

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



