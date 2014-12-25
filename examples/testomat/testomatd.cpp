#include "rofl_config.h"
#include <rofl/platform/unix/cunixenv.h>
#include <rofl/platform/unix/cdaemon.h>
#include <rofl/common/cparams.h>

#include "ccontrol.hpp"

#define ETHSWCTLD_LOG_FILE "/var/log/testomatd.log"
#define ETHSWCTLD_PID_FILE "/var/run/testomatd.pid"

int
main(int argc, char** argv)
{
	rofl::cunixenv env_parser(argc, argv);

	/* update defaults */
	env_parser.add_option(rofl::coption(true,REQUIRED_ARGUMENT,'l',"logfile","Log file used when daemonization", ETHSWCTLD_LOG_FILE));
	env_parser.add_option(rofl::coption(true, REQUIRED_ARGUMENT, 'p', "pidfile", "set pid-file", std::string(ETHSWCTLD_PID_FILE)));

	//Parse
	env_parser.parse_args();

	if (not env_parser.is_arg_set("daemonize")) {
		// only do this in non
		std::string ident(env_parser.get_arg("logfile"));

		rofl::logging::init();
		rofl::logging::set_debug_level(atoi(env_parser.get_arg("debug").c_str()));
	} else {

		rofl::cdaemon::daemonize(env_parser.get_arg("pidfile"), env_parser.get_arg("logfile"));
		rofl::logging::set_debug_level(atoi(env_parser.get_arg("debug").c_str()));
		rofl::logging::notice << "[ethswctld][main] daemonizing successful" << std::endl;
	}

	rofl::openflow::cofhello_elem_versionbitmap versionbitmap;
	versionbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
	testomat::ccontrol ctl(versionbitmap);

	rofl::cparams socket_params = rofl::csocket::get_default_params(rofl::csocket::SOCKET_TYPE_PLAIN);
	socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string() = std::string("6653");
	ctl.add_dpt_listening(0, rofl::csocket::SOCKET_TYPE_PLAIN, socket_params);

	rofl::cioloop::get_loop().run();

	rofl::cioloop::get_loop().shutdown();

	return 0;
}

