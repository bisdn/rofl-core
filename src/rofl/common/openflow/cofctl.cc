/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofctl.h"


cofctl::cofctl(
		crofbase *rofbase,
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				rofbase(rofbase),
				flags(0),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(OFPCR_ROLE_EQUAL),
				cached_generation_id(0),
				socket(new csocket(this, newsd, ra, domain, type, protocol)),
				fragment(0),
				reconnect_in_seconds(RECONNECT_START_TIMEOUT),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL)
{
	WRITELOG(CFWD, DBG, "cofctl(%p)::cofctl() TCP accept", this);

	rofbase->handle_ctl_open(this);
}



cofctl::cofctl(
		crofbase *rofbase,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				rofbase(rofbase),
				flags(COFCTL_FLAG_ACTIVE_SOCKET),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(OFPCR_ROLE_EQUAL),
				cached_generation_id(0),
				socket(new csocket(this, domain, type, protocol)),
				fragment(0),
				reconnect_in_seconds(RECONNECT_START_TIMEOUT),
				reconnect_counter(0),
				rpc_echo_interval(DEFAULT_RPC_ECHO_INTERVAL)
{
	WRITELOG(CFWD, DBG, "cofctl(%p)::cofctl() TCP connect", this);

	flags.set(COFCTL_FLAG_ACTIVE_SOCKET);

	socket->caopen(ra, caddress(AF_INET, "0.0.0.0"), domain, type, protocol);
}



cofctl::~cofctl()
{
	WRITELOG(CFWD, DBG, "cofctl(%p)::~cofctl()", this);

	rofbase->fsptable.delete_fsp_entries(this);

	delete socket;
}



void
cofctl::send_message(
		cofpacket *pack)
{
	switch (pack->ofh_header->type) {
	case OFPT_ERROR:
		{
			// ...
		}
		break;
	case OFPT_EXPERIMENTER:
		{
			// ...
		}
		break;
	case OFPT_FEATURES_REPLY:
		{
			features_reply_sent(pack);
		}
		break;
	case OFPT_GET_CONFIG_REPLY:
		{
			get_config_reply_sent(pack);
		}
		break;
	case OFPT_PACKET_IN:
		{
			// asynchronous ...
		}
		break;
	case OFPT_FLOW_REMOVED:
		{
			// asynchronous ...
		}
		break;
	case OFPT_PORT_STATUS:
		{
			// asynchronous ...
		}
		break;
	case OFPT_STATS_REPLY:
		{
			stats_reply_sent(pack);
		}
		break;
	case OFPT_BARRIER_REPLY:
		{
			barrier_reply_sent(pack);
		}
		break;
	case OFPT_QUEUE_GET_CONFIG_REPLY:
		{
			queue_get_config_reply_sent(pack);
		}
		break;
	case OFPT_ROLE_REPLY:
		{
			role_reply_sent(pack);
		}
		break;
	default:
		{
			WRITELOG(COFCTL, WARN, "cofctl(%p)::send_message() "
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
cofctl::handle_timeout(
		int opaque)
{
	switch (opaque) {
	case COFCTL_TIMER_RECONNECT:
		{
			if (socket)
			{
				socket->caopen(socket->raddr, caddress(AF_INET, "0.0.0.0"), socket->domain, socket->type, socket->protocol);
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
cofctl::handle_accepted(
		csocket *socket,
		int newsd,
		caddress const& ra)
{
	// do nothing
}



void
cofctl::handle_connected(
		csocket *socket,
		int sd)
{
	rofbase->handle_ctl_open(this);
}



void
cofctl::handle_connect_refused(
		csocket *socket,
		int sd)
{
	// TODO: signal event back to rofbase
	if (flags.test(COFCTL_FLAG_ACTIVE_SOCKET))
	{
		try_to_connect();
	}
}



void
cofctl::handle_read(
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
						WRITELOG(COFCTL, WARN, "cofctl(%p)::handle_read() "
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

				WRITELOG(COFCTL, WARN, "cofctl(%p)::handle_read() "
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

	} catch (cerror &e) {

		WRITELOG(COFCTL, WARN, "cofctl(%p)::handle_read() "
				"errno: %d (%s) generic error %s",
				this, errno, strerror(errno), pcppack->c_str());

		if (pcppack)
		{
			delete pcppack; pcppack = (cofpacket*)0;
		}

		handle_closed(socket, sd);

		throw;
	}

}



void
cofctl::handle_closed(
		csocket *socket,
		int sd)
{
	socket->cclose();

	if (flags.test(COFCTL_FLAG_ACTIVE_SOCKET))
	{
		try_to_connect();
	}
	else
	{
		rofbase->handle_ctl_close(this);
	}
}



void
cofctl::handle_message(
		cofpacket *pack)
{
	if (not pack->is_valid())
	{
		WRITELOG(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"dropping invalid packet: %s", this, pack->c_str());
		delete pack; return;
	}

#ifndef NDEBUG
	fprintf(stderr, "r:%d ", pack->ofh_header->type);
#endif

#if 0
	pack->ctl = this;
#endif

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
			echo_reply_rcvd(pack);
		}
		break;
	case OFPT_EXPERIMENTER:
		{
			experimenter_rcvd(pack);
		}
		break;
	case OFPT_FEATURES_REQUEST:
		{
			features_request_rcvd(pack);
		}
		break;
	case OFPT_GET_CONFIG_REQUEST:
		{
			get_config_request_rcvd(pack);
		}
		break;
	case OFPT_SET_CONFIG:
		{
			set_config_rcvd(pack);
		}
		break;
	case OFPT_PACKET_OUT:
		{
			packet_out_rcvd(pack);
		}
		break;
	case OFPT_FLOW_MOD:
		{
			flow_mod_rcvd(pack);
		}
		break;
	case OFPT_GROUP_MOD:
		{
			group_mod_rcvd(pack);
		}
		break;
	case OFPT_PORT_MOD:
		{
			port_mod_rcvd(pack);
		}
		break;
	case OFPT_TABLE_MOD:
		{
			table_mod_rcvd(pack);
		}
		break;
	case OFPT_STATS_REQUEST:
		{
			stats_request_rcvd(pack);
		}
		break;
	case OFPT_BARRIER_REQUEST:
		{
			barrier_request_rcvd(pack);
		}
		break;
	case OFPT_QUEUE_GET_CONFIG_REQUEST:
		{
			queue_get_config_request_rcvd(pack);
		}
		break;
	case OFPT_ROLE_REQUEST:
		{
			role_request_rcvd(pack);
		}
		break;
	default:
		{
			WRITELOG(COFCTL, WARN, "cofctl(%p)::handle_message() "
					"dropping packet: %s", this, pack->c_str());
			delete pack;
		}
		return;
	}
}



void
cofctl::hello_rcvd(cofpacket *pack)
{
	WRITELOG(COFRPC, DBG, "cofctl(%p)::hello_rcvd() pack: %s", this, pack->c_str());

	// OpenFlow versions do not match, send error, close connection
	if (pack->ofh_header->version != OFP_VERSION)
	{
		new_state(STATE_CTL_DISCONNECTED);

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

		send_message(reply);
	}
	else
	{
		new_state(STATE_CTL_ESTABLISHED);

		WRITELOG(COFRPC, DBG, "cofctl(%p): HELLO exchanged with peer entity, attaching ...", this);

		// start sending ECHO requests
		register_timer(COFCTL_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);
	}

	delete pack;
}



void
cofctl::echo_request_sent(cofpacket *pack)
{
	reset_timer(COFCTL_TIMER_ECHO_REPLY_TIMEOUT, 5); // TODO: multiple concurrent echo-requests?
}



void
cofctl::echo_request_rcvd(cofpacket *pack)
{
	// send echo reply back including any appended data
	rofbase->send_echo_reply(this, pack->get_xid(), pack->body.somem(), pack->body.memlen());

	/* Please note: we do not call a handler for echo-requests/replies in rofbase
	 * and take care of these liveness packets within cofctl and cofdpt
	 */
}



void
cofctl::echo_reply_rcvd(cofpacket *pack)
{
	cancel_timer(COFCTL_TIMER_ECHO_REPLY_TIMEOUT);
	register_timer(COFCTL_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);
}



void
cofctl::features_request_rcvd(cofpacket *pack)
{
	try {
		xidstore.xid_add(this, be32toh(pack->ofh_header->xid), 0);

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::features_request_rcvd() retransmission", this);
	}

	rofbase->handle_features_request(this, pack);
}



void
cofctl::features_reply_sent(cofpacket *pack)
{
	uint32_t xid = be32toh(pack->ofh_header->xid);
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::features_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}



void
cofctl::get_config_request_rcvd(cofpacket *pack)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		send_error_is_slave(pack); return;
	}

	rofbase->handle_get_config_request(this, pack);
}



void
cofctl::get_config_reply_sent(cofpacket *pack)
{
	uint32_t xid = be32toh(pack->ofh_header->xid);
	try {

		xidstore.xid_find(xid);

		xidstore.xid_rem(xid);

	} catch (eXidStoreNotFound& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::get_config_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}



void
cofctl::set_config_rcvd(cofpacket *pack)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		send_error_is_slave(pack); return;
	}

	rofbase->handle_set_config(this, pack);
}



void
cofctl::packet_out_rcvd(cofpacket *pack)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		send_error_is_slave(pack); return;
	}

	rofbase->handle_packet_out(this, pack);
}



void
cofctl::flow_mod_rcvd(cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "cofctl(%p)::flow_mod_rcvd() pack: %s", this, pack->c_str());

	try {
		if (OFPCR_ROLE_SLAVE == role)
		{
			send_error_is_slave(pack); return;
		}

		// check, whether the controlling pack->entity is allowed to install this flow-mod
		if (rofbase->fe_flags.test(crofbase::NSP_ENABLED))
		{
			switch (pack->ofh_flow_mod->command) {
			case OFPFC_ADD:
			case OFPFC_MODIFY:
			case OFPFC_MODIFY_STRICT:
				rofbase->fsptable.flow_mod_allowed(this, pack->match);
				break;
				/*
				 * this allows generic DELETE commands to be applied
				 * FIXME: does this affect somehow entries from other controllers?
				 */
			}
		}


		rofbase->handle_flow_mod(this, pack);


	} catch (eLockWouldBlock& e) {

		throw;

	} catch (eLockInval& e) {

		throw;

	} catch (eActionBadLen& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"invalid flow-mod packet received: action with bad length", this);

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_LEN,
					pack->soframe(), pack->framelen());

		delete pack;


	} catch (eFspNotAllowed& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"-FLOW-MOD- blocked due to mismatch in nsp "
				"registration", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_EPERM,
				pack->soframe(), pack->framelen());

		delete pack;

	} catch (eRofBaseTableNotFound& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"invalid flow-table %d specified",
				this, pack->ofh_flow_mod->table_id);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_BAD_TABLE_ID,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eInstructionInvalType& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"unknown instruction found", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNKNOWN_INST,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eRofBaseGotoTableNotFound& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"GOTO-TABLE instruction with invalid table-id", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_BAD_TABLE_ID,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eInstructionBadExperimenter& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"unknown OFPIT_EXPERIMENTER extension received", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNSUP_EXP_INST,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eOFmatchInvalBadValue& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"bad value in match structure", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_BAD_MATCH,
				OFPBMC_BAD_VALUE,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (cerror &e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"default catch for cerror exception", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_UNKNOWN,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	}
}



void
cofctl::group_mod_rcvd(cofpacket *pack)
{
	try {

		if (OFPCR_ROLE_SLAVE == role)
		{
			send_error_is_slave(pack); return;
		}

		rofbase->handle_group_mod(this, pack);


	} catch (eActionBadLen& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_group_mod() "
				"invalid group-mod packet received: action with "
				"bad length", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_BAD_ACTION,
				OFPBAC_BAD_LEN,
				pack->soframe(),
				pack->framelen());

		delete pack;

	} catch (...) {

		delete pack;
	}
}



void
cofctl::port_mod_rcvd(cofpacket *pack) throw (eOFctlPortNotFound)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		send_error_is_slave(pack); return;
	}

	rofbase->handle_port_mod(this, pack);
}


void
cofctl::table_mod_rcvd(cofpacket *pack)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		send_error_is_slave(pack); return;
	}

	rofbase->handle_table_mod(this, pack);
}



void
cofctl::stats_request_rcvd(cofpacket *pack)
{
	try {
		xidstore.xid_add(this, be32toh(pack->ofh_header->xid));

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::stats_request_rcvd() retransmission xid:0x%x",
				this, be32toh(pack->ofh_header->xid));
	}

	switch (be16toh(pack->ofh_stats_request->type)) {
	case OFPST_DESC:
		{
			rofbase->handle_desc_stats_request(this, pack);
		}
		break;
	case OFPST_TABLE:
		{
			rofbase->handle_table_stats_request(this, pack);
		}
		break;
	case OFPST_PORT:
		{
			rofbase->handle_port_stats_request(this, pack);
		}
		break;
	case OFPST_FLOW:
		{
			rofbase->handle_flow_stats_request(this, pack);
		}
		break;
	case OFPST_AGGREGATE:
		{
			rofbase->handle_aggregate_stats_request(this, pack);
		}
		break;
	case OFPST_QUEUE:
		{
			rofbase->handle_queue_stats_request(this, pack);
		}
		break;
	case OFPST_GROUP:
		{
			rofbase->handle_group_stats_request(this, pack);
		}
		break;
	case OFPST_GROUP_DESC:
		{
			rofbase->handle_group_desc_stats_request(this, pack);
		}
		break;
	case OFPST_GROUP_FEATURES:
		{
			rofbase->handle_group_features_stats_request(this, pack);
		}
		break;
	case OFPST_EXPERIMENTER:
		{
			rofbase->handle_experimenter_stats_request(this, pack);
		}
		break;
	default:
		{
			WRITELOG(CROFBASE, WARN, "crofbase(%p)::recv_stats_request() "
					"unknown stats request type (%d)",
					this, be16toh(pack->ofh_stats_request->type));

			rofbase->handle_stats_request(this, pack);
		}
		break;
	}
}



void
cofctl::stats_reply_sent(cofpacket *pack)
{
	uint32_t xid = be32toh(pack->ofh_header->xid);
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
cofctl::role_request_rcvd(cofpacket *pack)
{
	try {
		xidstore.xid_add(this, be32toh(pack->ofh_header->xid));

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::role_request_rcvd() retransmission xid:0x%x",
				this, be32toh(pack->ofh_header->xid));
	}

	switch (be32toh(pack->ofh_role_request->role)) {
	case OFPCR_ROLE_MASTER:
	case OFPCR_ROLE_SLAVE:
		if (role_initialized)
		{
			uint64_t gen_id = be64toh(pack->ofh_role_request->generation_id);
			uint64_t dist = (gen_id > cached_generation_id) ?
					(gen_id - cached_generation_id) % std::numeric_limits<uint64_t>::max() :
					(gen_id + std::numeric_limits<uint64_t>::max() + cached_generation_id) % std::numeric_limits<uint64_t>::max();

			if (dist >= (std::numeric_limits<uint64_t>::max() / 2))
			{
				rofbase->send_error_message(this, pack->get_xid(), OFPET_ROLE_REQUEST_FAILED, OFPRRFC_STALE);
				return;
			}
		}
		else
		{
			role_initialized = true;
		}
		cached_generation_id = be64toh(pack->ofh_role_request->generation_id);
		break;
	default:
		break;
	}

	role = be32toh(pack->ofh_role_request->role);

#if 0
	for (std::map<cofbase*, cofctl*>::iterator
			it = rofbase->ofctrl_list.begin(); it != rofbase->ofctrl_list.end(); ++it)
	{
		cofctl* ofctrl = it->second;

		if (ofctrl == this)
		{
			continue;
		}

		if (OFPCR_ROLE_MASTER == ofctrl->role)
		{
			ofctrl->role = OFPCR_ROLE_SLAVE;
		}
	}
#endif

	//pack->ofh_role_request->generation_id;

	rofbase->handle_role_request(this, pack);
}



void
cofctl::role_reply_sent(cofpacket *pack)
{
	uint32_t xid = be32toh(pack->ofh_header->xid);
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
cofctl::barrier_request_rcvd(cofpacket *pack)
{
	try {
		xidstore.xid_add(this, be32toh(pack->ofh_header->xid));

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::barrier_request_rcvd() retransmission xid:0x%x",
				this, be32toh(pack->ofh_header->xid));
	}

	rofbase->handle_barrier_request(this, pack);
}



void
cofctl::barrier_reply_sent(cofpacket *pack)
{
	uint32_t xid = be32toh(pack->ofh_header->xid);
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
cofctl::queue_get_config_request_rcvd(cofpacket *pack)
{
	try {
		xidstore.xid_add(this, be32toh(pack->ofh_header->xid));

	} catch (eXidStoreXidBusy& e) {
		WRITELOG(COFCTL, WARN, "cofctl(%p)::queue_get_config_request_rcvd() retransmission xid:0x%x",
				this, be32toh(pack->ofh_header->xid));
	}

	rofbase->handle_queue_get_config_request(this, pack);
}



void
cofctl::queue_get_config_reply_sent(cofpacket *pack)
{
	uint32_t xid = be32toh(pack->ofh_header->xid);
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
cofctl::experimenter_rcvd(cofpacket *pack)
{
	switch (be32toh(pack->ofh_experimenter->experimenter)) {
	case OFPEXPID_ROFL:
	{
		switch (be32toh(pack->ofh_experimenter->exp_type)) {
		case croflexp::OFPRET_FLOWSPACE:
		{
			croflexp rexp(pack->body.somem(), pack->body.memlen());

			switch (rexp.rext_fsp->command) {
			case croflexp::OFPRET_FSP_ADD:
			{
				try {

					WRITELOG(COFCTL, DBG, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => pending for %s",
							this, rexp.match.c_str());

					rofbase->fsptable.insert_fsp_entry(this, rexp.match);

					WRITELOG(COFCTL, DBG, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => -ADDED- %s\n%s",
							this, c_str(), rofbase->fsptable.c_str());

				} catch (eFspEntryOverlap& e) {

					WRITELOG(COFCTL, DBG, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => -REJECTED- (overlap)",
							this);

				}

				break;
			}
			case croflexp::OFPRET_FSP_DELETE:
			{
				try {

					WRITELOG(COFCTL, DBG, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => pending for %s",
							this, rexp.match.c_str());

					rofbase->fsptable.delete_fsp_entry(this, rexp.match, true /*strict*/);

					WRITELOG(COFCTL, DBG, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => -DELETED- %s\n%s",
							this, c_str(), rofbase->fsptable.c_str());

				} catch (eFspEntryNotFound& e) {

					WRITELOG(COFCTL, DBG, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => -NOT-FOUND-",
							this);

				}

				break;
			}
			}

			break;
		}

		}

		delete pack;
		break;
	}


	default:
		rofbase->handle_experimenter_message(this, pack);
		break;
	}
}



void
cofctl::handle_echo_reply_timeout()
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



const char*
cofctl::c_str()
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
cofctl::transaction(uint32_t xid)
{
	return xidstore.xid_find(xid);
}



void
cofctl::send_error_is_slave(cofpacket *pack)
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
cofctl::try_to_connect(bool reset_timeout)
{
	if (pending_timer(COFCTL_TIMER_RECONNECT))
	{
		return;
	}

	WRITELOG(COFCTL, DBG, "cofctl(%p)::try_to_connect() "
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

	reset_timer(COFCTL_TIMER_RECONNECT, reconnect_in_seconds);

	++reconnect_counter;
}



void
cofctl::send_message_via_socket(
		cofpacket *pack)
{
	if (0 == socket)
	{
		delete pack; return;
	}

	cmemory *mem = new cmemory(pack->length());

	pack->pack(mem->somem(), mem->memlen());

	WRITELOG(COFCTL, DBG, "cofctl(%p): new cmemory: %s",
				this, mem->c_str());

	delete pack;

	socket->send_packet(mem);
}



#if 0
switch (pack->ofh_header->type) {
case OFPT_HELLO:
	break;
case OFPT_ERROR:
	break;
case OFPT_ECHO_REQUEST:
	break;
case OFPT_ECHO_REPLY:
	break;
case OFPT_EXPERIMENTER:
	break;
case OFPT_FEATURES_REQUEST:
	break;
case OFPT_FEATURES_REPLY:
	break;
case OFPT_GET_CONFIG_REQUEST:
	break;
case OFPT_GET_CONFIG_REPLY:
	break;
case OFPT_SET_CONFIG:
	break;
case OFPT_PACKET_IN:
	break;
case OFPT_FLOW_REMOVED:
	break;
case OFPT_PORT_STATUS:
	break;
case OFPT_PACKET_OUT:
	break;
case OFPT_FLOW_MOD:
	break;
case OFPT_GROUP_MOD:
	break;
case OFPT_PORT_MOD:
	break;
case OFPT_TABLE_MOD:
	break;
case OFPT_STATS_REQUEST:
	break;
case OFPT_STATS_REPLY:
	break;
case OFPT_BARRIER_REQUEST:
	break;
case OFPT_BARRIER_REPLY:
	break;
case OFPT_QUEUE_GET_CONFIG_REQUEST:
	break;
case OFPT_QUEUE_GET_CONFIG_REPLY:
	break;
case OFPT_ROLE_REQUEST:
	break;
case OFPT_ROLE_REPLY:
	break;
}
#endif
