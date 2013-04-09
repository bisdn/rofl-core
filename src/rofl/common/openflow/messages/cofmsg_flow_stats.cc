#include "cofmsg_flow_stats.h"

using namespace rofl;





cofmsg_flow_stats_request::cofmsg_flow_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofflow_stats_request const& flow_stats) :
	cofmsg_stats(of_version, OFPT_STATS_REQUEST, xid, OFPST_FLOW, flags),
	flow_stats(flow_stats)
{
	switch (of_version) {
	case OFP10_VERSION:
	case OFP12_VERSION: {
		resize(length());
		pack(soframe(), framelen());
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_flow_stats_request::cofmsg_flow_stats_request(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{
	flow_stats.set_version(get_version());
}



cofmsg_flow_stats_request::cofmsg_flow_stats_request(
		cofmsg_flow_stats_request const& stats)
{
	*this = stats;
}



cofmsg_flow_stats_request&
cofmsg_flow_stats_request::operator= (
		cofmsg_flow_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_flow_stats = soframe();

	return *this;
}



cofmsg_flow_stats_request::~cofmsg_flow_stats_request()
{

}



void
cofmsg_flow_stats_request::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_flow_stats_request::resize(size_t len)
{
	cofmsg::resize(len);
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct ofp10_stats_request);
	} break;
	case OFP12_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct ofp12_stats_request);
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
cofmsg_flow_stats_request::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_stats_request) + flow_stats.length());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_request) + flow_stats.length());
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
cofmsg_flow_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (buflen < length())
			throw eInval();
		flow_stats.pack(buf + sizeof(struct ofp10_stats_request), flow_stats.length());
	} break;
	case OFP12_VERSION: {
		if (buflen < length())
			throw eInval();
		flow_stats.pack(buf + sizeof(struct ofp12_stats_request), flow_stats.length());
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
cofmsg_flow_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_flow_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	ofh_flow_stats = soframe();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_stats_request))
			throw eBadSyntaxTooShort();
		flow_stats.unpack(soframe() + sizeof(struct ofp10_stats_request), get_length() - sizeof(struct ofp10_stats_request));
	} break;
	case OFP12_VERSION: {
		if (get_length() < (sizeof(struct ofp12_stats_request) + sizeof(struct ofp12_flow_stats_request)))
			throw eBadSyntaxTooShort();
		flow_stats.unpack(soframe() + sizeof(struct ofp12_stats_request), get_length() - sizeof(struct ofp12_stats_request));
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofflow_stats_request&
cofmsg_flow_stats_request::get_flow_stats()
{
	return flow_stats;
}







cofmsg_flow_stats_reply::cofmsg_flow_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		std::vector<cofflow_stats_reply> const& flow_stats) :
	cofmsg_stats(of_version, OFPT_STATS_REPLY, xid, OFPST_FLOW, flags),
	flow_stats(flow_stats)
{
	switch (of_version) {
	case OFP10_VERSION:
	case OFP12_VERSION: {
		resize(length());
		pack(soframe(), framelen());
	} break;
	case OFP13_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_flow_stats_reply::cofmsg_flow_stats_reply(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{

}



cofmsg_flow_stats_reply::cofmsg_flow_stats_reply(
		cofmsg_flow_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_flow_stats_reply&
cofmsg_flow_stats_reply::operator= (
		cofmsg_flow_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_flow_stats = soframe();

	return *this;
}



cofmsg_flow_stats_reply::~cofmsg_flow_stats_reply()
{

}



void
cofmsg_flow_stats_reply::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_flow_stats_reply::resize(size_t len)
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
cofmsg_flow_stats_reply::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		size_t len = sizeof(struct ofp10_stats_reply);
		for (std::vector<cofflow_stats_reply>::const_iterator
				it = flow_stats.begin(); it != flow_stats.end(); ++it) {
			len += it->length();
		}
		return len;
	} break;
	case OFP12_VERSION: {
		size_t len = sizeof(struct ofp12_stats_reply);
		for (std::vector<cofflow_stats_reply>::const_iterator
				it = flow_stats.begin(); it != flow_stats.end(); ++it) {
			len += it->length();
		}
		return len;
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
cofmsg_flow_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (buflen < length())
			throw eInval();
		size_t len = sizeof(struct ofp10_stats_reply); // no ofp10_flow_stats structs stored yet
		for (unsigned int i = 0; i < flow_stats.size(); i++) {
			flow_stats[i].pack(buf + len, flow_stats[i].length());
			len += flow_stats[i].length();
		}
	} break;
	case OFP12_VERSION: {
		if (buflen < length())
			throw eInval();
		size_t len = sizeof(struct ofp12_stats_reply); // no ofp12_flow_stats structs stored yet
		for (unsigned int i = 0; i < flow_stats.size(); i++) {
			flow_stats[i].pack(buf + len, flow_stats[i].length());
			len += flow_stats[i].length();
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
cofmsg_flow_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_flow_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	flow_stats.clear();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_stats_reply))
			throw eBadSyntaxTooShort();

		struct ofp10_flow_stats *stats = (struct ofp10_flow_stats*)(soframe() + sizeof(struct ofp10_stats_reply));
		size_t len = get_length() - sizeof(struct ofp10_stats_reply);
		while (len >= sizeof(struct ofp10_flow_stats)) {

			if (be16toh(stats->length) > len)
				throw eBadSyntaxTooShort();

			cofflow_stats_reply flow_stats_reply;
			flow_stats_reply.set_version(OFP10_VERSION);
			flow_stats_reply.unpack((uint8_t*)stats, be16toh(stats->length));
			flow_stats.push_back(flow_stats_reply);

			len -= be16toh(stats->length);
			stats = (struct ofp10_flow_stats*)((uint8_t*)stats + be16toh(stats->length));
		}
	} break;
	case OFP12_VERSION: {
		if (get_length() < (sizeof(struct ofp12_stats_reply) + sizeof(struct ofp12_flow_stats)))
			throw eBadSyntaxTooShort();

		struct ofp12_flow_stats *stats = (struct ofp12_flow_stats*)(soframe() + sizeof(struct ofp12_stats_reply));
		size_t len = get_length() - sizeof(struct ofp12_stats_reply);
		while (len >= sizeof(struct ofp12_flow_stats)) {

			if (be16toh(stats->length) > len)
				throw eBadSyntaxTooShort();

			cofflow_stats_reply flow_stats_reply;
			flow_stats_reply.set_version(OFP12_VERSION);
			flow_stats_reply.unpack((uint8_t*)stats, be16toh(stats->length));
			flow_stats.push_back(flow_stats_reply);

			len -= be16toh(stats->length);
			stats = (struct ofp12_flow_stats*)((uint8_t*)stats + be16toh(stats->length));
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



std::vector<cofflow_stats_reply>&
cofmsg_flow_stats_reply::get_flow_stats()
{
	return flow_stats;
}


