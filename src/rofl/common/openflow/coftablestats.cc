#include "rofl/common/openflow/coftablestats.h"

#ifndef htobe16
#include "../endian_conversion.h"
#endif


using namespace rofl::openflow;



coftable_stats_reply::coftable_stats_reply(
		uint8_t of_version) :
				of_version(of_version),
				table_id(0),
				name(std::string("")),
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
				// not used by OF1.0
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
				// used by OF1.0
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


coftable_stats_reply::coftable_stats_reply(
		uint8_t of_version,
		uint8_t table_id,
		uint32_t active_count,
		uint64_t lookup_count,
		uint64_t matched_count) :
				of_version(of_version),
				table_id(table_id),
				// not used by OF1.3
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
				// used by OF1.3
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



bool
coftable_stats_reply::operator== (
		coftable_stats_reply const& ts) const
{
	return ((of_version 		== ts.of_version) &&
			(table_id 			== ts.table_id) &&
			(name 				== ts.name) &&
			(match 				== ts.match) &&
			(wildcards 			== ts.wildcards) &&
			(write_actions 		== ts.write_actions) &&
			(apply_actions 		== ts.apply_actions) &&
			(write_setfields 	== ts.write_setfields) &&
			(apply_setfields 	== ts.apply_setfields) &&
			(metadata_match 	== ts.metadata_match) &&
			(metadata_write 	== ts.metadata_write) &&
			(instructions 		== ts.instructions) &&
			(config 			== ts.config) &&
			(max_entries 		== ts.max_entries) &&
			(active_count 		== ts.active_count) &&
			(lookup_count 		== ts.lookup_count) &&
			(matched_count 		== ts.matched_count));
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
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return table_id;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_table_id(uint8_t table_id)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		this->table_id = table_id;
	} break;
	default:
		throw eBadVersion();
	}
}



std::string const&
coftable_stats_reply::get_name() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION: {
		return name;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_name(std::string const& name)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION: {
		this->name = name;
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
coftable_stats_reply::get_match() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return match;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_match(uint64_t match)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		this->match = match;
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
coftable_stats_reply::get_wildcards() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION: {
		return wildcards;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_wildcards(uint64_t wildcards)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION: {
		this->wildcards = wildcards;
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
coftable_stats_reply::get_write_actions() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return write_actions;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_write_actions(uint32_t write_actions)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		this->write_actions = write_actions;
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
coftable_stats_reply::get_apply_actions() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return apply_actions;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_apply_actions(uint32_t apply_actions)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		this->apply_actions = apply_actions;
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
coftable_stats_reply::get_write_setfields() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return write_setfields;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_write_setfields(uint64_t write_setfields)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		this->write_setfields = write_setfields;
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
coftable_stats_reply::get_apply_setfields() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return apply_setfields;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_apply_setfields(uint64_t apply_setfields)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		this->apply_setfields = apply_setfields;
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
coftable_stats_reply::get_metadata_match() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return metadata_match;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_metadata_match(uint64_t metadata_match)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		this->metadata_match = metadata_match;
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
coftable_stats_reply::get_metadata_write() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return metadata_write;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_metadata_write(uint64_t metadata_write)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		this->metadata_write = metadata_write;
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
coftable_stats_reply::get_instructions() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return instructions;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_instructions(uint32_t instructions)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		this->instructions = instructions;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_config(uint32_t config)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		this->config = config;
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
coftable_stats_reply::get_config() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return config;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_max_entries(uint32_t max_entries)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION: {
		this->max_entries = max_entries;
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
coftable_stats_reply::get_max_entries() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION: {
		return max_entries;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_active_count(uint32_t active_count)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		this->active_count = active_count;
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
coftable_stats_reply::get_active_count() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return active_count;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_lookup_count(uint64_t lookup_count)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		this->lookup_count = lookup_count;
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
coftable_stats_reply::get_lookup_count() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return lookup_count;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::set_matched_count(uint64_t matched_count)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		this->matched_count = matched_count;
	} break;
	default:
		throw eBadVersion();
	}
}



uint64_t
coftable_stats_reply::get_matched_count() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return matched_count;
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_table_stats))
			throw eInval();

		struct rofl::openflow10::ofp_table_stats *table_stats = (struct rofl::openflow10::ofp_table_stats*)buf;

		table_stats->table_id			= table_id;
		snprintf(table_stats->name, OFP_MAX_TABLE_NAME_LEN, name.c_str(), name.length());
		table_stats->wildcards 			= htobe32((uint32_t)(wildcards & 0x00000000ffffffff));
		table_stats->max_entries 		= htobe32(max_entries);
		table_stats->active_count 		= htobe32(active_count);
		table_stats->lookup_count 		= htobe64(lookup_count);
		table_stats->matched_count 		= htobe64(matched_count);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_table_stats))
			throw eInval();

		struct rofl::openflow12::ofp_table_stats *table_stats = (struct rofl::openflow12::ofp_table_stats*)buf;

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
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_table_stats))
			throw eInval();

		struct rofl::openflow13::ofp_table_stats *table_stats = (struct rofl::openflow13::ofp_table_stats*)buf;

		table_stats->table_id			= table_id;
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
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_table_stats))
			throw eInval();

		struct rofl::openflow10::ofp_table_stats *table_stats = (struct rofl::openflow10::ofp_table_stats*)buf;

		table_id 		= table_stats->table_id;
		name			= std::string(table_stats->name, strnlen(table_stats->name, OFP_MAX_TABLE_NAME_LEN));
		wildcards		= be32toh(table_stats->wildcards);
		max_entries		= be32toh(table_stats->max_entries);
		active_count	= be32toh(table_stats->active_count);
		lookup_count	= be64toh(table_stats->lookup_count);
		matched_count	= be64toh(table_stats->matched_count);

	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_table_stats))
			throw eInval();

		struct rofl::openflow12::ofp_table_stats *table_stats = (struct rofl::openflow12::ofp_table_stats*)buf;

		table_id 		= table_stats->table_id;
		name			= std::string(table_stats->name, strnlen(table_stats->name, OFP_MAX_TABLE_NAME_LEN));
		match			= be64toh(table_stats->match);
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
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_table_stats))
			throw eInval();

		struct rofl::openflow13::ofp_table_stats *table_stats = (struct rofl::openflow13::ofp_table_stats*)buf;

		table_id 		= table_stats->table_id;
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
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_table_stats));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_table_stats));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_table_stats));
	} break;
	default:
		throw eBadVersion();
	}
}



