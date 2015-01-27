/*
 * crofdpt.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "crofdpt.h"
#include "crofbase.h"

using namespace rofl;

/*static*/std::set<crofdpt_env*> crofdpt_env::rofdpt_envs;
/*static*/std::map<cdptid, crofdpt*> crofdpt::rofdpts;

/*static*/crofdpt&
crofdpt::get_dpt(
		const cdptid& dptid)
{
	if (crofdpt::rofdpts.find(dptid) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound("rofl::crofdpt::get_dpt() dptid not found");
	}
	return *(crofdpt::rofdpts[dptid]);
}



/*static*/crofdpt&
crofdpt::get_dpt(
		const cdpid& dpid)
{
	std::map<cdptid, crofdpt*>::iterator it;
	if ((it = find_if(crofdpt::rofdpts.begin(), crofdpt::rofdpts.end(),
			crofdpt::crofdpt_find_by_dpid(dpid.get_uint64_t()))) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound("rofl::crofdpt::get_dpt() dpid not found");
	}
	return *(it->second);
}



void
crofdpt::handle_timeout(int opaque, void *data)
{
	switch (opaque) {
	case TIMER_RUN_ENGINE: {
		work_on_eventqueue();
	} break;
	default: {
		LOGGING_ERROR << "[rofl-common][crofdpt] dpid:"
				<< std::hex << get_dpid().str() << std::dec
				<< " unknown timer event:" << opaque << std::endl;
	};
	}
}



void
crofdpt::work_on_eventqueue()
{
	flags.set(FLAG_ENGINE_IS_RUNNING);
	while (not events.empty()) {
		enum crofdpt_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_CONNECTED: {
			event_connected();
		} break;
		case EVENT_DISCONNECTED: {
			flags.reset(FLAG_ENGINE_IS_RUNNING);
			event_disconnected();
		} return;
		case EVENT_CONN_TERMINATED: {
			event_conn_terminated();
		} break;
		case EVENT_CONN_REFUSED: {
			event_conn_refused();
		} break;
		case EVENT_CONN_FAILED: {
			event_conn_failed();
		} break;
		case EVENT_FEATURES_REPLY_RCVD: {
			event_features_reply_rcvd();
		} break;
		case EVENT_FEATURES_REQUEST_EXPIRED: {
			event_features_request_expired();
		} break;
		case EVENT_GET_CONFIG_REPLY_RCVD: {
			event_get_config_reply_rcvd();
		} break;
		case EVENT_GET_CONFIG_REQUEST_EXPIRED: {
			event_get_config_request_expired();
		} break;
		case EVENT_TABLE_STATS_REPLY_RCVD: {
			event_table_stats_reply_rcvd();
		} break;
		case EVENT_TABLE_STATS_REQUEST_EXPIRED: {
			event_table_stats_request_expired();
		} break;
		case EVENT_TABLE_FEATURES_STATS_REPLY_RCVD: {
			event_table_features_stats_reply_rcvd();
		} break;
		case EVENT_TABLE_FEATURES_STATS_REQUEST_EXPIRED: {
			event_table_features_stats_request_expired();
		} break;
		case EVENT_PORT_DESC_STATS_REPLY_RCVD: {
			event_port_desc_reply_rcvd();
		} break;
		case EVENT_PORT_DESC_STATS_REQUEST_EXPIRED: {
			event_port_desc_request_expired();
		} break;
		default: {
			LOGGING_ERROR << "[rofl-common][crofdpt] unknown event seen, internal error" << std::endl << *this;
		};
		}
	}
	flags.reset(FLAG_ENGINE_IS_RUNNING);
}



void
crofdpt::event_connected()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] entering state -ofp-connected-" << std::endl;
	switch (state) {
	case STATE_INIT:
	case STATE_DISCONNECTED: {
		state = STATE_WAIT_FOR_FEATURES;
		send_features_request(cauxid(0));
		ports.set_version(rofchan.get_version());
		tables.set_version(rofchan.get_version());
#if 0
		/*
		 * skip sending Features request, Get-Config request, Table-Features-Stats request
		 * and Port-Desc-Stats request. This is up to the derived controller logic.
		 */
		state = STATE_ESTABLISHED;
		call_env().handle_chan_established(*this);
#endif
	} break;
	default: {
		LOGGING_ERROR << "[rofl-common][crofdpt] event -CONNECTED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdpt::event_disconnected()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] entering state -ofp-disconnected-" << std::endl;
	events.clear();
	rofchan.close();
	transactions.clear();
	tables.clear();
	ports.clear();
	state = STATE_DISCONNECTED;
	dlqueue.clear();
	call_env().handle_chan_terminated(*this);
}



void
crofdpt::event_conn_terminated()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -conn-terminated-" << std::endl;
	rofl::RwLock rwlock(conns_terminated_rwlock, rofl::RwLock::RWLOCK_WRITE);
	for (std::list<rofl::cauxid>::iterator
			it = conns_terminated.begin(); it != conns_terminated.end(); ++it) {
		call_env().handle_conn_terminated(*this, *it);
	}
	conns_terminated.clear();
}



void
crofdpt::event_conn_refused()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -conn-refused-" << std::endl;
	rofl::RwLock rwlock(conns_refused_rwlock, rofl::RwLock::RWLOCK_WRITE);
	for (std::list<rofl::cauxid>::iterator
			it = conns_refused.begin(); it != conns_refused.end(); ++it) {
		call_env().handle_conn_refused(*this, *it);
	}
	conns_refused.clear();
}



void
crofdpt::event_conn_failed()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -conn-failed-" << std::endl;
	rofl::RwLock rwlock(conns_failed_rwlock, rofl::RwLock::RWLOCK_WRITE);
	for (std::list<rofl::cauxid>::iterator
			it = conns_failed.begin(); it != conns_failed.end(); ++it) {
		call_env().handle_conn_failed(*this, *it);
	}
	conns_failed.clear();
}



void
crofdpt::event_features_reply_rcvd()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -features-reply-rcvd-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_FEATURES: {
		LOGGING_DEBUG << "[rofl-common][crofdpt] entering state -wait-for-get-config-" << std::endl;
		state = STATE_WAIT_FOR_GET_CONFIG;
		send_get_config_request(rofl::cauxid(0));
	} break;
	case STATE_ESTABLISHED: {
		// do nothing: Feature.requests may be sent by a derived class during state ESTABLISHED
	} break;
	default: {
	};
	}
}



void
crofdpt::event_features_request_expired(
		uint32_t xid)
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -features-request-expired-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_FEATURES: {
		//state = STATE_DISCONNECTED;
		push_on_eventqueue(EVENT_DISCONNECTED);
	} break;
	case STATE_ESTABLISHED: {
		call_env().handle_features_reply_timeout(*this, xid);
	} break;
	default: {
	};
	}
}



void
crofdpt::event_get_config_reply_rcvd()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -get-config-reply-rcvd-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_GET_CONFIG: {
		switch (rofchan.get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			LOGGING_DEBUG << "[rofl-common][crofdpt] entering state -established-" << std::endl;
			state = STATE_ESTABLISHED;
			call_env().handle_chan_established(*this);
			// send all postponed messages to higher layers
			while (not dlqueue.empty()) {
				recv_message(rofchan, rofl::cauxid(0), dlqueue.retrieve());
			}

		} break;
		case rofl::openflow12::OFP_VERSION: {
			LOGGING_DEBUG << "[rofl-common][crofdpt] entering state -wait-for-table-stats-" << std::endl;
			state = STATE_WAIT_FOR_TABLE_STATS;
			send_table_stats_request(rofl::cauxid(0));
		} break;
		case rofl::openflow13::OFP_VERSION:
		default: {
			LOGGING_DEBUG << "[rofl-common][crofdpt] entering state -wait-for-table-features-stats-" << std::endl;
			state = STATE_WAIT_FOR_TABLE_FEATURES_STATS;
			send_table_features_stats_request(rofl::cauxid(0), 0);
		} break;
		}
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
	} break;
	default: {
	};
	}
}



void
crofdpt::event_get_config_request_expired(
		uint32_t xid)
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -get-config-request-expired-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_GET_CONFIG: {
		transactions.clear();
		push_on_eventqueue(EVENT_DISCONNECTED);
	} break;
	case STATE_ESTABLISHED: {
		call_env().handle_get_config_reply_timeout(*this, xid);
	} break;
	default: {
	};
	}
}



void
crofdpt::event_table_stats_reply_rcvd()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -get-table-stats-reply-rcvd-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_TABLE_STATS: {
		switch (rofchan.get_version()) {
		case rofl::openflow12::OFP_VERSION: {
			LOGGING_DEBUG << "[rofl-common][crofdpt] entering state -established-" << std::endl;
			state = STATE_ESTABLISHED;
			call_env().handle_chan_established(*this);
			// send all postponed messages to higher layers
			while (not dlqueue.empty()) {
				recv_message(rofchan, rofl::cauxid(0), dlqueue.retrieve());
			}

		} break;
		default: {
			// do nothing
		};
		}
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
	} break;
	default: {
	};
	}
}



void
crofdpt::event_table_stats_request_expired(
		uint32_t xid)
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -table-stats-request-expired-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_TABLE_STATS: {
		transactions.clear();
		//state = STATE_DISCONNECTED;
		push_on_eventqueue(EVENT_DISCONNECTED);
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
		call_env().handle_table_stats_reply_timeout(*this, xid);
	} break;
	default: {
	};
	}
}


void
crofdpt::event_table_features_stats_reply_rcvd()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -get-table-features-stats-reply-rcvd-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_TABLE_FEATURES_STATS: {
		switch (rofchan.get_version()) {
		case rofl::openflow13::OFP_VERSION: {
			LOGGING_DEBUG << "[rofl-common][crofdpt] entering state -wait-for-port-desc-stats-" << std::endl;
			state = STATE_WAIT_FOR_PORT_DESC_STATS;
			send_port_desc_stats_request(rofl::cauxid(0), 0);
		} break;
		default: {
			// do nothing
		};
		}
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
	} break;
	default: {
	};
	}
}



void
crofdpt::event_table_features_stats_request_expired(
		uint32_t xid)
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -table-features-stats-request-expired-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_TABLE_FEATURES_STATS: {
		transactions.clear();
		push_on_eventqueue(EVENT_DISCONNECTED);
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
		call_env().handle_table_features_stats_reply_timeout(*this, xid);
	} break;
	default: {
	};
	}
}



void
crofdpt::event_port_desc_reply_rcvd()
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -get-port-desc-stats-reply-rcvd-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_PORT_DESC_STATS: {
		switch (rofchan.get_version()) {
		case rofl::openflow13::OFP_VERSION:
		default: {
			LOGGING_DEBUG << "[rofl-common][crofdpt] entering state -established-" << std::endl;
			state = STATE_ESTABLISHED;
			call_env().handle_chan_established(*this);
			// send all postponed messages to higher layers
			while (not dlqueue.empty()) {
				recv_message(rofchan, rofl::cauxid(0), dlqueue.retrieve());
			}

		} break;
		}
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
	} break;
	default: {
	};
	}
}



void
crofdpt::event_port_desc_request_expired(
		uint32_t xid)
{
	LOGGING_DEBUG << "[rofl-common][crofdpt] rcvd event -port-desc-request-expired-" << std::endl;
	switch (state) {
	case STATE_WAIT_FOR_PORT_DESC_STATS: {
		transactions.clear();
		push_on_eventqueue(EVENT_DISCONNECTED);
	} break;
	case STATE_ESTABLISHED: {
		call_env().handle_port_desc_stats_reply_timeout(*this, xid);
	} break;
	default: {
	};
	}
}



void
crofdpt::recv_message(crofchan& chan, const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg)
{
	try {
		switch (msg->get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_VENDOR: {
				experimenter_rcvd(auxid, msg);
			} break;
			case rofl::openflow10::OFPT_ERROR: {
				error_rcvd(auxid, msg);
			} break;
			case rofl::openflow10::OFPT_FEATURES_REPLY: {
				features_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow10::OFPT_GET_CONFIG_REPLY: {
				get_config_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow10::OFPT_PACKET_IN: {
				packet_in_rcvd(auxid, msg);
			} break;
			case rofl::openflow10::OFPT_FLOW_REMOVED: {
				flow_removed_rcvd(auxid, msg);
			} break;
			case rofl::openflow10::OFPT_PORT_STATUS: {
				port_status_rcvd(auxid, msg);
			} break;
			case rofl::openflow10::OFPT_STATS_REPLY: {
				multipart_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow10::OFPT_BARRIER_REPLY: {
				barrier_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY: {
				queue_get_config_reply_rcvd(auxid, msg);
			} break;
			default: {
			};
			}

		} break;
		case rofl::openflow12::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_EXPERIMENTER: {
				experimenter_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_ERROR: {
				error_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_FEATURES_REPLY: {
				features_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_GET_CONFIG_REPLY: {
				get_config_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_PACKET_IN: {
				packet_in_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_FLOW_REMOVED: {
				flow_removed_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_PORT_STATUS: {
				port_status_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_STATS_REPLY: {
				multipart_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_BARRIER_REPLY: {
				barrier_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY: {
				queue_get_config_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow12::OFPT_ROLE_REPLY: {
				role_reply_rcvd(auxid, msg);
			} break;
			default: {
			};
			}
		} break;
		case rofl::openflow13::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_EXPERIMENTER: {
				experimenter_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_ERROR: {
				error_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_FEATURES_REPLY: {
				features_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_GET_CONFIG_REPLY: {
				get_config_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_PACKET_IN: {
				packet_in_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_FLOW_REMOVED: {
				flow_removed_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_PORT_STATUS: {
				port_status_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_MULTIPART_REPLY: {
				multipart_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_BARRIER_REPLY: {
				barrier_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REPLY: {
				queue_get_config_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_ROLE_REPLY: {
				role_reply_rcvd(auxid, msg);
			} break;
			case rofl::openflow13::OFPT_GET_ASYNC_REPLY: {
				get_async_config_reply_rcvd(auxid, msg);
			} break;
			default: {
			};
			}
		} break;
		default: {

		};
		}

	} catch (eOpenFlowBase& e) {

		delete msg;

	} catch (RoflException& e) {

		delete msg;
	}
}




void
crofdpt::ta_expired(
		ctransactions& tas,
		ctransaction& ta)
{
	LOGGING_WARN << "[rofl-common][crofdpt] transaction expired, xid:" << std::endl << ta;

	switch (ta.get_msg_type()) {
	case rofl::openflow::OFPT_FEATURES_REQUEST: {
		event_features_request_expired(ta.get_xid());
	} break;
	case rofl::openflow::OFPT_GET_CONFIG_REQUEST: {
		event_get_config_request_expired(ta.get_xid());
	} break;
	case rofl::openflow::OFPT_MULTIPART_REQUEST: {
		switch (ta.get_msg_sub_type()) {
		case rofl::openflow::OFPMP_DESC: {
			call_env().handle_desc_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_FLOW: {
			call_env().handle_flow_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_AGGREGATE: {
			call_env().handle_aggregate_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_TABLE: {
			event_table_stats_request_expired(ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_PORT_STATS: {
			call_env().handle_port_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_QUEUE: {
			call_env().handle_queue_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_GROUP: {
			call_env().handle_group_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_GROUP_DESC: {
			call_env().handle_group_desc_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_GROUP_FEATURES: {
			call_env().handle_group_features_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_METER: {
			call_env().handle_meter_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_METER_CONFIG: {
			call_env().handle_meter_config_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_METER_FEATURES: {
			call_env().handle_meter_features_stats_reply_timeout(*this, ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_TABLE_FEATURES: {
			event_table_features_stats_request_expired(ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_PORT_DESC: {
			event_port_desc_request_expired(ta.get_xid());
		} break;
		case rofl::openflow::OFPMP_EXPERIMENTER: {
			call_env().handle_experimenter_stats_reply_timeout(*this, ta.get_xid());
		} break;
		default: {
			call_env().handle_stats_reply_timeout(*this, ta.get_xid(), ta.get_msg_sub_type());
		};
		}

	} break;
	case rofl::openflow::OFPT_BARRIER_REQUEST: {
		call_env().handle_barrier_reply_timeout(*this, ta.get_xid());
	} break;
	case rofl::openflow::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		call_env().handle_queue_get_config_reply_timeout(*this, ta.get_xid());
	} break;
	case rofl::openflow::OFPT_ROLE_REQUEST: {
		call_env().handle_role_reply_timeout(*this, ta.get_xid());
	} break;
	case rofl::openflow::OFPT_GET_ASYNC_REQUEST: {
		call_env().handle_get_async_config_reply_timeout(*this, ta.get_xid());
	} break;
	case rofl::openflow::OFPT_EXPERIMENTER: {
		call_env().handle_experimenter_timeout(*this, ta.get_xid());
	} break;
	default: {

	};
	}
}



void
crofdpt::flow_mod_reset()
{
	rofl::openflow::cofflowmod fe(rofchan.get_version());
	switch (rofchan.get_version()) {
	case openflow10::OFP_VERSION: {
		fe.set_command(rofl::openflow10::OFPFC_DELETE);
	} break;
	case openflow12::OFP_VERSION: {
		fe.set_command(rofl::openflow12::OFPFC_DELETE);
		fe.set_table_id(rofl::openflow12::OFPTT_ALL /*all tables*/);
	} break;
	case openflow13::OFP_VERSION: {
		fe.set_command(rofl::openflow13::OFPFC_DELETE);
		fe.set_table_id(rofl::openflow13::OFPTT_ALL /*all tables*/);
	} break;
	default: throw eBadVersion();
	}

	send_flow_mod_message(cauxid(0), fe);
}



void
crofdpt::group_mod_reset()
{
	crofdpt::clear_group_ids();
	rofl::openflow::cofgroupmod ge(rofchan.get_version());
	switch (rofchan.get_version()) {
	case openflow12::OFP_VERSION: {
		ge.set_command(openflow12::OFPGC_DELETE);
		ge.set_group_id(openflow12::OFPG_ALL);
	} break;
	case openflow13::OFP_VERSION: {
		ge.set_command(openflow13::OFPGC_DELETE);
		ge.set_group_id(openflow13::OFPG_ALL);
	} break;
	default:
		throw eBadVersion();
	}

	send_group_mod_message(cauxid(0), ge);
}



uint32_t
crofdpt::send_features_request(
		const rofl::cauxid& auxid,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_FEATURES_REQUEST);

		rofl::openflow::cofmsg_features_request *msg =
				new rofl::openflow::cofmsg_features_request(rofchan.get_version(), xid);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_get_config_request(
		const rofl::cauxid& auxid,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_GET_CONFIG_REQUEST);

		rofl::openflow::cofmsg_get_config_request *msg =
				new rofl::openflow::cofmsg_get_config_request(rofchan.get_version(), xid);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_stats_request(
		const rofl::cauxid& auxid,
		uint16_t stats_type,
		uint16_t stats_flags,
		uint8_t* body,
		size_t bodylen,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST);

		rofl::openflow::cofmsg_stats *msg =
				new rofl::openflow::cofmsg_stats(
						rofchan.get_version(),
						xid,
						stats_type,
						stats_flags,
						body,
						bodylen);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_desc_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_DESC);

		rofl::openflow::cofmsg_desc_stats_request *msg =
				new rofl::openflow::cofmsg_desc_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_flow_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofflow_stats_request& flow_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_FLOW);

		rofl::openflow::cofmsg_flow_stats_request *msg =
				new rofl::openflow::cofmsg_flow_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						flow_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_aggr_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofaggr_stats_request& aggr_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_AGGREGATE);

		rofl::openflow::cofmsg_aggr_stats_request *msg =
				new rofl::openflow::cofmsg_aggr_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						aggr_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_table_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_TABLE);

		rofl::openflow::cofmsg_table_stats_request *msg =
				new rofl::openflow::cofmsg_table_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_port_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofport_stats_request& port_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_PORT_STATS);

		rofl::openflow::cofmsg_port_stats_request *msg =
				new rofl::openflow::cofmsg_port_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						port_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_queue_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofqueue_stats_request& queue_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_QUEUE);

		rofl::openflow::cofmsg_queue_stats_request *msg =
				new rofl::openflow::cofmsg_queue_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						queue_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_group_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const rofl::openflow::cofgroup_stats_request& group_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_GROUP);

		rofl::openflow::cofmsg_group_stats_request *msg =
				new rofl::openflow::cofmsg_group_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						group_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_group_desc_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_GROUP_DESC);

		rofl::openflow::cofmsg_group_desc_stats_request *msg =
				new rofl::openflow::cofmsg_group_desc_stats_request(
						rofchan.get_version(),
						xid,
						flags);


		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_group_features_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_GROUP_FEATURES);

		rofl::openflow::cofmsg_group_features_stats_request *msg =
				new rofl::openflow::cofmsg_group_features_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_table_features_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_TABLE_FEATURES);

		rofl::openflow::cofmsg_table_features_stats_request *msg =
				new rofl::openflow::cofmsg_table_features_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_port_desc_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_PORT_DESC);

		rofl::openflow::cofmsg_port_desc_stats_request *msg =
				new rofl::openflow::cofmsg_port_desc_stats_request(
						rofchan.get_version(),
						xid,
						flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_experimenter_stats_request(
		const rofl::cauxid& auxid,
		uint16_t flags,
		uint32_t exp_id,
		uint32_t exp_type,
		const cmemory& body,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_EXPERIMENTER);

		rofl::openflow::cofmsg_experimenter_stats_request *msg =
				new rofl::openflow::cofmsg_experimenter_stats_request(
						rofchan.get_version(),
						xid,
						flags,
						exp_id,
						exp_type,
						body);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_meter_stats_request(
		const rofl::cauxid& auxid,
		uint16_t stats_flags,
		const rofl::openflow::cofmeter_stats_request& meter_stats_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_METER);

		rofl::openflow::cofmsg_meter_stats_request *msg =
				new rofl::openflow::cofmsg_meter_stats_request(
						rofchan.get_version(),
						xid,
						stats_flags,
						meter_stats_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_meter_config_stats_request(
		const rofl::cauxid& auxid,
		uint16_t stats_flags,
		const rofl::openflow::cofmeter_config_request& meter_config_request,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_METER_CONFIG);

		rofl::openflow::cofmsg_meter_config_stats_request *msg =
				new rofl::openflow::cofmsg_meter_config_stats_request(
						rofchan.get_version(),
						xid,
						stats_flags,
						meter_config_request);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_meter_features_stats_request(
		const rofl::cauxid& auxid,
		uint16_t stats_flags,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_MULTIPART_REQUEST, rofl::openflow::OFPMP_METER_FEATURES);

		rofl::openflow::cofmsg_meter_features_stats_request *msg =
				new rofl::openflow::cofmsg_meter_features_stats_request(
						rofchan.get_version(),
						xid,
						stats_flags);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_packet_out_message(
		const rofl::cauxid& auxid,
		uint32_t buffer_id,
		uint32_t in_port,
		const rofl::openflow::cofactions& actions,
		uint8_t *data,
		size_t datalen)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_packet_out *msg =
				new rofl::openflow::cofmsg_packet_out(
						rofchan.get_version(),
						xid,
						buffer_id,
						in_port,
						actions,
						data,
						datalen);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_barrier_request(
		const rofl::cauxid& auxid,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_BARRIER_REQUEST);

		rofl::openflow::cofmsg_barrier_request *msg =
				new rofl::openflow::cofmsg_barrier_request(
						rofchan.get_version(),
						xid);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_role_request(
		const rofl::cauxid& auxid,
		const rofl::openflow::cofrole& role,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_ROLE_REQUEST);

		rofl::openflow::cofmsg_role_request *msg =
				new rofl::openflow::cofmsg_role_request(
						rofchan.get_version(),
						xid,
						role);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_flow_mod_message(
		const rofl::cauxid& auxid,
		const rofl::openflow::cofflowmod& fe)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_flow_mod *msg =
				new rofl::openflow::cofmsg_flow_mod(
						rofchan.get_version(),
						xid,
						fe);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_group_mod_message(
		const rofl::cauxid& auxid,
		const rofl::openflow::cofgroupmod& ge)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_group_mod *msg =
				new rofl::openflow::cofmsg_group_mod(
						rofchan.get_version(),
						xid,
						ge);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_table_mod_message(
		const rofl::cauxid& auxid,
		uint8_t table_id,
		uint32_t config)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_table_mod *msg =
				new rofl::openflow::cofmsg_table_mod(
							rofchan.get_version(),
							xid,
							table_id,
							config);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_port_mod_message(
		const rofl::cauxid& auxid,
		uint32_t port_no,
		const caddress_ll& hwaddr,
		uint32_t config,
		uint32_t mask,
		uint32_t advertise)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_port_mod *msg =
				new rofl::openflow::cofmsg_port_mod(
						rofchan.get_version(),
						xid,
						port_no,
						hwaddr,
						config,
						mask,
						advertise);

		rofchan.send_message(auxid, msg);

		ports.set_port(port_no).recv_port_mod(config, mask, advertise);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_set_config_message(
		const rofl::cauxid& auxid,
		uint16_t flags,
		uint16_t miss_send_len)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_set_config *msg =
				new rofl::openflow::cofmsg_set_config(
						rofchan.get_version(),
						xid,
						flags,
						miss_send_len);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_queue_get_config_request(
		const rofl::cauxid& auxid,
		uint32_t port,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_QUEUE_GET_CONFIG_REQUEST);

		rofl::openflow::cofmsg_queue_get_config_request *msg =
				new rofl::openflow::cofmsg_queue_get_config_request(
						rofchan.get_version(),
						xid,
						port);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_get_async_config_request(
		const rofl::cauxid& auxid,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_GET_ASYNC_REQUEST);

		rofl::openflow::cofmsg_get_async_config_request *msg =
				new rofl::openflow::cofmsg_get_async_config_request(
						rofchan.get_version(),
						xid);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_set_async_config_message(
		const rofl::cauxid& auxid,
		const rofl::openflow::cofasync_config& async_config)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_set_async_config *msg =
				new rofl::openflow::cofmsg_set_async_config(
						rofchan.get_version(),
						xid,
						async_config);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



uint32_t
crofdpt::send_meter_mod_message(
		const rofl::cauxid& auxid,
		uint16_t command,
		uint16_t flags,
		uint32_t meter_id,
		const rofl::openflow::cofmeter_bands& meter_bands)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.get_async_xid();

		rofl::openflow::cofmsg_meter_mod *msg =
				new rofl::openflow::cofmsg_meter_mod(
							rofchan.get_version(),
							xid,
							command,
							flags,
							meter_id,
							meter_bands);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		throw;
	}
}



void
crofdpt::send_error_message(
		const rofl::cauxid& auxid,
		uint32_t xid,
		uint16_t type,
		uint16_t code,
		uint8_t* data,
		size_t datalen)
{
	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		rofl::openflow::cofmsg_error *msg =
				new rofl::openflow::cofmsg_error(
						rofchan.get_version(),
						xid,
						type,
						code,
						data,
						datalen);

		rofchan.send_message(auxid, msg);

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



uint32_t
crofdpt::send_experimenter_message(
		const rofl::cauxid& auxid,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen,
		const cclock& timeout)
{
	uint32_t xid = 0;

	try {
		if (not is_established()) {
			LOGGING_WARN << "[rofl-common][crofdpt] "
					<< "control channel not connected" << std::endl;
			throw eRofBaseNotConnected();
		}

		xid = transactions.add_ta(timeout, rofl::openflow::OFPT_EXPERIMENTER);

		rofl::openflow::cofmsg_experimenter *msg =
				new rofl::openflow::cofmsg_experimenter(
							rofchan.get_version(),
							xid,
							experimenter_id,
							exp_type,
							body,
							bodylen);

		rofchan.send_message(auxid, msg);

		return xid;

	} catch (eRofBaseCongested& e) {
		LOGGING_WARN << "[rofl-common][crofdpt] "
				<< "control channel congested" << std::endl;
		transactions.drop_ta(xid);
		throw;
	}
}



void
crofdpt::features_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_features_reply& reply = dynamic_cast<rofl::openflow::cofmsg_features_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << get_dpid().str()
				<< " rcvd Features-Reply: " << reply.str() << std::endl;

	try {
		transactions.drop_ta(msg->get_xid());

		dpid			= rofl::cdpid(reply.get_dpid());
		n_buffers 		= reply.get_n_buffers();
		n_tables 		= reply.get_n_tables();
		capabilities 	= reply.get_capabilities();

		switch (rofchan.get_version()) {
		case rofl::openflow10::OFP_VERSION:
		case rofl::openflow12::OFP_VERSION: {
			ports = reply.get_ports();
		} break;
		default: {
			// no ports in OpenFlow 1.3 and beyond in a Features.request
		} break;
		}

		// lower 48bits from dpid as datapath mac address
		hwaddr[0] = (get_dpid().get_uint64_t() & 0x0000ff0000000000ULL) >> 40;
		hwaddr[1] = (get_dpid().get_uint64_t() & 0x000000ff00000000ULL) >> 32;
		hwaddr[2] = (get_dpid().get_uint64_t() & 0x00000000ff000000ULL) >> 24;
		hwaddr[3] = (get_dpid().get_uint64_t() & 0x0000000000ff0000ULL) >> 16;
		hwaddr[4] = (get_dpid().get_uint64_t() & 0x000000000000ff00ULL) >>  8;
		hwaddr[5] = (get_dpid().get_uint64_t() & 0x00000000000000ffULL) >>  0;
		hwaddr[0] &= 0xfc;

		if (STATE_ESTABLISHED == state) {
			call_env().handle_features_reply(*this, auxid, reply);
		}

		push_on_eventqueue(EVENT_FEATURES_REPLY_RCVD);

	} catch (RoflException& e) {

		LOGGING_ERROR << "[rofl-common][crofdpt] eRoflException in Features.reply rcvd" << *msg << std::endl;
		push_on_eventqueue(EVENT_DISCONNECTED);
	}

	delete msg;
}



void
crofdpt::get_config_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_get_config_reply& reply = dynamic_cast<rofl::openflow::cofmsg_get_config_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid: " << get_dpid().str()
			<< " rcvd Get-Config-Reply: " << reply.str() << std::endl;

	transactions.drop_ta(msg->get_xid());

	config 			= reply.get_flags();
	miss_send_len 	= reply.get_miss_send_len();

	if (STATE_ESTABLISHED == state) {
		call_env().handle_get_config_reply(*this, auxid, reply);
	}
	delete msg;

	push_on_eventqueue(EVENT_GET_CONFIG_REPLY_RCVD);
}


void
crofdpt::multipart_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Multipart-Reply message received" << std::endl << *msg;

	transactions.drop_ta(msg->get_xid());

	rofl::openflow::cofmsg_multipart_reply *reply = dynamic_cast<rofl::openflow::cofmsg_multipart_reply*>( msg );
	assert(reply != NULL);

	switch (reply->get_stats_type()) {
	case rofl::openflow13::OFPMP_DESC: {
		desc_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_FLOW: {
		flow_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_AGGREGATE: {
		aggregate_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_TABLE: {
		table_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_PORT_STATS: {
		port_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_QUEUE: {
		queue_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_GROUP: {
		group_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_GROUP_DESC: {
		group_desc_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_GROUP_FEATURES: {
		group_features_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_METER: {
		meter_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_METER_CONFIG: {
		meter_config_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_METER_FEATURES: {
		meter_features_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_TABLE_FEATURES: {
		table_features_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_PORT_DESC: {
		port_desc_stats_reply_rcvd(auxid, msg);
	} break;
	case rofl::openflow13::OFPMP_EXPERIMENTER: {
		experimenter_stats_reply_rcvd(auxid, msg);
	} break;
	default: {
		if (STATE_ESTABLISHED != state) {
			LOGGING_WARN << "[rofl-common][crofdpt] rcvd Multipart-Reply without being "
					"established, dropping message:" << std::endl << *reply;
			return;
		}
		call_env().handle_stats_reply(*this, auxid, dynamic_cast<rofl::openflow::cofmsg_stats_reply&>( *msg ));
	};
	}
}



void
crofdpt::desc_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_desc_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_desc_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " rcvd Desc-Stats-Reply: " << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_desc_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::table_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_table_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_table_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << get_dpid().str()
			<< " rcvd Table-Stats-Reply: " << reply.str() << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_table_stats_reply(*this, auxid, reply);
	} else {
		push_on_eventqueue(EVENT_TABLE_STATS_REPLY_RCVD);
	}
	delete msg;
}



void
crofdpt::port_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_port_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_port_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Port-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_port_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::flow_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_flow_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Flow-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_flow_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::aggregate_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_aggr_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_aggr_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Aggregate-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_aggregate_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::queue_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_queue_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Queue-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_queue_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::group_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_group_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_group_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Group-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_group_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::group_desc_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_group_desc_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Group-Desc-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_group_desc_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::group_features_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_group_features_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_group_features_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Group-Features-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_group_features_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::meter_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_meter_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Meter-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_meter_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::meter_config_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_meter_config_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Meter-Config-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_meter_config_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::meter_features_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_meter_features_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Meter-Features-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_meter_features_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::table_features_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_table_features_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << get_dpid().str()
			<< " rcvd Table-Features-Stats-Reply: " << reply.str() << std::endl;

	tables = reply.get_tables();

	if (STATE_ESTABLISHED == state) {
		call_env().handle_table_features_stats_reply(*this, auxid, reply);
	}
	delete msg;

	push_on_eventqueue(EVENT_TABLE_FEATURES_STATS_REPLY_RCVD);
}



void
crofdpt::port_desc_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_port_desc_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " rcvd Port-Desc-Stats-Reply: " << reply.str() << std::endl;

	ports = reply.get_ports();

	if (STATE_ESTABLISHED == state) {
		call_env().handle_port_desc_stats_reply(*this, auxid, reply);
	}
	delete msg;

	push_on_eventqueue(EVENT_PORT_DESC_STATS_REPLY_RCVD);
}



void
crofdpt::experimenter_stats_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_experimenter_stats_reply& reply = dynamic_cast<rofl::openflow::cofmsg_experimenter_stats_reply&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Experimenter-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_experimenter_stats_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::barrier_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_barrier_reply& reply = dynamic_cast<rofl::openflow::cofmsg_barrier_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Barrier-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_barrier_reply(*this, auxid, reply);
	}
	delete msg;
}




void
crofdpt::flow_removed_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_flow_removed& flow_removed = dynamic_cast<rofl::openflow::cofmsg_flow_removed&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Flow-Removed message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_flow_removed(*this, auxid, flow_removed);
		delete msg;
	} else {
		dlqueue.store(msg);
	}
}



void
crofdpt::packet_in_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_packet_in& packet_in = dynamic_cast<rofl::openflow::cofmsg_packet_in&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Packet-In message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_packet_in(*this, auxid, packet_in);
		delete msg;
	} else {
		dlqueue.store(msg);
	}
}



void
crofdpt::port_status_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_port_status& port_status = dynamic_cast<rofl::openflow::cofmsg_port_status&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Port-Status message received" << std::endl;

	ports.set_version(rofchan.get_version());

	switch (port_status.get_reason()) {
	case openflow::OFPPR_ADD: {
		ports.add_port(port_status.get_port().get_port_no()) = port_status.get_port();
	} break;
	case openflow::OFPPR_DELETE: {
		ports.drop_port(port_status.get_port().get_port_no());
	} break;
	case openflow::OFPPR_MODIFY: {
		ports.set_port(port_status.get_port().get_port_no()) = port_status.get_port();
	} break;
	default: {
		delete msg; return;
	} break;
	}

	if (STATE_ESTABLISHED == state) {
		call_env().handle_port_status(*this, auxid, port_status);
		delete msg;
	} else {
		dlqueue.store(msg);
	}
}


void
crofdpt::experimenter_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_experimenter& exp = dynamic_cast<rofl::openflow::cofmsg_experimenter&>( *msg );

	transactions.drop_ta(msg->get_xid());

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Experimenter message received" << std::endl << exp;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_experimenter_message(*this, auxid, exp);
	}
	delete msg;
}



void
crofdpt::error_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_error& error = dynamic_cast<rofl::openflow::cofmsg_error&>( *msg );

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Error message received" << std::endl << error;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_error_message(*this, auxid, error);
	}
	delete msg;
}



void
crofdpt::role_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_role_reply& reply = dynamic_cast<rofl::openflow::cofmsg_role_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Role-Reply message received" << std::endl << reply;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_role_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::queue_get_config_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_queue_get_config_reply& reply = dynamic_cast<rofl::openflow::cofmsg_queue_get_config_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< " Queue-Get-Config-Reply message received" << std::endl << reply;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_queue_get_config_reply(*this, auxid, reply);
	}
	delete msg;
}



void
crofdpt::get_async_config_reply_rcvd(
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_get_async_config_reply& reply = dynamic_cast<rofl::openflow::cofmsg_get_async_config_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	LOGGING_DEBUG2 << "[rofl-common][crofdpt] dpid:" << std::hex << get_dpid().str() << std::dec
			<< "Get-Async-Config-Reply message received" << std::endl << reply;

	if (STATE_ESTABLISHED == state) {
		call_env().handle_get_async_config_reply(*this, auxid, reply);
	}
	delete msg;
}




