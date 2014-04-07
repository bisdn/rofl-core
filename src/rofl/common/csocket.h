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
#include "rofl/common/caddress.h"
#include "rofl/common/logging.h"
#include "rofl/common/cparams.h"

namespace rofl {

class eSocketBase		: public RoflException {};
class eSocketAgain		: public eSocketBase {};
class eSocketNotConnected	: public eSocketBase {};
class eSocketTypeNotFound	: public eSocketBase {};
class eSocketParamNotFound	: public eSocketBase {};

class csocket; // forward declaration for csocket_owner, see below

/**
 * @class csocket_owner
 * @brief	An abstract interface defining the consumer side of a csocket.
 *
 * This class defines an abstract interface for interacting with instances of type csocket.
 * It defines five methods for reception of notifications emitted by specific socket events:
 * - accepted
 * - connected
 * - connect_refised
 * - read
 * - closed
 * All methods are pure abstract and must be implemented by a class deriving from csocket_owner.
 *
 * @see csocket
 */
class csocket_owner
{
public:
	friend class csocket;

	/**
	 * @brief	Destructor.
	 */
	virtual
	~csocket_owner() {};

	/**
	 * @brief	Called once a listening socket has accepted a connection request from a remote peer entity.
	 *
	 * @param socket pointer to csocket instance emitting the motification
	 * @param newsd socket descriptor of new created Unix socket
	 * @param ra address of peer entity
	 */
	virtual void
	handle_accepted(
			csocket& socket, int newsd) = 0;

	/**
	 * @brief	Called once a connection request to a remote entity has succeeded.
	 *
	 * @param socket pointer to csocket instance emitting the motification
	 * @param sd socket descriptor used for new connection
	 */
	virtual void
	handle_connected(csocket& socket) = 0;

	/**
	 * @brief	Called once a connection request to a remote entity failed.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 * @param sd socket descriptor used for connection
	 */
	virtual void
	handle_connect_refused(csocket& socket) = 0;

	/**
	 * @brief	Called once new data is available for reading from the socket.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 * @param sd socket descriptor used by the connection
	 */
	virtual void
	handle_read(csocket& socket) = 0;

	/**
	 * @brief	Called once new data is available for reading from the socket.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 * @param sd socket descriptor used by the connection
	 */
	virtual void
	handle_write(csocket& socket) = 0;

	/**
	 * @brief	Called once the socket has been shutdown and closed.
	 *
	 * @param socket pointer to csocket instance emitting the notification
	 * @param sd socket descriptor used by the connection
	 */
	virtual void
	handle_closed(csocket& socket) = 0;
};


/**
 * @class csocket
 * @brief 	A single socket.
 *
 * This class provides basic support for socket based communication.
 * Its aim is to encapsulate functionality for establishing a socket
 * in active and passive mode. For using a socket, the owning class
 * must implement the interface defined in csocket_owner.
 *
 * The socket is set to non-blocking mode,
 * thus it does not block indefinitely during read or write operations,
 * rather it returns control to the calling entity asap.
 *
 * For listening sockets, method csocket_owner::handle_accepted() will be
 * called. The socket owner should create a new csocket instance and assigning
 * the new obtained socket descriptor to it.
 *
 * @see csocket_owner
 */
class csocket :
	public virtual ciosrv
{
public:

	/* supported socket types */
	enum socket_type_t {
		SOCKET_TYPE_PLAIN 		= 0,
		SOCKET_TYPE_OPENSSL 	= 1,
	};

protected:

	csocket_owner				*socket_owner;		/**< owner of this csocket instance */
	enum socket_type_t			socket_type;
	int 						sd; 				/**< the socket descriptor */
	caddress 					laddr; 				/**< local address socket is bound to */
	caddress 					raddr; 				/**< remote address of peer entity */
	int 						domain; 			/**< socket domain (PF_INET, PF_UNIX, ...) */
	int 						type; 				/**< socket type (SOCK_STREAM, SOCK_DGRAM, ...) */
	int 						protocol; 			/**< socket protocol (TCP, UDP, SCTP, ...) */
	int 						backlog; 			/**< backlog value for listen() system call */
	cparams						socket_params;		/**< parameters for a specific socket instance */

public:

	/**
	 *
	 */
	static csocket*
	csocket_factory(
			enum socket_type_t socket_type, csocket_owner *owner);

	/**
	 *
	 */
	static cparams
	get_default_params(
			enum socket_type_t socket_type);


public:



	/**
	 * @brief	Constructor for new empty csocket instances.
	 *
	 * @param owner socket owning entity implementing interface csocket_owner
	 */
	csocket(csocket_owner *owner, enum socket_type_t socket_type) :
		socket_owner(owner),
		socket_type(socket_type),
		sd(-1),
		domain(0),
		type(0),
		protocol(0),
		backlog(0)
	{};




	/**
	 * @brief 	Destructor.
	 *
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
			cparams const& params) = 0;


	/**
	 * @brief 	Handle accepted socket descriptor obtained from external listening socket
	 */
	virtual void
	accept(
			cparams const& socket_params, int sd) = 0;



	/**
	 * @brief	Open socket and connect to peer entity (client side).
	 *
	 * This opens a socket and connects to a peer entity.
	 *
	 * @param params parameters for this socket
	 */
	virtual void
	connect(
			cparams const& params) = 0;


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
	send(cmemory *mem, caddress const& dest = caddress(AF_INET, "0.0.0.0", 0)) = 0;


	/**
	 *
	 */
	virtual bool
	is_connected() const = 0;

public:

	/**
	 *
	 */
	enum rofl::csocket::socket_type_t
	get_socket_type() const { return socket_type; };

	/**
	 *
	 */
	cparams const&
	get_socket_params() const { return socket_params; };

	/**
	 *
	 */
	rofl::caddress&
	set_laddr() { return laddr; };

	/**
	 *
	 */
	rofl::caddress const&
	get_laddr() const { return laddr; };

	/**
	 *
	 */
	rofl::caddress&
	set_raddr() { return raddr; };

	/**
	 *
	 */
	rofl::caddress const&
	get_raddr() const { return raddr; };

	/**
	 *
	 */
	void
	set_domain(int domain) { this->domain = domain; };

	/**
	 *
	 */
	int
	get_domain() const { return domain; };

	/**
	 *
	 */
	void
	set_type(int type) { this->type = type; };

	/**
	 *
	 */
	int
	get_type() const { return type; };

	/**
	 *
	 */
	void
	set_protocol(int protocol) { this->protocol = protocol; };

	/**
	 *
	 */
	int
	get_protocol() const { return protocol; };


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

public:

	friend std::ostream&
	operator<< (std::ostream& os, csocket const& sock) {
		os << rofl::indent(0) << "<csocket "
			<< "sd:" << sock.sd << " "
			<< "domain:" << sock.domain << " "
			<< "type:" << sock.type << " "
			<< "protocol:" << sock.protocol << " ";
		os << ">" << std::endl;
		os << rofl::indent(2) << "<raddr: " << sock.raddr << " >" << std::endl;
		os << rofl::indent(2) << "<laddr: " << sock.laddr << " >" << std::endl;
		rofl::indent i(2);
		os << sock.socket_params;
		return os;
	};

};

}; // end of namespace

#endif
