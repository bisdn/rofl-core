
int main(int, char**) {
/* update defaults */
	rofl_set_logging_level(DBG_VERBOSE_LEVEL);
	rofl::csyslog::initlog( rofl::csyslog::LOGTYPE_STDERR, rofl::csyslog::DBG, std::string("/home/alien/tranny.log"), "tranny: ");
	
	signal(SIGINT, interrupt_handler);

	rofl::csyslog::set_debug_level("ciosrv", "dbg");
	rofl::csyslog::set_debug_level("cthread", "dbg");
	rofl::csyslog::set_debug_level("csocket", "dbg");
	rofl::csyslog::set_debug_level("cofpacket", "dbg");

	rofl::ciosrv::init();

	ctranslator tranny;

///	tranny.rpc_connect_to_ctl(rofl::caddress(AF_INET, "127.0.0.1", 6633));	// doxygen definition is invalid
	tranny.rpc_listen_for_dpts(rofl::caddress(AF_INET, "0.0.0.0", 16633));

///	sleep(3);       // TODO should check for valid connections throughout handlers - this is just a hack

//	tranny.rpc_connect_to_ctl(OFP12_VERSION,3,rofl::caddress(AF_INET, "127.0.0.1", 6633));	// working, but moved handle_dpath_open

	rofl::ciosrv::run();

	rofl::ciosrv::destroy();
	
	ROFL_INFO("House cleaned!\nGoodbye\n");
	
	exit(EXIT_SUCCESS);

}
