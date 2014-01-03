/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crofctlImpl.h"

using namespace rofl;


crofctlImpl::crofctlImpl(
		crofbase *rofbase,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap) :
				crofctl(rofbase),
				ctid(0),
				rofbase(rofbase),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(openflow12::OFPCR_ROLE_EQUAL),
				cached_generation_id(0),
				rofchan(this, versionbitmap)
{

}



crofctlImpl::crofctlImpl(
		crofbase *rofbase,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int newsd) :
				crofctl(rofbase),
				ctid(0),
				rofbase(rofbase),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(openflow12::OFPCR_ROLE_EQUAL),
				cached_generation_id(0),
				rofchan(this, versionbitmap)
{
	rofchan.add_conn(/*aux-id=*/0, newsd);
}



crofctlImpl::crofctlImpl(
		crofbase *rofbase,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol) :
				crofctl(rofbase),
				ctid(0),
				rofbase(rofbase),
				miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
				role_initialized(false),
				role(openflow12::OFPCR_ROLE_EQUAL),
				cached_generation_id(0),
				rofchan(this, versionbitmap)
{
	rofchan.add_conn(/*aux-id=*/0, domain, type, protocol, ra);
}



crofctlImpl::~crofctlImpl()
{
	rofbase->fsptable.delete_fsp_entries(this);
}




void
crofctlImpl::handle_timeout(
		int opaque)
{

}



void
crofctlImpl::handle_connected(
		rofl::openflow::crofchan *chan,
		uint8_t aux_id)
{
	logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " connection established:" << std::endl << *chan;
}



void
crofctlImpl::handle_closed(
		rofl::openflow::crofchan *chan,
		uint8_t aux_id)
{
	logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " connection closed:" << std::endl << *chan;

	if (0 == aux_id) {
		rofbase->rpc_ctl_failed(this); // send notification to crofbase, when main connection has been closed
	}
}



bool
crofctlImpl::is_slave() const
{
	switch (rofchan.get_version()) {
	case openflow12::OFP_VERSION: return (openflow12::OFPCR_ROLE_SLAVE == role);
	case openflow13::OFP_VERSION: return (openflow13::OFPCR_ROLE_SLAVE == role);
	default: return false;
	}
}



void
crofctlImpl::recv_message(
		rofl::openflow::crofchan *chan,
		uint8_t aux_id,
		cofmsg *msg)
{
	try {
		switch (msg->get_type()) {
		case rofl::openflow::OFPT_FEATURES_REQUEST: {
			features_request_rcvd(dynamic_cast<cofmsg_features_request*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_GET_CONFIG_REQUEST: {
			get_config_request_rcvd(dynamic_cast<cofmsg_get_config_request*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_SET_CONFIG: {
			set_config_rcvd(dynamic_cast<cofmsg_set_config*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_PACKET_OUT: {
			packet_out_rcvd(dynamic_cast<cofmsg_packet_out*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_FLOW_MOD: {
			flow_mod_rcvd(dynamic_cast<cofmsg_flow_mod*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_GROUP_MOD: {
			group_mod_rcvd(dynamic_cast<cofmsg_group_mod*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_PORT_MOD: {
			port_mod_rcvd(dynamic_cast<cofmsg_port_mod*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_TABLE_MOD: {
			table_mod_rcvd(dynamic_cast<cofmsg_table_mod*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_MULTIPART_REQUEST: {
			stats_request_rcvd(dynamic_cast<cofmsg_stats_request*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_BARRIER_REQUEST: {
			barrier_request_rcvd(dynamic_cast<cofmsg_barrier_request*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_QUEUE_GET_CONFIG_REQUEST: {
			queue_get_config_request_rcvd(dynamic_cast<cofmsg_queue_get_config_request*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_ROLE_REQUEST: {
			role_request_rcvd(dynamic_cast<cofmsg_role_request*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_GET_ASYNC_REQUEST: {
			get_async_config_request_rcvd(dynamic_cast<cofmsg_get_async_config_request*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_SET_ASYNC: {
			set_async_config_rcvd(dynamic_cast<cofmsg_set_async_config*>( msg ), aux_id);
		} break;
		case rofl::openflow::OFPT_METER_MOD: {
			//TODO: meter_mod_rcvd(dynamic_cast<cofmsg_meter_mod*>( msg ), aux_id);
		} break;
		default: {

		};
		}


	} catch (eBadSyntaxTooShort& e) {

		logging::error << "eBadSyntaxTooShort " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_len(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadVersion& e) {

		logging::error << "eBadVersion " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_version(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBadVersion& e) {

		logging::error << "eBadRequestBadVersion " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_version(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBadType& e) {

		logging::error << "eBadRequestBadType " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_type(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBadStat& e) {

		logging::error << "eBadRequestBadStat " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_stat(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBadExperimenter& e) {

		logging::error << "eBadRequestBadExperimenter " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_experimenter(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBadExpType& e) {

		logging::error << "eBadRequestBadExpType " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_exp_type(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestEperm& e) {

		logging::error << "eBadRequestEperm " << *msg << std::endl;
		rofbase->send_error_bad_request_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBadLen& e) {

		logging::error << "eBadRequestBadLen " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_len(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBufferEmpty& e) {

		logging::error << "eBadRequestBufferEmpty " << *msg << std::endl;
		rofbase->send_error_bad_request_buffer_empty(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBufferUnknown& e) {

		logging::error << "eBadRequestBufferUnknown " << *msg << std::endl;
		rofbase->send_error_bad_request_buffer_unknown(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBadTableId& e) {

		logging::error << "eBadRequestBadTableId " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_table_id(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestIsSlave& e) {

		logging::error << "eBadRequestIsSlave " << *msg << std::endl;
		rofbase->send_error_bad_request_is_slave(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBadPort& e) {

		logging::error << "eBadRequestBadPort " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_port(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBadPacket& e) {

		logging::error << "eBadRequestBadPacket " << *msg << std::endl;
		rofbase->send_error_bad_request_bad_packet(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestMultipartBufferOverflow& e) {

		logging::error << "eBadRequestMultipartBufferOverflow " << *msg << std::endl;
		rofbase->send_error_bad_request_multipart_buffer_overflow(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadRequestBase& e) {

		logging::error << "eBadRequestBase " << *msg << std::endl;
		delete msg;

	} catch (eBadActionBadType& e) {

		logging::error << "eBadActionBadType " << *msg << std::endl;
		rofbase->send_error_bad_action_bad_type(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionBadLen& e) {

		logging::error << "eBadActionBadLen " << *msg << std::endl;
		rofbase->send_error_bad_action_bad_len(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionBadExperimenter& e) {

		logging::error << "eBadActionBadExperimenter " << *msg << std::endl;
		rofbase->send_error_bad_action_bad_experimenter(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionBadExperimenterType& e) {

		logging::error << "eBadActionBadExperimenterType " << *msg << std::endl;
		rofbase->send_error_bad_action_bad_experimenter_type(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionBadOutPort& e) {

		logging::error << "eBadActionBadOutPort " << *msg << std::endl;
		rofbase->send_error_bad_action_bad_out_port(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionBadArgument& e) {

		logging::error << "eBadActionBadArgument " << *msg << std::endl;
		rofbase->send_error_bad_action_bad_argument(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionEperm& e) {

		logging::error << "eBadActionEperm " << *msg << std::endl;
		rofbase->send_error_bad_action_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionTooMany& e) {

		logging::error << "eBadActionTooMany " << *msg << std::endl;
		rofbase->send_error_bad_action_too_many(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionBadQueue& e) {

		logging::error << "eBadActionBadQueue " << *msg << std::endl;
		rofbase->send_error_bad_action_bad_queue(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionBadOutGroup& e) {

		logging::error << "eBadActionBadOutGroup " << *msg << std::endl;
		rofbase->send_error_bad_action_bad_out_group(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionMatchInconsistent& e) {

		logging::error << "eBadActionMatchInconsistent " << *msg << std::endl;
		rofbase->send_error_bad_action_match_inconsistent(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionUnsupportedOrder& e) {

		logging::error << "eBadActionUnsuportedOrder " << *msg << std::endl;
		rofbase->send_error_bad_action_unsupported_order(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionBadTag& e) {

		logging::error << "eBadActionBadTag " << *msg << std::endl;
		rofbase->send_error_bad_action_bad_tag(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionSetType& e) {

		logging::error << "eBadActionSetType " << *msg << std::endl;
		rofbase->send_error_bad_action_set_type(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionSetLen& e) {

		logging::error << "eBadActionSetLen " << *msg << std::endl;
		rofbase->send_error_bad_action_set_len(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionSetArgument& e) {

		logging::error << "eBadActionSetArgument " << *msg << std::endl;
		rofbase->send_error_bad_action_set_argument(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadActionBase& e) {

		logging::error << "eBadActionBase " << *msg << std::endl;
		delete msg;

	} catch (eBadInstUnknownInst& e) {

		logging::error << "eBadInstUnknownInst " << *msg << std::endl;
		rofbase->send_error_bad_inst_unknown_inst(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadInstUnsupInst& e) {

		logging::error << "eBadInstUnsupInst " << *msg << std::endl;
		rofbase->send_error_bad_inst_unsup_inst(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadInstBadTableId& e) {

		logging::error << "eBadInstBadTableId " << *msg << std::endl;
		rofbase->send_error_bad_inst_bad_table_id(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadInstUnsupMetadata& e) {

		logging::error << "eBadInstUnsupMetadata " << *msg << std::endl;
		rofbase->send_error_bad_inst_unsup_metadata(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadInstUnsupMetadataMask& e) {

		logging::error << "eBadInstUnsupMetadataMask " << *msg << std::endl;
		rofbase->send_error_bad_inst_unsup_metadata_mask(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadInstBadExperimenter& e) {

		logging::error << "eBadInstBadExperimenter " << *msg << std::endl;
		rofbase->send_error_bad_inst_bad_experimenter(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadInstBadExpType& e) {

		logging::error << "eBadInstBadExpType " << *msg << std::endl;
		rofbase->send_error_bad_inst_bad_exp_type(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadInstBadLen& e) {

		logging::error << "eBadInstBadLen " << *msg << std::endl;
		rofbase->send_error_bad_inst_bad_len(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadInstEPerm& e) {

		logging::error << "eBadInstEPerm " << *msg << std::endl;
		rofbase->send_error_bad_inst_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadInstBase& e) {

		logging::error << "eBadInstBase " << *msg << std::endl;
		delete msg;

	} catch (eBadMatchBadType& e) {

		logging::error << "eBadMatchBadType " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_type(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBadLen& e) {

		logging::error << "eBadMatchBadLen " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_len(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBadTag& e) {

		logging::error << "eBadMatchBadTag " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_tag(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBadDlAddrMask& e) {

		logging::error << "eBadMatchBadDlAddrMask " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_dladdr_mask(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBadNwAddrMask& e) {

		logging::error << "eBadMatchBadNwAddrMask " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_nwaddr_mask(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBadWildcards& e) {

		logging::error << "eBadMatchBadWildcards " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_wildcards(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBadField& e) {

		logging::error << "eBadMatchBadField " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_field(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBadValue& e) {

		logging::error << "eBadMatchBadValue " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_value(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBadMask& e) {

		logging::error << "eBadMatchBadMask " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_mask(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBadPrereq& e) {

		logging::error << "eBadMatchBadPrereq " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_prereq(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchDupField& e) {

		logging::error << "eBadMatchDupField " << *msg << std::endl;
		rofbase->send_error_bad_match_dup_field(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchEPerm& e) {

		logging::error << "eBadMatchEPerm " << *msg << std::endl;
		rofbase->send_error_bad_match_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eBadMatchBase& e) {

		logging::error << "eBadMatchBase " << *msg << std::endl;
		delete msg;
	}

#if 0
	switch (msg->get_type()) {
	case rofl::openflow::OFPT_FEATURES_REQUEST: {
		features_re
	} break;
	case OFPT_FEATURES_REPLY:
	case OFPT_GET_CONFIG_REQUEST:
	case OFPT_GET_CONFIG_REPLY:
	case OFPT_SET_CONFIG:
	case OFPT_PACKET_IN:
	case OFPT_FLOW_REMOVED:
	case OFPT_PORT_STATUS:
	case OFPT_PACKET_OUT:
	case OFPT_FLOW_MOD:
	case OFPT_GROUP_MOD:
	case OFPT_PORT_MOD:
	case OFPT_TABLE_MOD:
	case OFPT_MULTIPART_REQUEST:
	case OFPT_MULTIPART_REPLY:
	case OFPT_BARRIER_REQUEST:
	case OFPT_BARRIER_REPLY:
	case OFPT_QUEUE_GET_CONFIG_REQUEST:
	case OFPT_QUEUE_GET_CONFIG_REPLY:
	case OFPT_ROLE_REQUEST:
	case OFPT_ROLE_REPLY:
	case OFPT_GET_ASYNC_REQUEST:
	case OFPT_GET_ASYNC_REPLY:
	case OFPT_SET_ASYNC:
	case OFPT_METER_MOD:
	default: {

	};
	}
#endif
}



void
crofctlImpl::send_message(
		cofmsg *msg,
		uint8_t aux_id)
{
	if (not is_established()) {
		logging::error << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
    			<< "dropping message, control channel not available. " << std::endl << *msg;
        delete msg; return;
    }

	if ((rofchan.get_version() < rofl::openflow13::OFP_VERSION) && (aux_id > 0)) {
		aux_id = 0;
	}

    switch (msg->get_version()) {
    case openflow10::OFP_VERSION: {

        switch (msg->get_type()) {
        case openflow10::OFPT_ERROR: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Error message " << std::endl << *dynamic_cast<cofmsg_error*>(msg);
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
        case openflow12::OFPT_ERROR: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Error message " << std::endl << *dynamic_cast<cofmsg_error*>(msg);
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
        case openflow13::OFPT_ERROR: {
        	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << " sending Error message " << std::endl << *dynamic_cast<cofmsg_error*>(msg);
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
        case openflow13::OFPT_MULTIPART_REPLY: {
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

	send_message_via_ctl_channel(msg, aux_id);
}




void
crofctlImpl::features_request_rcvd(cofmsg_features_request *msg, uint8_t aux_id)
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
	switch (rofchan.get_version()) {
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
crofctlImpl::get_config_request_rcvd(cofmsg_get_config_request *msg, uint8_t aux_id)
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
crofctlImpl::set_config_rcvd(cofmsg_set_config *msg, uint8_t aux_id)
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

	} catch (eSwitchConfigEPerm& e) {

		logging::warn << "eSwitchConfigEPerm " << *msg << std::endl;
		rofbase->send_error_switch_config_failed_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eSwitchConfigBase& e) {

		logging::warn << "eSwitchConfigBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctlImpl::packet_out_rcvd(cofmsg_packet_out *msg, uint8_t aux_id)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Packet-Out message received" << std::endl << *msg << std::endl;

	check_role();

	rofbase->handle_packet_out(this, msg);
}



void
crofctlImpl::flow_mod_rcvd(cofmsg_flow_mod *msg, uint8_t aux_id)
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

	} catch (eFlowModBadFlags& e) {

		logging::warn << "eFlowModBadFlags " << *msg << std::endl;
		rofbase->send_error_flow_mod_failed_bad_flags(this, msg->get_xid(), msg->soframe(), msg->framelen());
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
		rofbase->send_error_bad_inst_bad_experimenter(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eOFmatchInvalBadValue& e) {

		logging::warn << "eOFmatchInvalBadValue " << *msg << std::endl;
		rofbase->send_error_bad_match_bad_value(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;
	}
}



void
crofctlImpl::group_mod_rcvd(cofmsg_group_mod *msg, uint8_t aux_id)
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
crofctlImpl::port_mod_rcvd(cofmsg_port_mod *msg, uint8_t aux_id)
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

	} catch (ePortModEPerm& e) {

		logging::warn << "ePortModEPerm " << *msg << std::endl;
		rofbase->send_error_port_mod_failed_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (ePortModBase& e) {

		logging::warn << "ePortModBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctlImpl::table_mod_rcvd(cofmsg_table_mod *msg, uint8_t aux_id)
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

	} catch (eTableModEPerm& e) {

		logging::warn << "eTableModEPerm " << *msg << std::endl;
		rofbase->send_error_table_mod_failed_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;

	} catch (eTableModBase& e) {

		logging::warn << "eTableModBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctlImpl::stats_request_rcvd(cofmsg_stats *msg, uint8_t aux_id)
{
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Stats-Request message received" << std::endl << *msg << std::endl;

	try {
		xidstore.xid_add(this, msg->get_xid());

	} catch (eXidStoreXidBusy& e) {
		logging::warn << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
				<< " retransmitted Stats-Request " << *msg << std::endl;
	}

	switch (msg->get_stats_type()) {
	case openflow13::OFPMP_DESC: {
		desc_stats_request_rcvd(dynamic_cast<cofmsg_desc_stats_request*>( msg ), aux_id);
	} break;
	case openflow13::OFPMP_FLOW: {
		flow_stats_request_rcvd(dynamic_cast<cofmsg_flow_stats_request*>( msg ), aux_id);
	} break;
	case openflow13::OFPMP_AGGREGATE: {
		aggregate_stats_request_rcvd(dynamic_cast<cofmsg_aggr_stats_request*>( msg ), aux_id);
	} break;
	case openflow13::OFPMP_TABLE: {
		table_stats_request_rcvd(dynamic_cast<cofmsg_table_stats_request*>( msg ), aux_id);
	} break;
	case openflow13::OFPMP_PORT_DESC: {
		port_stats_request_rcvd(dynamic_cast<cofmsg_port_stats_request*>( msg ), aux_id);
	} break;
	case openflow13::OFPMP_QUEUE: {
		queue_stats_request_rcvd(dynamic_cast<cofmsg_queue_stats_request*>( msg ), aux_id);
	} break;
	case openflow13::OFPMP_GROUP: {
		group_stats_request_rcvd(dynamic_cast<cofmsg_group_stats_request*>( msg ), aux_id);
	} break;
	case openflow13::OFPMP_GROUP_DESC: {
		group_desc_stats_request_rcvd(dynamic_cast<cofmsg_group_desc_stats_request*>( msg ), aux_id);
	} break;
	case openflow13::OFPMP_GROUP_FEATURES: {
		group_features_stats_request_rcvd(dynamic_cast<cofmsg_group_features_stats_request*>( msg ), aux_id);
	} break;
	// TODO: add remaining OF 1.3 statistics messages
	// TODO: experimenter statistics
	default: {
		rofbase->handle_stats_request(this, msg);
	} break;
	}
}



void
crofctlImpl::desc_stats_request_rcvd(cofmsg_desc_stats_request *msg, uint8_t aux_id)
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
crofctlImpl::table_stats_request_rcvd(cofmsg_table_stats_request* msg, uint8_t aux_id)
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
crofctlImpl::port_stats_request_rcvd(cofmsg_port_stats_request* msg, uint8_t aux_id)
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
crofctlImpl::flow_stats_request_rcvd(cofmsg_flow_stats_request* msg, uint8_t aux_id)
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
crofctlImpl::aggregate_stats_request_rcvd(cofmsg_aggr_stats_request* msg, uint8_t aux_id)
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
crofctlImpl::queue_stats_request_rcvd(cofmsg_queue_stats_request* msg, uint8_t aux_id)
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
crofctlImpl::group_stats_request_rcvd(cofmsg_group_stats_request* msg, uint8_t aux_id)
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
crofctlImpl::group_desc_stats_request_rcvd(cofmsg_group_desc_stats_request* msg, uint8_t aux_id)
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
crofctlImpl::group_features_stats_request_rcvd(cofmsg_group_features_stats_request* msg, uint8_t aux_id)
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
crofctlImpl::experimenter_stats_request_rcvd(cofmsg_experimenter_stats_request* msg, uint8_t aux_id)
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
crofctlImpl::role_request_rcvd(cofmsg_role_request *msg, uint8_t aux_id)
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
crofctlImpl::barrier_request_rcvd(cofmsg_barrier_request *msg, uint8_t aux_id)
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
crofctlImpl::queue_get_config_request_rcvd(cofmsg_queue_get_config_request *msg, uint8_t aux_id)
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
crofctlImpl::experimenter_rcvd(cofmsg_experimenter *msg, uint8_t aux_id)
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
crofctlImpl::get_async_config_request_rcvd(cofmsg_get_async_config_request *msg, uint8_t aux_id)
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
crofctlImpl::set_async_config_rcvd(cofmsg_set_async_config *msg, uint8_t aux_id)
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
crofctlImpl::send_message_via_ctl_channel(
		cofmsg *msg, uint8_t aux_id)
{
	if (not is_established()) {
		logging::error << "[rofl][ctl] no control connection available, dropping message." << std::endl << *msg;
		delete msg; return;
	}

	rofchan.send_message(msg, aux_id);
}



uint32_t
crofctlImpl::get_async_xid(rofl::openflow::crofchan *chan)
{
	return 0; // TODO
}

uint32_t
crofctlImpl::get_sync_xid(rofl::openflow::crofchan *chan)
{
	return 0; // TODO
}

void
crofctlImpl::release_sync_xid(rofl::openflow::crofchan *chan, uint32_t xid)
{

}


