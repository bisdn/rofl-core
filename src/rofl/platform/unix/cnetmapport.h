/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef NETMAP

#ifndef CNETMAPPORT_H
#define CNETMAPPORT_H 1

#include <inttypes.h>
#include <net/netmap.h>
#include <net/netmap_user.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "rofl/common/cvastring.h"
#include "rofl/common/ciosrv.h"
#include "rofl/common/cerror.h"
#include "clinuxport.h"

#ifndef HARDWARE

class cnetmapport :
	public clinuxport
{
public:

	/**
	 *
	 */
	cnetmapport(
			std::string devname,
			int port_no);

	/**
	 *
	 */
	virtual
	~cnetmapport();

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

private: // methods

	/**
	 *
	 */
	void
	netmap_open() throw (ePortOpenFailed);

	/**
	 *
	 */
	void
	netmap_close() throw (ePortCloseFailed);

	/**
	 *
	 */
	void
	netmap_read();

	/**
	 *
	 */
	void
	netmap_write();

protected:

	std::list<cpacket*> tx_queue;
	cmacaddr laddr; // our own mac address

#define NETMAP_DEVICE "/dev/netmap"

	int fd; // netmap file descriptor
	struct nmreq nmr; // netmap request structure
	struct netmap_ring *ring; // netmap pointer to ring buffer
	struct netmap_if *nifp; // netmap interface pointer in ring buffer
	void *mapped_region;

	uint64_t tx_pkts;
	uint64_t rx_pkts;

private:


};

#endif   // HARDWARE

#endif

#endif // NETMAP
