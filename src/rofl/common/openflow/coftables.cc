/*
 * coftables.cc
 *
 *  Created on: 16.02.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/coftables.h"

using namespace rofl::openflow;

coftables::coftables(
			uint8_t ofp_version) :
					ofp_version(ofp_version)
{

}



coftables::~coftables()
{

}



coftables::coftables(
			coftables const& tables)
{
	*this = tables;
}



coftables&
coftables::operator= (
			coftables const& tables)
{
	if (this == &tables)
		return *this;

	ofp_version = tables.ofp_version;

	this->tables.clear();
	for (std::map<uint8_t, coftable_features>::const_iterator
			it = tables.tables.begin(); it != tables.tables.end(); ++it) {
		this->tables[it->first] = it->second;
	}

	return *this;
}



void
coftables::clear()
{
	tables.clear();
}



size_t
coftables::length() const
{
	size_t len = 0;
	for (std::map<uint8_t, coftable_features>::const_iterator
			it = tables.begin(); it != tables.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
coftables::pack(
			uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eOFTablesInval();
	}

	for (std::map<uint8_t, coftable_features>::iterator
			it = tables.begin(); it != tables.end(); ++it) {
		it->second.pack(buf, it->second.length());
		buf += it->second.length();
	}
}



void
coftables::unpack(
			uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	tables.clear();
	while (buflen > 0) {
		struct rofl::openflow13::ofp_table_features *table = (struct rofl::openflow13::ofp_table_features*)buf;

		if ((be16toh(table->length) > buflen) || (be16toh(table->length) < sizeof(struct rofl::openflow13::ofp_table_features))) {
			throw eTableFeaturesReqBadLen();
		}

		rofl::openflow::coftable_features table_features(get_version());
		table_features.unpack(buf, be16toh(table->length));
		tables[table->table_id] = table_features;

		buf += be16toh(table->length);
		buflen -= be16toh(table->length);
	}
}



coftable_features&
coftables::add_table(uint8_t table_id)
{
	if (tables.find(table_id) != tables.end()) {
		tables.erase(table_id);
	}
	tables[table_id] = coftable_features(get_version());
	tables[table_id].set_table_id(table_id);
	return tables[table_id];
}



void
coftables::drop_table(uint8_t table_id)
{
	if (tables.find(table_id) == tables.end()) {
		return;
	}
	tables.erase(table_id);
}



coftable_features&
coftables::get_table(uint8_t table_id)
{
	if (tables.find(table_id) == tables.end()) {
		throw eOFTablesNotFound();
	}
	return tables[table_id];
}



coftable_features&
coftables::set_table(uint8_t table_id)
{
	if (tables.find(table_id) == tables.end()) {
		tables[table_id] = coftable_features(get_version());
		tables[table_id].set_table_id(table_id);
	}
	return tables[table_id];
}



bool
coftables::has_table(uint8_t table_id)
{
	return (tables.find(table_id) != tables.end());
}



