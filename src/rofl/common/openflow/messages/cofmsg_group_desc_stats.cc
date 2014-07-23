/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofmsg_group_desc_stats.h"

using namespace rofl::openflow;

cofmsg_group_desc_stats_request::cofmsg_group_desc_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags) :
	cofmsg_stats_request(of_version, xid, 0, flags)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {


	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REQUEST);
		set_stats_type(rofl::openflow12::OFPST_GROUP_DESC);
		resize(sizeof(struct rofl::openflow12::ofp_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		set_stats_type(rofl::openflow13::OFPMP_GROUP_DESC);
		resize(sizeof(struct rofl::openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_desc_stats_request::cofmsg_group_desc_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{

}



cofmsg_group_desc_stats_request::cofmsg_group_desc_stats_request(
		cofmsg_group_desc_stats_request const& stats)
{
	*this = stats;
}



cofmsg_group_desc_stats_request&
cofmsg_group_desc_stats_request::operator= (
		cofmsg_group_desc_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	return *this;
}



cofmsg_group_desc_stats_request::~cofmsg_group_desc_stats_request()
{

}



void
cofmsg_group_desc_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_group_desc_stats_request::resize(size_t len)
{
	return cofmsg_stats::resize(len);
}



size_t
cofmsg_group_desc_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_group_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_group_stats_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_desc_stats_request::pack(uint8_t *buf, size_t buflen)
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
cofmsg_group_desc_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_desc_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow12::ofp_stats_request)))//NOTE group_desc_stats_request has no body //+ sizeof(struct rofl::openflow12::ofp_group_desc_stats)))
			throw eBadSyntaxTooShort();
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow13::ofp_multipart_request)))//NOTE group_desc_stats_request has no body //+ sizeof(struct rofl::openflow12::ofp_group_desc_stats)))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}





cofmsg_group_desc_stats_reply::cofmsg_group_desc_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		rofl::openflow::cofgroupdescstatsarray const& groupdescstatsarray) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	groupdescstatsarray(groupdescstatsarray)
{
	this->groupdescstatsarray.set_version(of_version);

	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {


	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REPLY);
		set_stats_type(rofl::openflow12::OFPST_GROUP_DESC);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);
		set_stats_type(rofl::openflow13::OFPMP_GROUP_DESC);

	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_desc_stats_reply::cofmsg_group_desc_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	groupdescstatsarray(get_version())
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_desc_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_group_desc_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_desc_stats_reply::cofmsg_group_desc_stats_reply(
		cofmsg_group_desc_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_group_desc_stats_reply&
cofmsg_group_desc_stats_reply::operator= (
		cofmsg_group_desc_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_group_desc_stats = soframe();

	groupdescstatsarray = stats.groupdescstatsarray;

	return *this;
}



cofmsg_group_desc_stats_reply::~cofmsg_group_desc_stats_reply()
{

}



void
cofmsg_group_desc_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_group_desc_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_desc_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_group_desc_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_group_desc_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_reply) + groupdescstatsarray.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + groupdescstatsarray.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_desc_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		groupdescstatsarray.pack(buf + sizeof(struct rofl::openflow12::ofp_stats_reply), groupdescstatsarray.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		groupdescstatsarray.pack(buf + sizeof(struct rofl::openflow13::ofp_multipart_reply), groupdescstatsarray.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_group_desc_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_desc_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	groupdescstatsarray.clear();
	groupdescstatsarray.set_version(get_version());

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow12::ofp_stats_reply)))
			throw eBadSyntaxTooShort();
		groupdescstatsarray.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply), get_length() - sizeof(struct rofl::openflow12::ofp_stats_reply));

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow13::ofp_multipart_reply)))
			throw eBadSyntaxTooShort();
		groupdescstatsarray.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply), get_length() - sizeof(struct rofl::openflow13::ofp_multipart_reply));

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



