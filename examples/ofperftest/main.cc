#include <rofl/platform/unix/cunixenv.h>

#include "ofperftest.h"
#include "match_eth_dst.h"
#include "match_vlan_id.h"
#include "ipswitching.h"
#include "mmap_test.h"

void usage(int argc, char** argv);
void parse_args(int argc, char** argv);

std::string s_testcase;
unsigned int n_entries = 0;
std::string s_destip("127.0.0.1");
uint16_t destport = 5555;
unsigned int burst_interval = 10;
unsigned int pkt_interval = 500000;

int
main(int argc, char** argv)
{

	parse_args(argc, argv);

	/* update defaults */
	rofl::logging::init();

	cofhello_elem_versionbitmap versionbitmap;
	versionbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
	ofperftest* perftest;

	if (s_testcase == std::string("match_eth_dst")) {
		perftest = new match_eth_dst(versionbitmap, n_entries);
	} else if (s_testcase == std::string("match_vlan_id")) {
		perftest = new match_vlan_id(versionbitmap, n_entries);
	} else if (s_testcase == std::string("ipswitching")) {
		perftest = new ipswitching(versionbitmap, n_entries);
	} else if (s_testcase == std::string("mmap_test")) {
		mmap_test *mmaptest = new mmap_test(versionbitmap, caddress(AF_INET, "0.0.0.0", 4444), burst_interval, pkt_interval);
		mmaptest->udp_start_sending(caddress(AF_INET, s_destip.c_str(), destport));
		perftest = mmaptest;
	} else {
		fprintf(stderr, "testcase %s not found, aborting\n", s_testcase.c_str());
		exit(1);
	}

	//perftest->rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6633));
	//perftest->rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6632));

	perftest->run();

	return 0;
}



void
usage(int argc, char** argv)
{
	fprintf(stderr, "testcases for %s:\n", argv[0]);
	fprintf(stderr, "  match_eth_dst\n");
	fprintf(stderr, "  match_vlan_id\n");
	fprintf(stderr, "  ipswitching\n");

	exit(0);
}


void
parse_args(int argc, char** argv)
{

	int option_index = 0;
	static struct option long_options[] = {
	    {"testcase", required_argument, 0, 0},
	    {"entries", required_argument, 0, 0},
	    {"destip", required_argument, 0, 0},
	    {"destport", required_argument, 0, 0},
	    {"burstinterval", required_argument, 0, 0},
	    {"pktinterval", required_argument, 0, 0},
	    {0, 0, 0, 0}
	};



	while (1) {
		int c = getopt_long (argc, argv, "t:e:i:p:b:v:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 't':
			s_testcase.assign(optarg);
			break;

		case 'e':
			n_entries = atoi(optarg);
			break;

		case 'i':
			s_destip.assign(optarg);
			break;

		case 'p':
			destport = atoi(optarg);
			break;

		case 'b':
			burst_interval = atoi(optarg);
			break;

		case 'v':
			pkt_interval = atoi(optarg);
			break;

		case '?':
			break;

		default:
			usage(argc, argv);
		}
	}
}
