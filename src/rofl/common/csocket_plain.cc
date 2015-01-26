/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "csocket_plain.h"
#include "csocket_strings.h"


using namespace rofl;

//Defaults
bool const 			csocket_plain::PARAM_DEFAULT_VALUE_DO_RECONNECT		= false;
std::string const 	csocket_plain::PARAM_DEFAULT_VALUE_REMOTE_HOSTNAME(std::string("127.0.0.1"));
std::string const 	csocket_plain::PARAM_DEFAULT_VALUE_REMOTE_PORT(std::string("6653"));
std::string const 	csocket_plain::PARAM_DEFAULT_VALUE_LOCAL_HOSTNAME;
std::string const 	csocket_plain::PARAM_DEFAULT_VALUE_LOCAL_PORT;
std::string const	csocket_plain::PARAM_DEFAULT_VALUE_DOMAIN(__PARAM_DOMAIN_VALUE_INET_ANY);
std::string const	csocket_plain::PARAM_DEFAULT_VALUE_TYPE(__PARAM_TYPE_VALUE_STREAM);
std::string const	csocket_plain::PARAM_DEFAULT_VALUE_PROTOCOL(__PARAM_PROTOCOL_VALUE_TCP);

unsigned int const csocket_plain::DEFAULT_MAX_TXQUEUE_SIZE = 16;


/*static*/cparams
csocket_plain::get_default_params()
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



csocket_plain::csocket_plain(
		csocket_env *owner, pthread_t tid) :
				csocket(owner, rofl::csocket::SOCKET_TYPE_PLAIN, tid),
				had_short_write(false),
				max_txqueue_size(DEFAULT_MAX_TXQUEUE_SIZE),
				reconnect_start_timeout(RECONNECT_START_TIMEOUT),
				reconnect_in_seconds(RECONNECT_START_TIMEOUT),
				reconnect_counter(0)
{
	pthread_rwlock_init(&pout_squeue_lock, 0);

	//reconnect_in_seconds = reconnect_start_timeout = (reconnect_start_timeout == 0) ? 1 : reconnect_start_timeout;
	//rofl::logging::debug3 << "[rofl-common][csocket][plain] constructor " << std::hex << this << std::dec << std::endl;

	rofl::logging::debug2 << "[rofl-common][csocket][plain] "
			<< "constructor " << std::hex << this << std::dec
			<< ", parameter tid: " << std::hex << tid << std::dec
			<< ", target tid: " << std::hex << get_thread_id() << std::dec
			<< ", running tid: " << std::hex << pthread_self() << std::dec
			<< std::endl;
}



csocket_plain::~csocket_plain()
{
	rofl::logging::debug2 << "[rofl-common][csocket][plain] "
			<< "destructor " << std::hex << this << std::dec
			<< ", target tid: " << std::hex << get_thread_id() << std::dec
			<< ", running tid: " << std::hex << pthread_self() << std::dec
			<< std::endl;

	//rofl::logging::debug3 << "[rofl-common][csocket][plain] destructor " << std::hex << this << std::dec << std::endl;
	socket_env = NULL;

	close();

	pthread_rwlock_destroy(&pout_squeue_lock);
}



void
csocket_plain::handle_timeout(
		int opaque, void *data)
{
	switch (opaque) {
	case TIMER_RECONNECT: {
		connect(raddr, laddr, domain, type, protocol, true);
	} break;
	default:
		rofl::logging::error << "[rofl-common][csocket][plain] unknown timer type:" << opaque << std::endl;
	}
}



void
csocket_plain::handle_event(
		cevent const& ev)
{
	switch (ev.cmd) {
	case EVENT_CONN_RESET:
	case EVENT_DISCONNECTED: {
		close();

		if (sockflags.test(FLAG_DO_RECONNECT)) {
			sockflags.reset(FLAG_CONNECTED);
			backoff_reconnect(true);
		} else {
			//rofl::logging::info << "[rofl-common][csocket][plain] closed socket." << std::endl << *this;
			if (sockflags.test(FLAG_CLOSING)) {
				//rofl::logging::info << "[rofl-common][csocket][plain] sending CLOSED NOTIFICATION." << std::endl;
				sockflags.reset(FLAG_CLOSING);
				cancel_all_events();
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
csocket_plain::backoff_reconnect(bool reset_timeout)
{
	if (pending_timer(reconnect_timerid)) {
		return;
	}

	rofl::logging::info << "[rofl-common][csocket][plain] " << " scheduled reconnect in "
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

	reconnect_timerid = reset_timer(reconnect_timerid, reconnect_in_seconds);

	++reconnect_counter;
}



void
csocket_plain::handle_revent(int fd)
{
	// handle socket when in listening state
	if (sockflags[FLAG_LISTENING]) {
		int new_sd;
		csockaddr ra;

		if ((new_sd = ::accept(sd, (struct sockaddr*)(ra.somem()), &(ra.salen))) < 0) {
			switch (errno) {
			case EAGAIN:
				// do nothing, just wait for the next event
				return;
			default:
				throw eSysCall("accept");
			}
		}

		rofl::logging::info << "[rofl-common][csocket][plain] socket accepted " << str() << std::endl;

		handle_listen(new_sd);

		// handle socket when in normal (=non-listening) state
	} else {

		if (sockflags.test(FLAG_CONNECTED)) {
			handle_read(); // call method in derived class
		}
	}
}



void
csocket_plain::handle_wevent(int fd)
{
	if (sockflags[FLAG_CONNECTING]) {
		int rc;
		int optval = 0;
		int optlen = sizeof(optval);
		if ((rc = getsockopt(sd, SOL_SOCKET, SO_ERROR,
							 (void*)&optval, (socklen_t*)&optlen)) < 0) {
			throw eSysCall("getsockopt(SOL_SOCKET, SO_ERROR)");
		}

		switch (optval) {
		case /*EISCONN=*/0: {

			sockflags[FLAG_CONNECTING] = false;
			register_filedesc_w(sd);

			register_filedesc_r(sd);

			sockflags[FLAG_CONNECTED] = true;

			if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
				rofl::logging::error << "[rofl-common][csocket][plain] unable to read local address from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
			}

			if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
				rofl::logging::error << "[rofl-common][csocket][plain] unable to read remote address from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
			}

			if (sockflags.test(FLAG_DO_RECONNECT)) {
				cancel_timer(reconnect_timerid);
			}

			rofl::logging::info << "[rofl-common][csocket][plain][connect] connection established. " << str() << std::endl;

			handle_connected();
		} break;
		case EINPROGRESS: {
			rofl::logging::warn << "[rofl[csocket][plain][connect] connection establishment is pending. " << str() << std::endl;
			// do nothing
		} break;
		case ECONNREFUSED: {
			rofl::logging::warn << "[rofl-common][csocket][plain][connect] connection failed. " << str() << std::endl;
			close();

			if (sockflags.test(FLAG_DO_RECONNECT)) {
				backoff_reconnect(false);
			} else {
				handle_conn_refused();
			}
		} break;
		default: {
			rofl::logging::error << "[rofl-common][csocket][plain][connect] error occured during connection establishment. " << str() << std::endl;
			//throw eSysCall(optval);

			close();

			if (sockflags.test(FLAG_DO_RECONNECT)) {
				backoff_reconnect(false);
			} else {
				handle_conn_failed();
			}
		};
		}
	} else {
		if (sockflags.test(FLAG_CONNECTED)) {
			try {
				dequeue_packet();

				handle_write();
			} catch (eSysCall& e) {
				rofl::logging::error << "[rofl-common][csocket][plain] eSysCall " << e << std::endl;
			} catch (RoflException& e) {
				rofl::logging::error << "[rofl-common][csocket][plain] RoflException " << e << std::endl;
			}
		}
	}
}


void
csocket_plain::handle_xevent(int fd)
{
	rofl::logging::error << "[rofl[csocket][plain] error occured on socket descriptor" << str() << std::endl;
}



void
csocket_plain::listen(
		cparams const& params)
{
	this->socket_params = params;

	rofl::logging::debug3 << "[rofl-common][csocket][plain][listen] parameter set:" << std::endl << params;

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
		domain = sa_family = PF_INET6;
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

		caddrinfos addrinfos;
		addrinfos.set_ai_hints().set_ai_family(domain);
		addrinfos.set_node(params.get_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string());
		addrinfos.resolve();

		if (addrinfos.size() == 0) {
			throw eInval("csocket_plain::listen() unable to resolve hostname");
		}

		// we take simply the first result returned
		switch (domain) {
		case PF_INET: {
			caddress_in4 addr;
			addr.set_addr_nbo(addrinfos.get_addr_info(0).get_ai_addr().ca_s4addr->sin_addr.s_addr);
			binding_addr = addr.str();
		} break;
		case PF_INET6: {
			caddress_in6 addr;
			addr.unpack(addrinfos.get_addr_info(0).get_ai_addr().ca_s6addr->sin6_addr.s6_addr, 16);
			binding_addr = addr.str();
		} break;
		default: {
			// use the first entry and its domain
			switch (addrinfos.get_addr_info(0).get_ai_family()) {
			case PF_INET: {
				domain = PF_INET;
				caddress_in4 addr;
				addr.set_addr_nbo(addrinfos.get_addr_info(0).get_ai_addr().ca_s4addr->sin_addr.s_addr);
				binding_addr = addr.str();
			} break;
			case PF_INET6: {
				domain = PF_INET6;
				caddress_in6 addr;
				addr.unpack(addrinfos.get_addr_info(0).get_ai_addr().ca_s6addr->sin6_addr.s6_addr, 16);
				binding_addr = addr.str();
			} break;
			default:
				throw eInval("csocket_plain::connect() unable to resolve remote hostname");
			}
		};
		}

		//binding_addr = params.get_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string();

	} else {
		switch (domain) {
		case PF_INET: {
			binding_addr = std::string("0.0.0.0");
		} break;
		case PF_INET6: {
			binding_addr = std::string("0000:0000:0000:0000:0000:0000:0000:0000");
			//binding_addr = std::string("::FFFF:0000:0000");
		} break;
		}
	}

	uint16_t binding_port;

	if (not params.get_param(csocket::PARAM_KEY_LOCAL_PORT).get_string().empty()) {
		binding_port = atoi(params.get_param(csocket::PARAM_KEY_LOCAL_PORT).get_string().c_str());
	} else {
		binding_port = 0;
	}

	csockaddr laddr(domain, binding_addr, binding_port);

	listen(laddr, domain, type, protocol);
}



void
csocket_plain::listen(
	const csockaddr& la,
	int domain, 
	int type, 
	int protocol, 
	int backlog,
	std::string devname)
{
	rofl::logging::info << "[rofl-common][csocket][plain][listen] " << str() << std::endl;

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
		sockflags[FLAG_LISTENING] = true;
	} break;
	case SOCK_DGRAM: { 	// do nothing
		sockflags.set(FLAG_CONNECTED);
	} break;
	case SOCK_RAW: {  	// do nothing
		sockflags.set(FLAG_RAW_SOCKET);
	} break;
	}

	// setup was successful, register sd for read events
	register_filedesc_r(sd);
}


void
csocket_plain::accept(cparams const& socket_params, int sd)
{
	this->socket_params = socket_params;

	this->sd = sd;

	ciosrv::cancel_all_timers();
	ciosrv::cancel_all_events();

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
		rofl::logging::error << "[rofl-common][csocket][plain][accept] unable to read local address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

	if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
		rofl::logging::error << "[rofl-common][csocket][plain][accept] unable to read remote address from socket descriptor:"
				<< sd << " " << eSysCall() << std::endl;
	}

//Some kernels do not define this option
#ifdef SO_PROTOCOL
	optlen = sizeof(domain);
	if ((getsockopt(sd, SOL_SOCKET, SO_DOMAIN, &domain, &optlen)) < 0) {
		rofl::logging::error << "[rofl-common][csocket][plain][accept] unable to read domain from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}
#endif

	optlen = sizeof(type);
	if ((getsockopt(sd, SOL_SOCKET, SO_TYPE, &type, &optlen)) < 0) {
		rofl::logging::error << "[rofl-common][csocket][plain][accept] unable to read type from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}

//Some kernels do not define this option
#ifdef SO_PROTOCOL
	optlen = sizeof(protocol);
	if ((getsockopt(sd, SOL_SOCKET, SO_PROTOCOL, &protocol, &optlen)) < 0) {
		rofl::logging::error << "[rofl-common][csocket][plain][accept] unable to read protocol from socket descriptor:"
						<< sd << " " << eSysCall() << std::endl;
	}
#endif

	sockflags.set(FLAG_CONNECTED);
	register_filedesc_r(sd);
	handle_accepted();
}



void
csocket_plain::connect(
		cparams const& params)
{
	try {
		this->socket_params = params;

		rofl::logging::debug3 << "[rofl-common][csocket][plain][connect] parameter set:" << std::endl << params;

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



		std::string remote_addr;

		if (not params.get_param(csocket::PARAM_KEY_REMOTE_HOSTNAME).get_string().empty()) {

			caddrinfos addrinfos;
			addrinfos.set_ai_hints().set_ai_family(domain);
			addrinfos.set_node(params.get_param(csocket::PARAM_KEY_REMOTE_HOSTNAME).get_string());
			addrinfos.resolve();

			if (addrinfos.size() == 0) {
				throw eInval("csocket_plain::connect() unable to resolve hostname");
			}

			// we take simply the first result returned
			switch (domain) {
			case PF_INET: {
				caddress_in4 addr;
				addr.set_addr_nbo(addrinfos.get_addr_info(0).get_ai_addr().ca_s4addr->sin_addr.s_addr);
				remote_addr = addr.str();
			} break;
			case PF_INET6: {
				caddress_in6 addr;
				addr.unpack(addrinfos.get_addr_info(0).get_ai_addr().ca_s6addr->sin6_addr.s6_addr, 16);
				remote_addr = addr.str();
			} break;
			default: {
				// use the first entry and its domain
				switch (addrinfos.get_addr_info(0).get_ai_family()) {
				case PF_INET: {
					domain = PF_INET;
					caddress_in4 addr;
					addr.set_addr_nbo(addrinfos.get_addr_info(0).get_ai_addr().ca_s4addr->sin_addr.s_addr);
					remote_addr = addr.str();
				} break;
				case PF_INET6: {
					domain = PF_INET6;
					caddress_in6 addr;
					addr.unpack(addrinfos.get_addr_info(0).get_ai_addr().ca_s6addr->sin6_addr.s6_addr, 16);
					remote_addr = addr.str();
				} break;
				default:
					throw eInval("csocket_plain::connect() unable to resolve remote hostname");
				}
			};
			}

			//remote_addr = params.get_param(csocket::PARAM_KEY_REMOTE_HOSTNAME).get_string();

		} else {
			switch (domain) {
			case PF_INET: {
				remote_addr = std::string("0.0.0.0");
			} break;
			case PF_INET6: {
				remote_addr = std::string("0000:0000:0000:0000:0000:0000:0000:0000");
			} break;
			}
		}

		uint16_t remote_port = 0;

		if (not params.get_param(csocket::PARAM_KEY_REMOTE_PORT).get_string().empty()) {
			remote_port = atoi(params.get_param(csocket::PARAM_KEY_REMOTE_PORT).get_string().c_str());
		} else {
			remote_port = 0;
		}



		std::string local_addr;

		if (not params.get_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string().empty()) {

			caddrinfos addrinfos;
			addrinfos.set_ai_hints().set_ai_family(domain);
			addrinfos.set_node(params.get_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string());
			addrinfos.resolve();

			if (addrinfos.size() == 0) {
				throw eInval("csocket_plain::connect() unable to resolve hostname");
			}

			// we take simply the first result returned
			switch (domain) {
			case PF_INET: {
				caddress_in4 addr;
				addr.set_addr_nbo(addrinfos.get_addr_info(0).get_ai_addr().ca_s4addr->sin_addr.s_addr);
				local_addr = addr.str();
			} break;
			case PF_INET6: {
				caddress_in6 addr;
				addr.unpack(addrinfos.get_addr_info(0).get_ai_addr().ca_s6addr->sin6_addr.s6_addr, 16);
				local_addr = addr.str();
			} break;
			default: {
				// use the first entry and its domain
				switch (addrinfos.get_addr_info(0).get_ai_family()) {
				case PF_INET: {
					if (domain != PF_INET)
						throw eInval("csocket_plain::connect() unable to resolve local hostname in domain PF_INET");
					caddress_in4 addr;
					addr.set_addr_nbo(addrinfos.get_addr_info(0).get_ai_addr().ca_s4addr->sin_addr.s_addr);
					local_addr = addr.str();
				} break;
				case PF_INET6: {
					if (domain != PF_INET6)
						throw eInval("csocket_plain::connect() unable to resolve local hostname in domain PF_INET6");
					caddress_in6 addr;
					addr.unpack(addrinfos.get_addr_info(0).get_ai_addr().ca_s6addr->sin6_addr.s6_addr, 16);
					local_addr = addr.str();
				} break;
				default:
					throw eInval("csocket_plain::connect() unable to resolve remote hostname");
				}
			};
			}

			//local_addr = params.get_param(csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string();

		} else {
			switch (domain) {
			case PF_INET: {
				local_addr = std::string("0.0.0.0");
			} break;
			case PF_INET6: {
				local_addr = std::string("0000:0000:0000:0000:0000:0000:0000:0000");
			} break;
			}
		}

		uint16_t local_port;

		if (not params.get_param(csocket::PARAM_KEY_LOCAL_PORT).get_string().empty()) {
			local_port = atoi(params.get_param(csocket::PARAM_KEY_LOCAL_PORT).get_string().c_str());
		} else {
			local_port = 0;
		}





		csockaddr laddr(domain, local_addr, local_port);

		csockaddr raddr(domain, remote_addr, remote_port);

		bool do_reconnect = params.get_param(csocket::PARAM_KEY_DO_RECONNECT).get_bool();

		connect(raddr, laddr, domain, type, protocol, do_reconnect);

	} catch (eSysCall& e) {
		rofl::logging::crit << "[rofl-common][csocket][plain] connect failed" << e << std::endl << *this;
		handle_conn_refused();
	}
}



void
csocket_plain::connect(
	csockaddr ra,
	csockaddr la,
	int domain, 
	int type, 
	int protocol,
	bool do_reconnect)
{
	rofl::logging::debug3 << "[rofl-common][csocket][plain][connect] connecting " << str() << std::endl;

	int rc;
	this->domain 	= domain;
	this->type 		= type;
	this->protocol 	= protocol;
	this->laddr 	= la;
	this->raddr 	= ra;

	if (sd >= 0)
		close();

	ciosrv::cancel_all_timers();
	ciosrv::cancel_all_events();

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

	register_filedesc_r(sd);

	// bind to local address
	if ((rc = bind(sd, la.ca_saddr, (socklen_t)(la.salen))) < 0) {
		throw eSysCall("bind");
	}

	// connect to remote address
	if ((rc = ::connect(sd, (const struct sockaddr*)ra.ca_saddr, (socklen_t)ra.salen)) < 0) {
		switch (errno) {
		case EINPROGRESS: {		// connect is pending, register sd for write events
			sockflags[FLAG_CONNECTING] = true;
			register_filedesc_w(sd);
			rofl::logging::debug3 << "[rofl-common][csocket][plain][connect] socket EINPROGRESS " << str() << std::endl;

		} break;
		case ECONNREFUSED: {	// connect has been refused
			close();
			backoff_reconnect(false);
			rofl::logging::debug3 << "[rofl-common][csocket][plain][connect] ECONNREFUSED " << str() << std::endl;

		} break;
		default: {
			//throw eSysCall("connect ");
			close();
			backoff_reconnect(false);
			rofl::logging::debug3 << "[rofl-common][csocket][plain][connect] Unknown error:"
					<< strerror(errno) <<"("<< errno <<") " << str() << std::endl;
		};
		}
	} else {
		// connect was successful, register sd for read events
		register_filedesc_r(sd);
		sockflags.set(FLAG_CONNECTED);

		if (sockflags.test(FLAG_DO_RECONNECT)) {
			cancel_timer(reconnect_timerid);
		}

		if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
			throw eSysCall("getsockname");
		}

		if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
			throw eSysCall("getpeername");
		}

		rofl::logging::info << "[rofl-common][csocket][plain][connect] socket connected " << str() << std::endl;

		handle_connected();
	}
}



void
csocket_plain::reconnect()
{
	if (not sockflags.test(FLAG_ACTIVE_SOCKET)) {
		throw eInval();
	}
	close();
	ciosrv::cancel_all_timers();
	ciosrv::cancel_all_events();
	sockflags.reset(FLAG_CLOSING);
	sockflags.reset(FLAG_CONNECTING);
	connect(raddr, laddr, domain, type, protocol, sockflags.test(FLAG_DO_RECONNECT));
}



void
csocket_plain::close()
{
	rofl::logging::info << "[rofl-common][csocket][plain][close] closing socket " << str() << std::endl;

	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	int rc = 0;

	if (sd == -1)
		return;

	deregister_filedesc_r(sd);
	deregister_filedesc_w(sd);
	if (not sockflags.test(FLAG_RAW_SOCKET) and sockflags.test(FLAG_CONNECTED)) {
		if ((rc = shutdown(sd, SHUT_RDWR)) < 0) {
			rofl::logging::error << "[rofl-common][csocket][plain][close] error occured during shutdown(): "
					<< eSysCall("shutdown") << std::endl << *this;
		}
	}
	if ((rc = ::close(sd)) < 0) {
		rofl::logging::error << "[rofl-common][csocket][plain][close] error occured during close():"
				<< eSysCall("close") << std::endl << *this;
	}

	sd = -1;
	// keep SOCKET_IS_LISTENING in case of failed connection and required reestablishing
	//sockflags.reset(SOCKET_IS_LISTENING);
	sockflags.reset(FLAG_RAW_SOCKET);
	sockflags.reset(FLAG_CONNECTED);
	sockflags.set(FLAG_CLOSING);

	rofl::logging::info << "[rofl-common][csocket][plain][close] cleaning-up socket." << str() << std::endl;

	// purge pout_squeue
	while (not pout_squeue.empty()) {
		pout_entry_t entry = pout_squeue.front();
		delete entry.mem;
		pout_squeue.pop_front();
	}
}



ssize_t
csocket_plain::recv(void *buf, size_t count, int flags, rofl::csockaddr& from)
{
	if (sd == -1)
		throw eSocketNotConnected();
	int rc;

	// read from socket:
	switch (type) {
	case SOCK_STREAM:
	case SOCK_SEQPACKET: {
		rc = ::read(sd, (void*)buf, count);
		from = raddr;
	} break;
	case SOCK_DGRAM:
	case SOCK_RAW: {
		switch (domain) {
		case AF_INET:  from = csockaddr(caddress_in4("0.0.0.0"), 0); break;
		case AF_INET6: from = csockaddr(caddress_in6("::"), 0); break;
		}
		rc = recvfrom(sd, (void*)buf, count, flags, from.ca_saddr, &from.salen);
	} break;
	default: {
		return 0;
	};
	}


	if (rc > 0) {
		return rc;

	} else if (rc == 0) {
		rofl::logging::debug3 << "[rofl-common][csocket][plain] peer closed connection: "
				<< eSysCall("read") << " " << str() << std::endl;
		close();

		notify(cevent(EVENT_CONN_RESET));
		throw eSysCall("read()");

	} else if (rc < 0) {

		switch(errno) {
		case EAGAIN:
			throw eSocketRxAgain();
		case ECONNRESET: {
			rofl::logging::error << "[rofl-common][csocket][plain] connection reset on socket: "
					<< eSysCall("read") << ", closing endpoint. " << str() << std::endl;
			close();

			notify(cevent(EVENT_CONN_RESET));
			throw eSysCall("read()");
		} break;
		default: {
			rofl::logging::error << "[rofl-common][csocket][plain] error reading from socket: "
					<< eSysCall("read") << ", closing endpoint. " << str() << std::endl;
			close();

			notify(cevent(EVENT_DISCONNECTED));
			throw eSysCall("read()");
		} break;
		}
	}

	return 0;
}



void
csocket_plain::send(cmemory* mem, const rofl::csockaddr& dest)
{
	assert(mem);

	if (not sockflags.test(FLAG_CONNECTED) && not sockflags.test(FLAG_RAW_SOCKET)) {
		rofl::logging::warn << "[rofl-common][csocket][plain] socket not connected, dropping packet " << std::endl << *mem;
		delete mem; return;
	}

	RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

	register_filedesc_w(sd);

	if (not sockflags.test(FLAG_TX_WOULD_BLOCK)) {
		pout_squeue.push_back(pout_entry_t(mem, dest));
	} else {
		struct rofl::openflow::ofp_header* hdr = (struct rofl::openflow::ofp_header*)(mem->somem());

		if (pout_squeue.size() < max_txqueue_size) {
			rofl::logging::warn << "[rofl-common][csocket][plain] socket tx queue nearly full => congestion, "
					<< "xid:0x" << std::hex << (unsigned int)be32toh(hdr->xid) << std::dec << std::endl;
			pout_squeue.push_back(pout_entry_t(mem, dest));
			throw eSocketTxAgainCongestion();
		} else {
			if (not sockflags.test(FLAG_TX_WOULD_BLOCK_NOTIFIED)) {
				sockflags.set(FLAG_TX_WOULD_BLOCK_NOTIFIED);
				rofl::logging::warn << "[rofl-common][csocket][plain] socket tx queue full => congestion, "
						<< "last packet queued, tx-queue exhausted, xid:0x" << std::hex << (unsigned int)be32toh(hdr->xid) << std::dec << std::endl;
				pout_squeue.push_back(pout_entry_t(mem, dest));
				throw eSocketTxAgainTxQueueFull(); // inform sender about failed transmission
			} else {
				rofl::logging::warn << "[rofl-common][csocket][plain] socket tx queue full => congestion, "
						<< "dropping message, xid:0x" << std::hex << (unsigned int)be32toh(hdr->xid) << std::dec << std::endl;
				delete mem;
				throw eSocketTxAgainPacketDropped();
			}
		}
	}
}



void
csocket_plain::dequeue_packet()
{
	{
		RwLock lock(&pout_squeue_lock, RwLock::RWLOCK_WRITE);

		int rc = 0;

		while (not pout_squeue.empty()) {
			pout_entry_t& entry = pout_squeue.front(); // reference, do not make a copy

			rofl::logging::trace << "[rofl-common][csocket][plain] sending to socket, message: "
					<< std::endl << *(entry.mem);

			if (had_short_write) {
				rofl::logging::warn << "[rofl-common][csocket][plain] resending due to short write: " << std::endl << entry;
				had_short_write = false;
			}


			int flags = MSG_NOSIGNAL;
			if ((rc = sendto(sd, entry.mem->somem() + entry.msg_bytes_sent, entry.mem->memlen() - entry.msg_bytes_sent, flags,
									entry.dest.ca_saddr, entry.dest.salen)) < 0) {
				switch (errno) {
				case EAGAIN:
					sockflags.set(FLAG_TX_WOULD_BLOCK);
					return;
				case EPIPE:

					goto out;
					return;
				case EMSGSIZE:
					rofl::logging::warn << "[rofl-common][csocket][plain] dequeue_packet() dropping packet (EMSGSIZE) " << *(entry.mem) << std::endl;
					break;
				default:
					rofl::logging::warn << "[rofl-common][csocket][plain] dequeue_packet() dropping packet " << *(entry.mem) << std::endl;
					throw eSysCall("sendto");
				}
			}
			else if ((((unsigned int)(rc + entry.msg_bytes_sent)) < entry.mem->memlen())) {

				if (SOCK_STREAM == type) {
					had_short_write = true;
					entry.msg_bytes_sent += rc;
					rofl::logging::warn << "[rofl-common][csocket][plain] short write on socket descriptor:" << sd << ", retrying..." << std::endl << entry;
				} else {
					rofl::logging::warn << "[rofl-common][csocket][plain] short write on socket descriptor:" << sd << ", dropping packet." << std::endl;
					delete entry.mem;
					pout_squeue.pop_front();
				}
				return;
			}

			sockflags.reset(FLAG_TX_WOULD_BLOCK);
			sockflags.reset(FLAG_TX_WOULD_BLOCK_NOTIFIED);

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


