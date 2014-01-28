/*
 * cdaemon.h
 *
 *  Created on: 28.01.2014
 *      Author: andreas
 */

#ifndef CDAEMON_H_
#define CDAEMON_H_

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>

#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/cpipe.h"

namespace rofl {

class cdaemon {
public:

	/**
	 *
	 */
	static void
	daemonize(
			std::string const& pidfile, std::string const& logfile);

};

}; // end of namespace rofl

#endif /* CDAEMON_H_ */
