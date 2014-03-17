#include <rofl/common/logging.h>
#include <rofl/platform/unix/cunixenv.h>
#include "queuetest.h"

int
main(int argc, char** argv)
{

	rofl::cunixenv env_parser(argc, argv);
	
	//Parse
	env_parser.parse_args();

	/* update defaults */
	rofl::logging::init();
	rofl::logging::set_debug_level(atoi(env_parser.get_arg("debug").c_str()));

	cofhello_elem_versionbitmap versionbitmap;
	versionbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
	queuetest qtest(versionbitmap);

	qtest.rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6633));
	qtest.rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6632));

	rofl::ciosrv::run();

	return 0;
}

