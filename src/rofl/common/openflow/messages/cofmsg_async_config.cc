#include "cofmsg_async_config.h"

using namespace rofl;

cofmsg_get_async_config_request::cofmsg_get_async_config_request(
		uint8_t of_version,
		uint32_t xid) :
	cofmsg(sizeof(struct rofl::openflow::ofp_header))
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		set_version(of_version);
		set_length(sizeof(struct rofl::openflow::ofp_header));
		set_type(rofl::openflow13::OFPT_GET_ASYNC_REQUEST);
		set_xid(xid);
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofmsg_get_async_config_request::cofmsg_get_async_config_request(
		cmemory *memarea) :
	cofmsg(memarea)
{

}



cofmsg_get_async_config_request::cofmsg_get_async_config_request(
		cofmsg_get_async_config_request const& get_async_config_request)
{
	*this = get_async_config_request;
}



cofmsg_get_async_config_request&
cofmsg_get_async_config_request::operator= (
		cofmsg_get_async_config_request const& get_async_config_request)
{
	if (this == &get_async_config_request)
		return *this;

	cofmsg::operator =(get_async_config_request);

	return *this;
}



cofmsg_get_async_config_request::~cofmsg_get_async_config_request()
{

}



void
cofmsg_get_async_config_request::reset()
{
	cofmsg::reset();
}



size_t
cofmsg_get_async_config_request::length()
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow::ofp_header));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



void
cofmsg_get_async_config_request::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::pack(buf, buflen);
}



void
cofmsg_get_async_config_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_get_async_config_request::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		// nothing to do, ofp header only message, checked in cofmsg::validate() already
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}










cofmsg_get_async_config_reply::cofmsg_get_async_config_reply(
		uint8_t of_version,
		uint32_t xid,
		rofl::openflow::cofasync_config const& async_config) :
				cofmsg(sizeof(struct rofl::openflow::ofp_header)),
				async_config(async_config)
{
	this->async_config.set_version(of_version);

	set_version(of_version);
	set_xid(xid);

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_GET_ASYNC_REPLY);
		set_length(sizeof(struct rofl::openflow13::ofp_async_config));

	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_get_async_config_reply::cofmsg_get_async_config_reply(
		cmemory *memarea) :
	cofmsg(memarea)
{

}



cofmsg_get_async_config_reply::cofmsg_get_async_config_reply(
		cofmsg_get_async_config_reply const& msg)
{
	*this = msg;
}



cofmsg_get_async_config_reply&
cofmsg_get_async_config_reply::operator= (
		cofmsg_get_async_config_reply const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg::operator= (msg);

	async_config = msg.async_config;

	return *this;
}



cofmsg_get_async_config_reply::~cofmsg_get_async_config_reply()
{

}



size_t
cofmsg_get_async_config_reply::length() const
{
	switch (ofh_header->version) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_header) + async_config.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_get_async_config_reply::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_header));
		async_config.pack(buf + sizeof(struct rofl::openflow13::ofp_header), async_config.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_get_async_config_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_get_async_config_reply::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	async_config.clear();

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow13::ofp_async_config))
			throw eBadSyntaxTooShort();

		if (framelen() < sizeof(struct rofl::openflow13::ofp_async_config))
			throw eBadSyntaxTooShort();

		async_config.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_header),
				sizeof(struct rofl::openflow::cofasync_config::async_config_t));

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



















cofmsg_set_async_config::cofmsg_set_async_config(
		uint8_t of_version,
		uint32_t xid,
		rofl::openflow::cofasync_config const& async_config) :
				cofmsg(sizeof(struct rofl::openflow::ofp_header)),
				async_config(async_config)
{
	this->async_config.set_version(of_version);

	set_version(of_version);
	set_xid(xid);

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_GET_ASYNC_REPLY);
		set_length(sizeof(struct rofl::openflow13::ofp_async_config));

	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_set_async_config::cofmsg_set_async_config(
		cmemory *memarea) :
	cofmsg(memarea)
{

}



cofmsg_set_async_config::cofmsg_set_async_config(
		cofmsg_set_async_config const& msg)
{
	*this = msg;
}



cofmsg_set_async_config&
cofmsg_set_async_config::operator= (
		cofmsg_set_async_config const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg::operator= (msg);

	async_config = msg.async_config;

	return *this;
}



cofmsg_set_async_config::~cofmsg_set_async_config()
{

}



size_t
cofmsg_set_async_config::length() const
{
	switch (ofh_header->version) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_header) + async_config.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_set_async_config::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_header));
		async_config.pack(buf + sizeof(struct rofl::openflow13::ofp_header), async_config.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_set_async_config::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_set_async_config::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	async_config.clear();

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow13::ofp_async_config))
			throw eBadSyntaxTooShort();

		if (framelen() < sizeof(struct rofl::openflow13::ofp_async_config))
			throw eBadSyntaxTooShort();

		async_config.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_header),
				sizeof(struct rofl::openflow::cofasync_config::async_config_t));

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}




