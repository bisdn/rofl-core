#include "cofmsg_error.h"

using namespace rofl::openflow;

cofmsg_error::cofmsg_error(
		uint8_t of_version,
		uint32_t xid,
		uint16_t err_type,
		uint16_t err_code,
		uint8_t* data,
		size_t datalen) :
	cofmsg(sizeof(struct openflow::ofp_header)),
	body(0)
{
	size_t len = (datalen > 64) ? 64 : datalen;

	body.assign(data, len);

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		resize(sizeof(struct openflow10::ofp_error_msg) + body.memlen());
		set_type(openflow10::OFPT_ERROR);
		set_length(sizeof(struct openflow10::ofp_error_msg) + body.memlen());
	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct openflow12::ofp_error_msg) + body.memlen());
		set_type(openflow12::OFPT_ERROR);
		set_length(sizeof(struct openflow12::ofp_error_msg) + body.memlen());
	} break;
	case OFP13_VERSION: {
		resize(sizeof(struct openflow13::ofp_error_msg) + body.memlen());
		set_type(openflow13::OFPT_ERROR);
		set_length(sizeof(struct openflow13::ofp_error_msg) + body.memlen());
	} break;
	}

	set_err_type(err_type);
	set_err_code(err_code);
}



cofmsg_error::cofmsg_error(
		cmemory *memarea) :
	cofmsg(memarea)
{
	err_msg = soframe();
}



cofmsg_error::cofmsg_error(
		cofmsg_error const& error)
{
	*this = error;
}



cofmsg_error&
cofmsg_error::operator= (
		cofmsg_error const& error)
{
	if (this == &error)
		return *this;

	cofmsg::operator =(error);

	body = error.body;

	err_msg = soframe();

	return *this;
}



cofmsg_error::~cofmsg_error()
{

}



void
cofmsg_error::reset()
{
	cofmsg::reset();
	body.clear();
}


uint8_t*
cofmsg_error::resize(size_t len)
{
	return (err_msg = cofmsg::resize(len));
}



size_t
cofmsg_error::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_error_msg) + body.memlen());
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_error_msg) + body.memlen());
	} break;
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_error_msg) + body.memlen());
	} break;
	default:
		LOGGING_WARN << "cofmsg_error::length() unsupported OFP version" << std::endl;
		throw eBadVersion();
	}
}



void
cofmsg_error::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::pack(buf, buflen);

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		memcpy(buf + sizeof(struct openflow10::ofp_error_msg), body.somem(), body.memlen());
	} break;
	case openflow12::OFP_VERSION: {
		memcpy(buf + sizeof(struct openflow12::ofp_error_msg), body.somem(), body.memlen());
	} break;
	case openflow13::OFP_VERSION: {
		memcpy(buf + sizeof(struct openflow13::ofp_error_msg), body.somem(), body.memlen());
	} break;
	default:
		LOGGING_WARN << "cofmsg_error::pack() unsupported OFP version" << std::endl;
		throw eBadVersion();
	}
}



void
cofmsg_error::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_error::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	err_msg = soframe();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_error_msg))
			throw eBadSyntaxTooShort();
		if (get_length() > sizeof(struct openflow10::ofp_error_msg)) {
			body.assign(soframe() + sizeof(struct openflow10::ofp_error_msg), framelen() - sizeof(struct openflow10::ofp_error_msg));
		}
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_error_msg))
			throw eBadSyntaxTooShort();
		if (get_length() > sizeof(struct openflow12::ofp_error_msg)) {
			body.assign(soframe() + sizeof(struct openflow12::ofp_error_msg), framelen() - sizeof(struct openflow12::ofp_error_msg));
		}
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_error_msg))
			throw eBadSyntaxTooShort();
		if (get_length() > sizeof(struct openflow13::ofp_error_msg)) {
			body.assign(soframe() + sizeof(struct openflow13::ofp_error_msg), framelen() - sizeof(struct openflow13::ofp_error_msg));
		}
	} break;
	default:
		LOGGING_WARN << "cofmsg_error::validate() unsupported OFP version" << std::endl;
		throw eBadRequestBadVersion();
	}
}



uint16_t
cofmsg_error::get_err_type() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be16toh(err10_msg->type);
	} break;
	case openflow12::OFP_VERSION: {
		return be16toh(err12_msg->type);
	} break;
	case openflow13::OFP_VERSION: {
		return be16toh(err13_msg->type);
	} break;
	default:
		LOGGING_WARN << "cofmsg_error::get_err_type() unsupported OFP version" << std::endl;
		throw eBadVersion();
	}
}



void
cofmsg_error::set_err_type(uint16_t type)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		err10_msg->type = htobe16(type);
	} break;
	case openflow12::OFP_VERSION: {
		err12_msg->type = htobe16(type);
	} break;
	case openflow13::OFP_VERSION: {
		err13_msg->type = htobe16(type);
	} break;
	default:
		LOGGING_WARN << "cofmsg_error::set_err_type() unsupported OFP version" << std::endl;
		throw eBadVersion();
	}
}



uint16_t
cofmsg_error::get_err_code() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be16toh(err10_msg->code);
	} break;
	case openflow12::OFP_VERSION: {
		return be16toh(err12_msg->code);
	} break;
	case openflow13::OFP_VERSION: {
		return be16toh(err13_msg->code);
	} break;
	default:
		LOGGING_WARN << "cofmsg_error::get_err_code() unsupported OFP version" << std::endl;
		throw eBadVersion();
	}
}



void
cofmsg_error::set_err_code(uint16_t code)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		err10_msg->code = htobe16(code);
	} break;
	case openflow12::OFP_VERSION: {
		err12_msg->code = htobe16(code);
	} break;
	case openflow13::OFP_VERSION: {
		err13_msg->code = htobe16(code);
	} break;
	default:
		LOGGING_WARN << "cofmsg_error::set_err_code() unsupported OFP version" << std::endl;
		throw eBadVersion();
	}
}



rofl::cmemory&
cofmsg_error::get_body()
{
	return body;
}


