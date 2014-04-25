#include <vector>
#include <ostream>
#include <utility>
#include <iostream>
#include "cportvlan_mapper.h"

const PV_PORT_T PV_PORT_T::ANY = PV_PORT_T::make_ANY();
const PV_VLANID_T PV_VLANID_T::ANY = PV_VLANID_T::make_ANY();
const PV_VLANID_T PV_VLANID_T::NONE = PV_VLANID_T::make_NONE();

rofl::cofaclist cportvlan_mapper::action_convertor(const rofl::cofaclist & inlist) const {
	rofl::cofaclist outlist;
	for(rofl::cofaclist::iterator a = inlist.begin(); a != inlist.end(); ++ a) {
		if( ! (be16toh(a->oac_header->type) & m_parent->get_supported_actions() ) ) {
			// the action isn't supported by the underlying switch - complain - send an error message, then write to your MP. Start a Tea Party movement. Then start an Occupy OpenFlow group. If that still doesn't help become a recluse and blame the system.
			m_parent->send_error_message( src, msg->get_xid(), OFP10ET_FLOW_MOD_FAILED, OFP10FMFC_UNSUPPORTED, msg->soframe(), msg->framelen() );
			m_completed = true;
			return m_completed;
		}
		switch(be16toh(a->oac_header->type)) {
			case OFP10AT_OUTPUT: {
				uint16_t oport = be16toh(a->oac_10output->port);
				// TODO must add support for ports ALL and FLOOD
				if(oport > m_parent->get_mapper().get_number_virtual_ports() ) {
					// invalid virtual port number
					throw rofl::eBadActionBadOutPort();
				}
				cportvlan_mapper::port_spec_t real_port = m_parent->get_mapper().get_actual_port( oport );
				if(!real_port.vlanid_is_none()) {	// add a vlan tagger before an output if necessary
					outlist.next() = rofl::cofaction_set_vlan_vid( OFP10_VERSION, real_port.vlan );
					already_set_vlan = true;
				}
				outlist.next() =  rofl::cofaction_output( OFP10_VERSION, real_port.port, be16toh(a->oac_10output->max_len) );	// add translated output action
				already_did_output = true;
			} break;
			case OFP10AT_SET_VLAN_VID: {
				// VLAN-in-VLAN is not supported - return with error.
				throw rofl::eBadActionBadArgument();
			} break;
			case OFP10AT_SET_VLAN_PCP: {
				// VLAN-in-VLAN is not supported - return with error.
				throw rofl::eBadActionBadArgument();
			} break;
			case OFP10AT_STRIP_VLAN: {
				if(already_set_vlan) {
					// cannot strip after we've already added a set-vlan message
					std::cout << __FUNCTION__ << ": attempt was made to strip VLAN after an OFP10AT_OUTPUT action - this would strip VLAN from virtual port translation. Replying with OFPMFC_UNSUPPORTED." << std::endl;
					throw rofl::eBadActionBadArgument();
				}
			} break;
			case OFP10AT_ENQUEUE: {
				// Queues not supported for now.
				throw rofl::eBadActionBadArgument();
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
				throw rofl::eBadActionBadArgument();
			} break;
			default:
			std::cout << __FUNCTION__ << " unknown action type (" << (unsigned)be16toh(a->oac_header->type) << ")." << std::endl;
			throw rofl::eBadActionBadArgument();
			}
	}
return outlist;
}


#ifdef MSGTRANTEST

// to test: g++ ./cportvlan_mapper.cc -lrofl -lpthread -lrt -DMSGTRANTEST

int main ( int, char ** ) {

// create a list of virtual ports that will be passed to the mapper

	std::vector<cportvlan_mapper::port_spec_t> list;
	list.push_back( cportvlan_mapper::port_spec_t( PV_PORT_T(2), PV_VLANID_T(3) ) );
	list.push_back( cportvlan_mapper::port_spec_t( PV_PORT_T(4), PV_VLANID_T::NONE ) );
	list.push_back( cportvlan_mapper::port_spec_t( PV_PORT_T(2), PV_VLANID_T(5) ) );
	list.push_back( cportvlan_mapper::port_spec_t( PV_PORT_T(3), PV_VLANID_T(5) ) );
	list.push_back( cportvlan_mapper::port_spec_t( PV_PORT_T(4), PV_VLANID_T(2) ) );
//	list.push_back( cportvlan_mapper::port_spec_t( PV_PORT_T(7), PV_VLANID_T::NONE ) );

// create mapper, initialised with the above listed virtual ports

	cportvlan_mapper mapper( list.begin(), list.end() );

// adding this *virtual*port* should fail.. and it does.. so it's commented out.
//	mapper.add_virtual_port( cportvlan_mapper::port_spec_t( PV_PORT_T(1), PV_VLANID_T::ANY ) );

std::cout << "Just added port number " << mapper.add_virtual_port(cportvlan_mapper::port_spec_t( PV_PORT_T(7), PV_VLANID_T::NONE )) << std::endl;

// dump the mappings to check correct initialisation
	std::cout << "installed mapper: " << mapper << std::endl;
	
// a series of tests, including combinations of wildcards and non-existant ports
	cportvlan_mapper::port_spec_t tests [] = {
		cportvlan_mapper::port_spec_t( PV_PORT_T::ANY, PV_VLANID_T::ANY ),
		cportvlan_mapper::port_spec_t( PV_PORT_T(2),   PV_VLANID_T::ANY ),
		cportvlan_mapper::port_spec_t( PV_PORT_T(2),   PV_VLANID_T(4) ),
		cportvlan_mapper::port_spec_t( PV_PORT_T(4),   PV_VLANID_T(1) ),
		cportvlan_mapper::port_spec_t( PV_PORT_T(2),   PV_VLANID_T(3) ),
		cportvlan_mapper::port_spec_t( PV_PORT_T::ANY, PV_VLANID_T(5) ),
		cportvlan_mapper::port_spec_t( PV_PORT_T(4),   PV_VLANID_T::ANY ),
		cportvlan_mapper::port_spec_t( PV_PORT_T(4),   PV_VLANID_T::NONE ),
		cportvlan_mapper::port_spec_t( PV_PORT_T(3),   PV_VLANID_T::NONE ),
		cportvlan_mapper::port_spec_t( PV_PORT_T(1),   PV_VLANID_T::NONE ),
		cportvlan_mapper::port_spec_t( PV_PORT_T::ANY, PV_VLANID_T::NONE ),
	};
	
//	unsigned expected_results [][] = { {}, {} }
	
// try to match the test mappings against the earlier defined virtual ports
	std::cout << "Test mappings..\n";
	const unsigned N_tests = sizeof(tests)/sizeof(tests[0]);
	for(unsigned i=0; i < N_tests ; ++i) {
		std::cout << "**Testing " << tests[i] << "\n";
		std::vector<std::pair<uint16_t, cportvlan_mapper::port_spec_t> > res = mapper.actual_to_virtual_map(tests[i]);
		for(std::vector<std::pair<uint16_t, cportvlan_mapper::port_spec_t> >::const_iterator cit = res.begin(); cit != res.end(); ++cit )
			std::cout << cit->first << " => " << cit->second << "\n";
		}
	return 0;	
}

#endif
