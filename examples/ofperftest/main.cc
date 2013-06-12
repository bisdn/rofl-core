#include <rofl/platform/unix/cunixenv.h>

#include "ofperftest.h"
#include "match_eth_dst.h"

void usage(int argc, char** argv);

int
main(int argc, char** argv)
{
	if (argc < 2) {
		usage(argc, argv);
	}
	std::string s_testcase(argv[1]);


	/* update defaults */
	rofl::csyslog::initlog(
			rofl::csyslog::LOGTYPE_STDERR,
			rofl::csyslog::EMERGENCY,
			std::string("ofperftest.log"),
			"an example: ");

	rofl::csyslog::set_debug_level("ciosrv", "emergency");
	rofl::csyslog::set_debug_level("cthread", "emergency");



	ofperftest* perftest;

	if (s_testcase == std::string("match_eth_dst")) {
		perftest = new match_eth_dst();
	} else {
		fprintf(stderr, "testcase %s not found, aborting\n", s_testcase.c_str());
		exit(1);
	}

	perftest->rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6633));
	perftest->rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6632));

	perftest->run();

	return 0;
}



void
usage(int argc, char** argv)
{
	fprintf(stderr, "testcases for %s:\n", argv[0]);
	fprintf(stderr, "match_eth_dst\n");

	exit(0);
}
