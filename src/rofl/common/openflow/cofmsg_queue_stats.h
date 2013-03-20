/*
 * cofmsg_queue_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_QUEUE_STATS_H_
#define COFMSG_QUEUE_STATS_H_ 1

#include <vector>

#include "cofmsg_stats.h"
#include "cofqueuestats.h"

namespace rofl
{

//
// TODO: arrays of queue-stats !!!
//

/**
 *
 */
class cofmsg_queue_stats :
	public cofmsg_stats
{
private:

	std::vector<cofqueue_stats_reply> 	queue_stats;

	union {
		uint8_t*						ofhu_queue_stats;
		struct ofp10_queue_stats*		ofhu10_queue_stats;
		struct ofp12_queue_stats*		ofhu12_queue_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_queue_stats   			ofhu.ofhu_queue_stats
#define ofh10_queue_stats 			ofhu.ofhu10_queue_stats
#define ofh12_queue_stats 			ofhu.ofhu12_queue_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_queue_stats(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			std::vector<cofqueue_stats_reply> const& queue_stats = std::vector<cofqueue_stats_reply>(0));


	/**
	 *
	 */
	cofmsg_queue_stats(
			cofmsg_queue_stats const& stats);


	/**
	 *
	 */
	cofmsg_queue_stats&
	operator= (
			cofmsg_queue_stats const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_queue_stats() {};


	/**
	 *
	 */
	cofmsg_queue_stats(cmemory *memarea);


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
	std::vector<cofqueue_stats_reply>&
	get_queue_stats();
};

} // end of namespace rofl

#endif /* COFMSG_QUEUE_STATS_H_ */
