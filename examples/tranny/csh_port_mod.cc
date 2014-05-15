
#include "csh_port_mod.h"

// TODO translation check
morpheus::csh_port_mod::csh_port_mod(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_port_mod * const msg ):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_port_mod(src, msg);
	}

bool morpheus::csh_port_mod::process_port_mod ( rofl::cofctl * const src, rofl::cofmsg_port_mod * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_parent->send_port_mod_message( m_parent->get_dpt(), msg->get_port_no(), msg->get_hwaddr(), msg->get_config(), msg->get_mask(), msg->get_advertise() );
	m_completed = true;
	return m_completed;
}

morpheus::csh_port_mod::~csh_port_mod() { std::cout << __FUNCTION__ << " called." << std::endl; }	// nothing to do as we didn't register anywhere.

std::string morpheus::csh_port_mod::asString() const { return "csh_port_mod {no xid}"; }
