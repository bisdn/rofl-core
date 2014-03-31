/*
 * cofmsg_flow_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FLOW_STATS_H_
#define COFMSG_FLOW_STATS_H_ 1

#include <vector>

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofflowstats.h"
#include "rofl/common/openflow/cofflowstatsarray.h"


namespace rofl {
namespace openflow {

//
// TODO: arrays of flow-stats !!!
//


/**
 *
 */
class cofmsg_flow_stats_request :
	public cofmsg_stats_request
{
private:

	cofflow_stats_request 		flow_stats;

	union {
		uint8_t*										ofhu_flow_stats;
		struct rofl::openflow10::ofp_flow_stats*		ofhu10_flow_stats;
		struct rofl::openflow12::ofp_flow_stats*		ofhu12_flow_stats;
		struct rofl::openflow13::ofp_flow_stats*		ofhu13_flow_stats;
	} ofhu;

#define ofh_flow_stats   			ofhu.ofhu_flow_stats
#define ofh10_flow_stats 			ofhu.ofhu10_flow_stats
#define ofh12_flow_stats 			ofhu.ofhu12_flow_stats
#define ofh13_flow_stats 			ofhu.ofhu13_flow_stats

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
	cofflow_stats_request&
	set_flow_stats();


	/**
	 *
	 */
	cofflow_stats_request const&
	get_flow_stats() const;


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_flow_stats_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_flow_stats_request >" << std::endl;
		indent i(4);
		os << msg.flow_stats;
		return os;
	};
};



/**
 *
 */
class cofmsg_flow_stats_reply :
	public cofmsg_stats_reply
{
private:

	rofl::openflow::cofflowstatsarray					flowstatsarray;

	union {
		uint8_t*										ofhu_flow_stats;
		struct rofl::openflow10::ofp_flow_stats*		ofhu10_flow_stats;
		struct rofl::openflow12::ofp_flow_stats*		ofhu12_flow_stats;
		struct rofl::openflow13::ofp_flow_stats*		ofhu13_flow_stats;
	} ofhu;

#define ofh_flow_stats   			ofhu.ofhu_flow_stats
#define ofh10_flow_stats 			ofhu.ofhu10_flow_stats
#define ofh12_flow_stats 			ofhu.ofhu12_flow_stats
#define ofh13_flow_stats 			ofhu.ofhu13_flow_stats

public:


	/** constructor
	 *
	 */
	cofmsg_flow_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			rofl::openflow::cofflowstatsarray const& flow_stats = rofl::openflow::cofflowstatsarray());


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
	rofl::openflow::cofflowstatsarray&
	set_flow_stats_array() { return flowstatsarray; };


	/**
	 *
	 */
	rofl::openflow::cofflowstatsarray const&
	get_flow_stats_array() const { return flowstatsarray; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_flow_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_flow_stats_reply >" << std::endl;
		indent i(4);
		os << msg.flowstatsarray;
		return os;
	};
};

} // end of namespace rofl
} // end of namespace rofl

#endif /* COFMSG_FLOW_STATS_H_ */
