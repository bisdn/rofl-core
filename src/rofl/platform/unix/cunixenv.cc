/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cunixenv.h"


extern char* optarg;

/*static*/int cunixenv::debug = csyslog::EMERGENCY;
/*static*/int cunixenv::n_buffers = 262144;
/*static*/int cunixenv::block_size = 64;
/*static*/int cunixenv::n_blocks = 1;
/*static*/int cunixenv::frame_size = 2048;
/*static*/int cunixenv::n_fwdengines = 1;
/*static*/std::string cunixenv::controller_ip("127.0.0.1");
/*static*/u_int16_t cunixenv::controller_port = 6633;
/*static*/std::string cunixenv::config_filename("./default-cli.cfg");
/*static*/int cunixenv::enable_hal = 0;

void
cunixenv::usage(
		char *argv0)
{
	fprintf(stderr, "usage: %s [--daemonize] [--help]\n", argv0);
	fprintf(stderr, "[--debug|-d] <int:debuglevel>\n");
	fprintf(stderr, "[--blocksize|-s] <int:#pagenumbers>\n");
	fprintf(stderr, "[--blocks|-k] <int>\n");
	fprintf(stderr, "[--framesize|-r] <int:bytes>\n");
	fprintf(stderr, "[--buffers|-b] <int:#buffers>\n");
	fprintf(stderr, "[--fwdengines|-f] <int:#fwdengines>\n");
	fprintf(stderr, "[--configfile|-c] <string:filename>\n");
	fprintf(stderr, "[--ctl_ip_addr|-i] <ip:127.0.0.1>\n");
	fprintf(stderr, "[--ctl_port|-p] <int:#portnum>\n");
	fprintf(stderr, "[--hal|-a] <int: default(0) HAL(1)>\n");
	exit(0);
}


void
cunixenv::parse_args(
		int argc,
		char** argv)
{
	int c;
	int option_index;

	static struct option long_options[] = {
			{ "daemonize", 0, 0, 'D' },
			{ "help", 0, 0, 'h' },
			{ "debug", 1, 0, 'd' },
			{ "buffers", 1, 0, 'b' },
			{ "blocksize", 1, 0, 's' },
			{ "blocks", 1, 0, 'k' },
			{ "framesize", 1, 0, 'r' },
			{ "fwdengines", 1, 0, 'f' },
			{ "configfile", 1, 0, 'c' },
			{ "ctl_ip_addr", 1, 0, 'i' },
			{ "ctl_port", 1, 0, 'p' },
			{ "hal", 1, 0, 'a' },
			{ NULL, 0, 0, 0 }
	};

	while (true) {
		c = getopt_long(argc, argv, "Dhd:b:s:k:r:f:c:i:p:a:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			std::cerr << "option " << long_options[option_index].name;
			if (optarg)
				std::cerr << " with optarg " << optarg;
			std::cerr << std::endl;
			break;

		case 'D':
			detach();
			break;

		case 'h':
			usage(argv[0]);
			break;

		case 'd':
			debug = atoi(optarg);
			break;

		case 'b':
			n_buffers = atoi(optarg);
			break;

		case 's':
			block_size = atoi(optarg);
			break;

		case 'k':
			n_blocks = atoi(optarg);
			break;

		case 'r':
			frame_size = atoi(optarg);
			break;

		case 'f':
			n_fwdengines = atoi(optarg);
			break;

		case 'c':
			config_filename = std::string(optarg);
			break;

		case 'i':
			controller_ip = std::string(optarg);
			break;

		case 'p':
			controller_port = atoi(optarg);
			break;

		case 'a':
			enable_hal = atoi(optarg);
			break;
		}
	}

}


void
cunixenv::detach()
{
	pid_t pid = fork();

	if (pid < 0) // error occured
	{
		fprintf(stderr, "fork() sys-call failed: %d (%s)\n", errno, strerror(errno));
		exit(-1);
	}
	else if (pid > 0) // parent exit
	{
		exit(0);
	}

	// child code


	if ((pid = setsid()) < 0) // detach from controlling terminal
	{
		fprintf(stderr, "setsid() sys-call failed: %d (%s)\n", errno, strerror(errno));
		exit(-1);
	}


	// redirect stdout, stderr
	csyslog::initlog(csyslog::LOGTYPE_FILE); 	// reinitialize logging to logtype file

	dup2(STDOUT_FILENO, csyslog::getfd()); // redirect stdout
	dup2(STDERR_FILENO, csyslog::getfd()); // redirect stderr

	// set file mask
	umask(027);

	// change working directory
	if (chdir("/var/tmp") < 0)
	{
		fprintf(stderr, "chdir() sys-call failed: %d (%s)\n", errno, strerror(errno));
		exit(-1);
	}
}

