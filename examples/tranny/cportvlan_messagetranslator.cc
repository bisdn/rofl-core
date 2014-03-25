#include <vector>
#include <ostream>
#include <utility>
#include <iostream>
#include "cportvlan_messagetranslator.h"

#ifdef MSGTRANTEST

// to test: g++ ./cportvlan_messagetranslator.h -lrofl -lpthread -lrt -DMSGTRANTEST

int main ( int, char ** ) {
	cportvlan_messagetranslator::port_list_t list;
	list.push_back(cportvlan_messagetranslator::port_spec_t(2,3));
	list.push_back(cportvlan_messagetranslator::port_spec_t(4,cportvlan_messagetranslator::NO_VLAN));
	list.push_back(cportvlan_messagetranslator::port_spec_t(2,5));
	list.push_back(cportvlan_messagetranslator::port_spec_t(3,5));
	list.push_back(cportvlan_messagetranslator::port_spec_t(4,2));


	cportvlan_messagetranslator mapper(list);
	
	std::cout << "installed mapper: " << mapper << std::endl;
	
	cportvlan_messagetranslator::port_spec_t tests [] = {
		cportvlan_messagetranslator::port_spec_t(0,0,false,false),
		cportvlan_messagetranslator::port_spec_t(2,0,true,false),
		cportvlan_messagetranslator::port_spec_t(2,4),
		cportvlan_messagetranslator::port_spec_t(4,1),
		cportvlan_messagetranslator::port_spec_t(2,3),
		cportvlan_messagetranslator::port_spec_t(0,5,false,true),
		cportvlan_messagetranslator::port_spec_t(4,0,true,false),
		cportvlan_messagetranslator::port_spec_t(4,cportvlan_messagetranslator::NO_VLAN,true,true),
		cportvlan_messagetranslator::port_spec_t(3,cportvlan_messagetranslator::NO_VLAN,true,true)
	};
	
	std::cout << "Test mappings..\n";
	const unsigned N_tests = sizeof(tests)/sizeof(tests[0]);
	for(unsigned i=0; i < N_tests ; ++i) {
		std::cout << "**Testing " << tests[i] << "\n";
		std::vector<std::pair<uint16_t, cportvlan_messagetranslator::port_spec_t> > res = mapper.actual_to_virtual_map(tests[i]);
		for(std::vector<std::pair<uint16_t, cportvlan_messagetranslator::port_spec_t> >::const_iterator cit = res.begin(); cit != res.end(); ++cit )
			std::cout << cit->first << " => " << cit->second << "\n";
		}
	return 0;	
}

#endif
