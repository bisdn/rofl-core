#include <iostream>
#include <memory>
#include <unistd.h>

#include "tranny.h"

#include <rofl/platform/unix/cunixenv.h>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/cerror.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/utils/c_logger.h>

// OF10 not supported until I can either get an OF10 version of rofl::send_flow_mod_message, or access crofbase::xids_used
#define PROXYOFPVERSION OFP10_VERSION		// OFP10_VERSION or OFP12_VERSION

ctranslator::ctranslator():rofl::crofbase (1 << PROXYOFPVERSION),m_slave(0),m_master(0),m_of_version(PROXYOFPVERSION) {
	// read config file
	//..
}

ctranslator::~ctranslator() {
	// rpc_close_all();
	std::cout << std::endl << "ctranslator::~ctranslator() called." << std::endl;	// TODO: proper logging
}

void ctranslator::handle_error (rofl::cofdpt *dpt, rofl::cofmsg *msg) {
	std::cout << std::endl << "handle_error from " << dpt->c_str() << " : " << msg->c_str() << std::endl;
}

void ctranslator::handle_ctrl_open (rofl::cofctl *ctl) {
	// should be called automatically after call to rpc_connect_to_dpt in connect_to_slave
	std::cout << std::endl << "ctranslator::handle_ctrl_open called with " << (ctl?ctl->c_str():"NULL") << std::endl;
	m_master = ctl;	// TODO - what to do with previous m_master?
}

void ctranslator::handle_ctrl_close (rofl::cofctl *ctl) {
	std::cout << "ctranslator::handle_ctrl_close called with " << (ctl?ctl->c_str():"NULL") << std::endl;
	// this socket disconnecting could just be a temporary thing - mark it is dead, but expect a possible auto reconnect
	if(ctl!=m_master) std::cout << "ctranslator::handle_ctrl_close: was expecting " << (m_master?m_master->c_str():"NULL") << " but got " << (ctl?ctl->c_str():"NULL") << std::endl;
	m_master=0;	// TODO - m_naster ownership?
}

// This function feels alone, ignored, never called by anybody. Surely it should if it's 
// And I'm mad as hell and I'm not going to take it any more!
void ctranslator::handle_dpath_open (rofl::cofdpt *dpt) {
	// should be called automatically after call to rpc_connect_to_dpt in connect_to_slave
	std::cout << std::endl << "ctranslator::handle_dpath_open called with " << (dpt?dpt->c_str():"NULL") << std::endl;
	m_slave = dpt;	// TODO - what to do with previous m_slave?
	m_slave_dpid=dpt->get_dpid();	// TODO - check also get_config, get_capabilities etc
	m_dpid = m_slave_dpid + 1;
	rpc_connect_to_ctl(m_of_version,3,rofl::caddress(AF_INET, "127.0.0.1", 6633));
}

void ctranslator::handle_dpath_close (rofl::cofdpt *dpt) {
	std::cout << std::endl << "handle_dpath_close called with " << (dpt?dpt->c_str():"NULL") << std::endl;
//	assert(dpt==m_slave);
	if(dpt!=m_slave) std::cout << "ctranslator::handle_dpath_close: Was expecting " << (m_slave?m_slave->c_str():"NULL") << " but got " << (dpt?dpt->c_str():"NULL") << std::endl;
	// this socket disconnecting could just be a temporary thing - mark it is dead, but expect a possible auto reconnect
	m_slave=0;	// TODO - m_slave ownership?
}

void ctranslator::handle_flow_mod(rofl::cofctl *ctl, rofl::cofmsg_flow_mod *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << msg->c_str() << "." << std::endl;
///	rofl::cflowentry entry(m_of_version);	// this constructor may be broken - just throws rofl::eBadVersion whatever the version is.
///	std::cout << "TP" << __LINE__ << std::endl;
/*	entry.set_command(msg->get_command());
	entry.set_table_id(msg->get_table_id());
	entry.set_idle_timeout(msg->get_idle_timeout());
	entry.set_hard_timeout(msg->get_hard_timeout());
	entry.set_cookie(msg->get_cookie());
	entry.set_cookie_mask(msg->get_cookie_mask());
	entry.set_priority(msg->get_priority());
	entry.set_buffer_id(msg->get_buffer_id());
	entry.set_out_port(msg->get_out_port());
	entry.set_out_group(msg->get_out_group());
	entry.set_flags(msg->get_flags()); */
/// BOOST_PP_SEQ_FOR_EACH( CLONECMD, ((*msg))(entry), (command)(table_id)(idle_timeout)(hard_timeout)(cookie)(cookie_mask)(priority)(buffer_id)(out_port)(out_group)(flags) )
///	send_flow_mod_message( m_slave, entry );
	try {
		if(m_of_version==OFP10_VERSION) {
			rofl::cflowentry entry(OFP10_VERSION);
			BOOST_PP_SEQ_FOR_EACH( CLONECMD, ((*msg))(entry), (command)(table_id)(idle_timeout)(hard_timeout)(cookie)(cookie_mask)(priority)(buffer_id)(out_port)(out_group)(flags) )
/*			cofmsg_flow_mod	*pack = new cofmsg_flow_mod( OFP10_VERSION, ta_new_async_xid(), msg->get_cookie(), msg->get_cookie_mask(), \
				msg->get_table_id(), msg->get_command(), msg->get_idle_timeout(), msg->get_hard_timeout(), msg->get_priority(), \
				msg->get_buffer_id(), msg->get_out_port(), msg->get_out_group(), msg->get_flags(),
                                        inlist,
                                        ofmatch);
			pack->pack();
			m_slave->send_message(pack);*/
			entry.pack();
			send_flow_mod_message( m_slave, entry );
		} else {
			// TODO test this OF12 code
			assert(false);
			std::cout << func << ": About to create rofl::cofmatch." << std::endl;
			rofl::cofmatch match_(msg->get_match());
			std::cout << func << ": About to create rofl::cofinlist." << std::endl;
		/// get_instructions is not valid for < OF12
			rofl::cofinlist instructions_(msg->get_instructions());		// JSP errno 115 EINPROGRESS gets thrown here - the socket is blocked. Why, and which socket, I do not know.
			// TODO instructions must be fiddled to allow packet-ins to correctly traverse the proxy
			std::cout << func << ": About to send_flow_mod_message." << std::endl;
			send_flow_mod_message( m_slave, match_, msg->get_cookie(), msg->get_cookie_mask(), msg->get_table_id(), msg->get_command(), msg->get_idle_timeout(), \
				msg->get_hard_timeout(), msg->get_priority(), msg->get_buffer_id(), msg->get_out_port(), msg->get_out_group(), msg->get_flags(), instructions_ );
				std::cout << func << ": send_flow_mod_message called at " << m_slave->c_str() << "." << std::endl;
		}
	} catch (rofl::cerror &e) {
		std::cout << func << ": caught " << e << ". at " << __FILE__ << ":" << __LINE__ << std::endl;
	}
	// TODO what about get_xid() and get_actions() in cofmsg_flow_mod ? get_actions() may be  OFP10 only - at least according to cofmsg_flow_mod ctor
	// TODO perhaps use xid as key to map where a list of flow mods are stored? so we have a local record of flow mods
	
	delete(msg);
}

void ctranslator::handle_features_request(rofl::cofctl *ctl, rofl::cofmsg_features_request *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
	uint32_t masterxid = pack->get_xid();
	if(m_mxid_sxid.find(masterxid)!=m_mxid_sxid.end()) std::cout << "ERROR: ctranslator::handle_features_request : xid from incoming cofmsg_features_request already exists in m_mxid_sxid" << std::endl;
	// send features request
	if(m_slave) {
		uint32_t myxid = send_features_request(m_slave);
		std::cout << "handle_features_request called send_features_request(" << m_slave->c_str() << ")." << std::endl;
		if(m_sxid_mxid.find(myxid)!=m_sxid_mxid.end()) std::cout << "ERROR: ctranslator::handle_features_request : xid from send_features_request already exists in m_sxid_mxid" << std::endl;
		m_mxid_sxid[masterxid]=myxid;
		m_sxid_mxid[myxid]=masterxid;
	} else std::cout << "handle_features_request from " << ctl->c_str() << " dropped because no connection to datapath present." << std::endl;
}

void ctranslator::handle_features_reply(rofl::cofdpt * dpt, rofl::cofmsg_features_reply * msg ) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << dpt->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t myxid = msg->get_xid();
	xid_map_t::iterator map_it = m_sxid_mxid.find(myxid);
	if(map_it==m_sxid_mxid.end()) { std::cout << "ERROR: ctranslator::handle_features_reply : xid from reply doesn't exist in m_sxid_mxid" << std::endl; return; }
	uint32_t orig_xid = map_it->second;
	send_features_reply(m_master, orig_xid, m_dpid, msg->get_n_buffers(), msg->get_n_tables(), msg->get_capabilities(), 0, msg->get_actions_bitmap(), msg->get_ports() );	// TODO get_action_bitmap is OF1.0 only
	std::cout << "handle_features_reply: sent features reply to " << m_master->c_str() << "." << std::endl;
	m_sxid_mxid.erase(map_it);
	m_mxid_sxid.erase(orig_xid);
	delete(msg);
}
void ctranslator::handle_get_config_request(rofl::cofctl *ctl, rofl::cofmsg_get_config_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t masterxid = msg->get_xid();
	if(m_mxid_sxid.find(masterxid)!=m_mxid_sxid.end()) std::cout << "ERROR: " << func << " : xid from incoming cofmsg_get_config_request already exists in m_mxid_sxid" << std::endl;
	if(m_slave) {
		uint32_t myxid = send_get_config_request(m_slave);
		std::cout << func << " called send_get_config_request(" << m_slave->c_str() << ")." << std::endl;
		if(m_sxid_mxid.find(myxid)!=m_sxid_mxid.end()) std::cout << "ERROR: " << func << " : xid from send_get_config_request already exists in m_sxid_mxid" << std::endl;
		m_mxid_sxid[masterxid]=myxid;
		m_sxid_mxid[myxid]=masterxid;
	} else std::cout << func << " from " << ctl->c_str() << " dropped because no connection to datapath present." << std::endl;
	delete(msg);
}

void ctranslator::handle_get_config_reply(rofl::cofdpt * dpt, rofl::cofmsg_get_config_reply * msg ) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << dpt->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t myxid = msg->get_xid();
	xid_map_t::iterator map_it = m_sxid_mxid.find(myxid);
	if(map_it==m_sxid_mxid.end()) { std::cout << "ERROR: " << func << " : xid from slave's reply doesn't exist in m_sxid_mxid" << std::endl; return; }
	uint32_t orig_xid = map_it->second;
	send_get_config_reply(m_master, orig_xid, msg->get_flags(), msg->get_miss_send_len() );
	std::cout << func << " : sent features reply to " << m_master->c_str() << "." << std::endl;
	m_sxid_mxid.erase(map_it);
	m_mxid_sxid.erase(orig_xid);
	delete(msg);
}

void ctranslator::handle_desc_stats_request(rofl::cofctl *ctl, rofl::cofmsg_desc_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t masterxid = msg->get_xid();
	if(m_mxid_sxid.find(masterxid)!=m_mxid_sxid.end()) std::cout << "ERROR: " << func << " : xid from incoming cofmsg_desc_stats_request already exists in m_mxid_sxid" << std::endl;
	if(m_slave) {
		uint32_t myxid = send_desc_stats_request(m_slave, msg->get_stats_flags());
		std::cout << func << " called send_desc_stats_request(" << m_slave->c_str() << " ..)." << std::endl;
		if(m_sxid_mxid.find(myxid)!=m_sxid_mxid.end()) std::cout << "ERROR: " << func << " : xid from send_desc_stats_request already exists in m_sxid_mxid" << std::endl;
		m_mxid_sxid[masterxid]=myxid;
		m_sxid_mxid[myxid]=masterxid;
	} else std::cout << func << " from " << ctl->c_str() << " dropped because no connection to datapath present." << std::endl;
	delete(msg);
}

void ctranslator::handle_desc_stats_reply(rofl::cofdpt * dpt, rofl::cofmsg_desc_stats_reply * msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << dpt->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t myxid = msg->get_xid();
	xid_map_t::iterator map_it = m_sxid_mxid.find(myxid);
	if(map_it==m_sxid_mxid.end()) { std::cout << "ERROR: " << func << " : xid from slave's reply doesn't exist in m_sxid_mxid" << std::endl; return; }
	uint32_t orig_xid = map_it->second;
	// TODO this bit below is wrong, but I cannot find a proper description of cofmsg_desc_stats_reply to get the information from it
	rofl::cofdesc_stats_reply reply(m_of_version,"tranny_mfr_desc","tranny_hw_desc","tranny_sw_desc","tranny_serial_num","tranny_dp_desc");
	send_desc_stats_reply(m_master, orig_xid, reply, false );
	std::cout << func << " : sent desc stats reply to " << m_master->c_str() << "." << std::endl;
	m_sxid_mxid.erase(map_it);
	m_mxid_sxid.erase(orig_xid);
	delete(msg);
}

// "Called once an EXPERIMENTER.message was received from a controller entity. ".. or is it? The docs are wrong on this one.
void ctranslator::handle_set_config(rofl::cofctl *ctl, rofl::cofmsg_set_config *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << msg->c_str() << std::endl;
	send_set_config_message(m_slave, msg->get_flags(), msg->get_miss_send_len());
	std::cout << func << " : sent set_config_message to " << m_slave->c_str() << "." << std::endl;
	delete (msg);	
}

void ctranslator::handle_table_stats_request(rofl::cofctl *ctl, rofl::cofmsg_table_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << msg->c_str() << std::endl;
}
void ctranslator::handle_port_stats_request(rofl::cofctl *ctl, rofl::cofmsg_port_stats_request *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_flow_stats_request(rofl::cofctl *ctl, rofl::cofmsg_flow_stats_request *pack) {
// see ./examples/etherswitch/etherswitch.cc:95
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_aggregate_stats_request(rofl::cofctl *ctl, rofl::cofmsg_aggr_stats_request *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_queue_stats_request(rofl::cofctl *ctl, rofl::cofmsg_queue_stats_request *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_experimenter_stats_request(rofl::cofctl *ctl, rofl::cofmsg_stats_request *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_packet_out(rofl::cofctl *ctl, rofl::cofmsg_packet_out *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_barrier_request(rofl::cofctl *ctl, rofl::cofmsg_barrier_request *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_table_mod(rofl::cofctl *ctl, rofl::cofmsg_table_mod *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_port_mod(rofl::cofctl *ctl, rofl::cofmsg_port_mod *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_queue_get_config_request(rofl::cofctl *ctl, rofl::cofmsg_queue_get_config_request *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
void ctranslator::handle_experimenter_message(rofl::cofctl *ctl, rofl::cofmsg_features_request *pack) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << ctl->c_str() << " : " << pack->c_str() << std::endl;
}
