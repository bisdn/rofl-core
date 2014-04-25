#include <vector>
#include <ostream>
#include <utility>
#include <iostream>
#include "cportvlan_mapper.h"

const PV_PORT_T PV_PORT_T::ANY = PV_PORT_T::make_ANY();
const PV_VLANID_T PV_VLANID_T::ANY = PV_VLANID_T::make_ANY();
const PV_VLANID_T PV_VLANID_T::NONE = PV_VLANID_T::make_NONE();

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
