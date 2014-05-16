
#ifndef UCL_EE_MORPHEUS_H
#define UCL_EE_MORPHEUS_H

#include <sstream>
#include <string>
#include <map>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/openflow10.h>
#include <pthread.h>
#include <rofl/common/thread_helper.h>
#include "cportvlan_mapper.h"

std::string action_mask_to_string(const uint32_t action_types);
std::string capabilities_to_string(const uint32_t capabilities);
std::string port_as_string(uint16_t p);

bool operator==(const rofl::cofaclist & a, const rofl::cofaclist & b);
bool operator==(const rofl::cofaction & a, const rofl::cofaction & b);
	
class morpheus : public rofl::crofbase {

public:

// private:
// forward declare our nested classes which will encapsulate the transaction (composed of persistent data etc)
	class chandlersession_base;
	class csh_flow_mod;
	class csh_features_request;
	class csh_get_config;
	class csh_desc_stats;
	class csh_table_stats;
	class cport_stats_session;
	class csh_flow_stats;
	class csh_aggregate_stats;
	class cqueue_stats_session;
	class csh_packet_in;
	class csh_packet_out;
	class csh_packet_in;
	class csh_barrier;
	class csh_table_mod;
	class csh_port_mod;
	class csh_set_config;

protected:

// typedef boost::shared_ptr<chandlersession_base> session_ptr_t;
// typedef std::map < uint32_t, session_ptr_t > xid_session_map_t;
/// typedef std::map < uint32_t, chandlersession_base * > xid_session_map_t;
/// typedef std::map < chandlersession_base *, std::vector< std::pair< bool, uint32_t > > > xid_reverse_session_map_t;	// vector holds bool (true if ctl xid, false if dpt xid) and xid to do the reverse xid association with the session
/// xid_session_map_t m_ctl_sessions;	// TODO make MT safe
/// xid_session_map_t m_dpt_sessions;	// TODO make MT safe
typedef std::map < std::pair< bool, uint32_t >, chandlersession_base * > xid_session_map_t;	// if bool is true then the xid is an ctl xid, it's a dpt xid otherwise.
// typedef std::multimap < chandlersession_base *, std::pair< bool, uint32_t >  > xid_reverse_session_map_t;	// if bool is true then the xid is an ctl xid, it's a dpt xid otherwise.
xid_session_map_t m_sessions;
mutable pthread_rwlock_t m_sessions_lock;	// a lock for m_sessions
// xid_reverse_session_map_t m_reverse_sessions;

struct port_config_t {
	
};
std::map<uint16_t, port_config_t> port_enabled;
rofl::cofdpt * m_slave;		// the datapath device that we'll be misrepresenting
rofl::cofctl * m_master;	// the OF controller.
cportvlan_mapper m_mapper;
uint64_t m_slave_dpid;
uint64_t m_dpid;
const uint32_t m_supported_features;
const uint32_t m_supported_actions_mask;
uint32_t m_supported_actions;
uint32_t m_dpe_supported_actions;
bool m_dpe_supported_actions_valid;

// std::map<rofl::cofaclist, rofl::cofaclist> action_map;	// TODO - this isn't properly managed - it's hard to know the lifetime of these so we don;t remove them at the moment - we should
// std::map<rofl::cofmatch, rofl::cofmatch> match_map;	// TODO - this isn't properly managed - it's hard to know the lifetime of these so we don;t remove them at the moment - we should

struct flowentry {
	rofl::cofmatch match;
	rofl::cofaclist actions;
	uint64_t cookie;
	uint16_t idle_timeout;
	uint16_t hard_timeout;
	uint16_t priority;
	uint16_t flags;
	bool match_is_set;
	bool actions_is_set;
	bool cookie_is_set;
	bool idle_timeout_is_set;
	bool hard_timeout_is_set;
	bool priority_is_set;
	bool flags_is_set;
	flowentry(rofl::cofmatch & match_):match_is_set(true),actions_is_set(false),cookie_is_set(false),idle_timeout_is_set(false),hard_timeout_is_set(false),priority_is_set(false),flags_is_set(false) {}
	flowentry & set_match(const rofl::cofmatch & match_) { match_is_set=true; match=match_; return *this; }
	flowentry & set_actions(const rofl::cofaclist & actions_) { actions_is_set=true; actions=actions_; return *this; }
	flowentry & set_cookie(const uint64_t cookie_) { cookie_is_set=true; cookie=cookie_; return *this; }
	flowentry & set_idle_timeout(const uint16_t idle_timeout_) { idle_timeout_is_set=true; idle_timeout=idle_timeout_; return *this; }
	flowentry & set_hard_timeout(const uint16_t hard_timeout_) { hard_timeout_is_set=true; hard_timeout=hard_timeout_; return *this; }
	flowentry & set_priority(const uint16_t priority_) { priority_is_set=true; priority=priority_; return *this; }
	flowentry & set_flags(const uint16_t flags_) { flags_is_set=true; flags=flags_; return *this; }
	flowentry & unset_actions() { actions_is_set=false; return *this; }
	flowentry & unset_cookie() { cookie_is_set=false; return *this; }
	flowentry & unset_idle_timeout() { idle_timeout_is_set=false; return *this; }
	flowentry & unset_hard_timeout() { hard_timeout_is_set=false; return *this; }
	flowentry & unset_priority() { priority_is_set=false; return *this; }
	flowentry & unset_flags() { flags_is_set=false; return *this; }
	bool operator==(flowentry &b) { return ( \
		( (match_is_set & b.match_is_set)?(match==b.match):true ) && \
		( (actions_is_set & b.actions_is_set)?(actions==b.actions):true ) && \
		( (cookie_is_set & b.cookie_is_set)?(cookie==b.cookie):true ) && \
		( (idle_timeout_is_set & b.idle_timeout_is_set)?(idle_timeout==b.idle_timeout):true ) && \
		( (hard_timeout_is_set & b.hard_timeout_is_set)?(hard_timeout==b.hard_timeout):true ) && \
		( (priority_is_set & b.priority_is_set)?(priority==b.priority):true ) && \
		( (flags_is_set & b.flags_is_set)?(flags==b.flags):true ) );
	}
		// TODO is cofmatch::contains the right call? Don;t bother check the docs - there aren't any
	bool wildcard_match(flowentry &b) { return ( \
		( (match_is_set & b.match_is_set)?match.contains(b.match):true ) && \
		( (actions_is_set & b.actions_is_set)?(actions==b.actions):true ) && \
		( (cookie_is_set & b.cookie_is_set)?(cookie==b.cookie):true ) && \
		( (idle_timeout_is_set & b.idle_timeout_is_set)?(idle_timeout==b.idle_timeout):true ) && \
		( (hard_timeout_is_set & b.hard_timeout_is_set)?(hard_timeout==b.hard_timeout):true ) && \
		( (priority_is_set & b.priority_is_set)?(priority==b.priority):true ) && \
		( (flags_is_set & b.flags_is_set)?(flags==b.flags):true ) );
	}

	};

//std::map< morpheus::flowentry, morpheus::flowentry > m_flowentry_db;	// value could also be std::vector<morpheus::flowentry> or this could just be a multimap
typedef std::vector< std::pair<morpheus::flowentry, morpheus::flowentry> > flowentry_db_t;
flowentry_db_t m_flowentry_db;

bool indpt, inctl;
rofl::caddress dptaddr, ctladdr;

void init_dpe();

// uint32_t set_supported_actions (uint32_t new_actions);
void set_supported_dpe_features (uint32_t new_capabilities, uint32_t new_actions);

// crofbase overrides
	virtual void handle_dpath_open (rofl::cofdpt *);
	virtual void handle_dpath_close (rofl::cofdpt *);
	virtual void handle_ctrl_open (rofl::cofctl *);
	virtual void handle_ctrl_close (rofl::cofctl *);
	virtual void handle_features_request(rofl::cofctl *ctl, rofl::cofmsg_features_request * msg );
	virtual void handle_features_reply(rofl::cofdpt * dpt, rofl::cofmsg_features_reply * msg );
	virtual void handle_error (rofl::cofdpt *, rofl::cofmsg *msg);
	virtual void handle_get_config_request(rofl::cofctl *ctl, rofl::cofmsg_get_config_request * msg );
	virtual void handle_get_config_reply(rofl::cofdpt * dpt, rofl::cofmsg_get_config_reply * msg );
	
	

	virtual void handle_desc_stats_request(rofl::cofctl *ctl, rofl::cofmsg_desc_stats_request * msg );
	virtual void handle_desc_stats_reply(rofl::cofdpt * dpt, rofl::cofmsg_desc_stats_reply * msg );
	
	virtual void handle_table_stats_request(rofl::cofctl *ctl, rofl::cofmsg_table_stats_request * msg );
	virtual void handle_table_stats_reply(rofl::cofdpt *dpt, rofl::cofmsg_table_stats_reply * msg );

	virtual void handle_set_config(rofl::cofctl *ctl, rofl::cofmsg_set_config * msg );

	virtual void handle_port_stats_request(rofl::cofctl *ctl, rofl::cofmsg_port_stats_request * msg );
	virtual void handle_flow_stats_request(rofl::cofctl *ctl, rofl::cofmsg_flow_stats_request * msg );
	virtual void handle_aggregate_stats_request(rofl::cofctl *ctl, rofl::cofmsg_aggr_stats_request * msg );
	virtual void handle_aggregate_stats_reply(rofl::cofdpt *dpt, rofl::cofmsg_aggr_stats_reply * msg );
	
	virtual void handle_queue_stats_request(rofl::cofctl *ctl, rofl::cofmsg_queue_stats_request * msg );
	virtual void handle_experimenter_stats_request(rofl::cofctl *ctl, rofl::cofmsg_stats_request * msg );
	virtual void handle_packet_in(rofl::cofdpt *dpt, rofl::cofmsg_packet_in * msg); 	
	virtual void handle_packet_out(rofl::cofctl *ctl, rofl::cofmsg_packet_out * msg );
	virtual void handle_barrier_request(rofl::cofctl *ctl, rofl::cofmsg_barrier_request * msg );
	virtual void handle_barrier_reply ( rofl::cofdpt * dpt, rofl::cofmsg_barrier_reply * msg );
	// virtual void handle_table_mod(rofl::cofctl *ctl, rofl::cofmsg_table_mod * msg );
	virtual void handle_port_mod(rofl::cofctl *ctl, rofl::cofmsg_port_mod * msg );
	virtual void handle_queue_get_config_request(rofl::cofctl *ctl, rofl::cofmsg_queue_get_config_request * msg );
	virtual void handle_experimenter_message(rofl::cofctl *ctl, rofl::cofmsg_features_request * msg );
	virtual void handle_flow_mod(rofl::cofctl *ctl, rofl::cofmsg_flow_mod * msg );
// timeout methods?
	virtual void handle_timeout ( int opaque );
	virtual void handle_error ( rofl::cofdpt * src, rofl::cofmsg_error * msg );

public:
// our transaction management methods - they are public because the nested classes have to call them
	bool associate_xid( bool ctl_or_dpt_xid, const uint32_t new_xid, chandlersession_base * p );	// tells the translator that new_xid is an xid related to session_xid which is the xid of the original message that invoked the session - returns true if the session_xid was found and the association was made. false if session_xid not found ir new_xid already exists in m_ctl_sessions
	bool remove_xid_association( bool ctl_or_dpt_xid, const uint32_t xid );			// called to remove the association of the xid with a session_base - returns true if session_xid was found and removed, false otherwise
	unsigned remove_session( chandlersession_base * p );	// called to remove all associations to this session_base - returns the number of associations removed
	rofl::cofdpt * get_dpt() const;
	rofl::cofctl * get_ctl() const;
	uint64_t get_dpid() const { return m_dpid; }
	morpheus(const cportvlan_mapper & mapper, bool indpt, rofl::caddress dptaddr, bool inctl, rofl::caddress ctladdr);	// if indpt is true then morpheus will listen on dtpaddr, otherwise it will connect to it.
	virtual ~morpheus();
	const cportvlan_mapper & get_mapper() const { return m_mapper; }
/*
	bool add_flowmod_action_translation(const rofl::cofaclist & virt, const rofl::cofaclist & act);	// returns true if the new action overwrote an old entry
	bool remove_flowmod_action_translation(const rofl::cofaclist & virt);
//	void get_flowmod_action_translation(bool virtual_to_actual, const rofl::cofaclist & virt_or_act) const;
	rofl::cofaclist get_flowmod_action_translation(bool virtual_to_actual, const rofl::cofaclist & virt_or_act) const;

	bool add_flowmod_match_translation(const rofl::cofmatch & virt, const rofl::cofmatch & act);	// returns true if the new action overwrote an old entry
	bool remove_flowmod_match_translation(const rofl::cofmatch & virt);
	rofl::cofmatch get_flowmod_match_translation(bool virtual_to_actual, const rofl::cofmatch & virt_or_act) const;
*/
/*	std::vector<morpheus::flowentry> getTranslatedFlowentry (rofl::cofmatch matchspec);	// returns empty vector if no matches
	morpheus::flowentry getExactTranslatedFlowentry (rofl::cofmatch matchspec);	// throws std::out_of_range if not found
	std::vector<morpheus::flowentry> getUnTranslatedFlowentry (rofl::cofmatch matchspec); // returns empty vector if no matches
	morpheus::flowentry getExactUnTranslatedFlowentry (rofl::cofmatch matchspec); // throws std::out_of_range if not found
	bool addFlowentryTranslation ( const morpheus::flowentry & untranslated, const morpheus::flowentry & translated );	// return true if added, false if such an untranslated entry already exists, and then doesn't overwrite
	bool removeFlowentryTranslation ( const morpheus::flowentry & untranslated );	// return true if removed, false if not found.	- if you want wildcarded remove on match then use alongside getTranslatedFlowentry
*/
	std::vector<morpheus::flowentry> getTranslatedFlowentry (const morpheus::flowentry & untranslated_flowentry);
	morpheus::flowentry getExactTranslatedFlowentry (const morpheus::flowentry & untranslated_matchspec);
	std::vector<morpheus::flowentry> getUnTranslatedFlowentry (const morpheus::flowentry & translated_matchspec);
	morpheus::flowentry getExactUnTranslatedFlowentry (const morpheus::flowentry & translated_matchspec);
	bool addFlowentryTranslation ( const morpheus::flowentry & untranslated, const morpheus::flowentry & translated );
	bool removeFlowentryTranslation ( const morpheus::flowentry & untranslated );


uint32_t get_supported_actions();
uint32_t get_supported_features() { return m_supported_features; }


std::string dump_sessions() const;
std::string dump_config() const;
friend std::ostream & operator<< (std::ostream & os, const morpheus & morph);
// friend chandlersession_base;

};

#include "csh_aggregate_stats.h"
#include "csh_barrier.h"
#include "csh_desc_stats.h"
#include "csh_features_request.h"
#include "csh_flow_mod.h"
#include "csh_flow_stats.h"
#include "csh_get_config.h"
#include "csh_packet_in.h"
#include "csh_packet_out.h"
#include "csh_port_mod.h"
#include "csh_set_config.h"
#include "csh_table_mod.h"
#include "csh_table_stats.h"
#include "morpheus_nested.h"

#endif // UCL_EE_MORPHEUS_H

