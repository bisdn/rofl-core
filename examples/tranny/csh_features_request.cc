
#include "csh_features_request.h"
#include <rofl/common/openflow/openflow_rofl_exceptions.h>
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/cerror.h>
#include <rofl/common/openflow/cofaction.h>


morpheus::csh_features_request::csh_features_request(morpheus * parent):chandlersession_base(parent),m_local_request(true) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	uint32_t newxid = m_parent->send_features_request( m_parent->get_dpt() );
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	}

morpheus::csh_features_request::csh_features_request(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_features_request * const msg):chandlersession_base(parent),m_local_request(false) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_features_request(src, msg);
	}

bool morpheus::csh_features_request::process_features_request ( const rofl::cofctl * const src, const rofl::cofmsg_features_request * const msg ) {
//	if(msg->get_version() != OFP10_VERSION) { std::stringstream ss; ss << "Bad OF version packet received in " << __FUNCTION__; throw rofl::eBadVersion( ss.str() ); }
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_request_xid = msg->get_xid();
	uint32_t newxid = m_parent->send_features_request( m_parent->get_dpt() );
	if( ! m_parent->associate_xid( true, m_request_xid, this ) ) std::cout << "Problem associating ctl xid in " << __FUNCTION__ << std::endl;
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	return m_completed;
}

bool morpheus::csh_features_request::process_features_reply ( const rofl::cofdpt * const src, rofl::cofmsg_features_reply * const msg ) {
	assert(!m_completed);
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	if(m_local_request) {
		push_features( msg->get_capabilities(), msg->get_actions_bitmap() );
		m_completed = true;
	} else {
		uint64_t dpid = m_parent->get_dpid() + 1;		// TODO get this from config file
		uint32_t capabilities = msg->get_capabilities();
		// first check whether we have the ones we need, then rewrite them anyway
		std::cout << __FUNCTION__ << ": capabilities of DPE reported as:" << capabilities_to_string(capabilities) << std::endl;
		uint32_t of10_actions_bitmap = msg->get_actions_bitmap();	// 	TODO ofp10 only
		std::cout << __FUNCTION__ << ": supported actions of DPE reported as:" << action_mask_to_string(of10_actions_bitmap) << std::endl;
		push_features( capabilities, of10_actions_bitmap );

		capabilities = m_parent->get_supported_features();	// hard coded to return 0 - No stats, STP or the other magic.
				
		rofl::cofportlist realportlist = msg->get_ports();
		// check whether all the ports we're using are actually supported by the DPE
		
		rofl::cofportlist virtualportlist;

		// TODO this is hardcoded for testing. Need to implement better support for config in morpheus, then just grabbed the translated and validated (based on underlying switch) config from there
		const cportvlan_mapper & mapper = m_parent->get_mapper();
		for(unsigned portno = 1; portno <= mapper.get_number_virtual_ports(); ++portno) {
			cportvlan_mapper::port_spec_t vport = mapper.get_actual_port(portno);
			rofl::cofport p(OFP10_VERSION);
			p.set_config(OFP10PC_NO_STP);
			uint32_t feats = OFP10PF_10GB_FD | OFP10PF_FIBER;
			p.set_peer (feats);
			p.set_curr (feats);
			p.set_advertised (feats);
			p.set_supported (feats);
			p.set_state(0);	// link is up and ignoring STP
			p.set_port_no(portno);
			p.set_hwaddr(rofl::cmacaddr("00:B1:6B:00:B1:E5"));	// This is wrong - it should follow the actual MAC
			std::stringstream ss;
			ss << "V_" << vport;
			p.set_name(std::string(ss.str()));
			virtualportlist.next() = p;
			}
		m_parent->send_features_reply(m_parent->get_ctl(), m_request_xid, dpid, msg->get_n_buffers(), msg->get_n_tables(), capabilities, 0, of10_actions_bitmap, virtualportlist );	// TODO get_action_bitmap is OF1.0 only
		m_completed = true;
	}
	return m_completed;
}

morpheus::csh_features_request::~csh_features_request() { std::cout << __FUNCTION__ << " called." << std::endl; }

std::string morpheus::csh_features_request::asString() const { std::stringstream ss; ss << "csh_features_request {request_xid=" << m_request_xid << "}"; return ss.str(); }

