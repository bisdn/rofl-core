/*
 * crofctl.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "crofctl.h"
#include "crofbase.h"

using namespace rofl;

/*static*/std::set<crofctl_env*> crofctl_env::rofctl_envs;
/*static*/std::map<cctlid, crofctl*> crofctl::rofctls;

/*static*/crofctl&
crofctl::get_ctl(
		const cctlid& ctlid)
{
	if (crofctl::rofctls.find(ctlid) == crofctl::rofctls.end()) {
		throw eRofCtlNotFound();
	}
	return *(crofctl::rofctls[ctlid]);
}



void
crofctl::handle_timeout(int opaque, void *data)
{
	switch (opaque) {
	case TIMER_RUN_ENGINE: {
		work_on_eventqueue();
	} break;
	default: {
		rofl::logging::error << "[rofl-common][crofctl] "
				<< "ctlid: " << ctlid.str() << " unknown timer event:"
				<< opaque << std::endl;
	};
	}
}



void
crofctl::work_on_eventqueue()
{
	flags.set(FLAG_ENGINE_IS_RUNNING);
	while (not events.empty()) {
		enum crofctl_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_CHAN_TERMINATED: {
			flags.reset(FLAG_ENGINE_IS_RUNNING);
			event_chan_terminated();
		} return;
		case EVENT_CONN_TERMINATED: {
			event_conn_terminated();
		} break;
		case EVENT_CONN_REFUSED: {
			event_conn_refused();
		} break;
		case EVENT_CONN_FAILED: {
			event_conn_failed();
		} break;
		default: {
			rofl::logging::error << "[rofl-common][crofctl] unknown event seen, internal error" << std::endl << *this;
		};
		}
	}
	flags.reset(FLAG_ENGINE_IS_RUNNING);
}



void
crofctl::event_chan_terminated()
{
	state = STATE_DISCONNECTED;
	call_env().handle_chan_terminated(*this);
}



void
crofctl::event_conn_terminated()
{
	rofl::RwLock rwlock(conns_terminated_rwlock, rofl::RwLock::RWLOCK_WRITE);
	for (std::list<rofl::cauxid>::iterator
			it = conns_terminated.begin(); it != conns_terminated.end(); ++it) {
		call_env().handle_conn_terminated(*this, *it);
	}
	conns_terminated.clear();
}



void
crofctl::event_conn_refused()
{
	rofl::RwLock rwlock(conns_refused_rwlock, rofl::RwLock::RWLOCK_WRITE);
	for (std::list<rofl::cauxid>::iterator
			it = conns_refused.begin(); it != conns_refused.end(); ++it) {
		call_env().handle_conn_refused(*this, *it);
	}
	conns_refused.clear();
}



void
crofctl::event_conn_failed()
{
	rofl::RwLock rwlock(conns_failed_rwlock, rofl::RwLock::RWLOCK_WRITE);
	for (std::list<rofl::cauxid>::iterator
			it = conns_failed.begin(); it != conns_failed.end(); ++it) {
		call_env().handle_conn_failed(*this, *it);
	}
	conns_failed.clear();
}



bool
crofctl::is_slave() const
{
#if 0
	switch (rofchan.get_version()) {
	case openflow12::OFP_VERSION: return (openflow12::OFPCR_ROLE_SLAVE == role);
	case openflow13::OFP_VERSION: return (openflow13::OFPCR_ROLE_SLAVE == role);
	default: return false;
	}
#endif
	return false;
}






void
crofctl::recv_message(
		crofchan& chan,
		const cauxid& auxid,
		rofl::openflow::cofmsg *msg)
{
	try {
		switch (msg->get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_VENDOR: {
				experimenter_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_experimenter*>( msg ));
			} break;
			case rofl::openflow10::OFPT_ERROR: {
				error_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_error*>( msg ));
			} break;
			case rofl::openflow10::OFPT_FEATURES_REQUEST: {
				features_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_features_request*>( msg ));
			} break;
			case rofl::openflow10::OFPT_GET_CONFIG_REQUEST: {
				get_config_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_get_config_request*>( msg ));
			} break;
			case rofl::openflow10::OFPT_SET_CONFIG: {
				set_config_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_set_config*>( msg ));
			} break;
			case rofl::openflow10::OFPT_PACKET_OUT: {
				packet_out_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_packet_out*>( msg ));
			} break;
			case rofl::openflow10::OFPT_FLOW_MOD: {
				flow_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_flow_mod*>( msg ));
			} break;
			case rofl::openflow10::OFPT_PORT_MOD: {
				port_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_port_mod*>( msg ));
			} break;
			case rofl::openflow10::OFPT_STATS_REQUEST: {
				stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg ));
			} break;
			case rofl::openflow10::OFPT_BARRIER_REQUEST: {
				barrier_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_barrier_request*>( msg ));
			} break;
			case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST: {
				queue_get_config_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request*>( msg ));
			} break;
			default: {

			};
			}
		} break;
		case rofl::openflow12::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_EXPERIMENTER: {
				experimenter_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_experimenter*>( msg ));
			} break;
			case rofl::openflow12::OFPT_ERROR: {
				error_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_error*>( msg ));
			} break;
			case rofl::openflow12::OFPT_FEATURES_REQUEST: {
				features_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_features_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_GET_CONFIG_REQUEST: {
				get_config_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_get_config_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_SET_CONFIG: {
				set_config_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_set_config*>( msg ));
			} break;
			case rofl::openflow12::OFPT_PACKET_OUT: {
				packet_out_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_packet_out*>( msg ));
			} break;
			case rofl::openflow12::OFPT_FLOW_MOD: {
				flow_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_flow_mod*>( msg ));
			} break;
			case rofl::openflow12::OFPT_GROUP_MOD: {
				group_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_group_mod*>( msg ));
			} break;
			case rofl::openflow12::OFPT_PORT_MOD: {
				port_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_port_mod*>( msg ));
			} break;
			case rofl::openflow12::OFPT_TABLE_MOD: {
				table_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_table_mod*>( msg ));
			} break;
			case rofl::openflow12::OFPT_STATS_REQUEST: {
				stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_BARRIER_REQUEST: {
				barrier_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_barrier_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST: {
				queue_get_config_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request*>( msg ));
			} break;
			case rofl::openflow12::OFPT_ROLE_REQUEST: {
				role_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_role_request*>( msg ));
			} break;
			default: {

			};
			}
		} break;
		case openflow13::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_EXPERIMENTER: {
				experimenter_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_experimenter*>( msg ));
			} break;
			case rofl::openflow13::OFPT_ERROR: {
				error_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_error*>( msg ));
			} break;
			case rofl::openflow13::OFPT_FEATURES_REQUEST: {
				features_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_features_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_GET_CONFIG_REQUEST: {
				get_config_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_get_config_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_SET_CONFIG: {
				set_config_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_set_config*>( msg ));
			} break;
			case rofl::openflow13::OFPT_PACKET_OUT: {
				packet_out_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_packet_out*>( msg ));
			} break;
			case rofl::openflow13::OFPT_FLOW_MOD: {
				flow_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_flow_mod*>( msg ));
			} break;
			case rofl::openflow13::OFPT_GROUP_MOD: {
				group_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_group_mod*>( msg ));
			} break;
			case rofl::openflow13::OFPT_PORT_MOD: {
				port_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_port_mod*>( msg ));
			} break;
			case rofl::openflow13::OFPT_TABLE_MOD: {
				table_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_table_mod*>( msg ));
			} break;
			case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
				stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_BARRIER_REQUEST: {
				barrier_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_barrier_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST: {
				queue_get_config_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_ROLE_REQUEST: {
				role_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_role_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_GET_ASYNC_REQUEST: {
				get_async_config_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_get_async_config_request*>( msg ));
			} break;
			case rofl::openflow13::OFPT_SET_ASYNC: {
				set_async_config_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_set_async_config*>( msg ));
			} break;
			case rofl::openflow13::OFPT_METER_MOD: {
				meter_mod_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_meter_mod*>( msg ));
			} break;
			default: {

			};
			}
		} break;
		default: {

		};
		}



	} catch (eBadSyntaxTooShort& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadSyntaxTooShort " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_len(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadVersion& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadVersion " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_version(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBadVersion& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBadVersion " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_version(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBadType& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBadType " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_type(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBadStat& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBadStat " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_stat(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBadExperimenter& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBadExperimenter " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_experimenter(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBadExpType& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBadExpType " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_exp_type(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestEperm& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestEperm " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_eperm(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBadLen& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBadLen " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_len(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBufferEmpty& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBufferEmpty " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_buffer_empty(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBufferUnknown& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBufferUnknown " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_buffer_unknown(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBadTableId& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBadTableId " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_table_id(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestIsSlave& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestIsSlave " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_is_slave(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBadPort& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBadPort " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_port(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBadPacket& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBadPacket " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_bad_packet(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestMultipartBufferOverflow& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestMultipartBufferOverflow " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_request_multipart_buffer_overflow(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadRequestBase& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadRequestBase " << *msg << std::endl;
		delete msg;

	} catch (eBadActionBadType& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBadType " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_type(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadLen& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBadLen " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_len(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadExperimenter& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBadExperimenter " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_experimenter(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadExperimenterType& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBadExperimenterType " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_experimenter_type(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadOutPort& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBadOutPort " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_out_port(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadArgument& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBadArgument " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_argument(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionEperm& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionEperm " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_eperm(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionTooMany& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionTooMany " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_too_many(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadQueue& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBadQueue " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_queue(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadOutGroup& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBadOutGroup " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_out_group(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionMatchInconsistent& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionMatchInconsistent " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_match_inconsistent(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));

		delete msg;

	} catch (eBadActionUnsupportedOrder& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionUnsuportedOrder " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_unsupported_order(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadTag& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBadTag " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_tag(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadSetType& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionSetType " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_set_type(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadSetLen& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionSetLen " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_set_len(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBadSetArgument& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionSetArgument " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_action_bad_set_argument(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadActionBase& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadActionBase " << *msg << std::endl;
		delete msg;

	} catch (eBadInstUnknownInst& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstUnknownInst " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_unknown_inst(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadInstUnsupInst& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstUnsupInst " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_unsup_inst(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadInstBadTableId& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstBadTableId " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_bad_table_id(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadInstUnsupMetadata& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstUnsupMetadata " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_unsup_metadata(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadInstUnsupMetadataMask& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstUnsupMetadataMask " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_unsup_metadata_mask(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadInstBadExperimenter& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstBadExperimenter " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_bad_experimenter(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadInstBadExpType& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstBadExpType " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_bad_exp_type(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadInstBadLen& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstBadLen " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_bad_len(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadInstEperm& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstEPerm " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_eperm(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadInstBase& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadInstBase " << *msg << std::endl;
		delete msg;

	} catch (eBadMatchBadType& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadType " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_type(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchBadLen& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadLen " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_len(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchBadTag& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadTag " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_tag(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchBadDlAddrMask& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadDlAddrMask " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_dladdr_mask(

				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchBadNwAddrMask& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadNwAddrMask " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_nwaddr_mask(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchBadWildcards& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadWildcards " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_wildcards(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchBadField& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadField " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_field(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchBadValue& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadValue " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_value(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchBadMask& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadMask " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_mask(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));

		delete msg;

	} catch (eBadMatchBadPrereq& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBadPrereq " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_prereq(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchDupField& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchDupField " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_dup_field(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchEperm& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchEPerm " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_eperm(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eBadMatchBase& e) {

		rofl::logging::error << "[rofl-common][crofctl] eBadMatchBase " << *msg << std::endl;
		delete msg;

	} catch (eMeterModUnknown& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModUnknown " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_unknown(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModMeterExists& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModMeterExists " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_meter_exists(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModInvalidMeter& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModInvalidMeter " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_invalid_meter(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModUnknownMeter& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModUnknownMeter " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_unknown_meter(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadCommand& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModBadCommand " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_command(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadFlags& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModBadFlags " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_flags(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadRate& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModBadRate " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_rate(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadBurst& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModBadBurst " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_burst(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadBand& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModBadBand " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_band(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadBandValue& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModBadBandValue " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_band_value(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModOutOfMeters& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModOutOfMeters " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_out_of_meters(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModOutOfBands& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModOutOfBands " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_out_of_bands(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBase& e) {

		rofl::logging::error << "[rofl-common][crofctl] eMeterModBase " << *msg << std::endl;
		delete msg;

	} catch (RoflException& e) {

		rofl::logging::error << "[rofl-common][crofctl] RoflException " << *msg << std::endl;
		delete msg;
	}
}



void
crofctl::send_features_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint64_t dpid,
		uint32_t n_buffers,
		uint8_t n_tables,
		uint32_t capabilities,
		uint8_t of13_auxiliary_id,
		uint32_t of10_actions_bitmap,
		const rofl::openflow::cofports& ports)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Features-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_features_reply *msg =
				new rofl::openflow::cofmsg_features_reply(
						rofchan.get_version(),
						xid,
						dpid,
						n_buffers,
						n_tables,
						capabilities,
						of10_actions_bitmap,
						of13_auxiliary_id,
						ports);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Features-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_get_config_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint16_t flags,
		uint16_t miss_send_len)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Get-Config-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_get_config_reply *msg =
				new rofl::openflow::cofmsg_get_config_reply(
						rofchan.get_version(),
						xid,
						flags,
						miss_send_len);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Get-Config-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint16_t stats_type,
		uint16_t stats_flags,
		uint8_t *body, size_t bodylen)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_stats *msg =
				new rofl::openflow::cofmsg_stats(
						rofchan.get_version(),
						xid,
						stats_type,
						stats_flags,
						body,
						bodylen);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_desc_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofdesc_stats_reply& desc_stats,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Desc-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_desc_stats_reply *msg =
				new rofl::openflow::cofmsg_desc_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						desc_stats);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Desc-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_flow_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofflowstatsarray& flowstatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Flow-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_flow_stats_reply *msg =
				new rofl::openflow::cofmsg_flow_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						flowstatsarray);


		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Flow-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_aggr_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofaggr_stats_reply& aggr_stats,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Aggr-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_aggr_stats_reply *msg =
				new rofl::openflow::cofmsg_aggr_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						aggr_stats);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Aggregate-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_table_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::coftablestatsarray& tablestatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Table-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_table_stats_reply *msg =
				new rofl::openflow::cofmsg_table_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						tablestatsarray);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Table-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_port_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofportstatsarray& portstatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Port-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_port_stats_reply *msg =
				new rofl::openflow::cofmsg_port_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						portstatsarray);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Port-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_queue_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofqueuestatsarray& queuestatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Queue-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_queue_stats_reply *msg =
				new rofl::openflow::cofmsg_queue_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						queuestatsarray);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Queue-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_group_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofgroupstatsarray& groupstatsarray,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Group-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_group_stats_reply *msg =
				new rofl::openflow::cofmsg_group_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						groupstatsarray);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Group-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_group_desc_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofgroupdescstatsarray& groupdescs,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Group-Desc-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_group_desc_stats_reply *msg =
				new rofl::openflow::cofmsg_group_desc_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						groupdescs);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Group-Desc-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_group_features_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofgroup_features_stats_reply& group_features_stats,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Group-Features-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_group_features_stats_reply *msg =
				new rofl::openflow::cofmsg_group_features_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						group_features_stats);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Group-Features-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_table_features_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::coftables& tables,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Table-Features-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_table_features_stats_reply *msg =
				new rofl::openflow::cofmsg_table_features_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						tables);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Table-Features-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_port_desc_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofports& ports,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Port-Desc-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_port_desc_stats_reply *msg =
				new rofl::openflow::cofmsg_port_desc_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						ports);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Port-Desc-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_experimenter_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint32_t exp_id,
		uint32_t exp_type,
		const cmemory& body,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Experimenter-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_experimenter_stats_reply *msg =
				new rofl::openflow::cofmsg_experimenter_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						exp_id,
						exp_type,
						body);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Experimenter-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_meter_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofmeterstatsarray& meter_stats_array,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Meter-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_meter_stats_reply *msg =
				new rofl::openflow::cofmsg_meter_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						meter_stats_array);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Meter-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_meter_config_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofmeterconfigarray& meter_config_array,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Meter-Config-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_meter_config_stats_reply *msg =
				new rofl::openflow::cofmsg_meter_config_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						meter_config_array);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Meter-Config-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_meter_features_stats_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofmeter_features& meter_features,
		uint16_t stats_flags)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Meter-Features-Stats-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_meter_features_stats_reply *msg =
				new rofl::openflow::cofmsg_meter_features_stats_reply(
						rofchan.get_version(),
						xid,
						stats_flags,
						meter_features);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Meter-Features-Stats-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_packet_in_message(
		const cauxid& auxid,
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t reason,
		uint8_t table_id,
		uint64_t cookie,
		uint16_t in_port, // for OF 1.0
		const rofl::openflow::cofmatch& match,
		uint8_t* data,
		size_t datalen)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Packet-In message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_packet_in *msg =
				new rofl::openflow::cofmsg_packet_in(
						rofchan.get_version(),
						transactions.get_async_xid(),
						buffer_id,
						total_len,
						reason,
						table_id,
						cookie,
						in_port, /* in_port for OF1.0 */
						match,
						data,
						datalen);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Packet-In message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_barrier_reply(
		const cauxid& auxid,
		uint32_t xid)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Barrier-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_barrier_reply *msg =
				new rofl::openflow::cofmsg_barrier_reply(
						rofchan.get_version(),
						xid);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Barrier-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_role_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofrole& role)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Role-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_role_reply *msg =
				new rofl::openflow::cofmsg_role_reply(
						rofchan.get_version(),
						xid,
						role);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Role-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_error_message(
		const cauxid& auxid,
		uint32_t xid,
		uint16_t type,
		uint16_t code,
		uint8_t* data,
		size_t datalen)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Error message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_error *msg =
				new rofl::openflow::cofmsg_error(
						rofchan.get_version(),
						xid,
						type,
						code,
						data, datalen);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Error message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_experimenter_message(
		const cauxid& auxid,
		uint32_t xid,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Experimenter message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_experimenter *msg =
				new rofl::openflow::cofmsg_experimenter(
							rofchan.get_version(),
							xid,
							experimenter_id,
							exp_type,
							body,
							bodylen);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Experimenter message" << std::endl;

	}

	throw eRofBaseCongested();
}




void
crofctl::send_flow_removed_message(
		const cauxid& auxid,
		const rofl::openflow::cofmatch& match,
		uint64_t cookie,
		uint16_t priority,
		uint8_t reason,
		uint8_t table_id,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint64_t packet_count,
		uint64_t byte_count)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Flow-Removed message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_flow_removed *msg =
				new rofl::openflow::cofmsg_flow_removed(
						rofchan.get_version(),
						transactions.get_async_xid(),
						cookie,
						priority,
						reason,
						table_id,
						duration_sec,
						duration_nsec,
						idle_timeout,
						hard_timeout,
						packet_count,
						byte_count,
						match);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Flow-Removed message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_port_status_message(
		const cauxid& auxid,
		uint8_t reason,
		const rofl::openflow::cofport& port)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Port-Status message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_port_status *msg =
				new rofl::openflow::cofmsg_port_status(
							rofchan.get_version(),
							transactions.get_async_xid(),
							reason,
							port);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Port-Status message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_queue_get_config_reply(
		const cauxid& auxid,
		uint32_t xid,
		uint32_t portno,
		const rofl::openflow::cofpacket_queues& queues)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Queue-Get-Config-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_queue_get_config_reply *msg =
				new rofl::openflow::cofmsg_queue_get_config_reply(
						rofchan.get_version(),
						xid,
						portno,
						queues);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Queue-Get-Config-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}



void
crofctl::send_get_async_config_reply(
		const cauxid& auxid,
		uint32_t xid,
		const rofl::openflow::cofasync_config& async_config)
{
	try {
		if (not is_established()) {
			rofl::logging::warn << "[rofl-common][crofctl] not connected, dropping Get-Async-Config-Reply message" << std::endl;
			return;
		}

		rofl::openflow::cofmsg_get_async_config_reply *msg =
				new rofl::openflow::cofmsg_get_async_config_reply(
						rofchan.get_version(),
						xid,
						async_config);

		rofchan.send_message(auxid, msg);

		return;

	} catch (eRofSockTxAgain& e) {
		rofl::logging::warn << "[rofl-common][crofctl] control channel congested, dropping Get-Async-Config-Reply message" << std::endl;

	}

	throw eRofBaseCongested();
}






void
crofctl::check_role()
{
	switch (rofchan.get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		if (rofl::openflow13::OFPCR_ROLE_SLAVE == role.get_role())
			throw eBadRequestIsSlave();
	} break;
	}
}




void
crofctl::features_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_features_request *msg)
{
	rofl::openflow::cofmsg_features_request& request = dynamic_cast<rofl::openflow::cofmsg_features_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Features-Request message received" << std::endl << request;

	call_env().handle_features_request(*this, auxid, request);

	delete msg;
}



void
crofctl::get_config_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_get_config_request *msg)
{
	rofl::openflow::cofmsg_get_config_request& request = dynamic_cast<rofl::openflow::cofmsg_get_config_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Get-Config-Request message received" << std::endl << request;

	check_role();

	call_env().handle_get_config_request(*this, auxid, request);

	delete msg;
}



void
crofctl::set_config_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_set_config *msg)
{
	rofl::openflow::cofmsg_set_config& message = dynamic_cast<rofl::openflow::cofmsg_set_config&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Set-Config message received" << std::endl << message;

	try {
		check_role();

		call_env().handle_set_config(*this, auxid, message);

		delete msg;

	} catch (eSwitchConfigBadFlags& e) {

		rofl::logging::warn << "eSwitchConfigBadFlags " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_switch_config_failed_bad_flags(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eSwitchConfigBadLen& e) {

		rofl::logging::warn << "eSwitchConfigBadLen " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_switch_config_failed_bad_len(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eSwitchConfigEPerm& e) {

		rofl::logging::warn << "eSwitchConfigEPerm " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_switch_config_failed_eperm(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eSwitchConfigBase& e) {

		rofl::logging::warn << "eSwitchConfigBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctl::packet_out_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_packet_out *msg)
{
	rofl::openflow::cofmsg_packet_out& message = dynamic_cast<rofl::openflow::cofmsg_packet_out&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Packet-Out message received" << std::endl << message;

	check_role();

	call_env().handle_packet_out(*this, auxid, message);

	delete msg;
}



void
crofctl::flow_mod_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_flow_mod *msg)
{
	rofl::openflow::cofmsg_flow_mod& message = dynamic_cast<rofl::openflow::cofmsg_flow_mod&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Flow-Mod message received" << std::endl << message;

	message.check_prerequisites();

	try {
		check_role();

		call_env().handle_flow_mod(*this, auxid, message);

		delete msg;

	} catch (eFlowModUnknown& e) {

		rofl::logging::warn << "eFlowModUnknown " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_unknown(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eFlowModTableFull& e) {

		rofl::logging::warn << "eFlowModTableFull " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_table_full(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eFlowModBadTableId& e) {

		rofl::logging::warn << "eFlowModBadTableId " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_table_id(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eFlowModOverlap& e) {

		rofl::logging::warn << "eFlowModOverlap " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_overlap(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eFlowModEperm& e) {

		rofl::logging::warn << "eFlowModEperm " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_eperm(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eFlowModBadTimeout& e) {

		rofl::logging::warn << "eFlowModBadTimeout " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_timeout(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eFlowModBadCommand& e) {

		rofl::logging::warn << "eFlowModBadCommand " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_command(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eFlowModBadFlags& e) {

		rofl::logging::warn << "eFlowModBadFlags " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_flags(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eFlowModBase& e) {

		rofl::logging::warn << "eFlowModBase " << *msg << std::endl;
		delete msg;

	} catch (eRofBaseTableNotFound& e) {

		rofl::logging::warn << "eRofBaseTableNotFound " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_flow_mod_failed_bad_table_id(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (rofl::openflow::eInstructionInvalType& e) {

		rofl::logging::warn << "eInstructionInvalType " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_unknown_inst(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eRofBaseGotoTableNotFound& e) {

		rofl::logging::warn << "eRofBaseGotoTableNotFound " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_bad_table_id(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (rofl::openflow::eInstructionBadExperimenter& e) {

		rofl::logging::warn << "eInstructionBadExperimenter " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_inst_bad_experimenter(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (rofl::openflow::eOFmatchInvalBadValue& e) {

		rofl::logging::warn << "eOFmatchInvalBadValue " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_bad_match_bad_value(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;
	}
}



void
crofctl::group_mod_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_group_mod *msg)
{
	rofl::openflow::cofmsg_group_mod& message = dynamic_cast<rofl::openflow::cofmsg_group_mod&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Group-Mod message received" << std::endl << message;

	message.check_prerequisites();

	try {
		check_role();

		call_env().handle_group_mod(*this, auxid, message);

		delete msg;

	} catch (eGroupModExists& e) {

		rofl::logging::warn << "eGroupModExists " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_group_exists(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModInvalGroup& e) {

		rofl::logging::warn << "eGroupModInvalGroup " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_inval_group(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModWeightUnsupported& e) {

		rofl::logging::warn << "eGroupModWeightUnsupported " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_weight_unsupported(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModOutOfGroups& e) {

		rofl::logging::warn << "eGroupModOutOfGroups " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_out_of_groups(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModOutOfBuckets& e) {

		rofl::logging::warn << "eGroupModOutOfBuckets " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_out_of_buckets(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModChainingUnsupported& e) {

		rofl::logging::warn << "eGroupModChainingUnsupported " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_chaining_unsupported(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModWatchUnsupported& e) {

		rofl::logging::warn << "eGroupModWatchUnsupported " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_watch_unsupported(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModLoop& e) {

		rofl::logging::warn << "eGroupModLoop " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_loop(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModUnknownGroup& e) {

		rofl::logging::warn << "eGroupModUnknownGroup " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_unknown_group(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModChainedGroup& e) {

		rofl::logging::warn << "eGroupModChainedGroup " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_chained_group(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModBadType& e) {

		rofl::logging::warn << "eGroupModBadType " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_bad_type(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModBadCommand& e) {

		rofl::logging::warn << "eGroupModBadCommand " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_bad_command(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModBadBucket& e) {

		rofl::logging::warn << "eGroupModBadBucket " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_bad_bucket(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModBadWatch& e) {

		rofl::logging::warn << "eGroupModBadWatch " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_bad_watch(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModEperm& e) {

		rofl::logging::warn << "eGroupModEperm " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_group_mod_failed_eperm(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eGroupModBase& e) {

		rofl::logging::warn << "eGroupModBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctl::port_mod_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_port_mod *msg)
{
	rofl::openflow::cofmsg_port_mod& message = dynamic_cast<rofl::openflow::cofmsg_port_mod&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Port-Mod message received" << std::endl << message;

	try {
		check_role();

		call_env().handle_port_mod(*this, auxid, message);

		delete msg;

	} catch (ePortModBadPort& e) {

		rofl::logging::warn << "ePortModBadPort " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_bad_port(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (ePortModBadHwAddr& e) {

		rofl::logging::warn << "ePortModBadHwAddr " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_bad_hwaddr(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (ePortModBadConfig& e) {

		rofl::logging::warn << "ePortModBadConfig " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_bad_config(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (ePortModBadAdvertise& e) {

		rofl::logging::warn << "ePortModBadAdvertise " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_bad_advertise(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (ePortModEperm& e) {

		rofl::logging::warn << "ePortModEPerm " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_port_mod_failed_eperm(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (ePortModBase& e) {

		rofl::logging::warn << "ePortModBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctl::table_mod_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_table_mod *msg)
{
	rofl::openflow::cofmsg_table_mod& message = dynamic_cast<rofl::openflow::cofmsg_table_mod&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Table-Mod message received" << std::endl << message;

	try {
		check_role();

		call_env().handle_table_mod(*this, auxid, message);

		delete msg;

	} catch (eTableModBadTable& e) {

		rofl::logging::warn << "eTableModBadTable " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_table_mod_failed_bad_table(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eTableModBadConfig& e) {

		rofl::logging::warn << "eTableModBadConfig " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_table_mod_failed_bad_config(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eTableModEperm& e) {

		rofl::logging::warn << "eTableModEPerm " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_table_mod_failed_eperm(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eTableModBase& e) {

		rofl::logging::warn << "eTableModBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctl::meter_mod_rcvd(
		const cauxid& auxid,
		rofl::openflow::cofmsg_meter_mod *msg)
{
	rofl::openflow::cofmsg_meter_mod& message = dynamic_cast<rofl::openflow::cofmsg_meter_mod&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Meter-Mod message received" << std::endl << message;

	try {
		check_role();

		call_env().handle_meter_mod(*this, auxid, message);

		delete msg;

	} catch (eMeterModUnknown& e) {

		rofl::logging::warn << "eMeterModUnknown " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_unknown(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModMeterExists& e) {

		rofl::logging::warn << "eMeterModMeterExists " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_meter_exists(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModInvalidMeter& e) {

		rofl::logging::warn << "eMeterModInvalidMeter " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_invalid_meter(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModUnknownMeter& e) {

		rofl::logging::warn << "eMeterModUnknownMeter " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_unknown_meter(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadCommand& e) {

		rofl::logging::warn << "eMeterModBadCommand " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_command(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadFlags& e) {

		rofl::logging::warn << "eMeterModBadFlags " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_flags(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadRate& e) {

		rofl::logging::warn << "eMeterModBadRate " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_rate(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadBurst& e) {

		rofl::logging::warn << "eMeterModBadBurst " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_burst(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadBand& e) {

		rofl::logging::warn << "eMeterModBadBand " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_band(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBadBandValue& e) {

		rofl::logging::warn << "eMeterModBadBandValue " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_bad_band_value(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModOutOfMeters& e) {

		rofl::logging::warn << "eMeterModOutOfMeters " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_out_of_meters(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModOutOfBands& e) {

		rofl::logging::warn << "eMeterModOutOfBands " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_meter_mod_out_of_bands(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eMeterModBase& e) {

		rofl::logging::warn << "eMeterModBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctl::stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_stats *msg)
{
	rofl::openflow::cofmsg_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Stats-Request message received" << std::endl << request;

	switch (msg->get_stats_type()) {
	case openflow13::OFPMP_DESC: {
		desc_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_desc_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_FLOW: {
		flow_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_flow_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_AGGREGATE: {
		aggregate_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_TABLE: {
		table_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_table_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_PORT_STATS: {
		port_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_port_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_QUEUE: {
		queue_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_queue_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_GROUP: {
		group_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_group_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_GROUP_DESC: {
		group_desc_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_GROUP_FEATURES: {
		group_features_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_METER: {
		meter_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_meter_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_METER_CONFIG: {
		meter_config_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_METER_FEATURES: {
		meter_features_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_TABLE_FEATURES: {
		table_features_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request*>( msg ));
	} break;
	case openflow13::OFPMP_PORT_DESC: {
		port_desc_stats_request_rcvd(auxid, dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_request*>( msg ));
	} break;
	// TODO: add remaining OF 1.3 statistics messages
	// TODO: experimenter statistics
	default: {
		call_env().handle_stats_request(*this, auxid, request);
	} break;
	}
}



void
crofctl::desc_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_desc_stats_request *msg)
{
	rofl::openflow::cofmsg_desc_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_desc_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Desc-Stats-Request message received" << std::endl << request;

	call_env().handle_desc_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::table_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_table_stats_request* msg)
{
	rofl::openflow::cofmsg_table_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_table_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Table-Stats-Request message received" << std::endl << request;

	call_env().handle_table_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::port_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_port_stats_request* msg)
{
	rofl::openflow::cofmsg_port_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_port_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Port-Stats-Request message received" << std::endl << request;

	call_env().handle_port_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::flow_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_flow_stats_request* msg)
{
	rofl::openflow::cofmsg_flow_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_flow_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Flow-Stats-Request message received" << std::endl << request;

	call_env().handle_flow_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::aggregate_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_aggr_stats_request* msg)
{
	rofl::openflow::cofmsg_aggr_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Aggregate-Stats-Request message received" << std::endl << request;

	call_env().handle_aggregate_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::queue_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_queue_stats_request* msg)
{
	rofl::openflow::cofmsg_queue_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_queue_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Queue-Stats-Request message received" << std::endl << request;

	call_env().handle_queue_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::group_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_group_stats_request* msg)
{
	rofl::openflow::cofmsg_group_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_group_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Group-Stats-Request message received" << std::endl << request;

	call_env().handle_group_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::group_desc_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_group_desc_stats_request* msg)
{
	rofl::openflow::cofmsg_group_desc_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Group-Desc-Stats-Request message received" << std::endl << request;

	call_env().handle_group_desc_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::group_features_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_group_features_stats_request* msg)
{
	rofl::openflow::cofmsg_group_features_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_group_features_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Group-Features-Stats-Request message received" << std::endl << request;

	call_env().handle_group_features_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::meter_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_meter_stats_request* msg)
{
	rofl::openflow::cofmsg_meter_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_meter_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Meter-Stats-Request message received" << std::endl << request;

	call_env().handle_meter_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::meter_config_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_meter_config_stats_request* msg)
{
	rofl::openflow::cofmsg_meter_config_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Meter-Config-Stats-Request message received" << std::endl << request;

	call_env().handle_meter_config_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::meter_features_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_meter_features_stats_request* msg)
{
	rofl::openflow::cofmsg_meter_features_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_meter_features_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Meter-Features-Stats-Request message received" << std::endl << request;

	call_env().handle_meter_features_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::table_features_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_table_features_stats_request* msg)
{
	rofl::openflow::cofmsg_table_features_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Table-Features-Stats-Request message received" << std::endl << request;

	call_env().handle_table_features_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::port_desc_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_port_desc_stats_request* msg)
{
	rofl::openflow::cofmsg_port_desc_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Port-Desc-Stats-Request message received" << std::endl << request;

	call_env().handle_port_desc_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::experimenter_stats_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_experimenter_stats_request* msg)
{
	rofl::openflow::cofmsg_experimenter_stats_request& request = dynamic_cast<rofl::openflow::cofmsg_experimenter_stats_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Experimenter-Stats-Request message received" << std::endl << request;

	call_env().handle_experimenter_stats_request(*this, auxid, request);

	delete msg;
}



void
crofctl::role_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_role_request *msg)
{
	try {
		rofl::openflow::cofmsg_role_request& request = dynamic_cast<rofl::openflow::cofmsg_role_request&>( *msg );

		rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
				<< " Role-Request message received" << std::endl << request;

		switch (msg->get_role().get_role()) {
		// same for OF12 and OF13
		case rofl::openflow13::OFPCR_ROLE_EQUAL:
		case rofl::openflow13::OFPCR_ROLE_MASTER:
		case rofl::openflow13::OFPCR_ROLE_SLAVE:
		case rofl::openflow13::OFPCR_ROLE_NOCHANGE: {
			// continue with further checks
		} break;
		default:
			throw eRoleRequestBadRole();
		}

		call_env().role_request_rcvd(*this, msg->get_role().get_role(), msg->get_role().get_generation_id());

		// necessary for proxy implementations
		call_env().handle_role_request(*this, auxid, request);

		send_role_reply(auxid, msg->get_xid(), role);

		delete msg;

	} catch (eRoleRequestStale& e) {

		rofl::logging::warn << "eRoleRequestStale " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_role_request_failed_stale(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eRoleRequestUnsupported& e) {

		rofl::logging::warn << "eRoleRequestUnsupported " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_role_request_failed_unsupported(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eRoleRequestBadRole& e) {

		rofl::logging::warn << "eRoleRequestBadRole " << *msg << std::endl;
		rofchan.send_message(auxid, new rofl::openflow::cofmsg_error_role_request_failed_bad_role(
				rofchan.get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
		delete msg;

	} catch (eRoleRequestBase& e) {

		rofl::logging::warn << "eRoleRequestBase " << *msg << std::endl;
		delete msg;
	}
}



void
crofctl::barrier_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_barrier_request *msg)
{
	rofl::openflow::cofmsg_barrier_request& request = dynamic_cast<rofl::openflow::cofmsg_barrier_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Barrier-Request message received" << std::endl << request;

	call_env().handle_barrier_request(*this, auxid, request);

	delete msg;
}



void
crofctl::queue_get_config_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_queue_get_config_request *msg)
{
	rofl::openflow::cofmsg_queue_get_config_request& request = dynamic_cast<rofl::openflow::cofmsg_queue_get_config_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Queue-Get-Config-Request message received" << std::endl << request;

	call_env().handle_queue_get_config_request(*this, auxid, request);

	delete msg;
}



void
crofctl::experimenter_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_experimenter *msg)
{
	rofl::openflow::cofmsg_experimenter& message = dynamic_cast<rofl::openflow::cofmsg_experimenter&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Experimenter message received" << std::endl << message;

	switch (msg->get_experimenter_id()) {
	default: {
		call_env().handle_experimenter_message(*this, auxid, message);
	};
	}

	delete msg;
}



void
crofctl::error_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_error *msg)
{
	rofl::openflow::cofmsg_error& error = dynamic_cast<rofl::openflow::cofmsg_error&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Error message received" << std::endl << error;

	call_env().handle_error_message(*this, auxid, error);

	delete msg;
}



void
crofctl::get_async_config_request_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_get_async_config_request *msg)
{
	rofl::openflow::cofmsg_get_async_config_request& request = dynamic_cast<rofl::openflow::cofmsg_get_async_config_request&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Get-Async-Config-Request message received" << std::endl << request;

	send_get_async_config_reply(auxid, msg->get_xid(), async_config);

	delete msg;
}



void
crofctl::set_async_config_rcvd(const cauxid& auxid, rofl::openflow::cofmsg_set_async_config *msg)
{
	rofl::openflow::cofmsg_set_async_config& message = dynamic_cast<rofl::openflow::cofmsg_set_async_config&>( *msg );

	rofl::logging::debug2 << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
			<< " Set-Async-Config message received" << std::endl << message;

	async_config = msg->get_async_config();

	call_env().handle_set_async_config(*this, auxid, message);

	delete msg;
}






void
crofctl::init_async_config_role_default_template()
{
	async_config_role_default_template.set_packet_in_mask_master() =
			(1 << rofl::openflow13::OFPR_NO_MATCH) |
			(1 << rofl::openflow13::OFPR_ACTION);

	async_config_role_default_template.set_packet_in_mask_slave(0);

	async_config_role_default_template.set_port_status_mask_master() =
			(1 << rofl::openflow13::OFPPR_ADD) |
			(1 << rofl::openflow13::OFPPR_DELETE) |
			(1 << rofl::openflow13::OFPPR_MODIFY);

	async_config_role_default_template.set_port_status_mask_slave() =
			(1 << rofl::openflow13::OFPPR_ADD) |
			(1 << rofl::openflow13::OFPPR_DELETE) |
			(1 << rofl::openflow13::OFPPR_MODIFY);

	async_config_role_default_template.set_flow_removed_mask_master() =
			(1 << rofl::openflow13::OFPRR_IDLE_TIMEOUT) |
			(1 << rofl::openflow13::OFPRR_HARD_TIMEOUT) |
			(1 << rofl::openflow13::OFPRR_DELETE) |
			(1 << rofl::openflow13::OFPRR_GROUP_DELETE);

	async_config_role_default_template.set_flow_removed_mask_slave(0);
}


