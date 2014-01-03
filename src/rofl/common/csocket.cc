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
	backlog(backlog),
	reconnect_start_timeout(RECONNECT_START_TIMEOUT),
	reconnect_in_seconds(RECONNECT_START_TIMEOUT),
	reconnect_counter(0)
{
	pthread_rwlock_init(&pout_squeue_lock, 0);

	csock_list.insert(this);

	//reconnect_in_seconds = reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;
}



csocket::csocket(
		csocket_owner *owner,
		int sd) :
	socket_owner(owner),
	sd(sd),
	backlog(0),
	reconnect_start_timeout(RECONNECT_START_TIMEOUT),
	reconnect_in_seconds(RECONNECT_START_TIMEOUT),
	reconnect_counter(0)
{
	socklen_t optlen = 0;

	pthread_rwlock_init(&pout_squeue_lock, 0);

	sockflags.set(CONNECTED);

	if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
		logging::error << "[rofl][csocket] unable to read local address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

	if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
		logging::error << "[rofl][csocket] unable to read remote address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(domain);
	if ((getsockopt(sd, SOL_SOCKET, SO_DOMAIN, &domain, &optlen)) < 0) {
		logging::error << "[rofl][csocket] unable to read domain from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(type);
	if ((getsockopt(sd, SOL_SOCKET, SO_TYPE, &type, &optlen)) < 0) {
		logging::error << "[rofl][csocket] unable to read type from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(protocol);
	if ((getsockopt(sd, SOL_SOCKET, SO_PROTOCOL, &protocol, &optlen)) < 0) {
		logging::error << "[rofl][csocket] unable to read protocol from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	csock_list.insert(this);
	register_filedesc_r(sd);

	//reconnect_in_seconds = reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;
}



csocket::~csocket()
{
	cclose();

	pthread_rwlock_destroy(&pout_squeue_lock);

	csock_list.erase(this);
}



void
csocket::handle_timeout(
		int opaque)
{
	switch (opaque) {
	case TIMER_RECONNECT: cconnect(raddr, laddr, domain, type, protocol); break;
	default:
		logging::error << "[rofl][csocket] unknown timer type:" << opaque << std::endl;
	}
}



void
csocket::reconnect(bool reset_timeout)
{
	if (pending_timer(TIMER_RECONNECT)) {
		return;
	}

	logging::info << "[rofl][csocket] " << " scheduled reconnect in "
			<< (int)reconnect_in_seconds << " seconds." << std::endl << *this;

	int max_backoff = 16 * reconnect_start_timeout;

	if (reset_timeout) {
		reconnect_in_seconds = reconnect_start_timeout;
		reconnect_counter = 0;
	} else {
		reconnect_in_seconds *= 2;
	}


	if (reconnect_in_seconds > max_backoff) {
		reconnect_in_seconds = max_backoff;
	}

	reset_timer(TIMER_RECONNECT, reconnect_in_seconds);

	++reconnect_counter;
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
				throw eSysCall("accept");
			}
		}

		handle_accepted(new_sd, ra);

		// handle socket when in normal (=non-listening) state
	} else {

		// call method in derived class
		handle_read(fd);
	}
}



void
csocket::handle_wevent(int fd)
{
	if (sockflags[CONNECT_PENDING]) {
		int rc;
		int optval = 0;
		int optlen = sizeof(optval);
		if ((rc = getsockopt(sd, SOL_SOCKET, SO_ERROR,
							 (void*)&optval, (socklen_t*)&optlen)) < 0) {
			throw eSysCall("getsockopt(SOL_SOCKET, SO_ERROR)");
		}

		switch (optval) {
		case /*EISCONN=*/0: {
			logging::info << "[rofl][csocket] connection established." << std::endl << *this;

			sockflags[CONNECT_PENDING] = false;
			register_filedesc_w(sd);

			register_filedesc_r(sd);

			sockflags[CONNECTED] = true;

			if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
				logging::error << "[rofl][csocket] unable to read local address from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
			}

			if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
				logging::error << "[rofl][csocket] unable to read remote address from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
			}

			if (sockflags.test(FLAG_ACTIVE_SOCKET)) {
				cancel_timer(TIMER_RECONNECT);
			}

			handle_connected();
		} break;
		case EINPROGRESS: {
			logging::warn << "[rofl[csocket] connection establishment is pending." << std::endl << *this;
			// do nothing
		} break;
		case ECONNREFUSED: {
			logging::warn << "[rofl][csocket] connection failed." << std::endl << *this;
			cclose();
			handle_conn_refused();
			if (sockflags.test(FLAG_ACTIVE_SOCKET)) {
				reconnect(false);
			}
		} break;
		default: {
			logging::error << "[rofl][csocket] error occured during connection establishment." << std::endl << *this;
			throw eSocketError();
		};
		}
	} else {
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
	std::string devname)
{
	int rc;
	this->domain 	= domain;
	this->type 		= type;
	this->protocol 	= protocol;
	this->backlog 	= backlog;
	this->laddr 	= la;

	if (sd >= 0) {
		cclose();
	}

	// open socket
	if ((sd = socket(domain, type, protocol)) < 0) {
		throw eSysCall("socket");
	}



	// make socket non-blocking
	long flags;
	if ((flags = fcntl(sd, F_GETFL)) < 0) {
		throw eSysCall("fnctl(F_GETFL)");
	}
	flags |= O_NONBLOCK;
	if ((rc = fcntl(sd, F_SETFL, flags)) < 0) {
		throw eSysCall("fcntl(F_SETGL)");
	}


	if ((type == SOCK_STREAM) && (protocol == IPPROTO_TCP)) {
		int optval = 1;

		// set SO_REUSEADDR option on TCP sockets
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
		}

#if 0
		int on = 1;
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) < 0) {
			throw eSysCall("setsockopt(SOL_SOCKET, SO_REUSEPORT)");
		}
#endif

		// set TCP_NODELAY option on TCP sockets
		if ((rc = setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt(IPPROTO_TCP, TCP_NODELAY)");
		}

		// set SO_RCVLOWAT
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_RCVLOWAT, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt(SOL_SOCKET, SO_RCVLOWAT)");
		}

		// read TCP_NODELAY option for debugging purposes
		socklen_t optlen = sizeof(int);
		int optvalc;
		if ((rc = getsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optvalc, &optlen)) < 0) {
			throw eSysCall("getsockopt(IPPROTO_TCP, TCP_NODELAY)");
		}
	}
	else if ((type == SOCK_RAW) && (domain == PF_PACKET) && (not devname.empty()))
	{
		struct ifreq ifr;
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, devname.c_str(), sizeof(ifr.ifr_name));

		// read flags from interface
		if ((rc = ioctl(sd, SIOCGIFFLAGS, &ifr)) < 0) {
			throw eSysCall("ioctl(SIOCGIFFLAGS)");
		}

		// enable promiscuous flags
		ifr.ifr_flags |= IFF_PROMISC;
		if ((rc = ioctl(sd, SIOCSIFFLAGS, &ifr)) < 0) {
			throw eSysCall("ioctl(SIOCSIFFLAGS)");
		}

		// set SO_SNDBUF
		int optval = 1600;
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_SNDBUF, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt(SOL_SOCKET, SO_SNDBUF)");
		}
	}


	// bind to local address
	if ((rc = bind(sd, la.ca_saddr, (socklen_t)(la.salen))) < 0) {
		throw eSysCall("bind");
	}


	switch (type) {
	case SOCK_STREAM: {	// listen on socket
		if ((rc = listen(sd, backlog)) < 0) {
			throw eSysCall("listen");
		}
		sockflags[SOCKET_IS_LISTENING] = true;
	} break;
	case SOCK_DGRAM: { 	// do nothing
		sockflags.set(CONNECTED);
	} break;
	case SOCK_RAW: {  	// do nothing
		sockflags.set(RAW_SOCKET);
	} break;
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
	int protocol)
{
	int rc;
	this->domain 	= domain;
	this->type 		= type;
	this->protocol 	= protocol;
	this->laddr 	= la;
	this->raddr 	= ra;

	if (sd >= 0)
		cclose();

	sockflags.set(FLAG_ACTIVE_SOCKET);

	// open socket
	if ((sd = socket(domain, type, protocol)) < 0) {
		throw eSysCall("socket");
	}

	// make socket non-blockingSOCKET_IS_LISTENING
	long flags;
	if ((flags = fcntl(sd, F_GETFL)) < 0) {
		throw eSysCall("fnctl(F_GETFL");
	}
	flags |= O_NONBLOCK;
	if ((rc = fcntl(sd, F_SETFL, flags)) < 0) {
		throw eSysCall("fnctl(F_SETFL");
	}

	if ((type == SOCK_STREAM) && (protocol == IPPROTO_TCP)) {
		int optval = 1;

		// set SO_REUSEADDR option
		if ((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
		}

		// set TCP_NODELAY option
		if ((rc = setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval))) < 0) {
			throw eSysCall("setsockopt(IPPROTO_TCP, TCP_NODELAY)");
		}
	}

	// bind to local address
	if ((rc = bind(sd, la.ca_saddr, (socklen_t)(la.salen))) < 0) {
		throw eSysCall("bind");
	}

	// connect to remote address
	if ((rc = connect(sd, (const struct sockaddr*)ra.ca_saddr, (socklen_t)ra.salen)) < 0) {
		switch (errno) {
		case EINPROGRESS: {		// connect is pending, register sd for write events
			sockflags[CONNECT_PENDING] = true;
			register_filedesc_w(sd);
		} break;
		case ECONNREFUSED: {	// connect has been refused
			cclose();
			handle_conn_refused();
			reconnect(false);
		} break;
		default: {
			throw eSysCall("connect");
		};
		}
	} else {
		// connect was successful, register sd for read events
		register_filedesc_r(sd);
		sockflags.set(CONNECTED);

		if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
			throw eSysCall("getsockname");
		}

		if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
			throw eSysCall("getpeername");
		}

		handle_connected();
	}
}



void
csocket::cclose()
{
	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	int rc = 0;

	if (sd == -1)
		return;

	deregister_filedesc_r(sd);
	deregister_filedesc_w(sd);
	if (not sockflags[RAW_SOCKET]) {
		if ((rc = shutdown(sd, SHUT_RDWR)) < 0) {
			logging::error << "[rofl][csocket] error occured during shutdown():" << eSysCall("shutdown") << std::endl << *this;
		}
	}
	if ((rc = close(sd)) < 0) {
		logging::error << "[rofl][csocket] error occured during close():" << eSysCall("shutdown") << std::endl << *this;
	}

	sd = -1;
	// keep SOCKET_IS_LISTENING in case of failed connection and required reestablishing
	//sockflags.reset(SOCKET_IS_LISTENING);
	sockflags.reset(RAW_SOCKET);
	sockflags.reset(CONNECTED);

	// purge pout_squeue
	while (not pout_squeue.empty()) {
		pout_entry_t entry = pout_squeue.front();
		delete entry.mem;
		pout_squeue.pop_front();
	}
}



ssize_t
csocket::recv(void *buf, size_t count)
{
	// read from socket: TODO: TLS socket
	int rc = ::read(sd, (void*)buf, count);

	if ((rc == 0) && (sockflags.test(FLAG_ACTIVE_SOCKET))) {
		reconnect(true);
	}

	return rc;
}



void
csocket::send(cmemory* mem, caddress const& dest)
{
	if (not sockflags.test(CONNECTED) && not sockflags.test(RAW_SOCKET)) {
		logging::warn << "[rofl][csocket] socket not connected, dropping packet " << *mem << std::endl;
		delete mem; return;
	}

	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	pout_squeue.push_back(pout_entry_t(mem, dest));
	register_filedesc_w(sd);
}



void
csocket::dequeue_packet() throw (eSocketSendFailed, eSocketShortSend)
{
	{
		RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

		int rc = 0;

		while (not pout_squeue.empty()) {
			pout_entry_t entry = pout_squeue.front();

			int flags = MSG_NOSIGNAL;
			if ((rc = sendto(sd, entry.mem->somem(), entry.mem->memlen(), flags,
									entry.dest.ca_saddr, entry.dest.salen)) < 0) {
				switch (errno) {
				case EAGAIN:
					break;
				case EPIPE:

					goto out;
					return;
				case EMSGSIZE:
					logging::warn << "[rofl][csocket] dequeue_packet() dropping packet (EMSGSIZE) " << *(entry.mem) << std::endl;
					break;
				default:
					logging::warn << "[rofl][csocket] dequeue_packet() dropping packet " << *(entry.mem) << std::endl;
					throw eSysCall("sendto");
				}
			}
			else if ((rc < (int)entry.mem->memlen())) {
				throw eSysCall("sendto(short write)");
			}

			pout_squeue.pop_front();

			delete entry.mem;
		}

		if (pout_squeue.empty()) {
			deregister_filedesc_w(sd);
		}

		return;
	} // unlocks pout_squeue_lock
out:
	cclose(); // clears also pout_squeue
	handle_closed(sd);
}


