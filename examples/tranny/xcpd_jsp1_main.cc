#include <signal.h>
#include <iostream>
#include <rofl/common/crofbase.h>
#include <rofl/common/utils/c_logger.h>
#include "xcpd_jsp1.h"
#include "cportvlan_messagetranslator.h"

// to catch SIGINT
void interrupt_handler(int dummy=0) {
	rofl::ciosrv::stop();
}

int main(int, char**) {

	rofl_set_logging_level(DBG_VERBOSE_LEVEL);
	rofl::csyslog::initlog( rofl::csyslog::LOGTYPE_STDERR, rofl::csyslog::DBG, std::string("/home/alien/xcpd_jsp1.log"), "tranny: "); // first argument sets the type, second the level, third is logging file is logging to file (ignored for LOGTYPE_STDERR), fourth is prefix tag for log entries.
	
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

	cportvlan_messagetranslator::port_list_t list;
	list.push_back(cportvlan_messagetranslator::port_spec_t(0,cportvlan_messagetranslator::NO_VLAN));
	list.push_back(cportvlan_messagetranslator::port_spec_t(1,10));
	list.push_back(cportvlan_messagetranslator::port_spec_t(2,10));
	list.push_back(cportvlan_messagetranslator::port_spec_t(3,11));
	list.push_back(cportvlan_messagetranslator::port_spec_t(4,12));


	xcpd_jsp1 xcpd1;

///	xcpd1.rpc_connect_to_ctl(rofl::caddress(AF_INET, "127.0.0.1", 6633));	// doxygen definition is invalid
	xcpd1.rpc_listen_for_dpts(rofl::caddress(AF_INET, "0.0.0.0", 16633));

///	sleep(3);       // TODO should check for valid connections throughout handlers - this is just a hack

//	xcpd1.rpc_connect_to_ctl(OFP12_VERSION,3,rofl::caddress(AF_INET, "127.0.0.1", 6633));	// working, but moved handle_dpath_open

	rofl::ciosrv::run();

	rofl::ciosrv::destroy();
	
	ROFL_INFO("House cleaned!\nGoodbye\n");
	
	exit(EXIT_SUCCESS);

}
