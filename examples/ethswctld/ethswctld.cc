/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl_config.h"
#include <rofl/platform/unix/cunixenv.h>
#include <rofl/platform/unix/cdaemon.h>
#include <rofl/common/cparams.h>

#include "etherswitch.h"

#define ETHSWCTLD_LOG_FILE "/var/log/ethswctld.log"
#define ETHSWCTLD_PID_FILE "/var/run/ethswctld.pid"

int
main(int argc, char** argv)
{
	rofl::cunixenv env_parser(argc, argv);

	/* update defaults */
	env_parser.add_option(coption(true,REQUIRED_ARGUMENT,'l',"logfile","Log file used when daemonization", ETHSWCTLD_LOG_FILE));
	env_parser.add_option(coption(true, REQUIRED_ARGUMENT, 'p', "pidfile", "set pid-file", std::string(ETHSWCTLD_PID_FILE)));
#ifdef ROFL_HAVE_OPENSSL
	env_parser.add_option(coption(true, REQUIRED_ARGUMENT, 't', "cert-and-key-file", "Certificate and key to encrypt control traffic (PEM format)", std::string("")));
#endif
	//Parse
	env_parser.parse_args();

	if (not env_parser.is_arg_set("daemonize")) {
		// only do this in non
		std::string ident(env_parser.get_arg("logfile"));

		logging::init();
		rofl::logging::set_debug_level(atoi(env_parser.get_arg("debug").c_str()));
	} else {

		rofl::cdaemon::daemonize(env_parser.get_arg("pidfile"), env_parser.get_arg("logfile"));
		rofl::logging::set_debug_level(atoi(env_parser.get_arg("debug").c_str()));
		rofl::logging::notice << "[ethswctld][main] daemonizing successful" << std::endl;
	}

	rofl::openflow::cofhello_elem_versionbitmap versionbitmap;
	versionbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
	etherswitch::ethswitch sw(versionbitmap);

	rofl::cparams socket_params = csocket::get_default_params(rofl::csocket::SOCKET_TYPE_PLAIN);
	socket_params.set_param(csocket::PARAM_KEY_LOCAL_PORT).set_string() = std::string("6653");
	sw.rpc_listen_for_dpts(rofl::csocket::SOCKET_TYPE_PLAIN, socket_params);

	rofl::cioloop::run();

	rofl::cioloop::shutdown();

	return 0;
}

