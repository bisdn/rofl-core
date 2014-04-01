/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "csocket_openssl.h"

using namespace rofl;


std::string const 	csocket_openssl::SOCKET_PARAM_REMOTE_ADDR("remote-address");
std::string const 	csocket_openssl::SOCKET_PARAM_LOCAL_ADDR("local-address");
std::string const	csocket_openssl::SOCKET_PARAM_DOMAIN("domain");
std::string const	csocket_openssl::SOCKET_PARAM_TYPE("type");
std::string const	csocket_openssl::SOCKET_PARAM_CAPATH("capath");
std::string const	csocket_openssl::SOCKET_PARAM_CAFILE("cafile");
std::string const	csocket_openssl::SOCKET_PARAM_CERT("cert");
std::string const	csocket_openssl::SOCKET_PARAM_KEY("key");

/*static*/cparams
csocket_openssl::get_params()
{
	cparams p;
	p.add_param(csocket_openssl::SOCKET_PARAM_REMOTE_ADDR);
	p.add_param(csocket_openssl::SOCKET_PARAM_LOCAL_ADDR);
	p.add_param(csocket_openssl::SOCKET_PARAM_DOMAIN);
	p.add_param(csocket_openssl::SOCKET_PARAM_TYPE);
	p.add_param(csocket_openssl::SOCKET_PARAM_PROTOCOL);
	p.add_param(csocket_openssl::SOCKET_PARAM_CAPATH);
	p.add_param(csocket_openssl::SOCKET_PARAM_CAFILE);
	p.add_param(csocket_openssl::SOCKET_PARAM_CERT);
	p.add_param(csocket_openssl::SOCKET_PARAM_KEY);
	return p;
}

bool csocket_openssl::ssl_initialized = false;

void
csocket_openssl::ssl_init()
{
	if (ssl_initialized)
		return;

	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();

	ssl_initialized = true;
}


csocket_openssl::csocket_openssl(
		csocket_owner *owner) :
				csocket(rofl::csocket::SOCKET_TYPE_OPENSSL, owner),
				had_short_write(false),
				reconnect_start_timeout(RECONNECT_START_TIMEOUT),
				reconnect_in_seconds(RECONNECT_START_TIMEOUT),
				reconnect_counter(0),
				bio(NULL)
{
	csocket_openssl::ssl_init();

	pthread_rwlock_init(&pout_squeue_lock, 0);

	//reconnect_in_seconds = reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;
}



csocket_openssl::~csocket_openssl()
{
	logging::debug << "[rofl][csocket_openssl] destructor:" << std::endl << *this;
	close();

	pthread_rwlock_destroy(&pout_squeue_lock);
}



void
csocket_openssl::handle_timeout(
		int opaque, void *data)
{
	switch (opaque) {
	case TIMER_RECONNECT: {
		connect(raddr, laddr, domain, type, protocol, true);
	} break;
	default:
		logging::error << "[rofl][csocket_openssl] unknown timer type:" << opaque << std::endl;
	}
}



void
csocket_openssl::handle_event(
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
			//logging::info << "[rofl][csocket_openssl] closed socket." << std::endl << *this;
			if (sockflags.test(FLAG_SEND_CLOSED_NOTIFICATION)) {
				//logging::info << "[rofl][csocket_openssl] sending CLOSED NOTIFICATION." << std::endl;
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
csocket_openssl::backoff_reconnect(bool reset_timeout)
{
	if (pending_timer(TIMER_RECONNECT)) {
		return;
	}

	logging::info << "[rofl][csocket_openssl] " << " scheduled reconnect in "
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
csocket_openssl::handle_revent(int fd)
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

		logging::info << "[rofl][csocket_openssl] socket accepted " << new_sd << std::endl << *this;

		handle_accepted(new_sd, ra);

		// handle socket when in normal (=non-listening) state
	} else {

		if (sockflags.test(CONNECTED)) {
			handle_read(); // call method in derived class
		}
	}
}



void
csocket_openssl::handle_wevent(int fd)
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
			logging::info << "[rofl][csocket_openssl] connection established." << std::endl << *this;

			sockflags[CONNECT_PENDING] = false;
			register_filedesc_w(sd);

			register_filedesc_r(sd);

			sockflags[CONNECTED] = true;

			if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
				logging::error << "[rofl][csocket_openssl] unable to read local address from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
			}

			if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
				logging::error << "[rofl][csocket_openssl] unable to read remote address from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
			}

			if (sockflags.test(FLAG_DO_RECONNECT)) {
				cancel_timer(TIMER_RECONNECT);
			}

			handle_connected();
		} break;
		case EINPROGRESS: {
			logging::warn << "[rofl[csocket_openssl] connection establishment is pending." << std::endl << *this;
			// do nothing
		} break;
		case ECONNREFUSED: {
			logging::warn << "[rofl][csocket_openssl] connection failed." << std::endl << *this;
			close();

			if (sockflags.test(FLAG_DO_RECONNECT)) {
				backoff_reconnect(false);
			} else {
				handle_conn_refused();
			}
		} break;
		default: {
			logging::error << "[rofl][csocket_openssl] error occured during connection establishment." << std::endl << *this;
			throw eSysCall(optval);
		};
		}
	} else {
		if (sockflags.test(CONNECTED)) {
			try {
				dequeue_packet();
			} catch (eSysCall& e) {
				logging::error << "[rofl][csocket_openssl] eSysCall " << e << std::endl;
			} catch (RoflException& e) {
				logging::error << "[rofl][csocket_openssl] RoflException " << e << std::endl;
			}
		}
	}
}


void
csocket_openssl::handle_xevent(int fd)
{
	logging::error << "[rofl[csocket_openssl] error occured on socket descriptor" << std::endl << *this;
}



void
csocket_openssl::listen(
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
csocket_openssl::accept(int sd)
{
	this->sd = sd;

	sockflags.reset(FLAG_ACTIVE_SOCKET);

	socklen_t optlen = 0;
	if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
		logging::error << "[rofl][csocket_openssl][accept] unable to read local address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

	if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
		logging::error << "[rofl][csocket_openssl][accept] unable to read remote address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(domain);
	if ((getsockopt(sd, SOL_SOCKET, SO_DOMAIN, &domain, &optlen)) < 0) {
		logging::error << "[rofl][csocket_openssl][accept] unable to read domain from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(type);
	if ((getsockopt(sd, SOL_SOCKET, SO_TYPE, &type, &optlen)) < 0) {
		logging::error << "[rofl][csocket_openssl][accept] unable to read type from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(protocol);
	if ((getsockopt(sd, SOL_SOCKET, SO_PROTOCOL, &protocol, &optlen)) < 0) {
		logging::error << "[rofl][csocket_openssl][accept] unable to read protocol from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	sockflags.set(CONNECTED);
	register_filedesc_r(sd);
	handle_connected();
}

void
csocket_openssl::connect(
	caddress ra,
	caddress la,
	int domain, 
	int type, 
	int protocol,
	bool do_reconnect)
{
	this->domain 	= domain;
	this->type 		= type;
	this->protocol 	= protocol;
	this->laddr 	= la;
	this->raddr 	= ra;


	// check for open connection
	if (bio != NULL) {
		close();
	}

	// actively established connection
	sockflags.set(FLAG_ACTIVE_SOCKET);

	// reconnect after losing connection?
	if (do_reconnect)
		sockflags.set(FLAG_DO_RECONNECT);
	else
		sockflags.reset(FLAG_DO_RECONNECT);

	// create basic IO object == socket
	bio = BIO_new_connect((char*)ra.addr_c_str());
	if (NULL == bio) {
		throw eSysCall("BIO_new_connect()");
	}

	// make socket non-blocking (always returns 1)
	BIO_set_nbio(bio, 1);

	// establish connection
	if (BIO_do_connect(bio) <= 0) {
		if (BIO_should_retry(bio)) {
			sockflags.set(CONNECT_PENDING);
			BIO_get_fd(bio, (int*)&sd);
			register_filedesc_w(sd);
			logging::debug << "[rofl][csocket_openssl] BIO SHOULD-RETRY" << std::endl << *this;
		} else {
			close();
			backoff_reconnect(false);
			logging::debug << "[rofl][csocket_openssl] BIO CONNECT-FAILED" << std::endl << *this;
		}
		return;
	}

	BIO_get_fd(bio, (int*)&sd);
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

	logging::info << "[rofl][csocket_openssl] socket connected" << std::endl << *this;

	handle_connected();
}



void
csocket_openssl::reconnect()
{
	if (not sockflags.test(FLAG_ACTIVE_SOCKET)) {
		throw eInval();
	}
	close();
	connect(raddr, laddr, domain, type, protocol, sockflags.test(FLAG_DO_RECONNECT));
}



void
csocket_openssl::close()
{
	logging::info << "[rofl][csocket_openssl] close()" << std::endl;

	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	int rc = 0;

	if (sd == -1)
		return;

	deregister_filedesc_r(sd);
	deregister_filedesc_w(sd);

	if (not sockflags.test(RAW_SOCKET) and sockflags.test(CONNECTED)) {
		if ((rc = BIO_reset(bio)) < 0) {
			logging::error << "[rofl][csocket_openssl] error occured during BIO_reset(): "
					<< eSysCall("BIO_reset()") << std::endl << *this;
		}
	}

	sd = -1;
	// keep SOCKET_IS_LISTENING in case of failed connection and required reestablishing
	//sockflags.reset(SOCKET_IS_LISTENING);
	sockflags.reset(RAW_SOCKET);
	sockflags.reset(CONNECTED);
	sockflags.set(FLAG_SEND_CLOSED_NOTIFICATION);

	logging::info << "[rofl][csocket_openssl] cleaning-up socket." << std::endl << *this;

	// purge pout_squeue
	while (not pout_squeue.empty()) {
		pout_entry_t entry = pout_squeue.front();
		delete entry.mem;
		pout_squeue.pop_front();
	}
}

ssize_t
csocket_openssl::recv(void *buf, size_t count)
{
	if (sd == -1)
		throw eSocketNotConnected();
	int rc;

	// read from socket:
	rc = ::BIO_read(bio, (void*)buf, count);

	if (rc > 0) {
		return rc;

	} else if (rc == 0) {
		logging::error << "[rofl][csocket_openssl] peer closed connection: "
				<< eSysCall("BIO_read()") << std::endl << *this;
		close();

		notify(cevent(EVENT_CONN_RESET));
		throw eSysCall("BIO_read()");

	} else if (rc < 0) {

		if (BIO_should_retry(bio)) {
			throw eSocketAgain();
		}

		logging::error << "[rofl][csocket_openssl] error reading from socket: "
				<< eSysCall("BIO_read()") << ", closing endpoint." << std::endl << *this;
		close();

		notify(cevent(EVENT_DISCONNECTED));
		throw eSysCall("BIO_read()");
	}

	return 0;
}



void
csocket_openssl::send(cmemory* mem, caddress const& dest)
{
	if (not sockflags.test(CONNECTED) && not sockflags.test(RAW_SOCKET)) {
		logging::warn << "[rofl][csocket_openssl] socket not connected, dropping packet " << *mem << std::endl;
		delete mem; return;
	}

	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	pout_squeue.push_back(pout_entry_t(mem, dest));
	register_filedesc_w(sd);
}

void
csocket_openssl::dequeue_packet()
{
	{
		RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

		int rc = 0;

		while (not pout_squeue.empty()) {
			pout_entry_t& entry = pout_squeue.front(); // reference, do not make a copy

			if (had_short_write) {
				logging::warn << "[rofl][csocket_openssl] resending due to short write: " << std::endl << entry;
				had_short_write = false;
			}

			if ((rc = ::BIO_write(bio,
					entry.mem->somem() + entry.msg_bytes_sent,
					entry.mem->memlen() - entry.msg_bytes_sent)) <= 0) {

				if (not BIO_should_retry(bio)) {
					logging::warn << "[rofl][csocket_openssl] dequeue_packet() dropping packet " << *(entry.mem) << std::endl;

					//close(); // clears also pout_squeue
					//handle_closed();
					throw eSysCall("BIO_write()");
				}

			} else if ((((unsigned int)(rc + entry.msg_bytes_sent)) < entry.mem->memlen())) {

				if (SOCK_STREAM == type) {
					had_short_write = true;
					entry.msg_bytes_sent += rc;
					logging::warn << "[rofl][csocket_openssl] short write on socket descriptor:" << sd << ", retrying..." << std::endl << entry;
				} else {
					logging::warn << "[rofl][csocket_openssl] short write on socket descriptor:" << sd << ", dropping packet." << std::endl;
					delete entry.mem;
					pout_squeue.pop_front();
				}
				return;
			}

			delete entry.mem;

			pout_squeue.pop_front();
		}

		if (pout_squeue.empty()) {
			deregister_filedesc_w(sd);
		}

		return;
	} // unlocks pout_squeue_lock
}


