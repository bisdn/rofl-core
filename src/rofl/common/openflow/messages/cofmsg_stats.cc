/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_stats.h"

using namespace rofl::openflow;

cofmsg_stats::cofmsg_stats(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_type,
		uint16_t stats_flags,
		uint8_t *data,
		size_t datalen) :
	cofmsg(sizeof(struct openflow::ofp_header)),
	body(0)
{
	body.assign(data, datalen);

	ofh_stats_request = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case openflow10::OFP_VERSION: {
		resize(sizeof(struct openflow10::ofp_stats_request) + body.memlen());
		ofh12_stats_request->type			= htobe16(stats_type);
		ofh12_stats_request->flags			= htobe16(stats_flags);
	} break;
	case openflow12::OFP_VERSION: {
		resize(sizeof(struct openflow12::ofp_stats_request) + body.memlen());
		ofh12_stats_request->type			= htobe16(stats_type);
		ofh12_stats_request->flags			= htobe16(stats_flags);
	} break;
	case openflow13::OFP_VERSION: {
		resize(sizeof(struct openflow13::ofp_multipart_request) + body.memlen());
		ofh13_multipart_request->type		= htobe16(stats_type);
		ofh13_multipart_request->flags		= htobe16(stats_flags);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_stats::cofmsg_stats(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_stats_request = soframe();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		body.assign(memarea->somem() + sizeof(struct rofl::openflow10::ofp_stats_request),
					memarea->memlen() - sizeof(struct rofl::openflow10::ofp_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		body.assign(memarea->somem() + sizeof(struct rofl::openflow12::ofp_stats_request),
					memarea->memlen() - sizeof(struct rofl::openflow12::ofp_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		body.assign(memarea->somem() + sizeof(struct rofl::openflow13::ofp_multipart_request),
					memarea->memlen() - sizeof(struct rofl::openflow13::ofp_multipart_request));
	} break;
	default: {
		throw eBadRequestBadVersion();
	};
	}
}



cofmsg_stats::cofmsg_stats(
		cofmsg_stats const& stats)
{
	*this = stats;
}



cofmsg_stats&
cofmsg_stats::operator= (
		cofmsg_stats const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg::operator =(stats);

	ofh_stats_request = soframe();

	body	= stats.body;

	return *this;
}



cofmsg_stats::~cofmsg_stats()
{

}



void
cofmsg_stats::reset()
{
	cofmsg::reset();
}



uint8_t*
cofmsg_stats::resize(size_t len)
{
	cofmsg::resize(len);
	return (ofh_stats_request = soframe());
}



size_t
cofmsg_stats::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return sizeof(struct openflow10::ofp_stats_request) + body.memlen();
	} break;
	case openflow12::OFP_VERSION: {
		return sizeof(struct openflow12::ofp_stats_request) + body.memlen();
	} break;
	case openflow13::OFP_VERSION: {
		return sizeof(struct openflow13::ofp_multipart_request) + body.memlen();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_stats::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow10::ofp_stats_request) + body.memlen()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow10::ofp_stats_request));
		memcpy(buf + sizeof(struct openflow10::ofp_stats_request), body.somem(), body.memlen());
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow12::ofp_stats_request) + body.memlen()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow12::ofp_stats_request));
		memcpy(buf + sizeof(struct openflow12::ofp_stats_request), body.somem(), body.memlen());
	} break;
	case openflow13::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow13::ofp_multipart_request) + body.memlen()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow13::ofp_multipart_request));
		memcpy(buf + sizeof(struct openflow13::ofp_multipart_request), body.somem(), body.memlen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_stats::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	ofh_stats_request = soframe();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		body.assign(buf + sizeof(struct rofl::openflow10::ofp_stats_request),
					buflen - sizeof(struct rofl::openflow10::ofp_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		body.assign(buf + sizeof(struct rofl::openflow12::ofp_stats_request),
					buflen - sizeof(struct rofl::openflow12::ofp_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		body.assign(buf + sizeof(struct rofl::openflow13::ofp_multipart_request),
					buflen - sizeof(struct rofl::openflow13::ofp_multipart_request));
	} break;
	default: {
		throw eBadRequestBadVersion();
	};
	}

	validate();
}



void
cofmsg_stats::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_stats_request = soframe();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_stats_request))
			throw eBadSyntaxTooShort();
		body.assign(ofh10_stats_request->body, get_length() - sizeof(struct openflow10::ofp_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_stats_request))
			throw eBadSyntaxTooShort();
		body.assign(ofh12_stats_request->body, get_length() - sizeof(struct openflow12::ofp_stats_request));
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_multipart_request))
			throw eBadSyntaxTooShort();
		body.assign(ofh13_multipart_request->body, get_length() - sizeof(struct openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint16_t
cofmsg_stats::get_stats_type() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be16toh(ofh10_stats_request->type);
	} break;
	case openflow12::OFP_VERSION: {
		return be16toh(ofh12_stats_request->type);
	} break;
	case openflow13::OFP_VERSION: {
		return be16toh(ofh13_multipart_request->type);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_stats::set_stats_type(uint16_t stats_type)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_stats_request->type = htobe16(stats_type);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_stats_request->type = htobe16(stats_type);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_multipart_request->type = htobe16(stats_type);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_stats::get_stats_flags() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be16toh(ofh10_stats_request->flags);
	} break;
	case openflow12::OFP_VERSION: {
		return be16toh(ofh12_stats_request->flags);
	} break;
	case openflow13::OFP_VERSION: {
		return be16toh(ofh13_multipart_request->flags);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_stats::set_stats_flags(uint16_t stats_flags)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_stats_request->flags = htobe16(stats_flags);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_stats_request->flags = htobe16(stats_flags);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_multipart_request->flags = htobe16(stats_flags);
	} break;
	default:
		throw eBadVersion();
	}
}



rofl::cmemory&
cofmsg_stats::get_body()
{
	return body;
}


