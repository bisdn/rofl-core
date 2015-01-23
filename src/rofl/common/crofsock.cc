/*
 * cchannel.cpp
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "crofsock.h"

using namespace rofl;

crofsock::crofsock(
		crofsock_env *env,
		pthread_t tid) :
				ciosrv(tid),
				env(env),
				socket(NULL),
				state(STATE_INIT),
				fragment((cmemory*)0),
				msg_bytes_read(0),
				max_pkts_rcvd_per_round(DEFAULT_MAX_PKTS_RVCD_PER_ROUND),
				txqueues(QUEUE_MAX, crofqueue()),
				txweights(QUEUE_MAX, 1),
				socket_type(rofl::csocket::SOCKET_TYPE_UNKNOWN),
				sd(-1)
{
	// scheduler weights for transmission
	txweights[QUEUE_OAM ] = 4;
	txweights[QUEUE_MGMT] = 8;
	txweights[QUEUE_FLOW] = 4;
	txweights[QUEUE_PKT ] = 2;
	rofl::logging::debug2 << "[rofl-common][crofsock] "
			<< "constructor " << std::hex << this << std::dec
			<< ", target tid: " << std::hex << get_thread_id() << std::dec
			<< ", running tid: " << std::hex << pthread_self() << std::dec
			<< std::endl;
}



crofsock::~crofsock()
{
	rofl::logging::debug2 << "[rofl-common][crofsock] "
			<< "destructor " << std::hex << this << std::dec
			<< ", target tid: " << std::hex << get_thread_id() << std::dec
			<< ", running tid: " << std::hex << pthread_self() << std::dec
			<< std::endl;
	if (fragment)
		delete fragment;
	if (socket)
		delete socket;
}



unsigned int
crofsock::send_message(
		rofl::openflow::cofmsg *msg)
{
	unsigned int cwnd_size = 1;

	if (NULL == msg) {
		return cwnd_size;
	}

	if (not socket->is_established()) {
		delete msg; return 0;
	}

	log_message(std::string("queueing message for sending:"), *msg);

	switch (msg->get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		switch (msg->get_type()) {
		case rofl::openflow10::OFPT_PACKET_IN:
		case rofl::openflow10::OFPT_PACKET_OUT: {
			txqueues[QUEUE_PKT].store(msg);
		} break;
		case rofl::openflow10::OFPT_FLOW_MOD:
		case rofl::openflow10::OFPT_FLOW_REMOVED: {
			txqueues[QUEUE_FLOW].store(msg);
		} break;
		case rofl::openflow10::OFPT_ECHO_REQUEST:
		case rofl::openflow10::OFPT_ECHO_REPLY: {
			txqueues[QUEUE_OAM].store(msg);
		} break;
		default: {
			txqueues[QUEUE_MGMT].store(msg);
		};
		}
	} break;
	case rofl::openflow12::OFP_VERSION: {
		switch (msg->get_type()) {
		case rofl::openflow12::OFPT_PACKET_IN:
		case rofl::openflow12::OFPT_PACKET_OUT: {
			txqueues[QUEUE_PKT].store(msg);
		} break;
		case rofl::openflow12::OFPT_FLOW_MOD:
		case rofl::openflow12::OFPT_FLOW_REMOVED:
		case rofl::openflow12::OFPT_GROUP_MOD:
		case rofl::openflow12::OFPT_PORT_MOD:
		case rofl::openflow12::OFPT_TABLE_MOD: {
			txqueues[QUEUE_FLOW].store(msg);
		} break;
		case rofl::openflow12::OFPT_ECHO_REQUEST:
		case rofl::openflow12::OFPT_ECHO_REPLY: {
			txqueues[QUEUE_OAM].store(msg);
		} break;
		default: {
			txqueues[QUEUE_MGMT].store(msg);
		};
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {
		switch (msg->get_type()) {
		case rofl::openflow13::OFPT_PACKET_IN:
		case rofl::openflow13::OFPT_PACKET_OUT: {
			txqueues[QUEUE_PKT].store(msg);
		} break;
		case rofl::openflow13::OFPT_FLOW_MOD:
		case rofl::openflow13::OFPT_FLOW_REMOVED:
		case rofl::openflow13::OFPT_GROUP_MOD:
		case rofl::openflow13::OFPT_PORT_MOD:
		case rofl::openflow13::OFPT_TABLE_MOD: {
			txqueues[QUEUE_FLOW].store(msg);
		} break;
		case rofl::openflow13::OFPT_ECHO_REQUEST:
		case rofl::openflow13::OFPT_ECHO_REPLY: {
			txqueues[QUEUE_OAM].store(msg);
		} break;
		default: {
			txqueues[QUEUE_MGMT].store(msg);
		};
		}
	} break;
	default: {
		rofl::logging::alert << "[rofl-common][crofsock] dropping message with unsupported OpenFlow version" << std::endl;
		delete msg; return 0;
	};
	}

	rofl::ciosrv::notify(EVENT_CONGESTION_SOLVED);

	if (flags.test(FLAGS_CONGESTED)) {
		cwnd_size = 0;
	}
	return cwnd_size;
}



void
crofsock::send_from_queue()
{
	bool reschedule = false;

	for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; ++queue_id) {

		for (unsigned int num = 0; num < txweights[queue_id]; ++num) {

			cmemory *mem = (cmemory*)0;

			try {
				rofl::openflow::cofmsg *msg = txqueues[queue_id].front();
				if (NULL == msg)
					break;

				mem = new cmemory(msg->length());
				msg->pack(mem->somem(), mem->memlen());

				rofl::logging::debug2 << "[rofl-common][crofsock][send-from-queue] msg:"
						<< std::endl << *msg;

				rofl::logging::debug2 << "[rofl-common][crofsock][send-from-queue] mem:"
						<< std::endl << *mem;

				socket->send(mem); // may throw exception

				txqueues[queue_id].pop();
				delete msg;


			} catch (eSocketTxAgain& e) {
				rofl::logging::error << "[rofl-common][crofsock][send-from-queue] transport "
						<< "connection congested, waiting." << std::endl;

				flags.set(FLAGS_CONGESTED);
			}
		}

		if (not txqueues[queue_id].empty()) {
			reschedule = true;
		}
	}

	if (flags.test(FLAGS_CONGESTED)) {
		env->handle_write(*this);
	}

	if (reschedule && not flags.test(FLAGS_CONGESTED)) {
		rofl::ciosrv::notify(EVENT_CONGESTION_SOLVED);
	}
}



void
crofsock::handle_event(
		cevent const &ev)
{
	switch (ev.cmd) {
	case EVENT_NONE: {

	} break;
	case EVENT_CONGESTION_SOLVED: {
		send_from_queue();
	} break;
	default:
		rofl::logging::debug3 << "[rofl-common][crofsock] unknown event type:" << (int)ev.cmd << std::endl;
	}
}




void
crofsock::handle_read(
		csocket& socket)
{
	if (STATE_CLOSED == state) {
		return;
	}
	unsigned int pkts_rcvd_in_round = 0;

	try {

		while (true) {

			if (0 == fragment) {
				fragment = new cmemory(sizeof(struct openflow::ofp_header));
				msg_bytes_read = 0;
			}

			uint16_t msg_len = 0;

			// how many bytes do we have to read?
			if (msg_bytes_read < sizeof(struct openflow::ofp_header)) {
				msg_len = sizeof(struct openflow::ofp_header);
			} else {
				struct openflow::ofp_header *header = (struct openflow::ofp_header*)fragment->somem();
				msg_len = be16toh(header->length);
			}

			// sanity check: 8 <= msg_len <= 2^16
			if (msg_len < sizeof(struct openflow::ofp_header)) {
				rofl::logging::warn << "[rofl-common][crofsock] received message with invalid length field, closing socket." << std::endl;
				socket.close();
				return;
			}

			// resize msg buffer, if necessary
			if (fragment->memlen() < msg_len) {
				fragment->resize(msg_len);
			}

			// read from socket more bytes, at most "msg_len - msg_bytes_read"
			int rc = socket.recv((void*)(fragment->somem() + msg_bytes_read), msg_len - msg_bytes_read);

			msg_bytes_read += rc;

			// minimum message length received, check completeness of message
			if (fragment->memlen() >= sizeof(struct openflow::ofp_header)) {
				struct openflow::ofp_header *header =
						(struct openflow::ofp_header*)fragment->somem();
				uint16_t msg_len = be16toh(header->length);

				// ok, message was received completely
				if (msg_len == msg_bytes_read) {
					cmemory *mem = fragment;
					fragment = (cmemory*)0; // just in case, we get an exception from parse_message()
					msg_bytes_read = 0;

					parse_message(mem);

					pkts_rcvd_in_round++;
					// read at most max_pkts_rcvd_per_round (default: 16) packets from socket, reschedule afterwards
					if (pkts_rcvd_in_round >= max_pkts_rcvd_per_round) {
						rofl::logging::debug2 << "[rofl-common][crofsock] "
								<< "received " << pkts_rcvd_in_round
								<< " packet(s) from peer, rescheduling." << std::endl;
						return;
					}
				}
			}
		}

	} catch (eSocketRxAgain& e) {

		// more bytes are needed, keep pointer to msg in "fragment"
		rofl::logging::debug2 << "[rofl-common][crofsock] eSocketRxAgain: "
				<< "no further data available on socket, read "
				<< pkts_rcvd_in_round << " packet(s) in this round." << std::endl;

	} catch (eSysCall& e) {

		rofl::logging::warn << "[rofl-common][crofsock] "
				<< "eSysCall: failed to read from socket: " << e.what() << std::endl;

		// close socket, as it seems, we are out of sync
		run_engine(EVENT_PEER_DISCONNECTED);

	} catch (RoflException& e) {

		rofl::logging::warn << "[rofl-common][crofsock] "
				<< "generic ROFL exception: " << e.what() << std::endl;

		// close socket, as it seems, we are out of sync
		run_engine(EVENT_PEER_DISCONNECTED);
	}

}



void
crofsock::parse_message(
		cmemory *mem)
{
	if (mem == NULL) {
		return;
	}

	if (STATE_CLOSED == state) {
		delete mem; return;
	}

	rofl::openflow::cofmsg *msg = (rofl::openflow::cofmsg*)0;
	try {
		struct openflow::ofp_header* header =
				(struct openflow::ofp_header*)mem->somem();

		/* make sure to have a valid cofmsg* msg object after parsing */
		switch (header->version) {
		case rofl::openflow10::OFP_VERSION: {
			parse_of10_message(mem, &msg);
		} break;
		case rofl::openflow12::OFP_VERSION: {
			parse_of12_message(mem, &msg);
		} break;
		case rofl::openflow13::OFP_VERSION: {
			parse_of13_message(mem, &msg);
		} break;
		default: {
			msg = new rofl::openflow::cofmsg(mem);
		};
		}

		if (msg == NULL) {
			return;
		}

		//log_message(std::string("received message:"), *msg);

		if (env) {
			env->recv_message(*this, msg);
		}

	} catch (eBadRequestBadType& e) {

		if (msg) {
			rofl::logging::error << "[rofl-common][crofsock] eBadRequestBadType: " << std::endl << *msg;
			size_t len = (msg->framelen() > 64) ? 64 : msg->framelen();
			rofl::openflow::cofmsg_error_bad_request_bad_type *error =
					new rofl::openflow::cofmsg_error_bad_request_bad_type(
							msg->get_version(),
							msg->get_xid(),
							msg->soframe(),
							len);
			send_message(error);
			delete msg;
		} else {
			rofl::logging::error << "[rofl-common][crofsock] eBadRequestBadType " << std::endl;
		}

	} catch (RoflException& e) {

		if (msg) {
			rofl::logging::error << "[rofl-common][crofsock] RoflException: " << std::endl << *msg;
			delete msg;
		} else {
			rofl::logging::error << "[rofl-common][crofsock] RoflException " << std::endl;
		}

	}
}



void
crofsock::parse_of10_message(cmemory *mem, rofl::openflow::cofmsg **pmsg)
{
	struct openflow::ofp_header* header = (struct openflow::ofp_header*)mem->somem();

	switch (header->type) {
	case rofl::openflow10::OFPT_HELLO: {
		(*pmsg = new rofl::openflow::cofmsg_hello(mem))->validate();
	} break;

	case rofl::openflow10::OFPT_ERROR: {
		(*pmsg = new rofl::openflow::cofmsg_error(mem))->validate();
	} break;

	case rofl::openflow10::OFPT_ECHO_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_echo_request(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_ECHO_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_echo_reply(mem))->validate();
	} break;

	case rofl::openflow10::OFPT_VENDOR: {
		(*pmsg = new rofl::openflow::cofmsg_experimenter(mem))->validate();
	} break;

	case rofl::openflow10::OFPT_FEATURES_REQUEST:	{
		(*pmsg = new rofl::openflow::cofmsg_features_request(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_FEATURES_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_features_reply(mem))->validate();
	} break;

	case rofl::openflow10::OFPT_GET_CONFIG_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_get_config_request(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_GET_CONFIG_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_get_config_reply(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_SET_CONFIG: {
		(*pmsg = new rofl::openflow::cofmsg_set_config(mem))->validate();
	} break;

	case rofl::openflow10::OFPT_PACKET_OUT: {
		(*pmsg = new rofl::openflow::cofmsg_packet_out(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_PACKET_IN: {
		(*pmsg = new rofl::openflow::cofmsg_packet_in(mem))->validate();
	} break;

	case rofl::openflow10::OFPT_FLOW_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_flow_mod(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_FLOW_REMOVED: {
		(*pmsg = new rofl::openflow::cofmsg_flow_removed(mem))->validate();
	} break;

	case rofl::openflow10::OFPT_PORT_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_port_mod(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_PORT_STATUS: {
		(*pmsg = new rofl::openflow::cofmsg_port_status(mem))->validate();
	} break;

	case rofl::openflow10::OFPT_STATS_REQUEST: {
		if (mem->memlen() < sizeof(struct rofl::openflow10::ofp_stats_request)) {
			*pmsg = new rofl::openflow::cofmsg(mem);
			throw eBadSyntaxTooShort();
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow10::ofp_stats_request*)mem->somem())->type);

		switch (stats_type) {
		case rofl::openflow10::OFPST_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_desc_stats_request(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_FLOW: {
			(*pmsg = new rofl::openflow::cofmsg_flow_stats_request(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_AGGREGATE: {
			(*pmsg = new rofl::openflow::cofmsg_aggr_stats_request(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_TABLE: {
			(*pmsg = new rofl::openflow::cofmsg_table_stats_request(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_PORT: {
			(*pmsg = new rofl::openflow::cofmsg_port_stats_request(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_QUEUE: {
			(*pmsg = new rofl::openflow::cofmsg_queue_stats_request(mem))->validate();
		} break;
		// TODO: experimenter statistics
		default: {
			(*pmsg = new rofl::openflow::cofmsg_stats_request(mem))->validate();
		} break;
		}

	} break;
	case rofl::openflow10::OFPT_STATS_REPLY: {
		if (mem->memlen() < sizeof(struct rofl::openflow10::ofp_stats_reply)) {
			*pmsg = new rofl::openflow::cofmsg(mem);
			throw eBadSyntaxTooShort();
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow10::ofp_stats_reply*)mem->somem())->type);

		switch (stats_type) {
		case rofl::openflow10::OFPST_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_desc_stats_reply(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_FLOW: {
			(*pmsg = new rofl::openflow::cofmsg_flow_stats_reply(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_AGGREGATE: {
			(*pmsg = new rofl::openflow::cofmsg_aggr_stats_reply(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_TABLE: {
			(*pmsg = new rofl::openflow::cofmsg_table_stats_reply(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_PORT: {
			(*pmsg = new rofl::openflow::cofmsg_port_stats_reply(mem))->validate();
		} break;
		case rofl::openflow10::OFPST_QUEUE: {
			(*pmsg = new rofl::openflow::cofmsg_queue_stats_reply(mem))->validate();
		} break;
		// TODO: experimenter statistics
		default: {
			(*pmsg = new rofl::openflow::cofmsg_stats_reply(mem))->validate();
		} break;
		}

	} break;

	case rofl::openflow10::OFPT_BARRIER_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_barrier_request(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_BARRIER_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_barrier_reply(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_queue_get_config_request(mem))->validate();
	} break;
	case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_queue_get_config_reply(mem))->validate();
	} break;

	default: {
		(*pmsg = new rofl::openflow::cofmsg(mem))->validate();
		rofl::logging::warn << "[rofl-common][crofsock] dropping unknown message " << **pmsg << std::endl;
		throw eBadRequestBadType();
	} break;
	}
}



void
crofsock::parse_of12_message(cmemory *mem, rofl::openflow::cofmsg **pmsg)
{
	struct openflow::ofp_header* header = (struct openflow::ofp_header*)mem->somem();

	switch (header->type) {
	case rofl::openflow12::OFPT_HELLO: {
		(*pmsg = new rofl::openflow::cofmsg_hello(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_ERROR: {
		(*pmsg = new rofl::openflow::cofmsg_error(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_ECHO_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_echo_request(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_ECHO_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_echo_reply(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_EXPERIMENTER:	{
		(*pmsg = new rofl::openflow::cofmsg_experimenter(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_FEATURES_REQUEST:	{
		(*pmsg = new rofl::openflow::cofmsg_features_request(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_FEATURES_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_features_reply(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_GET_CONFIG_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_get_config_request(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_GET_CONFIG_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_get_config_reply(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_SET_CONFIG: {
		(*pmsg = new rofl::openflow::cofmsg_set_config(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_PACKET_OUT: {
		(*pmsg = new rofl::openflow::cofmsg_packet_out(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_PACKET_IN: {
		(*pmsg = new rofl::openflow::cofmsg_packet_in(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_FLOW_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_flow_mod(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_FLOW_REMOVED: {
		(*pmsg = new rofl::openflow::cofmsg_flow_removed(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_GROUP_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_group_mod(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_PORT_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_port_mod(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_PORT_STATUS: {
		(*pmsg = new rofl::openflow::cofmsg_port_status(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_TABLE_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_table_mod(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_STATS_REQUEST: {

		if (mem->memlen() < sizeof(struct rofl::openflow12::ofp_stats_request)) {
			*pmsg = new rofl::openflow::cofmsg(mem);
			throw eBadSyntaxTooShort();
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow12::ofp_stats_request*)mem->somem())->type);

		switch (stats_type) {
		case rofl::openflow12::OFPST_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_desc_stats_request(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_FLOW: {
			(*pmsg = new rofl::openflow::cofmsg_flow_stats_request(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_AGGREGATE: {
			(*pmsg = new rofl::openflow::cofmsg_aggr_stats_request(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_TABLE: {
			(*pmsg = new rofl::openflow::cofmsg_table_stats_request(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_PORT: {
			(*pmsg = new rofl::openflow::cofmsg_port_stats_request(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_QUEUE: {
			(*pmsg = new rofl::openflow::cofmsg_queue_stats_request(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_GROUP: {
			(*pmsg = new rofl::openflow::cofmsg_group_stats_request(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_GROUP_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_group_desc_stats_request(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_GROUP_FEATURES: {
			(*pmsg = new rofl::openflow::cofmsg_group_features_stats_request(mem))->validate();
		} break;
		// TODO: experimenter statistics
		default: {
			(*pmsg = new rofl::openflow::cofmsg_stats_request(mem))->validate();
		} break;
		}

	} break;
	case rofl::openflow12::OFPT_STATS_REPLY: {
		if (mem->memlen() < sizeof(struct rofl::openflow12::ofp_stats_reply)) {
			*pmsg = new rofl::openflow::cofmsg(mem);
			throw eBadSyntaxTooShort();
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow12::ofp_stats_reply*)mem->somem())->type);

		switch (stats_type) {
		case rofl::openflow12::OFPST_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_desc_stats_reply(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_FLOW: {
			(*pmsg = new rofl::openflow::cofmsg_flow_stats_reply(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_AGGREGATE: {
			(*pmsg = new rofl::openflow::cofmsg_aggr_stats_reply(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_TABLE: {
			(*pmsg = new rofl::openflow::cofmsg_table_stats_reply(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_PORT: {
			(*pmsg = new rofl::openflow::cofmsg_port_stats_reply(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_QUEUE: {
			(*pmsg = new rofl::openflow::cofmsg_queue_stats_reply(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_GROUP: {
			(*pmsg = new rofl::openflow::cofmsg_group_stats_reply(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_GROUP_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_group_desc_stats_reply(mem))->validate();
		} break;
		case rofl::openflow12::OFPST_GROUP_FEATURES: {
			(*pmsg = new rofl::openflow::cofmsg_group_features_stats_reply(mem))->validate();
		} break;
		// TODO: experimenter statistics
		default: {
			(*pmsg = new rofl::openflow::cofmsg_stats_reply(mem))->validate();
		} break;
		}

	} break;

	case rofl::openflow12::OFPT_BARRIER_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_barrier_request(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_BARRIER_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_barrier_reply(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_queue_get_config_request(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_queue_get_config_reply(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_ROLE_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_role_request(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_ROLE_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_role_reply(mem))->validate();
	} break;

	case rofl::openflow12::OFPT_GET_ASYNC_REQUEST: {
    	(*pmsg = new rofl::openflow::cofmsg_get_async_config_request(mem))->validate();
    } break;
	case rofl::openflow12::OFPT_GET_ASYNC_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_get_async_config_reply(mem))->validate();
	} break;
	case rofl::openflow12::OFPT_SET_ASYNC: {
    	(*pmsg = new rofl::openflow::cofmsg_set_async_config(mem))->validate();
    } break;

	default: {
		(*pmsg = new rofl::openflow::cofmsg(mem))->validate();
		rofl::logging::warn << "[rofl-common][crofsock] dropping unknown message " << **pmsg << std::endl;
		throw eBadRequestBadType();
	} return;
	}
}



void
crofsock::parse_of13_message(cmemory *mem, rofl::openflow::cofmsg **pmsg)
{
	struct openflow::ofp_header* header = (struct openflow::ofp_header*)mem->somem();

	switch (header->type) {
	case rofl::openflow13::OFPT_HELLO: {
		(*pmsg = new rofl::openflow::cofmsg_hello(mem))->validate();
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_hello&>( **pmsg );
	} break;

	case rofl::openflow13::OFPT_ERROR: {
		(*pmsg = new rofl::openflow::cofmsg_error(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_ECHO_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_echo_request(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_ECHO_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_echo_reply(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_EXPERIMENTER:	{
		(*pmsg = new rofl::openflow::cofmsg_experimenter(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_FEATURES_REQUEST:	{
		(*pmsg = new rofl::openflow::cofmsg_features_request(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_FEATURES_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_features_reply(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_GET_CONFIG_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_get_config_request(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_GET_CONFIG_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_get_config_reply(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_SET_CONFIG: {
		(*pmsg = new rofl::openflow::cofmsg_set_config(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_PACKET_OUT: {
		(*pmsg = new rofl::openflow::cofmsg_packet_out(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_PACKET_IN: {
		(*pmsg = new rofl::openflow::cofmsg_packet_in(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_FLOW_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_flow_mod(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_FLOW_REMOVED: {
		(*pmsg = new rofl::openflow::cofmsg_flow_removed(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_GROUP_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_group_mod(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_PORT_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_port_mod(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_PORT_STATUS: {
		(*pmsg = new rofl::openflow::cofmsg_port_status(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_TABLE_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_table_mod(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_MULTIPART_REQUEST: {

		if (mem->memlen() < sizeof(struct rofl::openflow13::ofp_multipart_request)) {
			*pmsg = new rofl::openflow::cofmsg(mem);
			throw eBadSyntaxTooShort();
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow13::ofp_multipart_request*)mem->somem())->type);

		switch (stats_type) {
		case rofl::openflow13::OFPMP_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_desc_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			(*pmsg = new rofl::openflow::cofmsg_flow_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			(*pmsg = new rofl::openflow::cofmsg_aggr_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			(*pmsg = new rofl::openflow::cofmsg_table_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			(*pmsg = new rofl::openflow::cofmsg_port_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			(*pmsg = new rofl::openflow::cofmsg_queue_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			(*pmsg = new rofl::openflow::cofmsg_group_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_group_desc_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			(*pmsg = new rofl::openflow::cofmsg_group_features_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_METER: {
			(*pmsg = new rofl::openflow::cofmsg_meter_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_METER_CONFIG: {
			(*pmsg = new rofl::openflow::cofmsg_meter_config_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_METER_FEATURES: {
			(*pmsg = new rofl::openflow::cofmsg_meter_features_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			(*pmsg = new rofl::openflow::cofmsg_table_features_stats_request(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_port_desc_stats_request(mem))->validate();
		} break;
		// TODO: experimenter statistics
		default: {
			(*pmsg = new rofl::openflow::cofmsg_stats_request(mem))->validate();
		} break;
		}

	} break;
	case rofl::openflow13::OFPT_MULTIPART_REPLY: {
		if (mem->memlen() < sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			*pmsg = new rofl::openflow::cofmsg(mem);
			throw eBadSyntaxTooShort();
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow13::ofp_multipart_reply*)mem->somem())->type);

		switch (stats_type) {
		case rofl::openflow13::OFPMP_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_desc_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			(*pmsg = new rofl::openflow::cofmsg_flow_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			(*pmsg = new rofl::openflow::cofmsg_aggr_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			(*pmsg = new rofl::openflow::cofmsg_table_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			(*pmsg = new rofl::openflow::cofmsg_port_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			(*pmsg = new rofl::openflow::cofmsg_queue_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			(*pmsg = new rofl::openflow::cofmsg_group_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_group_desc_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			(*pmsg = new rofl::openflow::cofmsg_group_features_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_METER: {
			(*pmsg = new rofl::openflow::cofmsg_meter_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_METER_CONFIG: {
			(*pmsg = new rofl::openflow::cofmsg_meter_config_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_METER_FEATURES: {
			(*pmsg = new rofl::openflow::cofmsg_meter_features_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			(*pmsg = new rofl::openflow::cofmsg_table_features_stats_reply(mem))->validate();
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			(*pmsg = new rofl::openflow::cofmsg_port_desc_stats_reply(mem))->validate();
		} break;
		// TODO: experimenter statistics
		default: {
			(*pmsg = new rofl::openflow::cofmsg_stats_reply(mem))->validate();
		} break;
		}

	} break;

	case rofl::openflow13::OFPT_BARRIER_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_barrier_request(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_BARRIER_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_barrier_reply(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_queue_get_config_request(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_queue_get_config_reply(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_ROLE_REQUEST: {
		(*pmsg = new rofl::openflow::cofmsg_role_request(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_ROLE_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_role_reply(mem))->validate();
	} break;

	case rofl::openflow13::OFPT_GET_ASYNC_REQUEST: {
    	(*pmsg = new rofl::openflow::cofmsg_get_async_config_request(mem))->validate();
    } break;
	case rofl::openflow13::OFPT_GET_ASYNC_REPLY: {
		(*pmsg = new rofl::openflow::cofmsg_get_async_config_reply(mem))->validate();
	} break;
	case rofl::openflow13::OFPT_SET_ASYNC: {
    	(*pmsg = new rofl::openflow::cofmsg_set_async_config(mem))->validate();
    } break;
	case rofl::openflow13::OFPT_METER_MOD: {
		(*pmsg = new rofl::openflow::cofmsg_meter_mod(mem))->validate();
	} break;

	default: {
		(*pmsg = new rofl::openflow::cofmsg(mem))->validate();
		rofl::logging::warn << "[rofl-common][crofsock] dropping unknown message " << **pmsg << std::endl;
		throw eBadRequestBadType();
	} return;
	}
}








void
crofsock::log_message(
		std::string const& text, rofl::openflow::cofmsg const& msg)
{
	rofl::logging::debug2 << "[rofl-common][crofsock] " << text << std::endl;

	try {
	switch (msg.get_version()) {
	case rofl::openflow10::OFP_VERSION: log_of10_message(msg); break;
	case rofl::openflow12::OFP_VERSION: log_of12_message(msg); break;
	case rofl::openflow13::OFP_VERSION: log_of13_message(msg); break;
	default: rofl::logging::debug2 << "[rolf][crofsock] unknown OFP version found in msg" << std::endl << msg; break;
	}
	} catch (...) {
		rofl::logging::debug2 << "[rofl-common][crofsock] log-message" << std::endl;
	}
}






void
crofsock::log_of10_message(
		rofl::openflow::cofmsg const& msg)
{
	switch (msg.get_type()) {
	case rofl::openflow10::OFPT_HELLO: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_hello const&>( msg );
	} break;
	case rofl::openflow10::OFPT_ERROR: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_error const&>( msg );
	} break;
	case rofl::openflow10::OFPT_ECHO_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_echo_request const&>( msg );
	} break;
	case rofl::openflow10::OFPT_ECHO_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_echo_reply const&>( msg );
	} break;
	case rofl::openflow10::OFPT_VENDOR: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_experimenter const&>( msg );
	} break;
	case rofl::openflow10::OFPT_FEATURES_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_features_request const&>( msg );
	} break;
	case rofl::openflow10::OFPT_FEATURES_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_features_reply const&>( msg );
	} break;
	case rofl::openflow10::OFPT_GET_CONFIG_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_config_request const&>( msg );
	} break;
	case rofl::openflow10::OFPT_GET_CONFIG_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_config_reply const&>( msg );
	} break;
	case rofl::openflow10::OFPT_SET_CONFIG: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_set_config const&>( msg );
	} break;
	case rofl::openflow10::OFPT_PACKET_OUT: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_packet_out const&>( msg );
	} break;
	case rofl::openflow10::OFPT_PACKET_IN: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_packet_in const&>( msg );
	} break;
	case rofl::openflow10::OFPT_FLOW_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_mod const&>( msg );
	} break;
	case rofl::openflow10::OFPT_FLOW_REMOVED: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_removed const&>( msg );
	} break;
	case rofl::openflow10::OFPT_PORT_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_mod const&>( msg );
	} break;
	case rofl::openflow10::OFPT_PORT_STATUS: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_status const&>( msg );
	} break;
	case rofl::openflow10::OFPT_STATS_REQUEST: {
		rofl::openflow::cofmsg_stats_request const& stats = dynamic_cast<rofl::openflow::cofmsg_stats_request const&>( msg );
		switch (stats.get_stats_type()) {
		case rofl::openflow10::OFPST_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_desc_stats_request const&>( msg );
		} break;
		case rofl::openflow10::OFPST_FLOW: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_stats_request const&>( msg );
		} break;
		case rofl::openflow10::OFPST_AGGREGATE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request const&>( msg );
		} break;
		case rofl::openflow10::OFPST_TABLE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_stats_request const&>( msg );
		} break;
		case rofl::openflow10::OFPST_PORT: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_stats_request const&>( msg );
		} break;
		case rofl::openflow10::OFPST_QUEUE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_stats_request const&>( msg );
		} break;
		// TODO: experimenter statistics
		default:
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_stats_request const&>( msg ); break;
		}

	} break;
	case rofl::openflow10::OFPT_STATS_REPLY: {
		rofl::openflow::cofmsg_stats_reply const& stats = dynamic_cast<rofl::openflow::cofmsg_stats_reply const&>( msg );
		switch (stats.get_stats_type()) {
		case rofl::openflow10::OFPST_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_desc_stats_reply const&>( msg );
		} break;
		case rofl::openflow10::OFPST_FLOW: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply const&>( msg );
		} break;
		case rofl::openflow10::OFPST_AGGREGATE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_reply const&>( msg );
		} break;
		case rofl::openflow10::OFPST_TABLE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const&>( msg );
		} break;
		case rofl::openflow10::OFPST_PORT: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_stats_reply const&>( msg );
		} break;
		case rofl::openflow10::OFPST_QUEUE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply const&>( msg );
		} break;
		// TODO: experimenter statistics
		default: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_stats_reply const&>( msg );
		} break;
		}

	} break;
	case rofl::openflow10::OFPT_BARRIER_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_barrier_request const&>( msg );
	} break;
	case rofl::openflow10::OFPT_BARRIER_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_barrier_reply const&>( msg );
	} break;
	case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request const&>( msg );
	} break;
	case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_get_config_reply const&>( msg );
	} break;
	default: {
		rofl::logging::debug2 << "[rofl-common][crofsock]  unknown message " << msg << std::endl;
	} break;
	}
}



void
crofsock::log_of12_message(
		rofl::openflow::cofmsg const& msg)
{
	switch (msg.get_type()) {
	case rofl::openflow12::OFPT_HELLO: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_hello const&>( msg );
	} break;
	case rofl::openflow12::OFPT_ERROR: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_error const&>( msg );
	} break;
	case rofl::openflow12::OFPT_ECHO_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_echo_request const&>( msg );
	} break;
	case rofl::openflow12::OFPT_ECHO_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_echo_reply const&>( msg );
	} break;
	case rofl::openflow12::OFPT_EXPERIMENTER:	{
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_experimenter const&>( msg );
	} break;
	case rofl::openflow12::OFPT_FEATURES_REQUEST:	{
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_features_request const&>( msg );
	} break;
	case rofl::openflow12::OFPT_FEATURES_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_features_reply const&>( msg );
	} break;
	case rofl::openflow12::OFPT_GET_CONFIG_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_config_request const&>( msg );
	} break;
	case rofl::openflow12::OFPT_GET_CONFIG_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_config_reply const&>( msg );
	} break;
	case rofl::openflow12::OFPT_SET_CONFIG: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_set_config const&>( msg );
	} break;
	case rofl::openflow12::OFPT_PACKET_OUT: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_packet_out const&>( msg );
	} break;
	case rofl::openflow12::OFPT_PACKET_IN: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_packet_in const&>( msg );
	} break;
	case rofl::openflow12::OFPT_FLOW_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_mod const&>( msg );
	} break;
	case rofl::openflow12::OFPT_FLOW_REMOVED: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_removed const&>( msg );
	} break;
	case rofl::openflow12::OFPT_GROUP_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_mod const&>( msg );
	} break;
	case rofl::openflow12::OFPT_PORT_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_mod const&>( msg );
	} break;
	case rofl::openflow12::OFPT_PORT_STATUS: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_status const&>( msg );
	} break;
	case rofl::openflow12::OFPT_TABLE_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_mod const&>( msg );
	} break;
	case rofl::openflow12::OFPT_STATS_REQUEST: {
		rofl::openflow::cofmsg_stats_request const& stats = dynamic_cast<rofl::openflow::cofmsg_stats_request const&>( msg );
		switch (stats.get_stats_type()) {
		case rofl::openflow12::OFPST_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_desc_stats_request const&>( msg );
		} break;
		case rofl::openflow12::OFPST_FLOW: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_stats_request const&>( msg );
		} break;
		case rofl::openflow12::OFPST_AGGREGATE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request const&>( msg );
		} break;
		case rofl::openflow12::OFPST_TABLE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_stats_request const&>( msg );
		} break;
		case rofl::openflow12::OFPST_PORT: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_stats_request const&>( msg );
		} break;
		case rofl::openflow12::OFPST_QUEUE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_stats_request const&>( msg );
		} break;
		case rofl::openflow12::OFPST_GROUP: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_stats_request const&>( msg );
		} break;
		case rofl::openflow12::OFPST_GROUP_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request const&>( msg );
		} break;
		case rofl::openflow12::OFPST_GROUP_FEATURES: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request const&>( msg );
		} break;
		// TODO: experimenter statistics
		default: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_stats_request const&>( msg );
		} break;
		}

	} break;
	case rofl::openflow12::OFPT_STATS_REPLY: {
		rofl::openflow::cofmsg_stats_reply const& stats = dynamic_cast<rofl::openflow::cofmsg_stats_reply const&>( msg );
		switch (stats.get_stats_type()) {
		case rofl::openflow12::OFPST_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_desc_stats_reply const&>( msg );
		} break;
		case rofl::openflow12::OFPST_FLOW: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply const&>( msg );
		} break;
		case rofl::openflow12::OFPST_AGGREGATE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_reply const&>( msg );
		} break;
		case rofl::openflow12::OFPST_TABLE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const&>( msg );
		} break;
		case rofl::openflow12::OFPST_PORT: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_stats_reply const&>( msg );
		} break;
		case rofl::openflow12::OFPST_QUEUE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply const&>( msg );
		} break;
		case rofl::openflow12::OFPST_GROUP: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_stats_reply const&>( msg );
		} break;
		case rofl::openflow12::OFPST_GROUP_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply const&>( msg );
		} break;
		case rofl::openflow12::OFPST_GROUP_FEATURES: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_features_stats_reply const&>( msg );
		} break;
		// TODO: experimenter statistics
		default: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_stats_reply const&>( msg );
		} break;
		}

	} break;
	case rofl::openflow12::OFPT_BARRIER_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_barrier_request const&>( msg );
	} break;
	case rofl::openflow12::OFPT_BARRIER_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_barrier_reply const&>( msg );
	} break;
	case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request const&>( msg );
	} break;
	case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_get_config_reply const&>( msg );
	} break;
	case rofl::openflow12::OFPT_ROLE_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_role_request const&>( msg );
	} break;
	case rofl::openflow12::OFPT_ROLE_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_role_reply const&>( msg );
	} break;
	case rofl::openflow12::OFPT_GET_ASYNC_REQUEST: {
    	rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_async_config_request const&>( msg );
    } break;
	case rofl::openflow12::OFPT_GET_ASYNC_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_async_config_reply const&>( msg );
	} break;
	case rofl::openflow12::OFPT_SET_ASYNC: {
    	rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_set_async_config const&>( msg );
    } break;
	default: {
		rofl::logging::debug2 << "[rofl-common][crofsock] unknown message " << msg << std::endl;
	} break;
	}
}



void
crofsock::log_of13_message(
		rofl::openflow::cofmsg const& msg)
{
	switch (msg.get_type()) {
	case rofl::openflow13::OFPT_HELLO: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_hello const&>( msg );
	} break;
	case rofl::openflow13::OFPT_ERROR: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_error const&>( msg );
	} break;
	case rofl::openflow13::OFPT_ECHO_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_echo_request const&>( msg );
	} break;
	case rofl::openflow13::OFPT_ECHO_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_echo_reply const&>( msg );
	} break;
	case rofl::openflow13::OFPT_EXPERIMENTER:	{
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_experimenter const&>( msg );
	} break;
	case rofl::openflow13::OFPT_FEATURES_REQUEST:	{
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_features_request const&>( msg );
	} break;
	case rofl::openflow13::OFPT_FEATURES_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_features_reply const&>( msg );
	} break;
	case rofl::openflow13::OFPT_GET_CONFIG_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_config_request const&>( msg );
	} break;
	case rofl::openflow13::OFPT_GET_CONFIG_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_config_reply const&>( msg );
	} break;
	case rofl::openflow13::OFPT_SET_CONFIG: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_set_config const&>( msg );
	} break;
	case rofl::openflow13::OFPT_PACKET_OUT: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_packet_out const&>( msg );
	} break;
	case rofl::openflow13::OFPT_PACKET_IN: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_packet_in const&>( msg );
	} break;
	case rofl::openflow13::OFPT_FLOW_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_mod const&>( msg );
	} break;
	case rofl::openflow13::OFPT_FLOW_REMOVED: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_removed const&>( msg );
	} break;
	case rofl::openflow13::OFPT_GROUP_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_mod const&>( msg );
	} break;
	case rofl::openflow13::OFPT_PORT_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_mod const&>( msg );
	} break;
	case rofl::openflow13::OFPT_PORT_STATUS: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_status const&>( msg );
	} break;
	case rofl::openflow13::OFPT_TABLE_MOD: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_mod const&>( msg );
	} break;
	case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
		rofl::openflow::cofmsg_multipart_request const& stats = dynamic_cast<rofl::openflow::cofmsg_multipart_request const&>( msg );
		switch (stats.get_stats_type()) {
		case rofl::openflow13::OFPMP_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_desc_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_METER: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_meter_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_METER_CONFIG: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_METER_FEATURES: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_request const&>( msg );
		} break;
		// TODO: experimenter statistics
		default: {

		} break;
		}

	} break;
	case rofl::openflow13::OFPT_MULTIPART_REPLY: {
		rofl::openflow::cofmsg_multipart_reply const& stats = dynamic_cast<rofl::openflow::cofmsg_multipart_reply const&>( msg );
		switch (stats.get_stats_type()) {
		case rofl::openflow13::OFPMP_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_desc_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_FLOW: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_AGGREGATE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_TABLE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_PORT_STATS: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_QUEUE: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_GROUP: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_GROUP_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_GROUP_FEATURES: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_group_features_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_METER: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_METER_CONFIG: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_METER_FEATURES: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_TABLE_FEATURES: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply const&>( msg );
		} break;
		case rofl::openflow13::OFPMP_PORT_DESC: {
			rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply const&>( msg );
		} break;
		// TODO: experimenter statistics
		default: {

		} break;
		}

	} break;

	case rofl::openflow13::OFPT_BARRIER_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_barrier_request const&>( msg );
	} break;
	case rofl::openflow13::OFPT_BARRIER_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_barrier_reply const&>( msg );
	} break;
	case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request const&>( msg );
	} break;
	case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_queue_get_config_reply const&>( msg );
	} break;
	case rofl::openflow13::OFPT_ROLE_REQUEST: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_role_request const&>( msg );
	} break;
	case rofl::openflow13::OFPT_ROLE_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_role_reply const&>( msg );
	} break;
	case rofl::openflow13::OFPT_GET_ASYNC_REQUEST: {
    	rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_async_config_request const&>( msg );
    } break;
	case rofl::openflow13::OFPT_GET_ASYNC_REPLY: {
		rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_get_async_config_reply const&>( msg );
	} break;
	case rofl::openflow13::OFPT_SET_ASYNC: {
    	rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_set_async_config const&>( msg );
    } break;
	case rofl::openflow13::OFPT_METER_MOD: {
    	rofl::logging::debug2 << dynamic_cast<rofl::openflow::cofmsg_meter_mod const&>( msg );
    } break;
	default: {
		rofl::logging::debug2 << "[rofl-common][crofsock] unknown message " << msg << std::endl;
	} break;
	}
}

