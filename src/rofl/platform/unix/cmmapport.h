/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMMAPPORT_H
#define CMMAPPORT_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/if_packet.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>

//#include <linux/uio.h>
#ifdef __cplusplus
}
#endif

#include <sys/socket.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <set>
#include <string>
#include <algorithm>

#include "rofl/common/cpacket.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/caddress.h"
#include "rofl/common/cerror.h"
#include "rofl/common/csocket.h" // for csocket error types
#include <rofl/platform/unix/clinuxport.h>

namespace rofl
{

#if 0
class cmapport_helper : csyslog {
public:
	static cmapport_helper &
	get_instance() {
		return instance;
	}

	void
	autocreate_ports();

private:
	static cmapport_helper instance;

	std::set<std::string> if_names;

	static void
	create_ports(const std::string &s);

	cmapport_helper();
	~cmapport_helper();
};
#endif


class eMMapPortBase 			: public cerror {}; // base error class for cmmapport
class eMMapPortSocketFailed 	: public eMMapPortBase {};
class eMMapPortIfaceNotFound 	: public eMMapPortBase {};
class eMMapPortBindFailed 		: public eMMapPortBase {};
class eMMapFailed 				: public eMMapPortBase {};



class cmmapdev :
	public ciosrv
{
private:

	cmemory 			ringptrs; 	// memory for storing the (struct iovec*) pointers

public:

	int 				block_size;
	int 				n_blocks;
	int 				frame_size;
	std::string 		devname; 	// device name e.g. "eth0"
	int 				ring_type;
	int 				sd; 		// socket descriptor
	caddress 			ll_addr; 	// link layer sockaddr
	struct tpacket_req 	req; 		// ring buffer
	struct iovec 		*ring; 		// auxiliary pointers into the mmap'ed area
	unsigned int 		rpos; 		// current position within ring buffer

public:

	/**
	 *
	 */
	cmmapdev(
			int ring_type,
			std::string devname,
			int block_size = 8,
			int n_blocks = 8,
			int frame_size = 2048);

	/**
	 *
	 */
	virtual
	~cmmapdev();


private:


	/**
	 *
	 */
	void
	initialize() throw (eMMapFailed);
};







class cmmapport :
	public clinuxport
{
protected:

	pthread_mutex_t			queuelock;
	std::list<cpacket*> 	tx_queue;
	cmmapdev 				txline;
	cmmapdev 				rxline;
	cmacaddr 				laddr; // our own mac address
	uint64_t 				tx_pkts;
	uint64_t 				rx_pkts;


public:

	/**
	 *
	 */
	cmmapport(
			cport_owner *owner,
			std::string devname,
			int port_no,
			int block_size = 8,
			int n_blocks = 8,
			int frame_size = 2048);

	/**
	 *
	 */
	virtual
	~cmmapport();


protected: // overloaded from ciosrv

	/**
	 *
	 */
	virtual void
	handle_timeout(
			int opaque);

	/**
	 *
	 */
	virtual void
	handle_revent(
			int fd);

	/**
	 *
	 */
	virtual void
	handle_wevent(
			int fd);

protected: // overloaded from cport

	/**
	 * handle out queue
	 */
	virtual void
	handle_out_queue();

protected: // overloaded from ciosrv

	/**
	 *
	 */
	virtual void
	handle_event(cevent const& ev);
};

}


#endif
