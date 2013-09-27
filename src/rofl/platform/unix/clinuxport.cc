/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * clinuxport.cc
 *
 *  Created on: May 2, 2012
 *      Author: tobi
 */

#include "clinuxport.h"

using namespace rofl;

clinuxport::clinuxport(
		cport_owner *owner,
		std::string devname,
		std::string devtype) :
		cport(owner, devname, devtype),
		ifindex(0)
{

}

void clinuxport::enable_interface() throw (ePortSocketCallFailed,
		ePortIoctlCallFailed) {
	struct ifreq ifr;
	int sd, rc;

	if ((sd = socket(AF_PACKET, SOCK_RAW, 0)) < 0)
		throw ePortSocketCallFailed();

	memset(&ifr, 0, sizeof(struct ifreq));
	strcpy(ifr.ifr_name, devname.c_str());

	if ((rc = ioctl(sd, SIOCGIFINDEX, &ifr)) < 0)
		throw ePortIoctlCallFailed();

	if ((rc = ioctl(sd, SIOCGIFFLAGS, &ifr)) < 0) {
		close(sd);
		throw ePortIoctlCallFailed();
	}

	ifr.ifr_flags |= IFF_UP;

	if ((rc = ioctl(sd, SIOCSIFFLAGS, &ifr)) < 0) {
		close(sd);
		throw ePortIoctlCallFailed();
	}

	close(sd);
}

void
clinuxport::disable_interface() throw (ePortSocketCallFailed,
		ePortIoctlCallFailed) {
	struct ifreq ifr;
	int sd, rc;

	if ((sd = socket(AF_PACKET, SOCK_RAW, 0)) < 0)
		throw ePortSocketCallFailed();

	memset(&ifr, 0, sizeof(struct ifreq));
	strcpy(ifr.ifr_name, devname.c_str());

	if ((rc = ioctl(sd, SIOCGIFINDEX, &ifr)) < 0)
		throw ePortIoctlCallFailed();

	if ((rc = ioctl(sd, SIOCGIFFLAGS, &ifr)) < 0) {
		close(sd);
		throw ePortIoctlCallFailed();
	}

	ifr.ifr_flags &= ~IFF_UP;

	if ((rc = ioctl(sd, SIOCSIFFLAGS, &ifr)) < 0) {
		close(sd);
		throw ePortIoctlCallFailed();
	}

	close(sd);
}

uint32_t clinuxport::get_port_no_from_kernel() throw (ePortSocketCallFailed,
		ePortIoctlCallFailed) {
	// get ifindex for devname
	int sd, rc;
	if ((sd = socket(AF_PACKET, SOCK_RAW, 0)) < 0)
		throw ePortSocketCallFailed();
	struct ifreq ifr;
	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, devname.c_str(), IFNAMSIZ);
	if ((rc = ioctl(sd, SIOCGIFINDEX, &ifr)) < 0)
		throw ePortIoctlCallFailed();
	ifindex = ifr.ifr_ifindex;
	close(sd);
	return ifr.ifr_ifindex;
}

cmacaddr&
clinuxport::get_hw_addr() throw (ePortSocketCallFailed, ePortIoctlCallFailed) {
	// get ifr_ifhwaddr for devname
	int sd, rc;
	if ((sd = socket(AF_PACKET, SOCK_RAW, 0)) < 0)
		throw ePortSocketCallFailed();
	struct ifreq ifr;
	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, devname.c_str(), IFNAMSIZ);
	if ((rc = ioctl(sd, SIOCGIFHWADDR, &ifr)) < 0)
		throw ePortIoctlCallFailed();
	memcpy(hwaddr.somem(), ifr.ifr_hwaddr.sa_data, OFP_ETH_ALEN);
	close(sd);

	return hwaddr;
}

void
clinuxport::set_hw_addr(cmacaddr const& maddr)
		throw (ePortSocketCallFailed, ePortInval, ePortSetHwAddrFailed) {
	// get ifr_ifhwaddr for devname
	int sd, rc;
	if ((sd = socket(AF_PACKET, SOCK_RAW, 0)) < 0)
		throw ePortSocketCallFailed();
	struct ifreq ifr;
	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, devname.c_str(), IFNAMSIZ);

	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	memcpy(ifr.ifr_hwaddr.sa_data, maddr.somem(), OFP_ETH_ALEN);

	if ((rc = ioctl(sd, SIOCSIFHWADDR, &ifr)) < 0) {
		fprintf(stderr, "rc=%d errno=%d (%s)\n", rc, errno, strerror(errno));
		throw ePortSetHwAddrFailed();
	}

	close(sd);
}

uint32_t
clinuxport::get_state() throw (ePortSocketCallFailed,
		ePortIoctlCallFailed) {
	// get ifr_flags for devname
	int sd, rc;
	if ((sd = socket(AF_PACKET, SOCK_RAW, 0)) < 0)
		throw ePortSocketCallFailed();
	struct ifreq ifr;
	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, devname.c_str(), IFNAMSIZ);
	if ((rc = ioctl(sd, SIOCGIFFLAGS, &ifr)) < 0)
		throw ePortIoctlCallFailed();
	if (ifr.ifr_flags & IFF_UP)
		state &= ~OFP12PS_LINK_DOWN;
	else if (!(ifr.ifr_flags & IFF_UP))
		state |= OFP12PS_LINK_DOWN;
	close(sd);

	return state;
}

uint32_t
clinuxport::get_curr_speed()
{
	// fixme this should be read via ioctl/netlink
	return curr_speed;
}

uint32_t
clinuxport::get_max_speed()
{
	// fixme this should be read via ioctl/netlink
	return max_speed;
}
