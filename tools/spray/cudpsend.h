/*
 * cudpsend.h
 *
 *  Created on: 31.08.2013
 *      Author: andreas
 */

#ifndef CUDPSEND_H_
#define CUDPSEND_H_

#include <rofl.h>
#include <pthread.h>
#include <map>
#include <exception>
#include <rofl/common/ciosrv.h>
#include <rofl/common/csocket.h>
#include <rofl/common/thread_helper.h>
#include <inttypes.h>

#include "cudpmsg.h"

namespace spray
{

class eUdpSendBase 				: public std::exception {};
class eUdpSendNotFound			: public eUdpSendBase {};

class cudpsend :
		public rofl::ciosrv
{
	static std::map<rofl::csockaddr, cudpsend*>	 udpsenders;

private:

#define DEFAULT_UDP_NODE_INTERVAL 1 /* usec */
#define DEFAULT_UDP_NODE_MSGLEN 1472 /* bytes */

	pthread_t			tid;
	int					duration;
	rofl::csockaddr		remote;
	rofl::csockaddr		local;
	int					stats_interval;
	time_t				starttime;
	time_t				stoptime;
	bool				keep_going;
	unsigned int		msglen;
	uint64_t			txbytes;
	uint64_t			npkts;

	enum cudpsend_timer_t {
		CUDPSEND_TIMER_STOP_SENDING = 1,
		CUDPSEND_TIMER_PRINT_STATS = 2,
	};

public:

	static cudpsend&
	get_udpsend(
			rofl::csockaddr const& peer);

	static void*
	start_udp_sending_thread(void* data);

public:

	cudpsend(
			rofl::csockaddr const& remote,
			rofl::csockaddr const& local,
			unsigned int msglen = DEFAULT_UDP_NODE_MSGLEN);

	virtual
	~cudpsend();

public:

	void
	start_sending(
			int interval = DEFAULT_UDP_NODE_INTERVAL);

	void
	stop_sending();

	void
	print_statistics();

private:

	virtual void
	handle_timeout(int opaque);

private:

	void
	send_udp_msgs();
};

}; // end of namespace

#endif /* CUDPSEND_H_ */
