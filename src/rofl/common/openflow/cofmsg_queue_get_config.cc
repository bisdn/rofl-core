#include "cofmsg_queue_get_config.h"

using namespace rofl;



cofmsg_queue_get_config_request::cofmsg_queue_get_config_request(
		uint8_t of_version,
		uint32_t xid,
		uint32_t port_no) :
	cofmsg(sizeof(struct ofp_header))
{
	ofh_queue_get_config_request = soframe();

	set_version(of_version);
	set_type(OFPT_QUEUE_GET_CONFIG_REQUEST);
	set_xid(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		resize(sizeof(struct ofp10_queue_get_config_request));
		ofh10_queue_get_config_request->port	= htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_queue_get_config_request));
		ofh12_queue_get_config_request->port	= htobe32(port_no);
	} break;
	case OFP13_VERSION: {
		resize(sizeof(struct ofp13_queue_get_config_request));
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
	validate();
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
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_queue_get_config_request));
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_queue_get_config_request));
	} break;
	case OFP13_VERSION: {
		return (sizeof(struct ofp13_queue_get_config_request));
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
	case OFP10_VERSION: {
		if (buflen < (sizeof(struct ofp10_queue_get_config_request)))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp10_queue_get_config_request));
	} break;
	case OFP12_VERSION: {
		if (buflen < (sizeof(struct ofp12_queue_get_config_request)))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp12_queue_get_config_request));
	} break;
	case OFP13_VERSION: {
		if (buflen < (sizeof(struct ofp13_queue_get_config_request)))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp13_queue_get_config_request));
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
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_queue_get_config_request))
			throw eBadSyntaxTooShort();
	} break;
	case OFP12_VERSION: {
		if (get_length() < sizeof(struct ofp12_queue_get_config_request))
			throw eBadSyntaxTooShort();
	} break;
	case OFP13_VERSION: {
		if (get_length() < sizeof(struct ofp13_queue_get_config_request))
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
	case OFP10_VERSION: {
		return be16toh(ofh10_queue_get_config_request->port);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofh12_queue_get_config_request->port);
	} break;
	case OFP13_VERSION: {
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
	case OFP10_VERSION: {
		ofh10_queue_get_config_request->port = htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case OFP12_VERSION: {
		ofh12_queue_get_config_request->port = htobe32(port_no);
	} break;
	case OFP13_VERSION: {
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
		uint8_t *data,
		size_t datalen) :
	cofmsg(sizeof(struct ofp_header)),
	queues(data, datalen)
{
	ofh_queue_get_config_reply = soframe();

	set_version(of_version);
	set_type(OFPT_QUEUE_GET_CONFIG_REPLY);
	set_xid(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		resize(sizeof(struct ofp10_queue_get_config_reply));
		ofh10_queue_get_config_reply->port	= htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_queue_get_config_reply));
		ofh12_queue_get_config_reply->port	= htobe32(port_no);
	} break;
	case OFP13_VERSION: {
		resize(sizeof(struct ofp13_queue_get_config_reply));
		ofh13_queue_get_config_reply->port	= htobe32(port_no);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_queue_get_config_reply::cofmsg_queue_get_config_reply(
		cmemory *memarea) :
	cofmsg(memarea)
{
	validate();
}



cofmsg_queue_get_config_reply::cofmsg_queue_get_config_reply(
		cofmsg_queue_get_config_reply const& queue_get_config)
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
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_queue_get_config_reply) + queues.memlen());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_queue_get_config_reply) + queues.memlen());
	} break;
	case OFP13_VERSION: {
		return (sizeof(struct ofp13_queue_get_config_reply) + queues.memlen());
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
	case OFP10_VERSION: {
		if (buflen < (sizeof(struct ofp10_queue_get_config_reply) + queues.memlen()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp10_queue_get_config_reply));
		memcpy(buf + sizeof(struct ofp10_queue_get_config_reply), queues.somem(), queues.memlen());
	} break;
	case OFP12_VERSION: {
		if (buflen < (sizeof(struct ofp12_queue_get_config_reply) + queues.memlen()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp12_queue_get_config_reply));
		memcpy(buf + sizeof(struct ofp12_queue_get_config_reply), queues.somem(), queues.memlen());
	} break;
	case OFP13_VERSION: {
		if (buflen < (sizeof(struct ofp13_queue_get_config_reply) + queues.memlen()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp13_queue_get_config_reply));
		memcpy(buf + sizeof(struct ofp13_queue_get_config_reply), queues.somem(), queues.memlen());
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

	queues.clear();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_queue_get_config_reply))
			throw eBadSyntaxTooShort();
		queues.assign((uint8_t*)(ofh10_queue_get_config_reply->queues), get_length() - sizeof(struct ofp10_queue_get_config_reply));
	} break;
	case OFP12_VERSION: {
		if (get_length() < sizeof(struct ofp12_queue_get_config_reply))
			throw eBadSyntaxTooShort();
		queues.assign((uint8_t*)(ofh12_queue_get_config_reply->queues), get_length() - sizeof(struct ofp12_queue_get_config_reply));
	} break;
	case OFP13_VERSION: {
		if (get_length() < sizeof(struct ofp13_queue_get_config_reply))
			throw eBadSyntaxTooShort();
		queues.assign((uint8_t*)(ofh13_queue_get_config_reply->queues), get_length() - sizeof(struct ofp13_queue_get_config_reply));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint32_t
cofmsg_queue_get_config_reply::get_port_no() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_queue_get_config_reply->port);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofh12_queue_get_config_reply->port);
	} break;
	case OFP13_VERSION: {
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
	case OFP10_VERSION: {
		ofh10_queue_get_config_reply->port = htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case OFP12_VERSION: {
		ofh12_queue_get_config_reply->port = htobe32(port_no);
	} break;
	case OFP13_VERSION: {
		ofh13_queue_get_config_reply->port = htobe32(port_no);
	} break;
	default:
		throw eBadVersion();
	}
}



cmemory&
cofmsg_queue_get_config_reply::get_queues()
{
	return queues;
}


