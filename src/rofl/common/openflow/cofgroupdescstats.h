/*
 * cofgroupdescstats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFGROUPDESCSTATS_H_
#define COFGROUPDESCSTATS_H_ 1

#include "../cmemory.h"
#include "../../platform/unix/csyslog.h"
#include "openflow.h"
#include "openflow_rofl_exceptions.h"
#include "cofbuckets.h"

namespace rofl
{



class cofgroup_desc_stats_reply
{
private: // data structures

	uint8_t 		of_version;
	uint8_t			type;
	uint32_t		group_id;
	cofbuckets		buckets;

public: // data structures


public:
	/**
	 *
	 */
	cofgroup_desc_stats_reply(
			uint8_t of_version = 0);

	/**
	 *
	 */
	cofgroup_desc_stats_reply(
			uint8_t of_version,
			uint8_t type,
			uint32_t group_id,
			cofbuckets const& buckets);

	/**
	 *
	 */
	virtual
	~cofgroup_desc_stats_reply();


	/**
	 *
	 */
	cofgroup_desc_stats_reply(
			cofgroup_desc_stats_reply const& stats);

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	operator= (
			cofgroup_desc_stats_reply const& stats);


	/**
	 *
	 */
	void
	pack(uint8_t *buf, size_t buflen);

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
	get_version() const { return of_version; };

	/**
	 *
	 */
	uint8_t
	get_type() const { return type; };

	/**
	 *
	 */
	uint32_t
	get_group_id() const { return group_id; };

	/**
	 *
	 */
	cofbuckets&
	get_buckets() { return buckets; };

	/**
	 *
	 */
	void
	set_version(uint8_t of_version) { this->of_version = of_version; };

	/**
	 *
	 */
	void
	set_type(uint8_t type) { this->type = type; };

	/**
	 *
	 */
	void
	set_group_id(uint32_t group_id) { this->group_id = group_id; };

	/**
	 *
	 */
	void
	set_buckets(cofbuckets const& buckets) { this->buckets = buckets; };
};

}

#endif /* COFGROUPDESCSTATS_H_ */
