#include "rofl/common/openflow/coftablestats.h"

using namespace rofl;



coftable_stats_reply::coftable_stats_reply(
		uint8_t of_version) :
				of_version(of_version),
				table_id(0),
				name(0),
				match(0),
				wildcards(0),
				write_actions(0),
				apply_actions(0),
				write_setfields(0),
				apply_setfields(0),
				metadata_match(0),
				metadata_write(0),
				instructions(0),
				config(0),
				max_entries(0),
				active_count(0),
				lookup_count(0),
				matched_count(0)
{}



coftable_stats_reply::coftable_stats_reply(
		uint8_t of_version,
		uint8_t table_id,
		std::string name,
		uint32_t wildcards,
		uint32_t max_entries,
		uint32_t active_count,
		uint64_t lookup_count,
		uint64_t matched_count) :
				of_version(of_version),
				table_id(table_id),
				name(name),
				match(0),
				wildcards(wildcards),
				write_actions(0),
				apply_actions(0),
				write_setfields(0),
				apply_setfields(0),
				metadata_match(0),
				metadata_write(0),
				instructions(0),
				config(0),
				max_entries(max_entries),
				active_count(active_count),
				lookup_count(lookup_count),
				matched_count(matched_count)
{}



coftable_stats_reply::coftable_stats_reply(
		uint8_t of_version,
		uint8_t table_id,
		std::string name,
		uint64_t match,
		uint64_t wildcards,
		uint32_t write_actions,
		uint32_t apply_actions,
		uint64_t write_setfields,
		uint64_t apply_setfields,
		uint64_t metadata_match,
		uint64_t metadata_write,
		uint32_t instructions,
		uint32_t config,
		uint32_t max_entries,
		uint32_t active_count,
		uint64_t lookup_count,
		uint64_t matched_count) :
				of_version(of_version),
				table_id(table_id),
				name(name),
				match(match),
				wildcards(wildcards),
				write_actions(write_actions),
				apply_actions(apply_actions),
				write_setfields(write_setfields),
				apply_setfields(apply_setfields),
				metadata_match(metadata_match),
				metadata_write(metadata_write),
				instructions(instructions),
				config(config),
				max_entries(max_entries),
				active_count(active_count),
				lookup_count(lookup_count),
				matched_count(matched_count)
{}



coftable_stats_reply::~coftable_stats_reply()
{}



coftable_stats_reply::coftable_stats_reply(
		coftable_stats_reply const& descstats)
{
	*this = descstats;
}



coftable_stats_reply&
coftable_stats_reply::operator= (
		coftable_stats_reply const& table_stats)
{
	if (this == &table_stats)
		return *this;

	of_version 		= table_stats.of_version;
	table_id		= table_stats.table_id;
	name			= table_stats.name;
	match			= table_stats.match;
	wildcards		= table_stats.wildcards;
	write_actions	= table_stats.write_actions;
	apply_actions	= table_stats.apply_actions;
	write_setfields	= table_stats.write_setfields;
	apply_setfields	= table_stats.apply_setfields;
	metadata_match	= table_stats.metadata_match;
	metadata_write	= table_stats.metadata_write;
	instructions	= table_stats.instructions;
	config			= table_stats.config;
	max_entries		= table_stats.max_entries;
	active_count	= table_stats.active_count;
	lookup_count	= table_stats.lookup_count;
	matched_count	= table_stats.matched_count;

	return *this;
}



void
coftable_stats_reply::set_version(uint8_t of_version)
{
	this->of_version = of_version;
}



uint8_t
coftable_stats_reply::get_version() const
{
	return of_version;
}



uint8_t
coftable_stats_reply::get_table_id() const
{
	return table_id;
}



std::string&
coftable_stats_reply::get_name()
{
	return name;
}



uint64_t
coftable_stats_reply::get_match() const
{
	return match;
}



uint64_t
coftable_stats_reply::get_wildcards() const
{
	return wildcards;
}



uint32_t
coftable_stats_reply::get_write_actions() const
{
	return write_actions;
}



uint32_t
coftable_stats_reply::get_apply_actions() const
{
	return apply_actions;
}



uint64_t
coftable_stats_reply::get_write_setfields() const
{
	return write_setfields;
}



uint64_t
coftable_stats_reply::get_apply_setfields() const
{
	return apply_setfields;
}



uint64_t
coftable_stats_reply::get_metadata_match() const
{
	return metadata_match;
}



uint64_t
coftable_stats_reply::get_metadata_write() const
{
	return metadata_write;
}



uint32_t
coftable_stats_reply::get_instructions() const
{
	return instructions;
}



uint32_t&
coftable_stats_reply::get_config()
{
	return config;
}



uint32_t
coftable_stats_reply::get_max_entries() const
{
	return max_entries;
}



uint32_t
coftable_stats_reply::get_active_count() const
{
	return active_count;
}



uint64_t&
coftable_stats_reply::get_lookup_count()
{
	return lookup_count;
}



uint64_t&
coftable_stats_reply::get_matched_count()
{
	return matched_count;
}



void
coftable_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_table_stats))
			throw eInval();

		struct ofp10_table_stats *table_stats = (struct ofp10_table_stats*)buf;

		table_stats->table_id			= table_id;
		snprintf(table_stats->name, OFP_MAX_TABLE_NAME_LEN, name.c_str(), name.length());
		table_stats->wildcards 			= htobe32((uint32_t)(wildcards & 0x00000000ffffffff));
		table_stats->max_entries 		= htobe32(max_entries);
		table_stats->active_count 		= htobe32(active_count);
		table_stats->lookup_count 		= htobe64(lookup_count);
		table_stats->matched_count 		= htobe64(matched_count);

	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_table_stats))
			throw eInval();

		struct ofp12_table_stats *table_stats = (struct ofp12_table_stats*)buf;

		table_stats->table_id			= table_id;
		snprintf(table_stats->name, OFP_MAX_TABLE_NAME_LEN, name.c_str(), name.length());
		table_stats->match				= htobe64(match);
		table_stats->wildcards 			= htobe64(wildcards);
		table_stats->write_actions		= htobe32(write_actions);
		table_stats->apply_actions		= htobe32(apply_actions);
		table_stats->write_setfields	= htobe64(write_setfields);
		table_stats->apply_setfields	= htobe64(apply_setfields);
		table_stats->metadata_match		= htobe64(metadata_match);
		table_stats->metadata_write		= htobe64(metadata_write);
		table_stats->instructions 		= htobe32(instructions);
		table_stats->config		 		= htobe32(config);
		table_stats->max_entries 		= htobe32(max_entries);
		table_stats->active_count 		= htobe32(active_count);
		table_stats->lookup_count 		= htobe64(lookup_count);
		table_stats->matched_count 		= htobe64(matched_count);

	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_table_stats))
			throw eInval();

		struct ofp10_table_stats *table_stats = (struct ofp10_table_stats*)buf;

		table_id 		= table_stats->table_id;
		name			= std::string(table_stats->name, OFP_MAX_TABLE_NAME_LEN);
		wildcards		= be32toh(table_stats->wildcards);
		max_entries		= be32toh(table_stats->max_entries);
		active_count	= be32toh(table_stats->active_count);
		lookup_count	= be64toh(table_stats->lookup_count);
		matched_count	= be64toh(table_stats->matched_count);

	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_table_stats))
			throw eInval();

		struct ofp12_table_stats *table_stats = (struct ofp12_table_stats*)buf;

		table_id 		= table_stats->table_id;
		name			= std::string(table_stats->name, OFP_MAX_TABLE_NAME_LEN);
		wildcards		= be64toh(table_stats->wildcards);
		write_actions	= be32toh(table_stats->write_actions);
		apply_actions	= be32toh(table_stats->apply_actions);
		write_setfields	= be64toh(table_stats->write_setfields);
		apply_setfields	= be64toh(table_stats->apply_setfields);
		metadata_match	= be64toh(table_stats->metadata_match);
		metadata_write	= be64toh(table_stats->metadata_write);
		instructions	= be32toh(table_stats->instructions);
		config			= be32toh(table_stats->config);
		max_entries		= be32toh(table_stats->max_entries);
		active_count	= be32toh(table_stats->active_count);
		lookup_count	= be64toh(table_stats->lookup_count);
		matched_count	= be64toh(table_stats->matched_count);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
coftable_stats_reply::length() const
{
	switch (of_version) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_table_stats));
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_table_stats));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}




