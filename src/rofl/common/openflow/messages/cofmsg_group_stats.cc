#include "cofmsg_group_stats.h"

using namespace rofl;




cofmsg_group_stats_request::cofmsg_group_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofgroup_stats_request const& group_stats) :
	cofmsg_stats(of_version, OFPT_STATS_REQUEST, xid, OFPST_GROUP, flags),
	group_stats(group_stats)
{
	switch (of_version) {
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_stats_request) + sizeof(struct ofp12_group_stats_request));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_stats_request::cofmsg_group_stats_request(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{

}



cofmsg_group_stats_request::cofmsg_group_stats_request(
		cofmsg_group_stats_request const& stats)
{
	*this = stats;
}



cofmsg_group_stats_request&
cofmsg_group_stats_request::operator= (
		cofmsg_group_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_group_stats = soframe();

	group_stats = stats.group_stats;

	return *this;
}



cofmsg_group_stats_request::~cofmsg_group_stats_request()
{

}



void
cofmsg_group_stats_request::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_group_stats_request::resize(size_t len)
{
	cofmsg::resize(len);
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct ofp12_stats_request);
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
cofmsg_group_stats_request::length() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_request) + sizeof(struct ofp12_group_stats_request));
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
cofmsg_group_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();


	switch (get_version()) {
	case OFP12_VERSION: {
		if (buflen < length())
			throw eInval();
		group_stats.unpack(buf + sizeof(struct ofp12_stats_request), sizeof(struct ofp12_group_stats_request));
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
cofmsg_group_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case OFP12_VERSION: {
		if (get_length() < (sizeof(struct ofp12_stats_request) + sizeof(struct ofp12_group_stats_request)))
			throw eBadSyntaxTooShort();
		ofh_group_stats = soframe() + sizeof(struct ofp12_stats_request);
		group_stats.unpack(soframe() + sizeof(struct ofp12_stats_request), sizeof(struct ofp12_group_stats_request));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofgroup_stats_request&
cofmsg_group_stats_request::get_group_stats()
{
	return group_stats;
}





cofmsg_group_stats_reply::cofmsg_group_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		std::vector<cofgroup_stats_reply> const& group_stats) :
	cofmsg_stats(of_version, OFPT_STATS_REPLY, xid, OFPST_GROUP, flags),
	group_stats(group_stats)
{
	switch (of_version) {
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_stats_reply) + group_stats.size() * sizeof(struct ofp12_group_stats));
		for (unsigned int i = 0; i < group_stats.size(); i++) {
			group_stats[i].pack(soframe() + i * sizeof(struct ofp12_stats_reply), sizeof(struct ofp12_group_stats));
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



cofmsg_group_stats_reply::cofmsg_group_stats_reply(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{

}



cofmsg_group_stats_reply::cofmsg_group_stats_reply(
		cofmsg_group_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_group_stats_reply&
cofmsg_group_stats_reply::operator= (
		cofmsg_group_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_group_stats = soframe();

	return *this;
}



cofmsg_group_stats_reply::~cofmsg_group_stats_reply()
{

}



void
cofmsg_group_stats_reply::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_group_stats_reply::resize(size_t len)
{
	cofmsg::resize(len);
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh_group_stats = soframe() + sizeof(struct ofp12_stats_reply);
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
cofmsg_group_stats_reply::length() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_reply) + group_stats.size() * sizeof(struct ofp12_group_stats));
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
cofmsg_group_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP12_VERSION: {
		if (buflen < length())
			throw eInval();
		for (unsigned int i = 0; i < group_stats.size(); i++) {
			group_stats[i].pack(soframe() + sizeof(struct ofp12_stats_reply) + i * sizeof(struct ofp12_group_stats), sizeof(struct ofp12_group_stats));
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
cofmsg_group_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	group_stats.clear();

	switch (get_version()) {
	case OFP12_VERSION: {
		if (get_length() < (sizeof(struct ofp12_stats_reply) + sizeof(struct ofp12_group_stats)))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct ofp12_stats_reply)) / sizeof(struct ofp12_group_stats)); i++) {
			cofgroup_stats_reply group_stats_reply(OFP12_VERSION);
			group_stats_reply.unpack(soframe() + sizeof(struct ofp12_stats_reply) + i * sizeof(struct ofp12_group_stats), sizeof(struct ofp12_group_stats));
			group_stats.push_back(group_stats_reply);
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



std::vector<cofgroup_stats_reply>&
cofmsg_group_stats_reply::get_group_stats()
{
	return group_stats;
}


