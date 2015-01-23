/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CSOCKET_H
#define CSOCKET_H

#include <list>
#include <bitset>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>

#include "rofl/common/croflexception.h"
#include "rofl/common/ciosrv.h"
#include "rofl/common/csockaddr.h"
#include "rofl/common/logging.h"
#include "rofl/common/cparams.h"

namespace rofl {

class eSocketBase					: public RoflException {};
class eSocketRxAgain				: public eSocketBase {};
class eSocketTxAgain				: public eSocketBase {};
class eSocketTxAgainCongestion		: public eSocketTxAgain {};
class eSocketTxAgainTxQueueFull		: public eSocketTxAgain {};
class eSocketTxAgainPacketDropped	: public eSocketTxAgain {};
class eSocketNotConnected			: public eSocketBase {};
class eSocketTypeNotFound			: public eSocketBase {};
class eSocketParamNotFound			: public eSocketBase {};

class csocket; // forward declaration

/**
 * @interface csocket_env
 * @ingroup common_devel_bsd_sockets
 * @brief	An abstract interface defining the consumer side of a csocket.
 *
 * This class defines an abstract interface for interacting with instances of type csocket.
 * It defines five methods for reception of notifications emitted by specific socket events:
 * - accepted
 * - connected
 * - connect_refised
 * - read
 * - closed
 * All methods are pure abstract and must be implemented by a class deriving from csocket_env.
 *
 * @see csocket
 */
class csocket_env {
	friend class csocket;
public:

	/**
	 * @brief	Destructor.
	 */
	virtual
	~csocket_env()
	{};

	/**
	 * @brief	Called once a listening socket has accepted a connection request from a remote peer entity.
	 *
	 * @param socket pointer to csocket instance emitting the motification
	 * @param newsd socket descriptor of new created Unix socket
	 * @param ra address of peer entity
	 */
	virtual void
	handle_listen(
			rofl::csocket& socket,
			int newsd) = 0;

	/**
	 * @brief	Called once this csocket entity has succeeded its accept() method.
	 *
	 * @param socket pointer to csocket instance emitting the motification
	 * @param newsd socket descriptor of new created Unix socket
	 * @param ra address of peer entity
	 */
	virtual void
	handle_accepted(
			rofl::csocket& socket) = 0;

	/**
	 * @brief	Called once accepting a request from a remote entity failed.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 */
	virtual void
	handle_accept_refused(
			rofl::csocket& socket) = 0;

	/**
	 * @brief	Called once a connection request has succeeded its connect() method.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 */
	virtual void
	handle_connected(
			rofl::csocket& socket) = 0;

	/**
	 * @brief	Called once a connection request to a remote entity failed.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 * @param sd socket descriptor used for connection
	 */
	virtual void
	handle_connect_refused(
			rofl::csocket& socket) = 0;

	/**
	 * @brief	Called once a connection request to a remote entity failed.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 * @param sd socket descriptor used for connection
	 */
	virtual void
	handle_connect_failed(
			rofl::csocket& socket) = 0;

	/**
	 * @brief	Called once new data is available for reading from the socket.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 * @param sd socket descriptor used by the connection
	 */
	virtual void
	handle_read(
			rofl::csocket& socket) = 0;

	/**
	 * @brief	Called once the socket accept additional data for sending.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 * @param sd socket descriptor used by the connection
	 */
	virtual void
	handle_write(
			rofl::csocket& socket) = 0;

	/**
	 * @brief	Called once the socket has been shutdown and closed.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 * @param sd socket descriptor used by the connection
	 */
	virtual void
	handle_closed(
			rofl::csocket& socket) = 0;
};


/**
 * @class csocket
 * @ingroup common_devel_bsd_sockets
 * @brief 	A single socket.
 *
 * This class provides basic support for socket based communication.
 * Its aim is to encapsulate functionality for establishing a socket
 * in active and passive mode. For using a socket, the owning class
 * must implement the interface defined in csocket_env.
 *
 * The socket is set to non-blocking mode,
 * thus it does not block indefinitely during read or write operations,
 * rather it returns control to the calling entity asap.
 *
 * For listening sockets, method rofl::csocket_env::handle_accepted() will be
 * called. The socket owner should create a new csocket instance and assigning
 * the new obtained socket descriptor to it.
 *
 * @see csocket_env
 */
class csocket :
	public virtual ciosrv
{
public:

	/* supported socket types */
	enum socket_type_t {
		SOCKET_TYPE_UNKNOWN		= 0,
		SOCKET_TYPE_PLAIN 		= 1,
		SOCKET_TYPE_OPENSSL 	= 2,
	};

public:

	/**
	 *
	 */
	static csocket*
	csocket_factory(
			enum socket_type_t socket_type, csocket_env *owner, pthread_t tid = 0);

	/**
	 *
	 */
	static cparams
	get_default_params(
			enum socket_type_t socket_type);

public:

	/**
	 * @brief	csocket constructor
	 *
	 * @param env environment for this csocket instance
	 */
	csocket(
			csocket_env *env,
			enum socket_type_t socket_type,
			pthread_t tid = 0) :
				ciosrv(tid),
				socket_env(env),
				socket_type(socket_type),
				sd(-1),
				domain(0),
				type(0),
				protocol(0),
				backlog(0)
	{};

	/**
	 * @brief 	csocket destructor
	 */
	virtual
	~csocket()
	{};

	/**
	 * @brief	Open socket in listening mode (server side).
	 *
	 * This opens a socket in listening mode bound to address 'la'
	 * with the specified socket parameters.
	 *
	 * @param params parameters for this socket
	 */
	virtual void
	listen(
			const rofl::cparams& params) = 0;

	/**
	 * @brief 	Handle accepted socket descriptor obtained from external listening socket
	 */
	virtual void
	accept(
			const rofl::cparams& socket_params,
			int sd) = 0;

	/**
	 * @brief	Open socket and connect to peer entity (client side).
	 *
	 * This opens a socket and connects to a peer entity.
	 *
	 * @param params parameters for this socket
	 */
	virtual void
	connect(
			const rofl::cparams& params) = 0;

	/**
	 * @brief	Reconnect this socket.
	 *
	 * Reconnects this socket to the previously connected peer.
	 * The socket must be an active one, i.e. we have all data
	 * required for calling ::connect() towards the peer. A passive
	 * socket is throwing an exception of type eSocketError.
	 */
	virtual void
	reconnect() = 0;


	/**
	 * @brief	Closes this socket.
	 *
	 * Calls the shutdown() system call and closes the socket.
	 * Also deletes all packets queued in pout_squeue for transmission.
	 * After calling cclose() it is safe to call caopen() or cpopen() again.
	 */
	virtual void
	close() = 0;


	/**
	 * @brief	Reads bytes from socket
	 *
	 */
	virtual ssize_t
	recv(void *buf, size_t count) = 0;
	virtual ssize_t
	recv(void *buf, size_t count, int flags, rofl::csockaddr& from) = 0;

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
	 * csocket will call mem's destructor in order to remove the packet from heap
	 * once it has been sent out. Make sure, that mem is pointing to a heap allocated
	 * cmemory instance!
	 *
	 * @param mem cmemory instance to be sent out
	 */
	virtual void
	send(cmemory *mem, rofl::csockaddr const& dest = rofl::csockaddr()) = 0;


	/**
	 *
	 */
	virtual bool
	is_established() const = 0;

	/**
	 *
	 */
	virtual bool
	write_would_block() const = 0;

public:

	/**
	 *
	 */
	int
	get_sd() const
	{ return sd; };

	/**
	 *
	 */
	enum rofl::csocket::socket_type_t
	get_socket_type() const
	{ return socket_type; };

	/**
	 *
	 */
	const rofl::cparams&
	get_socket_params() const
	{ return socket_params; };

	/**
	 *
	 */
	rofl::csockaddr&
	set_laddr()
	{ return laddr; };

	/**
	 *
	 */
	rofl::csockaddr const&
	get_laddr() const
	{ return laddr; };

	/**
	 *
	 */
	rofl::csockaddr&
	set_raddr()
	{ return raddr; };

	/**
	 *
	 */
	rofl::csockaddr const&
	get_raddr() const
	{ return raddr; };

	/**
	 *
	 */
	void
	set_domain(int domain)
	{ this->domain = domain; };

	/**
	 *
	 */
	int
	get_domain() const
	{ return domain; };

	/**
	 *
	 */
	void
	set_type(int type)
	{ this->type = type; };

	/**
	 *
	 */
	int
	get_type() const
	{ return type; };

	/**
	 *
	 */
	void
	set_protocol(int protocol)
	{ this->protocol = protocol; };

	/**
	 *
	 */
	int
	get_protocol() const
	{ return protocol; };


	/**
	 *
	 */
	static bool 
	supports_socket_type(enum socket_type_t socket_type);

	//Common Keys
	static std::string const 	PARAM_KEY_DO_RECONNECT;
	static std::string const 	PARAM_KEY_REMOTE_HOSTNAME;
	static std::string const 	PARAM_KEY_REMOTE_PORT;
	static std::string const 	PARAM_KEY_LOCAL_HOSTNAME;
	static std::string const 	PARAM_KEY_LOCAL_PORT;
	static std::string const	PARAM_KEY_DOMAIN;
	static std::string const	PARAM_KEY_TYPE;
	static std::string const	PARAM_KEY_PROTOCOL;

	//Common values (non-numeric)
	static std::string const	PARAM_DOMAIN_VALUE_INET_ANY;
	static std::string const	PARAM_DOMAIN_VALUE_INET;
	static std::string const	PARAM_DOMAIN_VALUE_INET6;
	static std::string const	PARAM_TYPE_VALUE_STREAM;
	static std::string const	PARAM_TYPE_VALUE_DGRAM;
	static std::string const	PARAM_PROTOCOL_VALUE_TCP;
	static std::string const	PARAM_PROTOCOL_VALUE_UDP;

	//Socket type specific keys
	static std::string const	PARAM_SSL_KEY_CA_PATH;
	static std::string const	PARAM_SSL_KEY_CA_FILE;
	static std::string const	PARAM_SSL_KEY_CERT;
	static std::string const	PARAM_SSL_KEY_PRIVATE_KEY;
	static std::string const	PARAM_SSL_KEY_PRIVATE_KEY_PASSWORD;
	static std::string const	PARAM_SSL_KEY_VERIFY_MODE;
	static std::string const	PARAM_SSL_KEY_VERIFY_DEPTH;
	static std::string const	PARAM_SSL_KEY_CIPHERS;

public:

	friend std::ostream&
	operator<< (std::ostream& os, csocket const& sock) {
		os << rofl::indent(0) << "<csocket "
			<< "sd:" << sock.sd << " "
			<< "domain:" << sock.domain << " "
			<< "type:" << sock.type << " "
			<< "protocol:" << sock.protocol << " ";
		os << ">" << std::endl;
		os << rofl::indent(2) << "<raddr: >" << std::endl;
		{ rofl::indent i(4); os << sock.raddr; };
		os << rofl::indent(2) << "<laddr: >" << std::endl;
		{ rofl::indent i(4); os << sock.laddr; };
		return os;
	};

protected:

	csocket_env*        socket_env;			/**< environment of this csocket instance */
	enum socket_type_t  socket_type;		/**< socket type (plain, ssl, ...) */
	int                 sd; 				/**< the socket descriptor */
	csockaddr           laddr; 				/**< local address socket is bound to */
	csockaddr           raddr; 				/**< remote address of peer entity */
	int                 domain; 			/**< socket domain (PF_INET, PF_UNIX, ...) */
	int                 type; 				/**< socket type (SOCK_STREAM, SOCK_DGRAM, ...) */
	int                 protocol; 			/**< socket protocol (TCP, UDP, SCTP, ...) */
	int                 backlog; 			/**< backlog value for listen() system call */
	cparams             socket_params;		/**< parameters for a specific socket instance */
};

}; // end of namespace

#endif
