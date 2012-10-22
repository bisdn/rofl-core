/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofrpc.h"

/* static */int cofrpc::rpc_echo_interval = DEFAULT_RPC_ECHO_INTERVAL;

cofrpc::cofrpc(int rpc_endpnt, cofiface *entity) :
		rpc_endpnt(rpc_endpnt),
		entity(entity),
		fragment(NULL),
		reconnect_in_seconds(RECONNECT_START_TIMEOUT),
		reconnect_counter(0)
{
	pthread_mutex_init(&fe_queue_mutex, NULL);
	flags.set(COFRPC_FLAG_SERVER_SOCKET);
	init_state(STATE_RPC_DISCONNECTED);
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc::cofrpc(%p) %s", this,
			(rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) ?
					"-north- accepts connections from datapath elements" :
					"-south- accepts connections from controller entities");
}


cofrpc::cofrpc(
		int rpc_endpnt,
		cofiface *entity,
		int sd,
		caddress ra,
		int domain,
		int type,
		int protocol,
		bool active) :
		csocket(sd, ra, domain, type, protocol),
		rpc_endpnt(rpc_endpnt),
		entity(entity),
		fragment(NULL),
		reconnect_in_seconds(RECONNECT_START_TIMEOUT),
		reconnect_counter(0)
{
	pthread_mutex_init(&fe_queue_mutex, NULL);
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc::cofrpc(%p) %s connection established to %s (sd:%d)",
			this,
			(rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) ? "-north-" : "-south-",
			ra.c_str(), sd);

	flags.set(COFRPC_FLAG_WORKER_SOCKET);

	sockflags.set(CONNECTED);

	init_state(STATE_RPC_CONNECTED);

	register_timer(TIMER_RPC_SEND_HELLO, 0);
}


cofrpc::~cofrpc()
{
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc::~cofrpc(%p) %s", this,
			(rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) ? "north" : "south");

	cclose();

	switch (rpc_endpnt) {
	case OF_RPC_TCP_NORTH_ENDPNT:
		send_up_hello_message(entity, true);
		break;
	case OF_RPC_TCP_SOUTH_ENDPNT:
		send_down_hello_message(entity, true);
		break;
	}

	while (not fe_queue.empty())
	{
		delete fe_queue.front();
		fe_queue.pop_front();
	}

	pthread_mutex_destroy(&fe_queue_mutex);
}


void
cofrpc::send_up_hello_message(
		cofiface *entity,
		bool bye)
{
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_up_hello_message()", this);
#if 0
	size_t packlen = sizeof(struct ofp_header) + sizeof(uint32_t);

	cofpacket *pack = new cofpacket(packlen, packlen);

	pack->ofh_header->version = OFP_VERSION;
	pack->ofh_header->type = OFPT_HELLO;
	pack->ofh_header->length = htobe16(packlen);
	pack->ofh_header->xid = htobe32(ta_new_async_xid());

	/* we use a cookie in HELLO messages to attach/detach from a
	 * adjacent  (layer N+1 or N-1)
	 * implementations must ignore any appended data beyond the plain
	 * OFP-header in a HELLO message according to OF1.1
	 *
	 */
	uint32_t *cookie = (uint32_t*)(pack->ofh_header + 1);
	*cookie = bye ? htobe32(FE_HELLO_BYE) : htobe32(FE_HELLO_ACTIVE);

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_up_hello_message() done %s",
			this, pack->c_str());

	entity->fe_up_hello_message(this, pack);
#endif

	uint32_t cookie = (bye == true) ? htobe32(FE_HELLO_BYE) : htobe32(FE_HELLO_ACTIVE);

	cofpacket *pack = new cofpacket_hello(ta_new_async_xid(), (uint8_t*)&cookie, sizeof(cookie));

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_up_hello_message() done %s",
			this, pack->c_str());

	entity->fe_up_hello_message(this, pack);
}


void
cofrpc::send_down_hello_message(
		cofiface *entity,
		bool bye)
{
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_down_hello_message()", this);
#if 0
	size_t packlen = sizeof(struct ofp_header) + sizeof(uint32_t);

	cofpacket *pack = new cofpacket(packlen, packlen);

	pack->ofh_header->version = OFP_VERSION;
	pack->ofh_header->type = OFPT_HELLO;
	pack->ofh_header->length = htobe16(packlen);
	pack->ofh_header->xid = htobe32(ta_new_async_xid());

	/* we use a cookie in HELLO messages to attach/detach from a
	 * adjacent cfwdelem (layer N+1 or N-1)
	 * implementations must ignore any appended data beyond the plain
	 * OFP-header in a HELLO message according to OF1.1
	 *
	 */
	uint32_t *cookie = (uint32_t*)(pack->ofh_header + 1);
	*cookie = bye ? htobe32(FE_HELLO_BYE) : htobe32(FE_HELLO_ACTIVE);

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_down_hello_message() done %s",
			this, pack->c_str());

	entity->fe_down_hello_message(this, pack);
#endif

	uint32_t cookie = (bye == true) ? htobe32(FE_HELLO_BYE) : htobe32(FE_HELLO_ACTIVE);

	cofpacket *pack = new cofpacket_hello(ta_new_async_xid(), (uint8_t*)&cookie, sizeof(cookie));

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_down_hello_message() done %s",
			this, pack->c_str());

	entity->fe_down_hello_message(this, pack);
}


void
cofrpc::fe_down_hello_message(
		cofiface *entity,
		cofpacket *pack)
{
	if (OFPT_HELLO != pack->ofh_header->type)
	{
		delete pack;
		return;
	}

	uint32_t cookie = 0;

	if (pack->body.memlen() >= sizeof(uint32_t))
	{
		memcpy(&cookie, pack->body.somem(), sizeof(uint32_t));
		cookie = be32toh(cookie);
	}

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::fe_down_hello_message() down pack:%s cookie:%s",
			this, pack->c_str(), cookie == FE_HELLO_ACTIVE ? "FE_HELLO_ACTIVE" : "FE_HELLO_BYE");

	switch (cookie) {
	case FE_HELLO_BYE: // close TCP connection
		{
			if (flags.test(COFRPC_FLAG_WORKER_SOCKET))
			{
				delete this; return;
			}
		}
		break;

	case FE_HELLO_ACTIVE:
		{
			// do nothing
		}
		break;

	default:
		{

		}
		break;
	}

	delete pack;
}


void
cofrpc::fe_up_hello_message(
		cofiface *entity,
		cofpacket *pack)
{
	if (OFPT_HELLO != pack->ofh_header->type)
	{
		delete pack;
		return;
	}

	uint32_t cookie = 0;

	if (pack->body.memlen() >= sizeof(uint32_t))
	{
		memcpy(&cookie, pack->body.somem(), sizeof(uint32_t));
		cookie = be32toh(cookie);
	}

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::fe_up_hello_message() up pack:%s cookie:%s",
			this, pack->c_str(), cookie == FE_HELLO_ACTIVE ? "FE_HELLO_ACTIVE" : "FE_HELLO_BYE");

	switch (cookie) {
	case FE_HELLO_BYE: // close TCP connection
		{
			if (flags.test(COFRPC_FLAG_WORKER_SOCKET))
			{
				delete this; return;
			}
		}
		break;

	case FE_HELLO_ACTIVE:
		{
			// do nothing
		}
		break;

	default:
		{

		}
		break;
	}

	delete pack;
}


void
cofrpc::clisten(caddress const& la)
{
	try {

		laddr = la;

		flags.set(COFRPC_FLAG_SERVER_SOCKET);
		flags.reset(COFRPC_FLAG_WORKER_SOCKET);

		cpopen(la, AF_INET, SOCK_STREAM, 0, 10);

	} catch (eSocketError& e) {
		WRITELOG(COFRPC, ROFL_WARN, "cofrpc(%p)::cofrpc() unable to open listening socket", this);
		throw;
	}
}


void
cofrpc::cconnect(caddress const& ra)
{
	try {
		if (!entity)
			return;

		raddr = ra;

		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::cconnect() (re-)connecting to %s", this,
				raddr.c_str());

		if (flags.test(COFRPC_FLAG_SERVER_SOCKET))
		{
			cofrpc *rpc = new cofrpc(rpc_endpnt, entity);
			rpc->caopen(ra, caddress(AF_INET, "0.0.0.0", 0), PF_INET, SOCK_STREAM, 0);
			rpc->flags.reset(COFRPC_FLAG_SERVER_SOCKET);
			rpc->flags.set(COFRPC_FLAG_WORKER_SOCKET);
			rpc->flags.set(COFRPC_FLAG_ACTIVE_SOCKET);
		}
		else if (flags.test(COFRPC_FLAG_WORKER_SOCKET))
		{
			flags.reset(COFRPC_FLAG_SERVER_SOCKET);
			flags.set(COFRPC_FLAG_WORKER_SOCKET);
			flags.set(COFRPC_FLAG_ACTIVE_SOCKET);

			new_state(STATE_RPC_CONNECTING);
			caddress la(AF_INET, "0.0.0.0", 0);
			caopen(ra, la, PF_INET, SOCK_STREAM, 0);
		}

	} catch (eSocketBase& e) {
		WRITELOG(COFRPC, ROFL_WARN, "unable to connect to %s "
				"via socket %s", raddr.c_str());
		throw;
	}
}


void
cofrpc::handle_connected()
{
	cancel_timer(TIMER_RPC_RECONNECT);

	new_state(STATE_RPC_CONNECTED);

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc::handle_connected() connection established to %s (sd:%d)",
			raddr.c_str(), sd);

	register_timer(TIMER_RPC_SEND_HELLO, 0); // sends HELLO to peer and starts OF negotiation
}


void
cofrpc::handle_conn_refused()
{
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc::handle_conn_refused() %s", raddr.c_str());

	try_to_connect();
}


void cofrpc::handle_closed(int sd)
{
	cancel_all_timer();

	WRITELOG(COFRPC, ROFL_WARN, "cofrpc::handle_closed() %s", raddr.c_str());

	new_state(STATE_RPC_DISCONNECTED);

	if (flags.test(COFRPC_FLAG_SERVER_SOCKET))
	{
		return;
	}

	switch (rpc_endpnt) {
	case OF_RPC_TCP_NORTH_ENDPNT:
		{
			send_up_hello_message(entity, true);
		}
		break;
	case OF_RPC_TCP_SOUTH_ENDPNT:
		{
			send_down_hello_message(entity, true);
		}
		break;
	}

	if (flags.test(COFRPC_FLAG_ACTIVE_SOCKET))
	{
		WRITELOG(COFRPC, ROFL_WARN, "cofrpc::handle_closed() ACTIVE socket, "
				"reconnecting to %s", raddr.c_str());
		register_timer(TIMER_RPC_RECONNECT, 5);
	}
	else
	{
		WRITELOG(COFRPC, ROFL_WARN, "cofrpc::handle_closed() PASSIVE socket, "
				"closing to %s", raddr.c_str());
		delete this;
	}
}


void
cofrpc::handle_event(cevent const& ev)
{
	switch (ev.cmd) {
	case COFRPC_EVENT_SEND_VIA_TCP:
		send_message_via_tcp();
		break;
	}
}


void
cofrpc::handle_timeout(int opaque)
{
	switch (opaque) {
 	case TIMER_RPC_RECONNECT:
		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::handle_timeout() "
				"TIMER_RPC_RECONNECT (0x%x) expired", this, opaque);
		if (flags.test(COFRPC_FLAG_WORKER_SOCKET)) {
			cconnect(raddr);
		}
		break;
	case TIMER_RPC_SEND_HELLO:
		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::handle_timeout() "
				"TIMER_RPC_SEND_HELLO (0x%x) expired", this, opaque);
		send_hello_message_via_tcp();
		break;
	case TIMER_RPC_SEND_VIA_TCP:
		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::handle_timeout() "
				"TIMER_RPC_SEND_VIA_TCP (0x%x) expired", this, opaque);
		send_message_via_tcp();
		break;
	case TIMER_RPC_CLOSE_CONNECTION:
		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::handle_timeout() "
				"TIMER_RPC_CLOSE_CONNECTION (0x%x) expired", this, opaque);
		cclose();
		handle_closed(sd);

		break;
	case TIMER_RPC_SEND_ECHO_REQUEST:
		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::handle_timeout() "
				"TIMER_RPC_SEND_ECHO_REQUEST (0x%x) expired", this, opaque);
		send_echo_request();
		register_timer(TIMER_RPC_SEND_ECHO_REQUEST, rpc_echo_interval);
		break;
	case TIMER_RPC_ECHO_REPLY_TIMEOUT:
		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::handle_timeout() "
				"TIMER_RPC_SEND_ECHO_REPLY_TIMEOUT (0x%x) expired", this, opaque);
		handle_echo_reply_timeout();
		break;

	case TIMER_RPC_ESTABLISH:
		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::handle_timeout() "
				"TIMER_RPC_ESTABLISH (0x%x) expired", this, opaque);
		register_filedesc_r(sd);
		break;
	default:

		break;
	}
}


void
cofrpc::handle_accepted(int newsd, caddress &ra)
{
	if (!entity)
	{
		close(newsd);
	}
	else
	{
		// create new cofrpc instance for accepted TCP connection
		new cofrpc(rpc_endpnt,
						entity,
						newsd,
						ra,
						domain,
						type,
						protocol,
						false /* passive socket */);
	}
}


void
cofrpc::handle_read(int fd)
{
	int rc;

	cofpacket *pcppack = NULL;
	try {

		pcppack = (0 != fragment) ? fragment : new cofpacket();
#if 0
		if (fragment)
		{
			pcppack = fragment;
		}
		else
		{
			pcppack = new cofpacket();
		}
#endif



		while (true)
		{
			// SSL support: client or server side, done in derived class

			// TODO: this will be replaced with SSL socket later
			rc = read(fd, (void*) pcppack->memptr(), pcppack->need_bytes());

#ifndef NDEBUG
			struct ofp_header *hdr = (struct ofp_header*) (pcppack->soframe());
			WRITELOG(COFRPC, ROFL_DBG,
					"cofrpc(%p:%s)::handle_read() sd:%d hdr->type:%d rc:%d pack: %s",
					this, (rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) ? "north" : "south",
							fd, hdr->type, rc, pcppack->c_str());
#endif


			if (rc < 0) // error occured (or non-blocking)
			{
				switch(errno) {
				case EAGAIN:
					{
						// more bytes are needed, store pcppack in fragment pointer
						fragment = pcppack;
					}
					return;
				case ECONNREFUSED:
					{
						try_to_connect(); // reconnect
					}
					return;
				case ECONNRESET:
				default:
					{
						WRITELOG(COFRPC, ROFL_WARN, "cofrpc(%p)::handle_revent() "
								"an error occured, closing => errno: %d (%s)",
								this, errno, strerror(errno));

						//cclose();

						handle_closed(sd);
					}
					return;
				}
			}
			else if (rc == 0) // socket was closed
			{
				//rfds.erase(fd);

				WRITELOG(COFRPC, ROFL_WARN, "cofrpc(%p)::handle_revent() "
						"peer closed connection, closing local endpoint => rc: %d",
						this, rc);

				deregister_filedesc_r(fd);

				handle_closed(fd);

				return;
			}
			else // rc > 0, // some bytes were received, check for completeness of packet
			{
				pcppack->stored_bytes(rc);

				//WRITELOG(COFRPC, ROFL_DBG, "cofrpc::handle_revent(fd=%d) rc=%d need_bytes=%d complete=%s",
				//		 fd, rc, (int)pcppack->need_bytes(), pcppack->complete() ? "true" : "false");

				// complete() parses the packet internally (otherwise we do not know
				// that the packet is complete ...)
				if (pcppack->complete())
				{
					// fragment is complete, set back to NULL
					fragment = NULL;
					handle_tcp(pcppack);

					break;
				}

			}
		}

	} catch (eRpcNotConnected& e) {

		WRITELOG(COFRPC, ROFL_WARN, "cofrpc(%p)::handle_read() received packet in non-connected state: %s", this, pcppack->c_str());

		if (pcppack)
		{
			delete pcppack; pcppack = NULL;
		}

	} catch (cerror &e) {

		WRITELOG(COFRPC, ROFL_WARN, "cofprc(%p)::handle_read() "
				"errno: %d (%s) "
				"generic error %s", this, errno, strerror(errno), pcppack->c_str());

		if (pcppack)
		{
			delete pcppack; pcppack = NULL;
		}

		//cclose();

		handle_closed(sd);

		throw;
	}

}


/** handle incoming packets from TCP socket
 */
void cofrpc::handle_tcp(
		cofpacket *pack)
{
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc::handle_tcp(%p) %s", pack, pack->c_str());

	try {

		if ((cur_state() == STATE_RPC_DISCONNECTED) ||
			(cur_state() == STATE_RPC_CONNECTING) ||
			((cur_state() == STATE_RPC_CONNECTED) && (not ((pack->ofh_header->type == OFPT_HELLO) || (pack->ofh_header->type == OFPT_ERROR))))) {
			WRITELOG(COFRPC, ROFL_DBG, "cofrpc::handle_tcp(%p) EXCEPTION! %s", pack,
					pack->c_str());
			delete pack;
			cclose();
			return;
			//throw eRpcNotConnected();
		}

		if (not pack->is_valid())
			throw eRpcPackInval();

#ifndef NDEBUG
		fprintf(stderr, "r:%d ", pack->ofh_header->type);
#endif

		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p:%s): received packet "
				"from TCP socket", this,
				(rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) ? "north" : "south");

//		WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%s): dpid:%lu received packet "
//				"from TCP socket: %s", (rpc_endpnt == OF_RPC_NORTH_ENDPNT) ?
//						"north" : "south", this->dpid, pack->c_str());

		switch (pack->ofh_header->type) {
		/* generic messages */
		case OFPT_HELLO:
			WRITELOG(COFRPC, ROFL_DBG, "OFPT_HELLO");
			handle_hello_message_via_tcp(pack);
			return;
		case OFPT_ECHO_REQUEST:
			WRITELOG(COFRPC, ROFL_DBG, "OFPT_ECHO_REQUEST");
			send_echo_reply(pack);
			return;
		case OFPT_ECHO_REPLY:
			WRITELOG(COFRPC, ROFL_DBG, "OFPT_ECHO_REPLY");
			handle_echo_reply(pack);
			return;
		case OFPT_ERROR:
			WRITELOG(COFRPC, ROFL_DBG, "OFPT_ERROR");
			if (be16toh(pack->ofh_error_msg->type) == OFPET_HELLO_FAILED) {
				handle_error(pack);
				return;
			}
			break;
		}

		if (!entity)
			throw eRpcNoEntity();

		if (rpc_endpnt == OF_RPC_TCP_SOUTH_ENDPNT) {
			switch (pack->ofh_header->type) {
			/* downstream messages */
			case OFPT_FEATURES_REQUEST:
				entity->fe_down_features_request(this, pack);
				break;
			case OFPT_GET_CONFIG_REQUEST:
				entity->fe_down_get_config_request(this, pack);
				break;
			case OFPT_PACKET_OUT:
				entity->fe_down_packet_out(this, pack);
				break;
			case OFPT_FLOW_MOD:
				entity->fe_down_flow_mod(this, pack);
				break;
			case OFPT_PORT_MOD:
				entity->fe_down_port_mod(this, pack);
				break;
			case OFPT_STATS_REQUEST:
				entity->fe_down_stats_request(this, pack);
				break;
			case OFPT_BARRIER_REQUEST:
				entity->fe_down_barrier_request(this, pack);
				break;
			case OFPT_QUEUE_GET_CONFIG_REQUEST:
				entity->fe_down_queue_get_config_request(this, pack);
				break;
			case OFPT_SET_CONFIG:
				entity->fe_down_set_config_request(this, pack);
				break;
			case OFPT_GROUP_MOD:
				entity->fe_down_group_mod(this, pack);
				break;
			case OFPT_TABLE_MOD:
				entity->fe_down_table_mod(this, pack);
				break;
			case OFPT_EXPERIMENTER:
				entity->fe_down_experimenter_message(this, pack);
				break;
			case OFPT_ROLE_REQUEST:
				entity->fe_down_role_request(this, pack);
				break;
				/* unknown messages */
			default:
				WRITELOG(COFRPC, ROFL_WARN, "packet with invalid type "
						"from TCP socket received (%d)",
						pack->ofh_header->type);
				delete pack;
				break;
			}
		} else if (rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) {
			switch (pack->ofh_header->type) {
			/* upstream messages */
			case OFPT_FEATURES_REPLY:
#if 0
				// NOOOOOOOOOOOOOOOO!!!!!!!!!!!!!!
				// replace this->dpid
				this->dpid = be64toh(pack->ofh_switch_features->datapath_id);
#endif
				entity->fe_up_features_reply(this, pack);
				break;
			case OFPT_GET_CONFIG_REPLY:
				entity->fe_up_get_config_reply(this, pack);
				break;
			case OFPT_PACKET_IN:
				entity->fe_up_packet_in(this, pack);
				break;
			case OFPT_FLOW_REMOVED:
				entity->fe_up_flow_removed(this, pack);
				break;
			case OFPT_PORT_STATUS:
				entity->fe_up_port_status(this, pack);
				break;
			case OFPT_STATS_REPLY:
				entity->fe_up_stats_reply(this, pack);
				break;
			case OFPT_BARRIER_REPLY:
				entity->fe_up_barrier_reply(this, pack);
				break;
			case OFPT_QUEUE_GET_CONFIG_REPLY:
				entity->fe_up_queue_get_config_reply(this, pack);
				break;
			case OFPT_ERROR:
				entity->fe_up_error(this, pack);
				break;
			case OFPT_EXPERIMENTER:
				entity->fe_up_experimenter_message(this, pack);
				return;
			case OFPT_ROLE_REPLY:
				entity->fe_up_role_reply(this, pack);
				return;
				/* unknown messages */
			default:
				WRITELOG(COFRPC, ROFL_WARN, "packet with invalid type "
						"from TCP socket received (%d)",
						pack->ofh_header->type);
				delete pack;
				break;
			}
		} else {
			delete pack;
		}

	} catch (eRpcPackInval& e) {
		WRITELOG(COFRPC, ROFL_WARN, "cofrpc(%p)::handle_tcp() "
				"invalid packet received from %s",
				this, raddr.c_str());

		delete pack;

	} catch (eRpcNoEntity& e) {
		WRITELOG(COFRPC, ROFL_WARN, "packet %s received from %s, "
				"no entity", pack->c_str(), raddr.c_str());

		delete pack;
	} catch (eInstructionBase& e) {
		WRITELOG(
				COFRPC,
				ROFL_WARN,
				"cofrpc(%p)::handle_tcp() invalid OF packet rcvd (eInstruction): %s",
				this, pack->c_str());

		delete pack;
	}
}

/*
 * generic up/down logic
 */

void
cofrpc::fe_queue_message(cofpacket *pack, bool priority)
{
	Lock lock(&fe_queue_mutex);

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc::%s()", __FUNCTION__);

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p:%s): queueing for transport via TCP: %s",
			this, (rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) ? "north" : "south",
			pack->c_str());
//	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%s): dpid:%lu queueing for transport via TCP: %s",
//			(rpc_endpnt == OF_RPC_NORTH_ENDPNT) ? "north" : "south",
//					this->dpid, pack->c_str());

	if ((cur_state() == STATE_RPC_DISCONNECTED)
			|| (cur_state() == STATE_RPC_CONNECTING)
			|| ((cur_state() == STATE_RPC_CONNECTED)
					&& (not ((pack->ofh_header->type == OFPT_HELLO)
							|| (pack->ofh_header->type == OFPT_ERROR))))
			|| not sockflags.test(CONNECTED))
			//if (not sockflags.test(CONNECTED))
	{
		delete pack;

		// FIXME lock
		while (not fe_queue.empty())
		{
			delete fe_queue.front();
			fe_queue.pop_front();
		}
		WRITELOG(COFRPC, ROFL_WARN,
				"cofrpc(%p)::fe_queue_message() not connected, dropping packet",
				this);
		return;
	}

	// FIXME lock
	if (priority)
	{
		fe_queue.push_front(pack);
	}
	else
	{
		fe_queue.push_back(pack);
	}

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p:%s): [%d] packets enqueued for transport via TCP",
			this, (rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) ? "north" : "south", fe_queue.size());

	if (tid == pthread_self())
	{
		reset_timer(TIMER_RPC_SEND_VIA_TCP, 0);
	}
	else
	{
		notify(cevent(COFRPC_EVENT_SEND_VIA_TCP));
	}
}


/** send message via TCP
 */
void
cofrpc::send_message_via_tcp()
{
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_message_via_tcp()", this);

	try {
		Lock lock(&fe_queue_mutex, false /*non-blocking*/);
		if (fe_queue.empty())
		{
			return;
		}

		while (not fe_queue.empty())
		{
			cofpacket *pack = fe_queue.front();

			if ((cur_state() == STATE_RPC_DISCONNECTED)
					|| (cur_state() == STATE_RPC_CONNECTING)
					|| ((cur_state() == STATE_RPC_CONNECTED)
							&& ((pack->ofh_header->type != OFPT_HELLO)
									&& (pack->ofh_header->type != OFPT_ERROR))))
			{
				WRITELOG(
						COFRPC,
						ROFL_DBG,
						"cofrpc::send_message_via_tcp() dropping packet, not connected");
				delete pack;

				// fixme lock
				fe_queue.pop_front();
				return;
				//throw eRpcNotConnected();
			}

			// fixme lock
			fe_queue.pop_front();

			// call pack->stored_bytes(...) with the correct ofp_header->length field
			//pack->stored_bytes(be16toh(((struct ofp_header*)pack->soframe())->length));

			WRITELOG(COFRPC, ROFL_DBG,
					"cofrpc(%p:%s): fe_queue.size()[%d] sending packet via TCP socket (sd=%d): %s", this,
					(rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) ? "north" : "south", fe_queue.size(), sd,
					pack->c_str());
		//	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%s): dpid:%lu sending packet via TCP socket: %s",
		//					(rpc_endpnt == OF_RPC_NORTH_ENDPNT) ? "north" : "south",
		//							this->dpid, pack->c_str());

			cmemory *mem = new cmemory(pack->length());

			pack->pack(mem->somem(), mem->memlen());

#ifndef NDEBUG
			fprintf(stderr, "s:%d ", pack->ofh_header->type);
#endif

			WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p:%s): new cmemory: %s",
					this,
					(rpc_endpnt == OF_RPC_TCP_NORTH_ENDPNT) ? "north" : "south",
					mem->c_str());

			// call csocket send method
			send_packet(mem);

			delete pack;
		}

		if (not fe_queue.empty())
		{
			reset_timer(TIMER_RPC_SEND_VIA_TCP, 0);
		}

	} catch (eLockWouldBlock& e) {
		register_timer(TIMER_RPC_SEND_VIA_TCP, 1);

	}
}

/*
 * HELLO message
 */
void
cofrpc::send_hello_message_via_tcp()
{
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_hello_message_via_tcp()", this);
	size_t packlen = sizeof(struct ofp_header);

	cofpacket *pack = new cofpacket(packlen, packlen);

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_hello_message_via_tcp() new %s", this,
			pack->c_str());

	pack->ofh_header->version = OFP_VERSION;
	pack->ofh_header->type = OFPT_HELLO;
	pack->ofh_header->length = htobe16(packlen);
	pack->ofh_header->xid = htobe32(ta_new_async_xid());

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_hello_message_via_tcp() done %s", this,
			pack->c_str());
	fe_queue_message(pack);
}


void
cofrpc::handle_hello_message_via_tcp(cofpacket *pack)
{
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::handle_hello_message_via_tcp() pack:%s", this, pack->c_str());

	// OpenFlow versions do not match, send error, close connection
	if (pack->ofh_header->version != OFP_VERSION)
	{
		// invalid OFP_VERSION
		char explanation[256];
		bzero(explanation, sizeof(explanation));
		snprintf(explanation, sizeof(explanation) - 1,
				"unsupported OF version (%d), supported version is (%d)",
				(pack->ofh_header->version), OFP_VERSION);

		send_error_message(OFPET_HELLO_FAILED, OFPHFC_INCOMPATIBLE,
				(uint8_t*) explanation, strlen(explanation));

		WRITELOG(COFRPC, ROFL_WARN, "cofrpc::handle_revent() closing (2.1)");
		register_timer(TIMER_RPC_CLOSE_CONNECTION, 1 /* seconds */);

		delete pack;

		return;
	}

	delete pack;

#if 1
	new_state(STATE_RPC_ESTABLISHED);

	// we delay reading from the TCP socket a little bit in order to allow the cioctl() to notify all cofbase instances
	deregister_filedesc_r(sd);

	// re-register the socket file descriptor after one second
	register_timer(TIMER_RPC_ESTABLISH, 0);

#else
	register_timer(TIMER_RPC_BUGGY_NOX_DELAY, 1); // postpone switching to state -ESTABLISHED- for buggy nox implementation
#endif

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p): HELLO exchanged with peer entity, attaching ...", this);

	switch (rpc_endpnt) {
	case OF_RPC_TCP_NORTH_ENDPNT:
		send_up_hello_message(entity);
		break;
	case OF_RPC_TCP_SOUTH_ENDPNT:
		send_down_hello_message(entity);
		break;
	default:

		break;
	}

	// start sending ECHO requests
	register_timer(TIMER_RPC_SEND_ECHO_REQUEST, rpc_echo_interval);
}


void
cofrpc::send_error_message(
		uint16_t type,
		uint16_t code,
		uint8_t* data,
		size_t datalen)
{
	cofpacket *pack = NULL;

	size_t packlen = sizeof(struct ofp_error_msg) + datalen;

	pack = new cofpacket(packlen, packlen);

	pack->ofh_header->version = OFP_VERSION;
	pack->ofh_header->type = OFPT_ERROR;
	pack->ofh_header->length = htobe16(packlen);
	pack->ofh_header->xid = htobe32(ta_new_async_xid());

	pack->ofh_error_msg->type = htobe16(type);
	pack->ofh_error_msg->code = htobe16(code);

	if (data) {
		memcpy(pack->ofh_error_msg->data, data, datalen);
	}

	fe_queue_message(pack);
}


void
cofrpc::handle_error(cofpacket *pack)
{
	switch(be16toh(pack->ofh_error_msg->code)) {
		case OFPHFC_INCOMPATIBLE:

		break;
		case OFPHFC_EPERM:

		break;
		default:
		WRITELOG(COFRPC, ROFL_WARN, "unknown error code (%d)",
				be16toh(pack->ofh_error_msg->code));
		break;
	}

	delete pack;

	WRITELOG(COFRPC, ROFL_WARN, "cofrpc(%p)::handle_revent() closing (2.2)", this);
	register_timer(TIMER_RPC_CLOSE_CONNECTION, 0);
}

/*
 * ECHO request/reply
 */

void
cofrpc::send_echo_request()
{
	time_t now = time(NULL);

	cofpacket_echo_request *pack =
			new cofpacket_echo_request(
							ta_add_request(OFPT_ECHO_REQUEST),
							(uint8_t*)&now,
							sizeof(now));

	fe_queue_message(pack);

	register_timer(TIMER_RPC_ECHO_REPLY_TIMEOUT, 15 /* seconds */);
}


void
cofrpc::send_echo_reply(cofpacket *request)
{
	cofpacket_echo_reply *pack = NULL;

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::send_echo_reply() %s", this, request->c_str());

	try {
		if (be16toh(request->ofh_header->length) < sizeof(struct ofp_header))
			throw eOFifaceInval();

		if (request->has_data())
		{
			pack = new cofpacket_echo_reply(request->get_xid(),
													request->get_data(),
													request->get_datalen());
		}
		else
		{
			pack = new cofpacket_echo_reply(request->get_xid());
		}

		pack->pack();

		fe_queue_message(pack, /*priority=*/true);

	} catch (eOFifaceInval& e) {
		send_error_message(OFPET_BAD_REQUEST, OFPBRC_BAD_LEN,
				(unsigned char*) pack->soframe(), pack->framelen());

		delete pack;
	}

	delete request;
}


void
cofrpc::handle_echo_reply(cofpacket *pack)
{
	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::handle_echo_reply()", this);
	try {
		if (be16toh(pack->ofh_header->length) < sizeof(struct ofp_header))
			throw eOFifaceInval();

		ta_validate(be32toh(pack->ofh_header->xid), OFPT_ECHO_REQUEST);

		cancel_timer(TIMER_RPC_ECHO_REPLY_TIMEOUT);

	} catch (eOFifaceInval& e) {
		WRITELOG(COFRPC, ROFL_WARN, "invalid echo reply");

		WRITELOG(COFRPC, ROFL_WARN, "cofrpc::handle_revent() closing (2.3)");
		register_timer(TIMER_RPC_CLOSE_CONNECTION, 1 /* seconds */);

	} catch (eOFifaceXidInval& e) {
		WRITELOG(COFRPC, ROFL_WARN, "invalid session exchange xid "
				"(0x%x) received", be32toh(pack->ofh_header->xid));

		WRITELOG(COFRPC, ROFL_WARN, "cofrpc::handle_revent() closing (2.4)");
		register_timer(TIMER_RPC_CLOSE_CONNECTION, 1 /* seconds */);
	}

	delete pack;
}


void
cofrpc::handle_echo_reply_timeout()
{
	WRITELOG(COFRPC, ROFL_WARN,
			"cofrpc(%p)::handle_echo_reply_timeout() closing (2.5)", this);
	register_timer(TIMER_RPC_CLOSE_CONNECTION, 0);
}


void
cofrpc::try_to_connect(bool reset_timeout)
{
	if (pending_timer(TIMER_RPC_RECONNECT))
	{
		return;
	}

	WRITELOG(COFRPC, ROFL_DBG, "cofrpc(%p)::try_to_connect() "
			"reconnect in %d seconds (reconnect_counter:%d)",
			this, reconnect_in_seconds, reconnect_counter);

	if ((reset_timeout) || (4 == reconnect_counter))
	{
		reconnect_in_seconds = RECONNECT_START_TIMEOUT;

		reconnect_counter = 0;
	}

	if (reconnect_counter > 3)
	{
		reconnect_in_seconds = RECONNECT_MAX_TIMEOUT;
	}

	reset_timer(TIMER_RPC_RECONNECT, reconnect_in_seconds);

	++reconnect_counter;
}


const char*
cofrpc::c_str()
{
	cvastring vas;
	info.assign(vas("cofrpc(%p) %s -> %s", this, laddr.c_str(),
			raddr.c_str()));

	return info.c_str();
}
