#include "cofmsg_group_desc_stats.h"

using namespace rofl;




cofmsg_group_desc_stats_request::cofmsg_group_desc_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags) :
	cofmsg_stats_request(of_version, xid, 0, flags)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REQUEST);
		set_stats_type(rofl::openflow12::OFPST_GROUP_DESC);
		resize(sizeof(struct rofl::openflow12::ofp_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_desc_stats_request::cofmsg_group_desc_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{

}



cofmsg_group_desc_stats_request::cofmsg_group_desc_stats_request(
		cofmsg_group_desc_stats_request const& stats)
{
	*this = stats;
}



cofmsg_group_desc_stats_request&
cofmsg_group_desc_stats_request::operator= (
		cofmsg_group_desc_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	return *this;
}



cofmsg_group_desc_stats_request::~cofmsg_group_desc_stats_request()
{

}



void
cofmsg_group_desc_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_group_desc_stats_request::resize(size_t len)
{
	return cofmsg_stats::resize(len);
}



size_t
cofmsg_group_desc_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_group_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_desc_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
	} break;
	case rofl::openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_group_desc_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_desc_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow12::ofp_stats_request)))//NOTE group_desc_stats_request has no body //+ sizeof(struct rofl::openflow12::ofp_group_desc_stats)))
			throw eBadSyntaxTooShort();
	} break;
	case rofl::openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}





cofmsg_group_desc_stats_reply::cofmsg_group_desc_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		std::vector<cofgroup_desc_stats_reply> const& group_desc_stats) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	group_desc_stats(group_desc_stats)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REPLY);
		set_stats_type(rofl::openflow12::OFPST_GROUP_DESC);
		resize(length());
		size_t offset = 0;
		std::vector<cofgroup_desc_stats_reply> tmp;
		for (std::vector<cofgroup_desc_stats_reply>::const_iterator
				it = group_desc_stats.begin(); it != group_desc_stats.end(); ++it) {
			tmp.push_back(*it);
		}
		for (unsigned int i = 0; i < tmp.size(); i++) {
			tmp[i].pack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply) + offset, tmp[i].length());
			offset += group_desc_stats[i].length();
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_desc_stats_reply::cofmsg_group_desc_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	group_desc_stats(get_version())
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_desc_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_desc_stats_reply::cofmsg_group_desc_stats_reply(
		cofmsg_group_desc_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_group_desc_stats_reply&
cofmsg_group_desc_stats_reply::operator= (
		cofmsg_group_desc_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_group_desc_stats = soframe();

	return *this;
}



cofmsg_group_desc_stats_reply::~cofmsg_group_desc_stats_reply()
{

}



void
cofmsg_group_desc_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_group_desc_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		ofh_group_desc_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_group_desc_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		size_t len = sizeof(struct rofl::openflow12::ofp_stats_reply);
		for (unsigned int i = 0; i < group_desc_stats.size(); i++) {
			len += group_desc_stats[i].length();
		}
		return len;
	} break;
	case rofl::openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_desc_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		size_t offset = 0;
		for (unsigned int i = 0; i < group_desc_stats.size(); i++) {
			group_desc_stats[i].pack(buf + sizeof(struct rofl::openflow12::ofp_stats_reply) + offset, group_desc_stats[i].length());
			offset += group_desc_stats[i].length();
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_group_desc_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_desc_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	group_desc_stats.clear();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow12::ofp_stats_reply) + sizeof(struct rofl::openflow12::ofp_group_desc_stats)))
			throw eBadSyntaxTooShort();

		ofh_group_desc_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);

		size_t residual = get_length() - sizeof(struct rofl::openflow12::ofp_stats_reply);

		while (residual >= sizeof(struct rofl::openflow12::ofp_group_desc_stats)) {

			uint8_t* desc_stats = soframe() + get_length() - residual;
			size_t length = be16toh(((struct rofl::openflow12::ofp_group_desc_stats*)desc_stats)->length);

			cofgroup_desc_stats_reply group_desc_stats_reply(rofl::openflow12::OFP_VERSION);
			group_desc_stats_reply.unpack(desc_stats, length);
			group_desc_stats.push_back(group_desc_stats_reply);
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow13::ofp_multipart_reply) + sizeof(struct rofl::openflow13::ofp_group_desc)))
			throw eBadSyntaxTooShort();

		ofh_group_desc_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);

		size_t residual = get_length() - sizeof(struct rofl::openflow13::ofp_multipart_reply);

		while (residual >= sizeof(struct rofl::openflow13::ofp_group_desc)) {

			uint8_t* desc_stats = soframe() + get_length() - residual;
			size_t length = be16toh(((struct rofl::openflow13::ofp_group_desc*)desc_stats)->length);

			cofgroup_desc_stats_reply group_desc_stats_reply(rofl::openflow13::OFP_VERSION);
			group_desc_stats_reply.unpack(desc_stats, length);
			group_desc_stats.push_back(group_desc_stats_reply);
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



std::vector<cofgroup_desc_stats_reply>&
cofmsg_group_desc_stats_reply::set_group_desc_stats()
{
	return group_desc_stats;
}



std::vector<cofgroup_desc_stats_reply> const&
cofmsg_group_desc_stats_reply::get_group_desc_stats() const
{
	return group_desc_stats;
}


