#include "mmap_test.h"

#include <inttypes.h>

mmap_test::mmap_test(caddress const& laddr, unsigned int burst_interval, unsigned int pkt_interval, size_t pkt_len) :
	sock(0),
	laddr(laddr),
	burst_interval(burst_interval),
	pkt_interval(pkt_interval),
	pkt_len(pkt_len),
	seqno(0),
	dport(1024)
{
	pkt_len = (pkt_len % sizeof(uint64_t)) ? pkt_len - (pkt_len % sizeof(uint64_t)) : pkt_len;

	sock = new csocket(this, AF_INET, SOCK_DGRAM, 0 /* udp */, 10);

	sock->clisten(laddr, AF_INET, SOCK_DGRAM, 0, 10);

	register_timer(MMAP_TEST_TIMER_PKT_INTERVAL, 0);

	//udp_start_sending(caddress(AF_INET, "127.0.0.1", 5555));
}



mmap_test::~mmap_test()
{
	if (sock) delete sock;
}



void
mmap_test::handle_timeout(int opaque)
{
	switch (opaque) {
	case MMAP_TEST_TIMER_PKT_INTERVAL: {
		udp_send();
	} break;
	default:
		crofbase::handle_timeout(opaque);
	}
}



void
mmap_test::udp_start_sending(caddress const& raddr)
{
	raddrs.insert(raddr);
}



void
mmap_test::udp_stop_sending(caddress const& raddr)
{
	raddrs.erase(raddr);
}


void
mmap_test::udp_send()
{
	cmemory payload(pkt_len);

	for (int j = 0; j < 1; j++) {
		for (unsigned int i = 0; i < (pkt_len / sizeof(uint64_t)); i++) {
			payload[0+i*sizeof(uint64_t)] = ((uint8_t*)&seqno)[7];
			payload[1+i*sizeof(uint64_t)] = ((uint8_t*)&seqno)[6];
			payload[2+i*sizeof(uint64_t)] = ((uint8_t*)&seqno)[5];
			payload[3+i*sizeof(uint64_t)] = ((uint8_t*)&seqno)[4];
			payload[4+i*sizeof(uint64_t)] = ((uint8_t*)&seqno)[3];
			payload[5+i*sizeof(uint64_t)] = ((uint8_t*)&seqno)[2];
			payload[6+i*sizeof(uint64_t)] = ((uint8_t*)&seqno)[1];
			payload[7+i*sizeof(uint64_t)] = ((uint8_t*)&seqno)[0];
		}

		fprintf(stderr, "payload: %s\n", payload.c_str());

		seqno++;

		dport++;
		dport = (dport <  1024) ? 1024 : dport;
		dport = (dport > 65535) ?    0 : dport;

		for (std::set<caddress>::iterator
				it = raddrs.begin(); it != raddrs.end(); ++it) {

			caddress ra(*it);
			caddress raddr(*it);
			raddr.ca_s4addr->sin_port += htobe16(dport);
			fprintf(stderr, "seqno: %lu dport: %d laddr: %s raddr(orig): %s raddr(dport): %s\n",
					seqno, dport, laddr.c_str(), ra.c_str(), raddr.c_str());

			sock->send_packet(new cmemory(payload), raddr);

#if 1
			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = pkt_interval; /* default: 100ms */
			select(0, 0, 0, 0, &timeout);
#endif
		}
	}

	register_timer(MMAP_TEST_TIMER_PKT_INTERVAL, burst_interval);
}
