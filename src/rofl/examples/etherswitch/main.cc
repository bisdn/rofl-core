#include "etherswitch.h"


int
main(int argc, char** argv)
{
	etherswitch ethswitch;

	ethswitch.rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6633));

	ciosrv::run();

	return 0;
}

