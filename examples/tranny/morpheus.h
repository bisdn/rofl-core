
#ifndef UCL_EE_MORPHEUS_H
#define UCL_EE_MORPHEUS_H

#include <boost/shared_ptr.hpp>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/openflow10.h>
#include "cportvlan_mapper.h"

class morpheus : public rofl::crofbase {

public:
class chandlersession_base;

protected:

typedef boost::shared_ptr<chandlersession_base> session_ptr_t;
typedef std::map < uint32_t, session_ptr_t > xid_session_map_t;
xid_session_map_t m_ctl_sessions;	// TODO make MT safe
xid_session_map_t m_dpt_sessions;	// TODO make MT safe
cportvlan_mapper m_mapper;
rofl::cofdpt * m_slave;		// the datapath device that we'll be misrepresenting
rofl::cofctl * m_master;	// the OF controller.
uint64_t m_slave_dpid;
uint64_t m_dpid;

// crofbase overrides
	virtual void handle_dpath_open (rofl::cofdpt *);
	virtual void handle_dpath_close (rofl::cofdpt *);
	virtual void handle_ctrl_open (rofl::cofctl *);
	virtual void handle_ctrl_close (rofl::cofctl *);
	virtual void handle_features_request(rofl::cofctl *ctl, rofl::cofmsg_features_request * msg );
	virtual void handle_features_reply(rofl::cofdpt * dpt, rofl::cofmsg_features_reply * msg );
	virtual void handle_error (rofl::cofdpt *, rofl::cofmsg *msg);
/*	virtual void handle_get_config_request(rofl::cofctl *ctl, rofl::cofmsg_get_config_request * msg );
	virtual void handle_get_config_reply(rofl::cofdpt * dpt, rofl::cofmsg_get_config_reply * msg );
	
	

	virtual void handle_desc_stats_request(rofl::cofctl *ctl, rofl::cofmsg_desc_stats_request * msg );
	virtual void handle_desc_stats_reply(rofl::cofdpt * dpt, rofl::cofmsg_desc_stats_reply * msg );
	
	virtual void handle_table_stats_request(rofl::cofctl *ctl, rofl::cofmsg_table_stats_request * msg );
	virtual void handle_table_stats_reply(rofl::cofdpt *dpt, rofl::cofmsg_table_stats_reply * msg );
	
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
	virtual void handle_set_config(rofl::cofctl *ctl, rofl::cofmsg_set_config * msg );
	virtual void handle_queue_get_config_request(rofl::cofctl *ctl, rofl::cofmsg_queue_get_config_request * msg );
	virtual void handle_experimenter_message(rofl::cofctl *ctl, rofl::cofmsg_features_request * msg ); */
	virtual void handle_flow_mod(rofl::cofctl *ctl, rofl::cofmsg_flow_mod * msg );

public:
// our transaction management methods - they are public because the nested classes have to call them
	// bool ctl_session_attach( uint32_t session_xid, uint32_t new_xid );	// tells the translator that new_xid is an xid related to session_xid which is the xid of the original message that invoked the session
	bool associate_ctl_xid( const uint32_t session_xid, const uint32_t new_xid );	// tells the translator that new_xid is an xid related to session_xid which is the xid of the original message that invoked the session - returns true if the session_xid was found and the association was made. false if session_xid not found ir new_xid already exists in m_ctl_sessions
	bool associate_dpt_xid( const uint32_t session_xid, const uint32_t new_xid );
	bool add_ctl_session( const uint32_t session_xid, const session_ptr_t session_ptr );	// returns true if it was added successfully, false 
	bool add_dpt_session( const uint32_t session_xid, const session_ptr_t session_ptr );
	bool remove_dpt_association( const uint32_t xid );			// called to remove the association of the xid with a session_base - returns true if session_xid was found and removed, false otherwise
	unsigned remove_dpt_session( session_ptr_t session_ptr );	// called to remove all associations to this session_base - returns the number of associations removed
	bool remove_ctl_association( const uint32_t xid );
	unsigned remove_ctl_session( session_ptr_t session_ptr );
//	bool process_impl ( const rofl::cofdpt * const src, const rofl::cofmsg * const msg);
//	bool process_impl ( const rofl::cofctl * const src, const rofl::cofmsg * const msg);
	rofl::cofdpt * get_dpt() const;
	rofl::cofctl * get_ctl() const;
	uint64_t get_dpid() const { return m_dpid; }
	morpheus(const cportvlan_mapper & mapper);
	virtual ~morpheus();

private:
// forward declare our nested classes which will encapsulate the transaction (persistent data etc)
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

std::string dump_sessions() const {	// TODO
}

std::string dump_config() const {	// TODO
}

friend std::ostream & operator<< (std::ostream & os, const morpheus & morph) {
	os << "cport_vlan_translator configuration:\n" << morph.dump_config();
	os << "cport_vlan_translator current sessions:" << morph.dump_sessions() << std::endl;
	return os;
	}

/// TODO bool set_virtual_port_config(...);	// returns true if this overwrote an existing port

protected:
// make_message_handler ( rofl::cxidowner * m_originator, rofl::cofmsg * m_msg )
// cmessage_handler make_message_handler ( uint8_t msg_type )	// throws rofl::eBadRequestBadType

};

#endif // UCL_EE_MORPHEUS_H

