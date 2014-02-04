#include <rofl/platform/unix/cunixenv.h>
#include <rofl/platform/unix/cdaemon.h>
#include "etherswitch.h"

#define ETHSWCTLD_LOG_FILE "/var/log/ethswctld.log"
#define ETHSWCTLD_PID_FILE "/var/run/ethswctld.pid"

int
main(int argc, char** argv)
{
	rofl::cunixenv env_parser(argc, argv);

	/* update defaults */
	env_parser.update_default_option("logfile", ETHSWCTLD_LOG_FILE);
	env_parser.add_option(coption(true, REQUIRED_ARGUMENT, 'p', "pidfile", "set pid-file", std::string(ETHSWCTLD_PID_FILE)));

	//Parse
	env_parser.parse_args();

	if (not env_parser.is_arg_set("daemonize")) {
		// only do this in non
		std::string ident(env_parser.get_arg("logfile"));

		csyslog::initlog(csyslog::LOGTYPE_FILE,
				static_cast<csyslog::DebugLevel>(atoi(env_parser.get_arg("debug").c_str())), // todo needs checking
				env_parser.get_arg("logfile"),
				ident.c_str());

		rofl::logging::set_debug_level(atoi(env_parser.get_arg("debug").c_str()));

	} else {

		rofl::cdaemon::daemonize(env_parser.get_arg("pidfile"), env_parser.get_arg("logfile"));
		rofl::logging::set_debug_level(atoi(env_parser.get_arg("debug").c_str()));
		rofl::logging::notice << "[ethswctld][main] daemonizing successful" << std::endl;
	}

	cofhello_elem_versionbitmap versionbitmap;
	versionbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
	etherswitch::ethswitch sw(versionbitmap);

	sw.rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6633));
	sw.rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6632));

	rofl::cioloop::run();

	rofl::cioloop::shutdown();

	return 0;
}

