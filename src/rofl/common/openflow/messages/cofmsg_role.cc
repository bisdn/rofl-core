#include "rofl/common/openflow/messages/cofmsg_role.h"

using namespace rofl::openflow;

cofmsg_role_request::cofmsg_role_request(
		uint8_t of_version,
		uint32_t xid,
		rofl::openflow::cofrole const& role) :
	cofmsg(sizeof(struct rofl::openflow::ofp_header)),
	role(of_version)
{
	this->role.set_version(of_version);

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_ROLE_REQUEST);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_ROLE_REQUEST);

	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_role_request::cofmsg_role_request(
		cmemory *memarea) :
	cofmsg(memarea),
	role(get_version())
{

}



cofmsg_role_request::cofmsg_role_request(
		cofmsg_role_request const& msg)
{
	*this = msg;
}



cofmsg_role_request&
cofmsg_role_request::operator= (
		cofmsg_role_request const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg::operator= (msg);

	role = msg.role;

	return *this;
}



cofmsg_role_request::~cofmsg_role_request()
{

}



void
cofmsg_role_request::reset()
{
	role.clear();
	cofmsg::reset();
}



size_t
cofmsg_role_request::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_header) + role.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_header) + role.length());
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
	case rofl::openflow12::OFP_VERSION: {

		memcpy(buf, soframe(), sizeof(struct rofl::openflow12::ofp_header));
		role.pack(buf + sizeof(struct rofl::openflow12::ofp_header), role.length());

	} break;
	case rofl::openflow13::OFP_VERSION: {

		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_header));
		role.pack(buf + sizeof(struct rofl::openflow13::ofp_header), role.length());

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

	role.clear();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow12::ofp_role_request))
			throw eBadSyntaxTooShort();

		if (framelen() < sizeof(struct rofl::openflow12::ofp_role_request))
			throw eBadSyntaxTooShort();

		role.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_header),
				sizeof(struct rofl::openflow::cofrole::role_t));

	} break;
	case rofl::openflow13::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow12::ofp_role_request))
			throw eBadSyntaxTooShort();

		if (framelen() < sizeof(struct rofl::openflow12::ofp_role_request))
			throw eBadSyntaxTooShort();

		role.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_header),
				sizeof(struct rofl::openflow::cofrole::role_t));

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}














cofmsg_role_reply::cofmsg_role_reply(
		uint8_t of_version,
		uint32_t xid,
		rofl::openflow::cofrole const& role) :
	cofmsg(sizeof(struct rofl::openflow::ofp_header)),
	role(of_version)
{
	this->role.set_version(of_version);

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_ROLE_REPLY);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_ROLE_REPLY);

	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_role_reply::cofmsg_role_reply(
		cmemory *memarea) :
	cofmsg(memarea),
	role(get_version())
{

}



cofmsg_role_reply::cofmsg_role_reply(
		cofmsg_role_reply const& msg)
{
	*this = msg;
}



cofmsg_role_reply&
cofmsg_role_reply::operator= (
		cofmsg_role_reply const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg::operator= (msg);

	role = msg.role;

	return *this;
}



cofmsg_role_reply::~cofmsg_role_reply()
{

}



void
cofmsg_role_reply::reset()
{
	role.clear();
	cofmsg::reset();
}



size_t
cofmsg_role_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_header) + role.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_header) + role.length());
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
	case rofl::openflow12::OFP_VERSION: {

		memcpy(buf, soframe(), sizeof(struct rofl::openflow12::ofp_header));
		role.pack(buf + sizeof(struct rofl::openflow12::ofp_header), role.length());

	} break;
	case rofl::openflow13::OFP_VERSION: {

		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_header));
		role.pack(buf + sizeof(struct rofl::openflow13::ofp_header), role.length());

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

	role.clear();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow12::ofp_role_request))
			throw eBadSyntaxTooShort();

		if (framelen() < sizeof(struct rofl::openflow12::ofp_role_request))
			throw eBadSyntaxTooShort();

		role.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_header),
				sizeof(struct rofl::openflow::cofrole::role_t));

	} break;
	case rofl::openflow13::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow12::ofp_role_request))
			throw eBadSyntaxTooShort();

		if (framelen() < sizeof(struct rofl::openflow12::ofp_role_request))
			throw eBadSyntaxTooShort();

		role.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_header),
				sizeof(struct rofl::openflow::cofrole::role_t));

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}




