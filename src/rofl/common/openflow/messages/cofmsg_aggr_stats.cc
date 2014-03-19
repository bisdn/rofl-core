#include "cofmsg_aggr_stats.h"

using namespace rofl;




cofmsg_aggr_stats_request::cofmsg_aggr_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofaggr_stats_request const& aggr_stats) :
	cofmsg_stats(of_version, xid, OFPST_AGGREGATE, flags),
	aggr_stats(aggr_stats)
{
std::cout << "TP" << __LINE__ << std::endl;
	switch (of_version) {
	case OFP10_VERSION: {
std::cout << "TP" << __LINE__ << std::endl;
		set_type(OFPT10_STATS_REQUEST);
std::cout << "TP" << __LINE__ << std::endl;
		resize(sizeof(struct ofp10_stats_request) + sizeof(struct ofp10_aggregate_stats_request));
std::cout << "TP" << __LINE__ << std::endl;
		this->aggr_stats.pack(soframe() + sizeof(struct ofp10_stats_request), sizeof(struct ofp10_aggregate_stats_request));
std::cout << "TP" << __LINE__ << std::endl;
	} break;
	case OFP12_VERSION: {
		set_type(OFPT12_STATS_REQUEST);
		resize(sizeof(struct ofp12_stats_request) + sizeof(struct ofp12_aggregate_stats_request));
		this->aggr_stats.pack(soframe() + sizeof(struct ofp12_stats_request), sizeof(struct ofp12_aggregate_stats_request));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_aggr_stats_request::cofmsg_aggr_stats_request(
		cmemory *memarea) :
	cofmsg_stats(memarea),
	aggr_stats(get_version())
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh_aggr_stats = soframe() + sizeof(struct ofp10_stats_reply);
	} break;
	case OFP12_VERSION: {
		ofh_aggr_stats = soframe() + sizeof(struct ofp12_stats_reply);
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_aggr_stats_request::cofmsg_aggr_stats_request(
		cofmsg_aggr_stats_request const& stats)
{
	*this = stats;
}



cofmsg_aggr_stats_request&
cofmsg_aggr_stats_request::operator= (
		cofmsg_aggr_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_aggr_stats = soframe();

	return *this;
}



cofmsg_aggr_stats_request::~cofmsg_aggr_stats_request()
{

}



void
cofmsg_aggr_stats_request::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_aggr_stats_request::resize(size_t len)
{
	cofmsg::resize(len);
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh_aggr_stats = soframe() + sizeof(struct ofp10_stats_request);
	} break;
	case OFP12_VERSION: {
		ofh_aggr_stats = soframe() + sizeof(struct ofp12_stats_request);
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
cofmsg_aggr_stats_request::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_stats_request) + aggr_stats.length());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_request) + aggr_stats.length());
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
cofmsg_aggr_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (buflen < (sizeof(struct ofp10_stats_request) + aggr_stats.length()))
			throw eInval();
		aggr_stats.pack(buf + sizeof(struct ofp10_stats_request), aggr_stats.length());
	} break;
	case OFP12_VERSION: {
		if (buflen < (sizeof(struct ofp12_stats_request) + aggr_stats.length()))
			throw eInval();
		aggr_stats.pack(buf + sizeof(struct ofp12_stats_request), aggr_stats.length());
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
cofmsg_aggr_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_aggr_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	ofh_aggr_stats = soframe();

	switch (get_version()) {
	case OFP10_VERSION: {
		aggr_stats.set_version(OFP10_VERSION);
		if (get_length() < (sizeof(struct ofp10_stats_request) + sizeof(struct ofp10_aggregate_stats_request)))
			throw eBadSyntaxTooShort();
		ofh_aggr_stats = soframe() + sizeof(struct ofp10_stats_request);
		aggr_stats.unpack(ofh_aggr_stats, sizeof(struct ofp10_aggregate_stats_request));
	} break;
	case OFP12_VERSION: {
		aggr_stats.set_version(OFP12_VERSION);
		if (get_length() < (sizeof(struct ofp12_stats_request) + sizeof(struct ofp12_aggregate_stats_request)))
			throw eBadSyntaxTooShort();
		ofh_aggr_stats = soframe() + sizeof(struct ofp12_stats_request);
		aggr_stats.unpack(ofh_aggr_stats, sizeof(struct ofp12_aggregate_stats_request));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofaggr_stats_request&
cofmsg_aggr_stats_request::get_aggr_stats()
{
	return aggr_stats;
}





cofmsg_aggr_stats_reply::cofmsg_aggr_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofaggr_stats_reply const& aggr_stats) :
	cofmsg_stats(of_version, xid, OFPST_AGGREGATE, flags),
	aggr_stats(aggr_stats)
{
	switch (of_version) {
	case OFP10_VERSION: {
		set_type(OFPT10_STATS_REPLY);
		resize(sizeof(struct ofp10_stats_reply) + sizeof(struct ofp10_aggregate_stats_reply));
		this->aggr_stats.pack(soframe() + sizeof(struct ofp10_stats_reply), sizeof(struct ofp10_aggregate_stats_reply));
	} break;
	case OFP12_VERSION: {
		set_type(OFPT12_STATS_REPLY);
		resize(sizeof(struct ofp12_stats_reply) + sizeof(struct ofp12_aggregate_stats_reply));
		this->aggr_stats.pack(soframe() + sizeof(struct ofp12_stats_reply), sizeof(struct ofp12_aggregate_stats_reply));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_aggr_stats_reply::cofmsg_aggr_stats_reply(
		cmemory *memarea) :
	cofmsg_stats(memarea),
	aggr_stats(get_version())
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh_aggr_stats = soframe() + sizeof(struct ofp10_stats_reply);
	} break;
	case OFP12_VERSION: {
		ofh_aggr_stats = soframe() + sizeof(struct ofp12_stats_reply);
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_aggr_stats_reply::cofmsg_aggr_stats_reply(
		cofmsg_aggr_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_aggr_stats_reply&
cofmsg_aggr_stats_reply::operator= (
		cofmsg_aggr_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_aggr_stats = soframe();

	return *this;
}



cofmsg_aggr_stats_reply::~cofmsg_aggr_stats_reply()
{

}



void
cofmsg_aggr_stats_reply::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_aggr_stats_reply::resize(size_t len)
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
cofmsg_aggr_stats_reply::length() const
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
cofmsg_aggr_stats_reply::pack(uint8_t *buf, size_t buflen)
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
cofmsg_aggr_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_aggr_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case OFP10_VERSION: {
		aggr_stats.set_version(OFP10_VERSION);
		if (get_length() < (sizeof(struct ofp10_stats_reply) + sizeof(struct ofp10_aggregate_stats_reply)))
			throw eBadSyntaxTooShort();
		ofh_aggr_stats = soframe() + sizeof(struct ofp10_stats_reply);
		aggr_stats.unpack(ofh_aggr_stats, sizeof(struct ofp10_aggregate_stats_reply));
	} break;
	case OFP12_VERSION: {
		aggr_stats.set_version(OFP12_VERSION);
		if (get_length() < (sizeof(struct ofp12_stats_reply) + sizeof(struct ofp12_aggregate_stats_reply)))
			throw eBadSyntaxTooShort();
		ofh_aggr_stats = soframe() + sizeof(struct ofp12_stats_reply);
		aggr_stats.unpack(ofh_aggr_stats, sizeof(struct ofp12_aggregate_stats_reply));
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
cofmsg_aggr_stats_reply::get_aggr_stats()
{
	return aggr_stats;
}


