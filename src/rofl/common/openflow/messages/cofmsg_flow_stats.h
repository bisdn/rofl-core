/*
 * cofmsg_flow_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FLOW_STATS_H_
#define COFMSG_FLOW_STATS_H_ 1

#include <vector>

#include "cofmsg_stats.h"
#include "rofl/common/openflow/cofflowstats.h"

namespace rofl
{

//
// TODO: arrays of flow-stats !!!
//


/**
 *
 */
class cofmsg_flow_stats_request :
	public cofmsg_stats
{
private:

	cofflow_stats_request 		flow_stats;

	union {
		uint8_t*						ofhu_flow_stats;
		struct openflow10::ofp_flow_stats*		ofhu10_flow_stats;
		struct openflow12::ofp_flow_stats*		ofhu12_flow_stats;
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
	cofmsg_flow_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofflow_stats_request const& flow_stats = cofflow_stats_request());


	/**
	 *
	 */
	cofmsg_flow_stats_request(
			cofmsg_flow_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_flow_stats_request&
	operator= (
			cofmsg_flow_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_flow_stats_request();


	/**
	 *
	 */
	cofmsg_flow_stats_request(cmemory *memarea);


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
	cofflow_stats_request&
	get_flow_stats();
};



/**
 *
 */
class cofmsg_flow_stats_reply :
	public cofmsg_stats
{
private:

	std::vector<cofflow_stats_reply> 	flow_stats;

	union {
		uint8_t*						ofhu_flow_stats;
		struct openflow10::ofp_flow_stats*		ofhu10_flow_stats;
		struct openflow12::ofp_flow_stats*		ofhu12_flow_stats;
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
	cofmsg_flow_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			std::vector<cofflow_stats_reply> const& flow_stats = std::vector<cofflow_stats_reply>(0));


	/**
	 *
	 */
	cofmsg_flow_stats_reply(
			cofmsg_flow_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_flow_stats_reply&
	operator= (
			cofmsg_flow_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_flow_stats_reply();


	/**
	 *
	 */
	cofmsg_flow_stats_reply(cmemory *memarea);


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
	std::vector<cofflow_stats_reply>&
	get_flow_stats();
};

} // end of namespace rofl

#endif /* COFMSG_FLOW_STATS_H_ */
