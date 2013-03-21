/*
 * cofflowstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFGROUPFEATURESSTATS_H_
#define COFGROUPFEATURESSTATS_H_ 1

#include "../cmemory.h"
#include "cofmatch.h"
#include "cofaclist.h"
#include "cofinlist.h"
#include "../../platform/unix/csyslog.h"
#include "openflow.h"
#include "openflow_rofl_exceptions.h"

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
};


} /* end of namespace */

#endif /* COFGROUPFEATURESSTATS_H_ */
