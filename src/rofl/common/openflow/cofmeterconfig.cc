/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofmeterconfig.h"

using namespace rofl::openflow;

cofmeter_config_request::cofmeter_config_request(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				meter_id(0)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofmeter_config_request::cofmeter_config_request(
		uint8_t of_version,
		uint32_t meter_id) :
				of_version(of_version),
				meter_id(meter_id)
{}



cofmeter_config_request::~cofmeter_config_request()
{}



cofmeter_config_request::cofmeter_config_request(
		const cofmeter_config_request& request)
{
	*this = request;
}



cofmeter_config_request&
cofmeter_config_request::operator= (
		const cofmeter_config_request& request)
{
	if (this == &request)
		return *this;

	of_version 	= request.of_version;
	meter_id	= request.meter_id;

	return *this;
}



const bool
cofmeter_config_request::operator== (
		const cofmeter_config_request& meter_config_request) const
{
	return (meter_id == meter_config_request.meter_id);
}



size_t
cofmeter_config_request::length() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow13::ofp_meter_multipart_request);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_config_request::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_multipart_request *req =
				(struct rofl::openflow13::ofp_meter_multipart_request*)buf;

		req->meter_id	= htobe32(meter_id);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_config_request::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_multipart_request *req =
				(struct rofl::openflow13::ofp_meter_multipart_request*)buf;

		meter_id	= be32toh(req->meter_id);

	} break;
	default:
		throw eBadVersion();
	}
}



cofmeter_config_reply::cofmeter_config_reply(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				flags(0),
				meter_id(0),
				mbands(of_version)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofmeter_config_reply::cofmeter_config_reply(
		uint8_t of_version,
		uint16_t flags,
		uint32_t meter_id,
		const rofl::openflow::cofmeter_bands& mbands) :
				of_version(of_version),
				flags(flags),
				meter_id(meter_id),
				mbands(mbands)
{}



cofmeter_config_reply::~cofmeter_config_reply()
{}



cofmeter_config_reply::cofmeter_config_reply(
		const cofmeter_config_reply& meter_config)
{
	*this = meter_config;
}



cofmeter_config_reply&
cofmeter_config_reply::operator= (
		const cofmeter_config_reply& reply)
{
	if (this == &reply)
		return *this;

	of_version 		= reply.of_version;
	flags			= reply.flags;
	meter_id		= reply.meter_id;
	mbands			= reply.mbands;

	return *this;
}



bool
cofmeter_config_reply::operator== (
		cofmeter_config_reply const& reply)
{
	return ((of_version			== reply.of_version) &&
			(flags				== reply.flags) &&
			(meter_id			== reply.meter_id) &&
			(mbands				== reply.mbands));
}



void
cofmeter_config_reply::clear()
{
	flags			= 0;
	meter_id		= 0;
	mbands.clear();
}



size_t
cofmeter_config_reply::length() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_meter_config) + mbands.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmeter_config_reply::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_config *meter_config =
				(struct rofl::openflow13::ofp_meter_config*)buf;

		meter_config->flags				= htobe16(flags);
		meter_config->meter_id			= htobe32(meter_id);
		meter_config->length			= htobe16(length());

		mbands.pack((uint8_t*)(meter_config->bands), mbands.length());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_config_reply::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		mbands.clear();

		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_config* meter_config =
				(struct rofl::openflow13::ofp_meter_config*)buf;

		if (be16toh(meter_config->length) < sizeof(struct rofl::openflow13::ofp_meter_config)) {
			throw eInval();
		}

		flags			= be16toh(meter_config->flags);
		meter_id		= be32toh(meter_config->meter_id);

		uint16_t mbands_len = be16toh(meter_config->length) - sizeof(struct rofl::openflow13::ofp_meter_config);

		if (mbands_len > 0) {
			mbands.unpack((uint8_t*)(meter_config->bands), mbands_len);
		}

	} break;
	default:
		throw eBadVersion();
	}
}


