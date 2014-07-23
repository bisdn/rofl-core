/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_meter_config_stats.h"

using namespace rofl::openflow;

cofmsg_meter_config_stats_request::cofmsg_meter_config_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_flags,
		uint32_t meter_id) :
	cofmsg_stats_request(of_version, xid, 0, stats_flags),
	mconfig(of_version)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		set_stats_type(rofl::openflow13::OFPMP_METER_CONFIG);
		mconfig.set_meter_id(meter_id);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_meter_config_stats_request::cofmsg_meter_config_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_flags,
		const rofl::openflow::cofmeter_config_request& mconfig) :
	cofmsg_stats_request(of_version, xid, 0, stats_flags),
	mconfig(mconfig)
{
	this->mconfig.set_version(of_version);

	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		set_stats_type(rofl::openflow13::OFPMP_METER_CONFIG);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_meter_config_stats_request::cofmsg_meter_config_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea),
	mconfig(get_version())
{

}



cofmsg_meter_config_stats_request::cofmsg_meter_config_stats_request(
		const cofmsg_meter_config_stats_request& msg)
{
	*this = msg;
}



cofmsg_meter_config_stats_request&
cofmsg_meter_config_stats_request::operator= (
		const cofmsg_meter_config_stats_request& msg)
{
	if (this == &msg)
		return *this;

	cofmsg_stats::operator =(msg);
	mconfig = msg.mconfig;

	return *this;
}



cofmsg_meter_config_stats_request::~cofmsg_meter_config_stats_request()
{

}



size_t
cofmsg_meter_config_stats_request::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request) + mconfig.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_meter_config_stats_request::pack(uint8_t *buf, size_t buflen)
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

		struct rofl::openflow13::ofp_multipart_request* req =
				(struct rofl::openflow13::ofp_multipart_request*)buf;

		mconfig.pack(req->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_meter_config_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_meter_config_stats_request::validate()
{
	mconfig.set_version(get_version());

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		if (cofmsg::get_length() < sizeof(struct rofl::openflow13::ofp_multipart_request)) {
			return;
		}
		mconfig.unpack(body.somem(), body.memlen());
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}













cofmsg_meter_config_stats_reply::cofmsg_meter_config_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_flags,
		const rofl::openflow::cofmeterconfigarray& array) :
	cofmsg_stats_reply(of_version, xid, 0, stats_flags),
	array(array)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow13::OFP_VERSION: {
		this->array.set_version(of_version);
		set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);
		set_stats_type(rofl::openflow13::OFPMP_METER_CONFIG);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_meter_config_stats_reply::cofmsg_meter_config_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	array(get_version())
{

}



cofmsg_meter_config_stats_reply::cofmsg_meter_config_stats_reply(
		cofmsg_meter_config_stats_reply const& msg)
{
	*this = msg;
}



cofmsg_meter_config_stats_reply&
cofmsg_meter_config_stats_reply::operator= (
		cofmsg_meter_config_stats_reply const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg_stats::operator= (msg);
	array	= msg.array;

	return *this;
}



cofmsg_meter_config_stats_reply::~cofmsg_meter_config_stats_reply()
{

}



void
cofmsg_meter_config_stats_reply::reset()
{
	cofmsg_stats::reset();
	array.clear();
}



uint8_t*
cofmsg_meter_config_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	return soframe();
}



size_t
cofmsg_meter_config_stats_reply::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + array.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_meter_config_stats_reply::pack(uint8_t *buf, size_t buflen)
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

		array.pack(req->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_meter_config_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_meter_config_stats_reply::validate()
{
	array.clear();
	array.set_version(get_version());

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		if (cofmsg::get_length() < sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			return;
		}
		array.unpack(body.somem(), body.memlen());
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}


