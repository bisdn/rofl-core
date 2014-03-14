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

	if (NULL != segmsg.msg) {
		csegmsg::clone(*(segmsg.msg));
	} else {
		msg 	= NULL;
	}

	return *this;
}



void
csegmsg::clone(cofmsg_stats const& msg_stats)
{
	if (NULL != msg) { delete msg; msg = NULL; }

	switch (msg_stats.get_type()) {
	case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
		switch (msg_stats.get_stats_type()) {
		case rofl::openflow13::OFPMP_DESC: {
			msg = new cofmsg_desc_stats_request(dynamic_cast<cofmsg_desc_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			msg = new cofmsg_flow_stats_request(dynamic_cast<cofmsg_flow_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			msg = new cofmsg_aggr_stats_request(dynamic_cast<cofmsg_aggr_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			msg = new cofmsg_table_stats_request(dynamic_cast<cofmsg_table_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			msg = new cofmsg_port_stats_request(dynamic_cast<cofmsg_port_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			msg = new cofmsg_queue_stats_request(dynamic_cast<cofmsg_queue_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			msg = new cofmsg_group_stats_request(dynamic_cast<cofmsg_group_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			msg = new cofmsg_group_desc_stats_request(dynamic_cast<cofmsg_group_desc_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			msg = new cofmsg_group_features_stats_request(dynamic_cast<cofmsg_group_features_stats_request const&>(msg_stats));
		} break;
		// TODO: meters
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			msg = new cofmsg_table_features_stats_request(dynamic_cast<cofmsg_table_features_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			msg = new cofmsg_port_desc_stats_request(dynamic_cast<cofmsg_port_desc_stats_request const&>(msg_stats));
		} break;
		default: {
			msg = new cofmsg_stats_request(dynamic_cast<cofmsg_stats_request const&>(msg_stats));
		};
		}

	} break;
	case rofl::openflow13::OFPT_MULTIPART_REPLY: {
		switch (msg_stats.get_stats_type()) {
		case rofl::openflow13::OFPMP_DESC: {
			msg = new cofmsg_desc_stats_reply(dynamic_cast<cofmsg_desc_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			msg = new cofmsg_flow_stats_reply(dynamic_cast<cofmsg_flow_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			msg = new cofmsg_aggr_stats_reply(dynamic_cast<cofmsg_aggr_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			msg = new cofmsg_table_stats_reply(dynamic_cast<cofmsg_table_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			msg = new cofmsg_port_stats_reply(dynamic_cast<cofmsg_port_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			msg = new cofmsg_queue_stats_reply(dynamic_cast<cofmsg_queue_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			msg = new cofmsg_group_stats_reply(dynamic_cast<cofmsg_group_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			msg = new cofmsg_group_desc_stats_reply(dynamic_cast<cofmsg_group_desc_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			msg = new cofmsg_group_features_stats_reply(dynamic_cast<cofmsg_group_features_stats_reply const&>(msg_stats));
		} break;
		// TODO: meters
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			msg = new cofmsg_table_features_stats_reply(dynamic_cast<cofmsg_table_features_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			msg = new cofmsg_port_desc_stats_reply(dynamic_cast<cofmsg_port_desc_stats_reply const&>(msg_stats));
		} break;
		default: {
			msg = new cofmsg_stats_reply(dynamic_cast<cofmsg_stats_reply const&>(msg_stats));
		};
		}

	} break;
	default: {
		// oops
	};
	}
}



void
csegmsg::store_and_merge_msg(cofmsg_stats const& msg_stats)
{
	if (NULL == msg) {

		csegmsg::clone(msg_stats);

	} else {

		if (msg->get_type() != msg_stats.get_type()) { // message types must match
			throw eSegmentedMessageInval();
		}

		if (msg->get_stats_type() != msg_stats.get_stats_type()) { // stats types must match
			throw eSegmentedMessageInval();
		}

		switch (msg_stats.get_type()) {
		case rofl::openflow13::OFPT_MULTIPART_REQUEST: {

			switch (msg_stats.get_stats_type()) {
			case rofl::openflow13::OFPMP_TABLE_FEATURES: {

				cofmsg_table_features_stats_request* msg_table =
						dynamic_cast<cofmsg_table_features_stats_request*>(msg);

				cofmsg_table_features_stats_request const& msg_table_stats =
						dynamic_cast<cofmsg_table_features_stats_request const&>(msg_stats);

				msg_table->set_tables() += msg_table_stats.get_tables();

			} break;
			default: {
				// cannot be fragmented, as request is too short
			};
			}

		} break;
		case rofl::openflow13::OFPT_MULTIPART_REPLY: {

			switch (msg_stats.get_stats_type()) {
			case rofl::openflow13::OFPMP_GROUP: {

				cofmsg_group_stats_reply* msg_group =
						dynamic_cast<cofmsg_group_stats_reply*>(msg);

				cofmsg_group_stats_reply const& msg_group_stats =
						dynamic_cast<cofmsg_group_stats_reply const&>(msg_stats);

				msg_group->set_groups() += msg_group_stats.get_groups();

			} break;
			case rofl::openflow13::OFPMP_GROUP_DESC: {

				cofmsg_group_desc_stats_reply* msg_group =
						dynamic_cast<cofmsg_group_desc_stats_reply*>(msg);

				cofmsg_group_desc_stats_reply const& msg_group_stats =
						dynamic_cast<cofmsg_group_desc_stats_reply const&>(msg_stats);

				msg_group->set_group_descs() += msg_group_stats.get_group_descs();

			} break;
			case rofl::openflow13::OFPMP_TABLE_FEATURES: {

				cofmsg_table_features_stats_reply* msg_table =
						dynamic_cast<cofmsg_table_features_stats_reply*>(msg);

				cofmsg_table_features_stats_reply const& msg_table_stats =
						dynamic_cast<cofmsg_table_features_stats_reply const&>(msg_stats);

				msg_table->set_tables() += msg_table_stats.get_tables();

			} break;
			case rofl::openflow13::OFPMP_PORT_DESC: {

				cofmsg_port_desc_stats_reply* msg_port_desc =
						dynamic_cast<cofmsg_port_desc_stats_reply*>(msg);

				cofmsg_port_desc_stats_reply const& msg_port_desc_stats =
						dynamic_cast<cofmsg_port_desc_stats_reply const&>(msg_stats);

				msg_port_desc->set_ports() += msg_port_desc_stats.get_ports();

			} break;
			default: {
				// cannot be fragmented, as request is too short
			};
			}

		} break;
		default: {

		};
		}

	} // end else block
}



rofl::cofmsg_stats*
csegmsg::retrieve_and_detach_msg()
{
	cofmsg_stats *tmp = msg; msg = NULL; return tmp;
}


