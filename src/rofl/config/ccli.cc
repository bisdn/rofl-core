/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ccli.cc
 *
 *  Created on: Jul 12, 2012
 *      Author: tobi
 */

#include "ccli.h"

ccli::ccli(u_int16_t port) :
	cli_fd(-1),
	baddr(AF_INET, "0.0.0.0", port)
{
	cpopen(baddr, PF_INET, SOCK_STREAM);

	// Must be called first to setup data structures
	cli = cli_init();

	// Set the hostname (shown in the the prompt)
	cli_set_hostname(cli, "rofl");

	// Set the greeting
	cli_set_banner(cli, "Welcome to the rofl CLI.");

	// Enable 2 username / password combinations
	//cli_allow_user(cli, "fred", "nerk");
	//cli_allow_user(cli, "foo", "bar");


	WRITELOG(CLI, ROFL_DBG, "ccli::ccli(%p) created", this);
}


ccli::~ccli()
{
	cli_done(cli);
}

/*virtual*/void
ccli::handle_accepted(int x, caddress &ra)
{
	if(this->cli_fd != -1) {
		close(x);
		WRITELOG(CLI, ROFL_DBG, "ccli(%p)::handle_accepted() closing %d (already connected)", this, x);
		return;
	}
	this->cli_fd = x;

	WRITELOG(CLI, ROFL_DBG, "pre ccli(%p)::handle_accepted() accept %d", this, x);

	pthread_t thread;
	int rc = pthread_create(&thread, NULL, ccli::run_terminal, this);

	if (rc != 0) {
#if 0
		switch (errno) {
		case EINVAL:
			// invalid attributes
			break;
		case EPERM:
			// permission denied for scheduling priorities as defined in attr
			break;
		case EAGAIN:
			// no resources, retry?
			break;
		}
#endif
		WRITELOG(CLI, ERROR, "ccli(%p)::handle_accepted() failed due to:%s", this, strerror(errno));
		this->cli_fd = -1;
	}
}

void
ccli::read_config_file(const std::string& filename) throw (eCliConfigFileNotFound)
{
	WRITELOG(CLI, ROFL_INFO, "open config file %s", filename.c_str());
	FILE *fh = fopen(filename.c_str(), "r");
	if (NULL != fh) {
		cli_file(this->cli, fh, PRIVILEGE_UNPRIVILEGED, MODE_EXEC);
		fclose(fh);
	} else {
		WRITELOG(CLI, ERROR, "config file not found %s", filename.c_str());
		throw eCliConfigFileNotFound();
	}
}

void *
ccli::run_terminal(void* arg)
{
	ccli *cli = (ccli*)arg;

	// Pass the connection off to libcli
	cli_loop(cli->cli, cli->cli_fd);
	close(cli->cli_fd);
	cli->cli_fd = -1;
	return NULL;
}
