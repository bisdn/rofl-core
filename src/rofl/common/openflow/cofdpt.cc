/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofdpt.h"

using namespace rofl;

cofdpt::cofdpt(
		crofbase *rofbase,
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
	dpid(0),
	dpmac(cmacaddr("00:00:00:00:00:00")),
	n_buffers(0),
	n_tables(0),
	capabilities(0),
	offlags(0),
	miss_send_len(0),
	socket(new csocket(this, newsd, ra, domain, type, protocol)),
	rofbase(rofbase),
	fragment(0),
	reconnect_in_seconds(RECONNECT_START_TIMEOUT),
	reconnect_counter(0),
	rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
	features_reply_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
	get_config_reply_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
	stats_reply_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
	barrier_reply_timeout(DEFAULT_DB_BARRIER_REPLY_TIMEOUT)
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



cofdpt::cofdpt(
		crofbase *rofbase,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
	dpid(0),
	dpmac(cmacaddr("00:00:00:00:00:00")),
	n_buffers(0),
	n_tables(0),
	capabilities(0),
	offlags(0),
	miss_send_len(0),
	socket(new csocket(this, domain, type, protocol)),
	rofbase(rofbase),
	fragment(0),
	reconnect_in_seconds(RECONNECT_START_TIMEOUT),
	reconnect_counter(0),
	rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL),
	features_reply_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
	get_config_reply_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
	stats_reply_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
	barrier_reply_timeout(DEFAULT_DB_BARRIER_REPLY_TIMEOUT)
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::cofdpt() "
			"dpid:%"PRIu64" ",
			this, dpid);

	init_state(COFDPT_STATE_DISCONNECTED);

	dptflags.set(COFDPT_FLAG_ACTIVE_SOCKET);

	socket->caopen(ra, caddress(AF_INET, "0.0.0.0"), domain, type, protocol);
}



cofdpt::~cofdpt()
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



void
cofdpt::handle_accepted(
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
cofdpt::handle_connected(
		csocket *socket,
		int sd)
{
#ifndef NDEBUG
	fprintf(stderr, "C:dpt[%s] ", socket->raddr.c_str());
#endif
	register_timer(COFDPT_TIMER_SEND_HELLO, 0);
}



void
cofdpt::handle_connect_refused(
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
cofdpt::handle_read(
		csocket *socket,
		int sd)
{
	int rc;

	cofpacket *pcppack = (cofpacket*)0;
	try {

		pcppack = (0 != fragment) ? fragment : new cofpacket();

		while (true)
		{
			// SSL support: client or server side, done in derived class

			// TODO: this will be replaced with SSL socket later
			rc = read(sd, (void*) pcppack->memptr(), pcppack->need_bytes());


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
						WRITELOG(COFDPT, WARN, "cofdpt(%p)::handle_read() "
								"an error occured, closing => errno: %d (%s)",
								this, errno, strerror(errno));


						handle_closed(socket, sd);
					}
					return;
				}
			}
			else if (rc == 0) // socket was closed
			{
				//rfds.erase(fd);

				WRITELOG(COFDPT, WARN, "cofdpt(%p)::handle_read() "
						"peer closed connection, closing local endpoint => rc: %d",
						this, rc);

				handle_closed(socket, sd);

				return;
			}
			else // rc > 0, // some bytes were received, check for completeness of packet
			{
				pcppack->stored_bytes(rc);

				//WRITELOG(COFRPC, DBG, "cofrpc::handle_revent(fd=%d) rc=%d need_bytes=%d complete=%s",
				//		 fd, rc, (int)pcppack->need_bytes(), pcppack->complete() ? "true" : "false");

				// complete() parses the packet internally (otherwise we do not know
				// that the packet is complete ...)
				if (pcppack->complete())
				{
					// fragment is complete, set back to NULL
					fragment = (cofpacket*)0;
					handle_message(pcppack);

					break;
				}

			}
		}

	} catch (eOFpacketInval& e) {

		WRITELOG(COFDPT, ERROR, "cofctl(%p)::handle_read() "
				"invalid packet received, dropping. Closing socket. Packet: %s",
				this, pcppack->c_str());

		if (pcppack)
		{
			delete pcppack; pcppack = (cofpacket*)0;
		}

		handle_closed(socket, sd);
	}
}



void
cofdpt::handle_closed(
		csocket *socket,
		int sd)
{
	socket->cclose();

	cancel_timer(COFDPT_TIMER_ECHO_REPLY);
	cancel_timer(COFDPT_TIMER_SEND_ECHO_REQUEST);

	if (dptflags.test(COFDPT_FLAG_ACTIVE_SOCKET))
	{
		try_to_connect();
	}
	else
	{
		rofbase->handle_dpt_close(this);
	}
}



void
cofdpt::handle_message(
		cofpacket *pack)
{
	try {
		if (not pack->is_valid())
		{
			writelog(COFDPT, ERROR, "cofdpt(%p)::handle_message() "
					"dropping invalid packet: %s", this, pack->c_str());
			delete pack; return;
		}

		if (not flags.test(COFDPT_FLAG_HELLO_RCVD) && (pack->ofh_header->type != OFPT_HELLO))
		{
			writelog(COFCTL, ERROR, "cofdpt(%p)::handle_message() "
				"no HELLO rcvd yet, dropping message, pack: %s", this, pack->c_str());
			delete pack; return;
		}

		switch (pack->ofh_header->type) {
		case OFPT_HELLO:
			{
				hello_rcvd(pack);
			}
			break;
		case OFPT_ECHO_REQUEST:
			{
				echo_request_rcvd(pack);
			}
			break;
		case OFPT_ECHO_REPLY:
			{
				rofbase->ta_validate(pack->get_xid(), OFPT_ECHO_REQUEST);

				echo_reply_rcvd(pack);
			}
			break;
		case OFPT_EXPERIMENTER:
			{
				experimenter_rcvd(pack);
			}
			break;
		case OFPT_FEATURES_REPLY:
			{
				rofbase->ta_validate(pack->get_xid(), OFPT_FEATURES_REQUEST);

				features_reply_rcvd(pack);
			}
			break;
		case OFPT_GET_CONFIG_REPLY:
			{
				rofbase->ta_validate(pack->get_xid(), OFPT_GET_CONFIG_REQUEST);

				get_config_reply_rcvd(pack);
			}
			break;
		case OFPT_PACKET_IN:
			{
				packet_in_rcvd(pack);
			}
			break;
		case OFPT_FLOW_REMOVED:
			{
				flow_rmvd_rcvd(pack);
			}
			break;
		case OFPT_PORT_STATUS:
			{
				port_status_rcvd(pack);
			}
			break;
		case OFPT_STATS_REPLY:
			{
				rofbase->ta_validate(pack->get_xid(), OFPT_STATS_REQUEST);

				stats_reply_rcvd(pack);
			}
			break;
		case OFPT_BARRIER_REPLY:
			{
				rofbase->ta_validate(pack->get_xid(), OFPT_BARRIER_REQUEST);

				barrier_reply_rcvd(pack);
			}
			break;
		case OFPT_QUEUE_GET_CONFIG_REPLY:
			{
				rofbase->ta_validate(pack->get_xid(), OFPT_QUEUE_GET_CONFIG_REQUEST);

				queue_get_config_reply_rcvd(pack);
			}
			break;
		case OFPT_ROLE_REPLY:
			{
				rofbase->ta_validate(pack->get_xid(), OFPT_ROLE_REQUEST);

				role_reply_rcvd(pack);
			}
			break;
		default:
			{
				WRITELOG(COFDPT, WARN, "cofdpt(%p)::handle_message() "
						"dropping packet: %s", this, pack->c_str());
				delete pack;
			}
			return;
		}


	} catch (eRofBaseXidInval& e) {

		writelog(COFDPT, ERROR, "cofdpt(%p)::handle_message() "
				"packet with invalid transaction id rcvd, pack: %s", this, pack->c_str());

		delete pack;
	}
}



void
cofdpt::send_message(
		cofpacket *pack)
{
    if (not flags.test(COFDPT_FLAG_HELLO_RCVD) && (pack->ofh_header->type != OFPT_HELLO))
    {
        WRITELOG(CFWD, DBG, "cofdpt(%p)::send_message() "
            "dropping message, as no HELLO rcvd from peer yet => pack: %s",
            this, pack->c_str());
        delete pack; return;
    }

	switch (pack->ofh_header->type) {
	case OFPT_HELLO:
		{
			// do nothing here
		}
		break;
	case OFPT_ECHO_REQUEST:
		{
			echo_request_sent(pack);
		}
		break;
	case OFPT_ECHO_REPLY:
		{
			// do nothing here
		}
		break;
	case OFPT_ERROR:
	case OFPT_EXPERIMENTER:
	case OFPT_SET_CONFIG:
	case OFPT_PACKET_OUT:
	case OFPT_FLOW_MOD:
	case OFPT_GROUP_MOD:
	case OFPT_PORT_MOD:
	case OFPT_TABLE_MOD:
		{
			// asynchronous messages, no transaction => do nothing here
		}
		break;
	case OFPT_FEATURES_REQUEST:
		{
			features_request_sent(pack);
		}
		break;
	case OFPT_GET_CONFIG_REQUEST:
		{
			get_config_request_sent(pack);
		}
		break;
	case OFPT_STATS_REQUEST:
		{
			stats_request_sent(pack);
		}
		break;
	case OFPT_BARRIER_REQUEST:
		{
			barrier_request_sent(pack);
		}
		break;
	case OFPT_QUEUE_GET_CONFIG_REQUEST:
		{
			queue_get_config_request_sent(pack);
		}
		break;
	case OFPT_ROLE_REQUEST:
		{
			role_request_sent(pack);
		}
		break;
	default:
		{
			WRITELOG(COFDPT, WARN, "cofdpt(%p)::send_message() "
					"dropping invalid packet: %s", this, pack->c_str());
			delete pack;
		}
		return;
	}

#ifndef NDEBUG
	fprintf(stderr, "s:%d ", pack->ofh_header->type);
#endif

	send_message_via_socket(pack);
}



void
cofdpt::handle_timeout(int opaque)
{
	switch (opaque) {
	case COFDPT_TIMER_SEND_HELLO:
		{
			rofbase->send_hello_message(this);

			flags.set(COFDPT_FLAG_HELLO_SENT);

			if (flags.test(COFDPT_FLAG_HELLO_RCVD))
			{
				rofbase->send_features_request(this);

				rofbase->send_echo_request(this);
			}
		}
		break;
	case COFDPT_TIMER_FEATURES_REQUEST:
		{
		    rofbase->send_features_request(this);
		}
		break;
	case COFDPT_TIMER_FEATURES_REPLY:
		{
			handle_features_reply_timeout();
		}
		break;
	case COFDPT_TIMER_GET_CONFIG_REPLY:
		{
			handle_get_config_reply_timeout();
		}
		break;
	case COFDPT_TIMER_STATS_REPLY:
		{
			handle_stats_reply_timeout();
		}
		break;
	case COFDPT_TIMER_BARRIER_REPLY:
		{
			handle_barrier_reply_timeout();
		}
		break;
	case COFDPT_TIMER_RECONNECT:
		{
			if (socket)
			{
				socket->caopen(socket->raddr, caddress(AF_INET, "0.0.0.0"), socket->domain, socket->type, socket->protocol);
			}
		}
		break;
	case COFDPT_TIMER_SEND_ECHO_REQUEST:
		{
			rofbase->send_echo_request(this);
		}
		break;
	case COFDPT_TIMER_ECHO_REPLY:
		{
			handle_echo_reply_timeout();
		}
		break;
	default:
		{
			WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_timeout() "
					"unknown timer event %d", this, opaque);
		}
		break;
	}
}



void
cofdpt::hello_rcvd(cofpacket *pack)
{
	try {
		WRITELOG(COFRPC, DBG, "cofdpt(%p)::hello_rcvd() pack: %s", this, pack->c_str());

		// OpenFlow versions do not match, send error, close connection
		if (pack->ofh_header->version != OFP_VERSION)
		{
			new_state(COFDPT_STATE_DISCONNECTED);

			// invalid OFP_VERSION
			char explanation[256];
			bzero(explanation, sizeof(explanation));
			snprintf(explanation, sizeof(explanation) - 1,
					"unsupported OF version (%d), supported version is (%d)",
					(pack->ofh_header->version), OFP_VERSION);

			cofpacket_error *reply = new cofpacket_error(
								pack->get_xid(),
								OFPET_HELLO_FAILED,
								OFPHFC_INCOMPATIBLE,
								(uint8_t*) explanation, strlen(explanation));

			send_message_via_socket(reply); // circumvent ::send_message, as COFDPT_FLAG_HELLO_RCVD is not set

			handle_closed(socket, socket->sd);
		}
		else
		{
			WRITELOG(COFRPC, DBG, "cofdpt(%p)::hello_rcvd() "
					"HELLO exchanged with peer entity, attaching ...", this);

			flags.set(COFDPT_FLAG_HELLO_RCVD);

			new_state(COFDPT_STATE_WAIT_FEATURES);

			if (flags.test(COFDPT_FLAG_HELLO_SENT))
			{
				rofbase->send_features_request(this);

				rofbase->send_echo_request(this);
			}
		}

		delete pack;

	} catch (eHelloIncompatible& e) {

		writelog(CROFBASE, ERROR, "cofctl(%p)::hello_rcvd() "
				"No compatible version, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_HELLO_FAILED,
					OFPHFC_INCOMPATIBLE,
					pack->soframe(), pack->framelen());

		delete pack;
		handle_closed(socket, socket->sd);
	} catch (eHelloEperm& e) {

		writelog(CROFBASE, ERROR, "cofctl(%p)::hello_rcvd() "
				"Permissions error, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_HELLO_FAILED,
					OFPHFC_EPERM,
					pack->soframe(), pack->framelen());

		delete pack;
		handle_closed(socket, socket->sd);
	}
}



void
cofdpt::echo_request_sent(cofpacket *pack)
{
	reset_timer(COFDPT_TIMER_ECHO_REPLY, 5); // TODO: multiple concurrent echo-requests?
}



void
cofdpt::echo_request_rcvd(cofpacket *pack)
{
	// send echo reply back including any appended data
	rofbase->send_echo_reply(this, pack->get_xid(), pack->body.somem(), pack->body.memlen());

	/* Please note: we do not call a handler for echo-requests/replies in rofbase
	 * and take care of these liveness packets within cofctl and cofdpt
	 */
}



void
cofdpt::echo_reply_rcvd(cofpacket *pack)
{
	cancel_timer(COFDPT_TIMER_ECHO_REPLY);
	register_timer(COFDPT_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);
}



void
cofdpt::handle_echo_reply_timeout()
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
cofdpt::features_request_sent(
		cofpacket *pack)
{
	register_timer(COFDPT_TIMER_FEATURES_REPLY, features_reply_timeout /* seconds */);
}



void
cofdpt::features_reply_rcvd(
		cofpacket *pack)
{
	try {
		cancel_timer(COFDPT_TIMER_FEATURES_REPLY);

		dpid 			= be64toh(pack->ofh_switch_features->datapath_id);
		n_buffers 		= be32toh(pack->ofh_switch_features->n_buffers);
		n_tables 		= pack->ofh_switch_features->n_tables;
		capabilities 	= be32toh(pack->ofh_switch_features->capabilities);

		int portslen = be16toh(pack->ofh_switch_features->header.length) -
												sizeof(struct ofp_switch_features);


		WRITELOG(COFDPT, DBG, "cofdpt(%p)::features_reply_rcvd() "
				"dpid:%"PRIu64" pack:%s",
				this, dpid, pack->c_str());


		cofport::ports_parse(ports, pack->ofh_switch_features->ports, portslen);

		WRITELOG(COFDPT, DBG, "cofdpt(%p)::features_reply_rcvd() %s", this, this->c_str());


		// dpid as std::string
		cvastring vas;
		s_dpid = std::string(vas("0x%llx", dpid));

		// lower 48bits from dpid as datapath mac address
		dpmac[0] = (dpid & 0x0000ff0000000000ULL) >> 40;
		dpmac[1] = (dpid & 0x000000ff00000000ULL) >> 32;
		dpmac[2] = (dpid & 0x00000000ff000000ULL) >> 24;
		dpmac[3] = (dpid & 0x0000000000ff0000ULL) >> 16;
		dpmac[4] = (dpid & 0x000000000000ff00ULL) >>  8;
		dpmac[5] = (dpid & 0x00000000000000ffULL) >>  0;
		dpmac[0] &= 0xfc;




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
cofdpt::handle_features_reply_timeout()
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_features_reply_timeout() ", this);

	rofbase->handle_features_reply_timeout(this);
}



void
cofdpt::get_config_request_sent(
		cofpacket *pack)
{
	register_timer(COFDPT_TIMER_GET_CONFIG_REPLY, get_config_reply_timeout);
}



void
cofdpt::get_config_reply_rcvd(
		cofpacket *pack)
{
	cancel_timer(COFDPT_TIMER_GET_CONFIG_REPLY);

	offlags = be16toh(pack->ofh_switch_config->flags);
	miss_send_len = be16toh(pack->ofh_switch_config->miss_send_len);

	WRITELOG(COFDPT, DBG, "cofdpt(%p)::get_config_reply_rcvd() "
			"dpid:%"PRIu64" ",
			this, dpid);

	rofbase->handle_get_config_reply(this, pack);

	if (COFDPT_STATE_WAIT_GET_CONFIG == cur_state())
	{
		// send stats request during initialization
		rofbase->send_stats_request(this, OFPST_TABLE, 0);

		new_state(COFDPT_STATE_WAIT_TABLE_STATS);
	}
}



void
cofdpt::handle_get_config_reply_timeout()
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_get_config_reply_timeout() "
			"dpid:%"PRIu64" ",
			this, dpid);

	rofbase->handle_get_config_reply_timeout(this);
}



void
cofdpt::stats_request_sent(
		cofpacket *pack)
{
	try {
		xidstore[OFPT_STATS_REQUEST].xid_add(this, pack->get_xid(), stats_reply_timeout);

		if (not pending_timer(COFDPT_TIMER_STATS_REPLY))
		{
			register_timer(COFDPT_TIMER_STATS_REPLY, stats_reply_timeout);
		}

	} catch (eXidStoreXidBusy& e) {

		// should never happen, TODO: log error
	}
}



void
cofdpt::stats_reply_rcvd(
		cofpacket *pack)
{
	cancel_timer(COFDPT_TIMER_STATS_REPLY);

	xidstore[OFPT_STATS_REQUEST].xid_rem(be32toh(pack->ofh_header->xid));

	WRITELOG(COFDPT, DBG, "cofdpt(%p)::stats_reply_rcvd() "
			"dpid:%"PRIu64" ",
			this, dpid);

	rofbase->handle_stats_reply(this, pack);


	if (COFDPT_STATE_WAIT_TABLE_STATS == cur_state()) // enter state running during initialization
	{
		new_state(COFDPT_STATE_CONNECTED);

		rofbase->handle_dpt_open(this);
	}
}



void
cofdpt::handle_stats_reply_timeout()
{
	WRITELOG(COFDPT, DBG, "cofdpt(%p)::handle_stats_reply_timeout() "
			"dpid:%"PRIu64" ",
			this, dpid);

restart:
	for (cxidstore::iterator
				it = xidstore[OFPT_STATS_REQUEST].begin();
							it != xidstore[OFPT_STATS_REQUEST].end(); ++it)
	{
		cxidtrans& xidt = it->second;

		if (xidt.timeout <= cclock::now())
		{
			rofbase->handle_stats_reply_timeout(this, xidt.xid);

			xidstore[OFPT_STATS_REQUEST].xid_rem(xidt.xid);

			goto restart;
		}
	}

	if (not xidstore.empty())
	{
		reset_timer(COFDPT_TIMER_STATS_REPLY, stats_reply_timeout);
	}
}



void
cofdpt::barrier_request_sent(
		cofpacket *pack)
{
	try {
		xidstore[OFPT_BARRIER_REQUEST].xid_add(this, pack->get_xid(), barrier_reply_timeout);

		if (not pending_timer(COFDPT_TIMER_BARRIER_REPLY))
		{
			register_timer(COFDPT_TIMER_BARRIER_REPLY, barrier_reply_timeout);
		}

	} catch (eXidStoreXidBusy& e) {

		// should never happen, TODO: log error
	}
}



void
cofdpt::barrier_reply_rcvd(cofpacket *pack)
{
	cancel_timer(COFDPT_TIMER_BARRIER_REPLY);

	xidstore[OFPT_BARRIER_REQUEST].xid_rem(be32toh(pack->ofh_header->xid));

	rofbase->handle_barrier_reply(this, pack);
}



void
cofdpt::handle_barrier_reply_timeout()
{
restart:
	for (cxidstore::iterator
			it = xidstore[OFPT_BARRIER_REQUEST].begin();
						it != xidstore[OFPT_BARRIER_REQUEST].end(); ++it)
	{
		cxidtrans& xidt = it->second;

		if (xidt.timeout <= cclock::now())
		{
			rofbase->handle_barrier_reply_timeout(this, xidt.xid);

			xidstore[OFPT_BARRIER_REQUEST].xid_rem(xidt.xid);

			goto restart;
		}
	}

	if (not xidstore.empty())
	{
		reset_timer(COFDPT_TIMER_BARRIER_REPLY, barrier_reply_timeout);
	}
}



void
cofdpt::flow_mod_sent(
		cofpacket *pack) throw (eOFdpathNotFound)
{
	try {
		WRITELOG(COFDPT, DBG, "cofdpt(%p)::flow_mod_sent() table_id: %d", this, pack->ofh_flow_mod->table_id);


	} catch (cerror& e) {
		WRITELOG(CFTTABLE, DBG, "unable to add ftentry to local flow_table instance");
	}
}



void
cofdpt::flow_rmvd_rcvd(
		cofpacket *pack)
{
	rofbase->handle_flow_removed(this, pack);
}



void
cofdpt::flow_mod_reset()
{
	cflowentry fe;
	fe.set_command(OFPFC_DELETE);
	fe.set_table_id(OFPTT_ALL /*all tables*/);

	rofbase->send_flow_mod_message(this, fe); // calls this->flow_mod_sent() implicitly
}



void
cofdpt::group_mod_sent(
		cofpacket *pack)
{

}



void
cofdpt::group_mod_reset()
{
	cgroupentry ge;
	ge.set_command(OFPGC_DELETE);
	ge.set_group_id(OFPG_ALL);

	rofbase->send_group_mod_message(this, ge); // calls this->group_mod_sent() implicitly
}



void
cofdpt::table_mod_sent(cofpacket *pack)
{
	// TODO: adjust local flowtable
}


void
cofdpt::port_mod_sent(cofpacket *pack)
{
	if (ports.find(be32toh(pack->ofh_port_mod->port_no)) == ports.end())
	{
		return;
	}

	ports[be32toh(pack->ofh_port_mod->port_no)]->recv_port_mod(
										be32toh(pack->ofh_port_mod->config),
										be32toh(pack->ofh_port_mod->mask),
										be32toh(pack->ofh_port_mod->advertise));
}

void
cofdpt::packet_in_rcvd(cofpacket *pack)
{
	try {
		WRITELOG(COFDPT, DBG, "cofdpt(%p)::packet_in_rcvd() %s", this, pack->c_str());

#if 0
		// update forwarding table
		uint32_t in_port = pack->match.get_in_port();
#endif

		// datalen must be at least one Ethernet header in size
		if (pack->packet.length() >= (2 * OFP_ETH_ALEN + sizeof(uint16_t)))
		{
#if 0
			// update local forwarding table
			fwdtable.mac_learning(pack->packet, dpid, in_port);

			WRITELOG(COFDPT, DBG, "cofdpt(0x%llx)::packet_in_rcvd() local fwdtable: %s",
					dpid, fwdtable.c_str());
#endif


#if 0
		rofbase->fwdtable.mac_learning(ether, dpid, in_port);

		WRITELOG(COFDPT, DBG, "cofdpt(0x%llx)::packet_in_rcvd() global fwdtable: %s",
				dpid, rofbase->fwdtable.c_str());
#endif

			// let derived class handle PACKET-IN event
			rofbase->handle_packet_in(this, pack);
		}
	} catch (eOFmatchNotFound& e) {

		WRITELOG(COFDPT, DBG, "cofdpt(0x%llx)::packet_in_rcvd() "
				"no in-port specified in Packet-In message", dpid);
	}
}


void
cofdpt::port_status_rcvd(cofpacket *pack)
{
	WRITELOG(COFDPT, DBG, "cofdpt(0x%016llx)::port_status_rcvd() %s",
			dpid, pack->c_str());

	std::map<uint32_t, cofport*>::iterator it;
	switch (pack->ofh_port_status->reason) {
	case OFPPR_ADD:
		if (ports.find(be32toh(pack->ofh_port_status->desc.port_no)) == ports.end())
		{
			cofport *lport = new cofport(&ports, be32toh(pack->ofh_port_status->desc.port_no), &(pack->ofh_port_status->desc), sizeof(struct ofp_port));

			// let derived class handle PORT-STATUS message
			rofbase->handle_port_status(this, pack, lport);
		}
		break;
	case OFPPR_DELETE:
		if (ports.find(be32toh(pack->ofh_port_status->desc.port_no)) != ports.end())
		{
			uint32_t port_no = be32toh(pack->ofh_port_status->desc.port_no);
			// let derived class handle PORT-STATUS message
			rofbase->handle_port_status(this, pack, ports[port_no]);

			// do not access pack here, as it was already deleted by rofbase->handle_port_status() !!!
			delete ports[port_no];

			ports.erase(port_no);
		}
		break;
	case OFPPR_MODIFY:
		if (ports.find(be32toh(pack->ofh_port_status->desc.port_no)) != ports.end())
		{
			ports[be32toh(pack->ofh_port_status->desc.port_no)]->unpack(
																&(pack->ofh_port_status->desc),
																sizeof(struct ofp_port));

			// let derived class handle PORT-STATUS message
			rofbase->handle_port_status(this, pack, ports[be32toh(pack->ofh_port_status->desc.port_no)]);
		}
		break;
	}
}


void
cofdpt::fsp_open(cofmatch const& ofmatch)
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
cofdpt::fsp_close(cofmatch const& ofmatch)
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
cofdpt::experimenter_rcvd(cofpacket *pack)
{
	switch (be32toh(pack->ofh_experimenter->experimenter)) {
	default:
		break;
	}

	// for now: send vendor extensions directly to class derived from crofbase
	rofbase->handle_experimenter_message(this, pack);
}



void
cofdpt::role_request_sent(
		cofpacket *pack)
{

}



void
cofdpt::role_reply_rcvd(cofpacket *pack)
{
	rofbase->handle_role_reply(this, pack);
}


void
cofdpt::queue_get_config_request_sent(
		cofpacket *pack)
{
	// TODO
}



void
cofdpt::queue_get_config_reply_rcvd(
		cofpacket *pack)
{
	// TODO
}



const char*
cofdpt::c_str()
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
cofdpt::find_cofport(
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
cofdpt::find_cofport(
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
cofdpt::find_cofport(
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
cofdpt::try_to_connect(bool reset_timeout)
{
	if (pending_timer(COFDPT_TIMER_RECONNECT))
	{
		return;
	}

	WRITELOG(COFCTL, DBG, "cofdpt(%p)::try_to_connect() "
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

	reset_timer(COFDPT_TIMER_RECONNECT, reconnect_in_seconds);

	++reconnect_counter;
}



void
cofdpt::send_message_via_socket(
		cofpacket *pack)
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
}


