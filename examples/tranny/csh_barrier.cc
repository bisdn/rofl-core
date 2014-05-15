
#include "csh_barrier.h"

morpheus::csh_barrier::csh_barrier(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_barrier_request * const msg):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_barrier_request(src, msg);
	}

bool morpheus::csh_barrier::process_barrier_request ( const rofl::cofctl * const src, const rofl::cofmsg_barrier_request * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_request_xid = msg->get_xid();
	uint32_t newxid = m_parent->send_barrier_request( m_parent->get_dpt() );
	if( ! m_parent->associate_xid( true, m_request_xid, this ) ) std::cout << "Problem associating ctl xid in " << __FUNCTION__ << std::endl;
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	return m_completed;
}

bool morpheus::csh_barrier::process_barrier_reply ( const rofl::cofdpt * const src, rofl::cofmsg_barrier_reply * const msg ) {
	assert(!m_completed);
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_parent->send_barrier_reply(m_parent->get_ctl(), m_request_xid );
	m_completed = true;
	return m_completed;
}

morpheus::csh_barrier::~csh_barrier() { std::cout << __FUNCTION__ << " called." << std::endl; }

std::string morpheus::csh_barrier::asString() const { std::stringstream ss; ss << "csh_barrier {request_xid=" << m_request_xid << "}"; return ss.str(); }

