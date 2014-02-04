#include "cofmsg_role.h"

using namespace rofl;



cofmsg_role_request::cofmsg_role_request(
		uint8_t of_version,
		uint32_t xid,
		uint32_t role,
		uint64_t generation_id) :
	cofmsg(sizeof(struct openflow::ofp_header))
{
	ofh_role_request = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_ROLE_REQUEST);
		resize(sizeof(struct openflow12::ofp_role_request));
		ofh12_role_request->role			= htobe32(role);
		ofh12_role_request->generation_id	= htobe64(generation_id);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_ROLE_REQUEST);
		resize(sizeof(struct openflow13::ofp_role_request));
		ofh13_role_request->role			= htobe32(role);
		ofh13_role_request->generation_id	= htobe64(generation_id);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_role_request::cofmsg_role_request(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_role_request = soframe();
}



cofmsg_role_request::cofmsg_role_request(
		cofmsg_role_request const& role)
{
	*this = role;
}



cofmsg_role_request&
cofmsg_role_request::operator= (
		cofmsg_role_request const& role)
{
	if (this == &role)
		return *this;

	cofmsg::operator =(role);

	ofh_role_request = soframe();

	return *this;
}



cofmsg_role_request::~cofmsg_role_request()
{

}



void
cofmsg_role_request::reset()
{
	cofmsg::reset();
}



void
cofmsg_role_request::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_role_request = soframe();
}



size_t
cofmsg_role_request::length() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return sizeof(struct openflow12::ofp_role_request);
	} break;
	case openflow13::OFP_VERSION: {
		return sizeof(struct openflow13::ofp_role_request);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_role_request::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct openflow12::ofp_role_request))
			throw eInval();
		memcpy(buf, soframe(), framelen());
	} break;
	case openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct openflow13::ofp_role_request))
			throw eInval();
		memcpy(buf, soframe(), framelen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_role_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_role_request::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_role_request = soframe();

	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_role_request))
			throw eBadSyntaxTooShort();
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_role_request))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint32_t
cofmsg_role_request::get_role() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_role_request->role);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_role_request->role);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_role_request::set_role(uint32_t role)
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		ofh12_role_request->role = htobe32(role);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_role_request->role = htobe32(role);
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
cofmsg_role_request::get_generation_id() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return be64toh(ofh12_role_request->generation_id);
	} break;
	case openflow13::OFP_VERSION: {
		return be64toh(ofh13_role_request->generation_id);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_role_request::set_generation_id(uint64_t generation_id)
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		ofh12_role_request->generation_id = htobe64(generation_id);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_role_request->generation_id = htobe32(generation_id);
	} break;
	default:
		throw eBadVersion();
	}
}









cofmsg_role_reply::cofmsg_role_reply(
		uint8_t of_version,
		uint32_t xid,
		uint32_t role,
		uint64_t generation_id) :
	cofmsg(sizeof(struct openflow::ofp_header))
{
	ofh_role_reply = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_ROLE_REPLY);
		resize(sizeof(struct openflow12::ofp_role_request)); // yes, this struct is used in the reply as well
		ofh12_role_reply->role			= htobe32(role);
		ofh12_role_reply->generation_id	= htobe64(generation_id);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_ROLE_REPLY);
		resize(sizeof(struct openflow13::ofp_role_request)); // yes, this struct is used in the reply as well
		ofh13_role_reply->role			= htobe32(role);
		ofh13_role_reply->generation_id	= htobe64(generation_id);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_role_reply::cofmsg_role_reply(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_role_reply = soframe();
}



cofmsg_role_reply::cofmsg_role_reply(
		cofmsg_role_reply const& role)
{
	*this = role;
}



cofmsg_role_reply&
cofmsg_role_reply::operator= (
		cofmsg_role_reply const& role)
{
	if (this == &role)
		return *this;

	cofmsg::operator =(role);

	ofh_role_reply = soframe();

	return *this;
}



cofmsg_role_reply::~cofmsg_role_reply()
{

}



void
cofmsg_role_reply::reset()
{
	cofmsg::reset();
}



void
cofmsg_role_reply::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_role_reply = soframe();
}



size_t
cofmsg_role_reply::length() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return sizeof(struct openflow12::ofp_role_request); // yes, this struct is used in the reply as well
	} break;
	case openflow13::OFP_VERSION: {
		return sizeof(struct openflow13::ofp_role_request); // yes, this struct is used in the reply as well
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_role_reply::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct openflow12::ofp_role_request)) // yes, this struct is used in the reply as well
			throw eInval();
		memcpy(buf, soframe(), framelen());
	} break;
	case openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct openflow13::ofp_role_request)) // yes, this struct is used in the reply as well
			throw eInval();
		memcpy(buf, soframe(), framelen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_role_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_role_reply::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_role_reply = soframe();

	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_role_request)) // yes, this struct is used in the reply as well
			throw eBadSyntaxTooShort();
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_role_request)) // yes, this struct is used in the reply as well
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint32_t
cofmsg_role_reply::get_role() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_role_reply->role);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_role_reply->role);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_role_reply::set_role(uint32_t role)
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		ofh12_role_reply->role = htobe32(role);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_role_reply->role = htobe32(role);
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
cofmsg_role_reply::get_generation_id() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return be64toh(ofh12_role_reply->generation_id);
	} break;
	case openflow13::OFP_VERSION: {
		return be64toh(ofh13_role_reply->generation_id);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_role_reply::set_generation_id(uint64_t generation_id)
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		ofh12_role_reply->generation_id = htobe64(generation_id);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_role_reply->generation_id = htobe32(generation_id);
	} break;
	default:
		throw eBadVersion();
	}
}



