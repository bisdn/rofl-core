
#include "csh_packet_out.h"

// TODO translation check
morpheus::csh_packet_out::csh_packet_out(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_packet_out * const msg ):chandlersession_base(parent) {
	std::cout << __PRETTY_FUNCTION__ << " called." << std::endl;
	process_packet_out(src, msg);
	}
	
bool morpheus::csh_packet_out::process_packet_out ( rofl::cofctl * const src, rofl::cofmsg_packet_out * const msg ) {
	if(msg->get_version() != OFP10_VERSION) throw rofl::eBadVersion();
	if(msg->get_buffer_id()!=0xffffffff) {		// TODO check that 0xffffffff is correct - specs say -1, but also say uint32_t
		std::cout << __FUNCTION__ << ": buffered packets in PacketOut not supported." << std::endl;
		assert(false);
		m_completed = true;
		return m_completed;
	}
	rofl::cofaclist actions(msg->get_actions());	// These actions are to be performed on either the buffered packet, or the one included in this message.
	std::cout << "*** Actions copied: ";
	for(rofl::cofaclist::iterator i = actions.begin(); i != actions.end(); ++i)
		std::cout << i->c_str() << " ";
	std::cout << std::endl;
	std::cout << "TP_" << __LINE__ << std::endl;
	rofl::cpacket packet(msg->get_packet());
	std::cout << "TP_" << __LINE__ << std::endl;

	if(packet.cnt_vlan_tags()>0) {
		std::cout << __FUNCTION__ << ": vlan tags in PacketOut packets not supported." << std::endl;
		assert(false);
		m_completed = true;
		return m_completed;
	}

	const cportvlan_mapper & mapper = m_parent->get_mapper();
	
	uint16_t in_vport = msg->get_in_port();
	std::cout << "packet_out in port is " <<  port_as_string(in_vport) << std::endl;
//	rofl::cofaclist outlist;
	if((actions.count_action_type(OFP10AT_SET_VLAN_VID)!=0)||(actions.count_action_type(OFP10AT_SET_VLAN_PCP)!=0)||(actions.count_action_type(OFP10AT_STRIP_VLAN)!=0)) {
		// VLAN-in-VLAN is not supported - return with error.
		std::cout << __FUNCTION__ << " VLAN-in-VLAN is not supported." << std::endl;
		assert(false);
		m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
		m_completed = true;
		return m_completed;
		}
	if(actions.count_action_type(OFP10AT_ENQUEUE)!=0) {
		// Queues not supported for now.	 TODO
		std::cout << __FUNCTION__ << " Queues not supported for now." << std::endl;
		assert(false);
		m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
		m_completed = true;
		return m_completed;
		}
	if(actions.count_action_type(OFP10AT_VENDOR)!=0) {
		// We have no idea what could be in the vendor message, so we can't translate, so we kill it.
		std::cout << __FUNCTION__ << " Vendor actions are unsupported. Sending error and dropping message." << std::endl;
		assert(false);
		m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
		m_completed = true;
		return m_completed;
		}
	std::cout << "Scanning actions for translation.." << std::endl;
	rofl::cofaclist masteroutlist;
	for(rofl::cofaclist::iterator i = actions.begin(); i != actions.end(); ++i) {
		switch( be16toh(i->oac_header->type) ) {
			case OFP10AT_OUTPUT: {
				uint16_t oport = be16toh(i->oac_10output->port);
				if(oport <= OFPP10_MAX) {
					if(oport > mapper.get_number_virtual_ports() ) {
						// invalid virtual port number
						m_parent->send_error_message(src, msg->get_xid(), OFP10ET_BAD_ACTION, OFP10BAC_BAD_OUT_PORT, msg->soframe(), msg->framelen() );
						m_completed = true;
						return m_completed;
					}
					cportvlan_mapper::port_spec_t outport_spec = mapper.get_actual_port( oport );
					rofl::cofaclist myoutlist = masteroutlist;
/*					rofl::cpacket outpacket(msg->get_packet());
					if(!outport_spec.vlanid_is_none()) {
						outpacket.push_vlan(rofl::fvlanframe::VLAN_CTAG_ETHER);	// create new vlan header
						outpacket.vlan()->set_dl_vlan_id(outport_spec.vlan);				// set the VLAN ID
						std::cout << "VLAN tag " << (unsigned)outport_spec.vlan << " inserted in to packet destined for virtual port " << (unsigned)oport << "." << std::endl;
					}
					outlist.next() =  rofl::cofaction_output( OFP10_VERSION, outport_spec.port, be16toh(i->oac_10output->max_len) );	// add translated output action
					std::cout << "Added action to output to actual port number " << (unsigned) outport_spec.port << std::endl;
					std::cout << "Packet bytes: " << std::endl;
					dumpBytes( std::cout, outpacket.soframe(), outpacket.framelen() );
					std::cout << std::endl;
					m_parent->send_packet_out_message(m_parent->get_dpt(), msg->get_buffer_id(), in_vport, outlist, outpacket.soframe(), outpacket.framelen() );	// TODO - the length fields are guesses.
					std::cout << "Translated packet-out successfully sent." << std::endl; */
					if(!outport_spec.vlanid_is_none()) {
//						myoutlist.next() =  rofl::cofaction_push_vlan( OFP10_VERSION, rofl::fvlanframe::VLAN_CTAG_ETHER);
						myoutlist.next() =  rofl::cofaction_set_vlan_vid( OFP10_VERSION, outport_spec.vlan);
						std::cout << "VLAN tagging action with VID " << (unsigned)outport_spec.vlan << " added to action list destined for virtual port " << (unsigned)oport << "." << std::endl;
					}
					myoutlist.next() =  rofl::cofaction_output( OFP10_VERSION, outport_spec.port, be16toh(i->oac_10output->max_len) );	// add translated output action
					std::cout << "Added action to output to actual port number " << (unsigned) outport_spec.port << std::endl;
					std::cout << "Packet bytes: " << std::endl;
					dumpBytes( std::cout, msg->get_packet().soframe(), msg->get_packet().framelen() );
					std::cout << std::endl;
					m_parent->send_packet_out_message(m_parent->get_dpt(), msg->get_buffer_id(), in_vport, myoutlist, msg->get_packet().soframe(), msg->get_packet().framelen() );	// TODO - the length fields are guesses.
					std::cout << "Translated packet-out successfully sent." << std::endl;
				} else
				// FLOOD == "All physical ports except input port and those disabled by STP."
				// ALL == "All physical ports except input port."
				if( ( oport == OFPP10_FLOOD ) || ( oport == OFPP10_ALL ) ) {
					// since virtual ports may cover less than the number of actual ports, we have to replicate the packet-out for each port manually
					// either send multiple packet-outs, or maybe on with multiple output actions?
					std::cout << "packet-out to all or flood requested.." << std::endl;
				/*	for(oport = 1; oport <= mapper.get_number_virtual_ports(); ++oport) {
						cportvlan_mapper::port_spec_t outport_spec = mapper.get_actual_port( oport );
						std::cout << "Performing translation from virtual port " << (unsigned) oport << " to actual " << outport_spec << "." << std::endl;
						rofl::cofaclist myoutlist = masteroutlist;
						if(!outport_spec.vlanid_is_none()) {
							myoutlist.next() =  rofl::cofaction_set_vlan_vid( OFP10_VERSION, outport_spec.vlan);
							std::cout << "VLAN tagging action with VID " << (unsigned)outport_spec.vlan << " added to action list destined for virtual port " << (unsigned)oport << "." << std::endl;
						}
						myoutlist.next() =  rofl::cofaction_output( OFP10_VERSION, outport_spec.port, be16toh(i->oac_10output->max_len) );	// add translated output action
						std::cout << "Added action to output to actual port number " << (unsigned) outport_spec.port << std::endl;
						std::cout << "Packet bytes: " << std::endl;
						dumpBytes( std::cout, msg->get_packet().soframe(), msg->get_packet().framelen() );
						std::cout << std::endl;
						m_parent->send_packet_out_message(m_parent->get_dpt(), msg->get_buffer_id(), in_vport, myoutlist, msg->get_packet().soframe(), msg->get_packet().framelen() );	// TODO - the length fields are guesses.
						std::cout << "Translated packet-out successfully sent." << std::endl;
					} */
					rofl::cofaclist myoutlist = masteroutlist;
					rofl::cofaclist taggedoutputs;
/*					rofl::cofaclist untaggedoutputs;
					for(oport = 1; oport <= mapper.get_number_virtual_ports(); ++oport) {
						cportvlan_mapper::port_spec_t outport_spec = mapper.get_actual_port( oport );
						std::cout << "Generating output action from virtual port " << (unsigned) oport << " to actual " << outport_spec << "." << std::endl;
						if(outport_spec.vlanid_is_none()) 
							untaggedoutputs.next() =  rofl::cofaction_output( OFP10_VERSION, outport_spec.port, be16toh(i->oac_10output->max_len) );
						else {
							taggedoutputs.next() =  rofl::cofaction_set_vlan_vid( OFP10_VERSION, outport_spec.vlan);
							taggedoutputs.next() =  rofl::cofaction_output( OFP10_VERSION, outport_spec.port, be16toh(i->oac_10output->max_len) );
						}
					} */
					// myoutlist += untaggedoutputs;
					// myoutlist += taggedoutputs;
					// std::copy(untaggedoutputs.begin(), untaggedoutputs.end(), std::back_inserter(myoutlist));
					// std::copy(taggedoutputs.begin(), taggedoutputs.end(), std::back_inserter(myoutlist));
					for(oport = 1; oport <= mapper.get_number_virtual_ports(); ++oport) {
						cportvlan_mapper::port_spec_t outport_spec = mapper.get_actual_port( oport );
						std::cout << "Generating output action from virtual port " << (unsigned) oport << " to actual " << outport_spec << "." << std::endl;
						if(outport_spec.vlanid_is_none()) 
							myoutlist.next() =  rofl::cofaction_output( OFP10_VERSION, outport_spec.port, be16toh(i->oac_10output->max_len) );
						else {
							taggedoutputs.next() =  rofl::cofaction_set_vlan_vid( OFP10_VERSION, outport_spec.vlan);
							taggedoutputs.next() =  rofl::cofaction_output( OFP10_VERSION, outport_spec.port, be16toh(i->oac_10output->max_len) );
						}
					}
					for(rofl::cofaclist::iterator toi = taggedoutputs.begin(); toi != taggedoutputs.end(); ++toi) myoutlist.next() = *toi;
					if(taggedoutputs.begin() != taggedoutputs.end()) myoutlist.next() = rofl::cofaction_strip_vlan( OFP10_VERSION );	// the input output action may not be the last such action so we need to clean up the VLAN that we've left on the "stack"
					m_parent->send_packet_out_message(m_parent->get_dpt(), msg->get_buffer_id(), in_vport, myoutlist, msg->get_packet().soframe(), msg->get_packet().framelen() );	// TODO - the length fields are guesses.
					std::cout << "Translated packet-out successfully sent." << std::endl;
//					assert(false);	// TODO
				} else {
					// unsupported output port in action
					std::cout << __FUNCTION__ << ": Unsupported port number " << std::hex << (unsigned) oport << " in output action of packet-out." << std ::endl;
					m_parent->send_error_message(src, msg->get_xid(), OFP10ET_BAD_ACTION, OFP10BAC_BAD_OUT_PORT, msg->soframe(), msg->framelen() );
					m_completed = true;
					return m_completed;
				}
			} break;
			case OFP10AT_SET_DL_SRC:
			case OFP10AT_SET_DL_DST:
			case OFP10AT_SET_NW_SRC:
			case OFP10AT_SET_NW_DST:
			case OFP10AT_SET_NW_TOS:
			case OFP10AT_SET_TP_SRC:
			case OFP10AT_SET_TP_DST: {
				// just pass the message through
				masteroutlist.next() = *i;
			} break;
			default:
				std::cout << "Unsupported action " << be16toh(i->oac_header->type) << " sent with packet_out." << std::endl;
				m_parent->send_error_message( src, msg->get_xid(), OFP10ET_BAD_ACTION, OFP10BAC_EPERM, msg->soframe(), msg->framelen() );
				m_completed = true;
				return m_completed;
		}
	}
	/*
	switch(in_vport) {
		case OFPP10_ALL: 	// All physical ports except input port.

		case OFPP10_IN_PORT:// send the packet out the input port.  This virtual port must be explicitly used in order to send back out of the input port.
		case OFPP10_TABLE:	// Perform actions in flow table. NB: This can only be the destination port for packet-out messages.
		case OFPP10_NORMAL:	// Process with normal L2/L3 switching.
		case OFPP10_CONTROLLER:	// Send to controller.
		case OFPP10_LOCAL:	// Local OpenFlow "port".
		case OFPP10_NONE:	// Not associated with a physical port.
		case OFPP10_FLOOD: 	// All physical ports except input port and those disabled by STP.
			std::cout << __FUNCTION__ << " doesn't support output port " << std::hex << (unsigned) in_vport << std::endl;
			m_completed = true;
			return m_completed;
		default:	// an actual numbered port
//		if(in_vport > OFPP10_MAX ) panic
			break;
	}; */
/*	cportvlan_mapper::port_spec_t inport_spec( mapper.get_actual_port(msg->get_in_port()) );
	if(!inport_spec.vlanid_is_none()) {
		packet.push_vlan(rofl::fvlanframe::VLAN_CTAG_ETHER);	// create new vlan header
		packet.vlan()->set_dl_vlan_id(inport_spec.vlan);				// set the VLAN ID
	}
	m_parent->send_packet_out_message(m_parent->get_dpt(), msg->get_buffer_id(), inport_spec.port, actions, packet.soframe(), packet.framelen() );	// TODO - the length fields are guesses.
*/
	m_completed = true;
	return m_completed;
}
morpheus::csh_packet_out::~csh_packet_out() { std::cout << __FUNCTION__ << " called." << std::endl; }	// nothing to do as we didn't register anywhere.

std::string morpheus::csh_packet_out::asString() const { return "csh_packet_out {no xid}"; }
