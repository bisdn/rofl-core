/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cgetopt.h"
#include "cudpsend.h"
#include "cudprecv.h"

void
usage();

int
main(int argc, char** argv)
{
	unixenv::cgetopt& getopt = unixenv::cgetopt::get_instance();

	getopt.add_long_option("help",	 	getopt.NO_ARG);
	getopt.add_long_option("sender", 	getopt.NO_ARG);
	getopt.add_long_option("receiver", 	getopt.NO_ARG);
	getopt.add_long_option("local", 	getopt.REQUIRED_ARG);
	getopt.add_long_option("remote",  	getopt.REQUIRED_ARG);
	getopt.add_long_option("duration",	getopt.REQUIRED_ARG);
	getopt.add_long_option("size",		getopt.REQUIRED_ARG);

	getopt.parse(argc, argv);

	if (getopt.has_opt("help")) {
		usage();
	}

	rofl::csockaddr local(AF_INET, "0.0.0.0", 5001);
	if (getopt.has_opt("local")) {
		std::string s_addr = getopt.get_opt("local").substr(0, getopt.get_opt("local").find_first_of(":"));
		std::string s_port = getopt.get_opt("local").substr(getopt.get_opt("local").find_first_of(":")+1);
		local = rofl::csockaddr(AF_INET, s_addr.c_str(), atoi(s_port.c_str()));
	}

	rofl::csockaddr remote(AF_INET, "0.0.0.0", 6001);
	if (getopt.has_opt("remote")) {
		std::string s_addr = getopt.get_opt("remote").substr(0, getopt.get_opt("remote").find_first_of(":"));
		std::string s_port = getopt.get_opt("remote").substr(getopt.get_opt("remote").find_first_of(":")+1);
		remote = rofl::csockaddr(AF_INET, s_addr.c_str(), atoi(s_port.c_str()));
	}

	std::cerr << "local: " << local << std::endl;
	std::cerr << "remote: " << remote << std::endl;

	int duration = 30;
	if (getopt.has_opt("duration")) {
		duration = atoi(getopt.get_opt("duration").c_str());
	}
	(void)duration;

	unsigned int msglen = 1472;
	if (getopt.has_opt("size")) {
		msglen = atoi(getopt.get_opt("size").c_str());
	}
	(void)msglen;
	msglen = (msglen < 22) ? 22 : msglen;
	msglen = (msglen > 1472) ? 1472 : msglen;

	if (getopt.has_opt("sender")) {

		spray::cudpsend udpsend(remote, local, msglen);

		udpsend.start_sending(duration);

		//sleep(duration);

		//udpsend.stop_sending();

		rofl::ciosrv::run();

	} else if (getopt.has_opt("receiver")) {

		spray::cudprecv udprecv(remote, local);

		udprecv.start_receiving();

		//sleep(10);

		//udprecv.stop_receiving();

		rofl::ciosrv::run();

	} else {
		std::cerr << "unknown mode, aborting." << std::endl;
		return -1;
	}

	return 0;
}


void
usage()
{
	fprintf(stderr, "spray\n"
			"\t[--sender|--receiver]\n"
			"\t[--local <ipaddr:port>]\n"
			"\t[--remote <ipaddr:port>]\n"
			"\t[--duration <of mesaurement in seconds>]\n"
			"\t[--size <of UDP messages in bytes>]\n");
	exit(0);
}
