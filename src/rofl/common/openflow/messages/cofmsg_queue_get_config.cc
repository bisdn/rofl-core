#include "cofmsg_queue_get_config.h"

using namespace rofl;



cofmsg_queue_get_config_request::cofmsg_queue_get_config_request(
		uint8_t of_version,
		uint32_t xid,
		uint32_t port_no) :
	cofmsg(sizeof(struct openflow::ofp_header))
{
	ofh_queue_get_config_request = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST);
		resize(sizeof(struct openflow10::ofp_queue_get_config_request));
		ofh10_queue_get_config_request->port	= htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST);
		resize(sizeof(struct openflow12::ofp_queue_get_config_request));
		ofh12_queue_get_config_request->port	= htobe32(port_no);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST);
		resize(sizeof(struct openflow13::ofp_queue_get_config_request));
		ofh13_queue_get_config_request->port	= htobe32(port_no);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_queue_get_config_request::cofmsg_queue_get_config_request(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_queue_get_config_request = soframe();
}



cofmsg_queue_get_config_request::cofmsg_queue_get_config_request(
		cofmsg_queue_get_config_request const& queue_get_config)
{
	*this = queue_get_config;
}



cofmsg_queue_get_config_request&
cofmsg_queue_get_config_request::operator= (
		cofmsg_queue_get_config_request const& queue_get_config)
{
	if (this == &queue_get_config)
		return *this;

	cofmsg::operator =(queue_get_config);

	ofh_queue_get_config_request = soframe();

	return *this;
}



cofmsg_queue_get_config_request::~cofmsg_queue_get_config_request()
{

}



void
cofmsg_queue_get_config_request::reset()
{
	cofmsg::reset();
}



void
cofmsg_queue_get_config_request::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_queue_get_config_request = soframe();
}



size_t
cofmsg_queue_get_config_request::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_queue_get_config_request));
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_queue_get_config_request));
	} break;
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_queue_get_config_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_queue_get_config_request::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow10::ofp_queue_get_config_request)))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow10::ofp_queue_get_config_request));
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow12::ofp_queue_get_config_request)))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow12::ofp_queue_get_config_request));
	} break;
	case openflow13::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow13::ofp_queue_get_config_request)))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow13::ofp_queue_get_config_request));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_queue_get_config_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_queue_get_config_request::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_queue_get_config_request = soframe();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_queue_get_config_request))
			throw eBadSyntaxTooShort();
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_queue_get_config_request))
			throw eBadSyntaxTooShort();
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_queue_get_config_request))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint32_t
cofmsg_queue_get_config_request::get_port_no() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be16toh(ofh10_queue_get_config_request->port);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_queue_get_config_request->port);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_queue_get_config_request->port);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_queue_get_config_request::set_port_no(uint32_t port_no)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_queue_get_config_request->port = htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_queue_get_config_request->port = htobe32(port_no);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_queue_get_config_request->port = htobe32(port_no);
	} break;
	default:
		throw eBadVersion();
	}
}





cofmsg_queue_get_config_reply::cofmsg_queue_get_config_reply(
		uint8_t of_version,
		uint32_t xid,
		uint32_t port_no,
		cofpacket_queue_list const& pql) :
	cofmsg(sizeof(struct openflow::ofp_header)),
	pql(pql)
{
	ofh_queue_get_config_reply = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_QUEUE_GET_CONFIG_REPLY);
		resize(sizeof(struct openflow10::ofp_queue_get_config_reply));
		ofh10_queue_get_config_reply->port	= htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_QUEUE_GET_CONFIG_REPLY);
		resize(sizeof(struct openflow12::ofp_queue_get_config_reply));
		ofh12_queue_get_config_reply->port	= htobe32(port_no);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_QUEUE_GET_CONFIG_REPLY);
		resize(sizeof(struct openflow13::ofp_queue_get_config_reply));
		ofh13_queue_get_config_reply->port	= htobe32(port_no);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_queue_get_config_reply::cofmsg_queue_get_config_reply(
		cmemory *memarea) :
	cofmsg(memarea),
	pql(openflow12::OFP_VERSION)
{
	ofh_queue_get_config_reply = soframe();
	pql = cofpacket_queue_list(get_version());
}



cofmsg_queue_get_config_reply::cofmsg_queue_get_config_reply(
		cofmsg_queue_get_config_reply const& queue_get_config) :
	pql(openflow12::OFP_VERSION)
{
	*this = queue_get_config;
}



cofmsg_queue_get_config_reply&
cofmsg_queue_get_config_reply::operator= (
		cofmsg_queue_get_config_reply const& queue_get_config)
{
	if (this == &queue_get_config)
		return *this;

	cofmsg::operator =(queue_get_config);
	pql = queue_get_config.pql;

	ofh_queue_get_config_reply = soframe();

	return *this;
}



cofmsg_queue_get_config_reply::~cofmsg_queue_get_config_reply()
{

}



void
cofmsg_queue_get_config_reply::reset()
{
	cofmsg::reset();
}



void
cofmsg_queue_get_config_reply::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_queue_get_config_reply = soframe();
}



size_t
cofmsg_queue_get_config_reply::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_queue_get_config_reply) + pql.length());
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_queue_get_config_reply) + pql.length());
	} break;
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_queue_get_config_reply) + pql.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_queue_get_config_reply::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct openflow10::ofp_queue_get_config_reply));
		pql.pack(buf + sizeof(struct openflow10::ofp_queue_get_config_reply), pql.length());
	} break;
	case openflow12::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct openflow12::ofp_queue_get_config_reply));
		pql.pack(buf + sizeof(struct openflow12::ofp_queue_get_config_reply), pql.length());
	} break;
	case openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct openflow13::ofp_queue_get_config_reply));
		pql.pack(buf + sizeof(struct openflow13::ofp_queue_get_config_reply), pql.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_queue_get_config_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_queue_get_config_reply::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_queue_get_config_reply = soframe();

	pql.clear();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_queue_get_config_reply))
			throw eBadSyntaxTooShort();
		pql.unpack((uint8_t*)(ofh10_queue_get_config_reply->queues), get_length() - sizeof(struct openflow10::ofp_queue_get_config_reply));
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_queue_get_config_reply))
			throw eBadSyntaxTooShort();
		pql.unpack((uint8_t*)(ofh12_queue_get_config_reply->queues), get_length() - sizeof(struct openflow12::ofp_queue_get_config_reply));
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_queue_get_config_reply))
			throw eBadSyntaxTooShort();
		pql.unpack((uint8_t*)(ofh13_queue_get_config_reply->queues), get_length() - sizeof(struct openflow13::ofp_queue_get_config_reply));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint32_t
cofmsg_queue_get_config_reply::get_port_no() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be16toh(ofh10_queue_get_config_reply->port);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_queue_get_config_reply->port);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_queue_get_config_reply->port);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_queue_get_config_reply::set_port_no(uint32_t port_no)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_queue_get_config_reply->port = htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_queue_get_config_reply->port = htobe32(port_no);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_queue_get_config_reply->port = htobe32(port_no);
	} break;
	default:
		throw eBadVersion();
	}
}



cofpacket_queue_list&
cofmsg_queue_get_config_reply::get_queues()
{
	return pql;
}


