#include "cofmsg_async_config.h"

using namespace rofl;

cofmsg_get_async_config_request::cofmsg_get_async_config_request(
		uint8_t of_version,
		uint32_t xid) :
	cofmsg(sizeof(struct openflow::ofp_header))
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		set_version(of_version);
		set_length(sizeof(struct openflow::ofp_header));
		set_type(openflow13::OFPT_GET_ASYNC_REQUEST);
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
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow::ofp_header));
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
	case openflow13::OFP_VERSION: {
		// nothing to do, ofp header only message, checked in cofmsg::validate() already
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}










cofmsg_get_async_config_reply::cofmsg_get_async_config_reply(
		uint8_t of_version,
		uint32_t xid,
		uint32_t packet_in_mask0,
		uint32_t packet_in_mask1,
		uint32_t port_status_mask0,
		uint32_t port_status_mask1,
		uint32_t flow_removed_mask0,
		uint32_t flow_removed_mask1) :
	cofmsg(sizeof(struct openflow::ofp_header))
{
	ofh_async_config = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_GET_ASYNC_REPLY);
		resize(sizeof(struct openflow13::ofp_async_config));
		set_length(sizeof(struct openflow13::ofp_async_config));

		ofh13_async_config->packet_in_mask[0]  		= htobe32(packet_in_mask0);
		ofh13_async_config->packet_in_mask[1]  		= htobe32(packet_in_mask1);
		ofh13_async_config->port_status_mask[0] 	= htobe32(port_status_mask0);
		ofh13_async_config->port_status_mask[1] 	= htobe32(port_status_mask1);
		ofh13_async_config->flow_removed_mask[0] 	= htobe32(flow_removed_mask0);
		ofh13_async_config->flow_removed_mask[1] 	= htobe32(flow_removed_mask1);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_get_async_config_reply::cofmsg_get_async_config_reply(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_async_config = soframe();
}



cofmsg_get_async_config_reply::cofmsg_get_async_config_reply(
		cofmsg_get_async_config_reply const& config)
{
	*this = config;
}



cofmsg_get_async_config_reply&
cofmsg_get_async_config_reply::operator= (
		cofmsg_get_async_config_reply const& config)
{
	if (this == &config)
		return *this;

	cofmsg::operator =(config);

	ofh_async_config = soframe();

	return *this;
}



cofmsg_get_async_config_reply::~cofmsg_get_async_config_reply()
{

}



void
cofmsg_get_async_config_reply::reset()
{
	cofmsg::reset();
}



void
cofmsg_get_async_config_reply::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_async_config = soframe();
}



size_t
cofmsg_get_async_config_reply::length() const
{
	switch (ofh_header->version) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_async_config));
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
	case openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), framelen());
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

	ofh_async_config = soframe();

	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_async_config))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}




uint32_t
cofmsg_get_async_config_reply::get_packet_in_mask(unsigned int index) const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		return be32toh(ofh13_async_config->packet_in_mask[index]);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_get_async_config_reply::set_packet_in_mask(unsigned int index, uint32_t mask)
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		ofh13_async_config->packet_in_mask[index] = htobe32(mask);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_get_async_config_reply::get_port_status_mask(unsigned int index) const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		return be32toh(ofh13_async_config->port_status_mask[index]);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_get_async_config_reply::set_port_status_mask(unsigned int index, uint32_t mask)
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		ofh13_async_config->port_status_mask[index] = htobe32(mask);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_get_async_config_reply::get_flow_removed_mask(unsigned int index) const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		return be32toh(ofh13_async_config->flow_removed_mask[index]);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_get_async_config_reply::set_flow_removed_mask(unsigned int index, uint32_t mask)
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		ofh13_async_config->flow_removed_mask[index] = htobe32(mask);
	} break;
	default:
		throw eBadVersion();
	}
}



























cofmsg_set_async_config::cofmsg_set_async_config(
		uint8_t of_version,
		uint32_t xid,
		uint32_t packet_in_mask0,
		uint32_t packet_in_mask1,
		uint32_t port_status_mask0,
		uint32_t port_status_mask1,
		uint32_t flow_removed_mask0,
		uint32_t flow_removed_mask1) :
	cofmsg(sizeof(struct openflow::ofp_header))
{
	ofh_async_config = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_SET_ASYNC);
		resize(sizeof(struct openflow13::ofp_async_config));
		set_length(sizeof(struct openflow13::ofp_async_config));

		ofh13_async_config->packet_in_mask[0]  		= htobe32(packet_in_mask0);
		ofh13_async_config->packet_in_mask[1]  		= htobe32(packet_in_mask1);
		ofh13_async_config->port_status_mask[0] 	= htobe32(port_status_mask0);
		ofh13_async_config->port_status_mask[1] 	= htobe32(port_status_mask1);
		ofh13_async_config->flow_removed_mask[0] 	= htobe32(flow_removed_mask0);
		ofh13_async_config->flow_removed_mask[1] 	= htobe32(flow_removed_mask1);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_set_async_config::cofmsg_set_async_config(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_async_config = soframe();
}



cofmsg_set_async_config::cofmsg_set_async_config(
		cofmsg_set_async_config const& config)
{
	*this = config;
}



cofmsg_set_async_config&
cofmsg_set_async_config::operator= (
		cofmsg_set_async_config const& config)
{
	if (this == &config)
		return *this;

	cofmsg::operator =(config);

	ofh_async_config = soframe();

	return *this;
}



cofmsg_set_async_config::~cofmsg_set_async_config()
{

}



void
cofmsg_set_async_config::reset()
{
	cofmsg::reset();
}



void
cofmsg_set_async_config::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_async_config = soframe();
}



size_t
cofmsg_set_async_config::length() const
{
	switch (ofh_header->version) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_async_config));
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
	case openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), framelen());
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

	ofh_async_config = soframe();

	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_async_config))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}




uint32_t
cofmsg_set_async_config::get_packet_in_mask(unsigned int index) const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		return be32toh(ofh13_async_config->packet_in_mask[index]);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_set_async_config::set_packet_in_mask(unsigned int index, uint32_t mask)
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		ofh13_async_config->packet_in_mask[index] = htobe32(mask);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_set_async_config::get_port_status_mask(unsigned int index) const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		return be32toh(ofh13_async_config->port_status_mask[index]);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_set_async_config::set_port_status_mask(unsigned int index, uint32_t mask)
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		ofh13_async_config->port_status_mask[index] = htobe32(mask);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_set_async_config::get_flow_removed_mask(unsigned int index) const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		return be32toh(ofh13_async_config->flow_removed_mask[index]);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_set_async_config::set_flow_removed_mask(unsigned int index, uint32_t mask)
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (index > 1)
			throw eInval();
		ofh13_async_config->flow_removed_mask[index] = htobe32(mask);
	} break;
	default:
		throw eBadVersion();
	}
}




