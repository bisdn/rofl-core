// nested classes

#ifndef UCL_EE_MORPHEUS_NESTED_H
#define UCL_EE_MORPHEUS_NESTED_H

#include <sstream>
#include <string>
#include <rofl/common/openflow/openflow_rofl_exceptions.h>
#include <rofl/common/cerror.h>

class morpheus;

void dumpBytes (std::ostream & os, uint8_t * bytes, size_t n_bytes);
/*
// print out a hexdump of bytes
void dumpBytes (std::ostream & os, uint8_t * bytes, size_t n_bytes) {
	if (0==n_bytes) return;
	for(size_t i = 0; i < (n_bytes-1); ++i) printf("%02x ", bytes[i]);
	printf("%02x", bytes[n_bytes-1]);
}
*/
class morpheus::chandlersession_base {
protected:
morpheus * m_parent;
bool m_completed;
chandlersession_base( morpheus * parent ):m_parent(parent),m_completed(false) {}

public:
virtual std::string asString() { return "**chandlersession_base**"; }
virtual bool isCompleted() { return m_completed; }	// returns true if the session has finished its work and shouldn't be kept alive further
virtual ~chandlersession_base() { std::cout << __FUNCTION__ << " called. Session was " << (m_completed?"":"NOT ") << "completed." << std::endl; }
};

class morpheus::cflow_mod_session : public morpheus::chandlersession_base {
public:
cflow_mod_session(morpheus * parent, const rofl::cofctl * const src, rofl::cofmsg_flow_mod * const msg ):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_flow_mod(src, msg);
	}
bool process_flow_mod ( const rofl::cofctl * const src, rofl::cofmsg_flow_mod * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	rofl::cflowentry entry(OFP10_VERSION);
	entry.set_command(msg->get_command());
	entry.set_idle_timeout(msg->get_idle_timeout());
	entry.set_hard_timeout(msg->get_hard_timeout());
	entry.set_cookie(msg->get_cookie());
	entry.set_priority(msg->get_priority());
	entry.set_buffer_id(msg->get_buffer_id());
	entry.set_out_port(msg->get_out_port());
	entry.set_flags(msg->get_flags());
std::cout << "TP" << __LINE__ << std::endl;
	entry.match = msg->get_match();
std::cout << "TP" << __LINE__ << std::endl;
	entry.actions = msg->get_actions();
std::cout << "TP" << __LINE__ << std::endl;
	m_parent->send_flow_mod_message( m_parent->get_dpt(), entry );
	m_completed = true;
	return m_completed;
}
~cflow_mod_session() { std::cout << __FUNCTION__ << " called." << std::endl; }	// nothing to do as we didn't register anywhere.
};

class morpheus::cfeatures_request_session : public morpheus::chandlersession_base {
protected:
uint32_t m_request_xid;
public:
cfeatures_request_session(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_features_request * const msg):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_features_request(src, msg);
	}
bool process_features_request ( const rofl::cofctl * const src, const rofl::cofmsg_features_request * const msg ) {
//	if(msg->get_version() != OFP10_VERSION) { std::stringstream ss; ss << "Bad OF version packet received in " << __FUNCTION__; throw rofl::eBadVersion( ss.str() ); }
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_request_xid = msg->get_xid();
	uint32_t newxid = m_parent->send_features_request( m_parent->get_dpt() );
	if( ! m_parent->associate_xid( true, m_request_xid, this ) ) std::cout << "Problem associating ctl xid in " << __FUNCTION__ << std::endl;
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	return m_completed;
}
bool process_features_reply ( const rofl::cofdpt * const src, rofl::cofmsg_features_reply * const msg ) {
	assert(!m_completed);
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_parent->send_features_reply(m_parent->get_ctl(), m_request_xid, m_parent->get_dpid(), msg->get_n_buffers(), msg->get_n_tables(), msg->get_capabilities(), 0, msg->get_actions_bitmap(), msg->get_ports() );	// TODO get_action_bitmap is OF1.0 only
	m_completed = true;
	return m_completed;
}
~cfeatures_request_session() { std::cout << __FUNCTION__ << " called." << std::endl; }
std::string asString() { std::stringstream ss; ss << "cfeatures_request_session {request_xid=" << m_request_xid << "}"; return ss.str(); }
};

class morpheus::cget_config_session : public morpheus::chandlersession_base {
protected:
uint32_t m_request_xid;
public:
cget_config_session(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_get_config_request * const msg):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_config_request(src, msg);
	}
bool process_config_request ( const rofl::cofctl * const src, const rofl::cofmsg_get_config_request * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_request_xid = msg->get_xid();
	uint32_t newxid = m_parent->send_get_config_request( m_parent->get_dpt() );
	if( ! m_parent->associate_xid( true, m_request_xid, this ) ) std::cout << "Problem associating ctl xid in " << __FUNCTION__ << std::endl;
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	return m_completed;
}
bool process_config_reply ( const rofl::cofdpt * const src, rofl::cofmsg_get_config_reply * const msg ) {
	assert(!m_completed);
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_parent->send_get_config_reply(m_parent->get_ctl(), m_request_xid, msg->get_flags(), msg->get_miss_send_len() );
	m_completed = true;
	return m_completed;
}
~cget_config_session() { std::cout << __FUNCTION__ << " called." << std::endl; }
std::string asString() { std::stringstream ss; ss << "cget_config_session {request_xid=" << m_request_xid << "}"; return ss.str(); }
};

class morpheus::cdesc_stats_session : public morpheus::chandlersession_base {
protected:
uint32_t m_request_xid;
public:
cdesc_stats_session(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_desc_stats_request * const msg):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_desc_stats_request(src, msg);
	}
bool process_desc_stats_request ( const rofl::cofctl * const src, const rofl::cofmsg_desc_stats_request * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_request_xid = msg->get_xid();
	uint32_t newxid = m_parent->send_desc_stats_request(m_parent->get_dpt(), msg->get_stats_flags());
	if( ! m_parent->associate_xid( true, m_request_xid, this ) ) std::cout << "Problem associating ctl xid in " << __FUNCTION__ << std::endl;
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	return m_completed;
}
bool process_desc_stats_reply ( rofl::cofdpt * const src, rofl::cofmsg_desc_stats_reply * const msg ) {
	assert(!m_completed);
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	rofl::cofdesc_stats_reply reply(src->get_version(),"tranny_mfr_desc","tranny_hw_desc","tranny_sw_desc","tranny_serial_num","tranny_dp_desc");
	m_parent->send_desc_stats_reply(m_parent->get_ctl(), m_request_xid, reply, false );
	m_completed = true;
	return m_completed;
}
~cdesc_stats_session() { std::cout << __FUNCTION__ << " called." << std::endl; }
std::string asString() { std::stringstream ss; ss << "cdesc_stats_session {request_xid=" << m_request_xid << "}"; return ss.str(); }
};

class morpheus::ctable_stats_session : public morpheus::chandlersession_base {
protected:
uint32_t m_request_xid;
public:
ctable_stats_session(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_table_stats_request * const msg):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_table_stats_request(src, msg);
	}
bool process_table_stats_request ( const rofl::cofctl * const src, const rofl::cofmsg_table_stats_request * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_request_xid = msg->get_xid();
	uint32_t newxid = m_parent->send_table_stats_request(m_parent->get_dpt(), msg->get_stats_flags());
	if( ! m_parent->associate_xid( true, m_request_xid, this ) ) std::cout << "Problem associating ctl xid in " << __FUNCTION__ << std::endl;
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	return m_completed;
}
bool process_table_stats_reply ( rofl::cofdpt * const src, rofl::cofmsg_table_stats_reply * const msg ) {
	assert(!m_completed);
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_parent->send_table_stats_reply(m_parent->get_ctl(), m_request_xid, msg->get_table_stats(), false ); // TODO how to deal with "more" flag (last arg)
	m_completed = true;
	return m_completed;
}
~ctable_stats_session() { std::cout << __FUNCTION__ << " called." << std::endl; }
std::string asString() { std::stringstream ss; ss << "ctable_stats_session {request_xid=" << m_request_xid << "}"; return ss.str(); }
};

class morpheus::cset_config_session : public morpheus::chandlersession_base {
public:
cset_config_session(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_set_config * const msg ):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_set_config(src, msg);
	}
bool process_set_config ( const rofl::cofctl * const src, const rofl::cofmsg_set_config * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_parent->send_set_config_message(m_parent->get_dpt(), msg->get_flags(), msg->get_miss_send_len());
	m_completed = true;
	return m_completed;
}
~cset_config_session() { std::cout << __FUNCTION__ << " called." << std::endl; }	// nothing to do as we didn't register anywhere.
};

class morpheus::caggregate_stats_session : public morpheus::chandlersession_base {
protected:
uint32_t m_request_xid;
public:
caggregate_stats_session(morpheus * parent, const rofl::cofctl * const src, rofl::cofmsg_aggr_stats_request * const msg):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_aggr_stats_request(src, msg);
	}
bool process_aggr_stats_request ( const rofl::cofctl * const src, rofl::cofmsg_aggr_stats_request * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_request_xid = msg->get_xid();
	uint16_t stats_flags = msg->get_stats_flags();
	rofl::cofaggr_stats_request aggr_req( msg->get_aggr_stats() );
	uint32_t newxid = m_parent->send_aggr_stats_request(m_parent->get_dpt(), stats_flags, aggr_req);
	if( ! m_parent->associate_xid( true, m_request_xid, this ) ) std::cout << "Problem associating ctl xid in " << __FUNCTION__ << std::endl;
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	return m_completed;
}
bool process_aggr_stats_reply ( rofl::cofdpt * const src, rofl::cofmsg_aggr_stats_reply * const msg ) {
	assert(!m_completed);
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_parent->send_aggr_stats_reply(m_parent->get_ctl(), m_request_xid, msg->get_aggr_stats(), false ); // TODO how to deal with "more" flag (last arg)
	m_completed = true;
	return m_completed;
}
~caggregate_stats_session() { std::cout << __FUNCTION__ << " called." << std::endl; }
std::string asString() { std::stringstream ss; ss << "caggregate_stats_session {request_xid=" << m_request_xid << "}"; return ss.str(); }
};

class morpheus::cpacket_in_session : public morpheus::chandlersession_base {
public:
cpacket_in_session(morpheus * parent, const rofl::cofdpt * const src, rofl::cofmsg_packet_in * const msg ):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_packet_in(src, msg);
	}
bool process_packet_in ( const rofl::cofdpt * const src, rofl::cofmsg_packet_in * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	rofl::cofctl * master = m_parent->get_ctl();
// std::cout << "TP" << __LINE__ << std::endl;
	rofl::cofmatch match(msg->get_match_const());
// std::cout << "TP" << __LINE__ << "match found to be " << match.c_str() << std::endl;	
	rofl::cpacket packet(msg->get_packet_const());
// std::cout << "TP" << __LINE__ << std::endl;
// std::cout << "packet.framelen = " << (unsigned)packet.framelen() << "packet.soframe = " << packet.soframe() << std::endl;
// std::cout << "TP" << __LINE__ << std::endl;
	packet.get_match().set_in_port(msg->get_in_port());
/* std::cout << "TP" << __LINE__ << std::endl;
std::cout << "packet bytes: ";
dumpBytes( std::cout, msg->get_packet_const().soframe(), msg->get_packet_const().framelen());
std::cout << std::endl;
std::cout << "frame bytes: ";
dumpBytes( std::cout, msg->get_packet().frame()->soframe(), msg->get_packet().frame()->framelen());
std::cout << "TP" << __LINE__ << std::endl;
std::cout << "source MAC: " << msg->get_packet().ether()->get_dl_src() << std::endl;
std::cout << "dest MAC: " << msg->get_packet().ether()->get_dl_dst() << std::endl;
std::cout << "OFP10_PACKET_IN_STATIC_HDR_LEN is " << OFP10_PACKET_IN_STATIC_HDR_LEN << std::endl;
std::cout << "TP" << __LINE__ << std::endl; */
	m_parent->send_packet_in_message(master, msg->get_buffer_id(), msg->get_total_len(), msg->get_reason(), 0, 0, msg->get_in_port(), match, packet.ether()->sopdu(), packet.framelen() );	// TODO - the length fields are guesses.
	std::cout << __FUNCTION__ << " : packet_in forwarded to " << master->c_str() << "." << std::endl;
	m_completed = true;
	return m_completed;
}
~cpacket_in_session() { std::cout << __FUNCTION__ << " called." << std::endl; }	// nothing to do as we didn't register anywhere.
};

class morpheus::cpacket_out_session : public morpheus::chandlersession_base {
public:
cpacket_out_session(morpheus * parent, const rofl::cofctl * const src, rofl::cofmsg_packet_out * const msg ):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_packet_out(src, msg);
	}
bool process_packet_out ( const rofl::cofctl * const src, rofl::cofmsg_packet_out * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	rofl::cofaclist actions(msg->get_actions());
	std::cout << "TP" << __LINE__ << std::endl;
	rofl::cpacket packet(msg->get_packet());
	std::cout << "TP" << __LINE__ << std::endl;
	m_parent->send_packet_out_message(m_parent->get_dpt(), msg->get_buffer_id(), msg->get_in_port(), actions, packet.soframe(), packet.framelen() );	// TODO - the length fields are guesses.
	m_completed = true;
	return m_completed;
}
~cpacket_out_session() { std::cout << __FUNCTION__ << " called." << std::endl; }	// nothing to do as we didn't register anywhere.

};

class morpheus::cbarrier_session : public morpheus::chandlersession_base {
protected:
uint32_t m_request_xid;
public:
cbarrier_session(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_barrier_request * const msg):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_barrier_request(src, msg);
	}
bool process_barrier_request ( const rofl::cofctl * const src, const rofl::cofmsg_barrier_request * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_request_xid = msg->get_xid();
	uint32_t newxid = m_parent->send_barrier_request( m_parent->get_dpt() );
	if( ! m_parent->associate_xid( true, m_request_xid, this ) ) std::cout << "Problem associating ctl xid in " << __FUNCTION__ << std::endl;
	if( ! m_parent->associate_xid( false, newxid, this ) ) std::cout << "Problem associating dpt xid in " << __FUNCTION__ << std::endl;
	m_completed = false;
	return m_completed;
}
bool process_barrier_reply ( const rofl::cofdpt * const src, rofl::cofmsg_barrier_reply * const msg ) {
	assert(!m_completed);
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	m_parent->send_barrier_reply(m_parent->get_ctl(), m_request_xid );
	m_completed = true;
	return m_completed;
}
~cbarrier_session() { std::cout << __FUNCTION__ << " called." << std::endl; }
std::string asString() { std::stringstream ss; ss << "cbarrier_session {request_xid=" << m_request_xid << "}"; return ss.str(); }
};


/*
class morpheus::ctable_mod_session : public morpheus::chandlersession_base {
ctable_mod_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cport_mod_session : public morpheus::chandlersession_base {
cport_mod_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cqueue_stats_session : public morpheus::chandlersession_base {
// cqueue_stats_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cport_stats_session : public morpheus::chandlersession_base {
// cport_stats_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cflow_stats_session : public morpheus::chandlersession_base {
// cflow_stats_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};



*/

#endif // UCL_EE_MORPHEUS_NESTED_H
