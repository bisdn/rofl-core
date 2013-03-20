/*
 * cofmsg_group_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_GROUP_STATS_H_
#define COFMSG_GROUP_STATS_H_ 1

#include <vector>

#include "cofmsg_stats.h"
#include "cofgroupstats.h"

namespace rofl
{

//
// TODO: arrays of group-stats !!!
//

/**
 *
 */
class cofmsg_group_stats :
	public cofmsg_stats
{
private:

	std::vector<cofgroup_stats_reply> 	group_stats;

	union {
		uint8_t*						ofhu_group_stats;
		struct ofp10_group_stats*		ofhu10_group_stats;
		struct ofp12_group_stats*		ofhu12_group_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_group_stats   			ofhu.ofhu_group_stats
#define ofh10_group_stats 			ofhu.ofhu10_group_stats
#define ofh12_group_stats 			ofhu.ofhu12_group_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_group_stats(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			std::vector<cofgroup_stats_reply> const& group_stats = std::vector<cofgroup_stats_reply>(0));


	/**
	 *
	 */
	cofmsg_group_stats(
			cofmsg_group_stats const& stats);


	/**
	 *
	 */
	cofmsg_group_stats&
	operator= (
			cofmsg_group_stats const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_group_stats() {};


	/**
	 *
	 */
	cofmsg_group_stats(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual void
	resize(size_t len);


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();


public:


	/**
	 *
	 */
	std::vector<cofgroup_stats_reply>&
	get_group_stats();
};

} // end of namespace rofl

#endif /* COFMSG_GROUP_STATS_H_ */
