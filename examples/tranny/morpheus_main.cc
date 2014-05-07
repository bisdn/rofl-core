#include <signal.h>
#include <string>
#include <iostream>
#include <rofl/common/crofbase.h>
#include <rofl/common/caddress.h>
#include <rofl/common/utils/c_logger.h>
#include "morpheus.h"
#include "cportvlan_mapper.h"

// to catch SIGINT
void interrupt_handler(int dummy=0) {
	rofl::ciosrv::stop();
}


bool parse_addr_port (std::string instring, rofl::caddress & addrout, const uint16_t def_port) {
//			std::string straddrport(instring);
			if(instring.find_first_not_of("0123456789.:") != std::string::npos) {
				return false;
			}
			size_t colon = instring.find(':');
			uint16_t port = def_port;
			std::string s, addrstr;
			if(colon!=std::string::npos) {
				addrstr = instring.substr(0,colon);
				s = instring.substr(colon+1);
				std::cout << "About to parse port from " << s << "." << std::endl;
				if(s.find_first_not_of("0123456789")!=std::string::npos) return false;
				std::istringstream iss(s);
				iss >> port;
			} else {
				addrstr = instring;
			}
			std::cout << "About to return a caddress with _" << addrstr << "_" << port << "_" << std::endl;
			addrout = rofl::caddress(AF_INET, addrstr.c_str(), port);	// TODO report this as a "bug" - the caddress constructor prototypes are too similar - c_str() is required to make sure the right one is chosen.
			return true;
}

int main(int argc, char** argv) {

	rofl_set_logging_level(DBG_VERBOSE_LEVEL);
	rofl::csyslog::initlog( rofl::csyslog::LOGTYPE_STDERR, rofl::csyslog::DBG, std::string("/home/alien/morpheus.log"), "tranny: "); // first argument sets the type, second the level, third is logging file is logging to file (ignored for LOGTYPE_STDERR), fourth is prefix tag for log entries.
	
	signal(SIGINT, interrupt_handler);
	bool indpt = true, inctl = false;	// by default listen for connection from dpt, and make connection to controller
	uint16_t default_dpt_port = 16633;
	uint16_t default_ctl_port = 6633;
	rofl::caddress defaultdptaddr(AF_INET, "0.0.0.0", default_dpt_port);	// listen on all interfaces for dpt connection
	rofl::caddress defaultctladdr(AF_INET, "127.0.0.1", default_ctl_port);	// make active connection to controller on localhost

	rofl::caddress dptaddr, ctladdr;
	uint16_t dptport = default_dpt_port, ctlport = default_ctl_port;
	bool dptset = false, ctlset = false;
	dptaddr = defaultdptaddr;
	ctladdr = defaultctladdr;
	for( int i = 1; i< argc; ++i ) {
		if(std::string(argv[i])=="-dptin") {
			if(dptset) {std::cout << "Options dptin or dptout can only be specified once." << std::endl; return 1;}
			indpt = true;
			dptset = true;
			if(i == (argc-1)) { std::cout << "Must specify address and optional port after dptin." << std::endl; return 1; }
			++i;
			if(!parse_addr_port(argv[i], dptaddr, default_dpt_port)){ std::cout << "Argument to dptin must be of the format 1.2.3.4:5678, and not " << argv[i] << std::endl; return 1; }
			else { std::cout << "dptin parsed to be " << dptaddr << std::endl; }
		} else if(std::string(argv[i])=="-dptout") {
			if(dptset) {std::cout << "Options dptin or dptout can only be specified once." << std::endl; return 1;}
			indpt = false;
			dptset = true;
			if(i == (argc-1)) { std::cout << "Must specify address and optional port after dptout." << std::endl; return 1; }
			++i;
			if(!parse_addr_port(argv[i], dptaddr, default_dpt_port)){ std::cout << "Argument to dptin must be of the format 1.2.3.4:5678, and not " << argv[i] << std::endl; return 1; }
			else { std::cout << "dptin parsed to be " << dptaddr << std::endl; }
		} else if(std::string(argv[i])=="-ctlin") {
			if(ctlset) {std::cout << "Options ctlin or ctlout can only be specified once." << std::endl; return 1;}
			inctl = true;
			ctlset = true;
			if(i == (argc-1)) { std::cout << "Must specify address and optional port after ctlin." << std::endl; return 1; }
			++i;
			if(!parse_addr_port(argv[i], ctladdr, default_ctl_port)){ std::cout << "Argument to ctlin must be of the format 1.2.3.4:5678, and not " << argv[i] << std::endl; return 1; }
			else { std::cout << "ctlin parsed to be " << ctladdr << std::endl; }
		} else if(std::string(argv[i])=="-ctlout") {
			if(ctlset) {std::cout << "Options ctlin or ctlout can only be specified once." << std::endl; return 1;}
			inctl = false;
			ctlset = true;
			if(i == (argc-1)) { std::cout << "Must specify address and optional port after ctlout." << std::endl; return 1; }
			++i;
			if(!parse_addr_port(argv[i], ctladdr, default_ctl_port)){ std::cout << "Argument to ctlout must be of the format 1.2.3.4:5678, and not " << argv[i] << std::endl; return 1; }
			else { std::cout << "ctlout parsed to be " << ctladdr << std::endl; }
		} else {
			std::cout << "Uknown command line argument: " << argv[i] << std::endl;
			return 1;
		}
	}

//	rofl::caddress dptaddr(AF_INET, "0.0.0.0", default_dpt_port);	// listen on all interfaces for dpt connection
//	rofl::caddress ctladdr(AF_INET, "127.0.0.1", default_ctl_port));	// make active connection to controller on localhost

	std::cout << "DPE connections will be " << (indpt?"PASSIVE at ":"ACTIVE to ") << dptaddr.c_str() << "." << std::endl;
	std::cout << "Controller connections will be " << (inctl?"PASSIVE at ":"ACTIVE to ") << ctladdr.c_str() << "." << std::endl;

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

	morpheus morph (mapper, indpt, dptaddr, inctl, ctladdr);

///	morph.rpc_connect_to_ctl(rofl::caddress(AF_INET, "127.0.0.1", 6633));	// doxygen definition is invalid
////	morph.rpc_listen_for_dpts(rofl::caddress(AF_INET, "0.0.0.0", 16633));	// for general floodlight use
///	morph.rpc_listen_for_dpts(rofl::caddress(AF_INET, "10.100.0.1", 16633));	// for the oftest config

///	sleep(3);       // TODO should check for valid connections throughout handlers - this is just a hack

//	morph.rpc_connect_to_ctl(OFP12_VERSION,3,rofl::caddress(AF_INET, "127.0.0.1", 6633));	// working, but moved handle_dpath_open

	rofl::ciosrv::run();

	rofl::ciosrv::destroy();
	
	ROFL_INFO("House cleaned!\nGoodbye\n");
	
	exit(EXIT_SUCCESS);

}
