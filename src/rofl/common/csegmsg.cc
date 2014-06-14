/*
 * csegmsg.cc
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#include "rofl/common/csegmsg.h"

using namespace rofl;

csegmsg::csegmsg() :
	xid(0),
	msg(NULL)
{
	set_expiration_in(DEFAULT_EXPIRATION_DELTA_SEC, DEFAULT_EXPIRATION_DELTA_NSEC);
}



csegmsg::csegmsg(uint32_t xid) :
	xid(xid),
	msg(NULL)
{
	set_expiration_in(DEFAULT_EXPIRATION_DELTA_SEC, DEFAULT_EXPIRATION_DELTA_NSEC);
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

	expires_at	= segmsg.expires_at;
	xid			= segmsg.xid;

	if (NULL != segmsg.msg) {
		csegmsg::clone(*(segmsg.msg));
	} else {
		msg 	= NULL;
	}

	return *this;
}



bool
csegmsg::has_expired() const
{
	return (cclock::now() > expires_at);
}



void
csegmsg::set_expiration_in(time_t delta_sec, time_t delta_nsec)
{
	expires_at = cclock::now() + cclock(delta_sec, delta_nsec);
}



void
csegmsg::clone(rofl::openflow::cofmsg_stats const& msg_stats)
{
	if (NULL != msg) { delete msg; msg = NULL; }

	switch (msg_stats.get_type()) {
	case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
		switch (msg_stats.get_stats_type()) {
		case rofl::openflow13::OFPMP_DESC: {
			msg = new rofl::openflow::cofmsg_desc_stats_request(dynamic_cast<rofl::openflow::cofmsg_desc_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			msg = new rofl::openflow::cofmsg_flow_stats_request(dynamic_cast<rofl::openflow::cofmsg_flow_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			msg = new rofl::openflow::cofmsg_aggr_stats_request(dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			msg = new rofl::openflow::cofmsg_table_stats_request(dynamic_cast<rofl::openflow::cofmsg_table_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			msg = new rofl::openflow::cofmsg_port_stats_request(dynamic_cast<rofl::openflow::cofmsg_port_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			msg = new rofl::openflow::cofmsg_queue_stats_request(dynamic_cast<rofl::openflow::cofmsg_queue_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			msg = new rofl::openflow::cofmsg_group_stats_request(dynamic_cast<rofl::openflow::cofmsg_group_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			msg = new rofl::openflow::cofmsg_group_desc_stats_request(dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			msg = new rofl::openflow::cofmsg_group_features_stats_request(dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_METER: {
			msg = new rofl::openflow::cofmsg_meter_stats_request(dynamic_cast<rofl::openflow::cofmsg_meter_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_METER_CONFIG: {
			msg = new rofl::openflow::cofmsg_meter_config_stats_request(dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_METER_FEATURES: {
			msg = new rofl::openflow::cofmsg_meter_features_stats_request(dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			msg = new rofl::openflow::cofmsg_table_features_stats_request(dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			msg = new rofl::openflow::cofmsg_port_desc_stats_request(dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_request const&>(msg_stats));
		} break;
		default: {
			msg = new rofl::openflow::cofmsg_stats_request(dynamic_cast<rofl::openflow::cofmsg_stats_request const&>(msg_stats));
		};
		}

		if (NULL != msg) {
			msg->set_stats_flags(msg->get_stats_flags() & ~rofl::openflow13::OFPMPF_REQ_MORE);
		}

	} break;
	case rofl::openflow13::OFPT_MULTIPART_REPLY: {
		switch (msg_stats.get_stats_type()) {
		case rofl::openflow13::OFPMP_DESC: {
			msg = new rofl::openflow::cofmsg_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_desc_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			msg = new rofl::openflow::cofmsg_flow_stats_reply(dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			msg = new rofl::openflow::cofmsg_aggr_stats_reply(dynamic_cast<rofl::openflow::cofmsg_aggr_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			msg = new rofl::openflow::cofmsg_table_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			msg = new rofl::openflow::cofmsg_port_stats_reply(dynamic_cast<rofl::openflow::cofmsg_port_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			msg = new rofl::openflow::cofmsg_queue_stats_reply(dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			msg = new rofl::openflow::cofmsg_group_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			msg = new rofl::openflow::cofmsg_group_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			msg = new rofl::openflow::cofmsg_group_features_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_features_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_METER: {
			msg = new rofl::openflow::cofmsg_meter_stats_reply(dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_METER_CONFIG: {
			msg = new rofl::openflow::cofmsg_meter_config_stats_reply(dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_METER_FEATURES: {
			msg = new rofl::openflow::cofmsg_meter_features_stats_reply(dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			msg = new rofl::openflow::cofmsg_table_features_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply const&>(msg_stats));
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			msg = new rofl::openflow::cofmsg_port_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply const&>(msg_stats));
		} break;
		default: {
			msg = new rofl::openflow::cofmsg_stats_reply(dynamic_cast<rofl::openflow::cofmsg_stats_reply const&>(msg_stats));
		};
		}

		if (NULL != msg) {
			msg->set_stats_flags(msg->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
		}

	} break;
	default: {
		// oops
	};
	}

}



void
csegmsg::store_and_merge_msg(rofl::openflow::cofmsg_stats const& msg_stats)
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

				rofl::openflow::cofmsg_table_features_stats_request* msg_table =
						dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request*>(msg);

				rofl::openflow::cofmsg_table_features_stats_request const& msg_table_stats =
						dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request const&>(msg_stats);

				msg_table->set_tables() += msg_table_stats.get_tables();

			} break;
			default: {
				// cannot be fragmented, as request is too short
			};
			}

		} break;
		case rofl::openflow13::OFPT_MULTIPART_REPLY: {

			switch (msg_stats.get_stats_type()) {
			case rofl::openflow13::OFPMP_FLOW: {

				rofl::openflow::cofmsg_flow_stats_reply* msg_flow =
						dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply*>(msg);

				rofl::openflow::cofmsg_flow_stats_reply const& msg_flow_stats =
						dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply const&>(msg_stats);

				msg_flow->set_flow_stats_array() += msg_flow_stats.get_flow_stats_array();

			} break;
			case rofl::openflow13::OFPMP_TABLE: {

				rofl::openflow::cofmsg_table_stats_reply* msg_table =
						dynamic_cast<rofl::openflow::cofmsg_table_stats_reply*>(msg);

				rofl::openflow::cofmsg_table_stats_reply const& msg_table_stats =
						dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const&>(msg_stats);

				msg_table->set_table_stats_array() += msg_table_stats.get_table_stats_array();

			} break;
			case rofl::openflow13::OFPMP_PORT_STATS: {

				rofl::openflow::cofmsg_port_stats_reply* msg_port =
						dynamic_cast<rofl::openflow::cofmsg_port_stats_reply*>(msg);

				rofl::openflow::cofmsg_port_stats_reply const& msg_port_stats =
						dynamic_cast<rofl::openflow::cofmsg_port_stats_reply const&>(msg_stats);

				msg_port->set_port_stats_array() += msg_port_stats.get_port_stats_array();

			} break;
			case rofl::openflow13::OFPMP_QUEUE: {

				rofl::openflow::cofmsg_queue_stats_reply* msg_queue =
						dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply*>(msg);

				rofl::openflow::cofmsg_queue_stats_reply const& msg_queue_stats =
						dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply const&>(msg_stats);

				msg_queue->set_queue_stats_array() += msg_queue_stats.get_queue_stats_array();

			} break;
			case rofl::openflow13::OFPMP_GROUP: {

				rofl::openflow::cofmsg_group_stats_reply* msg_group =
						dynamic_cast<rofl::openflow::cofmsg_group_stats_reply*>(msg);

				rofl::openflow::cofmsg_group_stats_reply const& msg_group_stats =
						dynamic_cast<rofl::openflow::cofmsg_group_stats_reply const&>(msg_stats);

				msg_group->set_group_stats_array() += msg_group_stats.get_group_stats_array();

			} break;
			case rofl::openflow13::OFPMP_GROUP_DESC: {

				rofl::openflow::cofmsg_group_desc_stats_reply* msg_group =
						dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply*>(msg);

				rofl::openflow::cofmsg_group_desc_stats_reply const& msg_group_stats =
						dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply const&>(msg_stats);

				msg_group->set_group_desc_stats_array() += msg_group_stats.get_group_desc_stats_array();

			} break;
			case rofl::openflow13::OFPMP_METER: {

				rofl::openflow::cofmsg_meter_stats_reply* msg_meter =
						dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply*>(msg);

				rofl::openflow::cofmsg_meter_stats_reply const& msg_meter_stats =
						dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply const&>(msg_stats);

				msg_meter->set_meter_stats_array() += msg_meter_stats.get_meter_stats_array();

			} break;
			case rofl::openflow13::OFPMP_METER_CONFIG: {

				rofl::openflow::cofmsg_meter_config_stats_reply* msg_meter =
						dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply*>(msg);

				rofl::openflow::cofmsg_meter_config_stats_reply const& msg_meter_stats =
						dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply const&>(msg_stats);

				msg_meter->set_meter_config_array() += msg_meter_stats.get_meter_config_array();

			} break;
			case rofl::openflow13::OFPMP_TABLE_FEATURES: {

				rofl::openflow::cofmsg_table_features_stats_reply* msg_table =
						dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply*>(msg);

				rofl::openflow::cofmsg_table_features_stats_reply const& msg_table_stats =
						dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply const&>(msg_stats);

				msg_table->set_tables() += msg_table_stats.get_tables();

			} break;
			case rofl::openflow13::OFPMP_PORT_DESC: {

				rofl::openflow::cofmsg_port_desc_stats_reply* msg_port_desc =
						dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply*>(msg);

				rofl::openflow::cofmsg_port_desc_stats_reply const& msg_port_desc_stats =
						dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply const&>(msg_stats);

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



rofl::openflow::cofmsg_stats*
csegmsg::retrieve_and_detach_msg()
{
	rofl::openflow::cofmsg_stats *tmp = msg; msg = NULL; return tmp;
}


