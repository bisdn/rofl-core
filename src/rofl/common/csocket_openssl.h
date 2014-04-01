/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CSOCKET_OPENSSL_H
#define CSOCKET_OPENSSL_H

#include <list>
#include <bitset>
#include <stdio.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "rofl/common/csocket.h"

namespace rofl {


/**
 * @class csocket_impl
 * @brief 	A single unencrypted socket.
 *
 * This class provides basic support for socket based communication.
 * Its aim is to encapsulate functionality for establishing a socket
 * in active and passive mode. For using a socket, the owning class
 * must implement the interface defined in csocket_impl_owner.
 *
 * The socket is set to non-blocking mode,
 * thus it does not block indefinitely during read or write operations,
 * rather it returns control to the calling entity asap.
 *
 * For listening sockets, method csocket_impl_owner::handle_accepted() will be
 * called. The socket owner should create a new csocket_impl instance and assigning
 * the new obtained socket descriptor to it.
 *
 * @see csocket_impl_owner
 */
class csocket_openssl :
	public csocket
{
	static bool ssl_initialized;

	/**
	 * @brief	Initialize SSL library internals.
	 */
	static void
	ssl_init();

private:

	struct pout_entry_t {
		cmemory *mem;
		caddress dest;
		size_t msg_bytes_sent;
		pout_entry_t(cmemory *mem = 0, caddress const& dest = caddress(AF_INET, "0.0.0.0", 0)) :
			mem(mem), dest(dest), msg_bytes_sent(0) {};
		pout_entry_t(pout_entry_t const& e) :
			mem(0), dest(caddress(AF_INET, "0.0.0.0", 0)), msg_bytes_sent(0) {
			*this = e;
		};
		struct pout_entry_t&
		operator= (pout_entry_t const& e) {
			if (this == &e) return *this;
			mem = e.mem;
			dest = e.dest;
			msg_bytes_sent = e.msg_bytes_sent;
			return *this;
		};
		friend std::ostream&
		operator<< (std::ostream& os, struct pout_entry_t const& entry) {
			os << indent(0) << "<struct pout_entry_t >" << std::endl;
			os << indent(2) << "<mem:0x" << entry.mem << " >" << std::endl;
			os << indent(2) << "<dest:" << entry.dest << " >" << std::endl;
			os << indent(2) << "<msg_bytes_sent:" << entry.msg_bytes_sent << " >" << std::endl;
			return os;
		};
	};

	bool 						had_short_write;
	pthread_rwlock_t			pout_squeue_lock;	/**< rwlock for access to pout_squeue */
	std::list<pout_entry_t> 	pout_squeue; 		/**< queue of outgoing packets */


	static std::string const 	SOCKET_PARAM_REMOTE_ADDR;
	static std::string const 	SOCKET_PARAM_LOCAL_ADDR;
	static std::string const	SOCKET_PARAM_DOMAIN;
	static std::string const	SOCKET_PARAM_TYPE;
	static std::string const	SOCKET_PARAM_PROTOCOL;
	static std::string const	SOCKET_PARAM_CAPATH;
	static std::string const	SOCKET_PARAM_CAFILE;
	static std::string const	SOCKET_PARAM_CERT;
	static std::string const	SOCKET_PARAM_KEY;

protected:

	enum socket_flag_t {
		SOCKET_IS_LISTENING = 1, 	/**< socket is in listening state */
		CONNECT_PENDING		= 2, 	/**< connect() call is pending */
		RAW_SOCKET			= 3, 	/**< socket is in raw mode (link layer) */
		CONNECTED			= 4,	/**< socket is connected */
		FLAG_ACTIVE_SOCKET	= 5,
		FLAG_DO_RECONNECT	= 6,
		FLAG_SEND_CLOSED_NOTIFICATION = 7,
	};

	std::bitset<16> 			sockflags; /**< socket flags (see below) */

	enum csocket_impl_timer_t {
		TIMER_RECONNECT 	= 1,
	};

	enum csocket_impl_event_t {
		EVENT_CONN_RESET	= 1,
		EVENT_DISCONNECTED	= 2,
	};



	int							reconnect_start_timeout;
	int 						reconnect_in_seconds; 	// reconnect in x seconds
	int 						reconnect_counter;

#define RECONNECT_START_TIMEOUT 1						// start reconnect timeout (default 1s)

	/*
	 * OpenSSL related structures
	 */
	BIO							*bio;


public:



	/**
	 * @brief	Constructor for new empty csocket_impl instances.
	 *
	 * @param owner socket owning entity implementing interface csocket_impl_owner
	 */
	csocket_openssl(csocket_owner *owner);




	/**
	 * @brief 	Destructor.
	 *
	 */
	virtual
	~csocket_openssl();




	/**
	 * @brief	Open socket in listening mode (server side).
	 *
	 * This opens a socket in listening mode bound to address 'la'
	 * with the specified socket parameters.
	 *
	 * @param la the local address for binding this socket
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 * @param backlog backlog value
	 * @throw eSocketListenFailed failure in listen() system call
	 * @throw eSocketAddressInUse bind error while calling bind()
	 * @throw eSocketError thrown for all other socket related errors
	 */
	virtual void
	listen(
		caddress la,
		int domain = PF_INET, 
		int type = SOCK_STREAM, 
		int protocol = 0,
		int backlog = 10,
		std::string devname = std::string(""));


	/**
	 * @brief 	Handle accepted socket descriptor obtained from external listening socket
	 */
	virtual void
	accept(int sd);



	/**
	 * @brief	Open socket and connect to peer entity (client side).
	 *
	 * This opens a socket and connects to a peer entity.
	 *
	 * @param ra remote address of peer entity to connect to
	 * @param la address used for binding socket locally
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 * @throw eSocketConnectFailed thrown if the connect() operation failed finally
	 * @throw eSocketError thrown for all other socket related errors
	 */
	virtual void
	connect(
		caddress ra,
		caddress la = caddress(AF_INET, "0.0.0.0", 0),
		int domain = PF_INET,
		int type = SOCK_STREAM,
		int protocol = 0,
		bool do_reconnect = false);


	/**
	 * @brief	Reconnect this socket.
	 *
	 * Reconnects this socket to the previously connected peer.
	 * The socket must be an active one, i.e. we have all data
	 * required for calling ::connect() towards the peer. A passive
	 * socket is throwing an exception of type eSocketError.
	 */
	virtual void
	reconnect();


	/**
	 * @brief	Closes this socket.
	 *
	 * Calls the shutdown() system call and closes the socket.
	 * Also deletes all packets queued in pout_squeue for transmission.
	 * After calling cclose() it is safe to call caopen() or cpopen() again.
	 */
	virtual void
	close();


	/**
	 * @brief	Reads bytes from socket
	 *
	 */
	virtual ssize_t
	recv(void *buf, size_t count);


	/**
	 * @brief	Store a packet for transmission.
	 *
	 * This method stores the packet in the outgoing queue for transmission.
	 * If the socket is not connected and not a raw socket, the packet
	 * will be deleted and thus dropped.
	 * After pushing the packet pointer onto the outgoing queue, the method registers
	 * the socket descriptor for a write operation and returns, giving the
	 * calling entity back control.
	 *
	 * csocket_impl will call mem's destructor in order to remove the packet from heap
	 * once it has been sent out. Make sure, that mem is pointing to a heap allocated
	 * cmemory instance!
	 *
	 * @param mem cmemory instance to be sent out
	 */
	virtual void
	send(cmemory *mem, caddress const& dest = caddress(AF_INET, "0.0.0.0", 0));


	/**
	 *
	 */
	virtual bool
	is_connected() const { return sockflags.test(CONNECTED); };


	/**
	 *
	 */
	static cparams
	get_params();


private:

	//
	// socket specific methods, must be overloaded in derived class
	//

	/**
	 * Connect on socket succeeded (client mode).
	 *
	 * This notification method is called if the connect() operation succeeds
	 * on the socket. It should be overwritten by a derived class
	 * if this signal is required for further operation.
	 */
	void
	handle_connected() {
		if (socket_owner) {
			socket_owner->handle_connected(*this);
		}
	};

	/**
	 * Connect on socket failed (client mode).
	 *
	 * This notification method is called if the connect() operation fails
	 * on the socket. It should be overwritten by a derived class
	 * if the derived class wants to act upon this condition.
	 */
	void
	handle_conn_refused() {
		if (socket_owner) {
			socket_owner->handle_connect_refused(*this);
		}
	};

	/**
	 * A new incoming connection was accepted (listening mode).
	 *
	 * This notification method is called upon receipt of a new incoming
	 * connection request. It should be overwritten by a derived class
	 * for creation of a new instance or direct handling.
	 * @param  newsd the new socket descriptor
	 * @param ra reference to the peer entity's address
	 */
	void
	handle_accepted(int newsd, caddress const& ra) {
		if (socket_owner) {
			socket_owner->handle_accepted(*this, newsd, ra);
		}
	};

	/**
	 * Socket was closed.
	 *
	 * This notification method is called when the socket is closed.
	 * @param sd the socket descriptor
	 */
	void
	handle_closed() {
		if (socket_owner) {
			socket_owner->handle_closed(*this);
		}
	};

	/**
	 * Read data from socket.
	 *
	 * This notification method is called from within csocket_impl::handle_revent().
	 * A derived class should read a packet from the socket. This method
	 * must be overwritten by a derived class.
	 * @param fd the socket descriptor
	 */
	void
	handle_read() {
		if (socket_owner) {
			socket_owner->handle_read(*this);
		}
	};



private:


	/**
	 *
	 */
	void
	backoff_reconnect(
			bool reset_timeout = false);


	/*
	 * inherited from ciosrv
	 */


	/**
	 *
	 */
	virtual void
	handle_timeout(
			int opaque, void *data = (void*)0);

	/**
	 *
	 */
	virtual void
	handle_event(
			cevent const& ev);


	/**
	 * Handle read events on socket descriptor.
	 *
	 * Implemented by csocket_impl, it either handles accept() in listening mode
	 * or read operations in non-listening mode. In listening mode and after
	 * return of the accept() system call, the handle_accepted() method is called.
	 * In non-listening mode the handle_read() method is called.
	 * @param fd the socket descriptor
	 */
	virtual void
	handle_revent(int fd);


	/**
	 * Handle write events on socket descriptor.
	 *
	 * This method controls the connect() process in connecting state
	 * or calls the dequeue_packet() method for transmitting any queued
	 * packets in established state.
	 * @param fd the socket descriptor
	 */
	virtual void
	handle_wevent(int fd);


	/**
	 * Handle exception events on socket descriptor.
	 * @param fd the socket descriptor
	 */
	virtual void
	handle_xevent(int fd);


private:


	/**
	 * Send packets in outgoing queue pout_squeue.
	 *
	 * This method transmits all pending packets from the transmission
	 * queue pout_squeue.
	 */
	void
	dequeue_packet();

public:

	friend std::ostream&
	operator<< (std::ostream& os, csocket_openssl const& sock) {
		os << indent(0) << "<csocket_openssl "
			<< "sd:" << sock.sd << " "
			<< "domain:" << sock.domain << " "
			<< "type:" << sock.type << " "
			<< "protocol:" << sock.protocol << " ";
		os << ">" << std::endl;
		os << indent(2) << "<raddr: " << sock.raddr << " >" << std::endl;
		os << indent(2) << "<laddr: " << sock.laddr << " >" << std::endl;
		os << indent(2) << "<flags: ";
		if (sock.sockflags.test(SOCKET_IS_LISTENING)) {
			os << "LISTENING ";
		}
		if (sock.sockflags.test(CONNECT_PENDING)) {
			os << "CONNECT-PENDING ";
		}
		if (sock.sockflags.test(RAW_SOCKET)) {
			os << "RAW-SOCKET ";
		}
		if (sock.sockflags.test(CONNECTED)) {
			os << "CONNECTED ";
		}
		os << ">" << std::endl;
		return os;
	};

};

}; // end of namespace

#endif
