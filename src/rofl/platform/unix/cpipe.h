/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CPIPE_H
#define CPIPE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>

#ifdef __cplusplus
}
#endif

#include "rofl/common/cerror.h"
#include "csyslog.h"

namespace rofl
{

class ePipeBase : public cerror {};
class ePipeInit : public ePipeBase {}; // initialization of pipe failed
class ePipeInval : public ePipeBase {}; // invalid parameter

class cpipe :
	public csyslog
{
	enum cpipe_cmd_t {
		CPIPE_WAKEUP = 1,
	};

public:

	/**
	 *
	 */
	cpipe() throw (ePipeInit);

	/**
	 *
	 */
	~cpipe();

	/**
	 *
	 */
	void
	writemsg(unsigned char msg = 0);

	/**
	 *
	 */
	unsigned char
	recvmsg();

public:

	int pipefd[2]; // pipefd[0]: read, pipefd[1]: write
	pthread_mutex_t pipelock; // mutex for this pipe

};

}; // end of namespace

#endif
