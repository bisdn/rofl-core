#include "cofmsg_table_stats.h"

using namespace rofl;



cofmsg_table_stats::cofmsg_table_stats(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		std::vector<coftable_stats_reply> const& table_stats) :
	cofmsg_stats(of_version, OFPT_STATS_REPLY, xid, OFPST_TABLE, flags),
	table_stats(table_stats)
{
	switch (of_version) {
	case OFP10_VERSION: {
		resize(sizeof(struct ofp10_stats_reply) + table_stats.size() * sizeof(struct ofp10_table_stats));
		for (unsigned int i = 0; i < table_stats.size(); i++) {
			table_stats[i].pack(soframe() + i * sizeof(struct ofh10_stats_reply), sizeof(struct ofp10_table_stats));
		}
	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_stats_reply) + table_stats.size() * sizeof(struct ofp12_table_stats));
		for (unsigned int i = 0; i < table_stats.size(); i++) {
			table_stats[i].pack(soframe() + i * sizeof(struct ofh12_stats_reply), sizeof(struct ofp12_table_stats));
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



cofmsg_table_stats::cofmsg_table_stats(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{
	validate();
}



cofmsg_table_stats::cofmsg_table_stats(
		cofmsg_table_stats const& stats)
{
	*this = stats;
}



cofmsg_table_stats&
cofmsg_table_stats::operator= (
		cofmsg_table_stats const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_table_stats = soframe();

	return *this;
}



cofmsg_table_stats::~cofmsg_table_stats()
{

}



void
cofmsg_table_stats::reset()
{
	cofmsg_stats::reset();
}



void
cofmsg_table_stats::resize(size_t len)
{
	cofmsg::resize(len);
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct ofp10_stats_reply);
	} break;
	case OFP12_VERSION: {
		ofh_table_stats = soframe() + sizeof(struct ofp12_stats_reply);
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
cofmsg_table_stats::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_stats_reply) + table_stats.size() * sizeof(struct ofp10_desc_stats));
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_reply) + table_stats.size() * sizeof(struct ofp12_desc_stats));
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
cofmsg_table_stats::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	table_stats.clear();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (buflen < length())
			throw eInval();
		for (unsigned int i = 0; i < table_stats.size(); i++) {
			table_stats[i].pack(soframe() + i * sizeof(struct ofh10_stats_reply), sizeof(struct ofp10_table_stats));
		}
	} break;
	case OFP12_VERSION: {
		if (buflen < length())
			throw eInval();
		for (unsigned int i = 0; i < table_stats.size(); i++) {
			table_stats[i].pack(soframe() + i * sizeof(struct ofh12_stats_reply), sizeof(struct ofp12_table_stats));
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
cofmsg_table_stats::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_stats::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_stats_reply))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct ofp10_stats_reply)) / sizeof(struct ofp10_table_stats)); i++) {
			coftable_stats_reply table_stats_reply;
			table_stats_reply.unpack(soframe() + sizeof(struct ofp10_stats_reply) + i * sizeof(struct ofp10_table_stats), sizeof(struct ofp10_table_stats));
			table_stats.push_back(table_stats_reply);
		}
	} break;
	case OFP12_VERSION: {
		if (get_length() < (sizeof(struct ofp12_stats_reply) + sizeof(struct ofp12_table_stats)))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct ofp12_stats_reply)) / sizeof(struct ofp12_table_stats)); i++) {
			coftable_stats_reply table_stats_reply;
			table_stats_reply.unpack(soframe() + sizeof(struct ofp12_stats_reply) + i * sizeof(struct ofp12_table_stats), sizeof(struct ofp12_table_stats));
			table_stats.push_back(table_stats_reply);
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



std::vector<coftable_stats_reply>&
cofmsg_table_stats::get_table_stats()
{
	return table_stats;
}


