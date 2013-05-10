/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ctapport.h"

using namespace rofl;

std::set<ctapport*> ctapport::ctapport_list;

ctapport::ctapport(
		cport_owner *owner,
		std::string devname,
		cmacaddr const& maddr) :
	clinuxport(owner, devname, std::string("phy")),
	fd(-1)
{
	WRITELOG(CPORT, DBG, "ctapport(%p)::ctapport dev(%s)", this, devname.c_str());
	tap_open(devname);
	ctapport_list.insert(this);

	if (not maddr.is_null())
	{
		try {
			disable_interface();
			set_hw_addr(maddr);
			enable_interface();
		} catch (ePortSetHwAddrFailed& e) {
			WRITELOG(CPORT, WARN, "port (%s): "
					"setting MAC address failed",
					devname.c_str());
		}
	}

	get_hw_addr();
	WRITELOG(CPORT, DBG, c_str());
}


ctapport::~ctapport()
{
	WRITELOG(CPORT, DBG, "ctapport(%p)::~ctapport dev(%s)", this, devname.c_str());
	ctapport_list.erase(this);
	tap_close();
}


void
ctapport::handle_revent(int fd)
{
	WRITELOG(CPORT, DBG, "ctapport(%p)::handle_revent()", this);

	cmemory *mem = new cmemory(1526);

	try {

		int rc = read(fd, (void*)mem->somem(), mem->memlen());

		mem->resize(rc);

		// error occured (or non-blocking)
		if (rc < 0)
		{
			switch (errno) {
			case EAGAIN:
				break;
			default:
				delete this;
				return;
			}

			throw eSocketReadFailed();
		}
		else
		{
			owner->enqueue(this, new cpacket(mem));
		}

	} catch (eSocketReadFailed& e) {
		delete mem;

	} catch (ePortNotFound& e) {
		delete mem;

	}
}


void
ctapport::handle_out_queue()
{
	WRITELOG(CPORT, DBG, "ctapport(%s)::handle_wevent() pout_queue.size()=%d", devname.c_str(), (int)pout_queue.size());
	cport::register_filedesc_w(fd); // register our socket for write operation
}


void
ctapport::handle_wevent(int fd)
{
	int rc;

	if (not pout_queue.empty())
	{

		class cpacket * pack = pout_queue.front();

		WRITELOG(CPORT, DBG, "ctapport(%p)::handle_wevent() pout_queue.size()=%d", this, (int)pout_queue.size());
		WRITELOG(CPORT, DBG, "ctapport(%p)::handle_wevent() pout_queue.size()=%d %s", this, (int)pout_queue.size(), pack->c_str());

		WRITELOG(CPORT, DBG, "ctapport(%p)::handle_wevent() "
				 "pack[%p]:%s",
				 this, pack, pack->c_str());

		if (pack)
		{
			cmemory mem(pack->length());

			pack->pack(mem.somem(), mem.memlen());

			if ((rc = write(fd, mem.somem(), mem.memlen())) < 0)
			{
				fprintf(stderr, "ERROR: errno=%d error=%s\n", errno, strerror(errno));
				switch (errno) {
				case EAGAIN:
					// keep fd in wfds
					return;
				default:
					throw eTapPortWriteFailed();
				}
			}
			WRITELOG(CPORT, DBG, "ctapport(%p)::handle_wevent() wrote %d bytes to socket %d", this, rc, fd);
		}

		delete pack;
		pout_queue.pop_front();
	}

	if (pout_queue.empty())
		cport::deregister_filedesc_w(fd);
}


void
ctapport::tap_open(std::string devname)
{
	struct ifreq ifr;
	int rc;

	if ((fd = open("/dev/net/tun", O_RDWR|O_NONBLOCK)) < 0) {
#ifndef NDEBUG
		fprintf(stderr, "ctapport(%s)::tap_open() errno: %d(%s)\n", devname.c_str(), errno, strerror(errno));
#endif
		throw eTapPortOpenFailed();
	}

	memset(&ifr, 0, sizeof(ifr));

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	 *        IFF_TAP   - TAP device
	 *
	 *        IFF_NO_PI - Do not provide packet information
	 */
	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
	strncpy(ifr.ifr_name, devname.c_str(), IFNAMSIZ);

	if ((rc = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
		close(fd);
		throw eTapPortInval();
	}

	enable_interface();

	register_filedesc_r(fd);

#if 0
	if (0 != owner) {
		owner->port_open(this);
	}
#endif
}


void
ctapport::tap_close()
{
	if (fd > 0)
	{
		deregister_filedesc_r(fd);
		close(fd);

#if 0
		if (0 != owner) {
			owner->port_close(this);
		}
#endif
	}
}


ctapport*
ctapport::find(std::string devname)
{
	std::set<ctapport*>::iterator it;
	if ((it = find_if(ctapport::ctapport_list.begin(),
					  ctapport::ctapport_list.end(),
					  ctapport::ctapport_search(devname))) == ctapport::ctapport_list.end())
	{
		return NULL;
	}
	return(*it);
}



