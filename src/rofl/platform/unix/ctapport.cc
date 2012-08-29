/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ctapport.h"

std::set<ctapport*> ctapport::ctapport_list;

ctapport::ctapport(std::string devname, int port_no, cmacaddr const& maddr) :
	clinuxport(devname, std::string("phy"), port_no),
	fd(-1)
{
	WRITELOG(CPORT, ROFL_DBG, "ctapport(%p)::ctapport dev(%s)", this, devname.c_str());
	tap_open(devname);
	ctapport_list.insert(this);

	if (not maddr.is_null())
	{
		try {
			disable_interface();
			set_hw_addr(maddr);
			enable_interface();
		} catch (ePortSetHwAddrFailed& e) {
			WRITELOG(CPORT, ROFL_WARN, "port (%s): "
					"setting MAC address failed",
					devname.c_str());
		}
	}

	get_port_no();
	get_config();
	get_hw_addr();
	WRITELOG(CPORT, ROFL_DBG, c_str());
}


ctapport::~ctapport()
{
	WRITELOG(CPORT, ROFL_DBG, "ctapport(%p)::~ctapport dev(%s)", this, devname.c_str());
	ctapport_list.erase(this);
	tap_close();
}


void
ctapport::handle_revent(int fd)
{
	WRITELOG(CPORT, ROFL_DBG, "ctapport(%p)::handle_revent()", this);
	class cpacket * pack = new cpacket();

	try {
		cmemory *mem = new cmemory(1526);

		int rc = read(fd, (void*)mem->somem(), mem->memlen());

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

			WRITELOG(CPORT, ROFL_DBG, "ctapport(%p)::handle_revent() %s", this, pack->c_str());

			port_owner()->store(this, new cpacket(mem, port_no));
			port_owner()->enqueue(this);
		}

	} catch (eSocketReadFailed& e) {
		delete pack;

	} catch (ePortNotFound& e) {
		delete pack;
	}
}


void
ctapport::handle_out_queue()
{
	cport::register_filedesc_w(fd); // register our socket for write operation
}


void
ctapport::handle_wevent(int fd)
{
	int rc;

	if (not pout_queue.empty())
	{

		class cpacket * pack = pout_queue.front();

		WRITELOG(CPORT, ROFL_DBG, "ctapport(%p)::handle_wevent() pout_queue.size()=%d", this, (int)pout_queue.size());
		WRITELOG(CPORT, ROFL_DBG, "ctapport(%p)::handle_wevent() pout_queue.size()=%d %s", this, (int)pout_queue.size(), pack->c_str());

		WRITELOG(CPORT, ROFL_DBG, "ctapport(%p)::handle_wevent() "
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
			WRITELOG(CPORT, ROFL_DBG, "ctapport(%p)::handle_wevent() wrote %d bytes to socket %d", this, rc, fd);
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
}


void
ctapport::tap_close()
{
	if (fd > 0)
	{
		deregister_filedesc_r(fd);
		close(fd);
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



