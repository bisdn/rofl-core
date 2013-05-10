/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cmmapport.h"

using namespace rofl;

#ifndef HARDWARE


/**
 *  fixme the cmmapport_helper should be a factory which is registered on creation
 * the scanning of ports should be moved into a config class (which is also responsible
 * for the available OF-Port-Numbers)
 **/
#ifndef NDEBUG
static void
my_print(const std::string &s) {
	puts(s.c_str());
}
#endif

#if 0
cmapport_helper cmapport_helper::instance;

cmapport_helper::cmapport_helper()
{
	/* do not use csyslog in here (not yet existing!) */

	struct ifaddrs *ifaddr, *ifa;

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		throw;
	}

	/* Walk through linked list, maintaining head pointer so we
	   can free list later */

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

		/* TODO what else do we not want to have show up as a cport? */
		if ( /* ignre all these types: */
				0 < (ifa->ifa_flags & IFF_LOOPBACK) ||
				0  < (ifa->ifa_flags & IFF_ISATAP))
			continue;

		// if_nameindex() could also be used, but then we have to check separately for the flags

		/*    SIOCGIFFLAGS, SIOCSIFFLAGS:
	          Get or set the active flag word of the device.  ifr_flags contains a bit mask of the following values:

	                                       Device flags
	          IFF_UP            Interface is running.
	          IFF_BROADCAST     Valid broadcast address set.
	          IFF_DEBUG         Internal debugging flag.
	          IFF_LOOPBACK      Interface is a loopback interface.
	          IFF_POINTOPOINT   Interface is a point-to-point link.
	          IFF_RUNNING       Resources allocated.

	          IFF_NOARP         No arp protocol, L2 destination address not set.
	          IFF_PROMISC       Interface is in promiscuous mode.
	          IFF_NOTRAILERS    Avoid use of trailers.
	          IFF_ALLMULTI      Receive all multicast packets.
	          IFF_MASTER        Master of a load balancing bundle.
	          IFF_SLAVE         Slave of a load balancing bundle.
	          IFF_MULTICAST     Supports multicast
	          IFF_PORTSEL       Is able to select media type via ifmap.
	          IFF_AUTOMEDIA     Auto media selection active.
	          IFF_DYNAMIC       The addresses are lost when the interface goes down.
	          IFF_LOWER_UP      Driver signals L1 up (since Linux 2.6.17)
	          IFF_DORMANT       Driver signals dormant (since Linux 2.6.17)
	          IFF_ECHO          Echo sent packets (since Linux 2.6.25)
		 */

		if_names.insert(std::string(ifa->ifa_name));
	}


//	ports[0] = new cmmapport(std::string("veth_t_u"), 47/*portno*/,
//			cunixenv::block_size,
//			cunixenv::n_blocks,
//			cunixenv::frame_size); // port "veth0"
//	ports[1] = new cmmapport(std::string("veth_u_t"), 48/*portno*/,
//			cunixenv::block_size,
//			cunixenv::n_blocks,
//			cunixenv::frame_size); // port "veth2"

	freeifaddrs(ifaddr);



#ifndef NDEBUG
	std::for_each(if_names.begin(), if_names.end(), my_print);
#endif
}

/*static*/void
cmapport_helper::create_ports(const std::string& s) {
	new cmmapport(s, 0);
}

void cmapport_helper::autocreate_ports() {
	std::for_each(if_names.begin(), if_names.end(), create_ports);
}

cmapport_helper::~cmapport_helper() {
}
#endif






cmmapdev::cmmapdev(
		int __type,
		std::string __devname,
		int __block_size,
		int __n_blocks,
		int __frame_size) :
		block_size(__block_size),
		n_blocks(__n_blocks),
		frame_size(__frame_size),
		devname(__devname),
		ring_type(__type),
		sd(-1),
		ll_addr(ETH_P_ALL, devname, 0, 0, NULL, 0),
		ring(NULL),
		rpos(0)
{
	WRITELOG(CPORT, DBG, "cmmapdev(%p)::cmmapdev() %s\n",
			this, (ring_type == PACKET_TX_RING) ? "TX-RING" : "RX-RING");
	// initialize();
}


cmmapdev::~cmmapdev()
{
	WRITELOG(CPORT, DBG, "cmmapdev(%p)::~cmmapdev() %s\n",
			this, (ring_type == PACKET_TX_RING) ? "TX-RING" : "RX-RING");
	if (-1 != sd)
	{
		// TODO: unmap memory area first?
		close(sd);
	}
}


void
cmmapdev::initialize() throw (eMMapFailed)
{
	int rc = 0;

	if (-1 != sd)
	{
		// CHECK: unmap memory area first?
		close(sd);
	}

	// open socket
	if ((sd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
	{
		throw eSocketError();
	}

	// make socket non-blocking
	long flags;
	if ((flags = fcntl(sd, F_GETFL)) < 0)
	{
		throw eSocketError();
	}
	flags |= O_NONBLOCK;
	if ((rc = fcntl(sd, F_SETFL, flags)) < 0)
	{
		throw eSocketError();
	}

	struct ifreq ifr; // for ioctls on socket

	// enable promiscous mode
	bzero((void*)&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, devname.c_str(), sizeof(ifr.ifr_name));
	if ((rc = ioctl(sd, SIOCGIFFLAGS, &ifr)) < 0)
	{
		throw eMMapPortIfaceNotFound();
	}
	ifr.ifr_flags |= IFF_PROMISC;
	if ((rc = ioctl(sd, SIOCSIFFLAGS, &ifr)) < 0)
	{
		throw eMMapPortIfaceNotFound();
	}

	// set SO_SNDBUF and SO_RCVBUF
	int optval = 131072;
	if ((rc = setsockopt(sd, SOL_SOCKET, SO_SNDBUF,
			(int*)&optval, sizeof(optval))) < 0)
	{
		throw eSocketError();
	}

	if ((rc = setsockopt(sd, SOL_SOCKET, SO_RCVBUF,
			(int*)&optval, sizeof(optval))) < 0)
	{
		throw eSocketError();
	}


	// get device ifindex from kernel
	bzero((void*)&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, devname.c_str(), sizeof(ifr.ifr_name));
	if ((rc = ioctl(sd, SIOCGIFINDEX, &ifr)) < 0)
	{
		throw eMMapPortIfaceNotFound();
	}

	// bind socket to device
	ll_addr.ca_sladdr->sll_ifindex = ifr.ifr_ifindex;
	if ((rc = bind(sd, ll_addr.ca_saddr, ll_addr.salen)))
	{
		switch (errno) {
		case EADDRINUSE:
			throw eSocketAddressInUse();
		default:
			throw eSocketBindFailed();
		}
	}

	// ...try a more conservative setting first
	req.tp_block_size 	= block_size * getpagesize(); // 8
	req.tp_block_nr 	= n_blocks; // 8
	req.tp_frame_size 	= frame_size; // 2048
	req.tp_frame_nr 	= req.tp_block_size * req.tp_block_nr / req.tp_frame_size;

	//fprintf(stdout, "frame_nr:%d\n", req.tp_frame_nr);

	WRITELOG(CPORT, DBG, "cmmapdev(%p)::initialize() block-size:%u block-nr:%u frame-size:%u frame-nr:%u",
			this,
			req.tp_block_size,
			req.tp_block_nr,
			req.tp_frame_size,
			req.tp_frame_nr);


	if ((rc = setsockopt(sd, SOL_PACKET, ring_type,
			(void *) &req, sizeof(req))) < 0)
	{
		WRITELOG(CPORT, DBG, "cmmapdev(%p)::initialize() setsockopt() sys-call failed "
				"rc: %d errno: %d (%s)\n", this, rc, errno, strerror(errno));
		throw eMMapPortSocketFailed();
	}


	void *map = NULL;

	// this is mapped as contiguous memory area by the kernel
	if ((map = (char*)mmap(0, req.tp_block_size * req.tp_block_nr,
			PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED,
			/*file descriptor*/sd, /*offset*/0)) == MAP_FAILED)
	{
		WRITELOG(CPORT, DBG, "cmmapdev(%p)::initialize() mmap() sys-call failed "
				"rc: %d errno: %d (%s)\n", this, rc, errno, strerror(errno));
		throw eMMapPortSocketFailed();
	}

	//fprintf(stdout, "map: %p\n", map);

	ring = (struct iovec*)ringptrs.resize(req.tp_frame_nr * sizeof(struct iovec));

	for (unsigned int i = 0; i < req.tp_frame_nr; ++i)
	{
		ring[i].iov_base = (void*)((uint8_t*)map + i * req.tp_frame_size);
		ring[i].iov_len  = req.tp_frame_size;
		WRITELOG(CPORT, DBG, "cmmapdev(%p)::initialize() ring[%d].iov_base: %p   ",
				this, i, ring[i].iov_base);
		WRITELOG(CPORT, DBG, "cmmapdev(%p)::initialize() ring[%d].iov_len:  %lu",
				this, i, ring[i].iov_len);
	}
}








cmmapport::cmmapport(
		cport_owner *owner,
		std::string devname,
		int port_no,
		int block_size,
		int n_blocks,
		int frame_size) :
	clinuxport(owner, devname, std::string("phy")),
	txline(PACKET_TX_RING, devname, block_size, 1 * n_blocks, frame_size),
	rxline(PACKET_RX_RING, devname, block_size, 2 * n_blocks, frame_size),
	tx_pkts(0),
	rx_pkts(0)
{
	WRITELOG(CPORT, WARN, "cmmapport(%s)::cmmapport()", devname.c_str());

	pthread_mutex_init(&queuelock, NULL);

	laddr = get_hw_addr();

	// register RX line on ciosrv
	register_filedesc_r(rxline.sd);
	// register TX line on ciosrv
	//register_filedesc_w(txline.sd);
}


cmmapport::~cmmapport()
{
	WRITELOG(CPORT, WARN, "cmmapport(%s)::~cmmapport()", devname.c_str());

	pthread_mutex_destroy(&queuelock);
}



void
cmmapport::handle_event(cevent const& ev)
{
	WRITELOG(CPORT, DBG, "cmmapport(%s:%p)::handle_event() event:%s",
			devname.c_str(), this, cevent(ev).c_str());
	switch (ev.cmd) {
	default:
		clinuxport::handle_event(ev);
		break;
	}
}


void
cmmapport::handle_timeout(
		int opaque)
{
	switch (opaque) {
	default:
		clinuxport::handle_timeout(opaque);
		break;
	}
}


void
cmmapport::handle_revent(
		int fd)
{
	if (rxline.sd != fd)
	{
		return;
	}

	WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_revent() total #slots:%d",
			devname.c_str(), rxline.req.tp_frame_nr);

#if 0
		fprintf(stdout, "cmmapport(%s)::handle_revent()\n",
				devname.c_str());
#endif


	//int pos = (rxline.rpos == 0) ? rxline.req.tp_frame_nr-1 : rxline.rpos-1;

	//for (unsigned int i = rxline.rpos; i < rxline.req.tp_frame_nr; ++i)
	//for (unsigned int i = 0; i < rxline.req.tp_frame_nr; ++i)

#if 0
	fprintf(stdout, "%s\t", devname.c_str());
	for (unsigned int i = 0; i < rxline.req.tp_frame_nr; i++)
	{
		struct tpacket_hdr *hdr = (struct tpacket_hdr*)rxline.ring[i].iov_base;
		struct sockaddr_ll *sll = (struct sockaddr_ll*)((uint8_t*)hdr + TPACKET_ALIGN(sizeof(struct tpacket_hdr)));

		if (hdr->tp_status & TP_STATUS_USER)
		{
			if (hdr->tp_status & TP_STATUS_COPY)
			{
				fprintf(stdout, "c");
			}
			else if (hdr->tp_status & TP_STATUS_LOSING)
			{
				fprintf(stdout, "d");
			}
			else if (hdr->tp_status & TP_STATUS_CSUMNOTREADY)
			{
				fprintf(stdout, "S");
			}
			else
			{
				if (PACKET_OUTGOING == sll->sll_pkttype)
				{
					fprintf(stdout, "o");
				}
				else
				{
					fprintf(stdout, "+");
				}
			}
		}
		else
		{
			fprintf(stdout, ".");
		}
	}

	struct tpacket_stats kstats;
	socklen_t kstatslen = sizeof(kstats);
	if (getsockopt(rxline.sd, SOL_PACKET, PACKET_STATISTICS,
								(void*)&kstats, &kstatslen) < 0)
	{
		// do something
	}
	fprintf(stdout, " kstats.tp_drops:%d kstats.tp_packets:%d \n", kstats.tp_drops, kstats.tp_packets);
#endif

	int pktall = 0; // #packets sent by kernel
	int pktcnt = 0; // #packets we sent to port_owner for handling

	unsigned int cnt = 0;

	unsigned int i = rxline.rpos - 1;

	//fprintf(stdout, " [v] in:%s ", devname.c_str());

	while (cnt++ < rxline.req.tp_frame_nr)
	{
		i = (i == rxline.req.tp_frame_nr - 1) ? 0 : (i+1);

		//i = rxline.rpos;
		/*
		   Frame structure:

		   - Start. Frame must be aligned to TPACKET_ALIGNMENT=16
		   - struct tpacket_hdr
		   - pad to TPACKET_ALIGNMENT=16
		   - struct sockaddr_ll
		   - Gap, chosen so that packet data (Start+tp_net) aligns to
		     TPACKET_ALIGNMENT=16
		   - Start+tp_mac: [ Optional MAC header ]
		   - Start+tp_net: Packet data, aligned to TPACKET_ALIGNMENT=16.
		   - Pad to align to TPACKET_ALIGNMENT=16
		 */

		struct tpacket_hdr *hdr = (struct tpacket_hdr*)rxline.ring[i].iov_base;
		struct sockaddr_ll *sll = (struct sockaddr_ll*)((uint8_t*)hdr + TPACKET_ALIGN(sizeof(struct tpacket_hdr)));
		fetherframe ether(((uint8_t*)rxline.ring[i].iov_base + hdr->tp_mac), hdr->tp_len);


		switch (hdr->tp_status) {
		case TP_STATUS_KERNEL: // slot is empty, ignore
			// do nothing

			WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_revent() no frame in slot i:%d", devname.c_str(), i);

			continue;

			break;

		default: // data available in slot
			hdr->tp_status = TP_STATUS_KERNEL; // mark packet to be consumed by user space

			++pktall;

			if (ether.get_dl_src() == laddr)
			{
				WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_revent() self-originating "
						"frame rcvd in slot i:%d, ignoring", devname.c_str(), i);

				continue; // ignore self-originating frames
			}
			if (PACKET_OUTGOING == sll->sll_pkttype)
			{
				WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_revent() outgoing "
						"frame rcvd in slot i:%d, ignoring", devname.c_str(), i);

				continue; // ignore outgoing frames
			}

			++pktcnt;

			cpacket* pack = new cpacket(((uint8_t*)rxline.ring[i].iov_base + hdr->tp_mac), hdr->tp_len, 0);
			//pack->stored_bytes(hdr->tp_len);

			WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_revent() frame rcvd in slot i:%d", devname.c_str(), i);

			rx_pkts++; // this is not the counter used by cport !!!
			owner->enqueue(this, pack);

			rxline.rpos = (i == rxline.req.tp_frame_nr - 1) ? 0 : (i+1);;

			break;
		}
	}

#if 0
	fprintf(stdout, " [V] (all(%p:%s):%d) (rx_pkts:%ld | tx_pkts:%ld)\n",
			this, devname.c_str(), pktcnt, rx_pkts, tx_pkts);
#endif

	//fprintf(stdout, "cmmapport(%s)::handle_revent() ---------------------\n", devname.c_str());

	//fprintf(stdout, "frame-nr:%d pktall:%d pktcnt:%d\n", rxline.req.tp_frame_nr, pktall, pktcnt);
	//register_timer(/*throw timer*/1, /*in*/3/*second*/);
}


void
cmmapport::handle_out_queue()
{
	WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_out_queue() #slots:%d pout_queue.size():%d",
			devname.c_str(), txline.req.tp_frame_nr, pout_queue.size());

	//fprintf(stdout, "(%s) pout_queue.size(): +[%ld]\n", devname.c_str(), pout_queue.size());

	int pktall = 0;
	int pktblocked = 0;

	unsigned int i = txline.rpos;
	unsigned int cnt = 0;


	{
		Lock lock(&queuelock, true /*blocking*/);
		while (not pout_queue.empty())
		{
			tx_queue.push_back(pout_queue.front());
			pout_queue.pop_front();
		}
	}

	WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_out_queue() "
			"pout_queue.size():%d tx_queue.size():%d",
			devname.c_str(), pout_queue.size(), tx_queue.size());

	//fprintf(stdout, " [x] out:%s ", devname.c_str());

	while (++cnt < txline.req.tp_frame_nr)
	{

		/*
		   Frame structure:

		   - Start. Frame must be aligned to TPACKET_ALIGNMENT=16
		   - struct tpacket_hdr
		   - pad to TPACKET_ALIGNMENT=16
		   - struct sockaddr_ll
		   - Gap, chosen so that packet data (Start+tp_net) aligns to
			 TPACKET_ALIGNMENT=16
		   - Start+tp_mac: [ Optional MAC header ]
		   - Start+tp_net: Packet data, aligned to TPACKET_ALIGNMENT=16.
		   - Pad to align to TPACKET_ALIGNMENT=16
		 */

		struct tpacket_hdr *hdr = (struct tpacket_hdr*)txline.ring[i].iov_base;
	//	struct sockaddr_ll *sll = (struct sockaddr_ll*)((uint8_t*)hdr + TPACKET_ALIGN(sizeof(struct tpacket_hdr)));
		uint8_t *data = (uint8_t*)txline.ring[i].iov_base + TPACKET_HDRLEN - sizeof(struct sockaddr_ll);

		//fprintf(stdout, "rxline.rpos:%d sll.sll_ifindex: %d\n", rxline.rpos, sll->sll_ifindex);

		if (hdr->tp_status == TP_STATUS_AVAILABLE)
		{
			////fprintf(stdout, "txline.rpos:%d hdr->tp_status:%lu\n", txline.rpos, hdr->tp_status);

			WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_out_queue() "
					"slot i:%d is available",
					devname.c_str(),
					i);

			cpacket *pack = 0;

#if 0
			try {
#endif

				//Lock lock(&queuelock, true /*blocking*/);
				if (not tx_queue.empty())
				{
					pack = tx_queue.front();
					tx_queue.pop_front();
				}

#if 0
			} catch (eLockWouldBlock& e) {
				WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_out_queue() lock failed, "
						"moving on to transmission", devname.c_str());
				throw eDebug();
				goto txmit;
			}
#endif

			if (0 != pack)
			{
				WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_out_queue() "
						"storing pack:%p in slot i:%d", devname.c_str(), pack, i);

				//fprintf(stdout, "cmmapport(%s)::handle_out_queue() "
				//		"storing pak:%p in slot i:%d\n", devname.c_str(), pack, i);

				pack->pack(data, txline.req.tp_frame_size);
#if 0
				memcpy(data, pack->soframe(), pack->framelen());
#endif

				hdr->tp_status = TP_STATUS_SEND_REQUEST;
				hdr->tp_len = pack->length();

				++pktall;

				//fprintf(stdout, "pack(%p) ", pack);

				tx_pkts++;

#if 0
				fprintf(stdout, "pack:%p "
								"reception: %s "
								"cdpath-in: %s "
								"cfwdengine-in: %s "
								"cfwdengine-out: %s "
								"cdpath-out: %s "
								"cport-enqueue: %s "
								"cport-send: %s\n",
								pack,
								pack->time_cport_recv.c_str(),
								pack->time_cdpath_in.c_str(),
								pack->time_cfwdengine_in.c_str(),
								pack->time_cfwdengine_out.c_str(),
								pack->time_cdpath_out.c_str(),
								pack->time_cport_enqueue.c_str(),
								pack->time_cport_send.c_str());
#endif

				delete pack;
			}
			else
			{
				WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_out_queue() "
						"no frame available for transmission", devname.c_str());

				goto txmit;
			}

#if 0
			fprintf(stdout, "cnt:%d txline.rpos:%d sll.sll_ifindex: %d => hdr->tp_status: %lu hdr->tp_len: %d\n",
					i, txline.rpos, sll->sll_ifindex, hdr->tp_status, hdr->tp_len);
#endif

		}
		else
		{
			WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_out_queue() "
					"slot i:%d is not available",
					devname.c_str(),
					i);

			++pktblocked;
		}

		i = (i == (txline.req.tp_frame_nr - 1)) ? 0 : i+1;
	}

txmit:

//fprintf(stdout, "(%s) rx_pkts:%lu tx_pkts:%lu\n", devname.c_str(), rx_pkts, tx_pkts);
#if 0
	fprintf(stdout, " [X] (all(%p:%s):%d) (tx_pkts:%ld | rx_pkts:%ld)\n",
			this, devname.c_str(), pktall, tx_pkts, rx_pkts);
#endif

	txline.rpos = i;

	//fprintf(stdout, "pktblocked:%d\n", pktblocked);

	//fprintf(stdout, "pktall:%d\n", pktall);

	send(txline.sd, 0,0,0);

	Lock lock(&queuelock, true /*blocking*/);
	if (not tx_queue.empty() or not pout_queue.empty())
	{
		WRITELOG(CPORT, DBG, "cmmapport(%s)::handle_out_queue() "
				"RESCHEDULING pout_queue.size():%d tx_queue.size():%d",
				devname.c_str(), pout_queue.size(), tx_queue.size());

		register_timer(CPORT_TIMER_POUT_QUEUE, 0);
	}
}


void
cmmapport::handle_wevent(
		int fd)
{
	// TODO: handle txline
	deregister_filedesc_w(txline.sd);
	handle_out_queue();
}

#endif /* HARDWARE */
