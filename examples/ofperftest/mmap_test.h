#ifndef MMAP_TEST_H
#define MMAP_TEST_H 1

#include <rofl.h>

ROFL_BEGIN_DECLS
#include <netdb.h>
ROFL_END_DECLS

#include <map>

#include "rofl/common/cmacaddr.h"
#include "rofl/common/caddress.h"
#include "rofl/common/crofbase.h"
#include "rofl/common/crofdpt.h"

#include "ofperftest.h"

using namespace rofl;

class mmap_test :
		public ofperftest
{
private:

#define MMAP_TEST_DEFAULT_BURST_INTERVAL 1
#define MMAP_TEST_DEFAULT_PKT_INTERVAL 500000
#define MMAP_TEST_DEFAULT_PKT_LEN 64

	enum mmap_test_timer_t {
		MMAP_TEST_TIMER_BASE = ((0x6271)),
		MMAP_TEST_TIMER_PKT_INTERVAL,
	};

	csocket				*sock;				// socket for UDP traffic
	caddress			 laddr; 			// local address

	std::set<caddress>	 raddrs;			// set of remote addresses to send UDP packets to
	unsigned int 		 burst_interval;	// interval between bursts of packets
	unsigned int		 pkt_interval;		// delay between packets within a single burst
	size_t				 pkt_len;			// length of UDP payload in bytes
	uint64_t			 seqno;				// sequence number of payload of UDP packets
	uint16_t			 dport;

public:

	mmap_test(
			caddress const& laddr = caddress(AF_INET, "0.0.0.0", 4444),
			unsigned int burst_interval = MMAP_TEST_DEFAULT_BURST_INTERVAL,
			unsigned int pkt_interval = MMAP_TEST_DEFAULT_PKT_INTERVAL,
			size_t pkt_len = MMAP_TEST_DEFAULT_PKT_LEN /* bytes */);

	virtual
	~mmap_test();

	void
	udp_start_sending(caddress const& raddr);

	void
	udp_stop_sending(caddress const& raddr);

protected:

	virtual void
	handle_timeout(int opaque);

private:

	void
	udp_send();
};

#endif
