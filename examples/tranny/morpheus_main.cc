#include <signal.h>
#include <iostream>
#include <rofl/common/crofbase.h>
#include <rofl/common/utils/c_logger.h>
#include "morpheus.h"
#include "cportvlan_mapper.h"

// to catch SIGINT
void interrupt_handler(int dummy=0) {
	rofl::ciosrv::stop();
}

int main(int, char**) {

	rofl_set_logging_level(DBG_VERBOSE_LEVEL);
	rofl::csyslog::initlog( rofl::csyslog::LOGTYPE_STDERR, rofl::csyslog::DBG, std::string("/home/alien/morpheus.log"), "tranny: "); // first argument sets the type, second the level, third is logging file is logging to file (ignored for LOGTYPE_STDERR), fourth is prefix tag for log entries.
	
	signal(SIGINT, interrupt_handler);

/*
	rofl::csyslog::set_debug_level("ciosrv", "dbg");
	rofl::csyslog::set_debug_level("cthread", "dbg");
	rofl::csyslog::set_debug_level("csocket", "dbg");
	rofl::csyslog::set_debug_level("cofpacket", "dbg");
	rofl::csyslog::set_debug_level("crofbase", "dbg");
*/
	rofl::csyslog::set_all_debug_levels(rofl::csyslog::DBG);

//	rofl::csyslog::set_all_debug_levels(rofl::csyslog::INFO);
	rofl::csyslog::set_debug_level("ciosrv", "emergency");
	rofl::csyslog::set_debug_level("cthread", "emergency");

	rofl::ciosrv::init();

	cportvlan_mapper mapper;
	mapper.add_virtual_port( cportvlan_mapper::port_spec_t( PV_PORT_T(1), PV_VLANID_T::NONE ));
	mapper.add_virtual_port( cportvlan_mapper::port_spec_t( PV_PORT_T(2), PV_VLANID_T(10) ) );
	mapper.add_virtual_port( cportvlan_mapper::port_spec_t( PV_PORT_T(2), PV_VLANID_T(11) ) );
	mapper.add_virtual_port( cportvlan_mapper::port_spec_t( PV_PORT_T(2), PV_VLANID_T(12) ) );
	mapper.add_virtual_port( cportvlan_mapper::port_spec_t( PV_PORT_T(2), PV_VLANID_T(13) ) );

	morpheus morph (mapper);

///	morph.rpc_connect_to_ctl(rofl::caddress(AF_INET, "127.0.0.1", 6633));	// doxygen definition is invalid
	morph.rpc_listen_for_dpts(rofl::caddress(AF_INET, "0.0.0.0", 16633));
///	morph.rpc_listen_for_dpts(rofl::caddress(AF_INET, "10.100.0.1", 16633));

///	sleep(3);       // TODO should check for valid connections throughout handlers - this is just a hack

//	morph.rpc_connect_to_ctl(OFP12_VERSION,3,rofl::caddress(AF_INET, "127.0.0.1", 6633));	// working, but moved handle_dpath_open

	rofl::ciosrv::run();

	rofl::ciosrv::destroy();
	
	ROFL_INFO("House cleaned!\nGoodbye\n");
	
	exit(EXIT_SUCCESS);

}
