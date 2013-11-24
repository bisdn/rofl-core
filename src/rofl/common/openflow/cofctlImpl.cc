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
    	logging::error << "dropping message (missing HELLO from peer) " << *pack << " " << *this << std::endl;
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
        	logging::error << "dropping invalid packet " << *pack << " " << *this << std::endl;
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
        	logging::error << "dropping invalid packet " << *pack << " " << *this << std::endl;
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
        	logging::error << "dropping invalid packet " << *pack << " " << *this << std::endl;
        	delete pack;
    	} return;
        }

    } break;
    default:
    	logging::error << "dropping packet due to bad version " << *pack << std::endl;
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

		logging::warn << "dropping invalid message " << *mem << std::endl;

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

		assert(NULL != mem);

		struct ofp_header* ofh_header = (struct ofp_header*)mem->somem();
		xid = be32toh(ofh_header->xid);

		const uint8_t OFPT_HELLO = 0; // == OFPT10_HELLO == OFPT12_HELLO == OFPT13_HELLO

		if (not flags.test(COFCTL_FLAG_HELLO_RCVD) && (OFPT_HELLO != ofh_header->type)) {
			logging::error << "dropping packet, no HELLO received from peer so far " << *mem << std::endl;
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
				logging::warn << "dropping unknown message " << *mem << std::endl;
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
				logging::warn << "dropping unknown message " << *mem << std::endl;
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
				logging::error << "dropping unknown packet " << *mem << std::endl;
				msg = new cofmsg(mem);
				msg->validate();
				throw eBadRequestBadType();
			} return;
			}


		} break;
		}


	} catch (eBadSyntaxTooShort& e) {

		logging::error << "eBadSyntaxTooShort " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_LEN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadVersion& e) {

		logging::error << "eBadVersion " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_VERSION,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadVersion& e) {

		logging::error << "eBadRequestBadVersion " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_VERSION,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadType& e) {

		logging::error << "eBadRequestBadType " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadStat& e) {

		logging::error << "eBadRequestBadStat " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_STAT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadExperimenter& e) {

		logging::error << "eBadRequestBadExperimenter " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_EXPERIMENTER,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadExpType& e) {

		logging::error << "eBadRequestBadExpType " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_EXP_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestEperm& e) {

		logging::error << "eBadRequestEperm " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_EPERM,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadLen& e) {

		logging::error << "eBadRequestBadLen " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_LEN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBufferEmpty& e) {

		logging::error << "eBadRequestBufferEmpty " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BUFFER_EMPTY,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBufferUnknown& e) {

		logging::error << "eBadRequestBufferUnknown " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BUFFER_UNKNOWN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadTableId& e) {

		logging::error << "eBadRequestBadTableId " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_TABLE_ID,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestIsSlave& e) {

		logging::error << "eBadRequestIsSlave " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_IS_SLAVE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadPort& e) {

		logging::error << "eBadRequestBadPort " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_PORT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBadPacket& e) {

		logging::error << "eBadRequestBadPacket " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_PACKET,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadRequestBase& e) {

		logging::error << "eBadRequestBase " << *mem << std::endl;

		delete msg;
	} catch (eBadActionBadType& e) {

		logging::error << "eBadActionBadType " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadLen& e) {

		logging::error << "eBadActionBadLen " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_LEN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadExperimenter& e) {

		logging::error << "eBadActionBadExperimenter " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_EXPERIMENTER,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadExperimenterType& e) {

		logging::error << "eBadActionBadExperimenterType " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_EXPERIMENTER_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadOutPort& e) {

		logging::error << "eBadActionBadOutPort " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_OUT_PORT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadArgument& e) {

		logging::error << "eBadActionBadArgument " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_ARGUMENT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionEperm& e) {

		logging::error << "eBadActionEperm " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_EPERM,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionTooMany& e) {

		logging::error << "eBadActionTooMany " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_TOO_MANY,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadQueue& e) {

		logging::error << "eBadActionBadQueue " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_QUEUE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadOutGroup& e) {

		logging::error << "eBadActionBadOutGroup " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_OUT_GROUP,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionMatchInconsistent& e) {

		logging::error << "eBadActionMatchInconsistent " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_MATCH_INCONSISTENT,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionUnsupportedOrder& e) {

		logging::error << "eBadActionUnsuportedOrder " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_UNSUPPORTED_ORDER,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBadTag& e) {

		logging::error << "eBadActionBadTag " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_ACTION,
					OFPBAC_BAD_TAG,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadActionBase& e) {

		logging::error << "eBadActionBase " << *mem << std::endl;

		delete msg;
	} catch (eBadInstUnknownInst& e) {

		logging::error << "eBadInstUnknownInst " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNKNOWN_INST,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstUnsupInst& e) {

		logging::error << "eBadInstUnsupInst " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_INST,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstBadTableId& e) {

		logging::error << "eBadInstBadTableId " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_BAD_TABLE_ID,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstUnsupMetadata& e) {

		logging::error << "eBadInstUnsupMetadata " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_METADATA,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstUnsupMetadataMask& e) {

		logging::error << "eBadInstUnsupMetadataMask " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_METADATA_MASK,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstUnsupExpInst& e) {

		logging::error << "eBadInstUnsupExpInst " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_EXP_INST,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadInstBase& e) {

		logging::error << "eBadInstBase " << *mem << std::endl;

		delete msg;
	} catch (eBadMatchBadType& e) {

		logging::error << "eBadMatchBadType " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_TYPE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadLen& e) {

		logging::error << "eBadMatchBadLen " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_LEN,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadTag& e) {

		logging::error << "eBadMatchBadTag " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_TAG,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadDlAddrMask& e) {

		logging::error << "eBadMatchBadDlAddrMask " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_DL_ADDR_MASK,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadNwAddrMask& e) {

		logging::error << "eBadMatchBadNwAddrMask " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_NW_ADDR_MASK,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadWildcards& e) {

		logging::error << "eBadMatchBadWildcards " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_WILDCARDS,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadField& e) {

		logging::error << "eBadMatchBadField " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_FIELD,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBadValue& e) {

		logging::error << "eBadMatchBadValue " << *mem << std::endl;

		rofbase->send_error_message(
					this,
					xid,
					OFPET_BAD_MATCH,
					OFPBMC_BAD_VALUE,
					mem->somem(), mem->memlen());

		delete msg;
	} catch (eBadMatchBase& e) {

		logging::error << "eBadMatchBase " << *mem << std::endl;

		delete msg;
	}
}




void
cofctlImpl::hello_rcvd(cofmsg_hello *msg)
{
	try {
		logging::debug << "HELLO message received " << *msg << std::endl;

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

			logging::info << "HELLO exchanged with peer entity, attaching..." << *this << std::endl;

			if (flags.test(COFCTL_FLAG_HELLO_SENT))
			{
				register_timer(COFCTL_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);

				rofbase->handle_ctl_open(this);
			}
		}

		delete msg;

	} catch (eHelloIncompatible& e) {

		logging::warn << "eHelloIncompatible " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_HELLO_FAILED,
					OFPHFC_INCOMPATIBLE,
					msg->soframe(), msg->framelen());

		delete msg;
		handle_closed(socket, socket->sd);
	} catch (eHelloEperm& e) {

		logging::warn << "eHelloEperm " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_HELLO_FAILED,
					OFPHFC_EPERM,
					msg->soframe(), msg->framelen());

		delete msg;
		handle_closed(socket, socket->sd);
	} catch (eHelloBase& e) {

		logging::warn << "eHelloBase " << *msg << std::endl;

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
		logging::debug << "retransmitted Features-Request " << *msg << std::endl;
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
		logging::error << "spurious xid in reply to Features-Request " << *msg << std::endl;
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
		logging::error << "spurious xid in reply to Get-Config-Request " << *msg << std::endl;
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

		logging::warn << "eSwitchConfigBadFlags " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_SWITCH_CONFIG_FAILED,
					OFPSCFC_BAD_FLAGS,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eSwitchConfigBadLen& e) {

		logging::warn << "eSwitchConfigBadLen " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_SWITCH_CONFIG_FAILED,
					OFPSCFC_BAD_LEN,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eSwitchConfigBase& e) {

		logging::warn << "eSwitchConfigBase " << *msg << std::endl;

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

		logging::warn << "eFlowModUnknown " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_UNKNOWN,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModTableFull& e) {

		logging::warn << "eFlowModTableFull " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_TABLE_FULL,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModBadTableId& e) {

		logging::warn << "eFlowModBadTableId " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_BAD_TABLE_ID,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModOverlap& e) {

		logging::warn << "eFlowModOverlap " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_OVERLAP,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModEperm& e) {

		logging::warn << "eFlowModEperm " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_EPERM,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModBadTimeout&e ) {

		logging::warn << "eFlowModBadTimeout " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_BAD_TIMEOUT,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModBadCommand& e) {

		logging::warn << "eFlowModBadCommand " << *msg << std::endl;

		rofbase->send_error_message(
					this,
					msg->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_BAD_COMMAND,
					msg->soframe(), msg->framelen());

		delete msg;
	} catch (eFlowModBase& e) {

		logging::warn << "eFlowModBase " << *msg << std::endl;

		delete msg;
	} catch (eFspNotAllowed& e) {

		logging::warn << "eFspNotAllowed " << *msg << " fsptable:" << rofbase->fsptable << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_EPERM,
				msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRofBaseTableNotFound& e) {

		logging::warn << "eRofBaseTableNotFound " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_BAD_TABLE_ID,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eInstructionInvalType& e) {

		logging::warn << "eInstructionInvalType " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNKNOWN_INST,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRofBaseGotoTableNotFound& e) {

		logging::warn << "eRofBaseGotoTableNotFound " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_BAD_TABLE_ID,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eInstructionBadExperimenter& e) {

		logging::warn << "eInstructionBadExperimenter " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNSUP_EXP_INST,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eOFmatchInvalBadValue& e) {

		logging::warn << "eOFmatchInvalBadValue " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_BAD_MATCH,
				OFPBMC_BAD_VALUE,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (cerror &e) {

		logging::warn << "cerror " << *msg << std::endl;

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

		logging::warn << "eGroupModExists " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_GROUP_EXISTS,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModInvalGroup& e) {

		logging::warn << "eGroupModInvalGroup " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_INVALID_GROUP,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModWeightUnsupported& e) {

		logging::warn << "eGroupModWeightUnsupported " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_WEIGHT_UNSUPPORTED,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModOutOfGroups& e) {

		logging::warn << "eGroupModOutOfGroups " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_OUT_OF_GROUPS,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModOutOfBuckets& e) {

		logging::warn << "eGroupModOutOfBuckets " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_OUT_OF_BUCKETS,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModChainingUnsupported& e) {

		logging::warn << "eGroupModChainingUnsupported " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_CHAINING_UNSUPPORTED,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModWatchUnsupported& e) {

		logging::warn << "eGroupModWatchUnsupported " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_WATCH_UNSUPPORTED,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModLoop& e) {

		logging::warn << "eGroupModLoop " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_LOOP,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModUnknownGroup& e) {

		logging::warn << "eGroupModUnknownGroup " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_UNKNOWN_GROUP,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModChainedGroup& e) {

		logging::warn << "eGroupModChainedGroup " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_CHAINED_GROUP,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBadType& e) {

		logging::warn << "eGroupModBadType " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_TYPE,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBadCommand& e) {

		logging::warn << "eGroupModBadCommand " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_COMMAND,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBadBucket& e) {

		logging::warn << "eGroupModBadBucket " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_BUCKET,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBadWatch& e) {

		logging::warn << "eGroupModBadWatch " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_WATCH,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModEperm& e) {

		logging::warn << "eGroupModEperm " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_EPERM,
				msg->soframe(),
				msg->framelen());

		delete msg;
	} catch (eGroupModBase& e) {

		logging::warn << "eGroupModBase " << *msg << std::endl;

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

		logging::warn << "ePortModBadPort " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_PORT,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (ePortModBadHwAddr& e) {

		logging::warn << "ePortModBadHwAddr " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_HW_ADDR,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (ePortModBadConfig& e) {

		logging::warn << "ePortModBadConfig " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_CONFIG,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (ePortModBadAdvertise& e) {

		logging::warn << "ePortModBadAdvertise " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_ADVERTISE,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (ePortModBase& e) {

		logging::warn << "ePortModBase " << *msg << std::endl;

		delete msg;
	}
}


void
cofctlImpl::table_mod_rcvd(cofmsg_table_mod *msg)
{
	try {
		if (OFP12CR_ROLE_SLAVE == role) {
			send_error_is_slave(msg); return;
		}

		rofbase->handle_table_mod(this, msg);

	} catch (eTableModBadTable& e) {

		logging::warn << "eTableModBadTable " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_TABLE_MOD_FAILED,
				OFPTMFC_BAD_TABLE,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eTableModBadConfig& e) {

		logging::warn << "eTableModBadConfig " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_TABLE_MOD_FAILED,
				OFPTMFC_BAD_CONFIG,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eTableModBase& e) {

		logging::warn << "eTableModBase " << *msg << std::endl;

		delete msg;
	}
}



void
cofctlImpl::stats_request_rcvd(cofmsg_stats *msg)
{
	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "retransmitted Stats-Request " << *msg << std::endl;
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
		logging::warn << "spurious xid in reply to Stats-Request " << *msg << std::endl;
	}
}



void
cofctlImpl::role_request_rcvd(cofmsg_role_request *msg)
{
	try {
		try {
			xidstore.xid_add(this, msg->get_xid());

		} catch (eXidStoreXidBusy& e) {
			logging::warn << "retransmitted Role-Request " << *msg << std::endl;
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

		logging::warn << "eRoleRequestStale " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_ROLE_REQUEST_FAILED,
				OFPRRFC_STALE,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRoleRequestUnsupported& e) {

		logging::warn << "eRoleRequestUnsupported " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_ROLE_REQUEST_FAILED,
				OFPRRFC_UNSUP,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRoleRequestBadRole& e) {

		logging::warn << "eRoleRequestBadRole " << *msg << std::endl;

		rofbase->send_error_message(
				this,
				msg->get_xid(),
				OFPET_ROLE_REQUEST_FAILED,
				OFPRRFC_BAD_ROLE,
				(uint8_t*)msg->soframe(), msg->framelen());

		delete msg;
	} catch (eRoleRequestBase& e) {

		logging::warn << "eRoleRequestBase " << *msg << std::endl;

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
		logging::warn << "spurious xid in reply to Role-Request " << *msg << std::endl;
	}
}



void
cofctlImpl::barrier_request_rcvd(cofmsg_barrier_request *msg)
{
	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "retransmitted Barrier-Request " << *msg << std::endl;
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
		logging::warn << "spurious xid in reply to Barrier-Request " << *msg << std::endl;
	}
}



void
cofctlImpl::queue_get_config_request_rcvd(cofmsg_queue_get_config_request *msg)
{
	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "retransmitted Queue-Get-Config-Request " << *msg << std::endl;
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
		logging::warn << "spurious xid in reply to Queue-Get-Config-Request " << *msg << std::endl;
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
		logging::warn << "retransmitted Get-Async-Config-Request " << *msg << std::endl;
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
		logging::warn << "spurious xid in reply to Get-Async-Config-Request " << *msg << std::endl;
	}
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

	logging::debug << "sending message " << *pack << std::endl;

	delete pack;

	socket->send_packet(mem);

	rofbase->wakeup(); // wake-up thread in case, we've been called from another thread
}


