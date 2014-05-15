
#include <rofl/common/openflow/openflow_rofl_exceptions.h>
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/cerror.h>
#include <rofl/common/openflow/cofaction.h>

#include "csh_flow_mod.h"

morpheus::csh_flow_mod::csh_flow_mod(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_flow_mod * const msg ):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_flow_mod(src, msg);
	}

bool morpheus::csh_flow_mod::process_flow_mod ( rofl::cofctl * const src, rofl::cofmsg_flow_mod * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	std::cout << "Incoming msg match: " << msg->get_match().c_str() << " msg actions: " << msg->get_actions().c_str() << std::endl;
	const cportvlan_mapper & mapper = m_parent->get_mapper();
	struct ofp10_match * p = msg->get_match().ofpu.ofp10u_match;
dumpBytes( std::cout, (uint8_t *) p, sizeof(struct ofp10_match) );
	rofl::cflowentry entry(OFP10_VERSION);
	entry.set_command(msg->get_command());
	// TODO !!!!!!!!
	if(msg->get_command() != OFPFC_ADD) {
		std::cout << __FUNCTION__ << ": FLOW_MOD command " << (unsigned)msg->get_command() << " not supported. Dropping message." << std::endl;
		m_completed = true;
		return m_completed;
	}
	entry.set_idle_timeout(msg->get_idle_timeout());
	entry.set_hard_timeout(msg->get_hard_timeout());
	entry.set_cookie(msg->get_cookie());
	entry.set_priority(msg->get_priority());
	entry.set_buffer_id(msg->get_buffer_id());
	entry.set_out_port(msg->get_out_port());	// TODO this will have to be translated if the message is OFPFC_DELETE*
	entry.set_flags(msg->get_flags());
std::cout << "TP" << __LINE__ << std::endl;
	entry.match = msg->get_match();
std::cout << "TP" << __LINE__ << std::endl;
	entry.actions = msg->get_actions();
std::cout << "TP" << __LINE__ << std::endl;
	rofl::cofaclist inlist = msg->get_actions();
	rofl::cofaclist outlist;
//	bool already_set_vlan = false;
	bool already_did_output = false;
// now translate the action and the match
	for(rofl::cofaclist::iterator a = inlist.begin(); a != inlist.end(); ++ a) {
std::cout << "TP" << __LINE__ << std::endl;
		uint32_t supported_actions = m_parent->get_supported_actions();
		std::cout << __FUNCTION__ << " supported actions by underlying switch found to be: " << action_mask_to_string(supported_actions) << "." << std::endl;
		if( ! ((1<<(be16toh(a->oac_header->type))) & supported_actions )) {
			// the action isn't supported by the underlying switch - complain - send an error message, then write to your MP. Start a Tea Party movement. Then start an Occupy OpenFlow group. If that still doesn't help become a recluse and blame the system.
			std::cout << "Received a flow-mod with an unsupported action: " << action_mask_to_string((1<<(be16toh(a->oac_header->type)))) << " (" << (1<<(be16toh(a->oac_header->type))) << "). Returning error." << std::endl;
			m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
			m_completed = true;
			return m_completed;
		}
		std::cout << "Processing incoming action " << action_mask_to_string((1<<(be16toh(a->oac_header->type))))  << "." << std::endl;
		switch(be16toh(a->oac_header->type)) {
			case OFP10AT_OUTPUT: {
				uint16_t oport = be16toh(a->oac_10output->port);
std::cout << "TP" << __LINE__ << std::endl;
				if ( ( oport == OFPP10_FLOOD ) || ( oport == OFPP10_ALL) ) {	// TODO check that the match isn't ALL
					// ALL is all except input port
					// FLOOD is all except input port and those disabled by STP.. which we don't support anyway - so I'm going to treat them the same way.
					// we need to generate a list of untagged output actions, then a list of tagged output actions for all interfaces except the input interface.
					rofl::cofaclist taggedoutputs;
					for(oport = 1; oport <= mapper.get_number_virtual_ports(); ++oport) {
						cportvlan_mapper::port_spec_t outport_spec = mapper.get_actual_port( oport );
std::cout << "TP" << __LINE__ << std::endl;
						if(outport_spec.port == msg->get_match().get_in_port()) {
							// this is the input port - skipping
							std::cout << "virtual port " << (unsigned) oport << " [" << outport_spec << "] is the input port - skipping as output." << std::endl;
							continue;
						}
std::cout << "TP" << __LINE__ << std::endl;
						std::cout << "Generating output action from virtual port " << (unsigned) oport << " to actual " << outport_spec << "." << std::endl;
						if(outport_spec.vlanid_is_none())
							outlist.next() =  rofl::cofaction_output( OFP10_VERSION, outport_spec.port, be16toh(a->oac_10output->max_len) );
						else {
							taggedoutputs.next() = rofl::cofaction_set_vlan_vid( OFP10_VERSION, outport_spec.vlan);
							taggedoutputs.next() = rofl::cofaction_output( OFP10_VERSION, outport_spec.port, be16toh(a->oac_10output->max_len) );
						}
					}
std::cout << "TP" << __LINE__ << std::endl;
					for(rofl::cofaclist::iterator toi = taggedoutputs.begin(); toi != taggedoutputs.end(); ++toi) outlist.next() = *toi;
					if(taggedoutputs.begin() != taggedoutputs.end()) outlist.next() = rofl::cofaction_strip_vlan( OFP10_VERSION );	// the input output action may not be the last such action so we need to clean up the VLAN that we've left on the "stack"
std::cout << "TP" << __LINE__ << std::endl;
				} else {	// not FLOOD
					if(oport > mapper.get_number_virtual_ports() ) {
						// invalid virtual port number
						m_parent->send_error_message(src, msg->get_xid(), OFP10ET_BAD_ACTION, OFP10BAC_BAD_OUT_PORT, msg->soframe(), msg->framelen() );
						m_completed = true;
						return m_completed;
					}
std::cout << "TP" << __LINE__ << std::endl;
					cportvlan_mapper::port_spec_t real_port = mapper.get_actual_port( oport );
					if(!real_port.vlanid_is_none()) {	// add a vlan tagger before an output if necessary
						outlist.next() = rofl::cofaction_set_vlan_vid( OFP10_VERSION, real_port.vlan );
//						already_set_vlan = true;
					}
std::cout << "TP" << __LINE__ << std::endl;
					outlist.next() =  rofl::cofaction_output( OFP10_VERSION, real_port.port, be16toh(a->oac_10output->max_len) );	// add translated output action
				}
std::cout << "TP" << __LINE__ << std::endl;
				already_did_output = true;
			} break;
			case OFP10AT_SET_VLAN_VID: {
				// VLAN-in-VLAN is not supported - return with error.
				assert(false);
				m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
				m_completed = true;
				return m_completed;
			} break;
			case OFP10AT_SET_VLAN_PCP: {
				// VLAN-in-VLAN is not supported - return with error.
				assert(false);
				m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
				m_completed = true;
				return m_completed;
			} break;
			case OFP10AT_STRIP_VLAN: {
				if(already_did_output) {	// cannot strip after output has already been done
//				if(already_set_vlan) {
//					// cannot strip after we've already added a set-vlan message  JSP TODO - is this correct?
					std::cout << __FUNCTION__ << ": attempt was made to strip VLAN after an OFP10AT_OUTPUT action. Rejecting flow-mod." << std::endl;
					m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
					m_completed = true;
					return m_completed;
				}
			} break;
			case OFP10AT_ENQUEUE: {
				// Queues not supported for now.
				assert(false);
				m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
				m_completed = true;
				return m_completed;
			} break;
			case OFP10AT_SET_DL_SRC:
			case OFP10AT_SET_DL_DST:
			case OFP10AT_SET_NW_SRC:
			case OFP10AT_SET_NW_DST:
			case OFP10AT_SET_NW_TOS:
			case OFP10AT_SET_TP_SRC:
			case OFP10AT_SET_TP_DST: {
				// just pass the message through
				outlist.next() = *a;
			} break;
			case OFP10AT_VENDOR: {
				// We have no idea what could be in the vendor message, so we can't translate, so we kill it.
				std::cout << __FUNCTION__ << " Vendor actions are unsupported. Sending error and dropping message." << std::endl;
				m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
				m_completed = true;
				return m_completed;
			} break;
			default:
			std::cout << __FUNCTION__ << " unknown action type (" << (unsigned)be16toh(a->oac_header->type) << "). Sending error and dropping message." << std::endl;
			m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
			m_completed = true;
			return m_completed;
		}
	}
	entry.actions = outlist;
	std::cout << "Actions translated." << std::endl;
	
	rofl::cofmatch newmatch = msg->get_match();
	rofl::cofmatch oldmatch = newmatch;
std::cout << "TP" << __LINE__ << std::endl;
	//check that VLANs are wildcarded (i.e. not being matched on)
	// TODO we *could* theoretically support incoming VLAN iff they are coming in on an port-translated-only port (i.e. a virtual port that doesn't map to a port+vlan, only a phsyical port), and that VLAN si then stripped in the action.
	try {
		oldmatch.get_vlan_vid_mask();	// ignore result - we only care if it'll throw
//		if(oldmatch.get_vlan_vid_mask() != 0xffff) {
			std::cout << __FUNCTION__ << ": received a match which didn't have VLAN wildcarded. Sending error and dropping message. match:" << oldmatch.c_str() << std::endl;
			m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
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
		std::cout << __FUNCTION__ << ": received a match request for an unknown port (" << old_inport << "). There are " << mapper.get_number_virtual_ports() << " ports.  Sending error and dropping message. match:" << oldmatch.c_str() << std::endl;
		m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
		m_completed = true;
		return m_completed;
	}
std::cout << "TP" << __LINE__ << std::endl;
	entry.match = newmatch;
	
	std::cout << __FUNCTION__ << ": About to send flow_mod {";
	std::cout << " command: " << (unsigned) entry.get_command();
	std::cout << ", idle_timeout: " << (unsigned) entry.get_idle_timeout();
	std::cout << ", hard_timeout: " <<  (unsigned) entry.get_hard_timeout();
	std::cout << ", cookie: " << (unsigned) entry.get_cookie();
	std::cout << ", priority: " << (unsigned) entry.get_priority();
	std::cout << ", buffer_id: " << (unsigned) entry.get_buffer_id();
	std::cout << ", out_port: " << (unsigned) entry.get_out_port();
	std::cout << ", match: " << entry.match.c_str();
	std::cout << ", actions: " << entry.actions.c_str() << " }" << std::endl;
	m_parent->send_flow_mod_message( m_parent->get_dpt(), entry );
	m_completed = true;
	return m_completed;
}

morpheus::csh_flow_mod::~csh_flow_mod() { std::cout << __FUNCTION__ << " called." << std::endl; }


std::string morpheus::csh_flow_mod::asString() const { return "csh_flow_mod {no xid}"; }

