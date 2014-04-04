// nested classes

#ifndef UCL_EE_MORPHEUS_NESTED_H
#define UCL_EE_MORPHEUS_NESTED_H

#include <sstream>
#include <string>
#include <rofl/common/openflow/openflow_rofl_exceptions.h>
#include <rofl/common/cerror.h>

class morpheus;

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
cflow_mod_session(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_flow_mod * const msg ):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_flow_mod(src, msg);
	}
bool process_flow_mod ( const rofl::cofctl * const src, const rofl::cofmsg_flow_mod * const msg ) {
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
	m_parent->add_ctl_session( m_request_xid, boost::shared_ptr<morpheus::chandlersession_base>(this) );
	m_parent->add_dpt_session( newxid, boost::shared_ptr<morpheus::chandlersession_base>(this) );
//	m_parent->associate_dpt_xid( newxid, m_request_xid );
	m_completed = false;
	return m_completed;
}
bool process_features_reply ( const rofl::cofdpt * const src, rofl::cofmsg_features_reply * const msg ) {
//	if(msg->get_version()) { std::stringstream ss; ss << "Bad OF version packet received in " << __FUNCTION__; throw rofl::eBadVersion( ss.str() ); }
	if(msg->get_version()) throw rofl::eBadVersion();
	m_parent->send_features_reply(m_parent->get_ctl(), m_request_xid, m_parent->get_dpid(), msg->get_n_buffers(), msg->get_n_tables(), msg->get_capabilities(), 0, msg->get_actions_bitmap(), msg->get_ports() );	// TODO get_action_bitmap is OF1.0 only
	// our work here is done - deregister us
	m_completed = true;
	unsigned num_associations = 0;
	num_associations += m_parent->remove_ctl_session(boost::shared_ptr<morpheus::chandlersession_base>(this));
	num_associations += m_parent->remove_dpt_session(boost::shared_ptr<morpheus::chandlersession_base>(this));
	std::cout << __FUNCTION__ << " de-registered " << num_associations << " associations." << std::endl;
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
	m_parent->add_ctl_session( m_request_xid, boost::shared_ptr<morpheus::chandlersession_base>(this) );
	m_parent->add_dpt_session( newxid, boost::shared_ptr<morpheus::chandlersession_base>(this) );
//	m_parent->associate_dpt_xid( newxid, m_request_xid );
	m_completed = false;
	return m_completed;
}
bool process_config_reply ( const rofl::cofdpt * const src, rofl::cofmsg_get_config_reply * const msg ) {
	if(msg->get_version()) throw rofl::eBadVersion();
	m_parent->send_get_config_reply(m_parent->get_ctl(), m_request_xid, msg->get_flags(), msg->get_miss_send_len() );
	// our work here is done - deregister us
	m_completed = true;
	unsigned num_associations = 0;
	num_associations += m_parent->remove_ctl_session(boost::shared_ptr<morpheus::chandlersession_base>(this));
	num_associations += m_parent->remove_dpt_session(boost::shared_ptr<morpheus::chandlersession_base>(this));
	std::cout << __FUNCTION__ << " de-registered " << num_associations << " associations." << std::endl;
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
	m_parent->add_ctl_session( m_request_xid, boost::shared_ptr<morpheus::chandlersession_base>(this) );
	m_parent->add_dpt_session( newxid, boost::shared_ptr<morpheus::chandlersession_base>(this) );
//	m_parent->associate_dpt_xid( newxid, m_request_xid );
	m_completed = false;
	return m_completed;
}
bool process_desc_stats_reply ( rofl::cofdpt * const src, rofl::cofmsg_desc_stats_reply * const msg ) {
	if(msg->get_version()) throw rofl::eBadVersion();
	rofl::cofdesc_stats_reply reply(src->get_version(),"tranny_mfr_desc","tranny_hw_desc","tranny_sw_desc","tranny_serial_num","tranny_dp_desc");
	m_parent->send_desc_stats_reply(m_parent->get_ctl(), m_request_xid, reply, false );
	// our work here is done - deregister us
	m_completed = true;
	unsigned num_associations = 0;
	num_associations += m_parent->remove_ctl_session(boost::shared_ptr<morpheus::chandlersession_base>(this));
	num_associations += m_parent->remove_dpt_session(boost::shared_ptr<morpheus::chandlersession_base>(this));
	std::cout << __FUNCTION__ << " de-registered " << num_associations << " associations." << std::endl;
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
	m_parent->add_ctl_session( m_request_xid, boost::shared_ptr<morpheus::chandlersession_base>(this) );
	m_parent->add_dpt_session( newxid, boost::shared_ptr<morpheus::chandlersession_base>(this) );
//	m_parent->associate_dpt_xid( newxid, m_request_xid );
	m_completed = false;
	return m_completed;
}
bool process_table_stats_reply ( rofl::cofdpt * const src, rofl::cofmsg_table_stats_reply * const msg ) {
	if(msg->get_version()) throw rofl::eBadVersion();
	m_parent->send_table_stats_reply(m_parent->get_ctl(), m_request_xid, msg->get_table_stats(), false ); // TODO how to deal with "more" flag (last arg)
	// our work here is done - deregister us
	m_completed = true;
	unsigned num_associations = 0;
	num_associations += m_parent->remove_ctl_session(boost::shared_ptr<morpheus::chandlersession_base>(this));
	num_associations += m_parent->remove_dpt_session(boost::shared_ptr<morpheus::chandlersession_base>(this));
	std::cout << __FUNCTION__ << " de-registered " << num_associations << " associations." << std::endl;
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
/*
class morpheus::cport_stats_session : public morpheus::chandlersession_base {
// cport_stats_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cflow_stats_session : public morpheus::chandlersession_base {
// cflow_stats_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::caggregate_stats_session : public morpheus::chandlersession_base {
// caggregate_stats_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cqueue_stats_session : public morpheus::chandlersession_base {
// cqueue_stats_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cpacket_in_session : public morpheus::chandlersession_base {
// cpacket_in_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cpacket_out_session : public morpheus::chandlersession_base {
cpacket_out_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cpacket_in_session : public morpheus::chandlersession_base {
cpacket_in_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cbarrier_session : public morpheus::chandlersession_base {
cbarrier_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::ctable_mod_session : public morpheus::chandlersession_base {
ctable_mod_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cport_mod_session : public morpheus::chandlersession_base {
cport_mod_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

*/

#endif // UCL_EE_MORPHEUS_NESTED_H
