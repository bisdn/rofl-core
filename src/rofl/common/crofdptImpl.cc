/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crofdptImpl.h"

using namespace rofl;


crofdptImpl::crofdptImpl(
		crofbase *rofbase,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap) :
				crofdpt(rofbase),
				rofchan(this, versionbitmap),
				dpid(0),
				hwaddr(cmacaddr("00:00:00:00:00:00")),
				n_buffers(0),
				n_tables(0),
				capabilities(0),
				config(0),
				miss_send_len(0),
				rofbase(rofbase),
				transactions(this),
				features_request_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
				get_config_request_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
				table_features_request_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
				barrier_reply_timeout(DEFAULT_DP_BARRIER_REPLY_TIMEOUT),
				get_async_config_reply_timeout(DEFAULT_DP_GET_ASYNC_CONFIG_REPLY_TIMEOUT),
				state(STATE_INIT)
{
	run_engine(EVENT_DISCONNECTED);
}



crofdptImpl::crofdptImpl(
		crofbase *rofbase,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int newsd) :
				crofdpt(rofbase),
				rofchan(this, versionbitmap),
				dpid(0),
				hwaddr(cmacaddr("00:00:00:00:00:00")),
				n_buffers(0),
				n_tables(0),
				capabilities(0),
				config(0),
				miss_send_len(0),
				rofbase(rofbase),
				transactions(this),
				features_request_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
				get_config_request_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
				table_features_request_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
				barrier_reply_timeout(DEFAULT_DP_BARRIER_REPLY_TIMEOUT),
				get_async_config_reply_timeout(DEFAULT_DP_GET_ASYNC_CONFIG_REPLY_TIMEOUT),
				state(STATE_INIT)
{
	run_engine(EVENT_CONNECTED);
}



crofdptImpl::crofdptImpl(
		crofbase *rofbase,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				crofdpt(rofbase),
				rofchan(this, versionbitmap),
				dpid(0),
				hwaddr(cmacaddr("00:00:00:00:00:00")),
				n_buffers(0),
				n_tables(0),
				capabilities(0),
				config(0),
				miss_send_len(0),
				rofbase(rofbase),
				transactions(this),
				features_request_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
				get_config_request_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
				table_features_request_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
				barrier_reply_timeout(DEFAULT_DP_BARRIER_REPLY_TIMEOUT),
				get_async_config_reply_timeout(DEFAULT_DP_GET_ASYNC_CONFIG_REPLY_TIMEOUT),
				state(STATE_INIT)
{
	rofchan.add_conn(0, domain, type, protocol, ra);
}



crofdptImpl::~crofdptImpl()
{
	// remove all cofport instances
	while (not ports.empty()) {
		delete (ports.begin()->second);
	}
}



void
crofdptImpl::run_engine(crofdptImpl_event_t event)
{
	if (EVENT_NONE != event) {
		events.push_back(event);
	}

	while (not events.empty()) {
		enum crofdptImpl_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_CONNECTED: 						event_connected(); 						break;
		case EVENT_DISCONNECTED:					event_disconnected();					break;
		case EVENT_FEATURES_REPLY_RCVD:				event_features_reply_rcvd();			break;
		case EVENT_FEATURES_REQUEST_EXPIRED:		event_features_request_expired();		break;
		case EVENT_GET_CONFIG_REPLY_RCVD:			event_get_config_reply_rcvd();			break;
		case EVENT_GET_CONFIG_REQUEST_EXPIRED:		event_get_config_request_expired();		break;
		case EVENT_TABLE_FEATURES_REPLY_RCVD:		event_table_features_reply_rcvd();		break;
		case EVENT_TABLE_FEATURES_REQUEST_EXPIRED:	event_table_features_request_expired(); break;
		default: {
			logging::error << "[rofl][dpt] unknown event seen, internal error" << std::endl << *this;
		};
		}
	}
}



void
crofdptImpl::event_connected()
{
	switch (state) {
	case STATE_INIT:
	case STATE_DISCONNECTED: {
		send_features_request();
		register_timer(TIMER_WAIT_FOR_FEATURES_REPLY, features_request_timeout);
		state = STATE_CONNECTED;
	} break;
	default: {
		logging::error << "[rofl][dpt] event -CONNECTED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdptImpl::event_disconnected()
{
	switch (state) {
	case STATE_ESTABLISHED: {
		cancel_all_timer();
		rofbase->handle_dpt_close(this);
		state = STATE_DISCONNECTED;
	} break;
	default: {
		logging::error << "[rofl][dpt] event -DISCONNECTED- in invalid state rcvd, internal error" << std::endl << *this;
		cancel_all_timer();
		state = STATE_DISCONNECTED;
	};
	}
}



void
crofdptImpl::event_features_reply_rcvd()
{
	switch (state) {
	case STATE_CONNECTED: {
		cancel_timer(TIMER_WAIT_FOR_FEATURES_REPLY);
		send_get_config_request();
		register_timer(TIMER_WAIT_FOR_GET_CONFIG_REPLY, get_config_request_timeout);
		state = STATE_FEATURES_RCVD;
	} break;
	case STATE_ESTABLISHED: {
		// do nothing: Feature.requests may be sent by a derived class during state ESTABLISHED
		cancel_timer(TIMER_WAIT_FOR_FEATURES_REPLY);
	} break;
	default: {
		logging::error << "[rofl][dpt] event -FEATURES-REPLY-RCVD- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdptImpl::event_features_request_expired()
{
	switch (state) {
	case STATE_CONNECTED: {
		cancel_all_timer();
		state = STATE_DISCONNECTED;
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
	} break;
	default: {
		logging::error << "[rofl][dpt] event -FEATURES-REQUEST-EXPIRED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdptImpl::event_get_config_reply_rcvd()
{
	switch (state) {
	case STATE_FEATURES_RCVD: {
		cancel_timer(TIMER_WAIT_FOR_GET_CONFIG_REPLY);
		send_table_features_stats_request(0);
		register_timer(TIMER_WAIT_FOR_FEATURES_REPLY, table_features_request_timeout);
		state = STATE_GET_CONFIG_RCVD;
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
		cancel_timer(TIMER_WAIT_FOR_GET_CONFIG_REPLY);
	} break;
	default: {
		logging::error << "[rofl][dpt] event -GET-CONFIG-REPLY-RCVD- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdptImpl::event_get_config_request_expired()
{
	switch (state) {
	case STATE_FEATURES_RCVD: {
		cancel_all_timer();
		state = STATE_DISCONNECTED;
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
	} break;
	default: {
		logging::error << "[rofl][dpt] event -GET-CONFIG-REQUEST-EXPIRED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdptImpl::event_table_features_reply_rcvd()
{
	switch (state) {
	case STATE_GET_CONFIG_RCVD: {
		cancel_timer(TIMER_WAIT_FOR_TABLE_FEATURES_REPLY);
		state = STATE_ESTABLISHED;
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
		cancel_timer(TIMER_WAIT_FOR_TABLE_FEATURES_REPLY);
	} break;
	default: {
		logging::error << "[rofl][dpt] event -GET-CONFIG-REPLY-RCVD- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdptImpl::event_table_features_request_expired()
{
	switch (state) {
	case STATE_GET_CONFIG_RCVD: {
		cancel_all_timer();
		state = STATE_DISCONNECTED;
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
	} break;
	default: {
		logging::error << "[rofl][dpt] event -GET-CONFIG-REQUEST-EXPIRED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdptImpl::handle_connected(rofl::openflow::crofchan *chan, uint8_t aux_id)
{
	logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " connected:" << std::endl << *chan;

	if (0 == aux_id) {
		run_engine(EVENT_CONNECTED);
	}
}

void
crofdptImpl::handle_closed(rofl::openflow::crofchan *chan, uint8_t aux_id)
{
	logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " connection closed:" << std::endl << *chan;

	if (0 == aux_id) {
		run_engine(EVENT_DISCONNECTED);
	}
}

void
crofdptImpl::recv_message(rofl::openflow::crofchan *chan, uint8_t aux_id, cofmsg *msg)
{
	try {
		switch (msg->get_type()) {
		case rofl::openflow::OFPT_FEATURES_REPLY: {
			features_reply_rcvd(dynamic_cast<cofmsg_features_reply*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_GET_CONFIG_REPLY: {
			get_config_reply_rcvd(dynamic_cast<cofmsg_get_config_reply*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_PACKET_IN: {
			packet_in_rcvd(dynamic_cast<cofmsg_packet_in*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_FLOW_REMOVED: {
			flow_removed_rcvd(dynamic_cast<cofmsg_flow_removed*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_PORT_STATUS: {
			port_status_rcvd(dynamic_cast<cofmsg_port_status*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_MULTIPART_REPLY: {
			multipart_reply_rcvd(dynamic_cast<cofmsg_multipart_reply*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_BARRIER_REPLY: {
			barrier_reply_rcvd(dynamic_cast<cofmsg_barrier_reply*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_QUEUE_GET_CONFIG_REPLY: {
			queue_get_config_reply_rcvd(dynamic_cast<cofmsg_queue_get_config_reply*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_ROLE_REPLY: {
			role_reply_rcvd(dynamic_cast<cofmsg_role_reply*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_GET_ASYNC_REPLY: {
			get_async_config_reply_rcvd(dynamic_cast<cofmsg_get_async_config_reply*>( msg ), aux_id);
		} break;
		default: {

		};
		}

	} catch (eOpenFlowBase& e) {

	}

	delete msg;
}

uint32_t
crofdptImpl::get_async_xid(rofl::openflow::crofchan *chan)
{
	return transactions.get_async_xid();
}

uint32_t
crofdptImpl::get_sync_xid(rofl::openflow::crofchan *chan)
{
	return transactions.add_ta(cclock(/*secs=*/5));
}

void
crofdptImpl::release_sync_xid(rofl::openflow::crofchan *chan, uint32_t xid)
{
	return transactions.drop_ta(xid);
}


void
crofdptImpl::ta_expired(
		rofl::openflow::ctransactions *tas,
		rofl::openflow::ctransaction& ta)
{
	logging::warn << "[rofl][dpt] transaction expired, xid:" << std::endl << ta;
	// TODO
	//rofbase->ta_expired(ta);
}



void
crofdptImpl::handle_timeout(int opaque)
{
	switch (opaque) {
	case TIMER_WAIT_FOR_FEATURES_REPLY: {
		run_engine(EVENT_FEATURES_REQUEST_EXPIRED);
	} break;
	case TIMER_WAIT_FOR_GET_CONFIG_REPLY: {
		run_engine(EVENT_GET_CONFIG_REQUEST_EXPIRED);
	} break;
	case TIMER_WAIT_FOR_TABLE_FEATURES_REPLY: {
		run_engine(EVENT_TABLE_FEATURES_REQUEST_EXPIRED);
	} break;
	default: {
		logging::error << "[rofl][dpt] dpid:0x"
				<< std::hex << dpid << std::dec
				<< " unknown timer event:" << opaque << std::endl;
	};
	}
}



uint32_t
crofdptImpl::send_features_request()
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_features_request *msg =
			new cofmsg_features_request(rofchan.get_version(), xid);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_get_config_request()
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_get_config_request *msg =
			new cofmsg_get_config_request(rofchan.get_version(), xid);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_table_features_stats_request(
		uint16_t stats_flags)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	throw eNotImplemented();
#if 0
	cofmsg_get_config_request *msg =
			new cofmsg_get_config_request(rofchan.get_version(), xid);

	rofchan.send_message(msg, 0);
#endif

	return xid;
}



uint32_t
crofdptImpl::send_stats_request(
	uint16_t stats_type,
	uint16_t stats_flags,
	uint8_t* body,
	size_t bodylen)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_stats *msg =
			new cofmsg_stats(
					rofchan.get_version(),
					xid,
					stats_type,
					stats_flags,
					body,
					bodylen);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_desc_stats_request(
		uint16_t flags)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_desc_stats_request *msg =
			new cofmsg_desc_stats_request(
					rofchan.get_version(),
					xid,
					flags);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_flow_stats_request(
		uint16_t flags,
		cofflow_stats_request const& flow_stats_request)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_flow_stats_request *msg =
			new cofmsg_flow_stats_request(
					rofchan.get_version(),
					xid,
					flags,
					flow_stats_request);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_aggr_stats_request(
		uint16_t flags,
		cofaggr_stats_request const& aggr_stats_request)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_aggr_stats_request *msg =
			new cofmsg_aggr_stats_request(
					rofchan.get_version(),
					xid,
					flags,
					aggr_stats_request);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_table_stats_request(
		uint16_t flags)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_table_stats_request *msg =
			new cofmsg_table_stats_request(
					rofchan.get_version(),
					xid,
					flags);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_port_stats_request(
		uint16_t flags,
		cofport_stats_request const& port_stats_request)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_port_stats_request *msg =
			new cofmsg_port_stats_request(
					rofchan.get_version(),
					xid,
					flags,
					port_stats_request);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_queue_stats_request(
	uint16_t flags,
	cofqueue_stats_request const& queue_stats_request)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_queue_stats_request *msg =
			new cofmsg_queue_stats_request(
					rofchan.get_version(),
					xid,
					flags,
					queue_stats_request);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_group_stats_request(
	uint16_t flags,
	cofgroup_stats_request const& group_stats_request)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_group_stats_request *msg =
			new cofmsg_group_stats_request(
					rofchan.get_version(),
					xid,
					flags,
					group_stats_request);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_group_desc_stats_request(
		uint16_t flags)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_group_desc_stats_request *msg =
			new cofmsg_group_desc_stats_request(
					rofchan.get_version(),
					xid,
					flags);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_group_features_stats_request(
		uint16_t flags)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_group_features_stats_request *msg =
			new cofmsg_group_features_stats_request(
					rofchan.get_version(),
					xid,
					flags);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_experimenter_stats_request(
	uint16_t flags,
	uint32_t exp_id,
	uint32_t exp_type,
	cmemory const& body)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_experimenter_stats_request *msg =
			new cofmsg_experimenter_stats_request(
					rofchan.get_version(),
					xid,
					flags,
					exp_id,
					exp_type,
					body);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_packet_out_message(
	uint32_t buffer_id,
	uint32_t in_port,
	cofactions& aclist,
	uint8_t *data,
	size_t datalen)
{
	uint32_t xid = transactions.get_async_xid();

	cofmsg_packet_out *msg =
			new cofmsg_packet_out(
					rofchan.get_version(),
					xid,
					buffer_id,
					in_port,
					aclist,
					data,
					datalen);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_barrier_request()
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_barrier_request *msg =
			new cofmsg_barrier_request(
					rofchan.get_version(),
					xid);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_role_request(
	uint32_t role,
	uint64_t generation_id)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_role_request *msg =
			new cofmsg_role_request(
					rofchan.get_version(),
					xid,
					role,
					generation_id);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_flow_mod_message(
		cflowentry const& fe)
{
	uint32_t xid = transactions.get_async_xid();

	cofmsg_flow_mod *msg =
			new cofmsg_flow_mod(
					rofchan.get_version(),
					xid,
					fe);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_group_mod_message(
		cgroupentry const& ge)
{
	uint32_t xid = transactions.get_async_xid();

	cofmsg_group_mod *msg =
			new cofmsg_group_mod(
					rofchan.get_version(),
					xid,
					ge);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_table_mod_message(
		uint8_t table_id,
		uint32_t config)
{
	uint32_t xid = transactions.get_async_xid();

	cofmsg_table_mod *msg =
			new cofmsg_table_mod(
						rofchan.get_version(),
						xid,
						table_id,
						config);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_port_mod_message(
	uint32_t port_no,
	cmacaddr const& hwaddr,
	uint32_t config,
	uint32_t mask,
	uint32_t advertise)
{
	uint32_t xid = transactions.get_async_xid();

	cofmsg_port_mod *msg =
			new cofmsg_port_mod(
					rofchan.get_version(),
					xid,
					port_no,
					hwaddr,
					config,
					mask,
					advertise);

	rofchan.send_message(msg, 0);

	ports[port_no]->recv_port_mod(config, mask, advertise);

	return xid;
}



uint32_t
crofdptImpl::send_set_config_message(
	uint16_t flags,
	uint16_t miss_send_len)
{
	uint32_t xid = transactions.get_async_xid();

	cofmsg_set_config *msg =
			new cofmsg_set_config(
					rofchan.get_version(),
					xid,
					flags,
					miss_send_len);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_queue_get_config_request(
	uint32_t port)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_queue_get_config_request *msg =
			new cofmsg_queue_get_config_request(
					rofchan.get_version(),
					xid,
					port);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_get_async_config_request()
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_get_async_config_request *msg =
			new cofmsg_get_async_config_request(
					rofchan.get_version(),
					xid);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_set_async_config_message(
	uint32_t packet_in_mask0,
	uint32_t packet_in_mask1,
	uint32_t port_status_mask0,
	uint32_t port_status_mask1,
	uint32_t flow_removed_mask0,
	uint32_t flow_removed_mask1)
{
	uint32_t xid = transactions.get_async_xid();

	cofmsg_set_async_config *msg =
			new cofmsg_set_async_config(
					rofchan.get_version(),
					xid,
					packet_in_mask0,
					packet_in_mask1,
					port_status_mask0,
					port_status_mask1,
					flow_removed_mask0,
					flow_removed_mask1);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_error_message(
	uint32_t xid,
	uint16_t type,
	uint16_t code,
	uint8_t* data,
	size_t datalen)
{
	cofmsg_error *msg =
			new cofmsg_error(
					rofchan.get_version(),
					xid,
					type,
					code,
					data,
					datalen);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdptImpl::send_experimenter_message(
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5));

	cofmsg_experimenter *msg =
			new cofmsg_experimenter(
						rofchan.get_version(),
						xid,
						experimenter_id,
						exp_type,
						body,
						bodylen);

	rofchan.send_message(msg, 0);

	return xid;
}







void
crofdptImpl::features_reply_rcvd(
		cofmsg_features_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Features-Reply message received" << std::endl << *msg;

	try {
		cancel_timer(COFDPT_TIMER_FEATURES_REPLY);

		dpid 			= msg->get_dpid();
		n_buffers 		= msg->get_n_buffers();
		n_tables 		= msg->get_n_tables();
		capabilities 	= msg->get_capabilities();

		switch (ofp_version) {
		case openflow10::OFP_VERSION:
		case openflow12::OFP_VERSION: {
			cofportlist& portlist = msg->get_ports();

			for (std::map<uint32_t, cofport*>::iterator it = ports.begin();
					it != ports.end(); ++it) {
				delete it->second;
			}
			ports.clear();

			for (cofportlist::iterator it = portlist.begin();
					it != portlist.end(); ++it) {
				cofport& port = (*it);
				ports[port.get_port_no()] = new cofport(port, &ports, port.get_port_no());
			}

		} break;
		case openflow13::OFP_VERSION: {
			// no ports in OpenFlow 1.3 in FeaturesRequest
		} break;
		}



		// dpid as std::string
		cvastring vas;
		s_dpid = std::string(vas("0x%llx", dpid));

		// lower 48bits from dpid as datapath mac address
		hwaddr[0] = (dpid & 0x0000ff0000000000ULL) >> 40;
		hwaddr[1] = (dpid & 0x000000ff00000000ULL) >> 32;
		hwaddr[2] = (dpid & 0x00000000ff000000ULL) >> 24;
		hwaddr[3] = (dpid & 0x0000000000ff0000ULL) >> 16;
		hwaddr[4] = (dpid & 0x000000000000ff00ULL) >>  8;
		hwaddr[5] = (dpid & 0x00000000000000ffULL) >>  0;
		hwaddr[0] &= 0xfc;

		rofbase->handle_features_reply(this, msg);

		if (STATE_WAIT_FEATURES == cur_state()) {

			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
					<< "Features-Reply rcvd (wait-features-reply -> wait-get-config-reply)" << std::endl;

			// next step: send GET-CONFIG request to datapath
			rofbase->send_get_config_request(this);

			new_state(STATE_WAIT_GET_CONFIG);
		}


	} catch (eOFportMalformed& e) {

		logging::error << "eOFportMalformed " << *msg << std::endl;

		socket->cclose();

		new_state(STATE_DISCONNECTED);

		rofbase->handle_dpt_close(this);
	}
}



void
crofdptImpl::handle_features_reply_timeout()
{
	logging::warn << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Features-Reply timer expired" << *this << std::endl;
	rofbase->handle_features_reply_timeout(this);
}



void
crofdptImpl::get_config_reply_rcvd(
		cofmsg_get_config_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Get-Config-Reply message received" << std::endl << *msg;

	cancel_timer(COFDPT_TIMER_GET_CONFIG_REPLY);

	config = msg->get_flags();
	miss_send_len = msg->get_miss_send_len();

	rofbase->handle_get_config_reply(this, msg);

	if (STATE_WAIT_GET_CONFIG == cur_state()) {

		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			new_state(STATE_CONNECTED);
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
					<< "Get-Config-Reply rcvd (wait-get-config-reply -> connected)" << std::endl;
			rofbase->handle_dpt_open(this);
		} break;
		case openflow12::OFP_VERSION: {
			rofbase->send_stats_request(this, openflow12::OFPST_TABLE, 0);
			new_state(STATE_WAIT_TABLE_STATS);
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
					<< "Get-Config-Reply rcvd (wait-get-config-reply -> wait-table-stats-reply)" << std::endl;
		} break;
		case openflow13::OFP_VERSION: {
			rofbase->send_stats_request(this, openflow13::OFPMP_TABLE, 0);
			new_state(STATE_WAIT_TABLE_STATS);
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
								<< "Get-Config-Reply rcvd (wait-get-config-reply -> wait-table-stats-reply)" << std::endl;
		} break;
		}

	}
}



void
crofdptImpl::handle_get_config_reply_timeout()
{
	logging::warn << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Get-Config-Reply timer expired" << *this << std::endl;
	rofbase->handle_get_config_reply_timeout(this);
}



void
crofdptImpl::stats_reply_rcvd(
		cofmsg_stats_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Stats-Reply message received" << std::endl << *msg;

	cancel_timer(COFDPT_TIMER_STATS_REPLY);

	switch (ofp_version) {
	case openflow10::OFP_VERSION: {
		xidstore[openflow10::OFPT_STATS_REQUEST].xid_rem(msg->get_xid());
	} break;
	case openflow12::OFP_VERSION: {
		xidstore[openflow12::OFPT_STATS_REQUEST].xid_rem(msg->get_xid());
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented(); // yet
	} break;
	default:
		throw eBadVersion();
	}

	rofbase->handle_stats_reply(this, dynamic_cast<cofmsg_stats*>( msg ));
}



void
crofdptImpl::handle_stats_reply_timeout()
{
	logging::warn << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Stats-Reply timer expired" << *this << std::endl;

	uint8_t msg_type = 0;

	switch (ofp_version) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_STATS_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_MULTIPART_REQUEST; break;
	default:
		throw eBadVersion();
	}


restart:
	for (cxidstore::iterator
				it = xidstore[msg_type].begin();
							it != xidstore[msg_type].end(); ++it) {
		cxidtrans& xidt = it->second;

		if (xidt.timeout <= cclock::now()) {
			rofbase->handle_stats_reply_timeout(this, xidt.xid);
			xidstore[msg_type].xid_rem(xidt.xid);
			goto restart;
		}
	}

	if (not xidstore.empty()) {
		reset_timer(COFDPT_TIMER_STATS_REPLY, stats_reply_timeout);
	}
}



void
crofdptImpl::desc_stats_reply_rcvd(cofmsg_desc_stats_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Desc-Stats-Reply message received" << std::endl << *msg;

	rofbase->handle_desc_stats_reply(this, msg);
}



void
crofdptImpl::table_stats_reply_rcvd(
		cofmsg_table_stats_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Table-Stats-Reply message received" << std::endl << *msg;

	// clear our old table map
	tables.clear();
	// iterate through all received table stats bodies and fill in our local tables map
	for (std::vector<coftable_stats_reply>::iterator
			it = msg->get_table_stats().begin(); it != msg->get_table_stats().end(); ++it) {
		coftable_stats_reply& table = (*it);
		tables[table.get_table_id()] = table;
	}

	rofbase->handle_table_stats_reply(this,  msg);

	if (STATE_WAIT_TABLE_STATS == cur_state()) {
		new_state(STATE_CONNECTED);
		logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
							<< "Table-Stats-Reply rcvd (wait-table-stats-reply -> connected)" << std::endl;
		rofbase->handle_dpt_open(this);
	}
}



void
crofdptImpl::port_stats_reply_rcvd(cofmsg_port_stats_reply* msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Port-Stats-Reply message received" << std::endl << *msg;

	rofbase->handle_port_stats_reply(this, msg);
}



void
crofdptImpl::flow_stats_reply_rcvd(cofmsg_flow_stats_reply* msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Flow-Stats-Reply message received" << std::endl << *msg;

	rofbase->handle_flow_stats_reply(this, msg);
}



void
crofdptImpl::aggregate_stats_reply_rcvd(cofmsg_aggr_stats_reply* msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Aggregate-Stats-Reply message received" << std::endl << *msg;

	rofbase->handle_aggregate_stats_reply(this, msg);
}



void
crofdptImpl::queue_stats_reply_rcvd(cofmsg_queue_stats_reply* msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Queue-Stats-Reply message received" << std::endl << *msg;

	rofbase->handle_queue_stats_reply(this, msg);
}



void
crofdptImpl::group_stats_reply_rcvd(cofmsg_group_stats_reply* msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Group-Stats-Reply message received" << std::endl << *msg;

	rofbase->handle_group_stats_reply(this, msg);
}



void
crofdptImpl::group_desc_stats_reply_rcvd(cofmsg_group_desc_stats_reply* msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Group-Desc-Stats-Reply message received" << std::endl << *msg;

	rofbase->handle_group_desc_stats_reply(this, msg);
}



void
crofdptImpl::group_features_stats_reply_rcvd(cofmsg_group_features_stats_reply* msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Group-Features-Stats-Reply message received" << std::endl << *msg;

	rofbase->handle_group_features_stats_reply(this, msg);
}



void
crofdptImpl::experimenter_stats_reply_rcvd(cofmsg_experimenter_stats_reply* msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Experimenter-Stats-Reply message received" << std::endl << *msg;

	rofbase->handle_experimenter_stats_reply(this, msg);
}



void
crofdptImpl::barrier_reply_rcvd(cofmsg_barrier_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Barrier-Reply message received" << std::endl << *msg << std::endl;

	cancel_timer(COFDPT_TIMER_BARRIER_REPLY);

	uint8_t msg_type = 0;

	switch (ofp_version) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_BARRIER_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_BARRIER_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_BARRIER_REQUEST; break;
	default:
		throw eBadVersion();
	}

	xidstore[msg_type].xid_rem(msg->get_xid());

	rofbase->handle_barrier_reply(this, msg);
}



void
crofdptImpl::handle_barrier_reply_timeout()
{
	logging::warn << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Barrier-Reply timer expired" << *this << std::endl;

	uint8_t msg_type = 0;

	switch (ofp_version) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_BARRIER_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_BARRIER_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_BARRIER_REQUEST; break;
	default:
		throw eBadVersion();
	}

restart:
	for (cxidstore::iterator
			it = xidstore[msg_type].begin();
						it != xidstore[msg_type].end(); ++it)
	{
		cxidtrans& xidt = it->second;

		if (xidt.timeout <= cclock::now())
		{
			rofbase->handle_barrier_reply_timeout(this, xidt.xid);

			xidstore[msg_type].xid_rem(xidt.xid);

			goto restart;
		}
	}

	if (not xidstore.empty())
	{
		reset_timer(COFDPT_TIMER_BARRIER_REPLY, barrier_reply_timeout);
	}
}



void
crofdptImpl::flow_rmvd_rcvd(
		cofmsg_flow_removed *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Flow-Removed message received" << std::endl << *msg << std::endl;

	rofbase->handle_flow_removed(this, msg);
}



void
crofdptImpl::flow_mod_reset()
{
	cflowentry fe(ofp_version);
	switch (ofp_version) {
	case openflow10::OFP_VERSION: {
		fe.set_command(openflow10::OFPFC_DELETE);
	} break;
	case openflow12::OFP_VERSION: {
		fe.set_command(openflow12::OFPFC_DELETE);
		fe.set_table_id(openflow12::OFPTT_ALL /*all tables*/);
	} break;
	case openflow13::OFP_VERSION: {
		fe.set_command(openflow13::OFPFC_DELETE);
		fe.set_table_id(openflow13::OFPTT_ALL /*all tables*/);
	} break;
	default: throw eBadVersion();
	}

	rofbase->send_flow_mod_message(this, fe); // calls this->flow_mod_sent() implicitly
}



void
crofdptImpl::group_mod_reset()
{
	cgroupentry ge(get_version());
	switch (ofp_version) {
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

	rofbase->send_group_mod_message(this, ge); // calls this->group_mod_sent() implicitly
}



void
crofdptImpl::packet_in_rcvd(cofmsg_packet_in *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Packet-In message received" << std::endl << *msg << std::endl;

	try {
		// datalen must be at least one Ethernet header in size
		if (msg->get_packet().length() >= (2 * OFP_ETH_ALEN + sizeof(uint16_t))) {

			// let derived class handle PACKET-IN event
			rofbase->handle_packet_in(this, msg);
		}
	} catch (eOFmatchNotFound& e) {

		logging::error << "eOFmatchNotFound " << *msg << std::endl;
	}
}



void
crofdptImpl::port_status_rcvd(cofmsg_port_status *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Port-Status message received" << std::endl << *msg << std::endl;

	std::map<uint32_t, cofport*>::iterator it;
	switch (msg->get_reason()) {
	case openflow::OFPPR_ADD: {
		if (ports.find(msg->get_port().get_port_no()) == ports.end()) {
			new cofport(msg->get_port(), &ports, msg->get_port().get_port_no());
			rofbase->handle_port_status(this, msg);
		}
	} break;
	case openflow::OFPPR_DELETE: {
		if (ports.find(msg->get_port().get_port_no()) != ports.end()) {
			uint32_t port_no = msg->get_port().get_port_no();
			rofbase->handle_port_status(this, msg);
			delete ports[port_no];
			ports.erase(port_no);
		}
	} break;
	case openflow::OFPPR_MODIFY: {
		if (ports.find(msg->get_port().get_port_no()) != ports.end()) {
			*(ports[msg->get_port().get_port_no()]) = msg->get_port();
			rofbase->handle_port_status(this, msg);
		}
	} break;
	default: {
		delete msg;
	} break;
	}
}


void
crofdptImpl::fsp_open(cofmatch const& ofmatch)
{
	cofmatch m(ofmatch);
	croflexp_flowspace rexp(croflexp::OFPRET_FSP_ADD, m);

	cmemory packed(rexp.length());

	rexp.pack(packed.somem(), packed.memlen());

	rofbase->send_experimenter_message(
			this,
			OFPEXPID_ROFL,
			croflexp::OFPRET_FLOWSPACE,
			packed.somem(),
			packed.memlen());

}


void
crofdptImpl::fsp_close(cofmatch const& ofmatch)
{
	cofmatch m(ofmatch);
	croflexp_flowspace rexp(croflexp::OFPRET_FSP_DELETE, m);

	cmemory packed(rexp.length());

	rexp.pack(packed.somem(), packed.memlen());

	rofbase->send_experimenter_message(
			this,
			OFPEXPID_ROFL,
			croflexp::OFPRET_FLOWSPACE,
			packed.somem(),
			packed.memlen());

}



void
crofdptImpl::experimenter_rcvd(cofmsg_experimenter *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Experimenter message received" << std::endl << *msg << std::endl;

	switch (msg->get_experimenter_id()) {
	default:
		break;
	}

	// for now: send vendor extensions directly to class derived from crofbase
	rofbase->handle_experimenter_message(this, msg);
}



void
crofdptImpl::role_reply_rcvd(cofmsg_role_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Role-Reply message received" << std::endl << *msg << std::endl;

	rofbase->handle_role_reply(this, msg);
}



void
crofdptImpl::queue_get_config_reply_rcvd(
		cofmsg_queue_get_config_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Queue-Get-Config-Reply message received" << std::endl << *msg << std::endl;

	rofbase->handle_queue_get_config_reply(this, msg);
}



void
crofdptImpl::get_async_config_reply_rcvd(
		cofmsg_get_async_config_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "Get-Async-Config-Reply message received" << std::endl << *msg << std::endl;

	cancel_timer(COFDPT_TIMER_GET_ASYNC_CONFIG_REPLY);

	// TODO: store mask values into local variables?

	rofbase->handle_get_async_config_reply(this, msg);
}



void
crofdptImpl::handle_get_async_config_reply_timeout()
{
	logging::warn << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Get-Async-Config-Reply timer expired" << *this << std::endl;
	rofbase->handle_get_async_config_reply_timeout(this);
}





cofport*
crofdptImpl::find_cofport(
	uint32_t port_no) throw (eOFdpathNotFound)
{
	std::map<uint32_t, cofport*>::iterator it;
	if (ports.find(port_no) == ports.end())
	{
		throw eOFdpathNotFound();
	}
	return ports[port_no];
}


cofport*
crofdptImpl::find_cofport(
	std::string port_name) throw (eOFdpathNotFound)
{
	std::map<uint32_t, cofport*>::iterator it;
	if ((it = find_if(ports.begin(), ports.end(),
			cofport_find_by_port_name(port_name))) == ports.end())
	{
		throw eOFdpathNotFound();
	}
	return it->second;
}


cofport*
crofdptImpl::find_cofport(
	cmacaddr const& maddr) throw (eOFdpathNotFound)
{
	std::map<uint32_t, cofport*>::iterator it;
	if ((it = find_if(ports.begin(), ports.end(),
			cofport_find_by_maddr(maddr))) == ports.end())
	{
		throw eOFdpathNotFound();
	}
	return it->second;
}


