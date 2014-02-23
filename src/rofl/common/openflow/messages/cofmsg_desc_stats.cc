#include "cofmsg_desc_stats.h"

using namespace rofl;





cofmsg_desc_stats_request::cofmsg_desc_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags) :
	cofmsg_stats_request(of_version, xid, 0, flags)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_STATS_REQUEST);
		set_stats_type(openflow10::OFPST_DESC);
		resize(sizeof(struct openflow10::ofp_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_STATS_REQUEST);
		set_stats_type(openflow12::OFPST_DESC);
		resize(sizeof(struct openflow12::ofp_stats_request));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_desc_stats_request::cofmsg_desc_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{

}



cofmsg_desc_stats_request::cofmsg_desc_stats_request(
		cofmsg_desc_stats_request const& stats)
{
	*this = stats;
}



cofmsg_desc_stats_request&
cofmsg_desc_stats_request::operator= (
		cofmsg_desc_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats_request::operator =(stats);

	return *this;
}



cofmsg_desc_stats_request::~cofmsg_desc_stats_request()
{

}



void
cofmsg_desc_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_desc_stats_request::resize(size_t len)
{
	return cofmsg_stats::resize(len);
}



size_t
cofmsg_desc_stats_request::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_stats_request));
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
cofmsg_desc_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow10::ofp_stats_request)))
			throw eInval();
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow12::ofp_stats_request)))
			throw eInval();
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
cofmsg_desc_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_desc_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow10::ofp_stats_request)))
			throw eBadSyntaxTooShort();
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow12::ofp_stats_request)))
			throw eBadSyntaxTooShort();
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}








cofmsg_desc_stats_reply::cofmsg_desc_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofdesc_stats_reply const& desc_stats) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	desc_stats(desc_stats)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_STATS_REPLY);
		set_stats_type(openflow10::OFPST_DESC);
		resize(sizeof(struct openflow10::ofp_stats_reply) + sizeof(struct openflow10::ofp_desc_stats));
		desc_stats.pack(soframe() + sizeof(struct openflow10::ofp_stats_reply), sizeof(struct openflow10::ofp_desc_stats));
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_STATS_REPLY);
		set_stats_type(openflow12::OFPST_DESC);
		resize(sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_desc_stats));
		desc_stats.pack(soframe() + sizeof(struct openflow12::ofp_stats_reply), sizeof(struct openflow12::ofp_desc_stats));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_desc_stats_reply::cofmsg_desc_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	desc_stats(get_version())
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_desc_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_desc_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_desc_stats_reply::cofmsg_desc_stats_reply(
		cofmsg_desc_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_desc_stats_reply&
cofmsg_desc_stats_reply::operator= (
		cofmsg_desc_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats_reply::operator =(stats);

	ofh_desc_stats = soframe();

	return *this;
}



cofmsg_desc_stats_reply::~cofmsg_desc_stats_reply()
{

}



void
cofmsg_desc_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_desc_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_desc_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_desc_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_desc_stats_reply::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_stats_reply) + desc_stats.length());
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_stats_reply) + desc_stats.length());
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
cofmsg_desc_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow10::ofp_stats_reply) + desc_stats.length()))
			throw eInval();
		desc_stats.pack(buf + sizeof(struct openflow10::ofp_stats_reply), desc_stats.length());
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow12::ofp_stats_reply) + desc_stats.length()))
			throw eInval();
		desc_stats.pack(buf + sizeof(struct openflow12::ofp_stats_reply), desc_stats.length());
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
cofmsg_desc_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_desc_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	ofh_desc_stats = soframe();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		desc_stats.set_version(openflow10::OFP_VERSION);
		if (get_length() < (sizeof(struct openflow10::ofp_stats_reply) + sizeof(struct openflow10::ofp_desc_stats)))
			throw eBadSyntaxTooShort();
		ofh_desc_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
		desc_stats.unpack(ofh_desc_stats, sizeof(struct openflow10::ofp_desc_stats));
	} break;
	case openflow12::OFP_VERSION: {
		desc_stats.set_version(openflow12::OFP_VERSION);
		if (get_length() < (sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_desc_stats)))
			throw eBadSyntaxTooShort();
		ofh_desc_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
		desc_stats.unpack(ofh_desc_stats, sizeof(struct openflow12::ofp_desc_stats));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofdesc_stats_reply&
cofmsg_desc_stats_reply::get_desc_stats()
{
	return desc_stats;
}


