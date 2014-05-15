
#include "csh_flow_stats.h"

morpheus::csh_flow_stats::csh_flow_stats(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_flow_stats_request * const msg):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_flow_stats_request(src, msg);
	}

bool morpheus::csh_flow_stats::process_flow_stats_request ( rofl::cofctl * const src, rofl::cofmsg_flow_stats_request * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	const cportvlan_mapper & mapper = m_parent->get_mapper();
	m_request_xid = msg->get_xid();
	rofl::cofmatch newmatch = msg->get_flow_stats().get_match();
	rofl::cofmatch oldmatch = newmatch;

std::cout << "TP" << __LINE__ << std::endl;
	//check that VLANs are not being matched on
	// TODO we *could*, but we don't, support incoming VLAN iff they are coming in on an port-translated-only port (i.e. a virtual port that doesn't map to a port+vlan, only a phsyical port), and that VLAN si then stripped in the action.
	try {
		oldmatch.get_vlan_vid_mask();	// ignore result - we only care if it'll throw
//		if(oldmatch.get_vlan_vid_mask() != 0xffff) {
			std::cout << __FUNCTION__ << ": received a match which didn't have VLAN wildcarded. Replying with blank stats response. match:" << oldmatch.c_str() << std::endl;
			m_parent->send_flow_stats_reply( src, m_request_xid, std::vector< rofl::cofflow_stats_reply > (), false );
			m_completed = true;
			return m_completed;
//		}
	} catch ( rofl::eOFmatchNotFound & ) {
		// do nothing - there was no vlan_vid_mask
	}
std::cout << "TP" << __LINE__ << std::endl;
	// make sure this is a valid port
	// TODO check whether port is ANY/ALL
	uint32_t old_inport = oldmatch.get_in_port();
std::cout << "TP" << __LINE__ << std::endl;
	try {
		cportvlan_mapper::port_spec_t real_port = mapper.get_actual_port( old_inport ); // could throw std::out_of_range
		if(!real_port.vlanid_is_none()) {
			// vlan is set in actual port - update the match
			newmatch.set_vlan_vid( real_port.vlan );
		}
		// update port
		newmatch.set_in_port( real_port.port );
std::cout << "TP" << __LINE__ << std::endl;
	} catch (std::out_of_range &) {
		std::cout << __FUNCTION__ << ": received a match request for an unknown port (" << old_inport << "). There are " << mapper.get_number_virtual_ports() << " ports.  Sending null reply message. match:" << oldmatch.c_str() << std::endl;
		m_parent->send_flow_stats_reply( src, m_request_xid, std::vector< rofl::cofflow_stats_reply > (), false );
		m_completed = true;
		return m_completed;
	}
	rofl::cofflow_stats_request flows_stats_req = msg->get_flow_stats();
	uint32_t new_outport, old_outport = flows_stats_req.get_out_port();
//	*** TODO update match according to new_outport
	rofl::cofflow_stats_request req(OFP10_VERSION, newmatch, flows_stats_req.get_table_id(), new_outport);
	uint32_t newxid = m_parent->send_flow_stats_request(m_parent->get_dpt(), msg->get_stats_flags(), req ); // TODO is get_stats_flags correct ??

	if( ! m_parent->associate_xid( true, m_request_xid, this ) ) std::cout << "Problem associating ctl xid in " << __FUNCTION__ << std::endl;
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	return m_completed;
}

bool morpheus::csh_flow_stats::process_flow_stats_reply ( rofl::cofdpt * const src, rofl::cofmsg_flow_stats_reply * const msg ) {
	assert(!m_completed);
	const cportvlan_mapper & mapper = m_parent->get_mapper();
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	rofl::cofdesc_stats_reply reply(src->get_version(),"morpheus_mfr_desc","morpheus_hw_desc","morpheus_sw_desc","morpheus_serial_num","morpheus_dp_desc");
	m_parent->send_desc_stats_reply(m_parent->get_ctl(), m_request_xid, reply, false );
	m_completed = true;
	return m_completed;
}

morpheus::csh_flow_stats::~csh_flow_stats() { std::cout << __FUNCTION__ << " called." << std::endl; }

std::string morpheus::csh_flow_stats::asString() const { std::stringstream ss; ss << "csh_flow_stats {request_xid=" << m_request_xid << "}"; return ss.str(); }
