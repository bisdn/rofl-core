
#ifndef UCL_EE_MORPHEUS_H
#define UCL_EE_MORPHEUS_H

// #include <boost/shared_ptr.hpp>
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

class morpheus : public rofl::crofbase {

public:

private:
// forward declare our nested classes which will encapsulate the transaction (composed of persistent data etc)
	class chandlersession_base;
	class cflow_mod_session;
	class cfeatures_request_session;
	class cget_config_session;
	class cdesc_stats_session;
	class ctable_stats_session;
	class cport_stats_session;
	class cflow_stats_session;
	class caggregate_stats_session;
	class cqueue_stats_session;
	class cpacket_in_session;
	class cpacket_out_session;
	class cpacket_in_session;
	class cbarrier_session;
	class ctable_mod_session;
	class cport_mod_session;
	class cset_config_session;

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
cportvlan_mapper m_mapper;
rofl::cofdpt * m_slave;		// the datapath device that we'll be misrepresenting
rofl::cofctl * m_master;	// the OF controller.
uint64_t m_slave_dpid;
uint64_t m_dpid;
const uint32_t m_supported_actions_mask;
uint32_t m_supported_actions;
bool m_supported_actions_valid;
const uint32_t m_supported_features;

// uint32_t set_supported_actions (uint32_t new_actions);
// void set_supported_features (uint32_t new_capabilities, uint32_t new_actions);

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
	virtual void handle_table_mod(rofl::cofctl *ctl, rofl::cofmsg_table_mod * msg );
	virtual void handle_port_mod(rofl::cofctl *ctl, rofl::cofmsg_port_mod * msg );
	virtual void handle_queue_get_config_request(rofl::cofctl *ctl, rofl::cofmsg_queue_get_config_request * msg );
	virtual void handle_experimenter_message(rofl::cofctl *ctl, rofl::cofmsg_features_request * msg );
	virtual void handle_flow_mod(rofl::cofctl *ctl, rofl::cofmsg_flow_mod * msg );

public:
// our transaction management methods - they are public because the nested classes have to call them
	bool associate_xid( bool ctl_or_dpt_xid, const uint32_t new_xid, chandlersession_base * p );	// tells the translator that new_xid is an xid related to session_xid which is the xid of the original message that invoked the session - returns true if the session_xid was found and the association was made. false if session_xid not found ir new_xid already exists in m_ctl_sessions
	bool remove_xid_association( bool ctl_or_dpt_xid, const uint32_t xid );			// called to remove the association of the xid with a session_base - returns true if session_xid was found and removed, false otherwise
	unsigned remove_session( chandlersession_base * p );	// called to remove all associations to this session_base - returns the number of associations removed
	rofl::cofdpt * get_dpt() const;
	rofl::cofctl * get_ctl() const;
	uint64_t get_dpid() const { return m_dpid; }
	morpheus(const cportvlan_mapper & mapper);
	virtual ~morpheus();
	const cportvlan_mapper & get_mapper() const { return m_mapper; }

uint32_t get_supported_actions();
uint32_t get_supported_features() { return m_supported_features; }


std::string dump_sessions() const;
std::string dump_config() const;
friend std::ostream & operator<< (std::ostream & os, const morpheus & morph);
friend chandlersession_base;

};

#include "morpheus_nested.h"


#endif // UCL_EE_MORPHEUS_H

