#include <vector>
#include <ostream>
#include <utility>
#include <iostream>
#include "cportvlan_messagetranslator.h"

std::vector<rofl::cofmsg_features_request> cportvlan_messagetranslator::translate_features_request( const rofl::cofctl * , rofl::cofmsg_features_request * ) { return std::vector<rofl::cofmsg_features_request>(); }
std::vector<rofl::cofmsg_features_reply> cportvlan_messagetranslator::translate_features_reply( const rofl::cofdpt * , rofl::cofmsg_features_reply *  ) { return std::vector<rofl::cofmsg_features_reply>(); }
std::vector<rofl::cofmsg_get_config_request> cportvlan_messagetranslator::translate_get_config_request( const rofl::cofctl * , rofl::cofmsg_get_config_request * ) { return std::vector<rofl::cofmsg_get_config_request>(); }
std::vector<rofl::cofmsg_get_config_reply> cportvlan_messagetranslator::translate_get_config_reply( const rofl::cofdpt * , rofl::cofmsg_get_config_reply *  ) { return std::vector<rofl::cofmsg_get_config_reply>(); }

std::vector<rofl::cofmsg> cportvlan_messagetranslator::translate_error ( const rofl::cofdpt * , rofl::cofmsg * ) { return std::vector<rofl::cofmsg>(); }

std::vector<rofl::cofmsg_desc_stats_request> cportvlan_messagetranslator::translate_desc_stats_request( const rofl::cofctl * , rofl::cofmsg_desc_stats_request * ) { return std::vector<rofl::cofmsg_desc_stats_request>(); }
std::vector<rofl::cofmsg_desc_stats_reply> cportvlan_messagetranslator::translate_desc_stats_reply( const rofl::cofdpt * , rofl::cofmsg_desc_stats_reply * ) { return std::vector<rofl::cofmsg_desc_stats_reply>(); }

std::vector<rofl::cofmsg_table_stats_request> cportvlan_messagetranslator::translate_table_stats_request( const rofl::cofctl * , rofl::cofmsg_table_stats_request * ) { return std::vector<rofl::cofmsg_table_stats_request>(); }
std::vector<rofl::cofmsg_table_stats_reply> cportvlan_messagetranslator::translate_table_stats_reply( const rofl::cofdpt * , rofl::cofmsg_table_stats_reply * ) { return std::vector<rofl::cofmsg_table_stats_reply>(); }

std::vector<rofl::cofmsg_port_stats_request> cportvlan_messagetranslator::translate_port_stats_request( const rofl::cofctl * , rofl::cofmsg_port_stats_request * ) { return std::vector<rofl::cofmsg_port_stats_request>(); }
std::vector<rofl::cofmsg_flow_stats_request> cportvlan_messagetranslator::translate_flow_stats_request( const rofl::cofctl * , rofl::cofmsg_flow_stats_request * ) { return std::vector<rofl::cofmsg_flow_stats_request>(); }
std::vector<rofl::cofmsg_aggr_stats_request> cportvlan_messagetranslator::translate_aggregate_stats_request( const rofl::cofctl * , rofl::cofmsg_aggr_stats_request * ) { return std::vector<rofl::cofmsg_aggr_stats_request>(); }
std::vector<rofl::cofmsg_aggr_stats_reply> cportvlan_messagetranslator::translate_aggregate_stats_reply( const rofl::cofdpt * , rofl::cofmsg_aggr_stats_reply * ) { return std::vector<rofl::cofmsg_aggr_stats_reply>(); }

std::vector<rofl::cofmsg_queue_stats_request> cportvlan_messagetranslator::translate_queue_stats_request( const rofl::cofctl * , rofl::cofmsg_queue_stats_request * ) { return std::vector<rofl::cofmsg_queue_stats_request>(); }
std::vector<rofl::cofmsg_stats_request> cportvlan_messagetranslator::translate_experimenter_stats_request( const rofl::cofctl * , rofl::cofmsg_stats_request * ) { return std::vector<rofl::cofmsg_stats_request>(); }
std::vector<rofl::cofmsg_packet_in> cportvlan_messagetranslator::translate_packet_in( const rofl::cofdpt * , rofl::cofmsg_packet_in * ) { return std::vector<rofl::cofmsg_packet_in>(); }
std::vector<rofl::cofmsg_packet_out> cportvlan_messagetranslator::translate_packet_out( const rofl::cofctl * , rofl::cofmsg_packet_out * ) { return std::vector<rofl::cofmsg_packet_out>(); }
std::vector<rofl::cofmsg_barrier_request> cportvlan_messagetranslator::translate_barrier_request( const rofl::cofctl * , rofl::cofmsg_barrier_request * ) { return std::vector<rofl::cofmsg_barrier_request>(); }
std::vector<rofl::cofmsg_barrier_reply> cportvlan_messagetranslator::translate_barrier_reply ( const rofl::cofdpt * , rofl::cofmsg_barrier_reply * ) { return std::vector<rofl::cofmsg_barrier_reply>(); }
std::vector<rofl::cofmsg_table_mod> cportvlan_messagetranslator::translate_table_mod( const rofl::cofctl * , rofl::cofmsg_table_mod * ) { return std::vector<rofl::cofmsg_table_mod>(); }
std::vector<rofl::cofmsg_port_mod> cportvlan_messagetranslator::translate_port_mod( const rofl::cofctl * , rofl::cofmsg_port_mod * ) { return std::vector<rofl::cofmsg_port_mod>(); }
std::vector<rofl::cofmsg_set_config> cportvlan_messagetranslator::translate_set_config( const rofl::cofctl * , rofl::cofmsg_set_config * ) { return std::vector<rofl::cofmsg_set_config>(); }
std::vector<rofl::cofmsg_queue_get_config_request> cportvlan_messagetranslator::translate_queue_get_config_request( const rofl::cofctl * , rofl::cofmsg_queue_get_config_request * ) { return std::vector<rofl::cofmsg_queue_get_config_request>(); }
std::vector<rofl::cofmsg_features_request> cportvlan_messagetranslator::translate_experimenter_message( const rofl::cofctl * , rofl::cofmsg_features_request * ) { return std::vector<rofl::cofmsg_features_request>(); }
std::vector<rofl::cofmsg_flow_mod> cportvlan_messagetranslator::translate_flow_mod( const rofl::cofctl * , rofl::cofmsg_flow_mod * ) { return std::vector<rofl::cofmsg_flow_mod>(); }

#ifdef MSGTRANTEST

// to test: g++ ./cportvlan_messagetranslator.cc -lrofl -lpthread -lrt -DMSGTRANTEST

int main ( int, char ** ) {

	std::vector<cportvlan_messagetranslator::port_spec_t> list;
	list.push_back( cportvlan_messagetranslator::port_spec_t( PV_PORT_T(2), PV_VLANID_T(3) ) );
	list.push_back( cportvlan_messagetranslator::port_spec_t( PV_PORT_T(4), PV_VLANID_T::NONE ) );
	list.push_back( cportvlan_messagetranslator::port_spec_t( PV_PORT_T(2), PV_VLANID_T(5) ) );
	list.push_back( cportvlan_messagetranslator::port_spec_t( PV_PORT_T(3), PV_VLANID_T(5) ) );
	list.push_back( cportvlan_messagetranslator::port_spec_t( PV_PORT_T(4), PV_VLANID_T(2) ) );
	list.push_back( cportvlan_messagetranslator::port_spec_t( PV_PORT_T(7), PV_VLANID_T::NONE ) );

	cportvlan_messagetranslator mapper(list.begin(), list.end());

//	mapper.add_virtual_port( cportvlan_messagetranslator::port_spec_t( PV_PORT_T(1), PV_VLANID_T::ANY ) );
	
	std::cout << "installed mapper: " << mapper << std::endl;
	
	cportvlan_messagetranslator::port_spec_t tests [] = {
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T::ANY, PV_VLANID_T::ANY ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T(2),   PV_VLANID_T::ANY ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T(2),   PV_VLANID_T(4) ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T(4),   PV_VLANID_T(1) ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T(2),   PV_VLANID_T(3) ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T::ANY, PV_VLANID_T(5) ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T(4),   PV_VLANID_T::ANY ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T(4),   PV_VLANID_T::NONE ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T(3),   PV_VLANID_T::NONE ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T(1),   PV_VLANID_T::NONE ),
		cportvlan_messagetranslator::port_spec_t( PV_PORT_T::ANY, PV_VLANID_T::NONE ),
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
