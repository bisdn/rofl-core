/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * csegmsg.h
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#ifndef CSEGMSG_H_
#define CSEGMSG_H_

#include <inttypes.h>

#include <iostream>

#include "rofl/common/cclock.h"
#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/messages/cofmsg_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_flow_stats.h"
#include "rofl/common/openflow/messages/cofmsg_aggr_stats.h"
#include "rofl/common/openflow/messages/cofmsg_table_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_stats.h"
#include "rofl/common/openflow/messages/cofmsg_queue_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_config_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_table_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_desc_stats.h"

namespace rofl {

class eSegmentedMessageBase			: public RoflException {};
class eSegmentedMessageInval		: public eSegmentedMessageBase {};
class eSegmentedMessageNotFound		: public eSegmentedMessageBase {};

class csegmsg {

	cclock								expires_at;	// time this cmultipart message will expire
	uint32_t 							xid;		// transaction id used by this multipart message
	rofl::openflow::cofmsg_stats*		msg;		// stitched multipart message, allocated on heap

	static time_t const DEFAULT_EXPIRATION_DELTA_SEC 	= 8;
	static time_t const DEFAULT_EXPIRATION_DELTA_NSEC 	= 0;

public:

	/**
	 *
	 */
	csegmsg();

	/**
	 *
	 */
	csegmsg(uint32_t xid);

	/**
	 *
	 */
	virtual
	~csegmsg();

	/**
	 *
	 */
	csegmsg(csegmsg const& segmsg);

	/**
	 *
	 */
	csegmsg&
	operator= (csegmsg const& segmsg);

public:

	/**
	 *
	 */
	bool
	has_expired() const;

	/**
	 *
	 */
	void
	set_expiration_in(
			time_t delta_sec = 0, time_t delta_nsec = 0);

	/**
	 *
	 */
	uint32_t
	get_xid() const { return xid; };

	/**
	 *
	 */
	rofl::openflow::cofmsg_stats const&
	get_msg() const {
		if (0 == msg)
			throw eInval();
		return *msg;
	};


public:

	/**
	 * @brief	Merges payload from msg within this->msg. Checks stats sub-type first.
	 */
	void
	store_and_merge_msg(rofl::openflow::cofmsg_stats const& msg);

	/**
	 * @brief	Returns pointer to this->msg and sets this->msg to NULL. The object resides on heap and must be destroyed by the calling entity.
	 */
	rofl::openflow::cofmsg_stats*
	retrieve_and_detach_msg();

private:

	/**
	 *
	 */
	void
	clone(rofl::openflow::cofmsg_stats const& msg);

public:

	friend std::ostream&
	operator<< (std::ostream& os, csegmsg const& msg) {
		os << rofl::indent(0) << "<csegmsg" << " >" << std::endl;
		os << rofl::indent(2) << "<expires: >" << std::endl;
		{ rofl::indent i(4); os << msg.expires_at; }
		os << rofl::indent(2) << "<xid: 0x" << std::hex << (int)msg.xid << std::dec << " >" << std::endl;
		rofl::indent i(2);
		switch (msg.msg->get_version()) {
		case rofl::openflow13::OFP_VERSION: {

			switch (msg.msg->get_type()) {
			case rofl::openflow13::OFPT_MULTIPART_REQUEST: {

				switch (msg.msg->get_stats_type()) {
				case rofl::openflow13::OFPMP_DESC: {
					os << dynamic_cast<rofl::openflow::cofmsg_desc_stats_request const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_FLOW: {
					os << dynamic_cast<rofl::openflow::cofmsg_flow_stats_request const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_AGGREGATE: {
					os << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_TABLE: {
					os << dynamic_cast<rofl::openflow::cofmsg_table_stats_request const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_PORT_STATS: {
					os << dynamic_cast<rofl::openflow::cofmsg_port_stats_request const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_QUEUE: {
					os << dynamic_cast<rofl::openflow::cofmsg_queue_stats_request const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_GROUP: {
					os << dynamic_cast<rofl::openflow::cofmsg_group_stats_request const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_GROUP_DESC: {
					os << dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_GROUP_FEATURES: {
					os << dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request const&>( *(msg.msg) );
				} break;
				// TODO: meters
				case rofl::openflow13::OFPMP_TABLE_FEATURES: {
					os << dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_PORT_DESC: {
					os << dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_request const&>( *(msg.msg) );
				} break;
				default: {
					os << *(msg.msg);
				};
				}

			} break;
			case rofl::openflow13::OFPT_MULTIPART_REPLY: {

				switch (msg.msg->get_stats_type()) {
				case rofl::openflow13::OFPMP_DESC: {
					os << dynamic_cast<rofl::openflow::cofmsg_desc_stats_reply const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_FLOW: {
					os << dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_AGGREGATE: {
					os << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_reply const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_TABLE: {
					os << dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_PORT_STATS: {
					os << dynamic_cast<rofl::openflow::cofmsg_port_stats_reply const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_QUEUE: {
					os << dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_GROUP: {
					os << dynamic_cast<rofl::openflow::cofmsg_group_stats_reply const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_GROUP_DESC: {
					os << dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_GROUP_FEATURES: {
					os << dynamic_cast<rofl::openflow::cofmsg_group_features_stats_reply const&>( *(msg.msg) );
				} break;
				// TODO: meters
				case rofl::openflow13::OFPMP_TABLE_FEATURES: {
					os << dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply const&>( *(msg.msg) );
				} break;
				case rofl::openflow13::OFPMP_PORT_DESC: {
					os << dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply const&>( *(msg.msg) );
				} break;
				default: {
					os << *(msg.msg);
				};
				}

			} break;
			default: {
				// oops, do nothing here
			};
			}

		} break;
		}
		return os;
	};
};

}; // end of namespace rofl

#endif /* CMULTIPART_H_ */
