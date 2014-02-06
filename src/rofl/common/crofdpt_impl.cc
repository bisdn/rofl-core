/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crofdpt_impl.h"

using namespace rofl;


crofdpt_impl::crofdpt_impl(
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
				state(STATE_INIT)
{

}



crofdpt_impl::crofdpt_impl(
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
				state(STATE_INIT)
{
	run_engine(EVENT_DISCONNECTED);
}



crofdpt_impl::crofdpt_impl(
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
				state(STATE_INIT)
{
	rofchan.add_conn(0, domain, type, protocol, ra);
}



crofdpt_impl::~crofdpt_impl()
{
	logging::info << "[rofl][dpt] removing datapath abstraction:" << std::endl << *this;
}



void
crofdpt_impl::run_engine(crofdpt_impl_event_t event)
{
	if (EVENT_NONE != event) {
		events.push_back(event);
	}

	while (not events.empty()) {
		enum crofdpt_impl_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_CONNECTED: {
			event_connected();
		} break;
		case EVENT_DISCONNECTED: {
			event_disconnected();
		} return;
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
			logging::error << "[rofl][dpt] unknown event seen, internal error" << std::endl << *this;
		};
		}
	}
}



void
crofdpt_impl::event_connected()
{
	switch (state) {
	case STATE_INIT:
	case STATE_DISCONNECTED: {
		state = STATE_CONNECTED;
		send_features_request();
	} break;
	default: {
		logging::error << "[rofl][dpt] event -CONNECTED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdpt_impl::event_disconnected()
{
	switch (state) {
	case STATE_ESTABLISHED: {
		rofchan.clear();
		rofbase->handle_dpt_close(this);
	} return;
	default: {
		logging::error << "[rofl][dpt] event -DISCONNECTED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}

	transactions.clear();
	state = STATE_DISCONNECTED;
}



void
crofdpt_impl::event_features_reply_rcvd()
{
	switch (state) {
	case STATE_CONNECTED: {
		logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
				<< "Features-Reply rcvd (connected -> features-reply-rcvd)" << std::endl;
		state = STATE_FEATURES_RCVD;
		send_get_config_request();

	} break;
	case STATE_ESTABLISHED: {
		// do nothing: Feature.requests may be sent by a derived class during state ESTABLISHED

	} break;
	default: {
		logging::error << "[rofl][dpt] event -FEATURES-REPLY-RCVD- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdpt_impl::event_features_request_expired()
{
	switch (state) {
	case STATE_CONNECTED: {
		state = STATE_DISCONNECTED;
	} break;
	case STATE_ESTABLISHED: {

	} break;
	default: {
		logging::error << "[rofl][dpt] event -FEATURES-REQUEST-EXPIRED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdpt_impl::event_get_config_reply_rcvd()
{
	switch (state) {
	case STATE_FEATURES_RCVD: {

		switch (rofchan.get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			state = STATE_ESTABLISHED;
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
							<< "Get-Config-Reply rcvd (features-reply-rcvd -> established)" << std::endl;
			rofbase->handle_dpath_open(*this);

		} break;
		case rofl::openflow12::OFP_VERSION: {
			state = STATE_GET_CONFIG_RCVD;
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
							<< "Get-Config-Reply rcvd (features-reply-rcvd -> get-config-reply-rcvd)" << std::endl;
			send_table_stats_request(0);

		} break;
		case rofl::openflow13::OFP_VERSION:
		default: {
			state = STATE_GET_CONFIG_RCVD;
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
							<< "Get-Config-Reply rcvd (features-reply-rcvd -> get-config-reply-rcvd)" << std::endl;
			send_table_features_stats_request(0);

		} break;
		}


	} break;
	case STATE_ESTABLISHED: {
		// do nothing

	} break;
	default: {
		logging::error << "[rofl][dpt] event -GET-CONFIG-REPLY-RCVD- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdpt_impl::event_get_config_request_expired()
{
	switch (state) {
	case STATE_FEATURES_RCVD: {
		transactions.clear();
		state = STATE_DISCONNECTED;
	} break;
	case STATE_ESTABLISHED: {

	} break;
	default: {
		logging::error << "[rofl][dpt] event -GET-CONFIG-REQUEST-EXPIRED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdpt_impl::event_port_desc_reply_rcvd()
{
	throw eNotImplemented();
#if 0
	switch (state) {
	case STATE_FEATURES_RCVD: {

		switch (rofchan.get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			state = STATE_ESTABLISHED;
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
							<< "Get-Config-Reply rcvd (features-reply-rcvd -> established)" << std::endl;
			rofbase->handle_dpath_open(*this);

		} break;
		case rofl::openflow12::OFP_VERSION: {
			state = STATE_GET_CONFIG_RCVD;
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
							<< "Get-Config-Reply rcvd (features-reply-rcvd -> get-config-reply-rcvd)" << std::endl;
			send_table_stats_request(0);

		} break;
		case rofl::openflow13::OFP_VERSION:
		default: {
			state = STATE_GET_CONFIG_RCVD;
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
							<< "Get-Config-Reply rcvd (features-reply-rcvd -> get-config-reply-rcvd)" << std::endl;
			send_table_features_stats_request(0);

		} break;
		}


	} break;
	case STATE_ESTABLISHED: {
		// do nothing

	} break;
	default: {
		logging::error << "[rofl][dpt] event -GET-CONFIG-REPLY-RCVD- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
#endif
}



void
crofdpt_impl::event_port_desc_request_expired()
{
	throw eNotImplemented();
#if 0
	switch (state) {
	case STATE_FEATURES_RCVD: {
		transactions.clear();
		state = STATE_DISCONNECTED;
	} break;
	case STATE_ESTABLISHED: {

	} break;
	default: {
		logging::error << "[rofl][dpt] event -GET-CONFIG-REQUEST-EXPIRED- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
#endif
}



void
crofdpt_impl::event_table_stats_reply_rcvd()
{
	switch (state) {
	case STATE_GET_CONFIG_RCVD: {

		switch (rofchan.get_version()) {
		case rofl::openflow12::OFP_VERSION: {
			state = STATE_ESTABLISHED;
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
								<< "Table-Stats-Reply rcvd (get-config-rcvd -> established)" << std::endl;
			rofbase->handle_dpt_open(this);
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
		logging::error << "[rofl][dpt] event -TABLE-STATS-REPLY-RCVD- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdpt_impl::event_table_stats_request_expired()
{
	switch (state) {
	case STATE_GET_CONFIG_RCVD: {
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
crofdpt_impl::event_table_features_stats_reply_rcvd()
{
	switch (state) {
	case STATE_GET_CONFIG_RCVD: {
		state = STATE_ESTABLISHED;
	} break;
	case STATE_ESTABLISHED: {
		// do nothing
	} break;
	default: {
		logging::error << "[rofl][dpt] event -GET-CONFIG-REPLY-RCVD- in invalid state rcvd, internal error" << std::endl << *this;
	};
	}
}



void
crofdpt_impl::event_table_features_stats_request_expired()
{
	switch (state) {
	case STATE_GET_CONFIG_RCVD: {
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
crofdpt_impl::handle_established(rofl::openflow::crofchan *chan)
{
	logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " channel established:" << std::endl << *chan;
	run_engine(EVENT_CONNECTED);
}


void
crofdpt_impl::handle_disconnected(rofl::openflow::crofchan *chan)
{
	logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " channel disconnected:" << std::endl << *chan;

	transactions.clear();

	run_engine(EVENT_DISCONNECTED);
}


void
crofdpt_impl::recv_message(rofl::openflow::crofchan *chan, uint8_t aux_id, cofmsg *msg)
{
	try {
		switch (msg->get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_VENDOR: {
				experimenter_rcvd(msg, aux_id);
			} break;
			case rofl::openflow10::OFPT_ERROR: {
				error_rcvd(msg, aux_id);
			} break;
			case rofl::openflow10::OFPT_FEATURES_REPLY: {
				features_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow10::OFPT_GET_CONFIG_REPLY: {
				get_config_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow10::OFPT_PACKET_IN: {
				packet_in_rcvd(msg, aux_id);
			} break;
			case rofl::openflow10::OFPT_FLOW_REMOVED: {
				flow_removed_rcvd(msg, aux_id);
			} break;
			case rofl::openflow10::OFPT_PORT_STATUS: {
				port_status_rcvd(msg, aux_id);
			} break;
			case rofl::openflow10::OFPT_STATS_REPLY: {
				multipart_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow10::OFPT_BARRIER_REPLY: {
				barrier_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY: {
				queue_get_config_reply_rcvd(msg, aux_id);
			} break;
			default: {
			};
			}

		} break;
		case rofl::openflow12::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_EXPERIMENTER: {
				experimenter_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_ERROR: {
				error_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_FEATURES_REPLY: {
				features_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_GET_CONFIG_REPLY: {
				get_config_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_PACKET_IN: {
				packet_in_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_FLOW_REMOVED: {
				flow_removed_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_PORT_STATUS: {
				port_status_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_STATS_REPLY: {
				multipart_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_BARRIER_REPLY: {
				barrier_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY: {
				queue_get_config_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow12::OFPT_ROLE_REPLY: {
				role_reply_rcvd(msg, aux_id);
			} break;
			default: {
			};
			}
		} break;
		case rofl::openflow13::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_EXPERIMENTER: {
				experimenter_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_ERROR: {
				error_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_FEATURES_REPLY: {
				features_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_GET_CONFIG_REPLY: {
				get_config_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_PACKET_IN: {
				packet_in_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_FLOW_REMOVED: {
				flow_removed_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_PORT_STATUS: {
				port_status_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_MULTIPART_REPLY: {
				multipart_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_BARRIER_REPLY: {
				barrier_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REPLY: {
				queue_get_config_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_ROLE_REPLY: {
				role_reply_rcvd(msg, aux_id);
			} break;
			case rofl::openflow13::OFPT_GET_ASYNC_REPLY: {
				get_async_config_reply_rcvd(msg, aux_id);
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

uint32_t
crofdpt_impl::get_async_xid(rofl::openflow::crofchan *chan)
{
	return transactions.get_async_xid();
}

uint32_t
crofdpt_impl::get_sync_xid(rofl::openflow::crofchan *chan, uint8_t msg_type, uint16_t msg_sub_type)
{
	return transactions.add_ta(cclock(/*secs=*/5), msg_type, msg_sub_type);
}

void
crofdpt_impl::release_sync_xid(rofl::openflow::crofchan *chan, uint32_t xid)
{
	return transactions.drop_ta(xid);
}


void
crofdpt_impl::ta_expired(
		rofl::openflow::ctransactions& tas,
		rofl::openflow::ctransaction& ta)
{
	logging::warn << "[rofl][dpt] transaction expired, xid:" << std::endl << ta;

	switch (ta.get_msg_type()) {
	case OFPT_FEATURES_REQUEST: {
		run_engine(EVENT_FEATURES_REQUEST_EXPIRED);
		rofbase->handle_features_reply_timeout(*this, ta.get_xid());
	} break;
	case OFPT_GET_CONFIG_REQUEST: {
		run_engine(EVENT_GET_CONFIG_REQUEST_EXPIRED);
		rofbase->handle_get_config_reply_timeout(*this, ta.get_xid());
	} break;
	case OFPT_MULTIPART_REQUEST: {
		switch (ta.get_msg_sub_type()) {
		case OFPMP_DESC: {

		} break;
		case OFPMP_FLOW: {

		} break;
		case OFPMP_AGGREGATE: {

		} break;
		case OFPMP_TABLE: {
			run_engine(EVENT_TABLE_STATS_REQUEST_EXPIRED);
		} break;
		case OFPMP_PORT_STATS: {

		} break;
		case OFPMP_QUEUE: {

		} break;
		case OFPMP_GROUP: {

		} break;
		case OFPMP_GROUP_DESC: {

		} break;
		case OFPMP_GROUP_FEATURES: {

		} break;
		case OFPMP_METER: {

		} break;
		case OFPMP_METER_CONFIG: {

		} break;
		case OFPMP_METER_FEATURES: {

		} break;
		case OFPMP_TABLE_FEATURES: {
			run_engine(EVENT_TABLE_FEATURES_STATS_REQUEST_EXPIRED);
		} break;
		case OFPMP_PORT_DESC: {
			run_engine(EVENT_PORT_DESC_STATS_REQUEST_EXPIRED);
		} break;
		case OFPMP_EXPERIMENTER: {

		} break;
		default: {

		};
		}
		rofbase->handle_multipart_reply_timeout(*this, ta.get_xid(), ta.get_msg_sub_type());
	} break;
	case OFPT_BARRIER_REQUEST: {
		rofbase->handle_barrier_reply_timeout(*this, ta.get_xid());
	} break;
	case OFPT_QUEUE_GET_CONFIG_REQUEST: {
		rofbase->handle_queue_get_config_reply_timeout(*this, ta.get_xid());
	} break;
	case OFPT_ROLE_REQUEST: {
		rofbase->handle_role_reply_timeout(*this, ta.get_xid());
	} break;
	case OFPT_GET_ASYNC_REQUEST: {
		rofbase->handle_get_async_config_reply_timeout(*this, ta.get_xid());
	} break;
	case OFPT_EXPERIMENTER: {
		rofbase->handle_experimenter_timeout(*this, ta.get_xid());
	} break;
	default: {

	};
	}
}



void
crofdpt_impl::handle_timeout(int opaque, void *data)
{
	switch (opaque) {
	default: {
		logging::error << "[rofl][dpt] dpid:0x"
				<< std::hex << dpid << std::dec
				<< " unknown timer event:" << opaque << std::endl;
	};
	}
}



void
crofdpt_impl::flow_mod_reset()
{
	cofflowmod fe(rofchan.get_version());
	switch (rofchan.get_version()) {
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

	send_flow_mod_message(fe);
}



void
crofdpt_impl::group_mod_reset()
{
	cofgroupmod ge(rofchan.get_version());
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

	send_group_mod_message(ge);
}



uint32_t
crofdpt_impl::send_features_request()
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Features-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_FEATURES_REQUEST);

	cofmsg_features_request *msg =
			new cofmsg_features_request(rofchan.get_version(), xid);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_get_config_request()
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Get-Config-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_GET_CONFIG_REQUEST);

	cofmsg_get_config_request *msg =
			new cofmsg_get_config_request(rofchan.get_version(), xid);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_table_features_stats_request(
		uint16_t stats_flags)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Table-Features-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_TABLE_FEATURES);

	throw eNotImplemented("crofdpt_impl::send_table_features_stats_request()");
#if 0
	cofmsg_get_config_request *msg =
			new cofmsg_get_config_request(rofchan.get_version(), xid);

	rofchan.send_message(msg, 0);
#endif

	return xid;
}



uint32_t
crofdpt_impl::send_stats_request(
	uint16_t stats_type,
	uint16_t stats_flags,
	uint8_t* body,
	size_t bodylen)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST);

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
crofdpt_impl::send_desc_stats_request(
		uint16_t flags)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Desc-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_DESC);

	cofmsg_desc_stats_request *msg =
			new cofmsg_desc_stats_request(
					rofchan.get_version(),
					xid,
					flags);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_flow_stats_request(
		uint16_t flags,
		cofflow_stats_request const& flow_stats_request)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Flow-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_FLOW);

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
crofdpt_impl::send_aggr_stats_request(
		uint16_t flags,
		cofaggr_stats_request const& aggr_stats_request)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Aggr-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_AGGREGATE);

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
crofdpt_impl::send_table_stats_request(
		uint16_t flags)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Table-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_TABLE);

	cofmsg_table_stats_request *msg =
			new cofmsg_table_stats_request(
					rofchan.get_version(),
					xid,
					flags);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_port_stats_request(
		uint16_t flags,
		cofport_stats_request const& port_stats_request)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Port-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_PORT_STATS);

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
crofdpt_impl::send_queue_stats_request(
	uint16_t flags,
	cofqueue_stats_request const& queue_stats_request)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Queue-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_QUEUE);

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
crofdpt_impl::send_group_stats_request(
	uint16_t flags,
	cofgroup_stats_request const& group_stats_request)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Group-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_GROUP);

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
crofdpt_impl::send_group_desc_stats_request(
		uint16_t flags)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Group-Desc-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_GROUP_DESC);

	cofmsg_group_desc_stats_request *msg =
			new cofmsg_group_desc_stats_request(
					rofchan.get_version(),
					xid,
					flags);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_group_features_stats_request(
		uint16_t flags)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Group-Features-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_GROUP_FEATURES);

	cofmsg_group_features_stats_request *msg =
			new cofmsg_group_features_stats_request(
					rofchan.get_version(),
					xid,
					flags);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_port_desc_stats_request(
		uint16_t flags)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Port-Desc-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_PORT_DESC);

	cofmsg_port_desc_stats_request *msg =
			new cofmsg_port_desc_stats_request(
					rofchan.get_version(),
					xid,
					flags);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_experimenter_stats_request(
	uint16_t flags,
	uint32_t exp_id,
	uint32_t exp_type,
	cmemory const& body)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Experimenter-Stats-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_MULTIPART_REQUEST, OFPMP_EXPERIMENTER);

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
crofdpt_impl::send_packet_out_message(
	uint32_t buffer_id,
	uint32_t in_port,
	cofactions& aclist,
	uint8_t *data,
	size_t datalen)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Packet-Out message" << std::endl;
		throw eRofBaseNotConnected();
	}

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
crofdpt_impl::send_barrier_request()
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Barrier-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_BARRIER_REQUEST);

	cofmsg_barrier_request *msg =
			new cofmsg_barrier_request(
					rofchan.get_version(),
					xid);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_role_request(
	uint32_t role,
	uint64_t generation_id)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Role-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_ROLE_REQUEST);

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
crofdpt_impl::send_flow_mod_message(
		cofflowmod const& fe)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Flow-Mod message" << std::endl;
		throw eRofBaseNotConnected();
	}

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
crofdpt_impl::send_group_mod_message(
		cofgroupmod const& ge)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Group-Mod message" << std::endl;
		throw eRofBaseNotConnected();
	}

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
crofdpt_impl::send_table_mod_message(
		uint8_t table_id,
		uint32_t config)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Table-Mod message" << std::endl;
		throw eRofBaseNotConnected();
	}

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
crofdpt_impl::send_port_mod_message(
	uint32_t port_no,
	cmacaddr const& hwaddr,
	uint32_t config,
	uint32_t mask,
	uint32_t advertise)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Port-Mod message" << std::endl;
		throw eRofBaseNotConnected();
	}

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
crofdpt_impl::send_set_config_message(
	uint16_t flags,
	uint16_t miss_send_len)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Set-Config message" << std::endl;
		throw eRofBaseNotConnected();
	}

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
crofdpt_impl::send_queue_get_config_request(
	uint32_t port)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Get-Config-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_QUEUE_GET_CONFIG_REQUEST);

	cofmsg_queue_get_config_request *msg =
			new cofmsg_queue_get_config_request(
					rofchan.get_version(),
					xid,
					port);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_get_async_config_request()
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Get-Async-Config-Request message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_GET_ASYNC_REQUEST);

	cofmsg_get_async_config_request *msg =
			new cofmsg_get_async_config_request(
					rofchan.get_version(),
					xid);

	rofchan.send_message(msg, 0);

	return xid;
}



uint32_t
crofdpt_impl::send_set_async_config_message(
	uint32_t packet_in_mask0,
	uint32_t packet_in_mask1,
	uint32_t port_status_mask0,
	uint32_t port_status_mask1,
	uint32_t flow_removed_mask0,
	uint32_t flow_removed_mask1)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Set-Async-Config message" << std::endl;
		throw eRofBaseNotConnected();
	}

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
crofdpt_impl::send_error_message(
	uint32_t xid,
	uint16_t type,
	uint16_t code,
	uint8_t* data,
	size_t datalen)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Error message" << std::endl;
		throw eRofBaseNotConnected();
	}

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
crofdpt_impl::send_experimenter_message(
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	if (not is_established()) {
		logging::warn << "[rofl][dpt] not connected, dropping Experimenter message" << std::endl;
		throw eRofBaseNotConnected();
	}

	uint32_t xid = transactions.add_ta(cclock(/*sec=*/5), OFPT_EXPERIMENTER);

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
crofdpt_impl::features_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_features_reply *reply = dynamic_cast<cofmsg_features_reply*>( msg );
	assert(reply != NULL);

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Features-Reply message received" << std::endl;

	try {
		transactions.drop_ta(msg->get_xid());

		dpid 			= reply->get_dpid();
		n_buffers 		= reply->get_n_buffers();
		n_tables 		= reply->get_n_tables();
		capabilities 	= reply->get_capabilities();

		switch (rofchan.get_version()) {
		case openflow10::OFP_VERSION:
		case openflow12::OFP_VERSION: {
			ports = reply->get_ports();
		} break;
		default: {
			// no ports in OpenFlow 1.3 and beyond in a Features.request
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

		if (STATE_ESTABLISHED == state) {
			rofbase->handle_features_reply(*this, *reply);
		}
		delete msg;

		run_engine(EVENT_FEATURES_REPLY_RCVD);

	} catch (RoflException& e) {

		logging::error << "[rofl][dpt] eRoflException in Features.reply rcvd" << *msg << std::endl;
		run_engine(EVENT_DISCONNECTED);
	}
}



void
crofdpt_impl::get_config_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_get_config_reply *reply = dynamic_cast<cofmsg_get_config_reply*>( msg );
	assert(reply != NULL);

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Get-Config-Reply message received" << std::endl;

	transactions.drop_ta(msg->get_xid());

	config 			= reply->get_flags();
	miss_send_len 	= reply->get_miss_send_len();

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_get_config_reply(*this, *reply);
	}
	delete msg;

	run_engine(EVENT_GET_CONFIG_REPLY_RCVD);
}


void
crofdpt_impl::multipart_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Multipart-Reply message received" << std::endl << *msg;

	transactions.drop_ta(msg->get_xid());

	// TODO: defragmentation

	cofmsg_multipart_reply *reply = dynamic_cast<cofmsg_multipart_reply*>( msg );
	assert(reply != NULL);

	switch (reply->get_stats_type()) {
	case rofl::openflow13::OFPMP_DESC: {
		desc_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_FLOW: {
		flow_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_AGGREGATE: {
		aggregate_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_TABLE: {
		table_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_PORT_STATS: {
		port_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_QUEUE: {
		queue_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_GROUP: {
		group_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_GROUP_DESC: {
		group_desc_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_GROUP_FEATURES: {
		group_features_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_METER: {
		meter_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_METER_CONFIG: {
		meter_config_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_METER_FEATURES: {
		meter_features_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_TABLE_FEATURES: {
		table_features_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_PORT_DESC: {
		port_desc_stats_reply_rcvd(msg, aux_id);
	} break;
	case rofl::openflow13::OFPMP_EXPERIMENTER: {
		experimenter_stats_reply_rcvd(msg, aux_id);
	} break;
	default: {
		if (STATE_ESTABLISHED != state) {
			logging::warn << "[rofl][dpt] rcvd Multipart-Reply without being "
					"established, dropping message:" << std::endl << *reply;
			return;
		}
		rofbase->handle_stats_reply(*this, dynamic_cast<cofmsg_stats&>( *msg ));
	};
	}
}



void
crofdpt_impl::desc_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_desc_stats_reply& reply = dynamic_cast<cofmsg_desc_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Desc-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_desc_stats_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::table_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_table_stats_reply& reply = dynamic_cast<cofmsg_table_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Table-Stats-Reply message received" << std::endl;

	switch (msg->get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		// clear our old table map
		tables.clear();
		// iterate through all received table stats bodies and fill in our local tables map
		for (std::vector<coftable_stats_reply>::iterator
				it = reply.get_table_stats().begin(); it != reply.get_table_stats().end(); ++it) {
			coftable_stats_reply& table = (*it);
			tables[table.get_table_id()] = table;
		}

	} break;
	}

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_table_stats_reply(*this, reply, aux_id);
	} else {
		run_engine(EVENT_TABLE_STATS_REPLY_RCVD);
	}
	delete msg;
}



void
crofdpt_impl::port_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_port_stats_reply& reply = dynamic_cast<cofmsg_port_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Port-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_port_stats_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::flow_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_flow_stats_reply& reply = dynamic_cast<cofmsg_flow_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Flow-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_flow_stats_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::aggregate_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_aggr_stats_reply& reply = dynamic_cast<cofmsg_aggr_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Aggregate-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_aggregate_stats_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::queue_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_queue_stats_reply& reply = dynamic_cast<cofmsg_queue_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Queue-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_queue_stats_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::group_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_group_stats_reply& reply = dynamic_cast<cofmsg_group_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Group-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_group_stats_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::group_desc_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_group_desc_stats_reply& reply = dynamic_cast<cofmsg_group_desc_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Group-Desc-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_group_desc_stats_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::group_features_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_group_features_stats_reply& reply = dynamic_cast<cofmsg_group_features_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Group-Features-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_group_features_stats_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::meter_stats_reply_rcvd(
		cofmsg *msg, uint8_t aux_id)
{
	delete msg;
	throw eNotImplemented("crofdpt_impl::meter_stats_reply_rcvd()"); // TODO
}



void
crofdpt_impl::meter_config_stats_reply_rcvd(
		cofmsg *msg, uint8_t aux_id)
{
	delete msg;
	throw eNotImplemented("crofdpt_impl::meter_config_stats_reply_rcvd()"); // TODO
}



void
crofdpt_impl::meter_features_stats_reply_rcvd(
		cofmsg *msg, uint8_t aux_id)
{
	delete msg;
	throw eNotImplemented("crofdpt_impl::meter_features_stats_reply_rcvd()"); // TODO
}



void
crofdpt_impl::table_features_stats_reply_rcvd(
		cofmsg *msg, uint8_t aux_id)
{
	delete msg;
	throw eNotImplemented("crofdpt_impl::table_features_stats_reply_rcvd()"); // TODO
}



void
crofdpt_impl::port_desc_stats_reply_rcvd(
		cofmsg *msg, uint8_t aux_id)
{
	cofmsg_port_desc_stats_reply& reply = dynamic_cast<cofmsg_port_desc_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Port-Desc-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_port_desc_stats_reply(*this, reply, aux_id);
	}
	delete msg;

	run_engine(EVENT_PORT_DESC_STATS_REPLY_RCVD);
}



void
crofdpt_impl::experimenter_stats_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_experimenter_stats_reply& reply = dynamic_cast<cofmsg_experimenter_stats_reply&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Experimenter-Stats-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_experimenter_stats_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::barrier_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_barrier_reply& reply = dynamic_cast<cofmsg_barrier_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Barrier-Reply message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_barrier_reply(*this, reply, aux_id);
	}
	delete msg;
}




void
crofdpt_impl::flow_removed_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_flow_removed& flow_removed = dynamic_cast<cofmsg_flow_removed&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Flow-Removed message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_flow_removed(*this, flow_removed, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::packet_in_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_packet_in& packet_in = dynamic_cast<cofmsg_packet_in&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Packet-In message received" << std::endl;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_packet_in(*this, packet_in, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::port_status_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_port_status& port_status = dynamic_cast<cofmsg_port_status&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Port-Status message received" << std::endl;

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
		rofbase->handle_port_status(*this, port_status, aux_id);
	}
	delete msg;
}


void
crofdpt_impl::fsp_open(cofmatch const& ofmatch)
{
	cofmatch m(ofmatch);
	croflexp_flowspace rexp(croflexp::OFPRET_FSP_ADD, m);

	cmemory packed(rexp.length());

	rexp.pack(packed.somem(), packed.memlen());

	send_experimenter_message(
			OFPEXPID_ROFL,
			croflexp::OFPRET_FLOWSPACE,
			packed.somem(),
			packed.memlen());

}


void
crofdpt_impl::fsp_close(cofmatch const& ofmatch)
{
	cofmatch m(ofmatch);
	croflexp_flowspace rexp(croflexp::OFPRET_FSP_DELETE, m);

	cmemory packed(rexp.length());

	rexp.pack(packed.somem(), packed.memlen());

	send_experimenter_message(
			OFPEXPID_ROFL,
			croflexp::OFPRET_FLOWSPACE,
			packed.somem(),
			packed.memlen());

}



void
crofdpt_impl::experimenter_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_experimenter& exp = dynamic_cast<cofmsg_experimenter&>( *msg );

	transactions.drop_ta(msg->get_xid());

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Experimenter message received" << std::endl << exp;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_experimenter_message(*this, exp, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::error_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_error& error = dynamic_cast<cofmsg_error&>( *msg );

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Error message received" << std::endl << error;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_error_message(*this, error, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::role_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_role_reply& reply = dynamic_cast<cofmsg_role_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Role-Reply message received" << std::endl << reply;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_role_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::queue_get_config_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_queue_get_config_reply& reply = dynamic_cast<cofmsg_queue_get_config_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " Queue-Get-Config-Reply message received" << std::endl << reply;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_queue_get_config_reply(*this, reply, aux_id);
	}
	delete msg;
}



void
crofdpt_impl::get_async_config_reply_rcvd(
		cofmsg *msg,
		uint8_t aux_id)
{
	cofmsg_get_async_config_reply& reply = dynamic_cast<cofmsg_get_async_config_reply&>( *msg );

	transactions.drop_ta(msg->get_xid());

	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< "Get-Async-Config-Reply message received" << std::endl << reply;

	if (STATE_ESTABLISHED == state) {
		rofbase->handle_get_async_config_reply(*this, reply, aux_id);
	}
	delete msg;
}




