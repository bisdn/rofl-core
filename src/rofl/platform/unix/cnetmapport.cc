/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef NETMAP

#include "cnetmapport.h"


cnetmapport::cnetmapport(
		std::string devname,
		int port_no) :
	clinuxport(devname, std::string("phy"), port_no),
	fd(-1),
	ring(0),
	nifp(0),
	mapped_region(MAP_FAILED)
{

}


cnetmapport::~cnetmapport()
{

}


void
cnetmapport::handle_timeout(int opaque)
{

}


void
cnetmapport::handle_revent(int fd) // throw (ePortNotOpen) ??? is that working?
{
	cvastring vas;

	if (0 == mapped_region)
	{
		throw ePortNotOpen(vas("cnetmapport(%s)::handle_revent() port is closed", devname.c_str()));
	}

	for (unsigned int q = 0 ; q < nifp->ni_rx_rings; q++)
	{
		struct netmap_ring *rxring = NETMAP_RXRING(nifp, q);
		while ((rxring->avail--) > 0)
		{
			int i = rxring->cur;
			char *buf = NETMAP_BUF(rxring, rxring->slot[i].buf_idx);
			fprintf(stderr, "buf:%p\n", buf);
		}
	}
}


void
cnetmapport::handle_wevent(int fd)
{

}


void
cnetmapport::handle_out_queue()
{

}


void
cnetmapport::handle_event(const cevent& ev)
{

}



/*
 * netmap related methods bottom-handler
 */
void
cnetmapport::netmap_open() throw (ePortOpenFailed)
{
	cvastring vas;
	int rc = 0;

	if ((mapped_region != MAP_FAILED) || (fd > -1))
	{
		netmap_close();
	}

	if ((fd = open(NETMAP_DEVICE, O_RDWR)) < 0)
	{
		throw ePortOpenFailed(std::string(vas("cnetmapport(%s)::netmap_open() open failed => errno:%d (%s)",
				devname.c_str(), errno, strerror(errno))));
	}

	bzero(&nmr, sizeof(nmr));
	strcpy(nmr.nr_name, devname.c_str());
	if ((rc = ioctl(fd, NIOCREGIF, &nmr)) < 0)
	{
		throw ePortOpenFailed(std::string(vas("cnetmapport(%s)::netmap_open() ioctl failed => errno:%d (%s)",
				devname.c_str(), errno, strerror(errno))));
	}

	if ((mapped_region = mmap(0, nmr.nr_memsize,
			PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED,
			fd, /*offset*/0)) == MAP_FAILED)
	{
		throw ePortOpenFailed(std::string(vas("cnetmapport(%s)::netmap_open() mmap failed => errno:%d (%s)",
				devname.c_str(), errno, strerror(errno))));
	}

	nifp = NETMAP_IF(mapped_region, nmr.nr_offset);
	ring = NETMAP_TXRING(nifp, 0);

	// make socket non-blocking
	long flags;
	if ((flags = fcntl(fd, F_GETFL)) < 0)
	{
		throw ePortOpenFailed(std::string(vas("cnetmapport(%s)::netmap_open() fcntl failed => errno:%d (%s)",
				devname.c_str(), errno, strerror(errno))));
	}
	flags |= O_NONBLOCK;
	if ((rc = fcntl(fd, F_SETFL, flags)) < 0)
	{
		throw ePortOpenFailed(std::string(vas("cnetmapport(%s)::netmap_open() fcntl failed => errno:%d (%s)",
				devname.c_str(), errno, strerror(errno))));
	}
}


void
cnetmapport::netmap_close() throw (ePortCloseFailed)
{
	cvastring vas;
	int rc = 0;

	if (mapped_region != MAP_FAILED)
	{
		munmap(mapped_region, nmr.nr_memsize); // can this fail? what happens, if it fails?
		mapped_region = MAP_FAILED;
	}

	bzero(&nmr, sizeof(nmr));
	strcpy(nmr.nr_name, devname.c_str()); // TODO: add port-no here
	if ((rc = ioctl(fd, NIOCUNREGIF, &nmr)) < 0)
	{
		throw ePortOpenFailed(std::string(vas("cnetmapport(%s)::netmap_close() ioctl failed => errno:%d (%s)",
				devname.c_str(), errno, strerror(errno))));
	}

	if (fd != -1)
	{
		close(fd);
		fd = -1;
	}
}


void
cnetmapport::netmap_read()
{

}


void
cnetmapport::netmap_write()
{

}

#endif
