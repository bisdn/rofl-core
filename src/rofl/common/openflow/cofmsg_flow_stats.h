/*
 * cofmsg_flow_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FLOW_STATS_H_
#define COFMSG_FLOW_STATS_H_ 1

#include "cofmsg_stats.h"
#include "cofflowstats.h"

namespace rofl
{

/**
 *
 */
class cofmsg_flow_stats :
	public cofmsg_stats
{
private:

	cofflow_stats_reply 		flow_stats;

	union {
		uint8_t*						ofhu_flow_stats;
		struct ofp10_flow_stats*		ofhu10_flow_stats;
		struct ofp12_flow_stats*		ofhu12_flow_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_flow_stats   			ofhu.ofhu_flow_stats
#define ofh10_flow_stats 			ofhu.ofhu10_flow_stats
#define ofh12_flow_stats 			ofhu.ofhu12_flow_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_flow_stats(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofflow_stats_reply const& flow_stats = cofflow_stats_reply());


	/**
	 *
	 */
	cofmsg_flow_stats(
			cofmsg_flow_stats const& stats);


	/**
	 *
	 */
	cofmsg_flow_stats&
	operator= (
			cofmsg_flow_stats const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_flow_stats() {};


	/**
	 *
	 */
	cofmsg_flow_stats(cmemory *memarea);


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
	cofflow_stats_reply&
	get_flow_stats();
};

} // end of namespace rofl

#endif /* COFMSG_FLOW_STATS_H_ */
