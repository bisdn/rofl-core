#include "cofmsg_port_stats.h"

using namespace rofl;



cofmsg_port_stats::cofmsg_port_stats(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		std::vector<cofport_stats_reply> const& port_stats) :
	cofmsg_stats(of_version, OFPT_STATS_REPLY, xid, OFPST_PORT, flags),
	port_stats(port_stats)
{
	switch (of_version) {
	case OFP10_VERSION: {
		resize(sizeof(struct ofp10_stats_reply) + port_stats.size() * sizeof(struct ofp10_port_stats));
		for (unsigned int i = 0; i < port_stats.size(); i++) {
			port_stats[i].pack(soframe() + i * sizeof(struct ofh10_stats_reply), sizeof(struct ofp10_port_stats));
		}
	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_stats_reply) + port_stats.size() * sizeof(struct ofp12_port_stats));
		for (unsigned int i = 0; i < port_stats.size(); i++) {
			port_stats[i].pack(soframe() + i * sizeof(struct ofh12_stats_reply), sizeof(struct ofp12_port_stats));
		}
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_stats::cofmsg_port_stats(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{
	validate();
}



cofmsg_port_stats::cofmsg_port_stats(
		cofmsg_port_stats const& stats)
{
	*this = stats;
}



cofmsg_port_stats&
cofmsg_port_stats::operator= (
		cofmsg_port_stats const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_port_stats = soframe();

	return *this;
}



cofmsg_port_stats::~cofmsg_port_stats()
{

}



void
cofmsg_port_stats::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_port_stats::resize(size_t len)
{
	cofmsg::resize(len);
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct ofp10_stats_reply);
	} break;
	case OFP12_VERSION: {
		ofh_port_stats = soframe() + sizeof(struct ofp12_stats_reply);
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}

}



size_t
cofmsg_port_stats::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_stats_reply) + port_stats.size() * sizeof(struct ofp10_desc_stats));
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_reply) + port_stats.size() * sizeof(struct ofp12_desc_stats));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_stats::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	port_stats.clear();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (buflen < length())
			throw eInval();
		for (unsigned int i = 0; i < port_stats.size(); i++) {
			port_stats[i].pack(soframe() + i * sizeof(struct ofh10_stats_reply), sizeof(struct ofp10_port_stats));
		}
	} break;
	case OFP12_VERSION: {
		if (buflen < length())
			throw eInval();
		for (unsigned int i = 0; i < port_stats.size(); i++) {
			port_stats[i].pack(soframe() + i * sizeof(struct ofh12_stats_reply), sizeof(struct ofp12_port_stats));
		}
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_port_stats::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_port_stats::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_stats_reply))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct ofp10_stats_reply)) / sizeof(struct ofp10_port_stats)); i++) {
			cofport_stats_reply port_stats_reply;
			port_stats_reply.unpack(soframe() + sizeof(struct ofp10_stats_reply) + i * sizeof(struct ofp10_port_stats), sizeof(struct ofp10_port_stats));
			port_stats.push_back(port_stats_reply);
		}
	} break;
	case OFP12_VERSION: {
		if (get_length() < (sizeof(struct ofp12_stats_reply) + sizeof(struct ofp12_port_stats)))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct ofp12_stats_reply)) / sizeof(struct ofp12_port_stats)); i++) {
			cofport_stats_reply port_stats_reply;
			port_stats_reply.unpack(soframe() + sizeof(struct ofp12_stats_reply) + i * sizeof(struct ofp12_port_stats), sizeof(struct ofp12_port_stats));
			port_stats.push_back(port_stats_reply);
		}
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



std::vector<cofport_stats_reply>&
cofmsg_port_stats::get_port_stats()
{
	return port_stats;
}


