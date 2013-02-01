/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofctl.h"

using namespace rofl;

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
	WRITELOG(CFWD, TRACE, "cofctl(%p)::cofctl() TCP accept", this);

        register_timer(COFCTL_TIMER_SEND_HELLO, 0);
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
	WRITELOG(COFCTL, TRACE, "cofctl(%p)::cofctl() TCP connect", this);

	flags.set(COFCTL_FLAG_ACTIVE_SOCKET);

	socket->caopen(ra, caddress(AF_INET, "0.0.0.0"), domain, type, protocol);
}



cofctl::~cofctl()
{
	WRITELOG(COFCTL, TRACE, "cofctl(%p)::~cofctl()", this);

	rofbase->fsptable.delete_fsp_entries(this);

	delete socket;
}



void
cofctl::send_message(
		cofpacket *pack)
{
    if (not flags.test(COFCTL_FLAG_HELLO_RCVD) && (pack->ofh_header->type != OFPT_HELLO))
    {
        WRITELOG(COFCTL, TRACE, "cofctrl(%p)::send_message() "
            "dropping message, as no HELLO rcvd from peer yet, pack: %s",
            this, pack->c_str());
        delete pack; return;
    }

    switch (pack->ofh_header->type) {
    case OFPT_HELLO:
            {
                    // ...
            }
            break;
    case OFPT_ERROR:
            {
                    // ...
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
#ifndef NDEBUG
	caddress raddr(ra);
	fprintf(stderr, "A:ctl[%s] ", raddr.c_str());
#endif
	writelog(COFCTL, WARN, "cofctl(%p)::handle_accepted() "
			"local:%s remote: %s",
			this, socket->laddr.c_str(), raddr.c_str());

	// do nothing
}



void
cofctl::handle_connected(
		csocket *socket,
		int sd)
{
#ifndef NDEBUG
	fprintf(stderr, "C:ctl[%s] ", socket->raddr.c_str());
#endif
	writelog(COFCTL, WARN, "cofctl(%p)::handle_connected() "
			"local:%s remote: %s",
			this, socket->laddr.c_str(), socket->raddr.c_str());

	register_timer(COFCTL_TIMER_SEND_HELLO, 0);
}



void
cofctl::handle_connect_refused(
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
cofctl::handle_read(
		csocket *socket,
		int sd)
{
	int rc;

	cofpacket *pcppack = (cofpacket*)0;
	try {

		pcppack = (0 != fragment) ? fragment : new cofpacket();

		while (true) {
			// SSL support: client or server side, done in derived class

			// TODO: this will be replaced with SSL socket later
			rc = read(sd, (void*) pcppack->memptr(), pcppack->need_bytes());


			if (rc < 0) // error occured (or non-blocking)
			{
				switch(errno) {
				case EAGAIN: {
					fragment = pcppack;	// more bytes are needed, store pcppack in fragment pointer
				}
					return;
				case ECONNREFUSED: {
					try_to_connect(); // reconnect
				}
					return;
				case ECONNRESET:
				default: {
					writelog(COFCTL, WARN, "cofctl(%p)::handle_read() "
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

				writelog(COFCTL, INFO, "cofctl(%p)::handle_read() "
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

		writelog(COFCTL, WARN, "cofctl(%p)::handle_read() "
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
cofctl::handle_closed(
		csocket *socket,
		int sd)
{
	writelog(COFCTL, WARN, "cofctl(%p)::handle_closed() "
			"local:%s remote: %s",
			this, socket->laddr.c_str(), socket->raddr.c_str());

	socket->cclose();

	cancel_timer(COFCTL_TIMER_ECHO_REPLY_TIMEOUT);
	cancel_timer(COFCTL_TIMER_SEND_ECHO_REQUEST);

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
	try {
		if (not pack->is_valid())
		{
			writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
					"dropping invalid packet: %s", this, pack->c_str());
			delete pack; return;
		}

		if (not flags.test(COFCTL_FLAG_HELLO_RCVD) && (pack->ofh_header->type != OFPT_HELLO)) {
			writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"no HELLO rcvd yet, dropping message, pack: %s", this, pack->c_str());
			delete pack; return;
		}

		switch (pack->ofh_header->type) {
		case OFPT_HELLO: {
			hello_rcvd(pack);
		} break;
		case OFPT_ECHO_REQUEST: {
			echo_request_rcvd(pack);
		} break;
		case OFPT_ECHO_REPLY: {
			echo_reply_rcvd(pack);
		} break;
		case OFPT_EXPERIMENTER:	{
			experimenter_rcvd(pack);
		} break;
		case OFPT_FEATURES_REQUEST:	{
			features_request_rcvd(pack);
		} break;
		case OFPT_GET_CONFIG_REQUEST: {
			get_config_request_rcvd(pack);
		} break;
		case OFPT_SET_CONFIG: {
			set_config_rcvd(pack);
		} break;
		case OFPT_PACKET_OUT: {
			packet_out_rcvd(pack);
		} break;
		case OFPT_FLOW_MOD: {
			flow_mod_rcvd(pack);
		} break;
		case OFPT_GROUP_MOD: {
			group_mod_rcvd(pack);
		} break;
		case OFPT_PORT_MOD: {
			port_mod_rcvd(pack);
		} break;
		case OFPT_TABLE_MOD: {
			table_mod_rcvd(pack);
		} break;
		case OFPT_STATS_REQUEST: {
			stats_request_rcvd(pack);
		} break;
		case OFPT_BARRIER_REQUEST: {
			barrier_request_rcvd(pack);
		} break;
		case OFPT_QUEUE_GET_CONFIG_REQUEST: {
			queue_get_config_request_rcvd(pack);
		} break;
		case OFPT_ROLE_REQUEST: {
			role_request_rcvd(pack);
		} break;
		default: {
			WRITELOG(COFCTL, ERROR, "cofctl(%p)::handle_message() "
					"dropping unknown packet: %s", this, pack->c_str());
			delete pack;
		} return;
		}

	} catch (eBadRequestBadVersion& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"ofp_header.version not supported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_VERSION,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBadType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"ofp_header.type not supported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_TYPE,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBadStat& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"ofp_stats_request.type not supported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBadExperimenter& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Experimenter id not supported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_EXPERIMENTER,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBadExpType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Experimenter type not supported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_EXP_TYPE,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestEperm& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Permissions error, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_EPERM,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBadLen& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Wrong request length for type, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_LEN,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBufferEmpty& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Specified buffer has already been used, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BUFFER_EMPTY,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBufferUnknown& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Specified buffer does not exist, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BUFFER_UNKNOWN,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBadTableId& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Specified table-id invalid or does not exist, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_TABLE_ID,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestIsSlave& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Denied because controller is slave, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_IS_SLAVE,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBadPort& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Invalid port, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_PORT,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBadPacket& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Invalid packet in packet-out, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_REQUEST,
					OFPBRC_BAD_PACKET,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadRequestBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"base class of exception eBadRequestBase caught, pack: %s", this, pack->c_str());

		delete pack;
	} catch (eBadActionBadType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unknown action type, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_TYPE,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionBadLen& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Length problem in actions, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_LEN,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionBadExperimenter& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unknown experimenter id specified, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_EXPERIMENTER,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionBadExperimenterType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unknown action type for experimenter id, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_EXPERIMENTER_TYPE,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionBadOutPort& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Problem validating output port, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_OUT_PORT,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionBadArgument& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Bad action argument, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_ARGUMENT,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionEperm& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Permissions error, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_EPERM,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionTooMany& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Can't handle this many actions, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_TOO_MANY,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionBadQueue& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Problem validating output queue, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_QUEUE,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionBadOutGroup& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Invalid group id in forward action, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_OUT_GROUP,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionMatchInconsistent& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Action can't apply for this match, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_MATCH_INCONSISTENT,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionUnsupportedOrder& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Action order is unsupported for the action list in an Apply-Actions instruction, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_UNSUPPORTED_ORDER,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionBadTag& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Actions uses an unsupported tag/encap, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_TAG,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadActionBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"base class of exception eBadActionBase caught, pack: %s", this, pack->c_str());

		delete pack;
	} catch (eBadInstUnknownInst& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unknown instruction, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNKNOWN_INST,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadInstUnsupInst& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Switch or table does not support the instruction, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_INST,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadInstBadTableId& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Invalid Table-ID specified, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_INSTRUCTION,
					OFPBIC_BAD_TABLE_ID,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadInstUnsupMetadata& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Metadata value unsupported by datapath, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_METADATA,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadInstUnsupMetadataMask& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Metadata mask value unsupported by datapath, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_METADATA_MASK,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadInstUnsupExpInst& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Specific experimenter instruction unsupported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_INSTRUCTION,
					OFPBIC_UNSUP_EXP_INST,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadInstBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"base class of exception eBadInstBase caught, pack: %s", this, pack->c_str());

		delete pack;
	} catch (eBadMatchBadType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported match type specified by the match, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_MATCH,
					OFPBMC_BAD_TYPE,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadMatchBadLen& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Length problem in match, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_MATCH,
					OFPBMC_BAD_LEN,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadMatchBadTag& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Match uses an unsupported tag/encap, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_MATCH,
					OFPBMC_BAD_TAG,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadMatchBadDlAddrMask& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported datalink addr mask, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_MATCH,
					OFPBMC_BAD_DL_ADDR_MASK,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadMatchBadNwAddrMask& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported network addr mask, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_MATCH,
					OFPBMC_BAD_NW_ADDR_MASK,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadMatchBadWildcards& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported wildcard specified in the match, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_MATCH,
					OFPBMC_BAD_WILDCARDS,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadMatchBadField& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported field in the match, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_MATCH,
					OFPBMC_BAD_FIELD,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadMatchBadValue& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"Unsupported value in a match field, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_BAD_MATCH,
					OFPBMC_BAD_VALUE,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eBadMatchBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::handle_message() "
				"base class of exception eBadMatchBase caught, pack: %s", this, pack->c_str());

		delete pack;
	}
}



void
cofctl::hello_rcvd(cofpacket *pack)
{
	try {
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

			throw eHelloIncompatible();
		}
		else
		{
			flags.set(COFCTL_FLAG_HELLO_RCVD);

			new_state(STATE_CTL_ESTABLISHED);

			WRITELOG(COFRPC, DBG, "cofctl(%p)::hello_rcvd() "
				"HELLO exchanged with peer entity, attaching ...", this);

			if (flags.test(COFCTL_FLAG_HELLO_SENT))
			{
				rofbase->send_echo_request(this);

				rofbase->handle_ctl_open(this);
			}
		}

		delete pack;

	} catch (eHelloIncompatible& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::hello_rcvd() "
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

		writelog(COFCTL, WARN, "cofctl(%p)::hello_rcvd() "
				"Permissions error, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_HELLO_FAILED,
					OFPHFC_EPERM,
					pack->soframe(), pack->framelen());

		delete pack;
		handle_closed(socket, socket->sd);
	} catch (eHelloBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::hello_rcvd() "
				"base class of exception eHelloBase caught, pack: %s", this, pack->c_str());

		delete pack;
	}
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
		WRITELOG(COFCTL, ERROR, "cofctl(%p)::features_request_rcvd() retransmission", this);
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
		WRITELOG(COFCTL, ERROR, "cofctl(%p)::features_reply_sent() "
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
		WRITELOG(COFCTL, ERROR, "cofctl(%p)::get_config_reply_sent() "
				"xid:0x%x not found",
				this, xid);
	}
}



void
cofctl::set_config_rcvd(cofpacket *pack)
{
	try {
		if (OFPCR_ROLE_SLAVE == role)
		{
			send_error_is_slave(pack); return;
		}

		rofbase->handle_set_config(this, pack);

	} catch (eSwitchConfigBadFlags& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::set_config_rcvd() "
				"Specified flags is invalid, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_SWITCH_CONFIG_FAILED,
					OFPSCFC_BAD_FLAGS,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eSwitchConfigBadLen& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::set_config_rcvd() "
				"Specified len is invalid, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_SWITCH_CONFIG_FAILED,
					OFPSCFC_BAD_LEN,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eSwitchConfigBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::set_config_rcvd() "
				"base class of exception eSwitchConfigBase caught, pack: %s", this, pack->c_str());

		delete pack;
	}
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
	WRITELOG(COFCTL, TRACE, "cofctl(%p)::flow_mod_rcvd() pack: %s", this, pack->c_str());

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

	} catch (eFlowModUnknown& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"unspecified error, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_UNKNOWN,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eFlowModTableFull& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"table full, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_TABLE_FULL,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eFlowModBadTableId& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"bad table-id, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_BAD_TABLE_ID,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eFlowModOverlap& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"flow-mod overlaps while CHECK_OVERLAP flag is set, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_OVERLAP,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eFlowModEperm& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"permissions error, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_EPERM,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eFlowModBadTimeout&e ) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"bad timeout value, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_BAD_TIMEOUT,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eFlowModBadCommand& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"bad command value, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
					this,
					pack->get_xid(),
					OFPET_FLOW_MOD_FAILED,
					OFPFMFC_BAD_COMMAND,
					pack->soframe(), pack->framelen());

		delete pack;
	} catch (eFlowModBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::flow_mod_rcvd() "
				"base class of exception eFlowModBase caught, pack: %s", this, pack->c_str());

		delete pack;
	} catch (eFspNotAllowed& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"-FLOW-MOD- blocked due to mismatch in flowspace "
				"registration\n%s", this, rofbase->fsptable.c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_EPERM,
				pack->soframe(), pack->framelen());

		delete pack;
	} catch (eRofBaseTableNotFound& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
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

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"unknown instruction found", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNKNOWN_INST,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (eRofBaseGotoTableNotFound& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"GOTO-TABLE instruction with invalid table-id", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_BAD_TABLE_ID,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (eInstructionBadExperimenter& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"unknown OFPIT_EXPERIMENTER extension received", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNSUP_EXP_INST,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (eOFmatchInvalBadValue& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
				"bad value in match structure", this);

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_BAD_MATCH,
				OFPBMC_BAD_VALUE,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (cerror &e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::flow_mod_rcvd() "
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

	} catch (eGroupModExists& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group-id already exists, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_GROUP_EXISTS,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModInvalGroup& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group specified is invalid, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_INVALID_GROUP,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModWeightUnsupported& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group weight unsupported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_WEIGHT_UNSUPPORTED,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModOutOfGroups& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"out of groups, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_OUT_OF_GROUPS,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModOutOfBuckets& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"out of buckets, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_OUT_OF_BUCKETS,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModChainingUnsupported& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"chaining unsupported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_CHAINING_UNSUPPORTED,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModWatchUnsupported& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group watch unsupported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_WATCH_UNSUPPORTED,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModLoop& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group would cause a loop, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_LOOP,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModUnknownGroup& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group-modify for non-existing group attempted, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_UNKNOWN_GROUP,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModChainedGroup& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"group not deleted, as another group is forwarding to it, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_CHAINED_GROUP,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModBadType& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"unsupported or unknown group type, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_TYPE,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModBadCommand& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"unsupported or unknown command, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_COMMAND,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModBadBucket& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"error in bucket, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_BUCKET,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModBadWatch& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"error in watch port/group, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_BAD_WATCH,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModEperm& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"permissions error, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_GROUP_MOD_FAILED,
				OFPGMFC_EPERM,
				pack->soframe(),
				pack->framelen());

		delete pack;
	} catch (eGroupModBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::group_mod_rcvd() "
				"base class of exception eGroupModBase caught, pack: %s", this, pack->c_str());

		delete pack;
	}
}



void
cofctl::port_mod_rcvd(cofpacket *pack)
{
	try {
		if (OFPCR_ROLE_SLAVE == role)
		{
			send_error_is_slave(pack); return;
		}

		rofbase->handle_port_mod(this, pack);

	} catch (ePortModBadPort& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"Specified port number does not exist, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_PORT,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (ePortModBadHwAddr& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"Specified hardware address does not match the port number, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_HW_ADDR,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (ePortModBadConfig& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"Specified config is invalid, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_CONFIG,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (ePortModBadAdvertise& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"Specified advertise is invalid, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_PORT_MOD_FAILED,
				OFPPMFC_BAD_ADVERTISE,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (ePortModBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::port_mod_rcvd() "
				"base class of exception ePortModBase caught, pack: %s", this, pack->c_str());

		delete pack;
	}
}


void
cofctl::table_mod_rcvd(cofpacket *pack)
{
	try {
		if (OFPCR_ROLE_SLAVE == role)
		{
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
			WRITELOG(COFCTL, WARN, "cofctl(%p)::recv_stats_request() "
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
					throw eRoleRequestStale();
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

	} catch (eRoleRequestStale& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::role_request_rcvd() "
				"Stale Message: old generation_id, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_ROLE_REQUEST_FAILED,
				OFPRRFC_STALE,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (eRoleRequestUnsupported& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::role_request_rcvd() "
				"Controller role change unsupported, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_ROLE_REQUEST_FAILED,
				OFPRRFC_UNSUP,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (eRoleRequestBadRole& e) {

		writelog(COFCTL, ERROR, "cofctl(%p)::role_request_rcvd() "
				"Invalid role, pack: %s", this, pack->c_str());

		rofbase->send_error_message(
				this,
				pack->get_xid(),
				OFPET_ROLE_REQUEST_FAILED,
				OFPRRFC_BAD_ROLE,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;
	} catch (eRoleRequestBase& e) {

		writelog(COFCTL, WARN, "cofctl(%p)::role_request_rcvd() "
				"base class of exception eRoleRequestBase caught, pack: %s", this, pack->c_str());

		delete pack;
	}
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

					WRITELOG(COFCTL, TRACE, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => pending for %s",
							this, rexp.match.c_str());

					rofbase->fsptable.insert_fsp_entry(this, rexp.match);

					WRITELOG(COFCTL, TRACE, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => -ADDED- %s\n%s",
							this, c_str(), rofbase->fsptable.c_str());

				} catch (eFspEntryOverlap& e) {

					WRITELOG(COFCTL, TRACE, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => -REJECTED- (overlap)",
							this);

				}

				break;
			}
			case croflexp::OFPRET_FSP_DELETE:
			{
				try {

					WRITELOG(COFCTL, TRACE, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => pending for %s",
							this, rexp.match.c_str());

					rofbase->fsptable.delete_fsp_entry(this, rexp.match, true /*strict*/);

					WRITELOG(COFCTL, TRACE, "cofctl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => -DELETED- %s\n%s",
							this, c_str(), rofbase->fsptable.c_str());

				} catch (eFspEntryNotFound& e) {

					WRITELOG(COFCTL, TRACE, "cofctl(%p)::experimenter_message_rcvd() "
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

	WRITELOG(COFCTL, INFO, "cofctl(%p)::try_to_connect() "
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

	WRITELOG(COFCTL, DBG, "cofctl(%p)::send_message_via_socket() new cmemory: %s",
				this, mem->c_str());

	delete pack;

	socket->send_packet(mem);
}


