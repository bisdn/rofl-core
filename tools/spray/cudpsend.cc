/*
 * cudpsend.cc
 *
 *  Created on: 31.08.2013
 *      Author: andreas
 */

#include <cudpsend.h>

using namespace spray;

std::map<rofl::caddress, cudpsend*> cudpsend::udpsenders;




cudpsend&
cudpsend::get_udpsend(rofl::caddress const& peer)
{
	if (cudpsend::udpsenders.find(peer) == cudpsend::udpsenders.end()) {
		throw eUdpSendNotFound();
	}

	return *(cudpsend::udpsenders[peer]);
}



cudpsend::cudpsend(
		rofl::caddress const& peer,
		rofl::caddress const& local,
		unsigned int msglen) :
				tid(0),
				duration(DEFAULT_UDP_NODE_INTERVAL),
				remote(peer),
				local(local),
				stats_interval(5),
				starttime(0),
				stoptime(0),
				keep_going(false),
				msglen(msglen),
				txbytes(0),
				npkts(0)
{
	cudpsend::udpsenders[peer] = this;
}



cudpsend::~cudpsend()
{
	cudpsend::udpsenders.erase(remote);
}



void
cudpsend::start_sending(int ival)
{
	duration = ival;

	register_timer(CUDPSEND_TIMER_STOP_SENDING, duration);

	keep_going = true;

	int rc = 0;

	if ((rc = pthread_create(&tid, NULL, cudpsend::start_udp_sending_thread, this)) < 0) {
		fprintf(stderr, "error creating thread: %d (%s)\n", errno, strerror(errno));
	}

	sleep(5);

	register_timer(CUDPSEND_TIMER_PRINT_STATS, 0);
}


void
cudpsend::stop_sending()
{
	keep_going = false;
}



void
cudpsend::handle_timeout(int opaque)
{
	switch (opaque) {
	case CUDPSEND_TIMER_STOP_SENDING: {
		stop_sending();
	} break;
	case CUDPSEND_TIMER_PRINT_STATS: {
		print_statistics();
	} break;
	}
}



void*
cudpsend::start_udp_sending_thread(void* data)
{
	cudpsend& udpsend = *(static_cast<cudpsend*>( data ));

	fprintf(stderr, "Sending thread [0x%x] created\n", (unsigned int)pthread_self());

	udpsend.send_udp_msgs();

	fprintf(stderr, "Destroying sending thread [0x%x]\n", (unsigned int)pthread_self());

	return NULL;
}





void
cudpsend::send_udp_msgs()
{
	int sd = 0, rc = 0;

	cudpmsg udpmsg(msglen);

	struct timeval tv;
	memset(&tv, 0, sizeof(tv));
	//tv.tv_sec  = 0;
	//tv.tv_usec = interval;

	uint32_t seqno = 0;


	if ((sd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "error on socket() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	//int sndbuf = 5120000;
	int sndbuf = 212992;
	if ((rc = setsockopt(sd, SOL_SOCKET, SO_SNDBUFFORCE, &sndbuf, sizeof(sndbuf))) < 0) {
		fprintf(stderr, "error on setsockopt() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	if ((rc = bind(sd, local.ca_saddr, local.salen)) < 0) {
		fprintf(stderr, "error on bind() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	if ((rc = connect(sd, remote.ca_saddr, remote.salen)) < 0) {
		fprintf(stderr, "error on connect() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	starttime = stoptime = time(0);

	udpmsg.set_seqno(seqno++);
	udpmsg.set_type(cudpmsg::UMT_START);

	if ((rc = send(sd, udpmsg.somem(), udpmsg.memlen(), 0)) < 0) {
		fprintf(stderr, "error send() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	txbytes = rc + 42;
	npkts = 1;

	udpmsg.set_type(cudpmsg::UMT_DATA);

#if 0
		fprintf(stdout, "type: %d seqno: %u udpmsg.memlen(): %u\n",
				udpmsg.get_type(), udpmsg.get_seqno(), (unsigned int)udpmsg.memlen());
#endif

	while (keep_going) {

		stoptime = time(0);

		udpmsg.set_seqno(seqno++);

		if ((rc = send(sd, udpmsg.somem(), udpmsg.memlen(), 0)) < 0) {
			fprintf(stderr, "error send() call: %d (%s)\n", errno, strerror(errno));
			return;
		}

		txbytes += rc + 42;
		npkts++;

#if 0
		fprintf(stdout, "type: %d seqno: %u udpmsg.memlen(): %u\n",
				udpmsg.get_type(), udpmsg.get_seqno(), (unsigned int)udpmsg.memlen());
#endif

#if 0
		tv.tv_sec  = 0;
		tv.tv_usec = 0;

		select(0, NULL, NULL, NULL, &tv);
#endif
	}

	stoptime = time(0);

	udpmsg.set_seqno(seqno++);
	udpmsg.set_type(cudpmsg::UMT_STOP);

	if ((rc = send(sd, udpmsg.somem(), udpmsg.memlen(), 0)) < 0) {
		fprintf(stderr, "error send() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	txbytes += rc + 42;
	npkts++;

#if 0
	fprintf(stdout, "type: %d seqno: %u udpmsg.memlen(): %u\n",
			udpmsg.get_type(), udpmsg.get_seqno(), (unsigned int)udpmsg.memlen());
#endif

	stoptime = time(0);

	sleep(3);

	close(sd);
}



void
cudpsend::print_statistics()
{
	double bitrate = (double)(8 * txbytes) / (stoptime - starttime) / 1000000;
	fprintf(stderr, "txbytes: %lu npkts: %lu bitrate: %.6lfMbps\n", txbytes, npkts, bitrate);

	if (keep_going)
		register_timer(CUDPSEND_TIMER_PRINT_STATS, stats_interval);
}



