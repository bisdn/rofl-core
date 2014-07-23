/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_experimenter.h"

using namespace rofl::openflow;

cofmsg_experimenter::cofmsg_experimenter(
		uint8_t of_version,
		uint32_t xid,
		uint32_t experimenter_id,
		uint32_t experimenter_type,
		uint8_t *data,
		size_t datalen) :
	cofmsg(sizeof(struct rofl::openflow::ofp_header) + datalen),
	body(0)
{
	body.assign(data, datalen);

	ofh_experimenter = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		set_type(rofl::openflow10::OFPT_VENDOR);
		resize(sizeof(struct rofl::openflow10::ofp_vendor_header) + body.memlen());
		ofh10_vendor->vendor				= htobe32(experimenter_id);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_EXPERIMENTER);
		resize(sizeof(struct rofl::openflow12::ofp_experimenter_header) + body.memlen());
		ofh12_experimenter->experimenter	= htobe32(experimenter_id);
		ofh12_experimenter->exp_type		= htobe32(experimenter_type);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_EXPERIMENTER);
		resize(sizeof(struct rofl::openflow13::ofp_experimenter_header) + body.memlen());
		ofh13_experimenter->experimenter	= htobe32(experimenter_id);
		ofh13_experimenter->exp_type		= htobe32(experimenter_type);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_experimenter::cofmsg_experimenter(
		cmemory *memarea) :
	cofmsg(memarea)
{

}



cofmsg_experimenter::cofmsg_experimenter(
		cofmsg_experimenter const& experimenter)
{
	*this = experimenter;
}



cofmsg_experimenter&
cofmsg_experimenter::operator= (
		cofmsg_experimenter const& experimenter)
{
	if (this == &experimenter)
		return *this;

	cofmsg::operator =(experimenter);

	ofh_experimenter = soframe();

	body	= experimenter.body;

	return *this;
}



cofmsg_experimenter::~cofmsg_experimenter()
{

}



void
cofmsg_experimenter::reset()
{
	cofmsg::reset();
}



uint8_t*
cofmsg_experimenter::resize(size_t len)
{
	return (ofh_experimenter = cofmsg::resize(len));
}



size_t
cofmsg_experimenter::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return sizeof(struct rofl::openflow10::ofp_vendor_header) + body.memlen();
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return sizeof(struct rofl::openflow12::ofp_experimenter_header) + body.memlen();
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow13::ofp_experimenter_header) + body.memlen();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_experimenter::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < (sizeof(struct rofl::openflow10::ofp_vendor_header) + body.memlen()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct rofl::openflow10::ofp_vendor_header));
		memcpy(buf + sizeof(struct rofl::openflow10::ofp_vendor_header), body.somem(), body.memlen());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct rofl::openflow12::ofp_experimenter_header) + body.memlen()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct rofl::openflow12::ofp_experimenter_header));
		memcpy(buf + sizeof(struct rofl::openflow12::ofp_experimenter_header), body.somem(), body.memlen());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < (sizeof(struct rofl::openflow13::ofp_experimenter_header) + body.memlen()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_experimenter_header));
		memcpy(buf + sizeof(struct rofl::openflow13::ofp_experimenter_header), body.somem(), body.memlen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_experimenter::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_experimenter::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_experimenter = soframe();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow10::ofp_vendor_header))
			throw eBadSyntaxTooShort();
		body.assign(ofh10_vendor->body, get_length() - sizeof(struct rofl::openflow10::ofp_vendor_header));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow12::ofp_experimenter_header))
			throw eBadSyntaxTooShort();
		body.assign(ofh12_experimenter->body, get_length() - sizeof(struct rofl::openflow12::ofp_experimenter_header));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow13::ofp_experimenter_header))
			throw eBadSyntaxTooShort();
		body.assign(ofh13_experimenter->body, get_length() - sizeof(struct rofl::openflow13::ofp_experimenter_header));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint32_t
cofmsg_experimenter::get_experimenter_id() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return be32toh(ofh10_vendor->vendor);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return be32toh(ofh12_experimenter->experimenter);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return be32toh(ofh13_experimenter->experimenter);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_experimenter::set_experimenter_id(uint32_t exp_id)
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		ofh10_vendor->vendor = htobe32(exp_id);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh12_experimenter->experimenter = htobe32(exp_id);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh13_experimenter->experimenter = htobe32(exp_id);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_experimenter::get_experimenter_type() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return be32toh(ofh12_experimenter->exp_type);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return be32toh(ofh13_experimenter->exp_type);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_experimenter::set_experimenter_type(uint32_t exp_type)
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh12_experimenter->exp_type = htobe32(exp_type);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh13_experimenter->exp_type = htobe32(exp_type);
	} break;
	default:
		throw eBadVersion();
	}
}



rofl::cmemory&
cofmsg_experimenter::get_body()
{
	return body;
}


