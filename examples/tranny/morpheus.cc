
#include "morpheus.h"
#include <memory>	// for auto_ptr
#include <cassert>
#include <utility>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/thread_helper.h>
#include "cportvlan_mapper.h"
// #include "morpheus_nested.h"

#define PROXYOFPVERSION OFP10_VERSION

std::ostream & operator<< (std::ostream & os, const morpheus & morph) {
	os << "morpheus configuration:\n" << morph.dump_config();
	os << "morpheus current sessions:" << morph.dump_sessions();
	os << "morpheus mappings:" << morph.m_mapper;
	os << std::endl;
	return os;
	}

// print out a hexdump of bytes
void dumpBytes (std::ostream & os, uint8_t * bytes, size_t n_bytes) {
	if (0==n_bytes) return;
	for(size_t i = 0; i < (n_bytes-1); ++i) printf("%02x ", bytes[i]);
	printf("%02x", bytes[n_bytes-1]);
}

std::string morpheus::dump_sessions() const {
	std::stringstream ss;
/*	std::cout << __FUNCTION__ << ": waiting for lock." << std::endl;
	rofl::RwLock lock(&m_sessions_lock, rofl::RwLock::RWLOCK_WRITE);
	std::cout << __FUNCTION__ << ": got lock." << std::endl; */
	for(xid_session_map_t::const_iterator it = m_sessions.begin(); it != m_sessions.end(); ++it)
		ss << ((it->first.first)?"ctl":"dpt") << " xid " << it->first.second << ": " << it->second->asString() << "\n";
	return ss.str();
}

std::string morpheus::dump_config() const {	// TODO
	return "";
}

// associates new_xid with the session p, taking ownership of p - returns true if associated, false if already exists.
bool morpheus::associate_xid( const bool ctl_or_dpt_xid, const uint32_t new_xid, chandlersession_base * p ) {
	std::pair< bool, uint32_t > xid_ ( ctl_or_dpt_xid, new_xid );
/*	std::cout << __FUNCTION__ << ": waiting for lock." << std::endl;
	rofl::RwLock lock(&m_sessions_lock, rofl::RwLock::RWLOCK_WRITE);
	std::cout << __FUNCTION__ << ": got lock." << std::endl; */
	xid_session_map_t::iterator sit(m_sessions.find(xid_));
	if(sit!=m_sessions.end()) {
		std::cout << "Attempt was made to associate " << ((ctl_or_dpt_xid)?"ctl":"dpt") << " xid " << new_xid << " with session base " << std::hex << p << " but xid already exists." << std::endl;
		return false;	// new_xid was already in the database
	}
// 	assert();	// if it wasn't found in m_sessions then it shouldn't be in m_reverse_sessions;
	std::cout << "Associating new " << ((ctl_or_dpt_xid)?"ctl":"dpt") << " xid " << new_xid << " with session base " << std::hex << p << std::endl;
	m_sessions[xid_] = p;
	return true;
}

// called to remove the association of the xid with a session_base - returns true if session_xid was found and removed, false otherwise
bool morpheus::remove_xid_association( const bool ctl_or_dpt_xid, const uint32_t new_xid ) {
	// remove the xid, and check whether the pointed to session_base is associated anywhere else, if it isn't, delete it.
	std::pair< bool, uint32_t > xid_ ( ctl_or_dpt_xid, new_xid );
/*	std::cout << __FUNCTION__ << ": waiting for lock." << std::endl;
	rofl::RwLock lock(&m_sessions_lock, rofl::RwLock::RWLOCK_WRITE);
	std::cout << __FUNCTION__ << ": got lock." << std::endl; */
	xid_session_map_t::iterator sit(m_sessions.find(xid_));
	if(sit==m_sessions.end()) return false;
	chandlersession_base * p = sit->second;
	m_sessions.erase(sit);
	// now check if that was the last reference to p, and delete p if it was.
	xid_session_map_t::iterator it = m_sessions.begin();
	while(it!=m_sessions.end()) {
		if(it->second==p) return true;
		++it;
	}
	std::cout << __FUNCTION__ << " deleting session " << std::hex << p << " as we have just removed last reference." << std::endl;
	delete(p);
	return true;
}

// called to remove all associations to this session_base - returns the number of associations removed - p is not deleted and reamins in the ownership of the caller
unsigned morpheus::remove_session( chandlersession_base * p ) {
	unsigned tally = 0;
/*	std::cout << __FUNCTION__ << ": waiting for lock." << std::endl;
	rofl::RwLock lock(&m_sessions_lock, rofl::RwLock::RWLOCK_WRITE);
	std::cout << __FUNCTION__ << ": got lock." << std::endl; */
	xid_session_map_t::iterator it = m_sessions.begin();
	xid_session_map_t::iterator old_it;
	while(it!=m_sessions.end()) {
		if(it->second==p) {
			old_it = it++;
			m_sessions.erase(old_it);
			++tally;
			continue;
			}
		else ++it;
	}
//	if(tally) delete(p);
	return tally;
}
/*
uint32_t morpheus::set_supported_actions (uint32_t new_actions) {
	uint32_t old_actions = m_supported_actions;
	m_supported_actions = new_actions & m_supported_actions_mask;
	m_supported_actions_valid = true;
	return old_actions;
}
*/
void morpheus::set_supported_dpe_features (uint32_t new_capabilities, uint32_t new_actions) {
	// TODO new_capabilities are ignored, befause, well, we don't support any of them.
	// m_supported_features = 0;
	m_dpe_supported_actions = new_actions;
	m_supported_actions = new_actions & m_supported_actions_mask;
	m_dpe_supported_actions_valid = true;
}

uint32_t morpheus::get_supported_actions() {
	if(!m_dpe_supported_actions_valid) {	// for when get_supported_actions is called before set_supported_features
		// we have no information on supported actions from the DPE, so we're going to have to ask ourselves.
		std::auto_ptr < morpheus::csh_features_request > s ( new morpheus::csh_features_request ( this ) );
		std::cout << __FUNCTION__ << ": sent request for features. Waiting..";
		unsigned wait_time = 5;
		while(wait_time && !s->isCompleted()) {
			sleep(1);	// TODO possible error - is crofbase a single thread, or is every call to a crofbase handler a new thread?
			std::cout << ".";
			--wait_time;
		}
		std::cout << std::endl;
		if(!s->isCompleted()) {
			s.release();
//			throw rofl::eInval(std::string("Request features in morpheus::get_supported_actions but never got a response."));
			throw rofl::eInval();
		}
	}
	return m_supported_actions;
}


morpheus::morpheus(const cportvlan_mapper & mapper_, const bool indpt_, const rofl::caddress dptaddr_, const bool inctl_, const rofl::caddress ctladdr_ ):rofl::crofbase (1 <<  PROXYOFPVERSION),m_slave(0),m_master(0),m_mapper(mapper_),m_supported_features(0),m_supported_actions_mask( (1<<OFP10AT_OUTPUT)|(1<<OFP10AT_SET_DL_SRC)|(1<<OFP10AT_SET_DL_DST)|(1<<OFP10AT_SET_NW_SRC)|(1<<OFP10AT_SET_NW_DST)|(1<<OFP10AT_SET_NW_TOS)|(1<<OFP10AT_SET_TP_SRC)|(1<<OFP10AT_SET_TP_DST) ), m_supported_actions(0), m_dpe_supported_actions(0), m_dpe_supported_actions_valid(false),indpt(indpt_),inctl(inctl_),dptaddr(dptaddr_),ctladdr(ctladdr_) {
	// TODO validate actual ports in port map against interrogated ports from DPE? if actual ports aren't available then from the interface as adminisrtatively down?
	pthread_rwlock_init(&m_sessions_lock, 0);
	init_dpe();
}

morpheus::~morpheus() {
	// rpc_close_all();
	std::cout << std::endl << __FUNCTION__ << " called." << std::endl;	// TODO: proper logging
	pthread_rwlock_destroy(&m_sessions_lock);
}

void morpheus::init_dpe(){
	if(indpt) rpc_listen_for_dpts(dptaddr);
	else rpc_connect_to_dpt(PROXYOFPVERSION, 5, dptaddr);
}

rofl::cofdpt * morpheus::get_dpt() const { return m_slave; }
rofl::cofctl * morpheus::get_ctl() const { return m_master; }


void morpheus::handle_error (rofl::cofdpt *src, rofl::cofmsg *msg) {
	std::cout << std::endl << "handle_error from " << src->c_str() << " : " << msg->c_str() << std::endl;
	rofl::RwLock lock(&m_sessions_lock, rofl::RwLock::RWLOCK_WRITE);
	xid_session_map_t::iterator sit(m_sessions.find(std::pair<bool, uint32_t>(false,msg->get_xid())));
	if(sit!=m_sessions.end()) {
		sit->second->handle_error( src, msg);
	} else std::cout << "**** received error message ( " << msg->c_str() << " ) from dpt ( " << std::hex << src << " ) for unknown xid ( " << msg->get_xid() << " ). Dropping." << std::endl;
	delete(msg);
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
	std::cout << " Following new DPE connection, sending features request.." << std::endl;

	rofl::RwLock lock(&m_sessions_lock, rofl::RwLock::RWLOCK_WRITE);
	morpheus::csh_features_request * s = new morpheus::csh_features_request ( this );	// this isn;t a memory leak - the object was registered with the xid database
	if(!s) std::cout << "Dummy message to prevent above line being optimised out." << std::endl;

//	rpc_connect_to_ctl(m_of_version,3,rofl::caddress(AF_INET, "127.0.0.1", 6633));
///	rpc_connect_to_ctl(PROXYOFPVERSION,3,rofl::caddress(AF_INET, "127.0.0.1", 6633)); // for general floodlight use
//	rpc_connect_to_ctl(PROXYOFPVERSION,3,rofl::caddress(AF_INET, "10.100.0.2", 6633)); // for the oftest config
	if(!m_master){
		if(inctl) rpc_listen_for_ctls(ctladdr);
		else rpc_connect_to_ctl(PROXYOFPVERSION,5,ctladdr);
	}
}

// TODO are all transaction IDs invalidated by a connection reset??
void morpheus::handle_dpath_close (rofl::cofdpt *src) {
	std::cout << std::endl << "handle_dpath_close called with " << (src?src->c_str():"NULL") << std::endl;
//	assert(src==m_slave);
	if(src!=m_slave) std::cout << "morpheus::handle_dpath_close: Was expecting " << (m_slave?m_slave->c_str():"NULL") << " but got " << (src?src->c_str():"NULL") << std::endl;
	// this socket disconnecting could just be a temporary thing - mark it is dead, but expect a possible auto reconnect
	m_slave=0;	// TODO - m_slave ownership?

	rpc_disconnect_from_ctl(m_master);
//	init_dpe();
// no need - we're still istening, apparently
}

void morpheus::handle_timeout ( int opaque ) {
	std::cout << "****" << __FUNCTION__ << " called with opaque = " << opaque << std::endl;
}

void morpheus::handle_error ( rofl::cofdpt * src, rofl::cofmsg_error * msg ) {
	std::cout << "****" << __FUNCTION__ << " called with dpt (" << std::hex << src << "): " << msg->c_str() << std::endl;
	delete(msg);
}

#undef STRINGIFY
#undef TOSTRING
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// TODO the auto_ptr and if(!s->isCompleted()) s.release(); are probably wrong and not necessary since the object takes ownership of itself.

// this is from a ctl if CTL_DPT is true, false otherwise
#define HANDLE_REQUEST_WITH_REPLY_TEMPLATE(CTL_DPT, MSG_TYPE, SESSION_TYPE) {\
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl; \
	if((!m_slave)||(!m_master)) { std::cout << "Dropping message due to lack of CTL/DPT connectivity." << std::endl; delete(msg); return; } \
	rofl::RwLock lock(&m_sessions_lock, rofl::RwLock::RWLOCK_WRITE); \
	xid_session_map_t::iterator sit(m_sessions.find(std::pair<bool, uint32_t>(CTL_DPT,msg->get_xid()))); \
	if(sit!=m_sessions.end()) { \
		std::cout << func << ": Duplicate ctl xid (" << msg->get_xid() << ") found. Dropping new "<< TOSTRING(MSG_TYPE) << " message." << std::endl; \
		} else { \
		try { \
			std::auto_ptr < SESSION_TYPE > s ( new SESSION_TYPE ( this, src, msg ) ); \
			if(!s->isCompleted()) s.release(); \
		} catch(rofl::cerror &e) { std::cout << "unhandled rofl::cerror: " << e.desc << std::endl; assert(false); } \
		catch (...) { std::cout << "unhandled exception"; assert(false); } \
	}\
	delete(msg); \
	std::cout << ">>>>>>>\n" << *this; \
}

#define HANDLE_REPLY_AFTER_REQUEST_TEMPLATE(CTL_DPT, MSG_TYPE, SESSION_TYPE, REPLY_FN) { \
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl; \
	if((!m_slave)||(!m_master)) { std::cout << "Dropping message due to lack of CTL/DPT connectivity." << std::endl; delete(msg); return; } \
	rofl::RwLock lock(&m_sessions_lock, rofl::RwLock::RWLOCK_WRITE); \
	xid_session_map_t::iterator sit(m_sessions.find(std::pair<bool, uint32_t>(CTL_DPT,msg->get_xid()))); \
	if(sit!=m_sessions.end()) { \
		SESSION_TYPE * s = dynamic_cast<SESSION_TYPE *>(sit->second); \
		if(!s) { std::cout << func << ": xid (" << msg->get_xid() << ") maps to existing session of wrong type." << std::endl; } \
		try { \
			s->REPLY_FN( src, msg ); \
		} catch(rofl::cerror &e) { std::cout << "unhandled rofl::cerror: " << e.desc << std::endl; assert(false); } \
		catch (...) { std::cout << "unhandled exception"; assert(false); } \
		if(s->isCompleted()) { remove_session(s); delete(s); } \
		} else { \
		std::cout << func << ": Unexpected " << TOSTRING(MSG_TYPE) << " received with xid " << msg->get_xid() << ". Dropping new message." << std::endl; \
	}\
	delete(msg); \
	std::cout << ">>>>>>>\n" << *this; \
}

#define HANDLE_MESSAGE_FORWARD_TEMPLATE(CTL_DPT, SESSION_TYPE) {\
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << "." << std::endl;\
	if((!m_slave)||(!m_master)) { std::cout << "Dropping message due to lack of CTL/DPT connectivity." << std::endl; delete(msg); return; } \
	rofl::RwLock lock(&m_sessions_lock, rofl::RwLock::RWLOCK_WRITE); \
	xid_session_map_t::iterator sit(m_sessions.find(std::pair<bool, uint32_t>(CTL_DPT,msg->get_xid()))); \
	if(sit!=m_sessions.end()) {\
		std::cout << func << ": Duplicate xid (" << msg->get_xid() << ") found. Dropping new message." << std::endl;\
		} else {\
		try { \
			std::auto_ptr < SESSION_TYPE > s ( new SESSION_TYPE ( this, src, msg ) ); \
			if(s->isCompleted()) { remove_session(s.get()); } \
			else { /* s.release(); */ std::cout << func << " failed to complete session!" << std::endl; assert(false); } \
		 } catch(rofl::cerror &e) { std::cout << "unhandled rofl::cerror: " << e.desc << std::endl; assert(false); } \
		 catch (...) { std::cout << "unhandled exception"; assert(false); } \
	} \
	delete(msg);\
	std::cout << ">>>>>>>\n" << *this; \
}
/*
#define HANDLE_MESSAGE_TIMEOUT(SESSION_TYPE, TIMEOUT_FN) {\
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << "." << std::endl;\
	if((!m_slave)||(!m_master)) { std::cout << "Dropping message due to lack of CTL/DPT connectivity." << std::endl; delete(msg); return; } \
	*/

void morpheus::handle_flow_mod(rofl::cofctl * src, rofl::cofmsg_flow_mod *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_MESSAGE_FORWARD_TEMPLATE(true, morpheus::csh_flow_mod)
//	HANDLE_MESSAGE_FORWARD_TEMPLATE(true, morpheus::DEBUG_csh_flow_mod)
}
void morpheus::handle_features_request(rofl::cofctl *src, rofl::cofmsg_features_request * msg ) {
	static const char * func = __FUNCTION__;
	HANDLE_REQUEST_WITH_REPLY_TEMPLATE( true, cofmsg_features_request, morpheus::csh_features_request )
}
void morpheus::handle_features_reply(rofl::cofdpt * src, rofl::cofmsg_features_reply * msg ) {
	static const char * func = __FUNCTION__;
	HANDLE_REPLY_AFTER_REQUEST_TEMPLATE( false, cofmsg_features_reply, morpheus::csh_features_request, process_features_reply )
}

void morpheus::handle_get_config_request(rofl::cofctl *src, rofl::cofmsg_get_config_request *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_REQUEST_WITH_REPLY_TEMPLATE( true, cofmsg_get_config_request, morpheus::csh_get_config )
}
void morpheus::handle_get_config_reply(rofl::cofdpt * src, rofl::cofmsg_get_config_reply * msg ) {
	static const char * func = __FUNCTION__;
	HANDLE_REPLY_AFTER_REQUEST_TEMPLATE( false, cofmsg_get_config_reply, morpheus::csh_get_config, process_config_reply )
}

void morpheus::handle_desc_stats_request(rofl::cofctl *src, rofl::cofmsg_desc_stats_request *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_REQUEST_WITH_REPLY_TEMPLATE( true, cofmsg_desc_stats_request, morpheus::csh_desc_stats )
}
void morpheus::handle_desc_stats_reply(rofl::cofdpt * src, rofl::cofmsg_desc_stats_reply * msg) {
	static const char * func = __FUNCTION__;
	HANDLE_REPLY_AFTER_REQUEST_TEMPLATE( false, cofmsg_desc_stats_reply, morpheus::csh_desc_stats, process_desc_stats_reply )
}

void morpheus::handle_set_config(rofl::cofctl *src, rofl::cofmsg_set_config *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_MESSAGE_FORWARD_TEMPLATE(true, morpheus::csh_set_config)
}

void morpheus::handle_table_stats_request(rofl::cofctl *src, rofl::cofmsg_table_stats_request *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_REQUEST_WITH_REPLY_TEMPLATE( true, cofmsg_table_stats_request, morpheus::csh_table_stats )
}
void morpheus::handle_table_stats_reply(rofl::cofdpt *src, rofl::cofmsg_table_stats_reply *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_REPLY_AFTER_REQUEST_TEMPLATE( false, cofmsg_table_stats_reply, morpheus::csh_table_stats, process_table_stats_reply )
}

void morpheus::handle_aggregate_stats_request(rofl::cofctl *src, rofl::cofmsg_aggr_stats_request *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_REQUEST_WITH_REPLY_TEMPLATE( true, cofmsg_aggr_stats_request, morpheus::csh_aggregate_stats )
}
void morpheus::handle_aggregate_stats_reply(rofl::cofdpt *src, rofl::cofmsg_aggr_stats_reply *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_REPLY_AFTER_REQUEST_TEMPLATE( false, cofmsg_aggr_stats_reply, morpheus::csh_aggregate_stats, process_aggr_stats_reply )
}

/*
void dumpBytes (std::ostream & os, uint8_t * bytes, size_t n_bytes) {
	if (0==n_bytes) return;
	for(size_t i = 0; i < (n_bytes-1); ++i) printf("%02x ", bytes[i]);
	printf("%02x", bytes[n_bytes-1]);
}
*/
void morpheus::handle_packet_in(rofl::cofdpt *src, rofl::cofmsg_packet_in * msg) {
	static const char * func = __FUNCTION__;
	HANDLE_MESSAGE_FORWARD_TEMPLATE(false, morpheus::csh_packet_in)
}
void morpheus::handle_packet_out(rofl::cofctl *src, rofl::cofmsg_packet_out *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_MESSAGE_FORWARD_TEMPLATE(true, morpheus::csh_packet_out)
}

void morpheus::handle_barrier_request(rofl::cofctl *src, rofl::cofmsg_barrier_request *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_REQUEST_WITH_REPLY_TEMPLATE( true, cofmsg_barrier_request, morpheus::csh_barrier )
}
void morpheus::handle_barrier_reply ( rofl::cofdpt * src, rofl::cofmsg_barrier_reply * msg) {
	static const char * func = __FUNCTION__;
	HANDLE_REPLY_AFTER_REQUEST_TEMPLATE( false, cofmsg_barrier_reply, morpheus::csh_barrier, process_barrier_reply )
}

void morpheus::handle_port_stats_request(rofl::cofctl *src, rofl::cofmsg_port_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	delete(msg);
}
void morpheus::handle_flow_stats_request(rofl::cofctl *src, rofl::cofmsg_flow_stats_request *msg) {
// see ./examples/etherswitch/etherswitch.cc:95
	static const char * func = __FUNCTION__;
//	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
//	delete(msg);
	HANDLE_REQUEST_WITH_REPLY_TEMPLATE( true, cofmsg_flow_stats_request, morpheus::csh_flow_stats )
}

void morpheus::handle_queue_stats_request(rofl::cofctl *src, rofl::cofmsg_queue_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	delete(msg);
}
void morpheus::handle_experimenter_stats_request(rofl::cofctl *src, rofl::cofmsg_stats_request *msg) {
	static const char * func = __FUNCTION__;
	std::cout << std::endl << func << " from " << src->c_str() << " : " << msg->c_str() << std::endl;
	delete(msg);
}
/*
void morpheus::handle_table_mod(rofl::cofctl *src, rofl::cofmsg_table_mod *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_MESSAGE_FORWARD_TEMPLATE(true, morpheus::csh_table_mod)
}*/
void morpheus::handle_port_mod(rofl::cofctl *src, rofl::cofmsg_port_mod *msg) {
	static const char * func = __FUNCTION__;
	HANDLE_MESSAGE_FORWARD_TEMPLATE(true, morpheus::csh_port_mod)
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

/*
bool morpheus::add_flowmod_action_translation(const rofl::cofaclist & virt, const rofl::cofaclist & act) {
	bool this_is_new_entry = (action_map.find(virt) == action_map.end());
	action_map[virt] = act;
	return this_is_new_entry;
}
bool morpheus::remove_flowmod_action_translation(const rofl::cofaclist & virt) {
	return action_map.erase(virt);
}
// could throw std::out_of_range if virt_or_act is not found
rofl::cofaclist morpheus::get_flowmod_action_translation(bool virtual_to_actual, const rofl::cofaclist & virt_or_act) const {
	if(virtual_to_actual) {
		// look up virtual action
		return action_map.at(virt_or_act);
	} else {
		// do reverse lookup - see if virt_or_act is a value for any of the keys in action_map and return key;
		for(std::map<rofl::cofaclist, rofl::cofaclist>::const_iterator i = action_map.begin(); i != action_map.begin(); ++i)
			if(i->second==virt_or_act) return i->first;
		// not found
		throw std::out_of_range();
	}
}


bool morpheus::add_flowmod_match_translation(const rofl::cofmatch & virt, const rofl::cofmatch & act) {
	bool this_is_new_entry = (match_map.find(virt) == action_map.end());
	match_map[virt] = act;
	return this_is_new_entry;
}
bool morpheus::remove_flowmod_match_translation(const rofl::cofmatch & virt) {
	return match_map.erase(virt);
}
// could throw std::out_of_range if virt_or_act is not found
rofl::cofmatch morpheus::get_flowmod_match_translation(bool virtual_to_actual, const rofl::cofmatch & virt_or_act) const {
	if(virtual_to_actual) {
		// look up virtual action
		return match_map.at(virt_or_act);
	} else {
		// do reverse lookup - see if virt_or_act is a value for any of the keys in action_map and return key;
		for(std::map<rofl::cofmatch, rofl::cofmatch>::const_iterator i = match_map.begin(); i != match_map.begin(); ++i)
			if(i->second==virt_or_act) return i->first;
		// not found
		throw std::out_of_range();
	}
}
*/
/*
// returns empty vector if no matches
std::vector<morpheus::flowentry_db_t::iterator> morpheus::getTranslatedFlowentry (const morpheus::flowentry & untranslated_flowentry) {
	return std::vector<morpheus::flowentry>();
}

// throws std::range_error if not found
morpheus::flowentry_db_t::iterator morpheus::getExactTranslatedFlowentry (const morpheus::flowentry & untranslated_flowspec){
	throw std::range_error("entry not found, or more than one entry found in morpheus::getExactTranslatedFlowentry");
}

// returns empty vector if no matches
std::vector<morpheus::flowentry_db_t::iterator> morpheus::getUnTranslatedFlowentry (const morpheus::flowentry & translated_flowspec) {
	return std::vector<morpheus::flowentry>();
}

// throws std::out_of_range if not found
morpheus::flowentry_db_t::iterator morpheus::getExactUnTranslatedFlowentry (const morpheus::flowentry & translated_flowspec) {
	throw std::out_of_range("entry not found in morpheus::getExactUnTranslatedFlowentry");
}

// return true if added, false if such an untranslated entry already exists, and then doesn't overwrite
bool morpheus::addFlowentryTranslation ( const morpheus::flowentry & untranslated, const morpheus::flowentry & translated ) {
//	return m_flowentry_db.insert(std::make_pair(untranslated,translated)).second;
}

// returns the number of removed entries
template <InputIterator T> size_t morpheus::removeFlowentryTranslation ( I begin, I end ) {
//	return m_flowentry_db.erase(untranslated);
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

std::string port_as_string(uint16_t p) {	// maps a ofp10_port_no port number to a string if it has a meaning (like FLOOD), or just to a number.
	static const uint32_t vals[] = { OFPP10_IN_PORT, OFPP10_TABLE, OFPP10_NORMAL, OFPP10_FLOOD, OFPP10_ALL, OFPP10_CONTROLLER, OFPP10_LOCAL, OFPP10_NONE };
	static const std::string names[] = { "OFPP10_IN_PORT", "OFPP10_TABLE", "OFPP10_NORMAL", "OFPP10_FLOOD", "OFPP10_ALL", "OFPP10_CONTROLLER", "OFPP10_LOCAL", "OFPP10_NONE" };
	static const size_t N = sizeof(vals)/sizeof(vals[0]);
	for(size_t i=0;i<N;++i) {
		if(vals[i]==p) return names[i];
	}
	std::stringstream s;
	if(p <= OFPP10_MAX) s << "PORT_NO:";
	else s << "INVALID_PORT_NO:";
	s << p;
	return s.str();
	}

std::string capabilities_to_string(uint32_t capabilities) {
	static const std::string capabilities_sz [] = { "OFPC_FLOW_STATS", "OFPC_TABLE_STATS", "OFPC_PORT_STATS", "OFPC_STP", "OFPC_RESERVED", "OFPC_IP_REASM", "OFPC_QUEUE_STATS", "OFPC_ARP_MATCH_IP" };
	std::string out;
	for(size_t index=0; capabilities!=0; ++index, capabilities >>= 1) if(capabilities & 0x00000001) out += capabilities_sz[index] + " ";
	return out;
}

namespace std {	// TODO untested

bool operator==(const rofl::cofaclist & a, const rofl::cofaclist & b) {
	if(a.length()!=b.length()) return false;
	return std::equal(a.begin(), a.end(), b.begin());
	}
/*
bool operator<(const rofl::cofaclist & a, const rofl::cofaclist & b) {
	size_t a_len = a.length(), b_len = b.length();
	if(a_len<b_len) return true;
	if(a_len>b_len) return false;
	// return std::equal(a.begin(), a.end(), b.begin());
	rofl::cofaclist::const_iterator a_i = a.begin(), b_i = b.begin();
	while(a_i != a.end()) {
		if(*a_i<)
	}
	}
*/

bool operator==(const rofl::cofaction & a, const rofl::cofaction & b) {
	if(a.get_type()!=b.get_type()) return false;
	
	switch (a.get_type()) {
		case OFP10AT_STRIP_VLAN: {
			// no fields present in struct - nothing to compare other than type, which was already done
			return true;
			} break;
		case OFP10AT_OUTPUT: {
			struct ofp10_action_output * a_ = (struct ofp10_action_output*)a.soaction();
			struct ofp10_action_output * b_ = (struct ofp10_action_output*)b.soaction();
			// no padding to clear
			// return ( *a_ == *b_ );
			return std::equal((uint8_t *)a_, ((uint8_t *)a_)+sizeof(struct ofp10_action_output), (uint8_t *)b_);
			} break;
		case OFP10AT_SET_NW_SRC:
		case OFP10AT_SET_NW_DST: {
			struct ofp10_action_nw_addr *a_ = (struct ofp10_action_nw_addr*)a.soaction();
			struct ofp10_action_nw_addr *b_ = (struct ofp10_action_nw_addr*)b.soaction();
			// no padding to clear
			return std::equal((uint8_t *)a_, ((uint8_t *)a_)+sizeof(struct ofp10_action_nw_addr), (uint8_t *)b_);
		} break;
		case OFP10AT_SET_VLAN_VID: {
			struct ofp10_action_vlan_vid a_ = *(struct ofp10_action_vlan_vid*)a.soaction();
			struct ofp10_action_vlan_vid b_ = *(struct ofp10_action_vlan_vid*)b.soaction();
			// copies of structs are made, instead of using referenced pointers to original, so that the padding can be zeroed for a safe comparison
			std::fill( &a_.pad[0], &a_.pad[0]+sizeof(a_.pad), 0);
			std::fill( &b_.pad[0], &b_.pad[0]+sizeof(b_.pad), 0);
			//return ( a_ == b_ );
			return std::equal((uint8_t *)&a_, ((uint8_t *)&a_)+sizeof(struct ofp10_action_vlan_vid), (uint8_t *)&b_);
			} break;
		case OFP10AT_SET_VLAN_PCP: {
			struct ofp10_action_vlan_pcp a_ = *(struct ofp10_action_vlan_pcp*)a.soaction();
			struct ofp10_action_vlan_pcp b_ = *(struct ofp10_action_vlan_pcp*)b.soaction();
			// copies of structs are made, instead of using referenced pointers to original, so that the padding can be zeroed for a safe comparison
			std::fill( &a_.pad[0], &a_.pad[0]+sizeof(a_.pad), 0);
			std::fill( &b_.pad[0], &b_.pad[0]+sizeof(b_.pad), 0);
			return std::equal((uint8_t *)&a_, ((uint8_t *)&a_)+sizeof(struct ofp10_action_vlan_pcp), (uint8_t *)&b_);
			} break;
		case OFP10AT_SET_DL_SRC:
		case OFP10AT_SET_DL_DST: {
			struct ofp10_action_dl_addr a_ = *(struct ofp10_action_dl_addr*)a.soaction();
			struct ofp10_action_dl_addr b_ = *(struct ofp10_action_dl_addr*)b.soaction();
			// copies of structs are made, instead of using referenced pointers to original, so that the padding can be zeroed for a safe comparison
			std::fill( &a_.pad[0], &a_.pad[0]+sizeof(a_.pad), 0);
			std::fill( &b_.pad[0], &b_.pad[0]+sizeof(b_.pad), 0);
			return std::equal((uint8_t *)&a_, ((uint8_t *)&a_)+sizeof(struct ofp10_action_dl_addr), (uint8_t *)&b_);
			} break;
		case OFP10AT_SET_NW_TOS: {
			struct ofp10_action_nw_tos a_ = *(struct ofp10_action_nw_tos*)a.soaction();
			struct ofp10_action_nw_tos b_ = *(struct ofp10_action_nw_tos*)b.soaction();
			// copies of structs are made, instead of using referenced pointers to original, so that the padding can be zeroed for a safe comparison
			std::fill( &a_.pad[0], &a_.pad[0]+sizeof(a_.pad), 0);
			std::fill( &b_.pad[0], &b_.pad[0]+sizeof(b_.pad), 0);
			return std::equal((uint8_t *)&a_, ((uint8_t *)&a_)+sizeof(struct ofp10_action_nw_tos), (uint8_t *)&b_);
		} break;
		case OFP10AT_SET_TP_SRC:
		case OFP10AT_SET_TP_DST: {
			struct ofp10_action_tp_port a_ = *(struct ofp10_action_tp_port*)a.soaction();
			struct ofp10_action_tp_port b_ = *(struct ofp10_action_tp_port*)b.soaction();
			// copies of structs are made, instead of using referenced pointers to original, so that the padding can be zeroed for a safe comparison
			std::fill( &a_.pad[0], &a_.pad[0]+sizeof(a_.pad), 0);
			std::fill( &b_.pad[0], &b_.pad[0]+sizeof(b_.pad), 0);
			return std::equal((uint8_t *)&a_, ((uint8_t *)&a_)+sizeof(struct ofp10_action_tp_port), (uint8_t *)&b_);
		} break;
		case OFP10AT_ENQUEUE: {
			struct ofp10_action_enqueue a_ = *(struct ofp10_action_enqueue*)a.soaction();
			struct ofp10_action_enqueue b_ = *(struct ofp10_action_enqueue*)b.soaction();
			// copies of structs are made, instead of using referenced pointers to original, so that the padding can be zeroed for a safe comparison
			std::fill( &a_.pad[0], &a_.pad[0]+sizeof(a_.pad), 0);
			std::fill( &b_.pad[0], &b_.pad[0]+sizeof(b_.pad), 0);
			return std::equal((uint8_t *)&a_, ((uint8_t *)&a_)+sizeof(struct ofp10_action_enqueue), (uint8_t *)&b_);
		} break;
		default:
			std::cout << __FUNCTION__ << " was asked to compare unknown action type." << std::endl;
			assert(false);
	}
	assert (false);	// should never get here.
	return false;
}

}	// namepsace std


