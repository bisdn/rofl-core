#include <rofl/platform/unix/cunixenv.h>
#include "queuetest.h"

int
main(int argc, char** argv)
{
	/* update defaults */
	rofl::csyslog::initlog(
			rofl::csyslog::LOGTYPE_STDERR,
			rofl::csyslog::EMERGENCY,
			std::string("etherswitch.log"),
			"an example: ");

	rofl::csyslog::set_debug_level("ciosrv", "emergency");
	rofl::csyslog::set_debug_level("cthread", "emergency");


	rofl::ciosrv::init();

	queuetest qtest;

	qtest.rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6633));
	qtest.rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6632));

	rofl::ciosrv::run();

	return 0;
}

