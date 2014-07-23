/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_meter_features_stats.h"

using namespace rofl::openflow;

cofmsg_meter_features_stats_request::cofmsg_meter_features_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_flags) :
	cofmsg_stats_request(of_version, xid, 0, stats_flags)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		set_stats_type(rofl::openflow13::OFPMP_METER_FEATURES);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_meter_features_stats_request::cofmsg_meter_features_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{

}



cofmsg_meter_features_stats_request::cofmsg_meter_features_stats_request(
		cofmsg_meter_features_stats_request const& msg)
{
	*this = msg;
}



cofmsg_meter_features_stats_request&
cofmsg_meter_features_stats_request::operator= (
		cofmsg_meter_features_stats_request const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg_stats::operator =(msg);

	return *this;
}



cofmsg_meter_features_stats_request::~cofmsg_meter_features_stats_request()
{

}













cofmsg_meter_features_stats_reply::cofmsg_meter_features_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_flags,
		const rofl::openflow::cofmeter_features_reply& meter_features) :
	cofmsg_stats_reply(of_version, xid, 0, stats_flags),
	meter_features(meter_features)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow13::OFP_VERSION: {
		this->meter_features.set_version(of_version);
		set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);
		set_stats_type(rofl::openflow13::OFPMP_METER_FEATURES);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_meter_features_stats_reply::cofmsg_meter_features_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	meter_features(get_version())
{

}



cofmsg_meter_features_stats_reply::cofmsg_meter_features_stats_reply(
		cofmsg_meter_features_stats_reply const& msg)
{
	*this = msg;
}



cofmsg_meter_features_stats_reply&
cofmsg_meter_features_stats_reply::operator= (
		cofmsg_meter_features_stats_reply const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg_stats::operator= (msg);
	meter_features	= msg.meter_features;

	return *this;
}



cofmsg_meter_features_stats_reply::~cofmsg_meter_features_stats_reply()
{

}



void
cofmsg_meter_features_stats_reply::reset()
{
	cofmsg_stats::reset();
	meter_features.clear();
}



uint8_t*
cofmsg_meter_features_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	return soframe();
}



size_t
cofmsg_meter_features_stats_reply::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + meter_features.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_meter_features_stats_reply::pack(uint8_t *buf, size_t buflen)
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
		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_multipart_reply));

		struct rofl::openflow13::ofp_multipart_reply* req =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;

		meter_features.pack(req->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_meter_features_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_meter_features_stats_reply::validate()
{
	meter_features.clear();
	meter_features.set_version(get_version());

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		if (cofmsg::get_length() < sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			return;
		}
		meter_features.unpack(body.somem(), body.memlen());
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}


