/*
 * csegmsg.cc
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#include "rofl/common/csegmsg.h"

using namespace rofl::openflow;

csegmsg::csegmsg() :
	xid(0),
	msg(NULL)
{

}



csegmsg::csegmsg(uint32_t xid) :
	xid(xid),
	msg(NULL)
{

}



csegmsg::~csegmsg()
{
	if (NULL != msg) {
		delete msg;
	}
}



csegmsg::csegmsg(csegmsg const& segmsg)
{
	*this = segmsg;
}



csegmsg&
csegmsg::operator= (csegmsg const& segmsg)
{
	if (this == &segmsg)
		return *this;

	expires		= segmsg.expires;
	xid			= segmsg.xid;

	if (msg) {
		delete msg;
	}

	if (segmsg.msg != NULL) {

		switch (segmsg.msg->get_type()) {
		case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
			switch (segmsg.msg->get_stats_type()) {
			case rofl::openflow13::OFPMP_DESC: {
				msg = new cofmsg_desc_stats_request(*(dynamic_cast<cofmsg_desc_stats_request const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_FLOW: {
				msg = new cofmsg_flow_stats_request(*(dynamic_cast<cofmsg_flow_stats_request const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_AGGREGATE: {
				msg = new cofmsg_aggr_stats_request(*(dynamic_cast<cofmsg_aggr_stats_request const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_TABLE: {
				msg = new cofmsg_table_stats_request(*(dynamic_cast<cofmsg_table_stats_request const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_PORT_STATS: {
				msg = new cofmsg_port_stats_request(*(dynamic_cast<cofmsg_port_stats_request const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_QUEUE: {
				msg = new cofmsg_queue_stats_request(*(dynamic_cast<cofmsg_queue_stats_request const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_GROUP: {
				msg = new cofmsg_group_stats_request(*(dynamic_cast<cofmsg_group_stats_request const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_GROUP_DESC: {
				msg = new cofmsg_group_desc_stats_request(*(dynamic_cast<cofmsg_group_desc_stats_request const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_GROUP_FEATURES: {
				msg = new cofmsg_group_features_stats_request(*(dynamic_cast<cofmsg_group_features_stats_request const*>(segmsg.msg)));
			} break;
			// TODO: meters
			case rofl::openflow13::OFPMP_TABLE_FEATURES: {
				msg = new cofmsg_table_features_stats_request(*(dynamic_cast<cofmsg_table_features_stats_request const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_PORT_DESC: {
				msg = new cofmsg_port_desc_stats_request(*(dynamic_cast<cofmsg_port_desc_stats_request const*>(segmsg.msg)));
			} break;
			default: {
				msg = new cofmsg_stats_request(*(dynamic_cast<cofmsg_stats_request const*>(segmsg.msg)));
			};
			}

		} break;
		case rofl::openflow13::OFPT_MULTIPART_REPLY: {
			switch (segmsg.msg->get_stats_type()) {
			case rofl::openflow13::OFPMP_DESC: {
				msg = new cofmsg_desc_stats_reply(*(dynamic_cast<cofmsg_desc_stats_reply const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_FLOW: {
				msg = new cofmsg_flow_stats_reply(*(dynamic_cast<cofmsg_flow_stats_reply const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_AGGREGATE: {
				msg = new cofmsg_aggr_stats_reply(*(dynamic_cast<cofmsg_aggr_stats_reply const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_TABLE: {
				msg = new cofmsg_table_stats_reply(*(dynamic_cast<cofmsg_table_stats_reply const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_PORT_STATS: {
				msg = new cofmsg_port_stats_reply(*(dynamic_cast<cofmsg_port_stats_reply const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_QUEUE: {
				msg = new cofmsg_queue_stats_reply(*(dynamic_cast<cofmsg_queue_stats_reply const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_GROUP: {
				msg = new cofmsg_group_stats_reply(*(dynamic_cast<cofmsg_group_stats_reply const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_GROUP_DESC: {
				msg = new cofmsg_group_desc_stats_reply(*(dynamic_cast<cofmsg_group_desc_stats_reply const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_GROUP_FEATURES: {
				msg = new cofmsg_group_features_stats_reply(*(dynamic_cast<cofmsg_group_features_stats_reply const*>(segmsg.msg)));
			} break;
			// TODO: meters
			case rofl::openflow13::OFPMP_TABLE_FEATURES: {
				msg = new cofmsg_table_features_stats_reply(*(dynamic_cast<cofmsg_table_features_stats_reply const*>(segmsg.msg)));
			} break;
			case rofl::openflow13::OFPMP_PORT_DESC: {
				msg = new cofmsg_port_desc_stats_reply(*(dynamic_cast<cofmsg_port_desc_stats_reply const*>(segmsg.msg)));
			} break;
			default: {
				msg = new cofmsg_stats_reply(*(dynamic_cast<cofmsg_stats_reply const*>(segmsg.msg)));
			};
			}

		} break;
		default: {
			// oops
		};
		}

	} else {
		msg 	= NULL;
	}

	return *this;
}



void
csegmsg::store_and_merge_msg(cofmsg_stats const& msg)
{

}



rofl::cofmsg_stats*
csegmsg::retrieve_and_detach_msg()
{
	cofmsg_stats *tmp = msg; msg = NULL; return tmp;
}


