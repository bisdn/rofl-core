/*
 * coftablestats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFTABLESTATS_H_
#define COFTABLESTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class eTableStatsBase			: public RoflException {};
class eTableStatsInval			: public eTableStatsBase {};
class eTableStatsNotFound		: public eTableStatsBase {};

class coftable_stats_reply
{
private: // data structures

	std::string		info;
	uint8_t 		of_version;
	uint8_t			table_id;
	std::string		name;
	uint64_t 		match;
	uint64_t		wildcards;
	uint32_t		write_actions;
	uint32_t 		apply_actions;
	uint64_t		write_setfields;
	uint64_t		apply_setfields;
	uint64_t		metadata_match;
	uint64_t		metadata_write;
	uint32_t		instructions;
	uint32_t		config;
	uint32_t		max_entries;
	uint32_t		active_count;
	uint64_t		lookup_count;
	uint64_t		matched_count;


public:
	/**
	 *
	 */
	coftable_stats_reply(
			uint8_t of_version = 0);

	/** OF1.0
	 *
	 */
	coftable_stats_reply(
			uint8_t of_version,
			uint8_t table_id,
			std::string name,
			uint32_t wildcards,
			uint32_t max_entries,
			uint32_t active_count,
			uint64_t lookup_count,
			uint64_t matched_count);

	/** OF1.2
	 *
	 */
	coftable_stats_reply(
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
			uint64_t matched_count);

	/** OF1.3
	 *
	 */
	coftable_stats_reply(
			uint8_t of_version,
			uint8_t table_id,
			uint32_t active_count,
			uint64_t lookup_count,
			uint64_t matched_count);

	/**
	 *
	 */
	virtual
	~coftable_stats_reply();


	/**
	 *
	 */
	coftable_stats_reply(
			coftable_stats_reply const& table_stats);

	/**
	 *
	 */
	coftable_stats_reply&
	operator= (
			coftable_stats_reply const& table_stats);

	/**
	 *
	 */
	bool
	operator== (
			coftable_stats_reply const& table_stats) const;

	/**
	 *
	 */
	size_t
	length() const;


	/**
	 *
	 */
	void
	set_version(uint8_t of_version);


	/**
	 *
	 */
	uint8_t
	get_version() const;


	/**
	 *
	 */
	uint8_t
	get_table_id() const;

	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id);

	/**
	 *
	 */
	std::string const&
	get_name() const;

	/**
	 *
	 */
	void
	set_name(std::string const& name);

	/**
	 *
	 */
	uint64_t
	get_match() const;

	/**
	 *
	 */
	void
	set_match(uint64_t match);

	/**
	 *
	 */
	uint64_t
	get_wildcards() const;

	/**
	 *
	 */
	void
	set_wildcards(uint64_t wildcards);

	/**
	 *
	 */
	uint32_t
	get_write_actions() const;

	/**
	 *
	 */
	void
	set_write_actions(uint32_t write_actions);

	/**
	 *
	 */
	uint32_t
	get_apply_actions() const;

	/**
	 *
	 */
	void
	set_apply_actions(uint32_t apply_actions);

	/**
	 *
	 */
	uint64_t
	get_write_setfields() const;

	/**
	 *
	 */
	void
	set_write_setfields(uint64_t write_setfields);

	/**
	 *
	 */
	uint64_t
	get_apply_setfields() const;

	/**
	 *
	 */
	void
	set_apply_setfields(uint64_t apply_setfields);

	/**
	 *
	 */
	uint64_t
	get_metadata_match() const;

	/**
	 *
	 */
	void
	set_metadata_match(uint64_t metadata_match);

	/**
	 *
	 */
	uint64_t
	get_metadata_write() const;

	/**
	 *
	 */
	void
	set_metadata_write(uint64_t metadata_write);

	/**
	 *
	 */
	uint32_t
	get_instructions() const;

	/**
	 *
	 */
	void
	set_instructions(uint32_t instructions);

	/**
	 *
	 */
	void
	set_config(uint32_t config);

	/**
	 *
	 */
	uint32_t
	get_config() const;

	/**
	 *
	 */
	void
	set_max_entries(uint32_t max_entries);

	/**
	 *
	 */
	uint32_t
	get_max_entries() const;

	/**
	 *
	 */
	void
	set_active_count(uint32_t active_count);

	/**
	 *
	 */
	uint32_t
	get_active_count() const;

	/**
	 *
	 */
	void
	set_lookup_count(uint64_t lookup_count);

	/**
	 *
	 */
	uint64_t
	get_lookup_count() const;

	/**
	 *
	 */
	void
	set_matched_count(uint64_t matched_count);

	/**
	 *
	 */
	uint64_t
	get_matched_count() const;

	/**
	 *
	 */
	void
	pack(uint8_t *buf, size_t buflen) const;

	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftable_stats_reply const& tsr) {
		switch (tsr.of_version) {
		case rofl::openflow10::OFP_VERSION: {
			os << indent(0) << "<coftable_stats_reply ";
					os << "table-id:" << (int)(tsr.table_id) << " ";
					os << "name:" << tsr.name << " ";
					os << std::hex;
					os << "match: 0x" << (unsigned long long)tsr.match << " ";
					os << "wildcards: 0x" << (unsigned long long)tsr.wildcards << " >" << std::endl;

					os << indent(2) << "<max_entries: 0x" << (unsigned int)tsr.max_entries << " ";
					os << "active-count: 0x" << (unsigned int)tsr.active_count << " ";
					os << "lookup-count: 0x" << (unsigned long long)tsr.lookup_count << " ";
					os << "matched-count: 0x" << (unsigned long long)tsr.matched_count << " ";
					os << std::dec;
					os << ">" << std::endl;
		} break;
		case rofl::openflow12::OFP_VERSION: {
			os << indent(0) << "<coftable_stats_reply ";
					os << "table-id:" << (int)(tsr.table_id) << " ";
					os << "name:" << tsr.name << " ";
					os << std::hex;
					os << "match: 0x" << (unsigned long long)tsr.match << " ";
					os << "wildcards: 0x" << (unsigned long long)tsr.wildcards << " >" << std::endl;
					os << indent(2) << "<write-actions: 0x" << (unsigned int)tsr.write_actions << " ";
					os << "apply-actions: 0x" << (unsigned int)tsr.apply_actions << " ";
					os << "write-setfields: 0x" << (unsigned long long)tsr.write_setfields << " ";
					os << "apply-setfields: 0x" << (unsigned long long)tsr.apply_setfields << " >" << std::endl;;
					os << indent(2) << "<metadata-match: 0x" << (unsigned long long)tsr.metadata_match << " ";
					os << "metadata-write: 0x" << (unsigned long long)tsr.metadata_write << " ";
					os << "instructions: 0x" << (unsigned int)tsr.instructions << " ";
					os << std::dec;
					os << "config:" << (unsigned int)tsr.config << " >" << std::endl;;
					os << indent(2) << "<max_entries:" << (unsigned int)tsr.max_entries << " ";
					os << "active-count:" << (unsigned int)tsr.active_count << " ";
					os << "lookup-count:" << (unsigned long long)tsr.lookup_count << " ";
					os << "matched-count:" << (unsigned long long)tsr.matched_count << " >" << std::endl;
		} break;
		case rofl::openflow13::OFP_VERSION: {
			os << indent(0) << "<coftable_stats_reply ofp-version:" << (int)tsr.of_version << " >" << std::endl;
			os << std::hex;
			os << indent(2) << "<table-id: 0x" << (int)(tsr.table_id) << " >" << std::endl;
			os << indent(2) << "<active-count: 0x" << (unsigned int)tsr.active_count << " >" << std::endl;
			os << indent(2) << "<lookup-count: 0x" << (unsigned long long)tsr.lookup_count << " >" << std::endl;
			os << indent(2) << "<matched-count: 0x" << (unsigned long long)tsr.matched_count << " >" << std::endl;
			os << std::dec;
		} break;
		default: {
			os << indent(0) << "<coftable_stats_reply >";
		};
		}
		return os;
	};
};

}
}

#endif /* COFTABLESTATS_H_ */
