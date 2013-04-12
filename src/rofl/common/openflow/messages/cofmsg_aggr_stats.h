/*
 * cofmsg_aggr_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_AGGR_STATS_H_
#define COFMSG_AGGR_STATS_H_ 1

#include "cofmsg_stats.h"
#include "rofl/common/openflow/cofaggrstats.h"

namespace rofl
{

/**
 *
 */
class cofmsg_aggr_stats_request :
	public cofmsg_stats
{
private:

	cofaggr_stats_request 		aggr_stats;

	union {
		uint8_t*								ofhu_aggr_stats;
		struct ofp10_aggregate_stats_request*	ofhu10_aggr_stats;
		struct ofp12_aggregate_stats_request*	ofhu12_aggr_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_aggr_stats   			ofhu.ofhu_aggr_stats
#define ofh10_aggr_stats 			ofhu.ofhu10_aggr_stats
#define ofh12_aggr_stats 			ofhu.ofhu12_aggr_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_aggr_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofaggr_stats_request const& aggr_stats = cofaggr_stats_request());


	/**
	 *
	 */
	cofmsg_aggr_stats_request(
			cofmsg_aggr_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_aggr_stats_request&
	operator= (
			cofmsg_aggr_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_aggr_stats_request();


	/**
	 *
	 */
	cofmsg_aggr_stats_request(cmemory *memarea);


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
	cofaggr_stats_request&
	get_aggr_stats();
};



/**
 *
 */
class cofmsg_aggr_stats_reply :
	public cofmsg_stats
{
private:

	cofaggr_stats_reply 		aggr_stats;

	union {
		uint8_t*								ofhu_aggr_stats;
		struct ofp10_aggregate_stats_reply*		ofhu10_aggr_stats;
		struct ofp12_aggregate_stats_reply*		ofhu12_aggr_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_aggr_stats   			ofhu.ofhu_aggr_stats
#define ofh10_aggr_stats 			ofhu.ofhu10_aggr_stats
#define ofh12_aggr_stats 			ofhu.ofhu12_aggr_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_aggr_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofaggr_stats_reply const& aggr_stats = cofaggr_stats_reply());


	/**
	 *
	 */
	cofmsg_aggr_stats_reply(
			cofmsg_aggr_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_aggr_stats_reply&
	operator= (
			cofmsg_aggr_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_aggr_stats_reply();


	/**
	 *
	 */
	cofmsg_aggr_stats_reply(cmemory *memarea);


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
	cofaggr_stats_reply&
	get_aggr_stats();
};

} // end of namespace rofl

#endif /* COFMSG_AGGR_STATS_H_ */
