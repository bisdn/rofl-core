/*
 * coftablestats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFTABLESTATS_H_
#define COFTABLESTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/cvastring.h"
#include "rofl/platform/unix/csyslog.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl
{

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

public: // data structures

	friend std::ostream&
	operator<< (std::ostream& os, coftable_stats_reply const& tsr) {
		os << indent(0) << "<coftable_stats_reply ";
				os << "table-id:" << (int)(tsr.table_id) << " ";
				os << "name:" << tsr.name << " ";
				os << std::hex;
				os << "match:" << (unsigned long long)tsr.match << " ";
				os << "wildcards:" << (unsigned int)tsr.wildcards << " >" << std::endl;
				os << indent(2) << "<write-actions:" << (unsigned int)tsr.write_actions << " ";
				os << "apply-actions:" << (unsigned int)tsr.apply_actions << " ";
				os << "write-setfields:" << (unsigned long long)tsr.write_setfields << " ";
				os << "apply-setfields:" << (unsigned long long)tsr.apply_setfields << " >" << std::endl;;
				os << indent(2) << "<metadata-match:" << (unsigned long long)tsr.metadata_match << " ";
				os << "metadata-write:" << (unsigned long long)tsr.metadata_write << " ";
				os << "instructions:" << (unsigned int)tsr.instructions << " ";
				os << std::dec;
				os << "config:" << (unsigned int)tsr.config << " >" << std::endl;;
				os << indent(2) << "<max_entries:" << (unsigned int)tsr.max_entries << " ";
				os << "active-count:" << (unsigned int)tsr.active_count << " ";
				os << "lookup-count:" << (unsigned long long)tsr.lookup_count << " ";
				os << "matched-count:" << (unsigned long long)tsr.matched_count << " >" << std::endl;
		return os;
	};

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
	const char*
	c_str();


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
	std::string&
	get_name();

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
	uint32_t&
	get_config();

	/**
	 *
	 */
	uint32_t
	get_max_entries() const;

	/**
	 *
	 */
	uint32_t
	get_active_count() const;

	/**
	 *
	 */
	uint64_t&
	get_lookup_count();

	/**
	 *
	 */
	uint64_t&
	get_matched_count();

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
};

}

#endif /* COFTABLESTATS_H_ */
