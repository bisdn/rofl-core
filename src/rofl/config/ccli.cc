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
#include "cconfigsyslog.h"

using namespace rofl;

// todo move into a separate config interface
static int
cmd_set_debuglevel(struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc)
{
	// usage
    if (1 > argc || 2 < argc ) {
        cli_print(cli, "usage: set debuglevel <class|all> <level>\r\n");
        return CLI_OK;
    }

    // todo code replication... needs refactoring

    std::string class_name(argv[0]);
    std::transform(class_name.begin(), class_name.end(), class_name.begin(),
        		::tolower);

    // one arg ending with '?' (handling class_name)
    if (1 == argc) {
    	if ('?' == *class_name.rbegin()) {

    		if (1 == class_name.length()) {
    			// print all
    			cli_print(cli, "usage: set debuglevel <class> <level>\r\n"
    					"classes:\r\n");

    			const std::vector<std::string>& names = csyslog::get_class_names();
    			for (std::vector<std::string>::const_iterator iter = names.begin();
    					iter != names.end(); ++iter) {
    				cli_print(cli, "\t%s", (*iter).c_str());
    			}
    		} else {
    			// print only these starting with argv
    			// print all
    			cli_print(cli, "usage: set debuglevel <class> <level>\r\n"
    					"classes:\r\n");

    			const std::vector<std::string>& names = csyslog::get_class_names();
    			for (std::vector<std::string>::const_iterator iter = names.begin();
    					iter != names.end(); ++iter) {
    				// compare without ending '?'
    				if ( 0 == (*iter).compare(0, class_name.length()-1, class_name,
    						0, class_name.length()-1) ) {
    					cli_print(cli, "\t%s", (*iter).c_str());
    				}
    			}
    		}
    	}
    	return	CLI_OK;
    }

    std::string level_name(argv[1]);
    std::transform(level_name.begin(), level_name.end(), level_name.begin(),
    		::tolower);
    // two arg last ending with '?' (handling level_name)
   	if ('?' == *level_name.rbegin()) {

   		if (1 == level_name.length()) {
   			// print all
   			cli_print(cli, "usage: set debuglevel %s <level>\r\n"
   					"levels:\r\n", class_name.c_str());

   			const std::vector<std::string>& names = csyslog::get_level_names();
   			for (std::vector<std::string>::const_iterator iter = names.begin();
   					iter != names.end(); ++iter) {
   				cli_print(cli, "\t%s", (*iter).c_str());
   			}
   		} else {
   			// print only these starting with argv
   			// print all
   			cli_print(cli, "usage: set debuglevel %s <level>\r\n"
   					"levels:\r\n", class_name.c_str());

   			const std::vector<std::string>& names = csyslog::get_level_names();
   			for (std::vector<std::string>::const_iterator iter = names.begin();
   					iter != names.end(); ++iter) {
   				// compare without ending '?'
   				if ( 0 == (*iter).compare(0, level_name.length()-1, level_name,
   						0, level_name.length()-1) ) {
   					cli_print(cli, "\t%s", (*iter).c_str());
   				}
   			}
   		}
   		return CLI_OK;
   	}


    if (0 == class_name.compare("all")) {
    	csyslog::set_all_debug_levels(level_name);
    } else {
    	csyslog::set_debug_level(class_name, level_name);
    }

	return CLI_OK;
}

static int
cmd_show_debuglevel(struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc)
{
	// usage
	if (1 > argc || 2 < argc ) {
		cli_print(cli, "usage: show debuglevel <class|all>\r\n");
		return CLI_OK;
	}

	std::string class_name(argv[0]);

	// todo refactor code duplication
	if ('?' == *class_name.rbegin()) {
		// print all debug classes

		if (1 == class_name.length()) {
			// print all
			cli_print(cli, "usage: show debuglevel <class|all>\r\n"
					"classes:\r\n");

			const std::vector<std::string>& names = csyslog::get_class_names();
			for (std::vector<std::string>::const_iterator iter = names.begin();
					iter != names.end(); ++iter) {
				cli_print(cli, "\t%s", (*iter).c_str());
			}
		} else {
			// print only these starting with argv
			// print all
			cli_print(cli, "usage: show debuglevel <class|all>\r\n"
					"classes:\r\n");

			const std::vector<std::string>& names = csyslog::get_class_names();
			for (std::vector<std::string>::const_iterator iter = names.begin();
					iter != names.end(); ++iter) {
				// compare without ending '?'
				if ( 0 == (*iter).compare(0, class_name.length()-1, class_name,
						0, class_name.length()-1) ) {
					cli_print(cli, "\t%s", (*iter).c_str());
				}
			}
		}

		return CLI_OK;
	}

	std::string out;
	if (0 == class_name.compare("all")) {
		// print all
		cconfigsyslog::getInstance().print_debug_level_all(&out);
	} else {
		// print only one class
		cconfigsyslog::getInstance().print_debug_level_of_class(class_name, &out);
	}

	cli_print(cli, "%s\r\n", out.c_str());

	return CLI_OK;
}



ccli::ccli(caddress addr) :
		socket(new csocket(this, PF_INET, SOCK_STREAM, IPPROTO_TCP, 10)),
		cli_fd(-1),
		baddr(addr)
{
	socket->cpopen(baddr, PF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Must be called first to setup data structures
	cli = cli_init();

	// Set the hostname (shown in the the prompt)
	cli_set_hostname(cli, "rofl");

	// Set the greeting
	cli_set_banner(cli, "Welcome to the rofl CLI.");

	// Enable 2 username / password combinations
	//cli_allow_user(cli, "fred", "nerk");
	//cli_allow_user(cli, "foo", "bar");


	struct cli_command *c1;


	// show
	c1 = cli_register_command(cli, NULL, "show", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
	// show debuglevel <class|all>
	cli_register_command(cli, c1, "debuglevel", cmd_show_debuglevel, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "show current debuglevel");

	// set <one of the following>
	c1 = cli_register_command(cli, NULL, "set", NULL, PRIVILEGE_PRIVILEGED, MODE_EXEC, NULL);
	// set debuglevel
	cli_register_command(cli, c1, "debuglevel", cmd_set_debuglevel, PRIVILEGE_PRIVILEGED, MODE_EXEC, NULL);


	WRITELOG(CLI, DBG, "ccli::ccli(%p) created", this);
}


ccli::~ccli()
{
	cli_done(cli);
}

/*virtual*/void
ccli::handle_accepted(
		csocket *socket,
		int x,
		caddress const& ra)
{
	if(this->cli_fd != -1) {
		socket->cclose();
		WRITELOG(CLI, DBG, "ccli(%p)::handle_accepted() closing %d (already connected)", this, x);
		return;
	}
	this->cli_fd = x;

	WRITELOG(CLI, DBG, "pre ccli(%p)::handle_accepted() accept %d", this, x);

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
	WRITELOG(CLI, INFO, "open config file %s", filename.c_str());
	FILE *fh = fopen(filename.c_str(), "r");
	if (NULL != fh) {
		cli_file(this->cli, fh, PRIVILEGE_UNPRIVILEGED, MODE_EXEC);
		fclose(fh);
	} else {
		WRITELOG(CLI, ERROR, "config file not found %s", filename.c_str());
		throw eCliConfigFileNotFound();
	}
}

struct cli_command *
ccli::find_command(const char * const name)
{
	cli_command *c2 = NULL;
	if (cli->commands) {

		for (c2 = cli->commands; c2; c2 = c2->next) {
			if (strcmp(c2->command, "show") == 0) {
				break;
			}
		}
	}
	return c2;
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
