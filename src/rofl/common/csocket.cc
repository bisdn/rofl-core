/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "csocket.h"

#ifdef HAVE_OPENSSL
#include "ssl_connection.h"
#include "ssl_lib.h"
#endif /* HAVE_OPENSSL */

#include <assert.h>

using namespace rofl;

std::set<csocket*> csocket::csock_list;

csocket::csocket(
		csocket_owner *owner,
		int domain,
		int type,
		int protocol,
		int backlog) :
	had_short_write(false),
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
#ifdef HAVE_OPENSSL
	,ssl_ctx(NULL)
	,ssl_conn(NULL)
#endif
{
	pthread_rwlock_init(&pout_squeue_lock, 0);

	csock_list.insert(this);

	//reconnect_in_seconds = reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;
}



csocket::csocket(
		csocket_owner *owner) :
	had_short_write(false),
	socket_owner(owner),
	sd(-1),
	domain(-1),
	type(-1),
	protocol(-1),
	backlog(0),
	reconnect_start_timeout(RECONNECT_START_TIMEOUT),
	reconnect_in_seconds(RECONNECT_START_TIMEOUT),
	reconnect_counter(0)
#ifdef HAVE_OPENSSL
	,ssl_ctx(NULL)
	,ssl_conn(NULL)
#endif
{
	pthread_rwlock_init(&pout_squeue_lock, 0);

	csock_list.insert(this);

	//reconnect_in_seconds = reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;
}



csocket::~csocket()
{
	logging::debug << "[rofl][csocket] destructor:" << std::endl << *this;
	close();

	pthread_rwlock_destroy(&pout_squeue_lock);

	csock_list.erase(this);
}



void
csocket::handle_timeout(
		int opaque, void *data)
{
	switch (opaque) {
	case TIMER_RECONNECT: {
#ifdef HAVE_OPENSSL
		connect(raddr, laddr, domain, type, protocol, ssl_ctx, true);
#else
		connect(raddr, laddr, domain, type, protocol, NULL, true);
#endif
	} break;
	default:
		logging::error << "[rofl][csocket] unknown timer type:" << opaque << std::endl;
	}
}



void
csocket::handle_event(
		cevent const& ev)
{
	switch (ev.cmd) {
	case EVENT_CONN_RESET:
	case EVENT_DISCONNECTED: {
		close();

		if (sockflags.test(FLAG_DO_RECONNECT)) {
			sockflags.reset(CONNECTED);
			backoff_reconnect(true);
		} else {
			//logging::info << "[rofl][csocket] closed socket." << std::endl << *this;
			if (sockflags.test(FLAG_SEND_CLOSED_NOTIFICATION)) {
				//logging::info << "[rofl][csocket] sending CLOSED NOTIFICATION." << std::endl;
				sockflags.reset(FLAG_SEND_CLOSED_NOTIFICATION);
				events_clear();
				handle_closed();
			}
			return;
		}
	} break;
	default:
		;;
	}
}



void
csocket::backoff_reconnect(bool reset_timeout)
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

		if ((new_sd = ::accept(sd, (struct sockaddr*)(ra.ca_saddr), &(ra.salen))) < 0) {
			switch (errno) {
			case EAGAIN:
				// do nothing, just wait for the next event
				return;
			default:
				throw eSysCall("accept");
			}
		}

		logging::info << "[rofl][csocket] socket accepted " << new_sd << std::endl << *this;

		handle_accepted(new_sd, ra);

		// handle socket when in normal (=non-listening) state
	} else {

		if (sockflags.test(CONNECTED)) {
			handle_read(); // call method in derived class
		}
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

			if (sockflags.test(FLAG_DO_RECONNECT)) {
				cancel_timer(TIMER_RECONNECT);
			}

#ifdef HAVE_OPENSSL
			ssl_connect();
#endif
			handle_connected();
		} break;
		case EINPROGRESS: {
			logging::warn << "[rofl[csocket] connection establishment is pending." << std::endl << *this;
			// do nothing
		} break;
		case ECONNREFUSED: {
			logging::warn << "[rofl][csocket] connection failed." << std::endl << *this;
			close();

			if (sockflags.test(FLAG_DO_RECONNECT)) {
				backoff_reconnect(false);
			} else {
				handle_conn_refused();
			}
		} break;
		default: {
			logging::error << "[rofl][csocket] error occured during connection establishment." << std::endl << *this;
			throw eSocketError();
		};
		}
	} else {
		if (sockflags.test(CONNECTED)) {
			try {
				dequeue_packet();
			} catch (eSysCall& e) {
				logging::error << "[rofl][csocket] eSysCall " << e << std::endl;
			} catch (RoflException& e) {
				logging::error << "[rofl][csocket] RoflException " << e << std::endl;
			}
		}
	}
}


void
csocket::handle_xevent(int fd)
{
	logging::error << "[rofl[csocket] error occured on socket descriptor" << std::endl << *this;
}



void
csocket::listen(
	caddress la,
	int domain, 
	int type, 
	int protocol, 
	ssl_context *ssl_ctx,
	int backlog,
	std::string devname)
{
	int rc;
	this->domain 	= domain;
	this->type 		= type;
	this->protocol 	= protocol;
	this->backlog 	= backlog;
	this->laddr 	= la;
#ifdef HAVE_OPENSSL
	this->ssl_ctx	= ssl_ctx;
#endif

	if (sd >= 0) {
		close();
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
		if ((rc = ::listen(sd, backlog)) < 0) {
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
csocket::accept(int sd, ssl_context *ssl_ctx)
{
	this->sd = sd;
#ifdef HAVE_OPENSSL
	this->ssl_ctx = ssl_ctx;
	logging::info << __PRETTY_FUNCTION__ << ": " << ((NULL != ssl_ctx) ? "SSL" : "NO SSL") << std::endl;
#endif

	sockflags.reset(FLAG_ACTIVE_SOCKET);

	socklen_t optlen = 0;
	if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
		logging::error << "[rofl][csocket][accept] unable to read local address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

	if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
		logging::error << "[rofl][csocket][accept] unable to read remote address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(domain);
	if ((getsockopt(sd, SOL_SOCKET, SO_DOMAIN, &domain, &optlen)) < 0) {
		logging::error << "[rofl][csocket][accept] unable to read domain from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(type);
	if ((getsockopt(sd, SOL_SOCKET, SO_TYPE, &type, &optlen)) < 0) {
		logging::error << "[rofl][csocket][accept] unable to read type from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(protocol);
	if ((getsockopt(sd, SOL_SOCKET, SO_PROTOCOL, &protocol, &optlen)) < 0) {
		logging::error << "[rofl][csocket][accept] unable to read protocol from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

#ifdef HAVE_OPENSSL
	// setup tls if we have a context
	if (NULL != this->ssl_ctx) {
		ssl_conn = this->ssl_ctx->create_ssl_connection(sd);
		assert(NULL != ssl_conn);
		ssl_conn->handle_accept(); // todo handle rv
	}
#endif
	sockflags.set(CONNECTED);
	register_filedesc_r(sd);
	handle_connected();
}

void
csocket::connect(
	caddress ra,
	caddress la,
	int domain, 
	int type, 
	int protocol,
	ssl_context *ssl_ctx,
	bool do_reconnect)
{
	int rc;
	this->domain 	= domain;
	this->type 		= type;
	this->protocol 	= protocol;
	this->laddr 	= la;
	this->raddr 	= ra;
#ifdef HAVE_OPENSSL
	this->ssl_ctx = ssl_ctx;
#endif /* HAVE_OPENSSL */

	if (sd >= 0)
		close();

	sockflags.set(FLAG_ACTIVE_SOCKET);

	if (do_reconnect)
		sockflags.set(FLAG_DO_RECONNECT);
	else
		sockflags.reset(FLAG_DO_RECONNECT);

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
	if ((rc = ::connect(sd, (const struct sockaddr*)ra.ca_saddr, (socklen_t)ra.salen)) < 0) {
		switch (errno) {
		case EINPROGRESS: {		// connect is pending, register sd for write events
			sockflags[CONNECT_PENDING] = true;
			register_filedesc_w(sd);
			logging::debug << "[rofl][csocket] socket EINPROGRESS" << std::endl << *this;

		} break;
		case ECONNREFUSED: {	// connect has been refused
			close();
			backoff_reconnect(false);
			logging::debug << "[rofl][csocket] ECONNREFUSED" << std::endl << *this;

		} break;
		default: {
			throw eSysCall("connect");
		};
		}
	} else {
		// connect was successful, register sd for read events
		register_filedesc_r(sd);
		sockflags.set(CONNECTED);

		if (sockflags.test(FLAG_DO_RECONNECT)) {
			cancel_timer(TIMER_RECONNECT);
		}

		if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
			throw eSysCall("getsockname");
		}

		if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
			throw eSysCall("getpeername");
		}

		logging::info << "[rofl][csocket] socket connected" << std::endl << *this;

#ifdef HAVE_OPENSSL
		ssl_connect();
#endif

		handle_connected();
	}
}

#ifdef HAVE_OPENSSL
void
csocket::ssl_connect()
{
	// setup tls if we have a context
	if (NULL != ssl_ctx && NULL == ssl_conn) {
		ssl_conn = ssl_ctx->create_ssl_connection(sd);
		ssl_conn->handle_connect();
	}
}
#endif



void
csocket::reconnect()
{
	if (not sockflags.test(FLAG_ACTIVE_SOCKET)) {
		throw eSocketError();
	}
	close();
#ifdef HAVE_OPENSSL
	connect(raddr, laddr, domain, type, protocol, ssl_ctx, sockflags.test(FLAG_DO_RECONNECT));
#else
	connect(raddr, laddr, domain, type, protocol, NULL, sockflags.test(FLAG_DO_RECONNECT));
#endif
}



void
csocket::close()
{
	logging::info << "[rofl][csocket] close()" << std::endl;

	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	int rc = 0;

	if (sd == -1)
		return;

	deregister_filedesc_r(sd);
	deregister_filedesc_w(sd);
	if (not sockflags.test(RAW_SOCKET) and sockflags.test(CONNECTED)) {
		if ((rc = shutdown(sd, SHUT_RDWR)) < 0) {
			logging::error << "[rofl][csocket] error occured during shutdown(): " << eSysCall("shutdown") << std::endl << *this;
		}
	}
	if ((rc = ::close(sd)) < 0) {
		logging::error << "[rofl][csocket] error occured during close():" << eSysCall("close") << std::endl << *this;
	}

	sd = -1;
	// keep SOCKET_IS_LISTENING in case of failed connection and required reestablishing
	//sockflags.reset(SOCKET_IS_LISTENING);
	sockflags.reset(RAW_SOCKET);
	sockflags.reset(CONNECTED);
	sockflags.set(FLAG_SEND_CLOSED_NOTIFICATION);

	logging::info << "[rofl][csocket] cleaning-up socket." << std::endl << *this;

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
	if (sd == -1)
		throw eSocketReadFailed();
	int rc;

#ifdef HAVE_OPENSSL
	// read from tls connection
	if (NULL != ssl_conn) {

		{
			RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);
			if (pout_squeue.size()) {
				register_filedesc_w(sd);
			}
		}

		rc = ssl_conn->read((void*)buf, count);
		if (0 >= rc) {
			switch (rc) {
			case -SSL_ERROR_WANT_WRITE:
				register_filedesc_w(sd);
				// todo? deregister_filedesc_r(sd);
				throw eSocketAgain();
				break;
			case -SSL_ERROR_WANT_READ:
				register_filedesc_r(sd);
				throw eSocketAgain();
				break;
			case 0:
			default:
				break;
			}
		} else {
			logging::debug << __PRETTY_FUNCTION__ << " read " << rc << " bytes" << std::endl;
			return rc;
		}
	} else {
#endif

		// read from socket:
		rc = ::read(sd, (void*)buf, count);

		if (rc > 0) {
			return rc;

		} else if (rc == 0) {
			logging::error << "[rofl][csocket] peer closed connection: "
					<< eSysCall("read") << std::endl << *this;
			close();

			notify(cevent(EVENT_CONN_RESET));
			throw eSocketReadFailed();

		} else if (rc < 0) {

			switch(errno) {
			case EAGAIN:
				throw eSocketAgain();
			case ECONNRESET: {
				logging::error << "[rofl][csocket] connection reset on socket: "
						<< eSysCall("read") << ", closing endpoint." << std::endl << *this;
				close();

				notify(cevent(EVENT_CONN_RESET));
				throw eSocketReadFailed();
			} break;
			default: {
				logging::error << "[rofl][csocket] error reading from socket: "
						<< eSysCall("read") << ", closing endpoint." << std::endl << *this;
				close();

				notify(cevent(EVENT_DISCONNECTED));
				throw eSocketReadFailed();
			} break;
			}
		}

#ifdef HAVE_OPENSSL
	} /* else (of NULL != ssl_conn)  */
#endif

	return 0;
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
csocket::dequeue_packet()
{
	{
		RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

		int rc = 0;

		while (not pout_squeue.empty()) {
			pout_entry_t& entry = pout_squeue.front(); // reference, do not make a copy

			if (had_short_write) {
				logging::warn << "[rofl][csocket] resending due to short write: " << std::endl << entry;
				had_short_write = false;
			}

#ifdef HAVE_OPENSSL
			if (NULL != ssl_conn) {
				rc = ssl_conn->write(entry.mem->somem() + entry.msg_bytes_sent, entry.mem->memlen() - entry.msg_bytes_sent);

				if (0 >= rc) {
					switch (rc) {
					case -SSL_ERROR_WANT_WRITE:
						register_filedesc_w(sd);
						return;
						break;
					case -SSL_ERROR_WANT_READ:
						register_filedesc_r(sd);
						deregister_filedesc_w(sd);
						return;
						break;
					case 0:
					default:
						logging::error << "unhandled error " << rc << std::endl;
						assert(0);
						break;
					}
				} else {

					logging::debug << __PRETTY_FUNCTION__ << " wrote " << rc << " bytes" << std::endl;
					// check if buffer was sent completely
					if (((unsigned int)(rc + entry.msg_bytes_sent)) < entry.mem->memlen()) {
						had_short_write = true;
						entry.msg_bytes_sent += rc;
						return;
					}
				}

			} else {
#endif

			int flags = MSG_NOSIGNAL;
			if ((rc = sendto(sd, entry.mem->somem() + entry.msg_bytes_sent, entry.mem->memlen() - entry.msg_bytes_sent, flags,
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
			else if ((((unsigned int)(rc + entry.msg_bytes_sent)) < entry.mem->memlen())) {

				if (SOCK_STREAM == type) {
					had_short_write = true;
					entry.msg_bytes_sent += rc;
					logging::warn << "[rofl][csocket] short write on socket descriptor:" << sd << ", retrying..." << std::endl << entry;
				} else {
					logging::warn << "[rofl][csocket] short write on socket descriptor:" << sd << ", dropping packet." << std::endl;
					delete entry.mem;
					pout_squeue.pop_front();
				}
				return;
			}
#ifdef HAVE_OPENSSL
			} /* else (of NULL != ssl_conn)  */
#endif

			delete entry.mem;

			pout_squeue.pop_front();
		}

		if (pout_squeue.empty()) {
			deregister_filedesc_w(sd);
		}

		return;
	} // unlocks pout_squeue_lock
out:
	close(); // clears also pout_squeue
	handle_closed();
}


