/*
 * cudprecv.cc
 *
 *  Created on: 31.08.2013
 *      Author: andreas
 */

#include "cudprecv.h"

using namespace spray;


std::map<rofl::caddress, cudprecv*> cudprecv::udpreceivers;


cudprecv&
cudprecv::get_udprecv(rofl::caddress const& remote)
{
	if (cudprecv::udpreceivers.find(remote) == cudprecv::udpreceivers.end()) {
		throw eUdpRecvNotFound();
	}

	return *(cudprecv::udpreceivers[remote]);
}


cudprecv::cudprecv(
		rofl::caddress const& remote,
		rofl::caddress const& local) :
				keep_going(false),
				tid(0),
				remote(remote),
				local(local),
				stats_interval(5),
				starttime(0),
				stoptime(0),
				startseqno(0),
				endseqno(0),
				npkts(0),
				rxseqno(0),
				rxlost(0),
				rxbytes(0)
{
	cudprecv::udpreceivers[remote] = this;
}




cudprecv::~cudprecv()
{
	cudprecv::udpreceivers.erase(remote);
}




void
cudprecv::handle_timeout(int opaque)
{
	switch (opaque) {
	case CUDPRECV_TIMER_PRINT_STATS: {
		print_statistics();
	} break;
	}
}



void*
cudprecv::start_udp_receiving_thread(void* data)
{
	cudprecv& udprecv = *(static_cast<cudprecv*>( data ));

	fprintf(stderr, "Receiving thread [0x%x] created\n", (unsigned int)pthread_self());

	udprecv.recv_udp_msgs();

	fprintf(stderr, "Destroying receiving thread [0x%x]\n", (unsigned int)pthread_self());

	return NULL;
}




void
cudprecv::start_receiving()
{
	keep_going = true;

	int rc = 0;

	if ((rc = pthread_create(&tid, NULL, cudprecv::start_udp_receiving_thread, this)) < 0) {
		fprintf(stderr, "error creating thread: %d (%s)\n", errno, strerror(errno));
	}

	register_timer(CUDPRECV_TIMER_PRINT_STATS, 0);
}



void
cudprecv::stop_receiving()
{
	keep_going = false;

	cancel_timer(CUDPRECV_TIMER_PRINT_STATS);
}




void
cudprecv::recv_udp_msgs()
{
	int sd = 0, rc = 0;

	rxbytes = 0;
	rxlost = 0;
	starttime = time(0);

	cudpmsg udpmsg(1500);

	if ((sd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "error on socket() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	//int rcvbuf = 5120000;
	int rcvbuf = 212992;
	if ((rc = setsockopt(sd, SOL_SOCKET, SO_RCVBUFFORCE, &rcvbuf, sizeof(rcvbuf))) < 0) {
		fprintf(stderr, "error on setsockopt() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	if ((rc = bind(sd, local.ca_saddr, local.salen)) < 0) {
		std::cerr << "local: " << local.addr_c_str() << std::endl;
		fprintf(stderr, "error on bind() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	if ((rc = connect(sd, remote.ca_saddr, remote.salen)) < 0) {
		std::cerr << "remote: " << remote.addr_c_str() << std::endl;
		fprintf(stderr, "error on connect() call: %d (%s)\n", errno, strerror(errno));
		return;
	}

	pktcnt = 0;

	while (keep_going) {

		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(sd, &rfds);

		rc = select(sd + 1, &rfds, NULL, NULL, (struct timeval*)0);

		if (rc < 0) {
			fprintf(stderr, "error on select() call: %d (%s)\n", errno, strerror(errno));
		}
		else if (rc == 0) {
			// do nothing, timeout

		}
		else if (rc > 0) {

			if ((rc = recvfrom(sd, udpmsg.somem(), udpmsg.memlen(), 0, NULL, 0)) < 0) {
				fprintf(stderr, "error on recv() call: %d (%s)\n", errno, strerror(errno));
			}

			if (0 == pktcnt) {
				starttime = time(0);
			}

			pktcnt++;

#if 0
			fprintf(stdout, "msglen: %d type: %d seqno: %u udpmsg.memlen(): %u\n",
					rc, udpmsg.get_type(), udpmsg.get_seqno(), (unsigned int)udpmsg.memlen());
#endif

			if (rc < 0) {

			}
			else if (rc == 0) {

				endseqno = rxseqno;
				fprintf(stderr, "ENDE![1]\n");

			}
			else if (rc > 0) {

				switch (udpmsg.get_type()) {
				case cudpmsg::UMT_START: {
					starttime = stoptime = time(0);

					startseqno = rxseqno = udpmsg.get_seqno();
					rxbytes = rc + 42;
					rxlost  = 0;

				} break;
				case cudpmsg::UMT_STOP: {
					stoptime = time(0);

					endseqno = rxseqno = udpmsg.get_seqno();
					rxbytes += rc + 42;

					keep_going = false;

				} break;
				case cudpmsg::UMT_DATA: {
					stoptime = time(0);

					if (udpmsg.get_seqno() != (rxseqno + 1)) {

						rxlost += (udpmsg.get_seqno() - (rxseqno + 1));

					}

					rxseqno = udpmsg.get_seqno();
					rxbytes += rc + 42;

				} break;
				case cudpmsg::UMT_UNKNOWN:
				default: {
					//fprintf(stdout, "+");
				} break;
				}
			}
		}
	}

	stoptime = time(0);

	close(sd);
}




void
cudprecv::print_statistics()
{
#if 0
					fprintf(stderr, "ENDE![2]\n");

					fprintf(stdout, "rxseqno: %u rxbytes: %lu rxlost: %lu npkts: %lu\n",
							rxseqno, rxbytes, rxlost, rxseqno - startseqno);
#endif
/*
 * 8*64=512
 * 1e9
 * 1953125 Pakete/Sekunde => 0.512 usec / Paket
 */


	npkts = rxseqno - startseqno;
	uint64_t rxrcvd = npkts - rxlost;
	double loss = 100 * ((double)rxlost / npkts);
	double bitrate = (double)(8 * rxbytes) / (stoptime - starttime) / 1000000;

	fprintf(stdout, "rxseqno: %"PRIu64" rxbytes: %"PRIu64" rxlost: %"PRIu64" npkts: %"PRIu64" rxrcvd: %"PRIu64" loss: %lf%% bitrate: %.6lfMbps\n",
			rxseqno, rxbytes, rxlost, rxseqno - startseqno, rxrcvd, loss, bitrate);

	if (keep_going)
		register_timer(CUDPRECV_TIMER_PRINT_STATS, stats_interval);
}


