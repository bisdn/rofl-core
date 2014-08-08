/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofctlImpl.h"

using namespace rofl;


cofctlImpl::cofctlImpl(
		crofbase *rofbase) :
				cofctl(rofbase),
				rofbase(rofbase),
				flags(0),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(OFP12CR_ROLE_EQUAL),
				cached_generation_id(0),
				socket(0),
				fragment(0),
				msg_bytes_read(0),
				reconnect_start_timeout(RECONNECT_START_TIMEOUT),
				reconnect_in_seconds(RECONNECT_START_TIMEOUT),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
				echo_reply_timeout(DEFAULT_ECHO_TIMEOUT),
				ofp_version(OFP12_VERSION)
{
	WRITELOG(CFWD, DBG, "cofctl(%p)::cofctl() TCP accept", this);

	register_timer(COFCTL_TIMER_SEND_HELLO, 0);
}



cofctlImpl::cofctlImpl(
		crofbase *rofbase,
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				cofctl(rofbase),
				rofbase(rofbase),
				flags(0),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(OFP12CR_ROLE_EQUAL),
				cached_generation_id(0),
				socket(new csocket(this, newsd, ra, domain, type, protocol)),
				fragment(0),
				msg_bytes_read(0),
				reconnect_start_timeout(0),
				reconnect_in_seconds(0),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
				echo_reply_timeout(DEFAULT_ECHO_TIMEOUT),
				ofp_version(OFP12_VERSION)
{
	WRITELOG(CFWD, DBG, "cofctl(%p)::cofctl() TCP accept", this);

	register_timer(COFCTL_TIMER_SEND_HELLO, 0);
}



cofctlImpl::cofctlImpl(
		crofbase *rofbase,
		uint8_t ofp_version,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				cofctl(rofbase),
				rofbase(rofbase),
				flags(COFCTL_FLAG_ACTIVE_SOCKET),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(OFP12CR_ROLE_EQUAL),
				cached_generation_id(0),
				socket(new csocket(this, domain, type, protocol)),
				fragment(0),
				msg_bytes_read(0),
				reconnect_start_timeout(reconnect_start_timeout),
				reconnect_in_seconds(reconnect_start_timeout),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
				echo_reply_timeout(DEFAULT_ECHO_TIMEOUT),
				ofp_version(ofp_version)
{
	WRITELOG(COFCTL, DBG, "cofctl(%p)::cofctl() TCP connect", this);

	flags.set(COFCTL_FLAG_ACTIVE_SOCKET);

	this->reconnect_in_seconds = this->reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;

	socket->cconnect(ra, caddress(AF_INET, "0.0.0.0"), domain, type, protocol);
}



cofctlImpl::~cofctlImpl()
{
	WRITELOG(COFCTL, DBG, "cofctl(%p)::~cofctl()", this);

	rofbase->fsptable.delete_fsp_entries(this);

	delete socket;
}



bool
cofctlImpl::is_established() const
{
	return (STATE_CTL_ESTABLISHED == cur_state());
}



bool
cofctlImpl::is_slave() const
{
	return (OFP12CR_ROLE_SLAVE == role);
}



uint8_t
cofctlImpl::get_version()
{
	return ofp_version;
}



caddress
cofctlImpl::get_peer_addr()
{
	return socket->raddr;
}



uint32_t
cofctlImpl::get_role() const
{
	return role;
}



void
cofctlImpl::set_role(uint32_t role)
{
	this->role = role;
}



void
cofctlImpl::send_message(
		cofmsg *pack)
{
	const uint8_t OFPT_HELLO = 0; // == OFPT10_HELLO == OFPT12_HELLO == OFPT13_HELLO

    if (not flags.test(COFCTL_FLAG_HELLO_RCVD) && (pack->get_type() != OFPT_HELLO))
    {
        WRITELOG(COFCTL, TRACE, "cofctrl(%p)::send_message() "
            "dropping message, as no HELLO rcvd from peer yet, pack: %s",
            this, pack->c_str());
        delete pack; return;
    }

    switch (pack->get_version()) {
    case OFP10_VERSION: {

        switch (pack->get_type()) {
        case OFPT10_HELLO:
        case OFPT10_ERROR: {
    		// ...
    	} break;
        case OFPT10_ECHO_REQUEST: {
    		echo_request_sent(pack);
    	} break;
        case OFPT10_ECHO_REPLY: {
        	// do nothing here
    	} break;
        case OFPT10_VENDOR: {
    		// ...
    	} break;
        case OFPT10_FEATURES_REPLY: {
    		features_reply_sent(pack);
    	} break;
        case OFPT10_GET_CONFIG_REPLY: {
    		get_config_reply_sent(pack);
    	} break;
        case OFPT10_PACKET_IN: {
    		// asynchronous ...
    	} break;
        case OFPT10_FLOW_REMOVED: {
    		// asynchronous ...
    	} break;
        case OFPT10_PORT_STATUS: {
    		// asynchronous ...
    	} break;
        case OFPT10_STATS_REPLY: {
    		stats_reply_sent(pack);
    	} break;
        case OFPT10_BARRIER_REPLY: {
    		barrier_reply_sent(pack);
    	} break;
        case OFPT10_QUEUE_GET_CONFIG_REPLY: {
    		queue_get_config_reply_sent(pack);
    	} break;
        default: {
    		WRITELOG(COFCTL, WARN, "cofctl(%p)::send_message() "
    						"dropping invalid packet: %s", this, pack->c_str());
    		delete pack;
    	} return;
        }

    } break;
    case OFP12_VERSION: {

        switch (pack->get_type()) {
        case OFPT12_HELLO:
        case OFPT12_ERROR: {
    		// ...
    	} break;
        case OFPT12_ECHO_REQUEST: {
    		echo_request_sent(pack);
    	} break;
        case OFPT12_ECHO_REPLY: {
        	// do nothing here
    	} break;
        case OFPT12_EXPERIMENTER: {
    		// ...
    	} break;
        case OFPT12_FEATURES_REPLY: {
    		features_reply_sent(pack);
    	} break;
        case OFPT12_GET_CONFIG_REPLY: {
    		get_config_reply_sent(pack);
    	} break;
        case OFPT12_PACKET_IN: {
    		// asynchronous ...
    	} break;
        case OFPT12_FLOW_REMOVED: {
    		// asynchronous ...
    	} break;
        case OFPT12_PORT_STATUS: {
    		// asynchronous ...
    	} break;
        case OFPT12_STATS_REPLY: {
    		stats_reply_sent(pack);
    	} break;
        case OFPT12_BARRIER_REPLY: {
    		barrier_reply_sent(pack);
    	} break;
        case OFPT12_QUEUE_GET_CONFIG_REPLY: {
    		queue_get_config_reply_sent(pack);
    	} break;
        case OFPT12_ROLE_REPLY: {
    		role_reply_sent(pack);
    	} break;
        default: {
    		WRITELOG(COFCTL, WARN, "cofctl(%p)::send_message() "
    						"dropping invalid packet: %s", this, pack->c_str());
    		delete pack;
    	} return;
        }


    } break;
    case OFP13_VERSION: {

        switch (pack->get_type()) {
        case OFPT13_HELLO:
        case OFPT13_ERROR: {
    		// ...
    	} break;
        case OFPT13_ECHO_REQUEST: {
    		echo_request_sent(pack);
    	} break;
        case OFPT13_ECHO_REPLY: {
        	// do nothing here
    	} break;
        case OFPT13_EXPERIMENTER: {
    		// ...
    	} break;
        case OFPT13_FEATURES_REPLY: {
    		features_reply_sent(pack);
    	} break;
        case OFPT13_GET_CONFIG_REPLY: {
    		get_config_reply_sent(pack);
    	} break;
        case OFPT13_PACKET_IN: {
    		// asynchronous ...
    	} break;
        case OFPT13_FLOW_REMOVED: {
    		// asynchronous ...
    	} break;
        case OFPT13_PORT_STATUS: {
    		// asynchronous ...
    	} break;
        case OFPT13_STATS_REPLY: {
    		stats_reply_sent(pack);
    	} break;
        case OFPT13_BARRIER_REPLY: {
    		barrier_reply_sent(pack);
    	} break;
        case OFPT13_QUEUE_GET_CONFIG_REPLY: {
    		queue_get_config_reply_sent(pack);
    	} break;
        case OFPT13_ROLE_REPLY: {
    		role_reply_sent(pack);
    	} break;
        case OFPT13_GET_ASYNC_REPLY: {
        	get_async_config_reply_sent(pack);
        } break;
        default: {
    		WRITELOG(COFCTL, WARN, "cofctl(%p)::send_message() "
    						"dropping invalid packet: %s", this, pack->c_str());
    		delete pack;
    	} return;
        }

    } break;
    default:
    	throw eBadVersion();
    }

	send_message_via_socket(pack);
}



void
cofctlImpl::handle_timeout(
		int opaque)
{
	switch (opaque) {
	case COFCTL_TIMER_SEND_HELLO:
		{
			rofbase->send_hello_message(this);

			flags.set(COFCTL_FLAG_HELLO_SENT);

			if (flags.test(COFCTL_FLAG_HELLO_RCVD))
			{
				rofbase->send_echo_request(this);

				rofbase->handle_ctl_open(this);
			}
		}
		break;
	case COFCTL_TIMER_RECONNECT:
		{
			if (socket)
			{
				new_state(STATE_CTL_CONNECTING);

				socket->cconnect(socket->raddr,
						caddress(AF_INET, "0.0.0.0"),
						socket->domain,
						socket->type,
						socket->protocol);
			}
		}
		break;
	case COFCTL_TIMER_SEND_ECHO_REQUEST:
		{
			rofbase->send_echo_request(this);
		}
		break;
	case COFCTL_TIMER_ECHO_REPLY_TIMEOUT:
		{
			handle_echo_reply_timeout();
		}
		break;
	}
}



void
cofctlImpl::handle_accepted(
		csocket *socket,
		int newsd,
		caddress const& ra)
{
	caddress raddr(ra);
	writelog(COFCTL, DBG, "A:ctl[%s] ", raddr.c_str());
	writelog(COFCTL, WARN, "cofctl(%p)::handle_accepted() "
			"local:%s remote: %s",
			this, socket->laddr.c_str(), raddr.c_str());

	// do nothing
}



void
cofctlImpl::handle_connected(
		csocket *socket,
		int sd)
{
	writelog(COFCTL, DBG, "C:ctl[%s] ", socket->raddr.c_str());
	writelog(COFCTL, WARN, "cofctl(%p)::handle_connected() "
			"local:%s remote: %s",
			this, socket->laddr.c_str(), socket->raddr.c_str());

	register_timer(COFCTL_TIMER_SEND_HELLO, 0);
}



void
cofctlImpl::handle_connect_refused(
		csocket *socket,
		int sd)
{
	writelog(COFCTL, WARN, "cofctl(%p)::handle_connect_refused() "
			"local:%s remote: %s",
			this, socket->laddr.c_str(), socket->raddr.c_str());

	// TODO: signal event back to rofbase
	if (flags.test(COFCTL_FLAG_ACTIVE_SOCKET))
	{
		try_to_connect();
	}
}



void
cofctlImpl::handle_read(
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

			if (rc < 0) // error occured (or non-blocking)
			{
				switch(errno) {
				case EAGAIN: {
					fragment = mem;	// more bytes are needed, store pointer to msg in "fragment"
				} return;
				case ECONNREFUSED: {
					try_to_connect(); // reconnect
				} return;
				case ECONNRESET:
				default: {
					writelog(COFCTL, WARN, "cofctl(%p)::handle_read() "
							"an error occured, closing => errno: %d (%s)",
							this, errno, strerror(errno));
					handle_closed(socket, sd);
				} return;
				}
			}
			else if (rc == 0) // socket was closed
			{
				//rfds.erase(fd);

				writelog(COFCTL, INFO, "cofctl(%p)::handle_read() "
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

		writelog(COFCTL, WARN, "cofctl(%p)::handle_read() "
				"invalid packet received, dropping. Closing socket. Packet: %s",
				this, mem->c_str());
		if (mem) {
			delete mem; fragment = (cmemory*)0;
		}
		handle_closed(socket, sd);
	}

}



void
cofctlImpl::handle_closed(
		csocket *socket,
		int sd)
{
	writelog(COFCTL, WARN, "cofctl(%p)::handle_closed() "
			"local:%s remote: %s",
			this, socket->laddr.c_str(), socket->raddr.c_str());

	socket->cclose();

	cancel_timer(COFCTL_TIMER_ECHO_REPLY_TIMEOUT);
	cancel_timer(COFCTL_TIMER_SEND_ECHO_REQUEST);

	new_state(STATE_CTL_DISCONNECTED);

	flags.reset(COFCTL_FLAG_HELLO_RCVD);
	flags.reset(COFCTL_FLAG_HELLO_SENT);

	if (flags.test(COFCTL_FLAG_ACTIVE_SOCKET))
	{
		try_to_connect(true);
	}
	else
	{
		rofbase->handle_ctl_close(this);
	}
}



void
cofctlImpl::handle_message(
		cmemory *mem)
{
	cofmsg *msg = (cofmsg*)0;
	uint32_t xid = 0;
	try {

		if (0 == mem) {
			writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
					"assert(msg != 0) failed", this);
			return;
		}

		WRITELOG(COFCTL, DBG, "receive: %s\n", mem->c_str());

		struct ofp_header* ofh_header = (struct ofp_header*)mem->somem();
		xid = be32toh(ofh_header->xid);

		const uint8_t OFPT_HELLO = 0; // == OFPT10_HELLO == OFPT12_HELLO == OFPT13_HELLO

		if (not flags.test(COFCTL_FLAG_HELLO_RCVD) && (OFPT_HELLO != ofh_header->type)) {
			writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
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
			case OFPT10_FEATURES_REQUEST:	{
				msg = new cofmsg_features_request(mem);
				msg->validate();
				features_request_rcvd(dynamic_cast<cofmsg_features_request*>( msg ));
			} break;
			case OFPT10_GET_CONFIG_REQUEST: {
				msg = new cofmsg_get_config_request(mem);
				msg->validate();
				get_config_request_rcvd(dynamic_cast<cofmsg_get_config_request*>( msg ));
			} break;
			case OFPT10_SET_CONFIG: {
				msg = new cofmsg_set_config(mem);
				msg->validate();
				set_config_rcvd(dynamic_cast<cofmsg_set_config*>( msg ));
			} break;
			case OFPT10_PACKET_OUT: {
				msg = new cofmsg_packet_out(mem);
				msg->validate();
				packet_out_rcvd(dynamic_cast<cofmsg_packet_out*>( msg ));
			} break;
			case OFPT10_FLOW_MOD: {
				msg = new cofmsg_flow_mod(mem);
				msg->validate();
				flow_mod_rcvd(dynamic_cast<cofmsg_flow_mod*>( msg ));
			} break;
			case OFPT10_PORT_MOD: {
				msg = new cofmsg_port_mod(mem);
				msg->validate();
				port_mod_rcvd(dynamic_cast<cofmsg_port_mod*>( msg ));
			} break;
			case OFPT10_STATS_REQUEST: {
				uint16_t stats_type = 0;
				switch (ofh_header->version) {
				case OFP10_VERSION: {
					if (mem->memlen() < sizeof(struct ofp10_stats_request)) {
						msg = new cofmsg(mem);
						throw eBadSyntaxTooShort();
					}
					stats_type = be16toh(((struct ofp10_stats_request*)mem->somem())->type);
				} break;
				default:
					throw eBadVersion();
				}

				switch (stats_type) {
				case OFPST_DESC: {
					msg = new cofmsg_desc_stats_request(mem);
				} break;
				case OFPST_FLOW: {
					msg = new cofmsg_flow_stats_request(mem);
				} break;
				case OFPST_AGGREGATE: {
					msg = new cofmsg_aggr_stats_request(mem);
				} break;
				case OFPST_TABLE: {
					msg = new cofmsg_table_stats_request(mem);
				} break;
				case OFPST_PORT: {
					msg = new cofmsg_port_stats_request(mem);
				} break;
				case OFPST_QUEUE: {
					msg = new cofmsg_queue_stats_request(mem);
				} break;
				// TODO: experimenter statistics
				default: {
					msg = new cofmsg_stats_request(mem);
				} break;
				}

				msg->validate();
				stats_request_rcvd(dynamic_cast<cofmsg_stats_request*>( msg ));
			} break;
			case OFPT10_BARRIER_REQUEST: {
				msg = new cofmsg_barrier_request(mem);
				msg->validate();
				barrier_request_rcvd(dynamic_cast<cofmsg_barrier_request*>( msg ));
			} break;
			case OFPT10_QUEUE_GET_CONFIG_REQUEST: {
				msg = new cofmsg_queue_get_config_request(mem);
				msg->validate();
				queue_get_config_request_rcvd(dynamic_cast<cofmsg_queue_get_config_request*>( msg ));
			} break;
			default: {
				WRITELOG(COFCTL, ERROR, "cofctl(%p)::handle_message() "
						"dropping unknown packet: %s", this, mem->c_str());
				msg = new cofmsg(mem);
				msg->validate();
				throw eBadRequestBadType();
			} break;
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
			case OFPT12_FEATURES_REQUEST:	{
				msg = new cofmsg_features_request(mem);
				msg->validate();
				features_request_rcvd(dynamic_cast<cofmsg_features_request*>( msg ));
			} break;
			case OFPT12_GET_CONFIG_REQUEST: {
				msg = new cofmsg_get_config_request(mem);
				msg->validate();
				get_config_request_rcvd(dynamic_cast<cofmsg_get_config_request*>( msg ));
			} break;
			case OFPT12_SET_CONFIG: {
				msg = new cofmsg_set_config(mem);
				msg->validate();
				set_config_rcvd(dynamic_cast<cofmsg_set_config*>( msg ));
			} break;
			case OFPT12_PACKET_OUT: {
				msg = new cofmsg_packet_out(mem);
				msg->validate();
				packet_out_rcvd(dynamic_cast<cofmsg_packet_out*>( msg ));
			} break;
			case OFPT12_FLOW_MOD: {
				msg = new cofmsg_flow_mod(mem);
				msg->validate();
				flow_mod_rcvd(dynamic_cast<cofmsg_flow_mod*>( msg ));
			} break;
			case OFPT12_GROUP_MOD: {
				msg = new cofmsg_group_mod(mem);
				msg->validate();
				group_mod_rcvd(dynamic_cast<cofmsg_group_mod*>( msg ));
			} break;
			case OFPT12_PORT_MOD: {
				msg = new cofmsg_port_mod(mem);
				msg->validate();
				port_mod_rcvd(dynamic_cast<cofmsg_port_mod*>( msg ));
			} break;
			case OFPT12_TABLE_MOD: {
				msg = new cofmsg_table_mod(mem);
				msg->validate();
				table_mod_rcvd(dynamic_cast<cofmsg_table_mod*>( msg ));
			} break;
			case OFPT12_STATS_REQUEST: {
				uint16_t stats_type = 0;
				switch (ofh_header->version) {
				case OFP12_VERSION: {
					if (mem->memlen() < sizeof(struct ofp12_stats_request)) {
						msg = new cofmsg(mem);
						throw eBadSyntaxTooShort();
					}
					stats_type = be16toh(((struct ofp12_stats_request*)mem->somem())->type);
				} break;
				case OFP13_VERSION: {
					if (mem->memlen() < sizeof(struct ofp13_multipart_request)) {
						msg = new cofmsg(mem);
						throw eBadSyntaxTooShort();
					}
					stats_type = be16toh(((struct ofp13_multipart_request*)mem->somem())->type);
				} break;
				default:
					throw eBadVersion();
				}

				switch (stats_type) {
				case OFPST_DESC: {
					msg = new cofmsg_desc_stats_request(mem);
				} break;
				case OFPST_FLOW: {
					msg = new cofmsg_flow_stats_request(mem);
				} break;
				case OFPST_AGGREGATE: {
					msg = new cofmsg_aggr_stats_request(mem);
				} break;
				case OFPST_TABLE: {
					msg = new cofmsg_table_stats_request(mem);
				} break;
				case OFPST_PORT: {
					msg = new cofmsg_port_stats_request(mem);
				} break;
				case OFPST_QUEUE: {
					msg = new cofmsg_queue_stats_request(mem);
				} break;
				case OFPST_GROUP: {
					msg = new cofmsg_group_stats_request(mem);
				} break;
				case OFPST_GROUP_DESC: {
					msg = new cofmsg_group_desc_stats_request(mem);
				} break;
				case OFPST_GROUP_FEATURES: {
					msg = new cofmsg_group_features_stats_request(mem);
				} break;
				// TODO: experimenter statistics
				default: {
					msg = new cofmsg_stats_request(mem);
				} break;
				}

				msg->validate();
				stats_request_rcvd(dynamic_cast<cofmsg_stats_request*>( msg ));
			} break;
			case OFPT12_BARRIER_REQUEST: {
				msg = new cofmsg_barrier_request(mem);
				msg->validate();
				barrier_request_rcvd(dynamic_cast<cofmsg_barrier_request*>( msg ));
			} break;
			case OFPT12_QUEUE_GET_CONFIG_REQUEST: {
				msg = new cofmsg_queue_get_config_request(mem);
				msg->validate();
				queue_get_config_request_rcvd(dynamic_cast<cofmsg_queue_get_config_request*>( msg ));
			} break;
			case OFPT12_ROLE_REQUEST: {
				msg = new cofmsg_role_request(mem);
				msg->validate();
				role_request_rcvd(dynamic_cast<cofmsg_role_request*>( msg ));
			} break;
		    case OFPT12_GET_ASYNC_REQUEST: {
		    	msg = new cofmsg_get_async_config_request(mem);
		    	msg->validate();
		    	get_async_config_request_rcvd(dynamic_cast<cofmsg_get_async_config_request*>( msg ));
		    } break;
		    case OFPT12_SET_ASYNC: {
		    	msg = new cofmsg_set_async_config(mem);
		    	msg->validate();
		    	set_async_config_rcvd(dynamic_cast<cofmsg_set_async_config*>( msg ));
		    } break;
			default: {
				WRITELOG(COFCTL, ERROR, "cofctl(%p)::handle_message() "
						"dropping unknown packet: %s", this, mem->c_str());
				msg = new cofmsg(mem);
				msg->validate();
				throw eBadRequestBadType();
			} break;;
			}


		} break;
		default: {

			switch (ofh_header->type) {
			case OFPT10_HELLO /* == OFPT12_HELLO == OFPT13_HELLO */: {
				msg = new cofmsg_hello(mem);
				msg->validate();
				hello_rcvd(dynamic_cast<cofmsg_hello*>( msg ));
			} break;
			default: {
				WRITELOG(COFCTL, ERROR, "cofctl(%p)::handle_message() "
						"dropping unknown packet: %s", this, mem->c_str());
				msg = new cofmsg(mem);
				msg->validate();
				throw eBadRequestBadType();
			} return;
			}


		} break;
		}


	} catch (eBadSyntaxTooShort& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Wrong request length for type, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_LEN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadVersion& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"ofp_header.version not supported, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_VERSION,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadVersion& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"ofp_header.version not supported, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_VERSION,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"ofp_header.type not supported, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadStat& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"ofp_stats_request.type not supported, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_STAT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadExperimenter& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Experimenter id not supported, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_EXPERIMENTER,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadExpType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Experimenter type not supported, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_EXP_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestEperm& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Permissions error, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_EPERM,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadLen& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Wrong request length for type, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_LEN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBufferEmpty& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Specified buffer has already been used, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BUFFER_EMPTY,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBufferUnknown& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Specified buffer does not exist, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BUFFER_UNKNOWN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadTableId& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Specified table-id invalid or does not exist, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_TABLE_ID,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestIsSlave& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Denied because controller is slave, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_IS_SLAVE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadPort& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Invalid port, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_PORT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadPacket& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Invalid packet in packet-out, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_PACKET,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"base class of exception eBadRequestBase caught, pack: %s", this, mem->c_str());

		delete msg;
	} catch (eBadActionBadType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unknown action type, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadLen& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Length problem in actions, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_LEN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadExperimenter& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unknown experimenter id specified, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_EXPERIMENTER,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadExperimenterType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unknown action type for experimenter id, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_EXPERIMENTER_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadOutPort& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Problem validating output port, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_OUT_PORT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadArgument& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Bad action argument, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_ARGUMENT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionEperm& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Permissions error, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_EPERM,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionTooMany& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Can't handle this many actions, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_TOO_MANY,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadQueue& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Problem validating output queue, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_QUEUE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadOutGroup& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Invalid group id in forward action, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_OUT_GROUP,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionMatchInconsistent& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Action can't apply for this match, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_MATCH_INCONSISTENT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionUnsupportedOrder& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Action order is unsupported for the action list in an Apply-Actions instruction, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_UNSUPPORTED_ORDER,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadTag& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Actions uses an unsupported tag/encap, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_TAG,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"base class of exception eBadActionBase caught, pack: %s", this, mem->c_str());

		delete msg;
	} catch (eBadInstUnknownInst& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unknown instruction, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNKNOWN_INST,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstUnsupInst& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Switch or table does not support the instruction, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_INST,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstBadTableId& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Invalid Table-ID specified, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_BAD_TABLE_ID,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstUnsupMetadata& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Metadata value unsupported by datapath, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_METADATA,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstUnsupMetadataMask& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Metadata mask value unsupported by datapath, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_METADATA_MASK,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstUnsupExpInst& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Specific experimenter instruction unsupported, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_EXP_INST,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"base class of exception eBadInstBase caught, pack: %s", this, mem->c_str());

		delete msg;
	} catch (eBadMatchBadType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported match type specified by the match, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadLen& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Length problem in match, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_LEN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadTag& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Match uses an unsupported tag/encap, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_TAG,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadDlAddrMask& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported datalink addr mask, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_DL_ADDR_MASK,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadNwAddrMask& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported network addr mask, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_NW_ADDR_MASK,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadWildcards& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported wildcard specified in the match, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_WILDCARDS,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadField& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported field in the match, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_FIELD,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadValue& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported value in a match field, pack: %s", this, mem->c_str());

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_VALUE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"base class of exception eBadMatchBase caught, pack: %s", this, mem->c_str());

		delete msg;
	}
}




void
cofctlImpl::hello_rcvd(cofmsg_hello *msg)
{
	try {
		WRITELOG(COFRPC, DBG, "cofctl(%p)::hello_rcvd() hello: %s", this, msg->c_str());

		// OpenFlow versions not supported on our side, send error, close connection
		if (not rofbase->is_ofp_version_supported(msg->get_version()))
		{
			new_state(STATE_CTL_DISCONNECTED);

			// invalid OFP_VERSION
			char explanation[256];
			bzero(explanation, sizeof(explanation));
			snprintf(explanation, sizeof(explanation) - 1,
							"unsupported OF version (%d), supported version is (%d)",
							(msg->get_version()), OFP12_VERSION);

			throw eHelloIncompatible();
		}
		else
		{
			// TODO: determine properly version from hello elements in OpenFlow 1.3
			ofp_version = msg->get_version();

			flags.set(COFCTL_FLAG_HELLO_RCVD);

			new_state(STATE_CTL_ESTABLISHED);

			WRITELOG(COFRPC, DBG, "cofctl(%p)::hello_rcvd() "
				"HELLO exchanged with peer entity and ofp version %d, attaching ...", this, ofp_version);

			if (flags.test(COFCTL_FLAG_HELLO_SENT))
			{
				register_timer(COFCTL_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);

				rofbase->handle_ctl_open(this);
			}
		}

		delete msg;

	} catch (eHelloIncompatible& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::hello_rcvd() "
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

		writelog(COFCTL, WARN, "cofctl(%p)::hello_rcvd() "
				"Permissions error, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_HELLO_FAILED,
					OFPHFC_EPERM,
					msg->soframe(), msg->framelen());

		delete msg;
		handle_closed(socket, socket->sd);
	} catch (eHelloBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::hello_rcvd() "
				"base class of exception eHelloBase caught, pack: %s", this, msg->c_str());

		delete msg;
	}
}



void
cofctlImpl::echo_request_sent(cofmsg *pack)
{
	reset_timer(COFCTL_TIMER_ECHO_REPLY_TIMEOUT, echo_reply_timeout); // TODO: multiple concurrent echo-requests?
}



void
cofctlImpl::echo_request_rcvd(cofmsg_echo_request *msg)
{
	// send echo reply back including any appended data
	rofbase->send_echo_reply(this, msg->get_xid(), msg->get_body().somem(), msg->get_body().memlen());

	delete msg;

	/* Please note: we do not call a handler for echo-requests/replies in rofbase
	 * and take care of these liveness packets within cofctl and cofdpt
	 */
}



void
cofctlImpl::echo_reply_rcvd(cofmsg_echo_reply *msg)
{
	cancel_timer(COFCTL_TIMER_ECHO_REPLY_TIMEOUT);
	register_timer(COFCTL_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);

	delete msg;
}



void
cofctlImpl::features_request_rcvd(cofmsg_features_request *msg)
{
	try {
		xidstore.xid_add(this, msg->get_xid(), 0);

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, ERROR, "cofctl(%p)::features_request_rcvd() retransmission", this);
	}

	rofbase->handle_features_request(this, msg);
}



void
cofctlImpl::features_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		WRITELOG(COFCTL, ERROR, "cofctl(%p)::features_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}



void
cofctlImpl::get_config_request_rcvd(cofmsg_get_config_request *msg)
{
	if (OFP12CR_ROLE_SLAVE == role) {
		send_error_is_slave(msg); return;
	}

	rofbase->handle_get_config_request(this, msg);
}



void
cofctlImpl::get_config_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		WRITELOG(COFCTL, ERROR, "cofctl(%p)::get_config_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}



void
cofctlImpl::set_config_rcvd(cofmsg_set_config *msg)
{
	try {
		if (OFP12CR_ROLE_SLAVE == role) {
			send_error_is_slave(msg); return;
		}

		rofbase->handle_set_config(this, msg);

	} catch (eSwitchConfigBadFlags& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::set_config_rcvd() "
				"Specified flags is invalid, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_SWITCH_CONFIG_FAILED,
					OFPSCFC_BAD_FLAGS,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eSwitchConfigBadLen& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::set_config_rcvd() "
				"Specified len is invalid, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_SWITCH_CONFIG_FAILED,
					OFPSCFC_BAD_LEN,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eSwitchConfigBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::set_config_rcvd() "
				"base class of exception eSwitchConfigBase caught, pack: %s", this, msg->c_str());

		delete msg;
	}
}



void
cofctlImpl::packet_out_rcvd(cofmsg_packet_out *msg)
{
	if (OFP12CR_ROLE_SLAVE == role) {
		send_error_is_slave(msg); return;
	}

	rofbase->handle_packet_out(this, msg);
}



void
cofctlImpl::flow_mod_rcvd(cofmsg_flow_mod *msg)
{
	WRITELOG(COFCTL, DBG, "cofctl(%p)::flow_mod_rcvd() pack: %s", this, msg->c_str());

	try {
		if (OFP12CR_ROLE_SLAVE == role) {
			send_error_is_slave(msg); return;
		}

		// check, whether the controlling pack->entity is allowed to install this flow-mod
		if (rofbase->fe_flags.test(crofbase::NSP_ENABLED))
		{
			switch (msg->get_command()) {
			case OFPFC_ADD:
			case OFPFC_MODIFY:
			case OFPFC_MODIFY_STRICT:
				/*
				 * deactivated for Darmstadt demonstration, needs exact fix
				 */
#if 0
				rofbase->fsptable.flow_mod_allowed(this, msg->match);
#endif
				break;
				/*
				 * this allows generic DELETE commands to be applied
				 * FIXME: does this affect somehow entries from other controllers?
				 */
			}
		}


		rofbase->handle_flow_mod(this, msg);

	} catch (eFlowModUnknown& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"unspecified error, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_UNKNOWN,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModTableFull& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"table full, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_TABLE_FULL,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModBadTableId& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"bad table-id, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_BAD_TABLE_ID,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModOverlap& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"flow-mod overlaps while CHECK_OVERLAP flag is set, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_OVERLAP,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModEperm& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"permissions error, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_EPERM,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModBadTimeout&e ) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"bad timeout value, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_BAD_TIMEOUT,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModBadCommand& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"bad command value, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_BAD_COMMAND,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::flow_mod_rcvd() "
				"base class of exception eFlowModBase caught, pack: %s",
				this, msg->c_str());

		delete msg;
	} catch (eFspNotAllowed& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"-FLOW-MOD- blocked due to mismatch in flowspace "
				"registration, match: %s\nflowspace-table: %s",
				this, msg->get_match().c_str(), rofbase->fsptable.c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_EPERM,
				msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRofBaseTableNotFound& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"invalid table-id %d specified",
				this, msg->get_table_id());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_BAD_TABLE_ID,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eInstructionInvalType& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"unknown instruction found", this);

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNKNOWN_INST,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRofBaseGotoTableNotFound& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"GOTO-TABLE instruction with invalid table-id", this);

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_BAD_TABLE_ID,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eInstructionBadExperimenter& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"unknown OFPIT_EXPERIMENTER extension received", this);

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNSUP_EXP_INST,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eOFmatchInvalBadValue& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"bad value in match structure", this);

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_BAD_MATCH,
				OFPBMC_BAD_VALUE,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (cerror &e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"default catch for cerror exception", this);

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_UNKNOWN,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	}
}



void
cofctlImpl::group_mod_rcvd(cofmsg_group_mod *msg)
{
	try {

		if (OFP12CR_ROLE_SLAVE == role) {
			send_error_is_slave(msg); return;
		}

		rofbase->handle_group_mod(this, msg);

	} catch (eGroupModExists& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group-id already exists, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_GROUP_EXISTS,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModInvalGroup& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group specified is invalid, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_INVALID_GROUP,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModWeightUnsupported& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group weight unsupported, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_WEIGHT_UNSUPPORTED,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModOutOfGroups& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"out of groups, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_OUT_OF_GROUPS,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModOutOfBuckets& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"out of buckets, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_OUT_OF_BUCKETS,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModChainingUnsupported& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"chaining unsupported, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_CHAINING_UNSUPPORTED,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModWatchUnsupported& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group watch unsupported, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_WATCH_UNSUPPORTED,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModLoop& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group would cause a loop, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_LOOP,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModUnknownGroup& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group-modify for non-existing group attempted, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_UNKNOWN_GROUP,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModChainedGroup& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group not deleted, as another group is forwarding to it, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_CHAINED_GROUP,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBadType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"unsupported or unknown group type, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_TYPE,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBadCommand& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"unsupported or unknown command, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_COMMAND,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBadBucket& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"error in bucket, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_BUCKET,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBadWatch& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"error in watch port/group, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_WATCH,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModEperm& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"permissions error, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_EPERM,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"base class of exception eGroupModBase caught, pack: %s", this, msg->c_str());

		delete msg;
	}
}



void
cofctlImpl::port_mod_rcvd(cofmsg_port_mod *msg)
{
	try {
		if (OFP12CR_ROLE_SLAVE == role) {
			send_error_is_slave(msg); return;
		}

		rofbase->handle_port_mod(this, msg);

	} catch (ePortModBadPort& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"Specified port number does not exist, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_PORT,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (ePortModBadHwAddr& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"Specified hardware address does not match the port number, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_HW_ADDR,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (ePortModBadConfig& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"Specified config is invalid, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_CONFIG,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (ePortModBadAdvertise& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"Specified advertise is invalid, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_ADVERTISE,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (ePortModBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"base class of exception ePortModBase caught, pack: %s", this, msg->c_str());

		delete msg;
	}
}


void
cofctlImpl::table_mod_rcvd(cofmsg_table_mod *pack)
{
	try {
		if (OFP12CR_ROLE_SLAVE == role) {
			send_error_is_slave(pack); return;
		}

		rofbase->handle_table_mod(this, pack);

	} catch (eTableModBadTable& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::table_mod_rcvd() "
				"Specified table does not exist, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_TABLE_MOD_FAILED,
				OFPTMFC_BAD_TABLE,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (eTableModBadConfig& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::table_mod_rcvd() "
				"Specified config is invalid, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_TABLE_MOD_FAILED,
				OFPTMFC_BAD_CONFIG,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (eTableModBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::table_mod_rcvd() "
				"base class of exception eTableModBase caught, pack: %s", this, pack->c_str());

		delete pack;
	}
}



void
cofctlImpl::stats_request_rcvd(cofmsg_stats *msg)
{
	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::stats_request_rcvd() "
				"retransmission xid:0x%x", this, msg->get_xid());
	}

	switch (msg->get_stats_type()) {
	case OFPST_DESC: {
		rofbase->handle_desc_stats_request(this, dynamic_cast<cofmsg_desc_stats_request*>( msg ));
	} break;
	case OFPST_TABLE: {
		rofbase->handle_table_stats_request(this, dynamic_cast<cofmsg_table_stats_request*>( msg ));
	} break;
	case OFPST_PORT: {
		rofbase->handle_port_stats_request(this, dynamic_cast<cofmsg_port_stats_request*>( msg ));
	} break;
	case OFPST_FLOW: {
		rofbase->handle_flow_stats_request(this, dynamic_cast<cofmsg_flow_stats_request*>( msg ));
	} break;
	case OFPST_AGGREGATE: {
		rofbase->handle_aggregate_stats_request(this, dynamic_cast<cofmsg_aggr_stats_request*>( msg ));
	} break;
	case OFPST_QUEUE: {
		rofbase->handle_queue_stats_request(this, dynamic_cast<cofmsg_queue_stats_request*>( msg ));
	} break;
	case OFPST_GROUP: {
		rofbase->handle_group_stats_request(this, dynamic_cast<cofmsg_group_stats_request*>( msg ));
	} break;
	case OFPST_GROUP_DESC: {
		rofbase->handle_group_desc_stats_request(this, dynamic_cast<cofmsg_group_desc_stats_request*>( msg ));
	} break;
	case OFPST_GROUP_FEATURES: {
		rofbase->handle_group_features_stats_request(this, dynamic_cast<cofmsg_group_features_stats_request*>( msg ));
	} break;
	case OFPST_EXPERIMENTER: {
		rofbase->handle_experimenter_stats_request(this, dynamic_cast<cofmsg_experimenter_stats_request*>( msg ));
	} break;
	default: {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::recv_stats_request() "
				"unknown stats request type (%d)",
				this, msg->get_type());
		rofbase->handle_stats_request(this, dynamic_cast<cofmsg_stats*>( msg ));
	} break;
	}
}



void
cofctlImpl::stats_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::stats_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}



void
cofctlImpl::role_request_rcvd(cofmsg_role_request *msg)
{
	try {
		try {
			xidstore.xid_add(this, msg->get_xid());

		} catch (eXidStoreXidBusy& e) {
			WRITELOG(COFCTL, WARN, "cofctl(%p)::role_request_rcvd() retransmission xid:0x%x",
					this, msg->get_xid());
		}

		switch (msg->get_role()) {
		case OFP12CR_ROLE_MASTER:
		case OFP12CR_ROLE_SLAVE:
			if (role_initialized)
			{
				uint64_t gen_id = msg->get_generation_id();
				uint64_t dist = (gen_id > cached_generation_id) ?
						(gen_id - cached_generation_id) % std::numeric_limits<uint64_t>::max() :
						(gen_id + std::numeric_limits<uint64_t>::max() + cached_generation_id) % std::numeric_limits<uint64_t>::max();

				if (dist >= (std::numeric_limits<uint64_t>::max() / 2)) {
					throw eRoleRequestStale();
				}
			}
			else
			{
				role_initialized = true;
			}
			cached_generation_id = msg->get_generation_id();
			break;
		default:
			break;
		}

		role = msg->get_role();

#if 0
		for (std::map<cofbase*, cofctl*>::iterator
				it = rofbase->ofctrl_list.begin(); it != rofbase->ofctrl_list.end(); ++it)
		{
			cofctl* ofctrl = it->second;

			if (ofctrl == this)
			{
				continue;
			}

			if (OFP12CR_ROLE_MASTER == ofctrl->role)
			{
				ofctrl->role = OFP12CR_ROLE_SLAVE;
			}
		}
#endif

		//pack->ofh_role_request->generation_id;

		rofbase->role_request_rcvd(this, role);

		rofbase->handle_role_request(this, msg);

	} catch (eRoleRequestStale& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::role_request_rcvd() "
				"Stale Message: old generation_id, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_ROLE_REQUEST_FAILED,
				OFPRRFC_STALE,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRoleRequestUnsupported& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::role_request_rcvd() "
				"Controller role change unsupported, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_ROLE_REQUEST_FAILED,
				OFPRRFC_UNSUP,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRoleRequestBadRole& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::role_request_rcvd() "
				"Invalid role, pack: %s", this, msg->c_str());

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_ROLE_REQUEST_FAILED,
				OFPRRFC_BAD_ROLE,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRoleRequestBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::role_request_rcvd() "
				"base class of exception eRoleRequestBase caught, pack: %s", this, msg->c_str());

		delete msg;
	}
}



void
cofctlImpl::role_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::role_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}



void
cofctlImpl::barrier_request_rcvd(cofmsg_barrier_request *msg)
{
	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::barrier_request_rcvd() retransmission xid:0x%x",
				this, msg->get_xid());
	}

	rofbase->handle_barrier_request(this, msg);
}



void
cofctlImpl::barrier_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::barrier_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}



void
cofctlImpl::queue_get_config_request_rcvd(cofmsg_queue_get_config_request *msg)
{
	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::queue_get_config_request_rcvd() retransmission xid:0x%x",
				this, msg->get_xid());
	}

	rofbase->handle_queue_get_config_request(this, msg);
}



void
cofctlImpl::queue_get_config_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::queue_get_config_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}



void
cofctlImpl::experimenter_rcvd(cofmsg_experimenter *msg)
{
	switch (msg->get_experimenter_id()) {
	case OFPEXPID_ROFL:
	{
		switch (msg->get_experimenter_type()) {
		case croflexp::OFPRET_FLOWSPACE:
		{
			croflexp rexp(msg->get_body().somem(), msg->get_body().memlen());

			switch (rexp.rext_fsp->command) {
			case croflexp::OFPRET_FSP_ADD:
			{
				try {

					rofbase->fsptable.insert_fsp_entry(this, rexp.match);

					WRITELOG(COFCTL, INFO, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => -ADDED-\n%s",
							this, rofbase->fsptable.c_str());

				} catch (eFspEntryOverlap& e) {

					WRITELOG(COFCTL, WARN, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => -REJECTED- (overlap)",
							this);

				}

				break;
			}
			case croflexp::OFPRET_FSP_DELETE:
			{
				try {

					rofbase->fsptable.delete_fsp_entry(this, rexp.match, true /*strict*/);

					WRITELOG(COFCTL, INFO, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => -DELETED-\n%s",
							this, rofbase->fsptable.c_str());

				} catch (eFspEntryNotFound& e) {

					WRITELOG(COFCTL, WARN, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => -NOT-FOUND-",
							this);

				}

				break;
			}
			}

			break;
		}

		}

		delete msg;
		break;
	}


	default:
		rofbase->handle_experimenter_message(this, msg);
		break;
	}
}



void
cofctlImpl::handle_echo_reply_timeout()
{
	WRITELOG(COFDPT, DBG, "cofctl(%p)::handle_echo_reply_timeout() ", this);

	// TODO: repeat ECHO request multiple times (should be configurable)

	socket->cclose();

	if (flags.test(COFCTL_FLAG_ACTIVE_SOCKET))
	{
		try_to_connect(true);
	}
	rofbase->handle_ctl_close(this);
}


void
cofctlImpl::get_async_config_request_rcvd(cofmsg_get_async_config_request *msg)
{
	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::role_request_rcvd() retransmission xid:0x%x",
				this, msg->get_xid());
	}

	// TODO: handle request
}



void
cofctlImpl::set_async_config_rcvd(cofmsg_set_async_config *msg)
{
	// TODO: handle request here in this cofctl instance
	rofbase->handle_set_async_config(this, msg);
}



void
cofctlImpl::get_async_config_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::get_async_config_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}




const char*
cofctlImpl::c_str()
{
#if 0
	std::string t_str;

	std::map<cofbase*, cofctl*>::iterator it;
	for (it = ofctrl_list->begin(); it != ofctrl_list->end(); ++it)
	{
		std::set<cofmatch*>::iterator nit;
		for (nit = it->second->nspaces.begin(); nit != it->second->nspaces.end(); ++nit)
		{
			t_str.append("\n");
			t_str.append(it->second->ctrl->c_str());
			t_str.append(" => ");
			t_str.append((*nit)->c_str());
			t_str.append("\n");
		}
	}
#endif

	cvastring vas;

	info.assign(vas("cofctl(%p) ", this));

	return info.c_str();
}


cxidtrans&
cofctlImpl::transaction(uint32_t xid)
{
	return xidstore.xid_find(xid);
}



void
cofctlImpl::send_error_is_slave(cofmsg *pack)
{
	size_t len = (pack->length() > 64) ? 64 : pack->length();
	rofbase->send_error_message(this,
			pack->get_xid(),
			OFPET_BAD_REQUEST,
			OFPBRC_IS_SLAVE,
			pack->soframe(),
			len);
}



void
cofctlImpl::try_to_connect(bool reset_timeout)
{
	if (pending_timer(COFCTL_TIMER_RECONNECT)) {
		return;
	}

	WRITELOG(COFCTL, INFO, "cofctl(%p)::try_to_connect() "
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

	reset_timer(COFCTL_TIMER_RECONNECT, reconnect_in_seconds);

	++reconnect_counter;
}



void
cofctlImpl::send_message_via_socket(
		cofmsg *pack)
{
	if (0 == socket)
	{
		delete pack; return;
	}

	cmemory *mem = new cmemory(pack->length());

	pack->pack(mem->somem(), mem->memlen());

	WRITELOG(COFCTL, DBG, "send: %s\n", mem->c_str());

	WRITELOG(COFCTL, DBG, "cofctl(%p)::send_message_via_socket() new cmemory: %s",
				this, mem->c_str());

	delete pack;

	socket->send_packet(mem);

	rofbase->wakeup(); // wake-up thread in case, we've been called from another thread
}


