#include "cofmsg_port_stats.h"

using namespace rofl;





cofmsg_port_stats_request::cofmsg_port_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofport_stats_request const& port_stats) :
	cofmsg_stats_request(of_version, xid, 0, flags),
	port_stats(port_stats)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_STATS_REQUEST);
		set_stats_type(openflow10::OFPST_PORT);
		resize(sizeof(struct openflow10::ofp_stats_request) + sizeof(struct openflow10::ofp_port_stats_request));
		port_stats.pack(soframe(), sizeof(struct openflow10::ofp_port_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_STATS_REQUEST);
		set_stats_type(openflow12::OFPST_PORT);
		resize(sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_port_stats_request));
		port_stats.pack(soframe(), sizeof(struct openflow12::ofp_port_stats_request));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_stats_request::cofmsg_port_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea),
	port_stats(get_version())
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct openflow10::ofp_stats_request);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct openflow12::ofp_stats_request);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_stats_request::cofmsg_port_stats_request(
		cofmsg_port_stats_request const& stats)
{
	*this = stats;
}



cofmsg_port_stats_request&
cofmsg_port_stats_request::operator= (
		cofmsg_port_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_port_stats = soframe();

	return *this;
}



cofmsg_port_stats_request::~cofmsg_port_stats_request()
{

}



void
cofmsg_port_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_port_stats_request::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct openflow10::ofp_stats_request);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct openflow12::ofp_stats_request);
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
cofmsg_port_stats_request::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_stats_request) + sizeof(struct openflow10::ofp_port_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_port_stats_request));
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
cofmsg_port_stats_request::pack(uint8_t *buf, size_t buflen)
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
		port_stats.pack(buf + sizeof(struct openflow10::ofp_stats_request), sizeof(struct openflow10::ofp_port_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		port_stats.pack(buf + sizeof(struct openflow12::ofp_stats_request), sizeof(struct openflow12::ofp_port_stats_request));
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
cofmsg_port_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_port_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_stats_request))
			throw eBadSyntaxTooShort();
		port_stats.unpack(soframe() + sizeof(struct openflow10::ofp_stats_request), sizeof(struct openflow10::ofp_port_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow12::ofp_stats_request) + sizeof(struct openflow12::ofp_port_stats)))
			throw eBadSyntaxTooShort();
		port_stats.unpack(soframe() + sizeof(struct openflow12::ofp_stats_request), sizeof(struct openflow12::ofp_port_stats_request));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofport_stats_request&
cofmsg_port_stats_request::get_port_stats()
{
	return port_stats;
}





cofmsg_port_stats_reply::cofmsg_port_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		std::vector<cofport_stats_reply> const& port_stats) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	port_stats(port_stats)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_STATS_REPLY);
		set_stats_type(openflow10::OFPST_PORT);
		resize(sizeof(struct openflow10::ofp_stats_reply) + port_stats.size() * sizeof(struct openflow10::ofp_port_stats));
		for (unsigned int i = 0; i < port_stats.size(); i++) {
			port_stats[i].pack(soframe() + sizeof(struct openflow10::ofp_stats_reply) + i * sizeof(struct openflow10::ofp_port_stats), sizeof(struct openflow10::ofp_port_stats));
		}
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_STATS_REPLY);
		set_stats_type(openflow12::OFPST_PORT);
		resize(sizeof(struct openflow12::ofp_stats_reply) + port_stats.size() * sizeof(struct openflow12::ofp_port_stats));
		for (unsigned int i = 0; i < port_stats.size(); i++) {
			port_stats[i].pack(soframe() + sizeof(struct openflow12::ofp_stats_reply) + i * sizeof(struct openflow12::ofp_port_stats), sizeof(struct openflow12::ofp_port_stats));
		}
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_stats_reply::cofmsg_port_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_stats_reply::cofmsg_port_stats_reply(
		cofmsg_port_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_port_stats_reply&
cofmsg_port_stats_reply::operator= (
		cofmsg_port_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_port_stats = soframe();

	return *this;
}



cofmsg_port_stats_reply::~cofmsg_port_stats_reply()
{

}



void
cofmsg_port_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_port_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
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
cofmsg_port_stats_reply::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_stats_reply) + port_stats.size() * sizeof(struct openflow10::ofp_port_stats));
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_stats_reply) + port_stats.size() * sizeof(struct openflow12::ofp_port_stats));
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
cofmsg_port_stats_reply::pack(uint8_t *buf, size_t buflen)
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
		for (unsigned int i = 0; i < port_stats.size(); i++) {
			port_stats[i].pack(buf + sizeof(struct openflow10::ofp_stats_reply) + i * sizeof(struct openflow10::ofp_port_stats), sizeof(struct openflow10::ofp_port_stats));
		}
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		for (unsigned int i = 0; i < port_stats.size(); i++) {
			port_stats[i].pack(buf + sizeof(struct openflow12::ofp_stats_reply) + i * sizeof(struct openflow12::ofp_port_stats), sizeof(struct openflow12::ofp_port_stats));
		}
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
cofmsg_port_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_port_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	port_stats.clear();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_stats_reply))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct openflow10::ofp_stats_reply)) / sizeof(struct openflow10::ofp_port_stats)); i++) {
			cofport_stats_reply port_stats_reply(openflow10::OFP_VERSION);
			port_stats_reply.unpack(soframe() + sizeof(struct openflow10::ofp_stats_reply) + i * sizeof(struct openflow10::ofp_port_stats), sizeof(struct openflow10::ofp_port_stats));
			port_stats.push_back(port_stats_reply);
		}
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_port_stats)))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct openflow12::ofp_stats_reply)) / sizeof(struct openflow12::ofp_port_stats)); i++) {
			cofport_stats_reply port_stats_reply(openflow12::OFP_VERSION);
			port_stats_reply.unpack(soframe() + sizeof(struct openflow12::ofp_stats_reply) + i * sizeof(struct openflow12::ofp_port_stats), sizeof(struct openflow12::ofp_port_stats));
			port_stats.push_back(port_stats_reply);
		}
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



std::vector<cofport_stats_reply>&
cofmsg_port_stats_reply::get_port_stats()
{
	return port_stats;
}


