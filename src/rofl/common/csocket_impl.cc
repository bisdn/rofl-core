/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "csocket_impl.h"


using namespace rofl;


//Defaults
bool const 			csocket_impl::PARAM_DEFAULT_VALUE_DO_RECONNECT		= false;
std::string const 	csocket_impl::PARAM_DEFAULT_VALUE_REMOTE_HOSTNAME	= std::string("127.0.0.1");
std::string const 	csocket_impl::PARAM_DEFAULT_VALUE_REMOTE_PORT		= std::string("6653");
std::string const 	csocket_impl::PARAM_DEFAULT_VALUE_LOCAL_HOSTNAME;
std::string const 	csocket_impl::PARAM_DEFAULT_VALUE_LOCAL_PORT;
std::string const	csocket_impl::PARAM_DEFAULT_VALUE_DOMAIN			= csocket::PARAM_DOMAIN_VALUE_INET_ANY;
std::string const	csocket_impl::PARAM_DEFAULT_VALUE_TYPE				= csocket::PARAM_TYPE_VALUE_STREAM;
std::string const	csocket_impl::PARAM_DEFAULT_VALUE_PROTOCOL			= csocket::PARAM_PROTOCOL_VALUE_TCP;


/*static*/cparams
csocket_impl::get_default_params()
{
	/*
	 * fill in cparams structure and fill in default values
	 */

	cparams p;
	p.add_param(csocket::PARAM_KEY_DO_RECONNECT).set_bool(PARAM_DEFAULT_VALUE_DO_RECONNECT);
	p.add_param(csocket::PARAM_KEY_REMOTE_HOSTNAME).set_string(PARAM_DEFAULT_VALUE_REMOTE_HOSTNAME);
	p.add_param(csocket::PARAM_KEY_REMOTE_PORT).set_string(PARAM_DEFAULT_VALUE_REMOTE_PORT);
	p.add_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string(PARAM_DEFAULT_VALUE_LOCAL_HOSTNAME);
	p.add_param(csocket::PARAM_KEY_LOCAL_PORT).set_string(PARAM_DEFAULT_VALUE_LOCAL_PORT);
	p.add_param(csocket::PARAM_KEY_DOMAIN).set_string(PARAM_DEFAULT_VALUE_DOMAIN);
	p.add_param(csocket::PARAM_KEY_TYPE).set_string(PARAM_DEFAULT_VALUE_TYPE);
	p.add_param(csocket::PARAM_KEY_PROTOCOL).set_string(PARAM_DEFAULT_VALUE_PROTOCOL);
	return p;
}



csocket_impl::csocket_impl(
		csocket_owner *owner) :
				csocket(owner, rofl::csocket::SOCKET_TYPE_PLAIN),
				had_short_write(false),
				reconnect_start_timeout(RECONNECT_START_TIMEOUT),
				reconnect_in_seconds(RECONNECT_START_TIMEOUT),
				reconnect_counter(0)
{
	pthread_rwlock_init(&pout_squeue_lock, 0);

	//reconnect_in_seconds = reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;
}



csocket_impl::~csocket_impl()
{
	logging::debug << "[rofl][csocket][impl] destructor:" << std::endl << *this;
	close();

	pthread_rwlock_destroy(&pout_squeue_lock);
}



void
csocket_impl::handle_timeout(
		int opaque, void *data)
{
	switch (opaque) {
	case TIMER_RECONNECT: {
		connect(raddr, laddr, domain, type, protocol, true);
	} break;
	default:
		logging::error << "[rofl][csocket][impl] unknown timer type:" << opaque << std::endl;
	}
}



void
csocket_impl::handle_event(
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
			//logging::info << "[rofl][csocket][impl] closed socket." << std::endl << *this;
			if (sockflags.test(FLAG_SEND_CLOSED_NOTIFICATION)) {
				//logging::info << "[rofl][csocket][impl] sending CLOSED NOTIFICATION." << std::endl;
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
csocket_impl::backoff_reconnect(bool reset_timeout)
{
	if (pending_timer(TIMER_RECONNECT)) {
		return;
	}

	logging::info << "[rofl][csocket][impl] " << " scheduled reconnect in "
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
csocket_impl::handle_revent(int fd)
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

		logging::info << "[rofl][csocket][impl] socket accepted " << new_sd << std::endl << *this;

		handle_listen(new_sd);

		// handle socket when in normal (=non-listening) state
	} else {

		if (sockflags.test(CONNECTED)) {
			handle_read(); // call method in derived class
		}
	}
}



void
csocket_impl::handle_wevent(int fd)
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

			sockflags[CONNECT_PENDING] = false;
			register_filedesc_w(sd);

			register_filedesc_r(sd);

			sockflags[CONNECTED] = true;

			if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
				logging::error << "[rofl][csocket][impl] unable to read local address from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
			}

			if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
				logging::error << "[rofl][csocket][impl] unable to read remote address from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
			}

			if (sockflags.test(FLAG_DO_RECONNECT)) {
				cancel_timer(TIMER_RECONNECT);
			}

			logging::info << "[rofl][csocket][impl] connection established." << std::endl << *this;

			handle_connected();
		} break;
		case EINPROGRESS: {
			logging::warn << "[rofl[csocket][impl] connection establishment is pending." << std::endl << *this;
			// do nothing
		} break;
		case ECONNREFUSED: {
			logging::warn << "[rofl][csocket][impl] connection failed." << std::endl << *this;
			close();

			if (sockflags.test(FLAG_DO_RECONNECT)) {
				backoff_reconnect(false);
			} else {
				handle_conn_refused();
			}
		} break;
		default: {
			logging::error << "[rofl][csocket][impl] error occured during connection establishment." << std::endl << *this;
			throw eSysCall(optval);
		};
		}
	} else {
		if (sockflags.test(CONNECTED)) {
			try {
				handle_write();

				dequeue_packet();
			} catch (eSysCall& e) {
				logging::error << "[rofl][csocket][impl] eSysCall " << e << std::endl;
			} catch (RoflException& e) {
				logging::error << "[rofl][csocket][impl] RoflException " << e << std::endl;
			}
		}
	}
}


void
csocket_impl::handle_xevent(int fd)
{
	logging::error << "[rofl[csocket][impl] error occured on socket descriptor" << std::endl << *this;
}



void
csocket_impl::listen(
		cparams const& params)
{
	this->socket_params = params;

	rofl::logging::debug << "[rofl][csocket][impl][listen] parameter set:" << std::endl << params;

	/*
	 * local address and domain
	 */
	int domain = 0, sa_family = 0;

	if (params.get_param(csocket::PARAM_KEY_DOMAIN).get_string() == csocket::PARAM_DOMAIN_VALUE_INET) {
		domain = sa_family = PF_INET;
	} else
	if (params.get_param(csocket::PARAM_KEY_DOMAIN).get_string() == csocket::PARAM_DOMAIN_VALUE_INET6) {
		domain = sa_family = PF_INET6;
	} else
	if (params.get_param(csocket::PARAM_KEY_DOMAIN).get_string() == csocket::PARAM_DOMAIN_VALUE_INET_ANY) {
		domain = sa_family = 0;
	}

	/*
	 * type
	 */
	int type = 0;

	if (params.get_param(csocket::PARAM_KEY_TYPE).get_string() == csocket::PARAM_TYPE_VALUE_STREAM) {
		type = SOCK_STREAM;
	} else
	if (params.get_param(csocket::PARAM_KEY_TYPE).get_string() == csocket::PARAM_TYPE_VALUE_DGRAM) {
		type = SOCK_DGRAM;
	}

	/*
	 * protocol
	 */
	int protocol = 0;

	if (params.get_param(csocket::PARAM_KEY_PROTOCOL).get_string() == csocket::PARAM_PROTOCOL_VALUE_TCP) {
		protocol = IPPROTO_TCP;
	} else
	if (params.get_param(csocket::PARAM_KEY_PROTOCOL).get_string() == csocket::PARAM_PROTOCOL_VALUE_UDP) {
		protocol = IPPROTO_UDP;
	}




	std::string binding_addr;

	if (not params.get_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string().empty()) {
		binding_addr = params.get_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string();
	} else {
		switch (raddr.get_domain()) {
		case PF_INET: {
			binding_addr = std::string("0.0.0.0");
		} break;
		case PF_INET6: {
			binding_addr = std::string("0000:0000:0000:0000:0000:0000:0000:0000");
		} break;
		}
	}

	std::string binding_port;

	if (not params.get_param(csocket::PARAM_KEY_LOCAL_PORT).get_string().empty()) {
		binding_port = params.get_param(csocket::PARAM_KEY_LOCAL_PORT).get_string();
	} else {
		binding_port = std::string("0");
	}

	caddress laddr(
				binding_addr,
				binding_port,
				/*flags=*/0,
				/*preferred-family=*/domain,
				/*preferred-socktype=*/type,
				/*preferred-protocol=*/protocol);

	listen(laddr, laddr.get_domain(), laddr.get_sock_type(), laddr.get_protocol());
}



void
csocket_impl::listen(
	caddress la,
	int domain, 
	int type, 
	int protocol, 
	int backlog,
	std::string devname)
{
	rofl::logging::debug << "[rofl][csocket][impl][listen] laddr:" << la << " domain:" << domain << std::endl;

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
csocket_impl::accept(cparams const& socket_params, int sd)
{
	this->socket_params = socket_params;

	this->sd = sd;

	sockflags.reset(FLAG_ACTIVE_SOCKET);


	// make socket non-blocking, as this status is not inherited
	long flags;
	if ((flags = fcntl(sd, F_GETFL)) < 0) {
		throw eSysCall("fnctl(F_GETFL)");
	}
	flags |= O_NONBLOCK;
	if ((fcntl(sd, F_SETFL, flags)) < 0) {
		throw eSysCall("fcntl(F_SETGL)");
	}

	socklen_t optlen = 0;
	if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
		logging::error << "[rofl][csocket][impl][accept] unable to read local address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

	if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
		logging::error << "[rofl][csocket][impl][accept] unable to read remote address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(domain);
	if ((getsockopt(sd, SOL_SOCKET, SO_DOMAIN, &domain, &optlen)) < 0) {
		logging::error << "[rofl][csocket][impl][accept] unable to read domain from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(type);
	if ((getsockopt(sd, SOL_SOCKET, SO_TYPE, &type, &optlen)) < 0) {
		logging::error << "[rofl][csocket][impl][accept] unable to read type from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	optlen = sizeof(protocol);
	if ((getsockopt(sd, SOL_SOCKET, SO_PROTOCOL, &protocol, &optlen)) < 0) {
		logging::error << "[rofl][csocket][impl][accept] unable to read protocol from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

	sockflags.set(CONNECTED);
	register_filedesc_r(sd);
	handle_accepted();
}



void
csocket_impl::connect(
		cparams const& params)
{
	this->socket_params = params;

	rofl::logging::debug << "[rofl][csocket][impl][connect] parameter set:" << std::endl << params;

	/*
	 * local, remote address and domain
	 */
	int domain = 0, sa_family = 0;

	if (params.get_param(csocket::PARAM_KEY_DOMAIN).get_string() == csocket::PARAM_DOMAIN_VALUE_INET) {
		domain = sa_family = PF_INET;
	} else
	if (params.get_param(csocket::PARAM_KEY_DOMAIN).get_string() == csocket::PARAM_DOMAIN_VALUE_INET6) {
		domain = sa_family = PF_INET6;
	} else
	if (params.get_param(csocket::PARAM_KEY_DOMAIN).get_string() == csocket::PARAM_DOMAIN_VALUE_INET_ANY) {
		domain = sa_family = 0;
	}

	/*
	 * type
	 */
	int type = 0;

	if (params.get_param(csocket::PARAM_KEY_TYPE).get_string() == csocket::PARAM_TYPE_VALUE_STREAM) {
		type = SOCK_STREAM;
	} else
	if (params.get_param(csocket::PARAM_KEY_TYPE).get_string() == csocket::PARAM_TYPE_VALUE_DGRAM) {
		type = SOCK_DGRAM;
	}

	/*
	 * protocol
	 */
	int protocol = 0;

	if (params.get_param(csocket::PARAM_KEY_PROTOCOL).get_string() == csocket::PARAM_PROTOCOL_VALUE_TCP) {
		protocol = IPPROTO_TCP;
	} else
	if (params.get_param(csocket::PARAM_KEY_PROTOCOL).get_string() == csocket::PARAM_PROTOCOL_VALUE_UDP) {
		protocol = IPPROTO_UDP;
	}


	caddress raddr(
			params.get_param(csocket::PARAM_KEY_REMOTE_HOSTNAME).get_string(),
			params.get_param(csocket::PARAM_KEY_REMOTE_PORT).get_string(),
			/*flags=*/0,
			/*preferred-family=*/domain,
			/*preferred-socktype=*/type,
			/*preferred-protocol=*/protocol);


	std::string binding_addr;

	if (not params.get_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string().empty()) {
		binding_addr = params.get_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string();
	} else {
		switch (raddr.get_domain()) {
		case PF_INET: {
			binding_addr = std::string("0.0.0.0");
		} break;
		case PF_INET6: {
			binding_addr = std::string("0000:0000:0000:0000:0000:0000:0000:0000");
		} break;
		}
	}

	std::string binding_port;

	if (not params.get_param(csocket::PARAM_KEY_LOCAL_PORT).get_string().empty()) {
		binding_port = params.get_param(csocket::PARAM_KEY_LOCAL_PORT).get_string();
	} else {
		binding_port = std::string("0");
	}

	caddress laddr(
				binding_addr,
				binding_port,
				/*flags=*/0,
				/*preferred-family=*/domain,
				/*preferred-socktype=*/type,
				/*preferred-protocol=*/protocol);


	bool do_reconnect = params.get_param(csocket::PARAM_KEY_DO_RECONNECT).get_bool();

	connect(raddr, laddr, raddr.get_domain(), raddr.get_sock_type(), raddr.get_protocol(), do_reconnect);
}



void
csocket_impl::connect(
	caddress ra,
	caddress la,
	int domain, 
	int type, 
	int protocol,
	bool do_reconnect)
{
	rofl::logging::debug << "[rofl][csocket][impl][connect] raddr:" << ra << " laddr:" << la << " domain:" << domain << std::endl;

	int rc;
	this->domain 	= domain;
	this->type 		= type;
	this->protocol 	= protocol;
	this->laddr 	= la;
	this->raddr 	= ra;

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
		throw eSysCall("fnctl(F_GETFL)");
	}
	flags |= O_NONBLOCK;
	if ((rc = fcntl(sd, F_SETFL, flags)) < 0) {
		throw eSysCall("fnctl(F_SETFL)");
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
			logging::debug << "[rofl][csocket][impl] socket EINPROGRESS" << std::endl << *this;

		} break;
		case ECONNREFUSED: {	// connect has been refused
			close();
			backoff_reconnect(false);
			logging::debug << "[rofl][csocket][impl] ECONNREFUSED" << std::endl << *this;

		} break;
		default: {
			logging::debug << "[rofl][csocket][impl] Unknown error:"<< strerror(errno) <<"("<< errno <<")"<< std::endl << *this;
			throw eSysCall("connect ");
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

		logging::info << "[rofl][csocket][impl] socket connected" << std::endl << *this;

		handle_connected();
	}
}



void
csocket_impl::reconnect()
{
	if (not sockflags.test(FLAG_ACTIVE_SOCKET)) {
		throw eInval();
	}
	close();
	connect(raddr, laddr, domain, type, protocol, sockflags.test(FLAG_DO_RECONNECT));
}



void
csocket_impl::close()
{
	logging::info << "[rofl][csocket][impl] close()" << std::endl;

	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	int rc = 0;

	if (sd == -1)
		return;

	deregister_filedesc_r(sd);
	deregister_filedesc_w(sd);
	if (not sockflags.test(RAW_SOCKET) and sockflags.test(CONNECTED)) {
		if ((rc = shutdown(sd, SHUT_RDWR)) < 0) {
			logging::error << "[rofl][csocket][impl] error occured during shutdown(): " << eSysCall("shutdown") << std::endl << *this;
		}
	}
	if ((rc = ::close(sd)) < 0) {
		logging::error << "[rofl][csocket][impl] error occured during close():" << eSysCall("close") << std::endl << *this;
	}

	sd = -1;
	// keep SOCKET_IS_LISTENING in case of failed connection and required reestablishing
	//sockflags.reset(SOCKET_IS_LISTENING);
	sockflags.reset(RAW_SOCKET);
	sockflags.reset(CONNECTED);
	sockflags.set(FLAG_SEND_CLOSED_NOTIFICATION);

	logging::info << "[rofl][csocket][impl] cleaning-up socket." << std::endl << *this;

	// purge pout_squeue
	while (not pout_squeue.empty()) {
		pout_entry_t entry = pout_squeue.front();
		delete entry.mem;
		pout_squeue.pop_front();
	}
}

ssize_t
csocket_impl::recv(void *buf, size_t count)
{
	if (sd == -1)
		throw eSocketNotConnected();
	int rc;


		// read from socket:
		rc = ::read(sd, (void*)buf, count);

		if (rc > 0) {
			return rc;

		} else if (rc == 0) {
			logging::error << "[rofl][csocket][impl] peer closed connection: "
					<< eSysCall("read") << std::endl << *this;
			close();

			notify(cevent(EVENT_CONN_RESET));
			throw eSysCall("read()");

		} else if (rc < 0) {

			switch(errno) {
			case EAGAIN:
				throw eSocketAgain();
			case ECONNRESET: {
				logging::error << "[rofl][csocket][impl] connection reset on socket: "
						<< eSysCall("read") << ", closing endpoint." << std::endl << *this;
				close();

				notify(cevent(EVENT_CONN_RESET));
				throw eSysCall("read()");
			} break;
			default: {
				logging::error << "[rofl][csocket][impl] error reading from socket: "
						<< eSysCall("read") << ", closing endpoint." << std::endl << *this;
				close();

				notify(cevent(EVENT_DISCONNECTED));
				throw eSysCall("read()");
			} break;
			}
		}


	return 0;
}



void
csocket_impl::send(cmemory* mem, caddress const& dest)
{
	if (not sockflags.test(CONNECTED) && not sockflags.test(RAW_SOCKET)) {
		logging::warn << "[rofl][csocket][impl] socket not connected, dropping packet " << *mem << std::endl;
		delete mem; return;
	}

	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	pout_squeue.push_back(pout_entry_t(mem, dest));
	register_filedesc_w(sd);
}

void
csocket_impl::dequeue_packet()
{
	{
		RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

		int rc = 0;

		while (not pout_squeue.empty()) {
			pout_entry_t& entry = pout_squeue.front(); // reference, do not make a copy

			if (had_short_write) {
				logging::warn << "[rofl][csocket][impl] resending due to short write: " << std::endl << entry;
				had_short_write = false;
			}


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
					logging::warn << "[rofl][csocket][impl] dequeue_packet() dropping packet (EMSGSIZE) " << *(entry.mem) << std::endl;
					break;
				default:
					logging::warn << "[rofl][csocket][impl] dequeue_packet() dropping packet " << *(entry.mem) << std::endl;
					throw eSysCall("sendto");
				}
			}
			else if ((((unsigned int)(rc + entry.msg_bytes_sent)) < entry.mem->memlen())) {

				if (SOCK_STREAM == type) {
					had_short_write = true;
					entry.msg_bytes_sent += rc;
					logging::warn << "[rofl][csocket][impl] short write on socket descriptor:" << sd << ", retrying..." << std::endl << entry;
				} else {
					logging::warn << "[rofl][csocket][impl] short write on socket descriptor:" << sd << ", dropping packet." << std::endl;
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
out:
	close(); // clears also pout_squeue
	handle_closed();
}


