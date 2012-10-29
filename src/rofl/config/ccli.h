/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ccli.h
 *
 *  Created on: Jul 12, 2012
 *      Author: tobi
 */

#ifndef CCLI_H_
#define CCLI_H_

#include <string>
#include <pthread.h>
#include <libcli.h>
#include <stdint.h>

#include "rofl/platform/unix/csyslog.h"
#include "rofl/common/csocket.h"
#include "rofl/common/cerror.h"

#ifdef __GNUC__
# define UNUSED(d) d __attribute__ ((unused))
#else
# define UNUSED(d) d
#endif


class eCliBase 						: public cerror {};
class eCliConfigFileNotFound 		: public eCliBase {};



class ccli : public csocket
{

public:
	ccli(caddress addr = caddress(AF_INET, "127.0.0.1", 6620));

	virtual
	~ccli();

	virtual void
	handle_accepted(int newsd, caddress &ra);

	void
	read_config_file(const std::string &filename) throw (eCliConfigFileNotFound);

	/**
	 * Read data from socket.
	 *
	 * This notification method is called from within csocket::handle_revent().
	 * A derived class should read a packet from the socket. This method
	 * must be overwritten by a derived class.
	 * @param fd the socket descriptor
	 */
	virtual void
	handle_read(int fd) {
		WRITELOG(CLI, DBG, "ccli(%p)::handle_read()", this);
	}

	struct cli_def *
	get_cli() {
		return this->cli;
	}

protected:
	struct cli_def *cli;

	struct cli_command *
	find_command(const char * const name);

private:
	static void *
	run_terminal(void*);

 	int cli_fd;

	// bound interface
	caddress baddr;
};

#endif /* CCLI_H_ */
