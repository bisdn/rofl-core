/*
 * cofmsg_queue_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_QUEUE_STATS_H_
#define COFMSG_QUEUE_STATS_H_ 1

#include <vector>

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofqueuestatsarray.h"
#include "rofl/common/openflow/cofqueuestats.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_queue_stats_request :
	public cofmsg_stats_request
{
private:

	rofl::openflow::cofqueue_stats_request		queue_stats;

public:


	/** constructor
	 *
	 */
	cofmsg_queue_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			rofl::openflow::cofqueue_stats_request const& queue_stats_request = rofl::openflow::cofqueue_stats_request());


	/**
	 *
	 */
	cofmsg_queue_stats_request(
			cofmsg_queue_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_queue_stats_request&
	operator= (
			cofmsg_queue_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_queue_stats_request();


	/**
	 *
	 */
	cofmsg_queue_stats_request(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual uint8_t*
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
	rofl::openflow::cofqueue_stats_request&
	set_queue_stats();

	/**
	 *
	 */
	rofl::openflow::cofqueue_stats_request const&
	get_queue_stats() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_queue_stats_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_queue_stats_request >" << std::endl;
		indent i(4);
		os << msg.queue_stats;
		return os;
	};
};




/**
 *
 */
class cofmsg_queue_stats_reply :
	public cofmsg_stats_reply
{
private:

	rofl::openflow::cofqueuestatsarray					queuestatsarray;

	union {
		uint8_t*										ofhu_queue_stats;
		struct rofl::openflow10::ofp_queue_stats*		ofhu10_queue_stats;
		struct rofl::openflow12::ofp_queue_stats*		ofhu12_queue_stats;
		struct rofl::openflow13::ofp_queue_stats*		ofhu13_queue_stats;
	} ofh_ofhu;

#define ofh_queue_stats   			ofh_ofhu.ofhu_queue_stats
#define ofh10_queue_stats 			ofh_ofhu.ofhu10_queue_stats
#define ofh12_queue_stats 			ofh_ofhu.ofhu12_queue_stats
#define ofh13_queue_stats 			ofh_ofhu.ofhu13_queue_stats

public:


	/** constructor
	 *
	 */
	cofmsg_queue_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			rofl::openflow::cofqueuestatsarray const& queue_stats = rofl::openflow::cofqueuestatsarray());


	/**
	 *
	 */
	cofmsg_queue_stats_reply(
			cofmsg_queue_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_queue_stats_reply&
	operator= (
			cofmsg_queue_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_queue_stats_reply();


	/**
	 *
	 */
	cofmsg_queue_stats_reply(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual uint8_t*
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
	rofl::openflow::cofqueuestatsarray&
	set_queue_stats_array() { return queuestatsarray; };

	/**
	 *
	 */
	rofl::openflow::cofqueuestatsarray const&
	get_queue_stats_array() const { return queuestatsarray; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_queue_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_queue_stats_reply >" << std::endl;
		indent i(4);
		os << msg.queuestatsarray;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_QUEUE_STATS_H_ */
