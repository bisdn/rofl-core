/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crofdptImpl.h"

using namespace rofl;


crofdptImpl::crofdptImpl(
		crofbase *rofbase) :
				crofdpt(rofbase),
				dpid(0),
				hwaddr(cmacaddr("00:00:00:00:00:00")),
				n_buffers(0),
				n_tables(0),
				capabilities(0),
				config(0),
				miss_send_len(0),
				socket(0),
				rofbase(rofbase),
				fragment(0),
				msg_bytes_read(0),
				reconnect_start_timeout(0),
				reconnect_in_seconds(RECONNECT_START_TIMEOUT),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
				ofp_version(openflow12::OFP_VERSION),
				features_reply_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
				get_config_reply_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
				stats_reply_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
				barrier_reply_timeout(DEFAULT_DP_BARRIER_REPLY_TIMEOUT),
				get_async_config_reply_timeout(DEFAULT_DP_GET_ASYNC_CONFIG_REPLY_TIMEOUT)
{

}



crofdptImpl::crofdptImpl(
		crofbase *rofbase,
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				crofdpt(rofbase),
				dpid(0),
				hwaddr(cmacaddr("00:00:00:00:00:00")),
				n_buffers(0),
				n_tables(0),
				capabilities(0),
				config(0),
				miss_send_len(0),
				socket(new csocket(this, newsd, ra, domain, type, protocol)),
				rofbase(rofbase),
				fragment(0),
				msg_bytes_read(0),
				reconnect_start_timeout(0),
				reconnect_in_seconds(0),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
				ofp_version(openflow12::OFP_VERSION),
				features_reply_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
				get_config_reply_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
				stats_reply_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
				barrier_reply_timeout(DEFAULT_DP_BARRIER_REPLY_TIMEOUT),
				get_async_config_reply_timeout(DEFAULT_DP_GET_ASYNC_CONFIG_REPLY_TIMEOUT)
{
	init_state(COFDPT_STATE_DISCONNECTED);

    register_timer(COFDPT_TIMER_SEND_HELLO, 0);
}



crofdptImpl::crofdptImpl(
		crofbase *rofbase,
		uint8_t ofp_version,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				crofdpt(rofbase),
				dpid(0),
				hwaddr(cmacaddr("00:00:00:00:00:00")),
				n_buffers(0),
				n_tables(0),
				capabilities(0),
				config(0),
				miss_send_len(0),
				socket(new csocket(this, domain, type, protocol)),
				rofbase(rofbase),
				fragment(0),
				msg_bytes_read(0),
				reconnect_start_timeout(reconnect_start_timeout),
				reconnect_in_seconds(reconnect_start_timeout),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
				ofp_version(ofp_version),
				features_reply_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
				get_config_reply_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
				stats_reply_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
				barrier_reply_timeout(DEFAULT_DP_BARRIER_REPLY_TIMEOUT),
				get_async_config_reply_timeout(DEFAULT_DP_GET_ASYNC_CONFIG_REPLY_TIMEOUT)
{
	init_state(COFDPT_STATE_DISCONNECTED);

	dptflags.set(COFDPT_FLAG_ACTIVE_SOCKET);

	this->reconnect_in_seconds = this->reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;

	socket->cconnect(ra, caddress(AF_INET, "0.0.0.0"), domain, type, protocol);
}



crofdptImpl::~crofdptImpl()
{
	// remove all cofport instances
	while (not ports.empty()) {
		delete (ports.begin()->second);
	}
}



uint8_t
crofdptImpl::get_version()
{
	return ofp_version;
}



caddress
crofdptImpl::get_peer_addr()
{
	return socket->raddr;
}



void
crofdptImpl::handle_accepted(
		csocket *socket,
		int newsd,
		caddress const& ra)
{
	logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " connection accepted:" << ra << " (init -> disconnected)" << std::endl;
}



void
crofdptImpl::handle_connected(
		csocket *socket,
		int sd)
{
	logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " connection established:" << socket->raddr << " (init -> disconnected)" << std::endl;

	register_timer(COFDPT_TIMER_SEND_HELLO, 0);
}



void
crofdptImpl::handle_connect_refused(
		csocket *socket,
		int sd)
{
	logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "connection failed:" << socket->raddr << std::endl;

	rofbase->rpc_dpt_failed(this); // signal event back to rofbase

	new_state(COFDPT_STATE_DISCONNECTED);
	if (dptflags.test(COFDPT_FLAG_ACTIVE_SOCKET)) {
		try_to_connect();
	}
}



void
crofdptImpl::handle_read(
		csocket *socket,
		int sd)
{
	int rc = 0;

	cmemory *mem = (cmemory*)0;
	try {

		if (0 == fragment) {
			mem = new cmemory(sizeof(struct openflow::ofp_header));
			msg_bytes_read = 0;
		} else {
			mem = fragment;
		}

		while (true) {

			uint16_t msg_len = 0;

			// how many bytes do we have to read?
			if (msg_bytes_read < sizeof(struct openflow::ofp_header)) {
				msg_len = sizeof(struct openflow::ofp_header);
			} else {
				struct openflow::ofp_header *ofh_header = (struct openflow::ofp_header*)mem->somem();
				msg_len = be16toh(ofh_header->length);
			}
			//fprintf(stderr, "how many? msg_len=%d mem: %s\n", msg_len, mem->c_str());

			// resize msg buffer, if necessary
			if (mem->memlen() < msg_len) {
				mem->resize(msg_len);
			}

			// TODO: SSL/TLS socket

			// read from socket
			rc = read(sd, (void*)(mem->somem() + msg_bytes_read), msg_len - msg_bytes_read);
			//fprintf(stderr, "read %d bytes\n", rc);


			if (rc < 0) // error occured (or non-blocking)
			{
				switch(errno) {
				case EAGAIN: {
					fragment = mem; // more bytes are needed, store pcppack in fragment pointer
				} return;
				case ECONNREFUSED: {
					try_to_connect(); // reconnect
				} return;
				case ECONNRESET:
				default: {
					logging::error << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " error reading from socket descriptor:" << sd
							<< " " << eSysCall() << ", closing socket. (... -> disconnected)" << std::endl;
					handle_closed(socket, sd);
				} return;
				}
			}
			else if (rc == 0) // socket was closed
			{
				//rfds.erase(fd);
				logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " peer closed connection. (... -> disconnected)" << *this << std::endl;

				if (mem) {
					delete mem; fragment = (cmemory*)0;
				}
				handle_closed(socket, sd);

				return;
			}
			else // rc > 0, // some bytes were received, check for completeness of packet
			{
				msg_bytes_read += rc;

				// minimum message length received, check completeness of message
				if (mem->memlen() >= sizeof(struct openflow::ofp_header)) {
					struct openflow::ofp_header *ofh_header = (struct openflow::ofp_header*)mem->somem();
					uint16_t msg_len = be16toh(ofh_header->length);

					// ok, message was received completely
					if (msg_len == msg_bytes_read) {
						fragment = (cmemory*)0;
						parse_message(mem);
						return;
					}
				}
			}
		}

	} catch (eOFpacketInval& e) {

		logging::warn << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "dpid:" << dpid << " dropping invalid packet " << *mem << std::endl;
		if (mem) {
			delete mem; fragment = (cmemory*)0;
		}
		// handle_closed(socket, sd);
	}
}



void
crofdptImpl::handle_closed(
		csocket *socket,
		int sd)
{
	logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec
			<< " connection closed. " << *this << std::endl;

	socket->cclose();

	cancel_timer(COFDPT_TIMER_ECHO_REPLY);
	cancel_timer(COFDPT_TIMER_SEND_ECHO_REQUEST);

	flags.reset(COFDPT_FLAG_HELLO_SENT);
	flags.reset(COFDPT_FLAG_HELLO_RCVD);

	if (dptflags.test(COFDPT_FLAG_ACTIVE_SOCKET)) {
		try_to_connect(true);
	} else {
		rofbase->handle_dpt_close(this);
	}
}



void
crofdptImpl::parse_message(
		cmemory *mem)
{
	cofmsg *msg = (cofmsg*)0;

	try {
		if (0 == mem) {
			return;
		}

		struct openflow::ofp_header* ofh_header = (struct openflow::ofp_header*)mem->somem();

		const uint8_t OFPT_HELLO = 0;

		if (not flags.test(COFDPT_FLAG_HELLO_RCVD) && (OFPT_HELLO != ofh_header->type)) {
	    	logging::error << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " dropping message (missing HELLO from peer) " << *mem << " " << *this << std::endl;
	    	delete mem; return;
		}

		switch (ofp_version) {
		case openflow10::OFP_VERSION: {

			switch (ofh_header->type) {
			case openflow10::OFPT_HELLO: {
				(msg = new cofmsg_hello(mem))->validate();
				hello_rcvd(dynamic_cast<cofmsg_hello*>( msg ));
			} break;
			case openflow10::OFPT_ECHO_REQUEST: {
				(msg = new cofmsg_echo_request(mem))->validate();
				echo_request_rcvd(dynamic_cast<cofmsg_echo_request*>( msg ));
			} break;
			case openflow10::OFPT_ECHO_REPLY: {
				(msg = new cofmsg_echo_reply(mem))->validate();
				echo_reply_rcvd(dynamic_cast<cofmsg_echo_reply*>( msg ));
			} break;
			case openflow10::OFPT_VENDOR:	{
				(msg = new cofmsg_experimenter(mem))->validate();
				experimenter_rcvd(dynamic_cast<cofmsg_experimenter*>( msg ));
			} break;
			case openflow10::OFPT_FEATURES_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow10::OFPT_FEATURES_REQUEST);
				(msg = new cofmsg_features_reply(mem))->validate();
				features_reply_rcvd(dynamic_cast<cofmsg_features_reply*>( msg ));
			} break;
			case openflow10::OFPT_GET_CONFIG_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow10::OFPT_GET_CONFIG_REQUEST);
				(msg = new cofmsg_get_config_reply(mem))->validate();
				get_config_reply_rcvd(dynamic_cast<cofmsg_get_config_reply*>( msg ));
			} break;
			case openflow10::OFPT_PACKET_IN: {
				(msg = new cofmsg_packet_in(mem))->validate();
				packet_in_rcvd(dynamic_cast<cofmsg_packet_in*>( msg ));
			} break;
			case openflow10::OFPT_FLOW_REMOVED: {
				(msg = new cofmsg_flow_removed(mem))->validate();
				flow_rmvd_rcvd(dynamic_cast<cofmsg_flow_removed*>( msg ));
			} break;
			case openflow10::OFPT_PORT_STATUS: {
				(msg = new cofmsg_port_status(mem))->validate();
				port_status_rcvd(dynamic_cast<cofmsg_port_status*>( msg ));
			} break;
			case openflow10::OFPT_STATS_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow10::OFPT_STATS_REQUEST);

				if (mem->memlen() < sizeof(struct openflow10::ofp_stats_reply)) {
					msg = new cofmsg(mem);
					throw eBadSyntaxTooShort();
				}
				uint16_t stats_type = be16toh(((struct openflow10::ofp_stats_reply*)mem->somem())->type);

				switch (stats_type) {
				case openflow10::OFPST_DESC: {
					(msg = new cofmsg_desc_stats_reply(mem))->validate();
					desc_stats_reply_rcvd(dynamic_cast<cofmsg_desc_stats_reply*>( msg ));
				} break;
				case openflow10::OFPST_FLOW: {
					(msg = new cofmsg_flow_stats_reply(mem))->validate();
					flow_stats_reply_rcvd(dynamic_cast<cofmsg_flow_stats_reply*>( msg ));
				} break;
				case openflow10::OFPST_AGGREGATE: {
					(msg = new cofmsg_aggr_stats_reply(mem))->validate();
					aggregate_stats_reply_rcvd(dynamic_cast<cofmsg_aggr_stats_reply*>( msg ));
				} break;
				case openflow10::OFPST_TABLE: {
					(msg = new cofmsg_table_stats_reply(mem))->validate();
					table_stats_reply_rcvd(dynamic_cast<cofmsg_table_stats_reply*>( msg ));
				} break;
				case openflow10::OFPST_PORT: {
					(msg = new cofmsg_port_stats_reply(mem))->validate();
					port_stats_reply_rcvd(dynamic_cast<cofmsg_port_stats_reply*>( msg ));
				} break;
				case openflow10::OFPST_QUEUE: {
					(msg = new cofmsg_queue_stats_reply(mem))->validate();
					queue_stats_reply_rcvd(dynamic_cast<cofmsg_queue_stats_reply*>( msg ));
				} break;
				// TODO: experimenter statistics
				default: {
					(msg = new cofmsg_stats_reply(mem))->validate();
					stats_reply_rcvd(dynamic_cast<cofmsg_stats*>( msg ));
				} break;
				}

			} break;
			case openflow10::OFPT_BARRIER_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow10::OFPT_BARRIER_REQUEST);
				(msg = new cofmsg_barrier_reply(mem))->validate();
				barrier_reply_rcvd(dynamic_cast<cofmsg_barrier_reply*>( msg ));
			} break;
			case openflow10::OFPT_QUEUE_GET_CONFIG_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST);
				(msg = new cofmsg_queue_get_config_reply(mem))->validate();
				queue_get_config_reply_rcvd(dynamic_cast<cofmsg_queue_get_config_reply*>( msg ));
			} break;
			default: {
				logging::warn << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " dropping unknown message type:" << (int)ofh_header->type << std::endl;
				delete mem;
			} return;
			}

		} break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {

			switch (ofh_header->type) {
			case openflow12::OFPT_HELLO: {
				(msg = new cofmsg_hello(mem))->validate();
				hello_rcvd(dynamic_cast<cofmsg_hello*>( msg ));
			} break;
			case openflow12::OFPT_ECHO_REQUEST: {
				(msg = new cofmsg_echo_request(mem))->validate();
				echo_request_rcvd(dynamic_cast<cofmsg_echo_request*>( msg ));
			} break;
			case openflow12::OFPT_ECHO_REPLY: {
				(msg = new cofmsg_echo_reply(mem))->validate();
				echo_reply_rcvd(dynamic_cast<cofmsg_echo_reply*>( msg ));
			} break;
			case openflow12::OFPT_EXPERIMENTER:	{
				(msg = new cofmsg_experimenter(mem))->validate();
				experimenter_rcvd(dynamic_cast<cofmsg_experimenter*>( msg ));
			} break;
			case openflow12::OFPT_FEATURES_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow12::OFPT_FEATURES_REQUEST);
				(msg = new cofmsg_features_reply(mem))->validate();
				features_reply_rcvd(dynamic_cast<cofmsg_features_reply*>( msg ));
			} break;
			case openflow12::OFPT_GET_CONFIG_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow12::OFPT_GET_CONFIG_REQUEST);
				(msg = new cofmsg_get_config_reply(mem))->validate();
				get_config_reply_rcvd(dynamic_cast<cofmsg_get_config_reply*>( msg ));
			} break;
			case openflow12::OFPT_PACKET_IN: {
				(msg = new cofmsg_packet_in(mem))->validate();
				packet_in_rcvd(dynamic_cast<cofmsg_packet_in*>( msg ));
			} break;
			case openflow12::OFPT_FLOW_REMOVED: {
				(msg = new cofmsg_flow_removed(mem))->validate();
				flow_rmvd_rcvd(dynamic_cast<cofmsg_flow_removed*>( msg ));
			} break;
			case openflow12::OFPT_PORT_STATUS: {
				(msg = new cofmsg_port_status(mem))->validate();
				port_status_rcvd(dynamic_cast<cofmsg_port_status*>( msg ));
			} break;
			case openflow12::OFPT_STATS_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow12::OFPT_STATS_REQUEST);

				if (mem->memlen() < sizeof(struct openflow12::ofp_stats_reply)) {
					msg = new cofmsg(mem);
					throw eBadSyntaxTooShort();
				}
				uint16_t stats_type = be16toh(((struct openflow12::ofp_stats_reply*)mem->somem())->type);

				switch (stats_type) {
				case openflow12::OFPST_DESC: {
					(msg = new cofmsg_desc_stats_reply(mem))->validate();
					desc_stats_reply_rcvd(dynamic_cast<cofmsg_desc_stats_reply*>( msg ));
				} break;
				case openflow12::OFPST_FLOW: {
					(msg = new cofmsg_flow_stats_reply(mem))->validate();
					flow_stats_reply_rcvd(dynamic_cast<cofmsg_flow_stats_reply*>( msg ));
				} break;
				case openflow12::OFPST_AGGREGATE: {
					(msg = new cofmsg_aggr_stats_reply(mem))->validate();
					aggregate_stats_reply_rcvd(dynamic_cast<cofmsg_aggr_stats_reply*>( msg ));
				} break;
				case openflow12::OFPST_TABLE: {
					(msg = new cofmsg_table_stats_reply(mem))->validate();
					table_stats_reply_rcvd(dynamic_cast<cofmsg_table_stats_reply*>( msg ));
				} break;
				case openflow12::OFPST_PORT: {
					(msg = new cofmsg_port_stats_reply(mem))->validate();
					port_stats_reply_rcvd(dynamic_cast<cofmsg_port_stats_reply*>( msg ));
				} break;
				case openflow12::OFPST_QUEUE: {
					(msg = new cofmsg_queue_stats_reply(mem))->validate();
					queue_stats_reply_rcvd(dynamic_cast<cofmsg_queue_stats_reply*>( msg ));
				} break;
				case openflow12::OFPST_GROUP: {
					(msg = new cofmsg_group_stats_reply(mem))->validate();
					group_stats_reply_rcvd(dynamic_cast<cofmsg_group_stats_reply*>( msg ));
				} break;
				case openflow12::OFPST_GROUP_DESC: {
					(msg = new cofmsg_group_desc_stats_reply(mem))->validate();
					group_desc_stats_reply_rcvd(dynamic_cast<cofmsg_group_desc_stats_reply*>( msg ));
				} break;
				case openflow12::OFPST_GROUP_FEATURES: {
					(msg = new cofmsg_group_features_stats_reply(mem))->validate();
					group_features_stats_reply_rcvd(dynamic_cast<cofmsg_group_features_stats_reply*>( msg ));
				} break;
				// TODO: experimenter statistics
				default: {
					(msg = new cofmsg_stats_reply(mem))->validate();
					stats_reply_rcvd(dynamic_cast<cofmsg_stats_reply*>( msg ));
				} break;
				}

			} break;
			case openflow12::OFPT_BARRIER_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow12::OFPT_BARRIER_REQUEST);
				(msg = new cofmsg_barrier_reply(mem))->validate();
				barrier_reply_rcvd(dynamic_cast<cofmsg_barrier_reply*>( msg ));
			} break;
			case openflow12::OFPT_QUEUE_GET_CONFIG_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST);
				(msg = new cofmsg_queue_get_config_reply(mem))->validate();
				queue_get_config_reply_rcvd(dynamic_cast<cofmsg_queue_get_config_reply*>( msg ));
			} break;
			case openflow12::OFPT_ROLE_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow12::OFPT_ROLE_REQUEST);
				(msg = new cofmsg_role_reply(mem))->validate();
				role_reply_rcvd(dynamic_cast<cofmsg_role_reply*>( msg ));
			} break;
			case openflow12::OFPT_GET_ASYNC_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), openflow12::OFPT_GET_ASYNC_REQUEST);
				(msg = new cofmsg_get_async_config_reply(mem))->validate();
				get_async_config_reply_rcvd(dynamic_cast<cofmsg_get_async_config_reply*>( msg ));
			} break;
			default: {
				logging::warn << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " dropping unknown message type:" << (int)ofh_header->type << std::endl;
				delete mem;
			} return;
			}

		} break;
		default:
			throw eBadVersion();
		}

	} catch (eBadSyntaxTooShort& e) {

		logging::error << "eBadSyntaxTooShort " << *mem << std::endl;
		delete msg;

	} catch (eBadVersion& e) {

		logging::warn << "eBadVersion " << *mem << std::endl;
		delete msg;

	} catch (eRofBaseXidInval& e) {

		logging::warn << "eRofBaseXidInval " << *mem << std::endl;
		delete msg;
	}
}



void
crofdptImpl::send_message(
		cofmsg *msg)
{
	const uint8_t OFPT_HELLO = 0;

    if (not flags.test(COFDPT_FLAG_HELLO_RCVD) && (msg->get_type() != OFPT_HELLO)) {
    	logging::error << "dropping message (missing HELLO from peer) " << *msg << " " << *this << std::endl;
    	delete msg; return;
    }

    switch (msg->get_version()) {
    case openflow10::OFP_VERSION: {

    	switch (msg->get_type()) {
    	case openflow10::OFPT_HELLO: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Hello message " << std::endl << *dynamic_cast<cofmsg_hello*>(msg);
    		// do nothing here
    	} break;
    	case openflow10::OFPT_ECHO_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Echo-Request message " << std::endl << *dynamic_cast<cofmsg_echo_request*>(msg);
    		echo_request_sent(msg);
    	} break;
    	case openflow10::OFPT_ECHO_REPLY: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Echo-Reply message " << std::endl << *dynamic_cast<cofmsg_echo_reply*>(msg);
    		// do nothing here
    	} break;
    	case openflow10::OFPT_ERROR: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Error message " << std::endl << *dynamic_cast<cofmsg_error*>(msg);
    	} break;
    	case openflow10::OFPT_VENDOR: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Vendor message " << std::endl << *dynamic_cast<cofmsg_experimenter*>(msg);
    	} break;
    	case openflow10::OFPT_SET_CONFIG: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Set-Config message " << std::endl << *dynamic_cast<cofmsg_set_config*>(msg);
    	} break;
    	case openflow10::OFPT_PACKET_OUT: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Packet-Out message " << std::endl << *dynamic_cast<cofmsg_packet_out*>(msg);
    	} break;
    	case openflow10::OFPT_FLOW_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Flow-Mod message " << std::endl << *dynamic_cast<cofmsg_flow_mod*>(msg);
    	} break;
    	case openflow10::OFPT_PORT_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Port-Mod message " << std::endl << *dynamic_cast<cofmsg_port_mod*>(msg);
    	} break;
    	case openflow10::OFPT_FEATURES_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Features-Request message " << std::endl << *dynamic_cast<cofmsg_features_request*>(msg);
    		features_request_sent(msg);
    	} break;
    	case openflow10::OFPT_GET_CONFIG_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Get-Config-Request message " << std::endl << *dynamic_cast<cofmsg_get_config_request*>(msg);
    		get_config_request_sent(msg);
    	} break;
    	case openflow10::OFPT_STATS_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Stats-Request message " << std::endl << *dynamic_cast<cofmsg_stats_request*>(msg);
    		stats_request_sent(msg);
    	} break;
    	case openflow10::OFPT_BARRIER_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Barrier-Request message " << std::endl << *dynamic_cast<cofmsg_barrier_request*>(msg);
    		barrier_request_sent(msg);
    	} break;
    	case openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Queue-Get-Config-Request message " << std::endl << *dynamic_cast<cofmsg_queue_get_config_request*>(msg);
    		queue_get_config_request_sent(msg);
    	} break;
    	default: {
        	logging::error << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " dropping invalid packet " << *dynamic_cast<cofmsg*>(msg) << " " << *this << std::endl;
    		delete msg;
    	} return;
    	}

    } break;
    case openflow12::OFP_VERSION: {

    	switch (msg->get_type()) {
    	case openflow12::OFPT_HELLO: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Hello message " << std::endl << *dynamic_cast<cofmsg_hello*>(msg);
    	} break;
    	case openflow12::OFPT_ECHO_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Echo-Request message " << std::endl << *dynamic_cast<cofmsg_echo_request*>(msg);
    		echo_request_sent(msg);
    	} break;
    	case openflow12::OFPT_ECHO_REPLY: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Echo-Reply message " << std::endl << *dynamic_cast<cofmsg_echo_reply*>(msg);
    	} break;
    	case openflow12::OFPT_ERROR: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Error message " << std::endl << *dynamic_cast<cofmsg_error*>(msg);
    	} break;
    	case openflow12::OFPT_EXPERIMENTER: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Experimenter message " << std::endl << *dynamic_cast<cofmsg_experimenter*>(msg);
    	} break;
    	case openflow12::OFPT_SET_CONFIG: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Set-Config message " << std::endl << *dynamic_cast<cofmsg_set_config*>(msg);
    	} break;
    	case openflow12::OFPT_PACKET_OUT: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Packet-Out message " << std::endl << *dynamic_cast<cofmsg_packet_out*>(msg);
    	} break;
    	case openflow12::OFPT_FLOW_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Flow-Mod message " << std::endl << *dynamic_cast<cofmsg_flow_mod*>(msg);
    	} break;
    	case openflow12::OFPT_GROUP_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Group-Mod message " << std::endl << *dynamic_cast<cofmsg_group_mod*>(msg);
    	} break;
    	case openflow12::OFPT_PORT_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Port-Mod message " << std::endl << *dynamic_cast<cofmsg_port_mod*>(msg);
    	} break;
    	case openflow12::OFPT_TABLE_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Table-Mod message " << std::endl << *dynamic_cast<cofmsg_table_mod*>(msg);
    	} break;
    	case openflow12::OFPT_FEATURES_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Features-Request message " << std::endl << *dynamic_cast<cofmsg_features_request*>(msg);
    		features_request_sent(msg);
    	} break;
    	case openflow12::OFPT_GET_CONFIG_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Get-Config-Request message " << std::endl << *dynamic_cast<cofmsg_get_config_request*>(msg);
    		get_config_request_sent(msg);
    	} break;
    	case openflow12::OFPT_STATS_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Stats-Request message " << std::endl << *dynamic_cast<cofmsg_stats_request*>(msg);
    		stats_request_sent(msg);
    	} break;
    	case openflow12::OFPT_BARRIER_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Barrier-Request message " << std::endl << *dynamic_cast<cofmsg_barrier_request*>(msg);
    		barrier_request_sent(msg);
    	} break;
    	case openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Queue-Get-Config-Request message " << std::endl << *dynamic_cast<cofmsg_queue_get_config_request*>(msg);
    		queue_get_config_request_sent(msg);
    	} break;
    	case openflow12::OFPT_ROLE_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Role-Request message " << std::endl << *dynamic_cast<cofmsg_role_request*>(msg);
    		role_request_sent(msg);
    	} break;
    	default: {
        	logging::error << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " dropping invalid packet " << *dynamic_cast<cofmsg*>(msg) << " " << *this << std::endl;
        	delete msg;
    	} return;
    	}

    } break;
    case openflow13::OFP_VERSION: {

    	switch (msg->get_type()) {
    	case openflow13::OFPT_HELLO: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Hello message " << std::endl << *dynamic_cast<cofmsg_hello*>(msg);
    	} break;
    	case openflow13::OFPT_ECHO_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Echo-Request message " << std::endl << *dynamic_cast<cofmsg_echo_request*>(msg);
    		echo_request_sent(msg);
    	} break;
    	case openflow13::OFPT_ECHO_REPLY: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Echo-Reply message " << std::endl << *dynamic_cast<cofmsg_echo_reply*>(msg);
    	} break;
    	case openflow13::OFPT_ERROR: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Error message " << std::endl << *dynamic_cast<cofmsg_error*>(msg);
    	} break;
    	case openflow13::OFPT_EXPERIMENTER: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Experimenter message " << std::endl << *dynamic_cast<cofmsg_experimenter*>(msg);
    	} break;
    	case openflow13::OFPT_SET_CONFIG: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Set-Config message " << std::endl << *dynamic_cast<cofmsg_set_config*>(msg);
    	} break;
    	case openflow13::OFPT_PACKET_OUT: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Packet-Out message " << std::endl << *dynamic_cast<cofmsg_packet_out*>(msg);
    	} break;
    	case openflow13::OFPT_FLOW_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Flow-Mod message " << std::endl << *dynamic_cast<cofmsg_flow_mod*>(msg);
    	} break;
    	case openflow13::OFPT_GROUP_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Group-Mod message " << std::endl << *dynamic_cast<cofmsg_group_mod*>(msg);
    	} break;
    	case openflow13::OFPT_PORT_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Port-Mod message " << std::endl << *dynamic_cast<cofmsg_port_mod*>(msg);
    	} break;
    	case openflow13::OFPT_TABLE_MOD: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Table-Mod message " << std::endl << *dynamic_cast<cofmsg_table_mod*>(msg);
    	} break;
    	case openflow13::OFPT_SET_ASYNC: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Set-Async message " << std::endl << *dynamic_cast<cofmsg_set_async_config*>(msg);
    	} break;
    	case openflow13::OFPT_FEATURES_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Features-Request message " << std::endl << *dynamic_cast<cofmsg_features_request*>(msg);
    		features_request_sent(msg);
    	} break;
    	case openflow13::OFPT_GET_CONFIG_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Get-Config-Request message " << std::endl << *dynamic_cast<cofmsg_get_config_request*>(msg);
    		get_config_request_sent(msg);
    	} break;
    	case openflow13::OFPT_MULTIPART_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Stats-Request message " << std::endl << *dynamic_cast<cofmsg_stats_request*>(msg);
    		stats_request_sent(msg);
    	} break;
    	case openflow13::OFPT_BARRIER_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Barrier-Request message " << std::endl << *dynamic_cast<cofmsg_barrier_request*>(msg);
    		barrier_request_sent(msg);
    	} break;
    	case openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Queue-Get-Config-Request message " << std::endl << *dynamic_cast<cofmsg_queue_get_config_request*>(msg);
    		queue_get_config_request_sent(msg);
    	} break;
    	case openflow13::OFPT_ROLE_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Role-Request message " << std::endl << *dynamic_cast<cofmsg_role_request*>(msg);
    		role_request_sent(msg);
    	} break;
    	case openflow13::OFPT_GET_ASYNC_REQUEST: {
    		logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " sending Get-Async-Request message " << std::endl << *dynamic_cast<cofmsg_get_async_config_request*>(msg);
    		get_async_config_request_sent(msg);
    	} break;
    	default: {
        	logging::error << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " dropping invalid packet " << *dynamic_cast<cofmsg*>(msg) << " " << *this << std::endl;
        	delete msg;
    	} return;
    	}

    } break;
    default:
    	throw eBadVersion();
    }
	send_message_via_socket(msg);
}



void
crofdptImpl::handle_timeout(int opaque)
{
	switch (opaque) {
	case COFDPT_TIMER_SEND_HELLO: {
		rofbase->send_hello_message(this);
		flags.set(COFDPT_FLAG_HELLO_SENT);

		if (flags.test(COFDPT_FLAG_HELLO_RCVD)) {
			rofbase->send_features_request(this);
			rofbase->send_echo_request(this);
		}
	} break;
	case COFDPT_TIMER_FEATURES_REQUEST: {
		rofbase->send_features_request(this);
	} break;
	case COFDPT_TIMER_FEATURES_REPLY: {
		handle_features_reply_timeout();
	} break;
	case COFDPT_TIMER_GET_CONFIG_REPLY: {
		handle_get_config_reply_timeout();
	} break;
	case COFDPT_TIMER_STATS_REPLY: {
		handle_stats_reply_timeout();
	} break;
	case COFDPT_TIMER_BARRIER_REPLY: {
		handle_barrier_reply_timeout();
	} break;
	case COFDPT_TIMER_RECONNECT: {
		if (socket) {
			socket->cconnect(socket->raddr, caddress(AF_INET, "0.0.0.0"), socket->domain, socket->type, socket->protocol);
		}
	} break;
	case COFDPT_TIMER_SEND_ECHO_REQUEST: {
		rofbase->send_echo_request(this);
	} break;
	case COFDPT_TIMER_ECHO_REPLY: {
		handle_echo_reply_timeout();
	} break;
	case COFDPT_TIMER_GET_ASYNC_CONFIG_REPLY: {
		handle_get_async_config_reply_timeout();
	} break;
	default: {
		logging::error << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " unknown timer event:" << opaque << std::endl;
	} break;
	}
}



void
crofdptImpl::hello_rcvd(cofmsg_hello *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Hello message received" << std::endl << *msg;

	try {
		// OpenFlow versions do not match, send error, close connection
		if (not rofbase->is_ofp_version_supported(msg->get_version()))
		{
			new_state(COFDPT_STATE_DISCONNECTED);

			logging::info << "unsupported OF version during HELLO exhange, closing." << *this << std::endl;

			// invalid OFP_VERSION
			char explanation[256];
			bzero(explanation, sizeof(explanation));
			snprintf(explanation, sizeof(explanation) - 1,
					"unsupported OF version (%d), supported version is (%d)",
					msg->get_version(), openflow12::OFP_VERSION);

			throw eHelloIncompatible();
		}
		else
		{
			ofp_version = msg->get_version();

			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
					<< "HELLO exchanged with peer entity (disconnected -> wait-features-reply)" << std::endl;

			flags.set(COFDPT_FLAG_HELLO_RCVD);

			new_state(COFDPT_STATE_WAIT_FEATURES);

			if (flags.test(COFDPT_FLAG_HELLO_SENT))
			{
				rofbase->send_features_request(this);

				register_timer(COFDPT_TIMER_SEND_ECHO_REQUEST, 2);
			}
		}

		delete msg;

	} catch (eHelloIncompatible& e) {

		logging::warn << "eHelloIncompatible " << *msg << std::endl;
		rofbase->send_error_hello_failed_incompatible(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;
		handle_closed(socket, socket->sd);

	} catch (eHelloEperm& e) {

		logging::warn << "eHelloEperm " << *msg << std::endl;
		rofbase->send_error_hello_failed_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;
		handle_closed(socket, socket->sd);

	} catch (eHelloBase& e) {

		logging::warn << "eHelloBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofdptImpl::echo_request_sent(cofmsg *pack)
{
	reset_timer(COFDPT_TIMER_ECHO_REPLY, 5); // TODO: multiple concurrent echo-requests?
}



void
crofdptImpl::echo_request_rcvd(cofmsg_echo_request *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Echo-Request message received" << std::endl << *msg;

	// send echo reply back including any appended data
	rofbase->send_echo_reply(this, msg->get_xid(), msg->get_body().somem(), msg->get_body().memlen());

	delete msg;

	/* Please note: we do not call a handler for echo-requests/replies in rofbase
	 * and take care of these liveness packets within cofctl and cofdpt
	 */
}



void
crofdptImpl::echo_reply_rcvd(cofmsg_echo_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Echo-Reply message received" << std::endl << *msg;

	cancel_timer(COFDPT_TIMER_ECHO_REPLY);
	register_timer(COFDPT_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);

	delete msg;
}



void
crofdptImpl::handle_echo_reply_timeout()
{
	logging::warn << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Echo-Reply timer expired" << *this << std::endl;

	// TODO: repeat ECHO request multiple times (should be configurable)

	socket->cclose();
	new_state(COFDPT_STATE_DISCONNECTED);
	if (dptflags.test(COFDPT_FLAG_ACTIVE_SOCKET)) {
			try_to_connect(true);
	}
	rofbase->handle_dpt_close(this);
}



void
crofdptImpl::features_request_sent(
		cofmsg *pack)
{
	register_timer(COFDPT_TIMER_FEATURES_REPLY, features_reply_timeout /* seconds */);
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

		if (COFDPT_STATE_WAIT_FEATURES == cur_state()) {

			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
					<< "Features-Reply rcvd (wait-features-reply -> wait-get-config-reply)" << std::endl;

			// next step: send GET-CONFIG request to datapath
			rofbase->send_get_config_request(this);

			new_state(COFDPT_STATE_WAIT_GET_CONFIG);
		}


	} catch (eOFportMalformed& e) {

		logging::error << "eOFportMalformed " << *msg << std::endl;

		socket->cclose();

		new_state(COFDPT_STATE_DISCONNECTED);

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
crofdptImpl::get_config_request_sent(
		cofmsg *pack)
{
	register_timer(COFDPT_TIMER_GET_CONFIG_REPLY, get_config_reply_timeout);
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

	if (COFDPT_STATE_WAIT_GET_CONFIG == cur_state()) {

		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			new_state(COFDPT_STATE_CONNECTED);
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
					<< "Get-Config-Reply rcvd (wait-get-config-reply -> connected)" << std::endl;
			rofbase->handle_dpt_open(this);
		} break;
		case openflow12::OFP_VERSION: {
			rofbase->send_stats_request(this, openflow12::OFPST_TABLE, 0);
			new_state(COFDPT_STATE_WAIT_TABLE_STATS);
			logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << "" << *this << indent(2)
					<< "Get-Config-Reply rcvd (wait-get-config-reply -> wait-table-stats-reply)" << std::endl;
		} break;
		case openflow13::OFP_VERSION: {
			rofbase->send_stats_request(this, openflow13::OFPMP_TABLE, 0);
			new_state(COFDPT_STATE_WAIT_TABLE_STATS);
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
crofdptImpl::stats_request_sent(
		cofmsg *pack)
{
	try {
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			xidstore[openflow10::OFPT_STATS_REQUEST].xid_add(this, pack->get_xid(), stats_reply_timeout);
		} break;
		case openflow12::OFP_VERSION: {
			xidstore[openflow12::OFPT_STATS_REQUEST].xid_add(this, pack->get_xid(), stats_reply_timeout);
		} break;
		case openflow13::OFP_VERSION: {
			throw eNotImplemented(); // yet
		} break;
		default:
			throw eBadVersion();
		}

		if (not pending_timer(COFDPT_TIMER_STATS_REPLY))
		{
			register_timer(COFDPT_TIMER_STATS_REPLY, stats_reply_timeout);
		}

	} catch (eXidStoreXidBusy& e) {

		// should never happen, TODO: log error
	}
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

	if (COFDPT_STATE_WAIT_TABLE_STATS == cur_state()) {
		new_state(COFDPT_STATE_CONNECTED);
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
crofdptImpl::barrier_request_sent(
		cofmsg *pack)
{
	try {
		uint8_t msg_type = 0;

		switch (ofp_version) {
		case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_BARRIER_REQUEST; break;
		case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_BARRIER_REQUEST; break;
		case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_BARRIER_REQUEST; break;
		default:
			throw eBadVersion();
		}

		xidstore[msg_type].xid_add(this, pack->get_xid(), barrier_reply_timeout);

		if (not pending_timer(COFDPT_TIMER_BARRIER_REPLY))
		{
			register_timer(COFDPT_TIMER_BARRIER_REPLY, barrier_reply_timeout);
		}

	} catch (eXidStoreXidBusy& e) {

		// should never happen, TODO: log error
	}
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
crofdptImpl::flow_mod_sent(
		cofmsg *msg)
{

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
crofdptImpl::group_mod_sent(
		cofmsg *pack)
{

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
crofdptImpl::table_mod_sent(cofmsg *pack)
{
	cofmsg_table_mod *table_mod = dynamic_cast<cofmsg_table_mod*>( pack );

	if (0 == table_mod) {
		return;
	}

	// TODO: adjust local flowtable
}


void
crofdptImpl::port_mod_sent(cofmsg *pack)
{
	cofmsg_port_mod *port_mod = dynamic_cast<cofmsg_port_mod*>( pack );

	if (0 == port_mod) {
		return;
	}

	if (ports.find(port_mod->get_port_no()) == ports.end())
	{
		return;
	}

	ports[port_mod->get_port_no()]->recv_port_mod(
											port_mod->get_config(),
											port_mod->get_mask(),
											port_mod->get_advertise());
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
crofdptImpl::role_request_sent(
		cofmsg *pack)
{

}



void
crofdptImpl::role_reply_rcvd(cofmsg_role_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Role-Reply message received" << std::endl << *msg << std::endl;

	rofbase->handle_role_reply(this, msg);
}


void
crofdptImpl::queue_get_config_request_sent(
		cofmsg *pack)
{
	// TODO
}



void
crofdptImpl::queue_get_config_reply_rcvd(
		cofmsg_queue_get_config_reply *msg)
{
	logging::debug << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " Queue-Get-Config-Reply message received" << std::endl << *msg << std::endl;

	rofbase->handle_queue_get_config_reply(this, msg);
}


void
crofdptImpl::get_async_config_request_sent(
		cofmsg *pack)
{
	register_timer(COFDPT_TIMER_GET_ASYNC_CONFIG_REPLY, get_async_config_reply_timeout);
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



void
crofdptImpl::try_to_connect(bool reset_timeout)
{
	if (pending_timer(COFDPT_TIMER_RECONNECT)) {
		return;
	}

	logging::info << "[rofl][dpt] dpid:0x" << std::hex << dpid << std::dec << " scheduled reconnect in " << (int)reconnect_in_seconds
			<< " seconds." << *this << std::endl;

	int max_backoff = 16 * reconnect_start_timeout;

	if (reset_timeout) {
		reconnect_in_seconds = reconnect_start_timeout;
		reconnect_counter = 0;
	} else {
		reconnect_in_seconds *= 2;
	}


	if (reconnect_in_seconds > max_backoff) {
		reconnect_in_seconds = max_backoff;
	}

	reset_timer(COFDPT_TIMER_RECONNECT, reconnect_in_seconds);

	++reconnect_counter;
}



void
crofdptImpl::send_message_via_socket(
		cofmsg *pack)
{
	if (0 == socket) {
		delete pack; return;
	}

	cmemory *mem = new cmemory(pack->length());

	pack->pack(mem->somem(), mem->memlen());

	delete pack;

	socket->send_packet(mem);

	rofbase->wakeup(); // wake-up thread in case, we've been called from another thread
}


