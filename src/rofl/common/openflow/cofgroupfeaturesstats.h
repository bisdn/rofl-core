/*
 * cofflowstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFGROUPFEATURESSTATS_H_
#define COFGROUPFEATURESSTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/platform/unix/csyslog.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl
{

class cofgroup_features_stats_reply
{
private: // data structures

	uint8_t 				of_version;
	uint32_t				types;
	uint32_t				capabilities;
	std::vector<uint32_t>	max_groups;
	std::vector<uint32_t>	actions;

public: // data structures


public:
	/**
	 *
	 */
	cofgroup_features_stats_reply(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);


	/**
	 *
	 */
	cofgroup_features_stats_reply(
			uint8_t of_version,
			uint32_t types,
			uint32_t capabilities,
			std::vector<uint32_t> const& max_groups,
			std::vector<uint32_t> const& actions);


	/**
	 *
	 */
	virtual
	~cofgroup_features_stats_reply();


	/**
	 *
	 */
	cofgroup_features_stats_reply(
			cofgroup_features_stats_reply const& group_features_stats);

	/**
	 *
	 */
	cofgroup_features_stats_reply&
	operator= (
			cofgroup_features_stats_reply const& group_features_stats);


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


	/**
	 *
	 */
	size_t
	length() const;


public:


	/**
	 *
	 */
	uint8_t
	get_version() const;


	/**
	 *
	 */
	void
	set_version(uint8_t of_version);


	/**
	 *
	 */
	uint32_t
	get_types() const;


	/**
	 *
	 */
	void
	set_types(uint32_t types);


	/**
	 *
	 */
	uint32_t
	get_capabilities() const;


	/**
	 *
	 */
	void
	set_capabilities(uint32_t capabilities);


	/**
	 *
	 */
	std::vector<uint32_t>&
	get_max_groups();


	/**
	 *
	 */
	std::vector<uint32_t>&
	get_actions();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroup_features_stats_reply const& reply) {
		os << indent(0) << "<cofgroup_features_stats_reply >" << std::endl;
		os << indent(2) << "<types: 0x" << std::hex << (int)reply.get_types() << std::dec << " >" << std::endl;
		os << indent(2) << "<capabilities: 0x" << std::hex << (int)reply.get_capabilities() << std::dec << " >" << std::endl;
		for (unsigned int i = 0; i < reply.max_groups.size(); ++i) {
			os << indent(2) << "<max-groups[" << i << "]: " << (int)reply.max_groups[i] << " >" << std::endl;
		}
		for (unsigned int i = 0; i < reply.actions.size(); ++i) {
			os << indent(2) << "<actions[" << i << "]: 0x" << std::hex << (int)reply.max_groups[i] << std::dec << " >" << std::endl;
		}
		return os;
	};
};


} /* end of namespace */

#endif /* COFGROUPFEATURESSTATS_H_ */
