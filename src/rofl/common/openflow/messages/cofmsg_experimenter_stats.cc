#include "cofmsg_experimenter_stats.h"

using namespace rofl;





cofmsg_experimenter_stats_request::cofmsg_experimenter_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		uint32_t exp_id,
		uint32_t exp_type,
		cmemory const& body) :
	cofmsg_stats(of_version, xid, 0, flags),
	body(body)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_STATS_REQUEST);
		set_stats_type(openflow10::OFPST_VENDOR);
		resize(sizeof(struct openflow10::ofp_stats_request) + sizeof(struct openflow10::ofp_vendor_stats_header) + body.memlen());
		set_exp_id(exp_id);
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_STATS_REQUEST);
		set_stats_type(openflow12::OFPST_EXPERIMENTER);
		resize(sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_experimenter_stats_header) + body.memlen());
		set_exp_id(exp_id);
		set_exp_type(exp_type);
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_experimenter_stats_request::cofmsg_experimenter_stats_request(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{

}



cofmsg_experimenter_stats_request::cofmsg_experimenter_stats_request(
		cofmsg_experimenter_stats_request const& stats)
{
	*this = stats;
}



cofmsg_experimenter_stats_request&
cofmsg_experimenter_stats_request::operator= (
		cofmsg_experimenter_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	return *this;
}



cofmsg_experimenter_stats_request::~cofmsg_experimenter_stats_request()
{

}



void
cofmsg_experimenter_stats_request::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_experimenter_stats_request::resize(size_t len)
{
	cofmsg::resize(len);

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_exp_stats = soframe() + sizeof(struct openflow10::ofp_stats_request);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_exp_stats = soframe() + sizeof(struct openflow12::ofp_stats_request);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	}
	default:
		throw eNotImplemented();
	}
}



size_t
cofmsg_experimenter_stats_request::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_stats_request) + sizeof(struct openflow10::ofp_vendor_stats_header) + body.memlen());
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_experimenter_stats_header) + body.memlen());
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_experimenter_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow10::ofp_stats_request) + sizeof(struct openflow10::ofp_vendor_stats_header));
		body.pack(buf + sizeof(struct openflow10::ofp_stats_request) + sizeof(struct openflow10::ofp_vendor_stats_header), buflen);
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_experimenter_stats_header));
		body.pack(buf + sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_experimenter_stats_header), buflen);
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_experimenter_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_experimenter_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	body.clear();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow10::ofp_stats_request) + sizeof(struct openflow10::ofp_vendor_stats_header)))
			throw eBadSyntaxTooShort();
		body.unpack(soframe() + sizeof(struct openflow10::ofp_stats_request) + sizeof(struct openflow10::ofp_vendor_stats_header),
				framelen() - sizeof(struct openflow10::ofp_stats_request) - sizeof(struct openflow10::ofp_vendor_stats_header));
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_experimenter_stats_header)))
			throw eBadSyntaxTooShort();
		body.unpack(soframe() + sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_experimenter_stats_header),
				framelen() - sizeof(struct openflow12::ofp_stats_request) - sizeof(struct openflow12::ofp_experimenter_stats_header));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



void
cofmsg_experimenter_stats_request::set_exp_id(uint32_t exp_id)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_exp_stats->vendor = htobe32(exp_id);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_exp_stats->experimenter = htobe32(exp_id);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_experimenter_stats_request::get_exp_id() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_exp_stats->vendor);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_exp_stats->experimenter);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_experimenter_stats_request::set_exp_type(uint32_t exp_type)
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		ofh12_exp_stats->exp_type = htobe32(exp_type);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_experimenter_stats_request::get_exp_type() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_exp_stats->exp_type);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	} break;
	default:
		throw eBadVersion();
	}
}



cmemory&
cofmsg_experimenter_stats_request::get_body()
{
	return body;
}







cofmsg_experimenter_stats_reply::cofmsg_experimenter_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		uint32_t exp_id,
		uint32_t exp_type,
		cmemory const& body) :
	cofmsg_stats(of_version, xid, 0, flags),
	body(body)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_STATS_REPLY);
		set_stats_type(openflow10::OFPST_VENDOR);
		resize(sizeof(struct openflow10::ofp_stats_reply) + sizeof(struct openflow10::ofp_vendor_stats_header) + body.memlen());
		set_exp_id(exp_id);
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_STATS_REPLY);
		set_stats_type(openflow12::OFPST_EXPERIMENTER);
		resize(sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_experimenter_stats_header) + body.memlen());
		set_exp_id(exp_id);
		set_exp_type(exp_type);
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_experimenter_stats_reply::cofmsg_experimenter_stats_reply(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{

}



cofmsg_experimenter_stats_reply::cofmsg_experimenter_stats_reply(
		cofmsg_experimenter_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_experimenter_stats_reply&
cofmsg_experimenter_stats_reply::operator= (
		cofmsg_experimenter_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	return *this;
}



cofmsg_experimenter_stats_reply::~cofmsg_experimenter_stats_reply()
{

}



void
cofmsg_experimenter_stats_reply::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_experimenter_stats_reply::resize(size_t len)
{
	cofmsg::resize(len);

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_exp_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_exp_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	}
	default:
		throw eNotImplemented();
	}
}



size_t
cofmsg_experimenter_stats_reply::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_stats_reply) + sizeof(struct openflow10::ofp_vendor_stats_header) + body.memlen());
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_experimenter_stats_header) + body.memlen());
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_experimenter_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow10::ofp_stats_reply) + sizeof(struct openflow10::ofp_vendor_stats_header));
		body.pack(buf + sizeof(struct openflow10::ofp_stats_reply) + sizeof(struct openflow10::ofp_vendor_stats_header), buflen);
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_experimenter_stats_header));
		body.pack(buf + sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_experimenter_stats_header), buflen);
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_experimenter_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_experimenter_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	body.clear();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow10::ofp_stats_reply) + sizeof(struct openflow10::ofp_vendor_stats_header)))
			throw eBadSyntaxTooShort();
		body.unpack(soframe() + sizeof(struct openflow10::ofp_stats_reply) + sizeof(struct openflow10::ofp_vendor_stats_header),
				framelen() - sizeof(struct openflow10::ofp_stats_reply) - sizeof(struct openflow10::ofp_vendor_stats_header));
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_experimenter_stats_header)))
			throw eBadSyntaxTooShort();
		body.unpack(soframe() + sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_experimenter_stats_header),
				framelen() - sizeof(struct openflow12::ofp_stats_reply) - sizeof(struct openflow12::ofp_experimenter_stats_header));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



void
cofmsg_experimenter_stats_reply::set_exp_id(uint32_t exp_id)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_exp_stats->vendor = htobe32(exp_id);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_exp_stats->experimenter = htobe32(exp_id);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_experimenter_stats_reply::get_exp_id() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_exp_stats->vendor);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_exp_stats->experimenter);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_experimenter_stats_reply::set_exp_type(uint32_t exp_type)
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		ofh12_exp_stats->exp_type = htobe32(exp_type);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_experimenter_stats_reply::get_exp_type() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_exp_stats->exp_type);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	} break;
	default:
		throw eBadVersion();
	}
}



cmemory&
cofmsg_experimenter_stats_reply::get_body()
{
	return body;
}


