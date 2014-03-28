/*
 * cudprecv.h
 *
 *  Created on: 31.08.2013
 *      Author: andreas
 */

#ifndef CUDPRECV_H_
#define CUDPRECV_H_

#include <rofl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <exception>

#include <rofl/common/ciosrv.h>
#include <rofl/common/csocket.h>

#include "cudpmsg.h"
#include "ctimeval.h"

namespace spray
{

class eUdpRecvBase			: public std::exception {};
class eUdpRecvNotFound		: public eUdpRecvBase {};


class cudprecv :
		public rofl::ciosrv
{
	static std::map<rofl::caddress, cudprecv*> udpreceivers;

	bool 				keep_going;
	pthread_t			tid;
	rofl::caddress		remote;
	rofl::caddress		local;
	int					stats_interval;
	time_t				starttime;
	time_t				stoptime;
	uint64_t			startseqno;
	uint64_t			endseqno;
	uint64_t			npkts;
	uint64_t			rxseqno;
	uint64_t			rxlost;
	uint64_t			rxbytes;
	int					pktcnt;

	enum cudprecv_timer_t {
		CUDPRECV_TIMER_PRINT_STATS = 1,
	};

public:

	static cudprecv&
	get_udprecv(rofl::caddress const& peer);

	static void*
	start_udp_receiving_thread(void* data);

public:

	cudprecv(
			rofl::caddress const& remote,
			rofl::caddress const& local);

	virtual
	~cudprecv();

public:

	void
	start_receiving();

	void
	stop_receiving();

	void
	print_statistics();

protected:

	virtual void
	handle_timeout(int opaque);

private:

	void
	recv_udp_msgs();

};

}; // end of namespace

#endif /* CUDPRECV_H_ */
