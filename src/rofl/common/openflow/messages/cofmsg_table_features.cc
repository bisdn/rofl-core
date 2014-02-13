#include "cofmsg_table_features.h"

using namespace rofl;




cofmsg_table_features::cofmsg_table_features(
		uint8_t of_version,
		uint32_t xid,
		uint16_t stats_flags) :
	cofmsg_stats(of_version, xid, 0, stats_flags)
{
}



cofmsg_table_features::cofmsg_table_features(
		cmemory *memarea) :
	cofmsg_stats(memarea)
{

}



cofmsg_table_features::cofmsg_table_features(
		cofmsg_table_features const& msg)
{
	*this = msg;
}



cofmsg_table_features&
cofmsg_table_features::operator= (
		cofmsg_table_features const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg_stats::operator =(msg);
	tables.clear();
	for (std::map<uint8_t, rofl::openflow::coftable_features>::const_iterator
			it = tables.begin(); it != tables.end(); ++it) {
		add_table(it->first, it->second);
	}

	return *this;
}



cofmsg_table_features::~cofmsg_table_features()
{

}



void
cofmsg_table_features::reset()
{
	cofmsg_stats::reset();
	tables.clear();
}



uint8_t*
cofmsg_table_features::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		ofh_ofhu.ofhu_generic = soframe() + sizeof(struct rofl::openflow13::ofp_table_features);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_table_features::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		size_t len = sizeof(struct rofl::openflow13::ofp_multipart_request); // same as ofp_multipart_reply
		for (std::map<uint8_t, rofl::openflow::coftable_features>::const_iterator
				it = tables.begin(); it != tables.end(); ++it) {
			len += it->second.length();
		}
		return len;
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_table_features::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length()) {
		throw eInval();
	}

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_multipart_request));
		buf += sizeof(struct rofl::openflow13::ofp_multipart_request);
		for (std::map<uint8_t, rofl::openflow::coftable_features>::iterator
						it = tables.begin(); it != tables.end(); ++it) {
			it->second.pack(buf, it->second.length());
			buf += it->second.length();
		}
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_table_features::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_features::validate()
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		if (cofmsg::get_length() == sizeof(struct rofl::openflow13::ofp_multipart_request)) {
			return;
		}

		uint8_t* buf 	= body.somem();
		size_t buflen 	= body.memlen();

		tables.clear();
		while (true) {
			struct rofl::openflow13::ofp_table_features *table = (struct rofl::openflow13::ofp_table_features*)buf;

			if ((be16toh(table->length) > buflen) || (be16toh(table->length) < sizeof(struct rofl::openflow13::ofp_table_features))) {
				throw eTableFeaturesReqBadLen();
			}

			rofl::openflow::coftable_features table_features(get_version());
			table_features.unpack(buf, be16toh(table->length));
			add_table(table->table_id, table_features);

			buf += be16toh(table->length);
			buflen -= be16toh(table->length);
		}

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



rofl::openflow::coftable_features&
cofmsg_table_features::add_table(
		uint8_t table_id,
		rofl::openflow::coftable_features const& table_features)
{
	if (tables.find(table_id) != tables.end()) {
		drop_table(table_id);
	}
	return tables[table_id];
}



void
cofmsg_table_features::drop_table(uint8_t table_id)
{
	if (tables.find(table_id) == tables.end()) {
		return;
	}
	tables.erase(table_id);
}



rofl::openflow::coftable_features&
cofmsg_table_features::get_table(uint8_t table_id)
{
	if (tables.find(table_id) == tables.end()) {
		throw rofl::openflow::eOFTableFeaturesNotFound();
	}
	return tables[table_id];
}



rofl::openflow::coftable_features&
cofmsg_table_features::set_table(uint8_t table_id)
{
	return tables[table_id];
}



bool
cofmsg_table_features::has_table(uint8_t table_id)
{
	return (tables.find(table_id) != tables.end());
}





