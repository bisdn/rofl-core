/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofdptImpl.h"

using namespace rofl;


cofdptImpl::cofdptImpl(
		crofbase *rofbase) :
				cofdpt(rofbase),
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
				ofp_version(OFP12_VERSION),
				features_reply_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
				get_config_reply_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
				stats_reply_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
				barrier_reply_timeout(DEFAULT_DP_BARRIER_REPLY_TIMEOUT),
				get_async_config_reply_timeout(DEFAULT_DP_GET_ASYNC_CONFIG_REPLY_TIMEOUT)
{

}



cofdptImpl::cofdptImpl(
		crofbase *rofbase,
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				cofdpt(rofbase),
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
				ofp_version(OFP12_VERSION),
				features_reply_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
				get_config_reply_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
				stats_reply_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
				barrier_reply_timeout(DEFAULT_DP_BARRIER_REPLY_TIMEOUT),
				get_async_config_reply_timeout(DEFAULT_DP_GET_ASYNC_CONFIG_REPLY_TIMEOUT)
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::cofdpt() "
			"dpid:%"PRIu64" ", this, dpid);

#ifndef NDEBUG
        caddress raddr(ra);
        fprintf(stderr, "A:dpt[%s] ", raddr.c_str());
#endif

	init_state(COFDPT_STATE_DISCONNECTED);

        register_timer(COFDPT_TIMER_SEND_HELLO, 0);
}



cofdptImpl::cofdptImpl(
		crofbase *rofbase,
		uint8_t ofp_version,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				cofdpt(rofbase),
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
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::cofdpt() "
			"dpid:%"PRIu64" ",
			this, dpid);

	init_state(COFDPT_STATE_DISCONNECTED);

	dptflags.set(COFDPT_FLAG_ACTIVE_SOCKET);

	this->reconnect_in_seconds = this->reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;

	socket->cconnect(ra, caddress(AF_INET, "0.0.0.0"), domain, type, protocol);
}



cofdptImpl::~cofdptImpl()
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::~cofdpt() "
			"dpid:%"PRIu64"  %s",
			this, dpid, this->c_str());

	// remove all cofport instances
	while (not ports.empty())
	{
		delete (ports.begin()->second);
	}
}



uint8_t
cofdptImpl::get_version()
{
	return ofp_version;
}



caddress
cofdptImpl::get_peer_addr()
{
	return socket->raddr;
}



void
cofdptImpl::handle_accepted(
		csocket *socket,
		int newsd,
		caddress const& ra)
{
#ifndef NDEBUG
	caddress raddr(ra);
	fprintf(stderr, "A:dpt[%s] ", raddr.c_str());
#endif
}



void
cofdptImpl::handle_connected(
		csocket *socket,
		int sd)
{
#ifndef NDEBUG
	fprintf(stderr, "C:dpt[%s] ", socket->raddr.c_str());
#endif
	register_timer(COFDPT_TIMER_SEND_HELLO, 0);
}



void
cofdptImpl::handle_connect_refused(
		csocket *socket,
		int sd)
{
	new_state(COFDPT_STATE_DISCONNECTED);
	if (dptflags.test(COFDPT_FLAG_ACTIVE_SOCKET))
	{
		try_to_connect();
	}
}



void
cofdptImpl::handle_read(
		csocket *socket,
		int sd)
{
	int rc = 0;

	cmemory *mem = (cmemory*)0;
	try {

		if (0 == fragment) {
			mem = new cmemory(sizeof(struct ofp_header));
			msg_bytes_read = 0;
		} else {
			mem = fragment;
		}

		while (true) {

			uint16_t msg_len = 0;

			// how many bytes do we have to read?
			if (msg_bytes_read < sizeof(struct ofp_header)) {
				msg_len = sizeof(struct ofp_header);
			} else {
				struct ofp_header *ofh_header = (struct ofp_header*)mem->somem();
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
					WRITELOG(COFDPT, WARN, "cofdpt(%p)::handle_read() "
							"an error occured, closing => errno: %d (%s)",
							this, errno, strerror(errno));


					handle_closed(socket, sd);
				} return;
				}
			}
			else if (rc == 0) // socket was closed
			{
				//rfds.erase(fd);

				WRITELOG(COFDPT, WARN, "cofdpt(%p)::handle_read() "
						"peer closed connection, closing local endpoint => rc: %d",
						this, rc);

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
				if (mem->memlen() >= sizeof(struct ofp_header)) {
					struct ofp_header *ofh_header = (struct ofp_header*)mem->somem();
					uint16_t msg_len = be16toh(ofh_header->length);

					// ok, message was received completely
					if (msg_len == msg_bytes_read) {
						fragment = (cmemory*)0;
						handle_message(mem);
						return;
					}
				}
			}
		}

	} catch (eOFpacketInval& e) {

		WRITELOG(COFDPT, ERROR, "cofctl(%p)::handle_read() "
				"invalid packet received, dropping. Closing socket. Packet: %s",
				this, mem->c_str());
		if (mem) {
			delete mem; fragment = (cmemory*)0;
		}
		handle_closed(socket, sd);
	}
}



void
cofdptImpl::handle_closed(
		csocket *socket,
		int sd)
{
	socket->cclose();

	cancel_timer(COFDPT_TIMER_ECHO_REPLY);
	cancel_timer(COFDPT_TIMER_SEND_ECHO_REQUEST);

	flags.reset(COFDPT_FLAG_HELLO_SENT);
	flags.reset(COFDPT_FLAG_HELLO_RCVD);

	if (dptflags.test(COFDPT_FLAG_ACTIVE_SOCKET))
	{
		try_to_connect(true);
	}
	else
	{
		rofbase->handle_dpt_close(this);
	}
}



void
cofdptImpl::handle_message(
		cmemory *mem)
{
	cofmsg *msg = (cofmsg*)0;

	try {
		if (0 == mem) {
			writelog(COFDPT, WARN, "cofdpt(%p)::handle_message() "
					"assert(msg != 0) failed", this);
			return;
		}

		struct ofp_header* ofh_header = (struct ofp_header*)mem->somem();

		const uint8_t OFPT_HELLO = 0;

		if (not flags.test(COFDPT_FLAG_HELLO_RCVD) && (OFPT_HELLO != ofh_header->type)) {
			writelog(COFDPT, WARN, "cofdpt(%p)::handle_message() "
				"no HELLO rcvd yet, dropping message, msg: %s", this, mem->c_str());
			delete mem; return;
		}

		switch (ofp_version) {
		case OFP10_VERSION: {

			switch (ofh_header->type) {
			case OFPT10_HELLO: {
				msg = new cofmsg_hello(mem);
				msg->validate();
				hello_rcvd(dynamic_cast<cofmsg_hello*>( msg ));
			} break;
			case OFPT10_ECHO_REQUEST: {
				msg = new cofmsg_echo_request(mem);
				msg->validate();
				echo_request_rcvd(dynamic_cast<cofmsg_echo_request*>( msg ));
			} break;
			case OFPT10_ECHO_REPLY: {
				msg = new cofmsg_echo_reply(mem);
				msg->validate();
				echo_reply_rcvd(dynamic_cast<cofmsg_echo_reply*>( msg ));
			} break;
			case OFPT10_VENDOR:	{
				msg = new cofmsg_experimenter(mem);
				msg->validate();
				experimenter_rcvd(dynamic_cast<cofmsg_experimenter*>( msg ));
			} break;
			case OFPT10_FEATURES_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT10_FEATURES_REQUEST);
				msg = new cofmsg_features_reply(mem);
				msg->validate();
				features_reply_rcvd(dynamic_cast<cofmsg_features_reply*>( msg ));
			} break;
			case OFPT10_GET_CONFIG_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT10_GET_CONFIG_REQUEST);
				msg = new cofmsg_get_config_reply(mem);
				msg->validate();
				get_config_reply_rcvd(dynamic_cast<cofmsg_get_config_reply*>( msg ));
			} break;
			case OFPT10_PACKET_IN: {
				msg = new cofmsg_packet_in(mem);
				msg->validate();
				packet_in_rcvd(dynamic_cast<cofmsg_packet_in*>( msg ));
			} break;
			case OFPT10_FLOW_REMOVED: {
				msg = new cofmsg_flow_removed(mem);
				msg->validate();
				flow_rmvd_rcvd(dynamic_cast<cofmsg_flow_removed*>( msg ));
			} break;
			case OFPT10_PORT_STATUS: {
				msg = new cofmsg_port_status(mem);
				msg->validate();
				port_status_rcvd(dynamic_cast<cofmsg_port_status*>( msg ));
			} break;
			case OFPT10_STATS_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT10_STATS_REQUEST);
				uint16_t stats_type = 0;
				switch (ofh_header->version) {
				case OFP10_VERSION: {
					if (mem->memlen() < sizeof(struct ofp10_stats_reply)) {
						msg = new cofmsg(mem);
						throw eBadSyntaxTooShort();
					}
					stats_type = be16toh(((struct ofp10_stats_reply*)mem->somem())->type);
				} break;
				case OFP12_VERSION: {
					if (mem->memlen() < sizeof(struct ofp12_stats_reply)) {
						msg = new cofmsg(mem);
						throw eBadSyntaxTooShort();
					}
					stats_type = be16toh(((struct ofp12_stats_reply*)mem->somem())->type);
				} break;
				case OFP13_VERSION: {
					if (mem->memlen() < sizeof(struct ofp13_multipart_reply)) {
						msg = new cofmsg(mem);
						throw eBadSyntaxTooShort();
					}
					stats_type = be16toh(((struct ofp13_multipart_reply*)mem->somem())->type);
				} break;
				default:
					throw eBadVersion();
				}

				switch (stats_type) {
				case OFPST_DESC: {
					msg = new cofmsg_desc_stats_reply(mem);
				} break;
				case OFPST_FLOW: {
					msg = new cofmsg_flow_stats_reply(mem);
				} break;
				case OFPST_AGGREGATE: {
					msg = new cofmsg_aggr_stats_reply(mem);
				} break;
				case OFPST_TABLE: {
					msg = new cofmsg_table_stats_reply(mem);
				} break;
				case OFPST_PORT: {
					msg = new cofmsg_port_stats_reply(mem);
				} break;
				case OFPST_QUEUE: {
					msg = new cofmsg_queue_stats_reply(mem);
				} break;
				// TODO: experimenter statistics
				default: {
					msg = new cofmsg_stats_reply(mem);
				} break;
				}

				msg->validate();
				stats_reply_rcvd(dynamic_cast<cofmsg_stats_reply*>( msg ));
			} break;
			case OFPT10_BARRIER_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT10_BARRIER_REQUEST);
				msg = new cofmsg_barrier_reply(mem);
				msg->validate();
				barrier_reply_rcvd(dynamic_cast<cofmsg_barrier_reply*>( msg ));
			} break;
			case OFPT10_QUEUE_GET_CONFIG_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT10_QUEUE_GET_CONFIG_REQUEST);
				msg = new cofmsg_queue_get_config_reply(mem);
				msg->validate();
				queue_get_config_reply_rcvd(dynamic_cast<cofmsg_queue_get_config_reply*>( msg ));
			} break;
			default: {
				WRITELOG(COFDPT, WARN, "cofdpt(%p)::handle_message() "
						"dropping packet: %s", this, mem->c_str());
				delete mem;
			} return;
			}

		} break;
		case OFP12_VERSION:
		case OFP13_VERSION: {


			switch (ofh_header->type) {
			case OFPT12_HELLO: {
				msg = new cofmsg_hello(mem);
				msg->validate();
				hello_rcvd(dynamic_cast<cofmsg_hello*>( msg ));
			} break;
			case OFPT12_ECHO_REQUEST: {
				msg = new cofmsg_echo_request(mem);
				msg->validate();
				echo_request_rcvd(dynamic_cast<cofmsg_echo_request*>( msg ));
			} break;
			case OFPT12_ECHO_REPLY: {
				msg = new cofmsg_echo_reply(mem);
				msg->validate();
				echo_reply_rcvd(dynamic_cast<cofmsg_echo_reply*>( msg ));
			} break;
			case OFPT12_EXPERIMENTER:	{
				msg = new cofmsg_experimenter(mem);
				msg->validate();
				experimenter_rcvd(dynamic_cast<cofmsg_experimenter*>( msg ));
			} break;
			case OFPT12_FEATURES_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT12_FEATURES_REQUEST);
				msg = new cofmsg_features_reply(mem);
				msg->validate();
				features_reply_rcvd(dynamic_cast<cofmsg_features_reply*>( msg ));
			} break;
			case OFPT12_GET_CONFIG_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT12_GET_CONFIG_REQUEST);
				msg = new cofmsg_get_config_reply(mem);
				msg->validate();
				get_config_reply_rcvd(dynamic_cast<cofmsg_get_config_reply*>( msg ));
			} break;
			case OFPT12_PACKET_IN: {
				msg = new cofmsg_packet_in(mem);
				msg->validate();
				packet_in_rcvd(dynamic_cast<cofmsg_packet_in*>( msg ));
			} break;
			case OFPT12_FLOW_REMOVED: {
				msg = new cofmsg_flow_removed(mem);
				msg->validate();
				flow_rmvd_rcvd(dynamic_cast<cofmsg_flow_removed*>( msg ));
			} break;
			case OFPT12_PORT_STATUS: {
				msg = new cofmsg_port_status(mem);
				msg->validate();
				port_status_rcvd(dynamic_cast<cofmsg_port_status*>( msg ));
			} break;
			case OFPT12_STATS_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT12_STATS_REQUEST);
				uint16_t stats_type = 0;
				switch (ofh_header->version) {
				case OFP10_VERSION: {
					if (mem->memlen() < sizeof(struct ofp10_stats_reply)) {
						msg = new cofmsg(mem);
						throw eBadSyntaxTooShort();
					}
					stats_type = be16toh(((struct ofp10_stats_reply*)mem->somem())->type);
				} break;
				case OFP12_VERSION: {
					if (mem->memlen() < sizeof(struct ofp12_stats_reply)) {
						msg = new cofmsg(mem);
						throw eBadSyntaxTooShort();
					}
					stats_type = be16toh(((struct ofp12_stats_reply*)mem->somem())->type);
				} break;
				case OFP13_VERSION: {
					if (mem->memlen() < sizeof(struct ofp13_multipart_reply)) {
						msg = new cofmsg(mem);
						throw eBadSyntaxTooShort();
					}
					stats_type = be16toh(((struct ofp13_multipart_reply*)mem->somem())->type);
				} break;
				default:
					throw eBadVersion();
				}

				switch (stats_type) {
				case OFPST_DESC: {
					msg = new cofmsg_desc_stats_reply(mem);
				} break;
				case OFPST_FLOW: {
					msg = new cofmsg_flow_stats_reply(mem);
				} break;
				case OFPST_AGGREGATE: {
					msg = new cofmsg_aggr_stats_reply(mem);
				} break;
				case OFPST_TABLE: {
					msg = new cofmsg_table_stats_reply(mem);
				} break;
				case OFPST_PORT: {
					msg = new cofmsg_port_stats_reply(mem);
				} break;
				case OFPST_QUEUE: {
					msg = new cofmsg_queue_stats_reply(mem);
				} break;
				case OFPST_GROUP: {
					msg = new cofmsg_group_stats_reply(mem);
				} break;
				case OFPST_GROUP_DESC: {
					msg = new cofmsg_group_desc_stats_reply(mem);
				} break;
				case OFPST_GROUP_FEATURES: {
					msg = new cofmsg_group_features_stats_reply(mem);
				} break;
				// TODO: experimenter statistics
				default: {
					msg = new cofmsg_stats_reply(mem);
				} break;
				}

				msg->validate();
				stats_reply_rcvd(dynamic_cast<cofmsg_stats_reply*>( msg ));
			} break;
			case OFPT12_BARRIER_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT12_BARRIER_REQUEST);
				msg = new cofmsg_barrier_reply(mem);
				msg->validate();
				barrier_reply_rcvd(dynamic_cast<cofmsg_barrier_reply*>( msg ));
			} break;
			case OFPT12_QUEUE_GET_CONFIG_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT12_QUEUE_GET_CONFIG_REQUEST);
				msg = new cofmsg_queue_get_config_reply(mem);
				msg->validate();
				queue_get_config_reply_rcvd(dynamic_cast<cofmsg_queue_get_config_reply*>( msg ));
			} break;
			case OFPT12_ROLE_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT12_ROLE_REQUEST);
				msg = new cofmsg_role_reply(mem);
				msg->validate();
				role_reply_rcvd(dynamic_cast<cofmsg_role_reply*>( msg ));
			} break;
			case OFPT12_GET_ASYNC_REPLY: {
				rofbase->ta_validate(be32toh(ofh_header->xid), OFPT12_GET_ASYNC_REQUEST);
				msg = new cofmsg_get_async_config_reply(mem);
				msg->validate();
				get_async_config_reply_rcvd(dynamic_cast<cofmsg_get_async_config_reply*>( msg ));
			} break;
			default: {
				WRITELOG(COFDPT, WARN, "cofdpt(%p)::handle_message() "
						"dropping packet: %s", this, mem->c_str());
				delete mem;
			} return;
			}

		} break;
		default:
			throw eBadVersion();
		}




	} catch (eBadSyntaxTooShort& e) {

		writelog(COFCTL, WARN, "cofdpt(%p)::handle_message() "
				"invalid length value for reply received, msg: %s", this, mem->c_str());

		delete msg;
	} catch (eBadVersion& e) {

		writelog(COFCTL, WARN, "cofdpt(%p)::handle_message() "
				"ofp_header.version not supported, pack: %s", this, mem->c_str());

		delete msg;
	} catch (eRofBaseXidInval& e) {

		writelog(COFDPT, ERROR, "cofdpt(%p)::handle_message() "
				"message with invalid transaction id received, message: %s", this, mem->c_str());

		delete msg;
	}
}



void
cofdptImpl::send_message(
		cofmsg *msg)
{
	const uint8_t OFPT_HELLO = 0;

    if (not flags.test(COFDPT_FLAG_HELLO_RCVD) && (msg->get_type() != OFPT_HELLO))
    {
        WRITELOG(CFWD, DBG, "cofdpt(%p)::send_message() "
            "dropping message, as no HELLO rcvd from peer yet => pack: %s",
            this, msg->c_str());
        delete msg; return;
    }

    switch (msg->get_version()) {
    case OFP10_VERSION: {

    	switch (msg->get_type()) {
    	case OFPT10_HELLO: {
    		// do nothing here
    	} break;
    	case OFPT10_ECHO_REQUEST: {
    		echo_request_sent(msg);
    	} break;
    	case OFPT10_ECHO_REPLY: {
    		// do nothing here
    	} break;
    	case OFPT10_ERROR:
    	case OFPT10_VENDOR:
    	case OFPT10_SET_CONFIG:
    	case OFPT10_PACKET_OUT:
    	case OFPT10_FLOW_MOD:
    	case OFPT10_PORT_MOD: {
    		// asynchronous messages, no transaction => do nothing here
    	} break;
    	case OFPT10_FEATURES_REQUEST: {
    		features_request_sent(msg);
    	} break;
    	case OFPT10_GET_CONFIG_REQUEST: {
    		get_config_request_sent(msg);
    	} break;
    	case OFPT10_STATS_REQUEST: {
    		stats_request_sent(msg);
    	} break;
    	case OFPT10_BARRIER_REQUEST: {
    		barrier_request_sent(msg);
    	} break;
    	case OFPT10_QUEUE_GET_CONFIG_REQUEST: {
    		queue_get_config_request_sent(msg);
    	} break;
    	default: {
    		WRITELOG(COFDPT, WARN, "cofdpt(%p)::send_message() "
    				"dropping invalid packet: %s", this, msg->c_str());
    		delete msg;
    	} return;
    	}

    } break;
    case OFP12_VERSION: {

    	switch (msg->get_type()) {
    	case OFPT12_HELLO: {
    		// do nothing here
    	} break;
    	case OFPT12_ECHO_REQUEST: {
    		echo_request_sent(msg);
    	} break;
    	case OFPT12_ECHO_REPLY: {
    		// do nothing here
    	} break;
    	case OFPT12_ERROR:
    	case OFPT12_EXPERIMENTER:
    	case OFPT12_SET_CONFIG:
    	case OFPT12_PACKET_OUT:
    	case OFPT12_FLOW_MOD:
    	case OFPT12_GROUP_MOD:
    	case OFPT12_PORT_MOD:
    	case OFPT12_TABLE_MOD: {
    		// asynchronous messages, no transaction => do nothing here
    	} break;
    	case OFPT12_FEATURES_REQUEST: {
    		features_request_sent(msg);
    	} break;
    	case OFPT12_GET_CONFIG_REQUEST: {
    		get_config_request_sent(msg);
    	} break;
    	case OFPT12_STATS_REQUEST: {
    		stats_request_sent(msg);
    	} break;
    	case OFPT12_BARRIER_REQUEST: {
    		barrier_request_sent(msg);
    	} break;
    	case OFPT12_QUEUE_GET_CONFIG_REQUEST: {
    		queue_get_config_request_sent(msg);
    	} break;
    	case OFPT12_ROLE_REQUEST: {
    		role_request_sent(msg);
    	} break;
    	default: {
    		WRITELOG(COFDPT, WARN, "cofdpt(%p)::send_message() "
    				"dropping invalid packet: %s", this, msg->c_str());
    		delete msg;
    	} return;
    	}

    } break;
    case OFP13_VERSION: {

    	switch (msg->get_type()) {
    	case OFPT13_HELLO: {
    		// do nothing here
    	} break;
    	case OFPT13_ECHO_REQUEST: {
    		echo_request_sent(msg);
    	} break;
    	case OFPT13_ECHO_REPLY: {
    		// do nothing here
    	} break;
    	case OFPT13_ERROR:
    	case OFPT13_EXPERIMENTER:
    	case OFPT13_SET_CONFIG:
    	case OFPT13_PACKET_OUT:
    	case OFPT13_FLOW_MOD:
    	case OFPT13_GROUP_MOD:
    	case OFPT13_PORT_MOD:
    	case OFPT13_TABLE_MOD:
    	case OFPT13_SET_ASYNC: {
    		// asynchronous messages, no transaction => do nothing here
    	} break;
    	case OFPT13_FEATURES_REQUEST: {
    		features_request_sent(msg);
    	} break;
    	case OFPT13_GET_CONFIG_REQUEST: {
    		get_config_request_sent(msg);
    	} break;
    	case OFPT13_STATS_REQUEST: {
    		stats_request_sent(msg);
    	} break;
    	case OFPT13_BARRIER_REQUEST: {
    		barrier_request_sent(msg);
    	} break;
    	case OFPT13_QUEUE_GET_CONFIG_REQUEST: {
    		queue_get_config_request_sent(msg);
    	} break;
    	case OFPT13_ROLE_REQUEST: {
    		role_request_sent(msg);
    	} break;
    	case OFPT13_GET_ASYNC_REQUEST: {
    		get_async_config_request_sent(msg);
    	} break;
    	default: {
    		WRITELOG(COFDPT, WARN, "cofdpt(%p)::send_message() "
    				"dropping invalid packet: %s", this, msg->c_str());
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
cofdptImpl::handle_timeout(int opaque)
{
	switch (opaque) {
	case COFDPT_TIMER_SEND_HELLO: {
		rofbase->send_hello_message(this);

		flags.set(COFDPT_FLAG_HELLO_SENT);

		if (flags.test(COFDPT_FLAG_HELLO_RCVD))
		{
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
		WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_timeout() "
				"unknown timer event %d", this, opaque);
	} break;
	}
}



void
cofdptImpl::hello_rcvd(cofmsg_hello *msg)
{
	try {
		WRITELOG(COFRPC, DBG, "cofdpt(%p)::hello_rcvd() pack: %s", this, msg->c_str());

		// OpenFlow versions do not match, send error, close connection
		if (not rofbase->is_ofp_version_supported(msg->get_version()))
		{
			new_state(COFDPT_STATE_DISCONNECTED);

			// invalid OFP_VERSION
			char explanation[256];
			bzero(explanation, sizeof(explanation));
			snprintf(explanation, sizeof(explanation) - 1,
					"unsupported OF version (%d), supported version is (%d)",
					msg->get_version(), OFP12_VERSION);

			cofmsg_error *reply = new cofmsg_error(
								OFP12_VERSION,
								msg->get_xid(),
								OFPET_HELLO_FAILED,
								OFPHFC_INCOMPATIBLE,
								(uint8_t*) explanation, strlen(explanation));

			send_message_via_socket(reply); // circumvent ::send_message, as COFDPT_FLAG_HELLO_RCVD is not set

			handle_closed(socket, socket->sd);
		}
		else
		{
			ofp_version = msg->get_version();

			WRITELOG(COFRPC, DBG, "cofdpt(%p)::hello_rcvd() "
					"HELLO exchanged with peer entity and ofp version %d, attaching ...", this, ofp_version);

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

		writelog(CROFBASE, ERROR, "cofctl(%p)::hello_rcvd() "
				"No compatible version, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_HELLO_FAILED,
					OFPHFC_INCOMPATIBLE,
					msg->soframe(), msg->framelen());

		delete msg;
		handle_closed(socket, socket->sd);
	} catch (eHelloEperm& e) {

		writelog(CROFBASE, ERROR, "cofctl(%p)::hello_rcvd() "
				"Permissions error, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_HELLO_FAILED,
					OFPHFC_EPERM,
					msg->soframe(), msg->framelen());

		delete msg;
		handle_closed(socket, socket->sd);
	}
}



void
cofdptImpl::echo_request_sent(cofmsg *pack)
{
	reset_timer(COFDPT_TIMER_ECHO_REPLY, 5); // TODO: multiple concurrent echo-requests?
}



void
cofdptImpl::echo_request_rcvd(cofmsg_echo_request *msg)
{
	// send echo reply back including any appended data
	rofbase->send_echo_reply(this, msg->get_xid(), msg->get_body().somem(), msg->get_body().memlen());

	delete msg;

	/* Please note: we do not call a handler for echo-requests/replies in rofbase
	 * and take care of these liveness packets within cofctl and cofdpt
	 */
}



void
cofdptImpl::echo_reply_rcvd(cofmsg_echo_reply *msg)
{
	cancel_timer(COFDPT_TIMER_ECHO_REPLY);
	register_timer(COFDPT_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);

	delete msg;
}



void
cofdptImpl::handle_echo_reply_timeout()
{
        WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_echo_reply_timeout() ", this);

        // TODO: repeat ECHO request multiple times (should be configurable)

        socket->cclose();
        new_state(COFDPT_STATE_DISCONNECTED);
        if (dptflags.test(COFDPT_FLAG_ACTIVE_SOCKET))
        {
                try_to_connect(true);
        }
        rofbase->handle_dpt_close(this);
}



void
cofdptImpl::features_request_sent(
		cofmsg *pack)
{
	register_timer(COFDPT_TIMER_FEATURES_REPLY, features_reply_timeout /* seconds */);
}



void
cofdptImpl::features_reply_rcvd(
		cofmsg_features_reply *msg)
{
	try {
		cancel_timer(COFDPT_TIMER_FEATURES_REPLY);

		dpid 			= msg->get_dpid();
		n_buffers 		= msg->get_n_buffers();
		n_tables 		= msg->get_n_tables();
		capabilities 	= msg->get_capabilities();

		switch (ofp_version) {
		case OFP10_VERSION:
		case OFP12_VERSION: {
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
		case OFP13_VERSION: {
			// no ports in OpenFlow 1.3 in FeaturesRequest
		} break;
		}

		WRITELOG(COFDPT, DBG, "cofdpt(%p)::features_reply_rcvd() "
				"dpid:%"PRIu64" pack:%s",
				this, dpid, msg->c_str());

		WRITELOG(COFDPT, DBG, "cofdpt(%p)::features_reply_rcvd() %s", this, this->c_str());



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

		if (COFDPT_STATE_WAIT_FEATURES == cur_state())
		{
			// next step: send GET-CONFIG request to datapath
			rofbase->send_get_config_request(this);

			new_state(COFDPT_STATE_WAIT_GET_CONFIG);
		}


	} catch (eOFportMalformed& e) {

		WRITELOG(COFDPT, DBG, "exception: malformed FEATURES reply received");

		socket->cclose();

		new_state(COFDPT_STATE_DISCONNECTED);

		rofbase->handle_dpt_close(this);
	}
}




void
cofdptImpl::handle_features_reply_timeout()
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_features_reply_timeout() ", this);

	rofbase->handle_features_reply_timeout(this);
}



void
cofdptImpl::get_config_request_sent(
		cofmsg *pack)
{
	register_timer(COFDPT_TIMER_GET_CONFIG_REPLY, get_config_reply_timeout);
}



void
cofdptImpl::get_config_reply_rcvd(
		cofmsg_get_config_reply *msg)
{
	cancel_timer(COFDPT_TIMER_GET_CONFIG_REPLY);

	config = msg->get_flags();
	miss_send_len = msg->get_miss_send_len();

	WRITELOG(COFDPT, DBG, "cofdpt(%p)::get_config_reply_rcvd() "
			"dpid:%"PRIu64" ",
			this, dpid);

	rofbase->handle_get_config_reply(this, msg);

	if (COFDPT_STATE_WAIT_GET_CONFIG == cur_state())
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			new_state(COFDPT_STATE_CONNECTED);

			rofbase->handle_dpt_open(this);

		} break;
		case OFP12_VERSION:
		case OFP13_VERSION: {
			// send stats request during initialization
			rofbase->send_stats_request(this, OFPST_TABLE, 0);

			new_state(COFDPT_STATE_WAIT_TABLE_STATS);

		} break;
		}

	}
}



void
cofdptImpl::handle_get_config_reply_timeout()
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_get_config_reply_timeout() "
			"dpid:%"PRIu64" ",
			this, dpid);

	rofbase->handle_get_config_reply_timeout(this);
}



void
cofdptImpl::stats_request_sent(
		cofmsg *pack)
{
	try {
		switch (ofp_version) {
		case OFP10_VERSION: {
			xidstore[OFPT10_STATS_REQUEST].xid_add(this, pack->get_xid(), stats_reply_timeout);
		} break;
		case OFP12_VERSION: {
			xidstore[OFPT12_STATS_REQUEST].xid_add(this, pack->get_xid(), stats_reply_timeout);
		} break;
		case OFP13_VERSION: {
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
cofdptImpl::stats_reply_rcvd(
		cofmsg_stats_reply *msg)
{
	cancel_timer(COFDPT_TIMER_STATS_REPLY);

	switch (ofp_version) {
	case OFP10_VERSION: {
		xidstore[OFPT10_STATS_REQUEST].xid_rem(msg->get_xid());
	} break;
	case OFP12_VERSION: {
		xidstore[OFPT12_STATS_REQUEST].xid_rem(msg->get_xid());
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented(); // yet
	} break;
	default:
		throw eBadVersion();
	}

	WRITELOG(COFDPT, DBG, "cofdpt(%p)::stats_reply_rcvd() "
			"dpid:%"PRIu64" ",
			this, dpid);

	// msg->get_stats_reply() !!!
	switch (msg->get_stats_type()) {
	case OFPST_DESC: {
		rofbase->handle_desc_stats_reply(this, dynamic_cast<cofmsg_desc_stats_reply*>( msg ));
	} break;
	case OFPST_TABLE: {
		table_stats_reply_rcvd(dynamic_cast<cofmsg_table_stats_reply*>( msg ));
	} break;
	case OFPST_PORT: {
		rofbase->handle_port_stats_reply(this, dynamic_cast<cofmsg_port_stats_reply*>( msg ));
	} break;
	case OFPST_FLOW: {
		rofbase->handle_flow_stats_reply(this, dynamic_cast<cofmsg_flow_stats_reply*>( msg ));
	} break;
	case OFPST_AGGREGATE: {
		rofbase->handle_aggregate_stats_reply(this, dynamic_cast<cofmsg_aggr_stats_reply*>( msg ));
	} break;
	case OFPST_QUEUE: {
		rofbase->handle_queue_stats_reply(this, dynamic_cast<cofmsg_queue_stats_reply*>( msg ));
	} break;
	case OFPST_GROUP: {
		rofbase->handle_group_stats_reply(this, dynamic_cast<cofmsg_group_stats_reply*>( msg ));
	} break;
	case OFPST_GROUP_DESC: {
		rofbase->handle_group_desc_stats_reply(this, dynamic_cast<cofmsg_group_desc_stats_reply*>( msg ));
	} break;
	case OFPST_GROUP_FEATURES: {
		rofbase->handle_group_features_stats_reply(this, dynamic_cast<cofmsg_group_features_stats_reply*>( msg ));
	} break;
	case OFPST_EXPERIMENTER: {
		rofbase->handle_experimenter_stats_reply(this, dynamic_cast<cofmsg_experimenter_stats_reply*>( msg ));
	} break;
	default: {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::recv_stats_request() "
				"unknown stats request type (%d)",
				this, msg->get_type());
		rofbase->handle_stats_reply(this, dynamic_cast<cofmsg_stats*>( msg ));
	} break;
	}

	if (COFDPT_STATE_WAIT_TABLE_STATS == cur_state()) // enter state running during initialization
	{
		new_state(COFDPT_STATE_CONNECTED);

		rofbase->handle_dpt_open(this);
	}
}



void
cofdptImpl::handle_stats_reply_timeout()
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_stats_reply_timeout() "
			"dpid:%"PRIu64" ",
			this, dpid);

	uint8_t msg_type = 0;

	switch (ofp_version) {
	case OFP10_VERSION: msg_type = OFPT10_STATS_REQUEST; break;
	case OFP12_VERSION: msg_type = OFPT12_STATS_REQUEST; break;
	case OFP13_VERSION: msg_type = OFPT13_STATS_REQUEST; break;
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
			rofbase->handle_stats_reply_timeout(this, xidt.xid);

			xidstore[msg_type].xid_rem(xidt.xid);

			goto restart;
		}
	}

	if (not xidstore.empty())
	{
		reset_timer(COFDPT_TIMER_STATS_REPLY, stats_reply_timeout);
	}
}



void
cofdptImpl::table_stats_reply_rcvd(
		cofmsg_table_stats_reply *msg)
{
	// clear our old table map
	tables.clear();
	// iterate through all received table stats bodies and fill in our local tables map
	for (std::vector<coftable_stats_reply>::iterator
			it = msg->get_table_stats().begin(); it != msg->get_table_stats().end(); ++it) {
		coftable_stats_reply& table = (*it);
		tables[table.get_table_id()] = table;
	}

	rofbase->handle_table_stats_reply(this,  msg);
}



void
cofdptImpl::barrier_request_sent(
		cofmsg *pack)
{
	try {
		uint8_t msg_type = 0;

		switch (ofp_version) {
		case OFP10_VERSION: msg_type = OFPT10_BARRIER_REQUEST; break;
		case OFP12_VERSION: msg_type = OFPT12_BARRIER_REQUEST; break;
		case OFP13_VERSION: msg_type = OFPT13_BARRIER_REQUEST; break;
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
cofdptImpl::barrier_reply_rcvd(cofmsg_barrier_reply *msg)
{
	cancel_timer(COFDPT_TIMER_BARRIER_REPLY);

	uint8_t msg_type = 0;

	switch (ofp_version) {
	case OFP10_VERSION: msg_type = OFPT10_BARRIER_REQUEST; break;
	case OFP12_VERSION: msg_type = OFPT12_BARRIER_REQUEST; break;
	case OFP13_VERSION: msg_type = OFPT13_BARRIER_REQUEST; break;
	default:
		throw eBadVersion();
	}

	xidstore[msg_type].xid_rem(msg->get_xid());

	rofbase->handle_barrier_reply(this, msg);
}



void
cofdptImpl::handle_barrier_reply_timeout()
{
	uint8_t msg_type = 0;

	switch (ofp_version) {
	case OFP10_VERSION: msg_type = OFPT10_BARRIER_REQUEST; break;
	case OFP12_VERSION: msg_type = OFPT12_BARRIER_REQUEST; break;
	case OFP13_VERSION: msg_type = OFPT13_BARRIER_REQUEST; break;
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
cofdptImpl::flow_mod_sent(
		cofmsg *msg)
{
	try {
		cofmsg_flow_mod *flow_mod = dynamic_cast<cofmsg_flow_mod*>( msg );
		(void)flow_mod; 
		WRITELOG(COFDPT, DBG, "cofdpt(%p)::flow_mod_sent() table_id: %d", this, flow_mod->get_table_id());

	} catch (cerror& e) {
		WRITELOG(CFTTABLE, DBG, "unable to add ftentry to local flow_table instance");
	}
}



void
cofdptImpl::flow_rmvd_rcvd(
		cofmsg_flow_removed *msg)
{
	rofbase->handle_flow_removed(this, msg);
}



void
cofdptImpl::flow_mod_reset()
{
	cflowentry fe(ofp_version);
	fe.set_command(OFPFC_DELETE);
	fe.set_table_id(OFPTT_ALL /*all tables*/);

	rofbase->send_flow_mod_message(this, fe); // calls this->flow_mod_sent() implicitly
}



void
cofdptImpl::group_mod_sent(
		cofmsg *pack)
{

}



void
cofdptImpl::group_mod_reset()
{
	cgroupentry ge(get_version());
	ge.set_command(OFPGC_DELETE);
	ge.set_group_id(OFPG12_ALL);

	rofbase->send_group_mod_message(this, ge); // calls this->group_mod_sent() implicitly
}



void
cofdptImpl::table_mod_sent(cofmsg *pack)
{
	cofmsg_table_mod *table_mod = dynamic_cast<cofmsg_table_mod*>( pack );

	if (0 == table_mod) {
		return;
	}

	// TODO: adjust local flowtable
}


void
cofdptImpl::port_mod_sent(cofmsg *pack)
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
cofdptImpl::packet_in_rcvd(cofmsg_packet_in *msg)
{
	try {
		WRITELOG(COFDPT, DBG, "cofdpt(%p)::packet_in_rcvd() %s", this, msg->c_str());

#if 0
		// update forwarding table
		uint32_t in_port = msg->match.get_in_port();
#endif

		// datalen must be at least one Ethernet header in size
		if (msg->get_packet().length() >= (2 * OFP_ETH_ALEN + sizeof(uint16_t)))
		{
#if 0
			// update local forwarding table
			fwdtable.mac_learning(msg->packet, dpid, in_port);

			WRITELOG(COFDPT, DBG, "cofdpt(0x%llx)::packet_in_rcvd() local fwdtable: %s",
					dpid, fwdtable.c_str());
#endif


#if 0
		rofbase->fwdtable.mac_learning(ether, dpid, in_port);

		WRITELOG(COFDPT, DBG, "cofdpt(0x%llx)::packet_in_rcvd() global fwdtable: %s",
				dpid, rofbase->fwdtable.c_str());
#endif

			// let derived class handle PACKET-IN event
			rofbase->handle_packet_in(this, msg);
		}
	} catch (eOFmatchNotFound& e) {

		WRITELOG(COFDPT, DBG, "cofdpt(0x%llx)::packet_in_rcvd() "
				"no in-port specified in Packet-In message", dpid);
	}
}



void
cofdptImpl::port_status_rcvd(cofmsg_port_status *msg)
{
	WRITELOG(COFDPT, DBG, "cofdpt(0x%016llx)::port_status_rcvd() %s",
			dpid, msg->c_str());

	std::map<uint32_t, cofport*>::iterator it;
	switch (msg->get_reason()) {
	case OFPPR_ADD: {
		if (ports.find(msg->get_port().get_port_no()) == ports.end())
		{
			//cofport *lport = new cofport(&ports, be32toh(pack->ofh_port_status->desc.port_no), &(pack->ofh_port_status->desc), sizeof(struct ofp_port));
			new cofport(msg->get_port(), &ports, msg->get_port().get_port_no());

			// let derived class handle PORT-STATUS message
			//rofbase->handle_port_status(this, pack, lport);
			rofbase->handle_port_status(this, msg);
		}
	} break;
	case OFPPR_DELETE: {
		if (ports.find(msg->get_port().get_port_no()) != ports.end())
		{
			uint32_t port_no = msg->get_port().get_port_no();
			// let derived class handle PORT-STATUS message
			//rofbase->handle_port_status(this, pack, ports[port_no]);
			rofbase->handle_port_status(this, msg);

			// do not access pack here, as it was already deleted by rofbase->handle_port_status() !!!
			delete ports[port_no];

			ports.erase(port_no);
		}
	} break;
	case OFPPR_MODIFY: {
		if (ports.find(msg->get_port().get_port_no()) != ports.end())
		{
			*(ports[msg->get_port().get_port_no()]) = msg->get_port();

			// let derived class handle PORT-STATUS message
			//rofbase->handle_port_status(this, pack, ports[be32toh(pack->ofh_port_status->desc.port_no)]);
			rofbase->handle_port_status(this, msg);
		}
	} break;
	default: {
		delete msg;
	} break;
	}
}


void
cofdptImpl::fsp_open(cofmatch const& ofmatch)
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
cofdptImpl::fsp_close(cofmatch const& ofmatch)
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
cofdptImpl::experimenter_rcvd(cofmsg_experimenter *msg)
{
	switch (msg->get_experimenter_id()) {
	default:
		break;
	}

	// for now: send vendor extensions directly to class derived from crofbase
	rofbase->handle_experimenter_message(this, msg);
}



void
cofdptImpl::role_request_sent(
		cofmsg *pack)
{

}



void
cofdptImpl::role_reply_rcvd(cofmsg_role_reply *pack)
{
	rofbase->handle_role_reply(this, pack);
}


void
cofdptImpl::queue_get_config_request_sent(
		cofmsg *pack)
{
	// TODO
}



void
cofdptImpl::queue_get_config_reply_rcvd(
		cofmsg_queue_get_config_reply *pack)
{
	rofbase->handle_queue_get_config_reply(this, pack);
}


void
cofdptImpl::get_async_config_request_sent(
		cofmsg *pack)
{
	register_timer(COFDPT_TIMER_GET_ASYNC_CONFIG_REPLY, get_async_config_reply_timeout);
}



void
cofdptImpl::get_async_config_reply_rcvd(
		cofmsg_get_async_config_reply *msg)
{
	cancel_timer(COFDPT_TIMER_GET_ASYNC_CONFIG_REPLY);

	// TODO: store mask values into local variables?

	WRITELOG(COFDPT, DBG, "cofdpt(%p)::get_async_config_reply_rcvd() "
			"dpid:%"PRIu64" ",
			this, dpid);

	rofbase->handle_get_async_config_reply(this, msg);
}



void
cofdptImpl::handle_get_async_config_reply_timeout()
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_get_async_config_reply_timeout() "
			"dpid:%"PRIu64" ",
			this, dpid);

	rofbase->handle_get_async_config_reply_timeout(this);
}



const char*
cofdptImpl::c_str()
{
	cvastring vas;
	info.assign(vas("cofdpt(%p) dpid:0x%llx buffers: %d tables: %d capabilities: 0x%x =>",
			this, dpid, n_buffers, n_tables, capabilities));

	std::map<uint32_t, cofport*>::iterator it;
	for (it = ports.begin(); it != ports.end(); ++it)
	{
		info.append(vas("\n  %s", it->second->c_str()));
	}

	return info.c_str();
}



cofport*
cofdptImpl::find_cofport(
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
cofdptImpl::find_cofport(
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
cofdptImpl::find_cofport(
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
cofdptImpl::try_to_connect(bool reset_timeout)
{
	if (pending_timer(COFDPT_TIMER_RECONNECT)) {
		return;
	}

	WRITELOG(COFCTL, DBG, "cofdpt(%p)::try_to_connect() "
			"reconnect in %d seconds (reconnect_counter:%d)",
			this, reconnect_in_seconds, reconnect_counter);

	int max_backoff = 1 * reconnect_start_timeout;

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
cofdptImpl::send_message_via_socket(
		cofmsg *pack)
{
	if (0 == socket)
	{
		delete pack; return;
	}

	cmemory *mem = new cmemory(pack->length());

	pack->pack(mem->somem(), mem->memlen());

	WRITELOG(COFDPT, DBG, "cofdpt(%p): new cmemory: %s",
				this, mem->c_str());

	delete pack;

	socket->send_packet(mem);

	rofbase->wakeup(); // wake-up thread in case, we've been called from another thread
}


