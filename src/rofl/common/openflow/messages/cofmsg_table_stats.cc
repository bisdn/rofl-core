#include "cofmsg_table_stats.h"

using namespace rofl;




cofmsg_table_stats_request::cofmsg_table_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags) :
	cofmsg_stats(of_version, xid, 0, flags)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_stats_type(openflow10::OFPST_TABLE);
		set_type(openflow10::OFPT_STATS_REQUEST);
		resize(sizeof(struct openflow10::ofp_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		set_stats_type(openflow12::OFPST_TABLE);
		set_type(openflow12::OFPT_STATS_REQUEST);
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



cofmsg_table_stats_request::cofmsg_table_stats_request(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{

}



cofmsg_table_stats_request::cofmsg_table_stats_request(
		cofmsg_table_stats_request const& stats)
{
	*this = stats;
}



cofmsg_table_stats_request&
cofmsg_table_stats_request::operator= (
		cofmsg_table_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	return *this;
}



cofmsg_table_stats_request::~cofmsg_table_stats_request()
{

}



void
cofmsg_table_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_table_stats_request::resize(size_t len)
{
	return cofmsg_stats::resize(len);
}



size_t
cofmsg_table_stats_request::length() const
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
cofmsg_table_stats_request::pack(uint8_t *buf, size_t buflen)
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
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < length())
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
cofmsg_table_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_stats_request))
			throw eBadSyntaxTooShort();
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_stats_request))
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





cofmsg_table_stats_reply::cofmsg_table_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		std::vector<coftable_stats_reply> const& table_stats) :
	cofmsg_stats(of_version, xid, 0, flags),
	table_stats(table_stats)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_stats_type(openflow10::OFPST_TABLE);
		set_type(openflow10::OFPT_STATS_REPLY);
		resize(sizeof(struct openflow10::ofp_stats_reply) + table_stats.size() * sizeof(struct openflow10::ofp_table_stats));
		for (unsigned int i = 0; i < table_stats.size(); i++) {
			table_stats[i].pack(soframe() + sizeof(struct openflow10::ofp_stats_reply) + i * sizeof(struct openflow10::ofp_stats_reply), sizeof(struct openflow10::ofp_table_stats));
		}
	} break;
	case openflow12::OFP_VERSION: {
		set_stats_type(openflow12::OFPST_TABLE);
		set_type(openflow12::OFPT_STATS_REPLY);
		resize(sizeof(struct openflow12::ofp_stats_reply) + table_stats.size() * sizeof(struct openflow12::ofp_table_stats));
		for (unsigned int i = 0; i < table_stats.size(); i++) {
			table_stats[i].pack(soframe() + sizeof(struct openflow12::ofp_stats_reply) + i * sizeof(struct openflow12::ofp_stats_reply), sizeof(struct openflow12::ofp_table_stats));
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



cofmsg_table_stats_reply::cofmsg_table_stats_reply(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // TODO
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_table_stats_reply::cofmsg_table_stats_reply(
		cofmsg_table_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_table_stats_reply&
cofmsg_table_stats_reply::operator= (
		cofmsg_table_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_table_stats = soframe();

	return *this;
}



cofmsg_table_stats_reply::~cofmsg_table_stats_reply()
{

}



void
cofmsg_table_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_table_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
	} break;
	case openflow13::OFP_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_table_stats_reply::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_stats_reply) + table_stats.size() * sizeof(struct openflow10::ofp_table_stats));
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_stats_reply) + table_stats.size() * sizeof(struct openflow12::ofp_table_stats));
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
cofmsg_table_stats_reply::pack(uint8_t *buf, size_t buflen)
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
		for (unsigned int i = 0; i < table_stats.size(); i++) {
			table_stats[i].pack(buf + sizeof(struct openflow10::ofp_stats_reply) + i * sizeof(struct openflow10::ofp_table_stats), sizeof(struct openflow10::ofp_table_stats));
		}
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		for (unsigned int i = 0; i < table_stats.size(); i++) {
			table_stats[i].pack(buf + sizeof(struct openflow12::ofp_stats_reply) + i * sizeof(struct openflow12::ofp_table_stats), sizeof(struct openflow12::ofp_table_stats));
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
cofmsg_table_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	table_stats.clear();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_stats_reply))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct openflow10::ofp_stats_reply)) / sizeof(struct openflow10::ofp_table_stats)); i++) {
			coftable_stats_reply table_stats_reply(openflow10::OFP_VERSION);
			table_stats_reply.unpack(soframe() + sizeof(struct openflow10::ofp_stats_reply) + i * sizeof(struct openflow10::ofp_table_stats), sizeof(struct openflow10::ofp_table_stats));
			table_stats.push_back(table_stats_reply);
		}
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_table_stats)))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct openflow12::ofp_stats_reply)) / sizeof(struct openflow12::ofp_table_stats)); i++) {
			coftable_stats_reply table_stats_reply(openflow12::OFP_VERSION);
			table_stats_reply.unpack(soframe() + sizeof(struct openflow12::ofp_stats_reply) + i * sizeof(struct openflow12::ofp_table_stats), sizeof(struct openflow12::ofp_table_stats));
			table_stats.push_back(table_stats_reply);
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



std::vector<coftable_stats_reply>&
cofmsg_table_stats_reply::get_table_stats()
{
	return table_stats;
}


