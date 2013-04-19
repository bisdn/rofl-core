/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "csocket.h"

using namespace rofl;

std::set<csocket*> csocket::csock_list;



csocket::csocket(
		csocket_owner *owner,
		int domain,
		int type,
		int protocol,
		int backlog) :
	socket_owner(owner),
	sd(-1),
	laddr(domain),
	raddr(domain),
	domain(domain),
	type(type),
	protocol(protocol),
	backlog(backlog)
{
	WRITELOG(CSOCKET, DBG, "csocket(%p)::csocket()", this);

	pthread_rwlock_init(&pout_squeue_lock, 0);

	csock_list.insert(this);
}



csocket::csocket(
		csocket_owner *owner,
		int sd,
		caddress const& ra,
		int domain,
		int type,
		int protocol,
		int backlog) :
	socket_owner(owner),
	sd(sd),
	laddr(domain),
	raddr(ra),
	domain(domain),
	type(type),
	protocol(protocol),
	backlog(backlog)
{

	WRITELOG(CSOCKET, DBG, "csocket(%p)::csocket()", this);

	pthread_rwlock_init(&pout_squeue_lock, 0);

	sockflags.set(CONNECTED);

	csock_list.insert(this);
	register_filedesc_r(sd);
}



csocket::~csocket()
{

	WRITELOG(CSOCKET, DBG, "csocket(%p)::~csocket()", this);

	cclose();

	pthread_rwlock_destroy(&pout_squeue_lock);

	csock_list.erase(this);
}



void
csocket::handle_revent(int fd)
{
	// handle socket when in listening state
	if (sockflags[SOCKET_IS_LISTENING]) {
		int new_sd;
		caddress ra(domain);

		if ((new_sd = accept(sd, (struct sockaddr*)(ra.ca_saddr), &(ra.salen))) < 0) {
			switch (errno) {
			case EAGAIN:
				// do nothing, just wait for the next event
				return;
			default:
				throw eSocketAcceptFailed();
			}
		}

		handle_accepted(new_sd, ra);

		// handle socket when in normal (=non-listening) state
	} else {

		WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_revent()", this);
		// call method in derived class
		handle_read(fd);
	}
}



void
csocket::handle_wevent(int fd)
{
	WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_wevent()", this);

	if (sockflags[CONNECT_PENDING])
	{
		int rc;
		int optval = 0;
		int optlen = sizeof(optval);
		if ((rc = getsockopt(sd, SOL_SOCKET, SO_ERROR,
							 (void*)&optval, (socklen_t*)&optlen)) < 0)
			throw eSocketError();

		switch (optval) {
		case 0:
		//case EISCONN:
			{
				WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_wevent() "
						"connection established to %s",
						this, raddr.addr_c_str());

				sockflags[CONNECT_PENDING] = false;
				register_filedesc_w(sd);

				register_filedesc_r(sd);

				sockflags[CONNECTED] = true;
				handle_connected();
			}
			break;
		case EINPROGRESS:
			{
				WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_wevent() "
						"connection establishment to %s is in progress",
						this, raddr.addr_c_str());

				// do nothing
			}
			break;
		case ECONNREFUSED:
			{
				WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_wevent() "
						"connection to %s failed",
						this, raddr.addr_c_str());

				cclose();
				handle_conn_refused();
			}
			break;
		default:
			{
				WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_wevent() "
						"connection establishment to %s => an error occured, errno: %d",
						this, raddr.addr_c_str(), optval);

				throw eSocketError();
			}
		}

	}
	else
	{
		dequeue_packet();
	}
}


void
csocket::handle_xevent(int fd)
{
	WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_xevent()", this);
}



void
csocket::clisten(
	caddress la,
	int domain, 
	int type, 
	int protocol, 
	int backlog,
	std::string devname) throw(eSocketError, eSocketListenFailed, eSocketAddressInUse)
{
	int rc;
	this->domain = domain;
	this->type = type;
	this->protocol = protocol;
	this->backlog = backlog;
	this->laddr = la;

	WRITELOG(CSOCKET, DBG, "csocket(%p)::cpopen(la=%s, domain=%d, type=%d, protocol=%d, backlog=%d)",
			 this, la.c_str(), domain, type, protocol, backlog);

	if (sd >= 0)
		cclose();

	// open socket
	if ((sd = socket(domain, type, protocol)) < 0)
		throw eSocketError();



	// make socket non-blocking
	long flags;
	if ((flags = fcntl(sd, F_GETFL)) < 0)
		throw eSocketError();
	flags |= O_NONBLOCK;
	if ((rc = fcntl(sd, F_SETFL, flags)) < 0)
		throw eSocketError();


	if ((type == SOCK_STREAM) && (protocol == IPPROTO_TCP))
	{
		int optval = 1;

		// set SO_REUSEADDR option on TCP sockets
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (int*)&optval, sizeof(optval))) < 0)
			throw eSocketError();

#if 0
		int on = 1;
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) < 0)
			throw eSocketError();
#endif

		// set TCP_NODELAY option on TCP sockets
		if ((rc = setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optval, sizeof(optval))) < 0)
			throw eSocketError();

		// set SO_RCVLOWAT
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_RCVLOWAT, (int*)&optval, sizeof(optval))) < 0)
			throw eSocketError();

		// read TCP_NODELAY option for debugging purposes
		socklen_t optlen = sizeof(int);
		int optvalc;
		if ((rc = getsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optvalc, &optlen)) < 0)
			throw eSocketError();
	}
	else if ((type == SOCK_RAW) && (domain == PF_PACKET) && (not devname.empty()))
	{
		struct ifreq ifr;
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, devname.c_str(), sizeof(ifr.ifr_name));

		// read flags from interface
		if ((rc = ioctl(sd, SIOCGIFFLAGS, &ifr)) < 0)
			throw eSocketIoctl();

		// enable promiscuous flags
		ifr.ifr_flags |= IFF_PROMISC;
		if ((rc = ioctl(sd, SIOCSIFFLAGS, &ifr)) < 0)
			throw eSocketIoctl();

		// set SO_SNDBUF
		int optval = 1600;
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_SNDBUF, (int*)&optval, sizeof(optval))) < 0)
			throw eSocketError();
	}


	// bind to local address
	if ((rc = bind(sd, la.ca_saddr,
				   (socklen_t)(la.salen))) < 0)
	{
		switch (rc) {
		case EADDRINUSE:
			throw eSocketAddressInUse();
		default:
			throw eSocketBindFailed();
		}
	}



	switch (type) {
	case SOCK_STREAM:
		// listen on socket
		if ((rc = listen(sd, backlog)) < 0)
			throw eSocketListenFailed();
		sockflags[SOCKET_IS_LISTENING] = true;
		break;
	case SOCK_DGRAM:
		// do nothing
		sockflags.set(CONNECTED);
		break;
	case SOCK_RAW:
		// do nothing
		sockflags[RAW_SOCKET] = true;
		break;
	}

	// setup was successful, register sd for read events
	register_filedesc_r(sd);
}


void
csocket::cconnect(
	caddress ra,
	caddress la,
	int domain, 
	int type, 
	int protocol) throw (eSocketError, eSocketConnectFailed)
{
	int rc;
	this->domain = domain;
	this->type = type;
	this->protocol = protocol;
	this->laddr = la;
	this->raddr = ra;

	if (sd >= 0)
		cclose();

	std::string lastr;
	lastr.assign(la.c_str());
	WRITELOG(CSOCKET, DBG, "csocket(%p)::caopen(ra=%s, la=%s, domain=%d, type=%d, protocol=%d)",
			 this, ra.c_str(), lastr.c_str(), domain, type, protocol);

	// open socket
	if ((sd = socket(domain, type, protocol)) < 0)
		throw eSocketError();

	// make socket non-blocking
	long flags;
	if ((flags = fcntl(sd, F_GETFL)) < 0)
		throw eSocketError();
	flags |= O_NONBLOCK;
	if ((rc = fcntl(sd, F_SETFL, flags)) < 0)
		throw eSocketError();

	if ((type == SOCK_STREAM) && (protocol == IPPROTO_TCP)) {
		int optval = 1;

		// set SO_REUSEADDR option
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) < 0)
			throw eSocketError();

		// set TCP_NODELAY option
		if ((rc = setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval))) < 0)
			throw eSocketError();
	}

	// bind to local address
	if ((rc = bind(sd, la.ca_saddr, (socklen_t)(la.salen))) < 0) {
		switch (errno) {
		default:
			throw eSocketError();
		}
	}

	// connect to remote address
	if ((rc = connect(sd, (const struct sockaddr*)ra.ca_saddr, (socklen_t)ra.salen)) < 0) {
		switch (errno) {
		case EINPROGRESS:
			sockflags[CONNECT_PENDING] = true;
			// connect is pending, register sd for write events
			register_filedesc_w(sd);
			break;
		case ECONNREFUSED:
			cclose();
			handle_conn_refused();
			break;
		default:
			throw eSocketConnectFailed();
		}
	} else {
		// connect was successful, register sd for read events
		register_filedesc_r(sd);
		sockflags.set(CONNECTED);
		handle_connected();
	}
}



void
csocket::cclose()
{
	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	WRITELOG(CSOCKET, DBG, "csocket(%p)::cclose()", this);
	int rc;

	if (sd == -1)
		return;

	deregister_filedesc_r(sd);
	deregister_filedesc_w(sd);
	if (not sockflags[RAW_SOCKET]) {
		if ((rc = shutdown(sd, SHUT_RDWR)) < 0) {
			WRITELOG(CSOCKET, DBG, "csocket(%p)::cclose() "
				"error code (%d:%s)", this, errno, strerror(errno));
			}
	}
	if ((rc = close(sd)) < 0) {
		WRITELOG(CSOCKET, DBG, "csocket(%p)::cclose() "
				"error code (%d:%s)", this, errno, strerror(errno));
	}

	sd = -1;
	// keep SOCKET_IS_LISTENING in case of failed connection and
	// required reestablishing
	//	sockflags[SOCKET_IS_LISTENING] = false;
	sockflags[RAW_SOCKET] = false;
	sockflags.reset(CONNECTED);

	// purge pout_squeue
	while (not pout_squeue.empty()) {
		cmemory *mem = pout_squeue.front();
		delete mem;
		pout_squeue.pop_front();
	}

}



void
csocket::send_packet(cmemory* pack)
{
	WRITELOG(CSOCKET, DBG, "csocket(%p)::send_packet() pack: %s", this, pack->c_str());
	if (not sockflags.test(CONNECTED) && not sockflags.test(RAW_SOCKET))
	{
		WRITELOG(UNKNOWN, WARN, "csocket(%p)::send_packet() not connected, dropping packet", this);
		delete pack;
		return;
	}

	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	pout_squeue.push_back(pack);
	register_filedesc_w(sd);
}



void
csocket::dequeue_packet() throw (eSocketSendFailed, eSocketShortSend)
{
	{
		RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

		int rc;
		WRITELOG(CSOCKET, DBG, "csocket(%p)::dequeue_packet() pout_squeue.size()=%d",
				this, pout_squeue.size());

		while (not pout_squeue.empty())
		{
			cmemory *pack = pout_squeue.front();

			int flags = MSG_NOSIGNAL;
			if ((rc = sendto(sd, pack->somem(), pack->memlen(), flags, NULL, 0)) < 0)
			{
				WRITELOG(CSOCKET, DBG, "csocket(%p)::dequeue_packet() "
						"errno=%d (%s) pack: %s",
						this, errno, strerror(errno), pack->c_str());

				switch (errno) {
				case EAGAIN:
					break;
				case EPIPE:

					goto out;
					return;
				case EMSGSIZE:
					WRITELOG(CSOCKET, DBG, "csocket(%p)::dequeue_packet() "
							"dropping packet, errno=%d (%s) pack: %s",
							this, errno, strerror(errno), pack->c_str());
					break;
				default:
					WRITELOG(CSOCKET, DBG, "csocket(%p)::dequeue_packet() "
							"errno=%d (%s) pack: %s",
							this, errno, strerror(errno), pack->c_str());
					throw eSocketSendFailed();
				}
			}
			else if ((rc < (int)pack->memlen()))
			{
				throw eSocketShortSend();
			}

			WRITELOG(CSOCKET, DBG, "csocket(%p)::dequeue_packet() "
					"wrote %d bytes to socket %d", this, rc, sd);

			pout_squeue.pop_front();

			delete pack;
		}

		if (pout_squeue.empty())
		{
			deregister_filedesc_w(sd);
		}

		return;
	} // unlocks pout_squeue_lock
out:
	cclose(); // clears also pout_squeue
	handle_closed(sd);
}

