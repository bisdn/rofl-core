/*
 * cofmsg_desc_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_DESC_STATS_H_
#define COFMSG_DESC_STATS_H_ 1

#include "cofmsg_stats.h"
#include "cofdescstats.h"

namespace rofl
{



/**
 *
 */
class cofmsg_desc_stats_request :
	public cofmsg_stats
{
private:

public:


	/** constructor
	 *
	 */
	cofmsg_desc_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0);


	/**
	 *
	 */
	cofmsg_desc_stats_request(
			cofmsg_desc_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_desc_stats_request&
	operator= (
			cofmsg_desc_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_desc_stats_request();


	/**
	 *
	 */
	cofmsg_desc_stats_request(cmemory *memarea);


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
};



/**
 *
 */
class cofmsg_desc_stats_reply :
	public cofmsg_stats
{
private:

	cofdesc_stats_reply 		desc_stats;

	union {
		uint8_t*						ofhu_desc_stats;
		struct ofp10_desc_stats*		ofhu10_desc_stats;
		struct ofp12_desc_stats*		ofhu12_desc_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_desc_stats   			ofhu.ofhu_desc_stats
#define ofh10_desc_stats 			ofhu.ofhu10_desc_stats
#define ofh12_desc_stats 			ofhu.ofhu12_desc_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_desc_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofdesc_stats_reply const& desc_stats = cofdesc_stats_reply());


	/**
	 *
	 */
	cofmsg_desc_stats_reply(
			cofmsg_desc_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_desc_stats_reply&
	operator= (
			cofmsg_desc_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_desc_stats_reply();


	/**
	 *
	 */
	cofmsg_desc_stats_reply(cmemory *memarea);


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
	cofdesc_stats_reply&
	get_desc_stats();
};

} // end of namespace rofl

#endif /* COFMSG_DESC_STATS_H_ */
