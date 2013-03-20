#include "cofmsg_aggr_stats.h"

using namespace rofl;



cofmsg_aggr_stats::cofmsg_aggr_stats(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofaggr_stats_reply const& aggr_stats) :
	cofmsg_stats(of_version, OFPT_STATS_REPLY, xid, OFPST_AGGREGATE, flags),
	aggr_stats(aggr_stats)
{
	switch (of_version) {
	case OFP10_VERSION: {
		resize(sizeof(struct ofp10_stats_reply) + sizeof(struct ofp10_aggr_stats));
		aggr_stats.pack(soframe() + sizeof(struct ofh10_stats_reply), sizeof(struct ofp10_aggr_stats));
	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_stats_reply) + sizeof(struct ofp12_aggr_stats));
		aggr_stats.pack(soframe() + sizeof(struct ofh12_stats_reply), sizeof(struct ofp12_aggr_stats));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_aggr_stats::cofmsg_aggr_stats(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{
	validate();
}



cofmsg_aggr_stats::cofmsg_aggr_stats(
		cofmsg_aggr_stats const& stats)
{
	*this = stats;
}



cofmsg_aggr_stats&
cofmsg_aggr_stats::operator= (
		cofmsg_aggr_stats const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_aggr_stats = soframe();

	return *this;
}



cofmsg_aggr_stats::~cofmsg_aggr_stats()
{

}



void
cofmsg_aggr_stats::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_aggr_stats::resize(size_t len)
{
	cofmsg::resize(len);
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh_aggr_stats = soframe() + sizeof(struct ofp10_stats_reply);
	} break;
	case OFP12_VERSION: {
		ofh_aggr_stats = soframe() + sizeof(struct ofp12_stats_reply);
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
cofmsg_aggr_stats::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_stats_reply) + aggr_stats.length());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_reply) + aggr_stats.length());
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
cofmsg_aggr_stats::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (buflen < (sizeof(struct ofp10_stats_reply) + aggr_stats.length()))
			throw eInval();
		aggr_stats.pack(buf + sizeof(struct ofp10_stats_reply), aggr_stats.length());
	} break;
	case OFP12_VERSION: {
		if (buflen < (sizeof(struct ofp12_stats_reply) + aggr_stats.length()))
			throw eInval();
		aggr_stats.pack(buf + sizeof(struct ofp12_stats_reply), aggr_stats.length());
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
cofmsg_aggr_stats::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_aggr_stats::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < (sizeof(struct ofp10_stats_reply) + sizeof(struct ofp10_aggr_stats)))
			throw eBadSyntaxTooShort();
		ofh_aggr_stats = soframe() + sizeof(struct ofp10_stats_reply);
		aggr_stats.unpack(ofh_aggr_stats, sizeof(struct ofp10_aggr_stats));
	} break;
	case OFP12_VERSION: {
		if (get_length() < (sizeof(struct ofp12_stats_request) + sizeof(struct ofp12_aggr_stats)))
			throw eBadSyntaxTooShort();
		ofh_aggr_stats = soframe() + sizeof(struct ofp12_stats_reply);
		aggr_stats.unpack(ofh_aggr_stats, sizeof(struct ofp12_aggr_stats));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofaggr_stats_reply&
cofmsg_aggr_stats::get_aggr_stats()
{
	return aggr_stats;
}


