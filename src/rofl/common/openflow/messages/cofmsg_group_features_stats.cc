/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_group_features_stats.h"

using namespace rofl::openflow;

cofmsg_group_features_stats_request::cofmsg_group_features_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags) :
	cofmsg_stats_request(of_version, xid, 0, flags)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REQUEST);
		set_stats_type(rofl::openflow12::OFPST_GROUP_FEATURES);
		resize(sizeof(struct rofl::openflow12::ofp_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		set_stats_type(rofl::openflow13::OFPMP_GROUP_FEATURES);
		resize(sizeof(struct rofl::openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_features_stats_request::cofmsg_group_features_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{

}



cofmsg_group_features_stats_request::cofmsg_group_features_stats_request(
		cofmsg_group_features_stats_request const& stats)
{
	*this = stats;
}



cofmsg_group_features_stats_request&
cofmsg_group_features_stats_request::operator= (
		cofmsg_group_features_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	return *this;
}



cofmsg_group_features_stats_request::~cofmsg_group_features_stats_request()
{

}



void
cofmsg_group_features_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_group_features_stats_request::resize(size_t len)
{
	return cofmsg_stats::resize(len);
}



size_t
cofmsg_group_features_stats_request::length() const
{
	switch (get_version()) {
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
cofmsg_group_features_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct rofl::openflow12::ofp_stats_request)))
			throw eInval();
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < (sizeof(struct rofl::openflow13::ofp_multipart_request)))
			throw eInval();
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_group_features_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_features_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow12::ofp_stats_request)))
			throw eBadSyntaxTooShort();
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow13::ofp_multipart_request)))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}





cofmsg_group_features_stats_reply::cofmsg_group_features_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofgroup_features_stats_reply const& group_features_stats) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	group_features_stats(group_features_stats)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REPLY);
		set_stats_type(rofl::openflow12::OFPST_GROUP_FEATURES);
		resize(sizeof(struct rofl::openflow12::ofp_stats_reply) + sizeof(struct rofl::openflow12::ofp_group_features_stats));
		group_features_stats.pack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply), sizeof(struct rofl::openflow12::ofp_group_features_stats));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);
		set_stats_type(rofl::openflow13::OFPMP_GROUP_FEATURES);
		resize(sizeof(struct rofl::openflow13::ofp_multipart_reply) + sizeof(struct rofl::openflow13::ofp_group_features));
		group_features_stats.pack(soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply), sizeof(struct rofl::openflow13::ofp_group_features));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_features_stats_reply::cofmsg_group_features_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	group_features_stats(get_version())
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_features_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_group_features_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_features_stats_reply::cofmsg_group_features_stats_reply(
		cofmsg_group_features_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_group_features_stats_reply&
cofmsg_group_features_stats_reply::operator= (
		cofmsg_group_features_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_group_features_stats = soframe();

	return *this;
}



cofmsg_group_features_stats_reply::~cofmsg_group_features_stats_reply()
{

}



void
cofmsg_group_features_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_group_features_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_features_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_group_features_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_group_features_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_reply) + group_features_stats.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + group_features_stats.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_features_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct rofl::openflow12::ofp_stats_reply) + group_features_stats.length()))
			throw eInval();
		group_features_stats.pack(buf + sizeof(struct rofl::openflow12::ofp_stats_reply), group_features_stats.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < (sizeof(struct rofl::openflow13::ofp_multipart_reply) + group_features_stats.length()))
			throw eInval();
		group_features_stats.pack(buf + sizeof(struct rofl::openflow13::ofp_multipart_reply), group_features_stats.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_group_features_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_features_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		group_features_stats.set_version(rofl::openflow12::OFP_VERSION);
		if (get_length() < (sizeof(struct rofl::openflow12::ofp_stats_request) + sizeof(struct rofl::openflow12::ofp_group_features_stats)))
			throw eBadSyntaxTooShort();
		ofh_group_features_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
		group_features_stats.unpack(ofh_group_features_stats, sizeof(struct rofl::openflow12::ofp_group_features_stats));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		group_features_stats.set_version(rofl::openflow13::OFP_VERSION);
		if (get_length() < (sizeof(struct rofl::openflow13::ofp_multipart_request) + sizeof(struct rofl::openflow13::ofp_group_features)))
			throw eBadSyntaxTooShort();
		ofh_group_features_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
		group_features_stats.unpack(ofh_group_features_stats, sizeof(struct rofl::openflow13::ofp_group_features));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofgroup_features_stats_reply&
cofmsg_group_features_stats_reply::get_group_features_stats()
{
	return group_features_stats;
}


