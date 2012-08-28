/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cpipe.h"

cpipe::cpipe() throw (ePipeInit)
{
	// create two pipes for connecting stdin and stdout
	if (pipe(pipefd) < 0)
	{
		throw ePipeInit();
	}

	// make pipe non-blocking
	int flags = 0;


	if ((flags = fcntl(pipefd[0], F_GETFL)) < 0)
	{
		throw ePipeInit();
	}

	flags |= O_NONBLOCK;

	if (fcntl(pipefd[0], F_SETFL, flags) < 0)
	{
		throw ePipeInit();
	}

	pthread_mutex_init(&pipelock, NULL);
}


cpipe::~cpipe()
{
	pthread_mutex_destroy(&pipelock);

	close(pipefd[0]);
	close(pipefd[1]);
}


void
cpipe::writemsg(unsigned char msg)
{
	int rc = write(pipefd[1], &msg, sizeof(msg));
	WRITELOG(CPIPE, ROFL_DBG, "cpipe(%p)::writemsg() thread:0x%lx rc:%d",
			this, pthread_self(), rc);
	if (rc == 0)
	{

	}
	else if (rc < 0)
	{
		fprintf(stderr, "cpipe(%p)::writemsg() rc:%d errno:%d %s\n",
				this, rc, errno, strerror(errno));
		throw eDebug();
	}
}


unsigned char
cpipe::recvmsg()
{
	unsigned char msg = 0;
	int rc;
	if ((rc = read(pipefd[0], &msg, sizeof(msg))) < 0)
	{
		WRITELOG(CPIPE, ROFL_DBG, "cpipe(%p)::recvmsg() rc:%d errno:%d %s",
				this, rc, errno, strerror(errno));

		switch (errno) {
		case EAGAIN:
			return 0;
		default:
			fprintf(stderr, "cpipe(%p)::recvmsg() error occured tid: 0x%lx rc: %d errno:%d %s\n",
								this, pthread_self(), rc, errno, strerror(errno));
						break;
			WRITELOG(CPIPE, ROFL_DBG, "cpipe::recvmsg() error occured tid: 0x%lx rc: %d errno:%d %s",
					pthread_self(), rc, errno, strerror(errno));
			break;
		}
	}
	WRITELOG(CPIPE, ROFL_DBG, "cpipe(%p)::recvmsg() thread:0x%lx rc:%d",
			this, pthread_self(), rc);
	return msg;
#if 0
	uint32_t msg = 0;
	while (true)
	{
		int rc = read(pipefd[0], &msg, sizeof(msg));
		WRITELOG(CPIPE, ROFL_DBG, "cpipe::recvmsg() tid: 0x%lx rc: %d",
				pthread_self(), rc);
		if (rc == 0)
		{
			fprintf(stderr, "cpipe::recvmsg() eDebug\n");
			throw eDebug();
		}
		else if (rc < 0)
		{
			switch (errno) {
			case EWOULDBLOCK:
				WRITELOG(CPIPE, ROFL_DBG, "cpipe::recvmsg() EAGAIN tid: 0x%lx rc: %d errno:%d %s",
						pthread_self(), rc, errno, strerror(errno));
				return msg;
			default:
				WRITELOG(CPIPE, ROFL_DBG, "cpipe::recvmsg() ??? tid: 0x%lx rc: %d errno:%d %s",
						pthread_self(), rc, errno, strerror(errno));
				return msg;
			}
		}
	}
	return msg;
#endif
}


