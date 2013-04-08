#include "cofmsg_config.h"

using namespace rofl;

cofmsg_get_config_request::cofmsg_get_config_request(
		uint8_t of_version,
		uint32_t xid) :
	cofmsg(sizeof(struct ofp_header))
{
	set_version(of_version);
	set_length(sizeof(struct ofp_header));
	set_type(OFPT_GET_CONFIG_REQUEST);
	set_xid(xid);
}



cofmsg_get_config_request::cofmsg_get_config_request(
		cmemory *memarea) :
	cofmsg(memarea)
{

}



cofmsg_get_config_request::cofmsg_get_config_request(
		cofmsg_get_config_request const& get_config_request)
{
	*this = get_config_request;
}



cofmsg_get_config_request&
cofmsg_get_config_request::operator= (
		cofmsg_get_config_request const& get_config_request)
{
	if (this == &get_config_request)
		return *this;

	cofmsg::operator =(get_config_request);

	return *this;
}



cofmsg_get_config_request::~cofmsg_get_config_request()
{

}



void
cofmsg_get_config_request::reset()
{
	cofmsg::reset();
}



size_t
cofmsg_get_config_request::length()
{
	return (sizeof(struct ofp_header));
}



void
cofmsg_get_config_request::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::pack(buf, buflen);
}



void
cofmsg_get_config_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_get_config_request::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	switch (get_version()) {
	case OFP10_VERSION: {

	} break;
	case OFP12_VERSION: {

	} break;
	case OFP13_VERSION: {

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}










cofmsg_config::cofmsg_config(
		uint8_t of_version,
		uint8_t type,
		uint32_t xid,
		uint16_t flags,
		uint16_t miss_send_len) :
	cofmsg(sizeof(struct ofp_header))
{
	ofh_switch_config = soframe();

	set_version(of_version);
	set_type(type);
	set_xid(xid);

	switch (get_version()) {
	case OFP10_VERSION: {
		resize(sizeof(struct ofp10_switch_config));
		set_length(sizeof(struct ofp10_switch_config));

		ofh10_switch_config->flags 			= htobe16(flags);
		ofh10_switch_config->miss_send_len	= htobe16(miss_send_len);
	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_switch_config));
		set_length(sizeof(struct ofp12_switch_config));

		ofh12_switch_config->flags 			= htobe16(flags);
		ofh12_switch_config->miss_send_len	= htobe16(miss_send_len);
	} break;
	case OFP13_VERSION: {
		resize(sizeof(struct ofp13_switch_config));
		set_length(sizeof(struct ofp13_switch_config));

		ofh13_switch_config->flags 			= htobe16(flags);
		ofh13_switch_config->miss_send_len	= htobe16(miss_send_len);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_config::cofmsg_config(
		cmemory *memarea) :
	cofmsg(memarea)
{

}



cofmsg_config::cofmsg_config(
		cofmsg_config const& config)
{
	*this = config;
}



cofmsg_config&
cofmsg_config::operator= (
		cofmsg_config const& config)
{
	if (this == &config)
		return *this;

	cofmsg::operator =(config);

	ofh_switch_config = soframe();

	return *this;
}



cofmsg_config::~cofmsg_config()
{

}



void
cofmsg_config::reset()
{
	cofmsg::reset();
}



void
cofmsg_config::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_switch_config = soframe();
}



size_t
cofmsg_config::length() const
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_switch_config));
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_switch_config));
	} break;
	case OFP13_VERSION: {
		return (sizeof(struct ofp13_switch_config));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_config::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP10_VERSION: {
		memcpy(buf, soframe(), framelen());
	} break;
	case OFP12_VERSION: {
		memcpy(buf, soframe(), framelen());
	} break;
	case OFP13_VERSION: {
		memcpy(buf, soframe(), framelen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_config::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_config::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_switch_config = soframe();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_switch_config))
			throw eBadSyntaxTooShort();
	} break;
	case OFP12_VERSION: {
		if (get_length() < sizeof(struct ofp12_switch_config))
			throw eBadSyntaxTooShort();
	} break;
	case OFP13_VERSION: {
		if (get_length() < sizeof(struct ofp13_switch_config))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}




uint16_t
cofmsg_config::get_flags() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_switch_config->flags);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_switch_config->flags);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_switch_config->flags);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_config::set_flags(uint16_t flags)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_switch_config->flags = htobe16(flags);
	} break;
	case OFP12_VERSION: {
		ofh12_switch_config->flags = htobe16(flags);
	} break;
	case OFP13_VERSION: {
		ofh13_switch_config->flags = htobe16(flags);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_config::get_miss_send_len() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_switch_config->miss_send_len);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_switch_config->miss_send_len);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_switch_config->miss_send_len);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_config::set_miss_send_len(uint16_t miss_send_len)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_switch_config->miss_send_len = htobe16(miss_send_len);
	} break;
	case OFP12_VERSION: {
		ofh12_switch_config->miss_send_len = htobe16(miss_send_len);
	} break;
	case OFP13_VERSION: {
		ofh13_switch_config->miss_send_len = htobe16(miss_send_len);
	} break;
	default:
		throw eBadVersion();
	}
}

