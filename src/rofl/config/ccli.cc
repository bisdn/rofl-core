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

enum mode_config {
	MODE_CONFIG_INTERFACE = (MODE_CONFIG + 1),
	MODE_CONFIG_OPENFLOW,
	MODE_CONFIG_OPENFLOW_CONFIGURE
};

static int
cmd_show_port(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc))
{
/*    if (strcmp(argv[0], "?") == 0)
        cli_print(cli, "  usage: show port [name]");*/

	// currently always all port information is printed
	std::string out;
	cconfigport::getInstance().print_port_information(&out);

	cli_print(cli, "%s", out.c_str());

	return CLI_OK;
}

static int
cmd_show_openflow(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc))
{
/*    if (strcmp(argv[0], "?") == 0)
        cli_print(cli, "  usage: show port [name]");*/

	// currently always all port information is printed
	std::string out;
	cconfigfwdelem::getInstance().print_datapath_information(&out);

	cli_print(cli, "%s", out.c_str());

	return CLI_OK;
}

static int
cmd_test(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    cli_print(cli, "called %s with %s\r\n"
    		"  mode is %d config_desc is %s", __FUNCTION__, command, cli->mode, cli->modestring);
    return CLI_OK;
}

/*static int cmd_set(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    if (argc < 2)
    {
	cli_print(cli, "Specify a variable to set\r\n");
	return CLI_OK;
    }
    cli_print(cli, "Setting %s to %s\r\n", argv[0], argv[1]);
    return CLI_OK;
}

static int
cmd_config_interface(struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc)
{
    if (argc < 1)
    {
        cli_print(cli, "Specify an interface to configure");
        return CLI_OK;
    }

    if (strcmp(argv[0], "?") == 0)
        cli_print(cli, "  test0/0");

    else if (strcasecmp(argv[0], "test0/0") == 0)
        cli_set_configmode(cli, MODE_CONFIG_INTERFACE, "test");
    else
        cli_print(cli, "Unknown interface %s", argv[0]);

    return CLI_OK;
}*/

static int
cmd_config_openflow(struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc)
{
	// todo check for argv = '?'
    cli_set_configmode(cli, MODE_CONFIG_OPENFLOW, "of");
    return CLI_OK;
}

static int
cmd_config_openflow_configure(struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc)
{
    if (argc < 1) {
        cli_print(cli, "Specify a datapath to configure\n"
        		"usage: configure <dpname>");
        return CLI_OK;
    }

    if (strcmp(argv[0], "?") == 0) {
    	std::string out;
    	cconfigfwdelem::getInstance().print_datapath_information(&out);
    	cli_print(cli, "%s", out.c_str());
    	return CLI_OK;
    }

    // todo check if there is more than one parameter?

    if (cconfigfwdelem::getInstance().is_datapath(std::string(argv[0]))) {
    	std::string config_mode("of-");
    	config_mode.append(argv[0]);

    	cli_set_configmode(cli, MODE_CONFIG_OPENFLOW_CONFIGURE, config_mode.c_str());
    } else {
    	cli_print(cli, "Unknown interface %s", argv[0]);
    }

    return CLI_OK;
}

static int
cmd_config_openflow_configure_attach_port(struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc)
{
    if (argc < 1) {
        cli_print(cli, "usage: attach_port <portname> <port_number>");
        return CLI_OK;
    }

    if (strcmp(argv[0], "?") == 0) {
    	std::string out;
    	cconfigport::getInstance().print_port_information(&out);
    	cli_print(cli, "Available ports:");
    	cli_print(cli, "%s", out.c_str());
    	return CLI_OK;
    }

    // todo check if there is more than one parameter?
    std::string portname(argv[0]);

    if (cconfigport::getInstance().is_port(portname)) {

    	std::string dpname(&cli->modestring[strlen("(config-of-")]);
    	dpname.erase(dpname.size()-1, 1); // remove ')'

    	uint32_t of_port_no = 0; /* invalid port number */

    	/* check if second parameter (port_number) is set */
    	if (2 == argc) {
    		errno = 0;
    		unsigned long int tmp = strtoul(argv[1], NULL, 10);

    		switch (errno) {
				case EINVAL:
				case ERANGE:
					break;
				default:
					of_port_no = tmp;
					break;
			}
    	}

    	// check port_number
    	if (0 == of_port_no) {
    		cli_print(cli, "port number has to be greater than 0", argv[0]);
    		return CLI_OK;
    	}

    	if (cconfigport::getInstance().attach_port_to_dp(portname, dpname, of_port_no)) {
    		cli_print(cli, "attach port %s", argv[0]);
    	} else {
    		cli_print(cli, "attach port %s to dp %s failed (already attached?)", argv[0], dpname.c_str());
    	}
    } else {
    	cli_print(cli, "Unknown interface %s", argv[0]);
    }

    return CLI_OK;
}

static int
cmd_config_subcmd_exit(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc))
{
	int next_mode;

	// todo could use a map to get next state
	switch (cli->mode) {
	case MODE_CONFIG_OPENFLOW_CONFIGURE:
		next_mode = MODE_CONFIG_OPENFLOW;
		break;
	case MODE_CONFIG_OPENFLOW:
	case MODE_CONFIG_INTERFACE:
	default:
		next_mode = MODE_CONFIG;
		break;
	}

    cli_set_configmode(cli, next_mode, NULL);
    return CLI_OK;
}

ccli::ccli(u_int16_t port) :
	cli_fd(-1),
	baddr(AF_INET, "0.0.0.0", port)
{
	cpopen(baddr, PF_INET, SOCK_STREAM);

	struct cli_command *c1;
	struct cli_command *c2;

	// Must be called first to setup data structures
	cli = cli_init();

	// Set the hostname (shown in the the prompt)
	cli_set_hostname(cli, "rofl");

	// Set the greeting
	cli_set_banner(cli, "Welcome to the rofl CLI.");

	// Enable 2 username / password combinations
	//cli_allow_user(cli, "fred", "nerk");
	//cli_allow_user(cli, "foo", "bar");


	/** UNPRIVILEGED COMMANDS **/

	// show
	c1 = cli_register_command(cli, NULL, "show", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
	// show port
	cli_register_command(cli, c1, "port", cmd_show_port, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
	// show openflow
	c2 = cli_register_command(cli, c1, "openflow", cmd_show_openflow, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
	// show openflow flow <all|#id>
	cli_register_command(cli, c2, "flow", cmd_test, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
	// show openflow stats
	cli_register_command(cli, c2, "stat", cmd_test, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);

	/** PRIVILEGED COMMANDS  "enable" **/

	// clear <one of the following>
	c1 = cli_register_command(cli, NULL, "clear", NULL, PRIVILEGE_PRIVILEGED, MODE_EXEC, NULL);
	// clear flowtable <entryid>
	cli_register_command(cli, c1, "flowtable", cmd_test, PRIVILEGE_PRIVILEGED, MODE_EXEC, NULL);
	// clear log
	cli_register_command(cli, c1, "log", cmd_test, PRIVILEGE_PRIVILEGED, MODE_EXEC, NULL);
	// debuglevel [category] [level]
	cli_register_command(cli, NULL, "debuglevel", cmd_test, PRIVILEGE_PRIVILEGED, MODE_EXEC, NULL);


	/** CONFIG COMMANDS  config terminal **/


	/* interface <name>			(todo not yet implemented)  */
	cli_register_command(cli, NULL, "interface", cmd_test, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "Configure an interface");
	/* openflow 				(entry function) */
	cli_register_command(cli, NULL, "openflow", cmd_config_openflow, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "Configure OpenFlow");


//	/** CONFIG INTERFACE COMMANDS  interface **/
//
//	// address w.x.y.z (for interface <name>)
//	cli_register_command(cli, NULL, "address", cmd_test, PRIVILEGE_PRIVILEGED, MODE_CONFIG_INTERFACE, "Set IP address");
//	// exit (back to config)
//	cli_register_command(cli, NULL, "exit", cmd_config_subcmd_exit, PRIVILEGE_PRIVILEGED, MODE_CONFIG_INTERFACE, "Exit from interface configuration");


	/** CONFIG OPENFLOW COMMANDS  openflow **/


	// create
	cli_register_command(cli, NULL, "create", cmd_test, PRIVILEGE_PRIVILEGED, MODE_CONFIG_OPENFLOW, "Create an OpenFlow datapath");
	// configure <dpname>
	cli_register_command(cli, NULL, "configure", cmd_config_openflow_configure, PRIVILEGE_PRIVILEGED, MODE_CONFIG_OPENFLOW, "Configure an OpenFlow datapath (param: <dpname>)");

	// show
	c1 = cli_register_command(cli, NULL, "show", NULL, PRIVILEGE_PRIVILEGED, MODE_CONFIG_OPENFLOW, NULL);
	// show datapath
	cli_register_command(cli, c1, "openflow", cmd_show_openflow, PRIVILEGE_PRIVILEGED, MODE_CONFIG_OPENFLOW, NULL);
	// show resources
	cli_register_command(cli, c1, "port", cmd_show_port, PRIVILEGE_PRIVILEGED, MODE_CONFIG_OPENFLOW, NULL);

	// exit (back to config)
	cli_register_command(cli, NULL, "exit", cmd_config_subcmd_exit, PRIVILEGE_PRIVILEGED, MODE_CONFIG_OPENFLOW, "Exit from OpenFlow configuration");


	/** CONFIG OPENFLOW CONFIGURE COMMANDS  configure **/


	// attach_port
	cli_register_command(cli, NULL, "attach_port", cmd_config_openflow_configure_attach_port, PRIVILEGE_PRIVILEGED, MODE_CONFIG_OPENFLOW_CONFIGURE, "Attach port to OpenFlow datapath <>");
	// exit (back to config)
	cli_register_command(cli, NULL, "exit", cmd_config_subcmd_exit, PRIVILEGE_PRIVILEGED, MODE_CONFIG_OPENFLOW_CONFIGURE, "Exit from OpenFlow configuration");


	WRITELOG(CLI, DBG, "ccli::ccli(%p) created", this);
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
ccli::read_config_file(const std::string& filename)
{
	WRITELOG(CLI, INFO, "open config file %s", filename.c_str());
	FILE *fh = fopen(filename.c_str(), "r");
	if (NULL != fh) {
		cli_file(this->cli, fh, PRIVILEGE_UNPRIVILEGED, MODE_EXEC);
		fclose(fh);
	} else {
		WRITELOG(CLI, ERROR, "config file not found %s", filename.c_str());
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
