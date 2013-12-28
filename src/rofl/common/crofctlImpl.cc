/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crofctlImpl.h"

using namespace rofl;


crofctlImpl::crofctlImpl(
		crofbase *rofbase) :
				crofctl(rofbase),
				ctid(0),
				rofbase(rofbase),
				flags(0),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(openflow12::OFPCR_ROLE_EQUAL),
				cached_generation_id(0),
				socket(0),
				fragment(0),
				msg_bytes_read(0),
				reconnect_start_timeout(RECONNECT_START_TIMEOUT),
				reconnect_in_seconds(RECONNECT_START_TIMEOUT),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
				echo_reply_timeout(DEFAULT_ECHO_TIMEOUT),
				ofp_version(openflow12::OFP_VERSION)
{
	register_timer(COFCTL_TIMER_SEND_HELLO, 0);
}



crofctlImpl::crofctlImpl(
		crofbase *rofbase,
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				crofctl(rofbase),
				ctid(0),
				rofbase(rofbase),
				flags(0),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(openflow12::OFPCR_ROLE_EQUAL),
				cached_generation_id(0),
				socket(new csocket(this, newsd, ra, domain, type, protocol)),
				fragment(0),
				msg_bytes_read(0),
				reconnect_start_timeout(0),
				reconnect_in_seconds(0),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
				echo_reply_timeout(DEFAULT_ECHO_TIMEOUT),
				ofp_version(openflow12::OFP_VERSION)
{
	register_timer(COFCTL_TIMER_SEND_HELLO, 0);
}



crofctlImpl::crofctlImpl(
		crofbase *rofbase,
		uint8_t ofp_version,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				crofctl(rofbase),
				ctid(0),
				rofbase(rofbase),
				flags(COFCTL_FLAG_ACTIVE_SOCKET),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(openflow12::OFPCR_ROLE_EQUAL),
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
	flags.set(COFCTL_FLAG_ACTIVE_SOCKET);

	this->reconnect_in_seconds = this->reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;

	socket->cconnect(ra, caddress(AF_INET, "0.0.0.0"), domain, type, protocol);
}



crofctlImpl::~crofctlImpl()
{
	rofbase->fsptable.delete_fsp_entries(this);

	delete socket;
}



bool
crofctlImpl::is_established() const
{
	return (STATE_CTL_ESTABLISHED == cur_state());
}



bool
crofctlImpl::is_slave() const
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: return (openflow12::OFPCR_ROLE_SLAVE == role);
	case openflow13::OFP_VERSION: return (openflow13::OFPCR_ROLE_SLAVE == role);
	default: return false;
	}
}



uint8_t
crofctlImpl::get_version()
{
	return ofp_version;
}



caddress
crofctlImpl::get_peer_addr()
{
	return socket->raddr;
}



uint32_t
crofctlImpl::get_role() const
{
	return role;
}



void
crofctlImpl::set_role(uint32_t role)
{
	this->role = role;
}



void
crofctlImpl::send_message(
		cofmsg *msg)
{
	const uint8_t OFPT_HELLO = 0; // == openflow10::OFPT_HELLO == openflow12::OFPT_HELLO == openflow13::OFPT_HELLO

    if (not flags.test(COFCTL_FLAG_HELLO_RCVD) && (msg->get_type() != OFPT_HELLO)) {
    	logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
    			<< "dropping message (missing HELLO from peer) " << *msg << " " << *this << std::endl;
        delete msg; return;
    }

    switch (msg->get_version()) {
    case openflow10::OFP_VERSION: {

        switch (msg->get_type()) {
        case openflow10::OFPT_HELLO: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Hello message " << std::endl << *dynamic_cast<cofmsg_hello*>(msg);
        } break;
        case openflow10::OFPT_ERROR: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Error message " << std::endl << *dynamic_cast<cofmsg_error*>(msg);
    	} break;
        case openflow10::OFPT_ECHO_REQUEST: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Echo-Request message " << std::endl << *dynamic_cast<cofmsg_echo_request*>(msg);
    		echo_request_sent(msg);
    	} break;
        case openflow10::OFPT_ECHO_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Echo-Reply message " << std::endl << *dynamic_cast<cofmsg_echo_reply*>(msg);
    	} break;
        case openflow10::OFPT_VENDOR: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Vendor message " << std::endl << *dynamic_cast<cofmsg_experimenter*>(msg);
    	} break;
        case openflow10::OFPT_FEATURES_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Features-Reply message " << std::endl << *dynamic_cast<cofmsg_features_reply*>(msg);
    		features_reply_sent(msg);
    	} break;
        case openflow10::OFPT_GET_CONFIG_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Get-Config-Reply message " << std::endl << *dynamic_cast<cofmsg_get_config_reply*>(msg);
    		get_config_reply_sent(msg);
    	} break;
        case openflow10::OFPT_PACKET_IN: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Packet-In message " << std::endl << *dynamic_cast<cofmsg_packet_in*>(msg);
    	} break;
        case openflow10::OFPT_FLOW_REMOVED: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Flow-Removed message " << std::endl << *dynamic_cast<cofmsg_flow_removed*>(msg);
    	} break;
        case openflow10::OFPT_PORT_STATUS: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Port-Status message " << std::endl << *dynamic_cast<cofmsg_port_status*>(msg);
    	} break;
        case openflow10::OFPT_STATS_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Stats-Reply message " << std::endl << *dynamic_cast<cofmsg_stats_reply*>(msg);
    		stats_reply_sent(msg);
    	} break;
        case openflow10::OFPT_BARRIER_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Barrier-Reply message " << std::endl << *dynamic_cast<cofmsg_barrier_reply*>(msg);
    		barrier_reply_sent(msg);
    	} break;
        case openflow10::OFPT_QUEUE_GET_CONFIG_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Queue-Get-Config-Reply message " << std::endl << *dynamic_cast<cofmsg_queue_get_config_reply*>(msg);
    		queue_get_config_reply_sent(msg);
    	} break;
        default: {
        	logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending dropping invalid packet " << *msg << " " << *this << std::endl;
    		delete msg;
    	} return;
        }

    } break;
    case openflow12::OFP_VERSION: {

        switch (msg->get_type()) {
        case openflow12::OFPT_HELLO: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Hello message " << std::endl << *dynamic_cast<cofmsg_hello*>(msg);
        } break;
        case openflow12::OFPT_ERROR: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Error message " << std::endl << *dynamic_cast<cofmsg_error*>(msg);
    	} break;
        case openflow12::OFPT_ECHO_REQUEST: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Echo-Request message " << std::endl << *dynamic_cast<cofmsg_echo_request*>(msg);
    		echo_request_sent(msg);
    	} break;
        case openflow12::OFPT_ECHO_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Echo-Reply message " << std::endl << *dynamic_cast<cofmsg_echo_reply*>(msg);
    	} break;
        case openflow12::OFPT_EXPERIMENTER: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Experimenter message " << std::endl << *dynamic_cast<cofmsg_experimenter*>(msg);
    	} break;
        case openflow12::OFPT_FEATURES_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Features-Reply message " << std::endl << *dynamic_cast<cofmsg_features_reply*>(msg);
    		features_reply_sent(msg);
    	} break;
        case openflow12::OFPT_GET_CONFIG_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Get-Config-Reply message " << std::endl << *dynamic_cast<cofmsg_get_config_reply*>(msg);
    		get_config_reply_sent(msg);
    	} break;
        case openflow12::OFPT_PACKET_IN: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Packet-In message " << std::endl << *dynamic_cast<cofmsg_packet_in*>(msg);
    	} break;
        case openflow12::OFPT_FLOW_REMOVED: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Flow-Removed message " << std::endl << *dynamic_cast<cofmsg_flow_removed*>(msg);
    	} break;
        case openflow12::OFPT_PORT_STATUS: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Port-Status message " << std::endl << *dynamic_cast<cofmsg_port_status*>(msg);
    	} break;
        case openflow12::OFPT_STATS_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Stats-Reply message " << std::endl << *dynamic_cast<cofmsg_stats_reply*>(msg);
    		stats_reply_sent(msg);
    	} break;
        case openflow12::OFPT_BARRIER_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Barrier-Reply message " << std::endl << *dynamic_cast<cofmsg_barrier_reply*>(msg);
    		barrier_reply_sent(msg);
    	} break;
        case openflow12::OFPT_QUEUE_GET_CONFIG_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Queue-Get-Config-Reply message " << std::endl << *dynamic_cast<cofmsg_queue_get_config_reply*>(msg);
    		queue_get_config_reply_sent(msg);
    	} break;
        case openflow12::OFPT_ROLE_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Role-Reply message " << std::endl << *dynamic_cast<cofmsg_role_reply*>(msg);
    		role_reply_sent(msg);
    	} break;
        default: {
        	logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending dropping invalid packet " << *dynamic_cast<cofmsg*>(msg) << " " << *this << std::endl;
        	delete msg;
    	} return;
        }


    } break;
    case openflow13::OFP_VERSION: {

        switch (msg->get_type()) {
        case openflow13::OFPT_HELLO: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Hello message " << std::endl << *dynamic_cast<cofmsg_hello*>(msg);
        } break;
        case openflow13::OFPT_ERROR: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Error message " << std::endl << *dynamic_cast<cofmsg_error*>(msg);
    	} break;
        case openflow13::OFPT_ECHO_REQUEST: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Echo-Request message " << std::endl << *dynamic_cast<cofmsg_echo_request*>(msg);
    		echo_request_sent(msg);
    	} break;
        case openflow13::OFPT_ECHO_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Echo-Reply message " << std::endl << *dynamic_cast<cofmsg_echo_reply*>(msg);
    	} break;
        case openflow13::OFPT_EXPERIMENTER: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Experimenter message " << std::endl << *dynamic_cast<cofmsg_experimenter*>(msg);
    	} break;
        case openflow13::OFPT_FEATURES_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Features-Reply message " << std::endl << *dynamic_cast<cofmsg_features_reply*>(msg);
    		features_reply_sent(msg);
    	} break;
        case openflow13::OFPT_GET_CONFIG_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Get-Config-Reply message " << std::endl << *dynamic_cast<cofmsg_get_config_reply*>(msg);
    		get_config_reply_sent(msg);
    	} break;
        case openflow13::OFPT_PACKET_IN: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Packet-In message " << std::endl << *dynamic_cast<cofmsg_packet_in*>(msg);
    	} break;
        case openflow13::OFPT_FLOW_REMOVED: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Flow-Removed message " << std::endl << *dynamic_cast<cofmsg_flow_removed*>(msg);
    	} break;
        case openflow13::OFPT_PORT_STATUS: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Port-Status message " << std::endl << *dynamic_cast<cofmsg_port_status*>(msg);
    	} break;
        case openflow13::OFPT_STATS_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Stats-Reply message " << std::endl << *dynamic_cast<cofmsg_stats_reply*>(msg);
    		stats_reply_sent(msg);
    	} break;
        case openflow13::OFPT_BARRIER_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Barrier-Reply message " << std::endl << *dynamic_cast<cofmsg_barrier_reply*>(msg);
    		barrier_reply_sent(msg);
    	} break;
        case openflow13::OFPT_QUEUE_GET_CONFIG_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Queue-Get-Config-Reply message " << std::endl << *dynamic_cast<cofmsg_queue_get_config_reply*>(msg);
    		queue_get_config_reply_sent(msg);
    	} break;
        case openflow13::OFPT_ROLE_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Role-Reply message " << std::endl << *dynamic_cast<cofmsg_role_reply*>(msg);
    		role_reply_sent(msg);
    	} break;
        case openflow13::OFPT_GET_ASYNC_REPLY: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Get-Async-Reply message " << std::endl << *dynamic_cast<cofmsg_get_async_config_reply*>(msg);
        	get_async_config_reply_sent(msg);
        } break;
        default: {
        	logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending dropping invalid packet " << *msg << " " << *this << std::endl;
        	delete msg;
    	} return;
        }

    } break;
    default:
    	logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " dropping packet due to bad version " << *msg << std::endl;
    	throw eBadVersion();
    }

	send_message_via_socket(msg);
}



void
crofctlImpl::handle_timeout(
		int opaque)
{
	switch (opaque) {
	case COFCTL_TIMER_SEND_HELLO: {
		rofbase->send_hello_message(this);
		flags.set(COFCTL_FLAG_HELLO_SENT);
		if (flags.test(COFCTL_FLAG_HELLO_RCVD)) {
			rofbase->send_echo_request(this);
			logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << "" << *this << indent(2)
					<< "Hello exchanged with peer entity (disconnected -> connected)" << std::endl;
			rofbase->handle_ctl_open(this);
		}
	} break;
	case COFCTL_TIMER_RECONNECT: {
		if (socket) {
			new_state(STATE_CTL_CONNECTING);
			socket->cconnect(socket->raddr,
					caddress(AF_INET, "0.0.0.0"),
					socket->domain,
					socket->type,
					socket->protocol);
		}
	} break;
	case COFCTL_TIMER_SEND_ECHO_REQUEST: {
		rofbase->send_echo_request(this);
	} break;
	case COFCTL_TIMER_ECHO_REPLY_TIMEOUT: {
		handle_echo_reply_timeout();
	} break;
	}
}



void
crofctlImpl::handle_accepted(
		csocket *socket,
		int newsd,
		caddress const& ra)
{
	logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " connection accepted:" << ra << " (init -> disconnected)" << std::endl;
}



void
crofctlImpl::handle_connected(
		csocket *socket,
		int sd)
{
	logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " connection established:" << socket->raddr << " (init -> disconnected)" << std::endl;

	register_timer(COFCTL_TIMER_SEND_HELLO, 0);
}



void
crofctlImpl::handle_connect_refused(
		csocket *socket,
		int sd)
{
	logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " connection failed:" << socket->raddr << std::endl;

	rofbase->rpc_ctl_failed(this); // signal event back to rofbase

	if (flags.test(COFCTL_FLAG_ACTIVE_SOCKET)) {
		try_to_connect();
	}
}



void
crofctlImpl::handle_read(
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
					logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " error reading from socket descriptor:" << sd
							<< " " << eSysCall() << ", closing socket. (... -> disconnected)" << std::endl;
					handle_closed(socket, sd);
				} return;
				}
			}
			else if (rc == 0) // socket was closed
			{
				//rfds.erase(fd);
				logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " peer closed connection. (... -> disconnected)" << *this << std::endl;

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

		logging::warn << "dropping invalid message " << *mem << std::endl;

		if (mem) {
			delete mem; fragment = (cmemory*)0;
		}
		// handle_closed(socket, sd);
	}

}



void
crofctlImpl::handle_closed(
		csocket *socket,
		int sd)
{
	logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " connection closed. " << *this << std::endl;

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
crofctlImpl::parse_message(
		cmemory *mem)
{
	cofmsg *msg = (cofmsg*)0;
	uint32_t xid = 0;
	try {

		assert(NULL != mem);

		struct openflow::ofp_header* ofh_header = (struct openflow::ofp_header*)mem->somem();
		xid = be32toh(ofh_header->xid);

		const uint8_t OFPT_HELLO = 0; // == openflow10::OFPT_HELLO == openflow12::OFPT_HELLO == openflow13::OFPT_HELLO

		if (not flags.test(COFCTL_FLAG_HELLO_RCVD) && (OFPT_HELLO != ofh_header->type)) {
			logging::error << "dropping packet, no HELLO received from peer so far " << *mem << std::endl;
			delete mem; return;
		}

		switch (ofp_version) {
		case openflow10::OFP_VERSION: parse_of10_message(mem, &msg); break;
		case openflow12::OFP_VERSION: parse_of12_message(mem, &msg); break;
		case openflow13::OFP_VERSION: parse_of13_message(mem, &msg); break;
		default: {

			switch (ofh_header->type) {
			case openflow10::OFPT_HELLO /* == openflow12::OFPT_HELLO == openflow13::OFPT_HELLO */: {
				msg = new cofmsg_hello(mem);
				msg->validate();
				hello_rcvd(dynamic_cast<cofmsg_hello*>( msg ));
			} break;
			default: {
				logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
						<< " dropping unknown packet " << *mem << std::endl;
				msg = new cofmsg(mem);
				msg->validate();
				throw eBadRequestBadType();
			} return;
			}


		} break;
		}


	} catch (eBadSyntaxTooShort& e) {

		logging::error << "eBadSyntaxTooShort " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_len(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadVersion& e) {

		logging::error << "eBadVersion " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_version(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBadVersion& e) {

		logging::error << "eBadRequestBadVersion " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_version(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBadType& e) {

		logging::error << "eBadRequestBadType " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_type(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBadStat& e) {

		logging::error << "eBadRequestBadStat " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_stat(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBadExperimenter& e) {

		logging::error << "eBadRequestBadExperimenter " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_experimenter(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBadExpType& e) {

		logging::error << "eBadRequestBadExpType " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_exp_type(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestEperm& e) {

		logging::error << "eBadRequestEperm " << *mem << std::endl;
		rofbase->send_error_bad_request_eperm(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBadLen& e) {

		logging::error << "eBadRequestBadLen " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_len(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBufferEmpty& e) {

		logging::error << "eBadRequestBufferEmpty " << *mem << std::endl;
		rofbase->send_error_bad_request_buffer_empty(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBufferUnknown& e) {

		logging::error << "eBadRequestBufferUnknown " << *mem << std::endl;
		rofbase->send_error_bad_request_buffer_unknown(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBadTableId& e) {

		logging::error << "eBadRequestBadTableId " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_table_id(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestIsSlave& e) {

		logging::error << "eBadRequestIsSlave " << *mem << std::endl;
		rofbase->send_error_bad_request_is_slave(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBadPort& e) {

		logging::error << "eBadRequestBadPort " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_port(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBadPacket& e) {

		logging::error << "eBadRequestBadPacket " << *mem << std::endl;
		rofbase->send_error_bad_request_bad_packet(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadRequestBase& e) {

		logging::error << "eBadRequestBase " << *mem << std::endl;
		delete msg;

	} catch (eBadActionBadType& e) {

		logging::error << "eBadActionBadType " << *mem << std::endl;
		rofbase->send_error_bad_action_bad_type(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionBadLen& e) {

		logging::error << "eBadActionBadLen " << *mem << std::endl;
		rofbase->send_error_bad_action_bad_len(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionBadExperimenter& e) {

		logging::error << "eBadActionBadExperimenter " << *mem << std::endl;
		rofbase->send_error_bad_action_bad_experimenter(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionBadExperimenterType& e) {

		logging::error << "eBadActionBadExperimenterType " << *mem << std::endl;
		rofbase->send_error_bad_action_bad_experimenter_type(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionBadOutPort& e) {

		logging::error << "eBadActionBadOutPort " << *mem << std::endl;
		rofbase->send_error_bad_action_bad_out_port(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionBadArgument& e) {

		logging::error << "eBadActionBadArgument " << *mem << std::endl;
		rofbase->send_error_bad_action_bad_argument(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionEperm& e) {

		logging::error << "eBadActionEperm " << *mem << std::endl;
		rofbase->send_error_bad_action_eperm(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionTooMany& e) {

		logging::error << "eBadActionTooMany " << *mem << std::endl;
		rofbase->send_error_bad_action_too_many(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionBadQueue& e) {

		logging::error << "eBadActionBadQueue " << *mem << std::endl;
		rofbase->send_error_bad_action_bad_queue(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionBadOutGroup& e) {

		logging::error << "eBadActionBadOutGroup " << *mem << std::endl;
		rofbase->send_error_bad_action_bad_out_group(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionMatchInconsistent& e) {

		logging::error << "eBadActionMatchInconsistent " << *mem << std::endl;
		rofbase->send_error_bad_action_match_inconsistent(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionUnsupportedOrder& e) {

		logging::error << "eBadActionUnsuportedOrder " << *mem << std::endl;
		rofbase->send_error_bad_action_unsupported_order(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionBadTag& e) {

		logging::error << "eBadActionBadTag " << *mem << std::endl;
		rofbase->send_error_bad_action_bad_tag(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadActionBase& e) {

		logging::error << "eBadActionBase " << *mem << std::endl;
		delete msg;

	} catch (eBadInstUnknownInst& e) {

		logging::error << "eBadInstUnknownInst " << *mem << std::endl;
		rofbase->send_error_bad_inst_unknown_inst(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadInstUnsupInst& e) {

		logging::error << "eBadInstUnsupInst " << *mem << std::endl;
		rofbase->send_error_bad_inst_unsup_inst(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadInstBadTableId& e) {

		logging::error << "eBadInstBadTableId " << *mem << std::endl;
		rofbase->send_error_bad_inst_bad_table_id(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadInstUnsupMetadata& e) {

		logging::error << "eBadInstUnsupMetadata " << *mem << std::endl;
		rofbase->send_error_bad_inst_unsup_metadata(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadInstUnsupMetadataMask& e) {

		logging::error << "eBadInstUnsupMetadataMask " << *mem << std::endl;
		rofbase->send_error_bad_inst_unsup_metadata_mask(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadInstUnsupExpInst& e) {

		logging::error << "eBadInstUnsupExpInst " << *mem << std::endl;
		rofbase->send_error_bad_inst_unsup_exp_inst(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadInstBase& e) {

		logging::error << "eBadInstBase " << *mem << std::endl;
		delete msg;

	} catch (eBadMatchBadType& e) {

		logging::error << "eBadMatchBadType " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_type(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBadLen& e) {

		logging::error << "eBadMatchBadLen " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_len(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBadTag& e) {

		logging::error << "eBadMatchBadTag " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_tag(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBadDlAddrMask& e) {

		logging::error << "eBadMatchBadDlAddrMask " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_dladdr_mask(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBadNwAddrMask& e) {

		logging::error << "eBadMatchBadNwAddrMask " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_nwaddr_mask(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBadWildcards& e) {

		logging::error << "eBadMatchBadWildcards " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_wildcards(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBadField& e) {

		logging::error << "eBadMatchBadField " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_field(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBadValue& e) {

		logging::error << "eBadMatchBadValue " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_value(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBadMask& e) {

		logging::error << "eBadMatchBadMask " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_mask(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBadPrereq& e) {

		logging::error << "eBadMatchBadPrereq " << *mem << std::endl;
		rofbase->send_error_bad_match_bad_prereq(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchDupField& e) {

		logging::error << "eBadMatchDupField " << *mem << std::endl;
		rofbase->send_error_bad_match_dup_field(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchEPerm& e) {

		logging::error << "eBadMatchEPerm " << *mem << std::endl;
		rofbase->send_error_bad_match_eperm(this, xid, mem->somem(), mem->memlen());
		delete msg;

	} catch (eBadMatchBase& e) {

		logging::error << "eBadMatchBase " << *mem << std::endl;
		delete msg;
	}
}



void
crofctlImpl::parse_of10_message(cmemory *mem, cofmsg **pmsg)
{
	struct openflow::ofp_header* ofh_header = (struct openflow::ofp_header*)mem->somem();

	switch (ofh_header->type) {
	case openflow10::OFPT_HELLO: {
		(*pmsg = new cofmsg_hello(mem))->validate();
		hello_rcvd(dynamic_cast<cofmsg_hello*>( *pmsg ));
	} break;
	case openflow10::OFPT_ECHO_REQUEST: {
		(*pmsg = new cofmsg_echo_request(mem))->validate();
		echo_request_rcvd(dynamic_cast<cofmsg_echo_request*>( *pmsg ));
	} break;
	case openflow10::OFPT_ECHO_REPLY: {
		(*pmsg = new cofmsg_echo_reply(mem))->validate();
		echo_reply_rcvd(dynamic_cast<cofmsg_echo_reply*>( *pmsg ));
	} break;
	case openflow10::OFPT_VENDOR: {
		(*pmsg = new cofmsg_experimenter(mem))->validate();
		experimenter_rcvd(dynamic_cast<cofmsg_experimenter*>( *pmsg ));
	} break;
	case openflow10::OFPT_FEATURES_REQUEST:	{
		(*pmsg = new cofmsg_features_request(mem))->validate();
		features_request_rcvd(dynamic_cast<cofmsg_features_request*>( *pmsg ));
	} break;
	case openflow10::OFPT_GET_CONFIG_REQUEST: {
		(*pmsg = new cofmsg_get_config_request(mem))->validate();
		get_config_request_rcvd(dynamic_cast<cofmsg_get_config_request*>( *pmsg ));
	} break;
	case openflow10::OFPT_SET_CONFIG: {
		(*pmsg = new cofmsg_set_config(mem))->validate();
		set_config_rcvd(dynamic_cast<cofmsg_set_config*>( *pmsg ));
	} break;
	case openflow10::OFPT_PACKET_OUT: {
		(*pmsg = new cofmsg_packet_out(mem))->validate();
		packet_out_rcvd(dynamic_cast<cofmsg_packet_out*>( *pmsg ));
	} break;
	case openflow10::OFPT_FLOW_MOD: {
		(*pmsg = new cofmsg_flow_mod(mem))->validate();
		dynamic_cast<cofmsg_flow_mod*>( *pmsg )->get_match().check_prerequisites();
		flow_mod_rcvd(dynamic_cast<cofmsg_flow_mod*>( *pmsg ));
	} break;
	case openflow10::OFPT_PORT_MOD: {
		(*pmsg = new cofmsg_port_mod(mem))->validate();
		port_mod_rcvd(dynamic_cast<cofmsg_port_mod*>( *pmsg ));
	} break;
	case openflow10::OFPT_STATS_REQUEST: {
		if (mem->memlen() < sizeof(struct openflow10::ofp_stats_request)) {
			*pmsg = new cofmsg(mem);
			throw eBadSyntaxTooShort();
		}
		uint16_t stats_type = be16toh(((struct openflow10::ofp_stats_request*)mem->somem())->type);

		switch (stats_type) {
		case openflow10::OFPST_DESC: {
			(*pmsg = new cofmsg_desc_stats_request(mem))->validate();
			desc_stats_request_rcvd(dynamic_cast<cofmsg_desc_stats_request*>( *pmsg ));
		} break;
		case openflow10::OFPST_FLOW: {
			(*pmsg = new cofmsg_flow_stats_request(mem))->validate();
			flow_stats_request_rcvd(dynamic_cast<cofmsg_flow_stats_request*>( *pmsg ));
		} break;
		case openflow10::OFPST_AGGREGATE: {
			(*pmsg = new cofmsg_aggr_stats_request(mem))->validate();
			aggregate_stats_request_rcvd(dynamic_cast<cofmsg_aggr_stats_request*>( *pmsg ));
		} break;
		case openflow10::OFPST_TABLE: {
			(*pmsg = new cofmsg_table_stats_request(mem))->validate();
			table_stats_request_rcvd(dynamic_cast<cofmsg_table_stats_request*>( *pmsg ));
		} break;
		case openflow10::OFPST_PORT: {
			(*pmsg = new cofmsg_port_stats_request(mem))->validate();
			port_stats_request_rcvd(dynamic_cast<cofmsg_port_stats_request*>( *pmsg ));
		} break;
		case openflow10::OFPST_QUEUE: {
			(*pmsg = new cofmsg_queue_stats_request(mem))->validate();
			queue_stats_request_rcvd(dynamic_cast<cofmsg_queue_stats_request*>( *pmsg ));
		} break;
		// TODO: experimenter statistics
		default: {
			(*pmsg = new cofmsg_stats_request(mem))->validate();
			stats_request_rcvd(dynamic_cast<cofmsg_stats_request*>( *pmsg ));
		} break;
		}

	} break;
	case openflow10::OFPT_BARRIER_REQUEST: {
		(*pmsg = new cofmsg_barrier_request(mem))->validate();
		barrier_request_rcvd(dynamic_cast<cofmsg_barrier_request*>( *pmsg ));
	} break;
	case openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		(*pmsg = new cofmsg_queue_get_config_request(mem))->validate();
		queue_get_config_request_rcvd(dynamic_cast<cofmsg_queue_get_config_request*>( *pmsg ));
	} break;
	default: {
		logging::warn << "dropping unknown message " << *mem << std::endl;
		(*pmsg = new cofmsg(mem))->validate();
		throw eBadRequestBadType();
	} break;
	}
}



void
crofctlImpl::parse_of12_message(cmemory *mem, cofmsg **pmsg)
{
	struct openflow::ofp_header* ofh_header = (struct openflow::ofp_header*)mem->somem();

	switch (ofh_header->type) {
	case openflow12::OFPT_HELLO: {
		(*pmsg = new cofmsg_hello(mem))->validate();
		hello_rcvd(dynamic_cast<cofmsg_hello*>( *pmsg ));
	} break;
	case openflow12::OFPT_ECHO_REQUEST: {
		(*pmsg = new cofmsg_echo_request(mem))->validate();
		echo_request_rcvd(dynamic_cast<cofmsg_echo_request*>( *pmsg ));
	} break;
	case openflow12::OFPT_ECHO_REPLY: {
		(*pmsg = new cofmsg_echo_reply(mem))->validate();
		echo_reply_rcvd(dynamic_cast<cofmsg_echo_reply*>( *pmsg ));
	} break;
	case openflow12::OFPT_EXPERIMENTER:	{
		(*pmsg = new cofmsg_experimenter(mem))->validate();
		experimenter_rcvd(dynamic_cast<cofmsg_experimenter*>( *pmsg ));
	} break;
	case openflow12::OFPT_FEATURES_REQUEST:	{
		(*pmsg = new cofmsg_features_request(mem))->validate();
		features_request_rcvd(dynamic_cast<cofmsg_features_request*>( *pmsg ));
	} break;
	case openflow12::OFPT_GET_CONFIG_REQUEST: {
		(*pmsg = new cofmsg_get_config_request(mem))->validate();
		get_config_request_rcvd(dynamic_cast<cofmsg_get_config_request*>( *pmsg ));
	} break;
	case openflow12::OFPT_SET_CONFIG: {
		(*pmsg = new cofmsg_set_config(mem))->validate();
		set_config_rcvd(dynamic_cast<cofmsg_set_config*>( *pmsg ));
	} break;
	case openflow12::OFPT_PACKET_OUT: {
		(*pmsg = new cofmsg_packet_out(mem))->validate();
		packet_out_rcvd(dynamic_cast<cofmsg_packet_out*>( *pmsg ));
	} break;
	case openflow12::OFPT_FLOW_MOD: {
		(*pmsg = new cofmsg_flow_mod(mem))->validate();
		flow_mod_rcvd(dynamic_cast<cofmsg_flow_mod*>( *pmsg ));
	} break;
	case openflow12::OFPT_GROUP_MOD: {
		(*pmsg = new cofmsg_group_mod(mem))->validate();
		group_mod_rcvd(dynamic_cast<cofmsg_group_mod*>( *pmsg ));
	} break;
	case openflow12::OFPT_PORT_MOD: {
		(*pmsg = new cofmsg_port_mod(mem))->validate();
		port_mod_rcvd(dynamic_cast<cofmsg_port_mod*>( *pmsg ));
	} break;
	case openflow12::OFPT_TABLE_MOD: {
		(*pmsg = new cofmsg_table_mod(mem))->validate();
		table_mod_rcvd(dynamic_cast<cofmsg_table_mod*>( *pmsg ));
	} break;
	case openflow12::OFPT_STATS_REQUEST: {

		if (mem->memlen() < sizeof(struct openflow12::ofp_stats_request)) {
			*pmsg = new cofmsg(mem);
			throw eBadSyntaxTooShort();
		}
		uint16_t stats_type = be16toh(((struct openflow12::ofp_stats_request*)mem->somem())->type);

		switch (stats_type) {
		case openflow12::OFPST_DESC: {
			(*pmsg = new cofmsg_desc_stats_request(mem))->validate();
			desc_stats_request_rcvd(dynamic_cast<cofmsg_desc_stats_request*>( *pmsg ));
		} break;
		case openflow12::OFPST_FLOW: {
			(*pmsg = new cofmsg_flow_stats_request(mem))->validate();
			flow_stats_request_rcvd(dynamic_cast<cofmsg_flow_stats_request*>( *pmsg ));
		} break;
		case openflow12::OFPST_AGGREGATE: {
			(*pmsg = new cofmsg_aggr_stats_request(mem))->validate();
			aggregate_stats_request_rcvd(dynamic_cast<cofmsg_aggr_stats_request*>( *pmsg ));
		} break;
		case openflow12::OFPST_TABLE: {
			(*pmsg = new cofmsg_table_stats_request(mem))->validate();
			table_stats_request_rcvd(dynamic_cast<cofmsg_table_stats_request*>( *pmsg ));
		} break;
		case openflow12::OFPST_PORT: {
			(*pmsg = new cofmsg_port_stats_request(mem))->validate();
			port_stats_request_rcvd(dynamic_cast<cofmsg_port_stats_request*>( *pmsg ));
		} break;
		case openflow12::OFPST_QUEUE: {
			(*pmsg = new cofmsg_queue_stats_request(mem))->validate();
			queue_stats_request_rcvd(dynamic_cast<cofmsg_queue_stats_request*>( *pmsg ));
		} break;
		case openflow12::OFPST_GROUP: {
			(*pmsg = new cofmsg_group_stats_request(mem))->validate();
			group_stats_request_rcvd(dynamic_cast<cofmsg_group_stats_request*>( *pmsg ));
		} break;
		case openflow12::OFPST_GROUP_DESC: {
			(*pmsg = new cofmsg_group_desc_stats_request(mem))->validate();
			group_desc_stats_request_rcvd(dynamic_cast<cofmsg_group_desc_stats_request*>( *pmsg ));
		} break;
		case openflow12::OFPST_GROUP_FEATURES: {
			(*pmsg = new cofmsg_group_features_stats_request(mem))->validate();
			group_features_stats_request_rcvd(dynamic_cast<cofmsg_group_features_stats_request*>( *pmsg ));
		} break;
		// TODO: experimenter statistics
		default: {
			(*pmsg = new cofmsg_stats_request(mem))->validate();
			stats_request_rcvd(dynamic_cast<cofmsg_stats_request*>( *pmsg ));
		} break;
		}

	} break;
	case openflow12::OFPT_BARRIER_REQUEST: {
		(*pmsg = new cofmsg_barrier_request(mem))->validate();
		barrier_request_rcvd(dynamic_cast<cofmsg_barrier_request*>( *pmsg ));
	} break;
	case openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		(*pmsg = new cofmsg_queue_get_config_request(mem))->validate();
		queue_get_config_request_rcvd(dynamic_cast<cofmsg_queue_get_config_request*>( *pmsg ));
	} break;
	case openflow12::OFPT_ROLE_REQUEST: {
		(*pmsg = new cofmsg_role_request(mem))->validate();
		role_request_rcvd(dynamic_cast<cofmsg_role_request*>( *pmsg ));
	} break;
    case openflow12::OFPT_GET_ASYNC_REQUEST: {
    	(*pmsg = new cofmsg_get_async_config_request(mem))->validate();
    	get_async_config_request_rcvd(dynamic_cast<cofmsg_get_async_config_request*>( *pmsg ));
    } break;
    case openflow12::OFPT_SET_ASYNC: {
    	(*pmsg = new cofmsg_set_async_config(mem))->validate();
    	set_async_config_rcvd(dynamic_cast<cofmsg_set_async_config*>( *pmsg ));
    } break;
	default: {
		logging::warn << "dropping unknown message " << *mem << std::endl;
		(*pmsg = new cofmsg(mem))->validate();
		throw eBadRequestBadType();
	} break;;
	}
}



void
crofctlImpl::parse_of13_message(cmemory *mem, cofmsg **pmsg)
{
	struct openflow::ofp_header* ofh_header = (struct openflow::ofp_header*)mem->somem();

	switch (ofh_header->type) {
	case openflow13::OFPT_HELLO: {
		(*pmsg = new cofmsg_hello(mem))->validate();
		hello_rcvd(dynamic_cast<cofmsg_hello*>( *pmsg ));
	} break;
	case openflow13::OFPT_ECHO_REQUEST: {
		(*pmsg = new cofmsg_echo_request(mem))->validate();
		echo_request_rcvd(dynamic_cast<cofmsg_echo_request*>( *pmsg ));
	} break;
	case openflow13::OFPT_ECHO_REPLY: {
		(*pmsg = new cofmsg_echo_reply(mem))->validate();
		echo_reply_rcvd(dynamic_cast<cofmsg_echo_reply*>( *pmsg ));
	} break;
	case openflow13::OFPT_EXPERIMENTER:	{
		(*pmsg = new cofmsg_experimenter(mem))->validate();
		experimenter_rcvd(dynamic_cast<cofmsg_experimenter*>( *pmsg ));
	} break;
	case openflow13::OFPT_FEATURES_REQUEST:	{
		(*pmsg = new cofmsg_features_request(mem))->validate();
		features_request_rcvd(dynamic_cast<cofmsg_features_request*>( *pmsg ));
	} break;
	case openflow13::OFPT_GET_CONFIG_REQUEST: {
		(*pmsg = new cofmsg_get_config_request(mem))->validate();
		get_config_request_rcvd(dynamic_cast<cofmsg_get_config_request*>( *pmsg ));
	} break;
	case openflow13::OFPT_SET_CONFIG: {
		(*pmsg = new cofmsg_set_config(mem))->validate();
		set_config_rcvd(dynamic_cast<cofmsg_set_config*>( *pmsg ));
	} break;
	case openflow13::OFPT_PACKET_OUT: {
		(*pmsg = new cofmsg_packet_out(mem))->validate();
		packet_out_rcvd(dynamic_cast<cofmsg_packet_out*>( *pmsg ));
	} break;
	case openflow13::OFPT_FLOW_MOD: {
		(*pmsg = new cofmsg_flow_mod(mem))->validate();
		flow_mod_rcvd(dynamic_cast<cofmsg_flow_mod*>( *pmsg ));
	} break;
	case openflow13::OFPT_GROUP_MOD: {
		(*pmsg = new cofmsg_group_mod(mem))->validate();
		group_mod_rcvd(dynamic_cast<cofmsg_group_mod*>( *pmsg ));
	} break;
	case openflow13::OFPT_PORT_MOD: {
		(*pmsg = new cofmsg_port_mod(mem))->validate();
		port_mod_rcvd(dynamic_cast<cofmsg_port_mod*>( *pmsg ));
	} break;
	case openflow13::OFPT_TABLE_MOD: {
		(*pmsg = new cofmsg_table_mod(mem))->validate();
		table_mod_rcvd(dynamic_cast<cofmsg_table_mod*>( *pmsg ));
	} break;
	case openflow13::OFPT_STATS_REQUEST: {

		if (mem->memlen() < sizeof(struct openflow13::ofp_multipart_request)) {
			*pmsg = new cofmsg(mem);
			throw eBadSyntaxTooShort();
		}
		uint16_t stats_type = be16toh(((struct openflow13::ofp_multipart_request*)mem->somem())->type);

		switch (stats_type) {
		case openflow13::OFPST_DESC: {
			(*pmsg = new cofmsg_desc_stats_request(mem))->validate();
			desc_stats_request_rcvd(dynamic_cast<cofmsg_desc_stats_request*>( *pmsg ));
		} break;
		case openflow13::OFPST_FLOW: {
			(*pmsg = new cofmsg_flow_stats_request(mem))->validate();
			flow_stats_request_rcvd(dynamic_cast<cofmsg_flow_stats_request*>( *pmsg ));
		} break;
		case openflow13::OFPST_AGGREGATE: {
			(*pmsg = new cofmsg_aggr_stats_request(mem))->validate();
			aggregate_stats_request_rcvd(dynamic_cast<cofmsg_aggr_stats_request*>( *pmsg ));
		} break;
		case openflow13::OFPST_TABLE: {
			(*pmsg = new cofmsg_table_stats_request(mem))->validate();
			table_stats_request_rcvd(dynamic_cast<cofmsg_table_stats_request*>( *pmsg ));
		} break;
		case openflow13::OFPST_PORT: {
			(*pmsg = new cofmsg_port_stats_request(mem))->validate();
			port_stats_request_rcvd(dynamic_cast<cofmsg_port_stats_request*>( *pmsg ));
		} break;
		case openflow13::OFPST_QUEUE: {
			(*pmsg = new cofmsg_queue_stats_request(mem))->validate();
			queue_stats_request_rcvd(dynamic_cast<cofmsg_queue_stats_request*>( *pmsg ));
		} break;
		case openflow13::OFPST_GROUP: {
			(*pmsg = new cofmsg_group_stats_request(mem))->validate();
			group_stats_request_rcvd(dynamic_cast<cofmsg_group_stats_request*>( *pmsg ));
		} break;
		case openflow13::OFPST_GROUP_DESC: {
			(*pmsg = new cofmsg_group_desc_stats_request(mem))->validate();
			group_desc_stats_request_rcvd(dynamic_cast<cofmsg_group_desc_stats_request*>( *pmsg ));
		} break;
		case openflow13::OFPST_GROUP_FEATURES: {
			(*pmsg = new cofmsg_group_features_stats_request(mem))->validate();
			group_features_stats_request_rcvd(dynamic_cast<cofmsg_group_features_stats_request*>( *pmsg ));
		} break;
		// TODO: experimenter statistics
		default: {
			(*pmsg = new cofmsg_stats_request(mem))->validate();
			stats_request_rcvd(dynamic_cast<cofmsg_stats_request*>( *pmsg ));
		} break;
		}

	} break;
	case openflow13::OFPT_BARRIER_REQUEST: {
		(*pmsg = new cofmsg_barrier_request(mem))->validate();
		barrier_request_rcvd(dynamic_cast<cofmsg_barrier_request*>( *pmsg ));
	} break;
	case openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST: {
		(*pmsg = new cofmsg_queue_get_config_request(mem))->validate();
		queue_get_config_request_rcvd(dynamic_cast<cofmsg_queue_get_config_request*>( *pmsg ));
	} break;
	case openflow13::OFPT_ROLE_REQUEST: {
		(*pmsg = new cofmsg_role_request(mem))->validate();
		role_request_rcvd(dynamic_cast<cofmsg_role_request*>( *pmsg ));
	} break;
    case openflow13::OFPT_GET_ASYNC_REQUEST: {
    	(*pmsg = new cofmsg_get_async_config_request(mem))->validate();
    	get_async_config_request_rcvd(dynamic_cast<cofmsg_get_async_config_request*>( *pmsg ));
    } break;
    case openflow13::OFPT_SET_ASYNC: {
    	(*pmsg = new cofmsg_set_async_config(mem))->validate();
    	set_async_config_rcvd(dynamic_cast<cofmsg_set_async_config*>( *pmsg ));
    } break;
	default: {
		logging::warn << "dropping unknown message " << *mem << std::endl;
		(*pmsg = new cofmsg(mem))->validate();
		throw eBadRequestBadType();
	} break;;
	}
}



void
crofctlImpl::hello_rcvd(cofmsg_hello *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Hello message received" << std::endl << *msg << std::endl;

	try {
		// OpenFlow versions not supported on our side, send error, close connection
		if (not rofbase->is_ofp_version_supported(msg->get_version()))
		{
			new_state(STATE_CTL_DISCONNECTED);

			// invalid OFP_VERSION
			char explanation[256];
			bzero(explanation, sizeof(explanation));
			snprintf(explanation, sizeof(explanation) - 1,
							"unsupported OF version (%d), supported version is (%d)",
							(msg->get_version()), openflow12::OFP_VERSION);

			throw eHelloIncompatible();
		}
		else
		{
			// TODO: determine properly version from hello elements in OpenFlow 1.3
			ofp_version = msg->get_version();

			flags.set(COFCTL_FLAG_HELLO_RCVD);

			new_state(STATE_CTL_ESTABLISHED);

			logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
					<< " Hello exchanged with peer entity, attached. " << *this << std::endl;

			if (flags.test(COFCTL_FLAG_HELLO_SENT)) {
				register_timer(COFCTL_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);

				logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << "" << *this << indent(2)
						<< "Hello exchanged with peer entity (disconnected -> connected)" << std::endl;

				rofbase->handle_ctl_open(this);
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
crofctlImpl::echo_request_sent(cofmsg *pack)
{
	reset_timer(COFCTL_TIMER_ECHO_REPLY_TIMEOUT, echo_reply_timeout); // TODO: multiple concurrent echo-requests?
}



void
crofctlImpl::echo_request_rcvd(cofmsg_echo_request *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Echo-Request message received" << std::endl << *msg << std::endl;

	// send echo reply back including any appended data
	rofbase->send_echo_reply(this, msg->get_xid(), msg->get_body().somem(), msg->get_body().memlen());

	delete msg;

	/* Please note: we do not call a handler for echo-requests/replies in rofbase
	 * and take care of these liveness packets within cofctl and cofdpt
	 */
}



void
crofctlImpl::echo_reply_rcvd(cofmsg_echo_reply *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Echo-Reply message received" << std::endl << *msg << std::endl;

	cancel_timer(COFCTL_TIMER_ECHO_REPLY_TIMEOUT);
	register_timer(COFCTL_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);

	delete msg;
}



void
crofctlImpl::features_request_rcvd(cofmsg_features_request *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Features-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid(), 0);

	} catch (eXidStoreXidBusy& e) {
		logging::debug << "retransmitted Features-Request " << *msg << std::endl;
	}

	rofbase->handle_features_request(this, msg);
}



void
crofctlImpl::features_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " spurious xid in reply to Features-Request " << *msg << std::endl;
	}
}



void
crofctlImpl::check_role()
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: {
		if (openflow12::OFPCR_ROLE_SLAVE == role)
			throw eBadRequestIsSlave();
	} break;
	case openflow13::OFP_VERSION: {
		if (openflow12::OFPCR_ROLE_SLAVE == role)
			throw eBadRequestIsSlave();
	} break;
	}
}



void
crofctlImpl::get_config_request_rcvd(cofmsg_get_config_request *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Get-Config-Request message received" << std::endl << *msg << std::endl;

	check_role();

	rofbase->handle_get_config_request(this, msg);
}



void
crofctlImpl::get_config_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " spurious xid in reply to Get-Config-Request " << *msg << std::endl;
	}
}



void
crofctlImpl::set_config_rcvd(cofmsg_set_config *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Set-Config message received" << std::endl << *msg << std::endl;

	try {
		check_role();

		rofbase->handle_set_config(this, msg);

	} catch (eSwitchConfigBadFlags& e) {

		logging::warn << "eSwitchConfigBadFlags " << *msg << std::endl;
		rofbase->send_error_switch_config_failed_bad_flags(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eSwitchConfigBadLen& e) {

		logging::warn << "eSwitchConfigBadLen " << *msg << std::endl;
		rofbase->send_error_switch_config_failed_bad_len(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eSwitchConfigBase& e) {

		logging::warn << "eSwitchConfigBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctlImpl::packet_out_rcvd(cofmsg_packet_out *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Packet-Out message received" << std::endl << *msg << std::endl;

	check_role();

	rofbase->handle_packet_out(this, msg);
}



void
crofctlImpl::flow_mod_rcvd(cofmsg_flow_mod *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Flow-Mod message received" << std::endl << *msg << std::endl;

	try {
		check_role();

		// check, whether the controlling pack->entity is allowed to install this flow-mod
		if (rofbase->fe_flags.test(crofbase::NSP_ENABLED)) {
			switch (get_version()) {
			case openflow10::OFP_VERSION: {
				switch (msg->get_command()) {
				case openflow10::OFPFC_ADD:
				case openflow10::OFPFC_MODIFY:
				case openflow10::OFPFC_MODIFY_STRICT: {
#if 0
					/* deactivated for Darmstadt demonstration, needs exact fix */
					rofbase->fsptable.flow_mod_allowed(this, msg->match);
#endif
				} break;
				/*
				 * this allows generic DELETE commands to be applied
				 * FIXME: does this affect somehow entries from other controllers?
				 */
				}
			} break;
			case openflow12::OFP_VERSION: {
				switch (msg->get_command()) {
				case openflow12::OFPFC_ADD:
				case openflow12::OFPFC_MODIFY:
				case openflow12::OFPFC_MODIFY_STRICT: {
#if 0
					/* deactivated for Darmstadt demonstration, needs exact fix */
					rofbase->fsptable.flow_mod_allowed(this, msg->match);
#endif
				} break;
				/*
				 * this allows generic DELETE commands to be applied
				 * FIXME: does this affect somehow entries from other controllers?
				 */
				}
			} break;
			case openflow13::OFP_VERSION: {
				switch (msg->get_command()) {
				case openflow13::OFPFC_ADD:
				case openflow13::OFPFC_MODIFY:
				case openflow13::OFPFC_MODIFY_STRICT: {
#if 0
					/* deactivated for Darmstadt demonstration, needs exact fix */
					rofbase->fs<ptable.flow_mod_allowed(this, msg->match);
#endif
				} break;
				/*
				 * this allows generic DELETE commands to be applied
				 * FIXME: does this affect somehow entries from other controllers?
				 */
				}
			} break;
			}
		}

		rofbase->handle_flow_mod(this, msg);

	} catch (eFlowModUnknown& e) {

		logging::warn << "eFlowModUnknown " << *msg << std::endl;
		rofbase->send_error_flow_mod_failed_unknown(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eFlowModTableFull& e) {

		logging::warn << "eFlowModTableFull " << *msg << std::endl;
		rofbase->send_error_flow_mod_failed_table_full(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eFlowModBadTableId& e) {

		logging::warn << "eFlowModBadTableId " << *msg << std::endl;
		rofbase->send_error_flow_mod_failed_bad_table_id(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eFlowModOverlap& e) {

		logging::warn << "eFlowModOverlap " << *msg << std::endl;
		rofbase->send_error_flow_mod_failed_overlap(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eFlowModEperm& e) {

		logging::warn << "eFlowModEperm " << *msg << std::endl;
		rofbase->send_error_flow_mod_failed_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eFlowModBadTimeout&e ) {

		logging::warn << "eFlowModBadTimeout " << *msg << std::endl;
		rofbase->send_error_flow_mod_failed_bad_timeout(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eFlowModBadCommand& e) {

		logging::warn << "eFlowModBadCommand " << *msg << std::endl;
		rofbase->send_error_flow_mod_failed_bad_command(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eFlowModBase& e) {

		logging::warn << "eFlowModBase " << *msg << std::endl;
		delete msg;

	} catch (eFspNotAllowed& e) {

		logging::warn << "eFspNotAllowed " << *msg << " fsptable:" << rofbase->fsptable << std::endl;
		rofbase->send_error_flow_mod_failed_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eRofBaseTableNotFound& e) {

		logging::warn << "eRofBaseTableNotFound " << *msg << std::endl;
		rofbase->send_error_flow_mod_failed_bad_table_id(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eInstructionInvalType& e) {

		logging::warn << "eInstructionInvalType " << *msg << std::endl;
		rofbase->send_error_bad_inst_unknown_inst(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eRofBaseGotoTableNotFound& e) {

		logging::warn << "eRofBaseGotoTableNotFound " << *msg << std::endl;
		rofbase->send_error_bad_inst_bad_table_id(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eInstructionBadExperimenter& e) {

		logging::warn << "eInstructionBadExperimenter " << *msg << std::endl;
		rofbase->send_error_bad_inst_unsup_exp_inst(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eOFmatchInvalBadValue& e) {

		logging::warn << "eOFmatchInvalBadValue " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_value(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;
	}
}



void
crofctlImpl::group_mod_rcvd(cofmsg_group_mod *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Group-Mod message received" << std::endl << *msg << std::endl;

	try {
		check_role();

		rofbase->handle_group_mod(this, msg);

	} catch (eGroupModExists& e) {

		logging::warn << "eGroupModExists " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_group_exists(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModInvalGroup& e) {

		logging::warn << "eGroupModInvalGroup " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_inval_group(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModWeightUnsupported& e) {

		logging::warn << "eGroupModWeightUnsupported " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_weight_unsupported(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModOutOfGroups& e) {

		logging::warn << "eGroupModOutOfGroups " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_out_of_groups(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModOutOfBuckets& e) {

		logging::warn << "eGroupModOutOfBuckets " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_out_of_buckets(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModChainingUnsupported& e) {

		logging::warn << "eGroupModChainingUnsupported " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_chaining_unsupported(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModWatchUnsupported& e) {

		logging::warn << "eGroupModWatchUnsupported " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_watch_unsupported(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModLoop& e) {

		logging::warn << "eGroupModLoop " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_loop(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModUnknownGroup& e) {

		logging::warn << "eGroupModUnknownGroup " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_unknown_group(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModChainedGroup& e) {

		logging::warn << "eGroupModChainedGroup " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_chained_group(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModBadType& e) {

		logging::warn << "eGroupModBadType " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_bad_type(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModBadCommand& e) {

		logging::warn << "eGroupModBadCommand " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_bad_command(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModBadBucket& e) {

		logging::warn << "eGroupModBadBucket " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_bad_bucket(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModBadWatch& e) {

		logging::warn << "eGroupModBadWatch " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_bad_watch(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModEperm& e) {

		logging::warn << "eGroupModEperm " << *msg << std::endl;
		rofbase->send_error_group_mod_failed_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eGroupModBase& e) {

		logging::warn << "eGroupModBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctlImpl::port_mod_rcvd(cofmsg_port_mod *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Port-Mod message received" << std::endl << *msg << std::endl;

	try {
		check_role();

		rofbase->handle_port_mod(this, msg);

	} catch (ePortModBadPort& e) {

		logging::warn << "ePortModBadPort " << *msg << std::endl;
		rofbase->send_error_port_mod_failed_bad_port(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (ePortModBadHwAddr& e) {

		logging::warn << "ePortModBadHwAddr " << *msg << std::endl;
		rofbase->send_error_port_mod_failed_bad_hw_addr(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (ePortModBadConfig& e) {

		logging::warn << "ePortModBadConfig " << *msg << std::endl;
		rofbase->send_error_port_mod_failed_bad_config(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (ePortModBadAdvertise& e) {

		logging::warn << "ePortModBadAdvertise " << *msg << std::endl;
		rofbase->send_error_port_mod_failed_bad_advertise(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (ePortModBase& e) {

		logging::warn << "ePortModBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctlImpl::table_mod_rcvd(cofmsg_table_mod *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Table-Mod message received" << std::endl << *msg << std::endl;

	try {
		check_role();

		rofbase->handle_table_mod(this, msg);

	} catch (eTableModBadTable& e) {

		logging::warn << "eTableModBadTable " << *msg << std::endl;
		rofbase->send_error_table_mod_failed_bad_table(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eTableModBadConfig& e) {

		logging::warn << "eTableModBadConfig " << *msg << std::endl;
		rofbase->send_error_table_mod_failed_bad_config(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eTableModBase& e) {

		logging::warn << "eTableModBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctlImpl::stats_request_rcvd(cofmsg_stats *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_stats_request(this, msg);
}



void
crofctlImpl::desc_stats_request_rcvd(cofmsg_desc_stats_request *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Desc-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Desc-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_desc_stats_request(this, msg);
}



void
crofctlImpl::table_stats_request_rcvd(cofmsg_table_stats_request* msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Table-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Table-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_table_stats_request(this, msg);
}



void
crofctlImpl::port_stats_request_rcvd(cofmsg_port_stats_request* msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Port-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Port-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_port_stats_request(this, msg);
}



void
crofctlImpl::flow_stats_request_rcvd(cofmsg_flow_stats_request* msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Flow-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Flow-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_flow_stats_request(this, msg);
}



void
crofctlImpl::aggregate_stats_request_rcvd(cofmsg_aggr_stats_request* msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Aggregate-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Aggregate-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_aggregate_stats_request(this, msg);
}



void
crofctlImpl::queue_stats_request_rcvd(cofmsg_queue_stats_request* msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Queue-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Queue-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_queue_stats_request(this, msg);
}



void
crofctlImpl::group_stats_request_rcvd(cofmsg_group_stats_request* msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Group-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Group-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_group_stats_request(this, msg);
}



void
crofctlImpl::group_desc_stats_request_rcvd(cofmsg_group_desc_stats_request* msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Group-Desc-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Group-Desc-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_group_desc_stats_request(this, msg);
}



void
crofctlImpl::group_features_stats_request_rcvd(cofmsg_group_features_stats_request* msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Group-Features-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Group-Features-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_group_features_stats_request(this, msg);
}



void
crofctlImpl::experimenter_stats_request_rcvd(cofmsg_experimenter_stats_request* msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Experimenter-Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Experimenter-Stats-Request " << *msg << std::endl;
	}

	rofbase->handle_experimenter_stats_request(this, msg);
}



void
crofctlImpl::stats_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " spurious xid in reply to Stats-Request " << *msg << std::endl;
	}
}



void
crofctlImpl::role_request_rcvd(cofmsg_role_request *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Role-Request message received" << std::endl << *msg << std::endl;

	try {
		try {
			xidstore.xid_add(this, msg->get_xid());

		} catch (eXidStoreXidBusy& e) {
			logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
					<< " retransmitted Role-Request " << *msg << std::endl;
		}

		switch (msg->get_role()) {
		case openflow12::OFPCR_ROLE_MASTER:
		case openflow12::OFPCR_ROLE_SLAVE:
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
		for (std::map<cofbase*, crofctl*>::iterator
				it = rofbase->ofctrl_list.begin(); it != rofbase->ofctrl_list.end(); ++it)
		{
			crofctl* ofctrl = it->second;

			if (ofctrl == this)
			{
				continue;
			}

			if (openflow12::OFPCR_ROLE_MASTER == ofctrl->role)
			{
				ofctrl->role = openflow12::OFPCR_ROLE_SLAVE;
			}
		}
#endif

		//pack->ofh_role_request->generation_id;

		rofbase->role_request_rcvd(this, role);

		rofbase->handle_role_request(this, msg);

	} catch (eRoleRequestStale& e) {

		logging::warn << "eRoleRequestStale " << *msg << std::endl;
		rofbase->send_error_role_request_failed_stale(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eRoleRequestUnsupported& e) {

		logging::warn << "eRoleRequestUnsupported " << *msg << std::endl;
		rofbase->send_error_role_request_failed_unsupported(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eRoleRequestBadRole& e) {

		logging::warn << "eRoleRequestBadRole " << *msg << std::endl;
		rofbase->send_error_role_request_failed_bad_role(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eRoleRequestBase& e) {

		logging::warn << "eRoleRequestBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctlImpl::role_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " spurious xid in reply to Role-Request " << *msg << std::endl;
	}
}



void
crofctlImpl::barrier_request_rcvd(cofmsg_barrier_request *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Barrier-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Barrier-Request " << *msg << std::endl;
	}

	rofbase->handle_barrier_request(this, msg);
}



void
crofctlImpl::barrier_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " spurious xid in reply to Barrier-Request " << *msg << std::endl;
	}
}



void
crofctlImpl::queue_get_config_request_rcvd(cofmsg_queue_get_config_request *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Queue-Get-Config-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Queue-Get-Config-Request " << *msg << std::endl;
	}

	rofbase->handle_queue_get_config_request(this, msg);
}



void
crofctlImpl::queue_get_config_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " spurious xid in reply to Queue-Get-Config-Request " << *msg << std::endl;
	}
}



void
crofctlImpl::experimenter_rcvd(cofmsg_experimenter *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Experimenter message received" << std::endl << *msg << std::endl;

	switch (msg->get_experimenter_id()) {
	case OFPEXPID_ROFL: {
		switch (msg->get_experimenter_type()) {
		case croflexp::OFPRET_FLOWSPACE: {
			croflexp rexp(msg->get_body().somem(), msg->get_body().memlen());

			switch (rexp.rext_fsp->command) {
			case croflexp::OFPRET_FSP_ADD:
			try {

				rofbase->fsptable.insert_fsp_entry(this, rexp.match);

				logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
						<< "flowspace registration accepted" << std::endl << indent(2) << rexp.match;

			} catch (eFspEntryOverlap& e) {

				logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
						<< "flowspace registration rejected" << std::endl << indent(2) << rexp.match;

			} break;
			case croflexp::OFPRET_FSP_DELETE:
			try {

				rofbase->fsptable.delete_fsp_entry(this, rexp.match, true /*strict*/);

				logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
						<< "flowspace registration deleted" << std::endl << indent(2) << rexp.match;

			} catch (eFspEntryNotFound& e) {

				logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
						<< "flowspace registration deletion failed" << std::endl << indent(2) << rexp.match;

			} break;
			default:
				break;
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
crofctlImpl::handle_echo_reply_timeout()
{
	logging::warn << "[rofl][dpt] ctid:0x" << std::hex << ctid << std::dec << " Echo-Reply timer expired" << *this << std::endl;

	// TODO: repeat ECHO request multiple times (should be configurable)

	socket->cclose();

	if (flags.test(COFCTL_FLAG_ACTIVE_SOCKET)) {
		try_to_connect(true);
	}
	rofbase->handle_ctl_close(this);
}



void
crofctlImpl::get_async_config_request_rcvd(cofmsg_get_async_config_request *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Get-Async-Config-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Get-Async-Config-Request " << *msg << std::endl;
	}

	// TODO: handle request
}



void
crofctlImpl::set_async_config_rcvd(cofmsg_set_async_config *msg)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Set-Async-Config message received" << std::endl << *msg << std::endl;

	// TODO: handle request here in this cofctl instance
	rofbase->handle_set_async_config(this, msg);
}



void
crofctlImpl::get_async_config_reply_sent(cofmsg *msg)
{
	uint32_t xid = msg->get_xid();
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " spurious xid in reply to Get-Async-Config-Request " << *msg << std::endl;
	}
}



cxidtrans&
crofctlImpl::transaction(uint32_t xid)
{
	return xidstore.xid_find(xid);
}



void
crofctlImpl::send_error_is_slave(cofmsg *pack)
{
	size_t len = (pack->length() > 64) ? 64 : pack->length();
	rofbase->send_error_bad_request_is_slave(this, pack->get_xid(), pack->soframe(), len);
}



void
crofctlImpl::try_to_connect(bool reset_timeout)
{
	if (pending_timer(COFCTL_TIMER_RECONNECT)) {
		return;
	}

	logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " scheduled reconnect in " << (int)reconnect_in_seconds
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

	reset_timer(COFCTL_TIMER_RECONNECT, reconnect_in_seconds);

	++reconnect_counter;
}



void
crofctlImpl::send_message_via_socket(
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


