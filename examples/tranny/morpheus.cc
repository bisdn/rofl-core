
#include "morpheus.h"
#include <boost/shared_ptr.hpp>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/openflow10.h>
#include "cportvlan_mapper.h"
#include "morpheus_nested.h"

#define PROXYOFPVERSION OFP10_VERSION

bool morpheus::associate_ctl_xid(uint32_t session_xid, uint32_t new_xid) {
	xid_session_map_t::iterator sit(m_ctl_sessions.find(session_xid));
	if(sit==m_ctl_sessions.end()) return false;	// session_xid not found
	xid_session_map_t::iterator nit(m_ctl_sessions.find(new_xid));
	if(nit!=m_ctl_sessions.end()) return false;	// new_xid already exists in m_ctl_sessions - complain.
	m_ctl_sessions[new_xid] = sit->second;
	return true;
}

bool morpheus::remove_ctl_association( const uint32_t xid ) {
	return (m_ctl_sessions.erase(xid)!=0);
}

unsigned morpheus::remove_ctl_session( session_ptr_t session_ptr ) {
	unsigned tally = 0;
	xid_session_map_t::iterator it = m_ctl_sessions.begin();
	xid_session_map_t::iterator old_it;
	while(it!=m_ctl_sessions.end()) {
		if(it->second==session_ptr) {
			old_it = it++;
			m_ctl_sessions.erase(old_it);
			++tally;
			continue;
			}
		else ++it;
	}
	return tally;
}

bool morpheus::associate_dpt_xid(uint32_t session_xid, uint32_t new_xid) {
	xid_session_map_t::iterator sit(m_dpt_sessions.find(session_xid));
	if(sit==m_dpt_sessions.end()) return false;	// session_xid not found
	xid_session_map_t::iterator nit(m_dpt_sessions.find(new_xid));
	if(nit!=m_dpt_sessions.end()) return false;	// new_xid already exists in m_dpt_sessions - complain.
	m_dpt_sessions[new_xid] = sit->second;
	return true;
}

bool morpheus::remove_dpt_association( const uint32_t xid ) {
	return (m_dpt_sessions.erase(xid)!=0);
}

unsigned morpheus::remove_dpt_session( session_ptr_t session_ptr ) {
	unsigned tally = 0;
	xid_session_map_t::iterator it = m_dpt_sessions.begin();
	xid_session_map_t::iterator old_it;
	while(it!=m_dpt_sessions.end()) {
		if(it->second==session_ptr) {
			old_it = it++;
			m_dpt_sessions.erase(old_it);
			++tally;
			continue;
			}
		else ++it;
	}
	return tally;
}

bool morpheus::add_ctl_session( const uint32_t session_xid, const session_ptr_t session_ptr ) {
xid_session_map_t::iterator sit(m_ctl_sessions.find(session_xid));
if(sit!=m_ctl_sessions.end()) return false;	// session_xid not found
m_ctl_sessions[session_xid] = session_ptr;
return true;	
}
bool morpheus::add_dpt_session( const uint32_t session_xid, const session_ptr_t session_ptr ) {
xid_session_map_t::iterator sit(m_dpt_sessions.find(session_xid));
if(sit!=m_dpt_sessions.end()) return false;	// session_xid not found
m_dpt_sessions[session_xid] = session_ptr;
return true;
}


morpheus::morpheus(const cportvlan_mapper & mapper_):rofl::crofbase (1 <<  PROXYOFPVERSION),m_slave(0),m_master(0),m_mapper(mapper_) {
	// TODO validate actual ports in port map against interrogated ports from DPE? if actual ports aren't available then from the interface as adminisrtatively down?
}

morpheus::~morpheus() {
	// rpc_close_all();
	std::cout << std::endl << "morpheus::~morpheus() called." << std::endl;	// TODO: proper logging
}

rofl::cofdpt * morpheus::get_dpt() const { return m_slave; }
rofl::cofctl * morpheus::get_ctl() const { return m_master; }


void morpheus::handle_error (rofl::cofdpt *src, rofl::cofmsg *msg) {
	std::cout << std::endl << "handle_error from " << src->c_str() << " : " << msg->c_str() << std::endl;
}

void morpheus::handle_ctrl_open (rofl::cofctl *src) {
	// should be called automatically after call to rpc_connect_to_dpt in connect_to_slave
	std::cout << std::endl << "morpheus::handle_ctrl_open called with " << (src?src->c_str():"NULL") << std::endl;
	m_master = src;	// TODO - what to do with previous m_master?
}

// TODO are all transaction IDs invalidated by a connection reset??
void morpheus::handle_ctrl_close (rofl::cofctl *src) {
	std::cout << "morpheus::handle_ctrl_close called with " << (src?src->c_str():"NULL") << std::endl;
	// controller disconnected - now disconnect from switch.
	rpc_disconnect_from_dpt(m_slave);
	// hopefully dpt will reconnect to us, causing us in turn to reconnect to ctl.
	
	// this socket disconnecting could just be a temporary thing - mark it is dead, but expect a possible auto reconnect
	if(src!=m_master) std::cout << "morpheus::handle_ctrl_close: was expecting " << (m_master?m_master->c_str():"NULL") << " but got " << (src?src->c_str():"NULL") << std::endl;
	m_master=0;	// TODO - m_naster ownership?
}

void morpheus::handle_dpath_open (rofl::cofdpt *src) {
	// should be called automatically after call to rpc_connect_to_dpt in connect_to_slave
	std::cout << std::endl << "morpheus::handle_dpath_open called with " << (src?src->c_str():"NULL") << std::endl;
	m_slave = src;	// TODO - what to do with previous m_slave?
	m_slave_dpid=src->get_dpid();	// TODO - check also get_config, get_capabilities etc
	m_dpid = m_slave_dpid + 1;
//	rpc_connect_to_ctl(m_of_version,3,rofl::caddress(AF_INET, "127.0.0.1", 6633));
	rpc_connect_to_ctl(PROXYOFPVERSION,3,rofl::caddress(AF_INET, "127.0.0.1", 6633));
}

// TODO are all transaction IDs invalidated by a connection reset??
void morpheus::handle_dpath_close (rofl::cofdpt *src) {
	std::cout << std::endl << "handle_dpath_close called with " << (src?src->c_str():"NULL") << std::endl;
//	assert(src==m_slave);
	if(src!=m_slave) std::cout << "morpheus::handle_dpath_close: Was expecting " << (m_slave?m_slave->c_str():"NULL") << " but got " << (src?src->c_str():"NULL") << std::endl;
	// this socket disconnecting could just be a temporary thing - mark it is dead, but expect a possible auto reconnect
	m_slave=0;	// TODO - m_slave ownership?
}

void morpheus::handle_flow_mod(rofl::cofctl * src, rofl::cofmsg_flow_mod *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << "." << std::endl;
	// check if this message belongs to an existing transaction
	xid_session_map_t::iterator sit(m_ctl_sessions.find(msg->get_xid()));
	if(sit!=m_ctl_sessions.end()) {
		// xid already found - it's a dupe
		std::cout << func << ": Duplicate xid (" << msg->get_xid() << ") found. Dropping new message." << std::endl;
		delete(msg);
		} else {
		// xid is a new one - create a new handler for it
		boost::shared_ptr <cflow_mod_session> s (new morpheus::cflow_mod_session( this, src, msg ));	// will take ownership of message, start processing immediately, and register itself if necessary
	}
}

void morpheus::handle_features_request(rofl::cofctl *src, rofl::cofmsg_features_request * msg ) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	xid_session_map_t::iterator sit(m_ctl_sessions.find(msg->get_xid()));
	if(sit!=m_ctl_sessions.end()) {
		// xid already found - it's a dupe
		std::cout << func << ": Duplicate xid (" << msg->get_xid() << ") found. Dropping new message." << std::endl;
		delete(msg);
		} else {
		// xid is a new one - create a new handler for it
		boost::shared_ptr <cfeatures_request_session> s (new morpheus::cfeatures_request_session( this, src, msg ));	// will take ownership of message, start processing immediately, and register itself
	}
}

void morpheus::handle_features_reply(rofl::cofdpt * src, rofl::cofmsg_features_reply * msg ) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	xid_session_map_t::iterator sit(m_dpt_sessions.find(msg->get_xid()));
	if(sit!=m_dpt_sessions.end()) {
		// known xid
		cfeatures_request_session * s = dynamic_cast<cfeatures_request_session *>(sit->second.get());
		if(!s) { std::cout << func << ": xid (" << msg->get_xid() << ") maps to existing session of wrong type." << std::endl; }
		s->process_features_reply(src, msg);	// will send reply and deregister
		} else {
		std::cout << func << ": Unexpected cofmsg_features_reply received with xid " << msg->get_xid() << ". Dropping new message." << std::endl;
		delete(msg);
	}
}
/*
void morpheus::handle_get_config_request(rofl::cofctl *src, rofl::cofmsg_get_config_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t masterxid = msg->get_xid();
	if(m_mxid_sxid.find(masterxid)!=m_mxid_sxid.end()) std::cout << "ERROR: " << func << " : xid from incoming cofmsg_get_config_request already exists in m_mxid_sxid" << std::endl;

	const std::vector< rofl::cofmsg_get_config_request > res(mapper.translate_config_request( src, msg ));
	if(res.size()!=1) std::cout << "ERROR: " << func << " : multiple translated messages not supported in handler which uses transaction IDs" << std::endl;
	uint32_t actions = res.front()->get_actions_bitmap();
	
	if(m_slave) {
		uint32_t myxid = send_get_config_request(m_slave);
		std::cout << func << " called send_get_config_request(" << m_slave->c_str() << ")." << std::endl;
		if(m_sxid_mxid.find(myxid)!=m_sxid_mxid.end()) std::cout << "ERROR: " << func << " : xid from send_get_config_request already exists in m_sxid_mxid" << std::endl;
		m_mxid_sxid[masterxid]=myxid;
		m_sxid_mxid[myxid]=masterxid;
	} else std::cout << func << " from " << src->c_str() << " dropped because no connection to datapath present." << std::endl;
	delete(msg);
}

void morpheus::handle_get_config_reply(rofl::cofdpt * src, rofl::cofmsg_get_config_reply * msg ) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t myxid = msg->get_xid();
	xid_map_t::iterator map_it = m_sxid_mxid.find(myxid);
	if(map_it==m_sxid_mxid.end()) { std::cout << "ERROR: " << func << " : xid from slave's reply doesn't exist in m_sxid_mxid" << std::endl; return; }
	uint32_t orig_xid = map_it->second;
	send_get_config_reply(m_master, orig_xid, msg->get_flags(), msg->get_miss_send_len() );
	std::cout << func << " : sent reply to " << m_master->c_str() << "." << std::endl;
	m_sxid_mxid.erase(map_it);
	m_mxid_sxid.erase(orig_xid);
	delete(msg);
}

void morpheus::handle_desc_stats_request(rofl::cofctl *src, rofl::cofmsg_desc_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t masterxid = msg->get_xid();
	if(m_mxid_sxid.find(masterxid)!=m_mxid_sxid.end()) std::cout << "ERROR: " << func << " : xid from incoming cofmsg_desc_stats_request already exists in m_mxid_sxid" << std::endl;
	if(m_slave) {
		uint32_t myxid = send_desc_stats_request(m_slave, msg->get_stats_flags());
		std::cout << func << " called send_desc_stats_request(" << m_slave->c_str() << " ..)." << std::endl;
		if(m_sxid_mxid.find(myxid)!=m_sxid_mxid.end()) std::cout << "ERROR: " << func << " : xid from send_desc_stats_request already exists in m_sxid_mxid" << std::endl;
		m_mxid_sxid[masterxid]=myxid;
		m_sxid_mxid[myxid]=masterxid;
	} else std::cout << func << " from " << src->c_str() << " dropped because no connection to datapath present." << std::endl;
	delete(msg);
}

void morpheus::handle_desc_stats_reply(rofl::cofdpt * src, rofl::cofmsg_desc_stats_reply * msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t myxid = msg->get_xid();
	xid_map_t::iterator map_it = m_sxid_mxid.find(myxid);
	if(map_it==m_sxid_mxid.end()) { std::cout << "ERROR: " << func << " : xid from slave's reply doesn't exist in m_sxid_mxid" << std::endl; return; }
	uint32_t orig_xid = map_it->second;
	// TODO this bit below is wrong, but I cannot find a proper description of cofmsg_desc_stats_reply to get the information from it
	rofl::cofdesc_stats_reply reply(src->get_version(),"tranny_mfr_desc","tranny_hw_desc","tranny_sw_desc","tranny_serial_num","tranny_dp_desc");
	send_desc_stats_reply(m_master, orig_xid, reply, false );
	std::cout << func << " : sent desc stats reply to " << m_master->c_str() << "." << std::endl;
	m_sxid_mxid.erase(map_it);
	m_mxid_sxid.erase(orig_xid);
	delete(msg);
}

// "Called once an EXPERIMENTER.message was received from a controller entity. ".. or is it? The docs are wrong on this one.
void morpheus::handle_set_config(rofl::cofctl *src, rofl::cofmsg_set_config *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	send_set_config_message(m_slave, msg->get_flags(), msg->get_miss_send_len());
	std::cout << func << " : sent set_config_message to " << m_slave->c_str() << "." << std::endl;
	delete (msg);	
}

void morpheus::handle_table_stats_request(rofl::cofctl *src, rofl::cofmsg_table_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t masterxid = msg->get_xid();
	if(m_mxid_sxid.find(masterxid)!=m_mxid_sxid.end()) std::cout << "ERROR: " << func << " : xid from incoming cofmsg_table_stats_request already exists in m_mxid_sxid" << std::endl;
	if(m_slave) {
		uint32_t myxid = send_table_stats_request(m_slave, msg->get_stats_flags());
		std::cout << func << " called send_table_stats_request(" << m_slave->c_str() << " ..)." << std::endl;
		if(m_sxid_mxid.find(myxid)!=m_sxid_mxid.end()) std::cout << "ERROR: " << func << " : xid from cofmsg_table_stats_request already exists in m_sxid_mxid" << std::endl;
		m_mxid_sxid[masterxid]=myxid;
		m_sxid_mxid[myxid]=masterxid;
	} else std::cout << func << " from " << src->c_str() << " dropped because no connection to datapath present." << std::endl;
	delete(msg);
}
void morpheus::handle_table_stats_reply(rofl::cofdpt *src, rofl::cofmsg_table_stats_reply *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t myxid = msg->get_xid();
	xid_map_t::iterator map_it = m_sxid_mxid.find(myxid);
	if(map_it==m_sxid_mxid.end()) { std::cout << "ERROR: " << func << " : xid from slave's reply doesn't exist in m_sxid_mxid" << std::endl; return; }
	uint32_t orig_xid = map_it->second;
	send_table_stats_reply(m_master, orig_xid, msg->get_table_stats(), false ); // TODO how to deal with "more" flag (last arg)
	std::cout << func << " : sent reply to " << m_master->c_str() << "." << std::endl;
	m_sxid_mxid.erase(map_it);
	m_mxid_sxid.erase(orig_xid);
	delete(msg);
}

void morpheus::handle_port_stats_request(rofl::cofctl *src, rofl::cofmsg_port_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
}
void morpheus::handle_flow_stats_request(rofl::cofctl *src, rofl::cofmsg_flow_stats_request *msg) {
// see ./examples/etherswitch/etherswitch.cc:95
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
}
void morpheus::handle_aggregate_stats_request(rofl::cofctl *src, rofl::cofmsg_aggr_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t masterxid = msg->get_xid();
	if(m_mxid_sxid.find(masterxid)!=m_mxid_sxid.end()) std::cout << "ERROR: " << func << " : xid from incoming handle_aggregate_stats_request already exists in m_mxid_sxid" << std::endl;
	if(m_slave) {
std::cout << "TP" << __LINE__ << std::endl;
		uint16_t stats_flags = msg->get_stats_flags();
std::cout << "TP" << __LINE__ << std::endl;
		rofl::cofaggr_stats_request aggr_req( msg->get_aggr_stats() );
std::cout << "TP" << __LINE__ << std::endl;
		uint32_t myxid = send_aggr_stats_request(m_slave, stats_flags, aggr_req);
std::cout << "TP" << __LINE__ << std::endl;
		std::cout << func << " called send_aggr_stats_request(" << m_slave->c_str() << " ..)." << std::endl;
		if(m_sxid_mxid.find(myxid)!=m_sxid_mxid.end()) std::cout << "ERROR: " << func << " : xid from handle_aggregate_stats_request already exists in m_sxid_mxid" << std::endl;
		m_mxid_sxid[masterxid]=myxid;
		m_sxid_mxid[myxid]=masterxid;
	} else std::cout << func << " from " << src->c_str() << " dropped because no connection to datapath present." << std::endl;
	delete(msg);
}
void morpheus::handle_aggregate_stats_reply(rofl::cofdpt *src, rofl::cofmsg_aggr_stats_reply *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t myxid = msg->get_xid();
	xid_map_t::iterator map_it = m_sxid_mxid.find(myxid);
	if(map_it==m_sxid_mxid.end()) { std::cout << "ERROR: " << func << " : xid from slave's reply doesn't exist in m_sxid_mxid" << std::endl; return; }
	uint32_t orig_xid = map_it->second;
	send_aggr_stats_reply(m_master, orig_xid, msg->get_aggr_stats(), false );	// TODO how to deal with "more" flag (last arg)
	std::cout << func << " : sent reply to " << m_master->c_str() << "." << std::endl;
	m_sxid_mxid.erase(map_it);
	m_mxid_sxid.erase(orig_xid);
	delete(msg);
}


void morpheus::handle_queue_stats_request(rofl::cofctl *src, rofl::cofmsg_queue_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
}
void morpheus::handle_experimenter_stats_request(rofl::cofctl *src, rofl::cofmsg_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
}
*/
void dumpBytes (std::ostream & os, uint8_t * bytes, size_t n_bytes) {
	if (0==n_bytes) return;
	for(size_t i = 0; i < (n_bytes-1); ++i) printf("%02x ", bytes[i]);
	printf("%02x", bytes[n_bytes-1]);
}
/*
void morpheus::handle_packet_in(rofl::cofdpt *src, rofl::cofmsg_packet_in * msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << "\n" << *msg << std::endl;
	// forward packet to master
std::cout << "TP" << __LINE__ << std::endl;
	rofl::cofmatch match(msg->get_match_const());
std::cout << "TP" << __LINE__ << "match found to be " << match.c_str() << std::endl;	
std::cout << "TP" << __LINE__ << std::endl;
	rofl::cpacket packet(msg->get_packet_const());
std::cout << "TP" << __LINE__ << std::endl;
std::cout << "packet.framelen = " << (unsigned)packet.framelen() << "packet.soframe = " << packet.soframe() << std::endl;
std::cout << "TP" << __LINE__ << std::endl;
	packet.get_match().set_in_port(msg->get_in_port());
std::cout << "TP" << __LINE__ << std::endl;
std::cout << "packet bytes: ";
dumpBytes(std::cout,msg->get_packet_const().soframe(), msg->get_packet_const().framelen());
std::cout << std::endl;
std::cout << "frame bytes: ";
dumpBytes(std::cout,msg->get_packet().frame()->soframe(), msg->get_packet().frame()->framelen());
std::cout << "TP" << __LINE__ << std::endl;
std::cout << "source MAC: " << msg->get_packet().ether()->get_dl_src() << std::endl;
std::cout << "dest MAC: " << msg->get_packet().ether()->get_dl_dst() << std::endl;
std::cout << "OFP10_PACKET_IN_STATIC_HDR_LEN is " << OFP10_PACKET_IN_STATIC_HDR_LEN << std::endl;
// *** JSP Continue working from here - The data returned by msg->get_packet() is missing the first four bytes, and because of this the printed MAC addresses are wrong.
// ** TODO: check who creates msg and figure out why it "starts late"
std::cout << "TP" << __LINE__ << std::endl;
	send_packet_in_message(m_master, msg->get_buffer_id(), msg->get_total_len(), msg->get_reason(), 0, 0, msg->get_in_port(), match, packet.ether()->sopdu(), packet.framelen() );	// TODO - the length fields are guesses.
//	send_packet_in_message(m_master, msg->get_buffer_id(), msg->get_total_len(), msg->get_reason(), 0, 0, msg->get_in_port(), match, packet.soframe(), packet.framelen() );	// TODO - the length fields are guesses.
//	send_packet_in_message(m_master, msg->get_buffer_id(), msg->get_total_len(), msg->get_reason(), 0, 0, msg->get_in_port(), match, packet.frame()->sopdu(), packet.frame()->pdulen() );	// TODO - the length fields are guesses.
	std::cout << func << " : packet_in forwarded to " << m_master->c_str() << "." << std::endl;
#if 0
std::cout << "TP" << __LINE__ << std::endl;
	std::cout << "dpt version is " << (unsigned)src->get_version() << std::endl;
std::cout << "TP" << __LINE__ << std::endl;
//	send_packet_in_message(m_master, msg->get_buffer_id(), msg->get_total_len(), msg->get_reason(), msg->get_table_id(), msg->get_cookie(), msg->get_in_port(), match, packet.soframe(), packet.framelen() );	// TODO - the length fields are guesses.
		uint32_t  	buffer_id_ = msg->get_buffer_id();
std::cout << "TP" << __LINE__ << std::endl;
		uint16_t  	total_len_ = msg->get_total_len();
std::cout << "TP" << __LINE__ << std::endl;
		uint8_t  	reason_ = msg->get_reason();
//		uint8_t  	table_id,
std::cout << "TP" << __LINE__ << std::endl;
//		uint64_t  	cookie_ = msg->get_cookie();
uint64_t  	cookie_ = 0;
std::cout << "TP" << __LINE__ << std::endl;
		uint16_t  	in_port_ = msg->get_in_port();
std::cout << "TP" << __LINE__ << std::endl;
	send_packet_in_message(m_master, buffer_id_, total_len_, reason_, 0, cookie_, in_port_, match, packet.soframe(), packet.framelen() );	// TODO - the length fields are guesses.
std::cout << "TP" << __LINE__ << std::endl;
#endif

	delete(msg);
}
void morpheus::handle_packet_out(rofl::cofctl *src, rofl::cofmsg_packet_out *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	rofl::cofaclist actions(msg->get_actions());
	std::cout << "TP" << __LINE__ << std::endl;
	rofl::cpacket packet(msg->get_packet());
	std::cout << "TP" << __LINE__ << std::endl;
	send_packet_out_message(m_slave, msg->get_buffer_id(), msg->get_in_port(), actions, packet.soframe(), packet.framelen() );	// TODO - the length fields are guesses.
	std::cout << "TP" << __LINE__ << std::endl;
	delete(msg);
}
void morpheus::handle_barrier_request(rofl::cofctl *src, rofl::cofmsg_barrier_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t masterxid = msg->get_xid();
	if(m_mxid_sxid.find(masterxid)!=m_mxid_sxid.end()) std::cout << "ERROR: " << func << " : xid from incoming handle_barrier_request already exists in m_mxid_sxid" << std::endl;
	if(m_slave) {
		uint32_t myxid = send_barrier_request(m_slave);
		if(m_sxid_mxid.find(myxid)!=m_sxid_mxid.end()) std::cout << "ERROR: " << func << " : xid from handle_barrier_request already exists in m_sxid_mxid" << std::endl;
		m_mxid_sxid[masterxid]=myxid;
		m_sxid_mxid[myxid]=masterxid;
	} else std::cout << func << " from " << src->c_str() << " dropped because no connection to datapath present." << std::endl;
	delete(msg);
}

void morpheus::handle_barrier_reply ( rofl::cofdpt * src, rofl::cofmsg_barrier_reply * msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	uint32_t myxid = msg->get_xid();
	xid_map_t::iterator map_it = m_sxid_mxid.find(myxid);
	if(map_it==m_sxid_mxid.end()) { std::cout << "ERROR: " << func << " : xid from slave's reply doesn't exist in m_sxid_mxid" << std::endl; return; }
	uint32_t orig_xid = map_it->second;
	send_barrier_reply(m_master, orig_xid );
	std::cout << func << " : sent reply to " << m_master->c_str() << "." << std::endl;
	m_sxid_mxid.erase(map_it);
	m_mxid_sxid.erase(orig_xid);
	delete(msg);
}

void morpheus::handle_table_mod(rofl::cofctl *src, rofl::cofmsg_table_mod *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	delete(msg);
}
void morpheus::handle_port_mod(rofl::cofctl *src, rofl::cofmsg_port_mod *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	delete(msg);
}
void morpheus::handle_queue_get_config_request(rofl::cofctl *src, rofl::cofmsg_queue_get_config_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	delete(msg);
}
void morpheus::handle_experimenter_message(rofl::cofctl *src, rofl::cofmsg_features_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	delete(msg);
}
*/
std::string action_mask_to_string(const uint32_t action_types) {
	std::string out;
	static const uint32_t vals[] = { OFP10AT_OUTPUT, OFP10AT_SET_VLAN_VID, OFP10AT_SET_VLAN_PCP, OFP10AT_STRIP_VLAN, OFP10AT_SET_DL_SRC, OFP10AT_SET_DL_DST, OFP10AT_SET_NW_SRC, OFP10AT_SET_NW_DST, OFP10AT_SET_NW_TOS, OFP10AT_SET_TP_SRC, OFP10AT_SET_TP_DST, OFP10AT_ENQUEUE };
	static const std::string names[] = { "OFP10AT_OUTPUT", "OFP10AT_SET_VLAN_VID", "OFP10AT_SET_VLAN_PCP", "OFP10AT_STRIP_VLAN", "OFP10AT_SET_DL_SRC", "OFP10AT_SET_DL_DST", "OFP10AT_SET_NW_SRC", "OFP10AT_SET_NW_DST", "OFP10AT_SET_NW_TOS", "OFP10AT_SET_TP_SRC", "OFP10AT_SET_TP_DST", "OFP10AT_ENQUEUE" };
	static const size_t N = sizeof(vals)/sizeof(vals[0]);
	for(size_t i=0;i<N;++i) {
		if(action_types & (1<<vals[i])) out += names[i] + " ";
	}
	return out;
}


