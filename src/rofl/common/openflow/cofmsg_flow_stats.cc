#include "cofmsg_flow_stats.h"

using namespace rofl;



cofmsg_flow_stats::cofmsg_flow_stats(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofflow_stats_reply const& flow_stats) :
	cofmsg_stats(of_version, OFPT_STATS_REPLY, xid, OFPST_FLOW, flags),
	flow_stats(flow_stats)
{
	switch (of_version) {
	case OFP10_VERSION: {
		resize(sizeof(struct ofp10_stats_reply) + sizeof(struct ofp10_flow_stats));
		flow_stats.pack(soframe() + sizeof(struct ofh10_stats_reply), sizeof(struct ofp10_flow_stats));
	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_stats_reply) + sizeof(struct ofp12_flow_stats));
		flow_stats.pack(soframe() + sizeof(struct ofh12_stats_reply), sizeof(struct ofp12_flow_stats));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_flow_stats::cofmsg_flow_stats(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{
	validate();
}



cofmsg_flow_stats::cofmsg_flow_stats(
		cofmsg_flow_stats const& stats)
{
	*this = stats;
}



cofmsg_flow_stats&
cofmsg_flow_stats::operator= (
		cofmsg_flow_stats const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_flow_stats = soframe();

	return *this;
}



cofmsg_flow_stats::~cofmsg_flow_stats()
{

}



void
cofmsg_flow_stats::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_flow_stats::resize(size_t len)
{
	cofmsg::resize(len);
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct ofp10_stats_reply);
	} break;
	case OFP12_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct ofp12_stats_reply);
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
cofmsg_flow_stats::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_stats_reply) + flow_stats.length());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_reply) + flow_stats.length());
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
cofmsg_flow_stats::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (buflen < (sizeof(struct ofp10_stats_reply) + flow_stats.length()))
			throw eInval();
		flow_stats.pack(buf + sizeof(struct ofp10_stats_reply), flow_stats.length());
	} break;
	case OFP12_VERSION: {
		if (buflen < (sizeof(struct ofp12_stats_reply) + flow_stats.length()))
			throw eInval();
		flow_stats.pack(buf + sizeof(struct ofp12_stats_reply), flow_stats.length());
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
cofmsg_flow_stats::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_flow_stats::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < (sizeof(struct ofp10_stats_reply) + sizeof(struct ofp10_flow_stats)))
			throw eBadSyntaxTooShort();
		ofh_flow_stats = soframe() + sizeof(struct ofp10_stats_reply);
		flow_stats.unpack(ofh_flow_stats, sizeof(struct ofp10_flow_stats));
	} break;
	case OFP12_VERSION: {
		if (get_length() < (sizeof(struct ofp12_stats_request) + sizeof(struct ofp12_flow_stats)))
			throw eBadSyntaxTooShort();
		ofh_flow_stats = soframe() + sizeof(struct ofp12_stats_reply);
		flow_stats.unpack(ofh_flow_stats, sizeof(struct ofp12_flow_stats));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofflow_stats_reply&
cofmsg_flow_stats::get_flow_stats()
{
	return flow_stats;
}


