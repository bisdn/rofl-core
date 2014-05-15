
#include "csh_table_mod.h"

// TODO translation check

morpheus::csh_table_mod::csh_table_mod(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_table_mod * const msg ):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_table_mod(src, msg);
	}

bool morpheus::csh_table_mod::process_table_mod ( rofl::cofctl * const src, rofl::cofmsg_table_mod * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_parent->send_table_mod_message( m_parent->get_dpt(), msg->get_table_id(), msg->get_config() );
	m_completed = true;
	return m_completed;
}

morpheus::csh_table_mod::~csh_table_mod() { std::cout << __FUNCTION__ << " called." << std::endl; }	// nothing to do as we didn't register anywhere.

std::string morpheus::csh_table_mod::asString() const { return "csh_table_mod {no xid}"; }
