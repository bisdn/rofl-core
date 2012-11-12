/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CSOCKET_H
#define CSOCKET_H

#include <list>
#include <bitset>
#include <stdio.h>

#include "cerror.h"
#include "ciosrv.h"
#include "caddress.h"
#include "cmemory.h"
#include "thread_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <pthread.h>

#ifdef __cplusplus
}
#endif

/* error classes */
class eSocketBase     		: public cerror {}; /**< base class for socket related errors */
class eSocketBindFailed		: public eSocketBase {}; /**< bind system call failed */
class eSocketAddressInUse 	: public eSocketBase {}; /**< address for socket is busy */
class eSocketListenFailed 	: public eSocketBase {}; /**< listen operation on socket failed */
class eSocketAcceptFailed 	: public eSocketBase {}; /**< accept operation on socket failed */
class eSocketConnectFailed 	: public eSocketBase {}; /**< connect operation on socket failed (finally) */
class eSocketSendFailed		: public eSocketBase {}; /**< send operation on socket failed */
class eSocketError   		: public eSocketBase {}; /**< generic operation on socket failed, e.g. getsockopt(), ioctl(), ... */
class eSocketIoctl			: public eSocketBase {}; /**< ioctl failed */
class eSocketShortSend		: public eSocketBase {}; /**< send() 2 call returned with fewer bytes than expected */
class eSocketReadFailed		: public eSocketBase {}; /**< read() failed or packet cannot be read completely */

class csocket; // forward declaration for csocket_owner, see below

class csocket_owner
{
public:
	virtual ~csocket_owner() {};
	virtual void handle_accepted(csocket *socket, int newsd, caddress const& ra) = 0;
	virtual void handle_connected(csocket *socket, int sd) = 0;
	virtual void handle_connect_refused(csocket *socket, int sd) = 0;
	virtual void handle_read(csocket *socket, int sd) = 0;
	virtual void handle_closed(csocket *socket, int sd) = 0;
};


/**
 * Abstract base class for socket operations.
 *
 * This class provides basic support for socket based communication.
 * Its aim is to encapsulate functionality for establishing a socket
 * and to provide send() related functions. The reading side must be
 * provided by the derived class. The socket is set to non-blocking mode,
 * thus it does not block indefinitely during read or write operations,
 * rather it returns control to the calling entity asap.
 *
 * The following function must be overwritten:
 * - handle_read()
 *
 * The following functions may be overwritten:
 * - handle_accepted()
 * - handle_connected()
 * - handle_conn_refused()
 * - handle_closed()
 *
 * default: TCP/IPv4 with backlog=10 in non-blocking mode
 * csocket *sock = new csocket();
 * caddress la = caddress(AF_INET, "0.0.0.0", 1111);
 * sock->popen(la); => opens a socket with default values on port 1111 in listening mode
 */
class csocket :
	public virtual ciosrv
{
private:

		pthread_rwlock_t	pout_squeue_lock;	/**< rwlock for access to pout_squeue */
		std::list<cmemory*> pout_squeue; 		/**< queue of outgoing packets */

public: // static data structures

	static std::set<csocket*> csock_list; /**< list of all csocket instances */

protected:

	csocket_owner			*socket_owner;		/**< owner of this csocket instance */

	enum socket_flag_t {
		SOCKET_IS_LISTENING = 1, /**< socket is in listening state */
		CONNECT_PENDING, /**< connect() call is pending */
		RAW_SOCKET, /**< socket is in raw mode (link layer) */
		CONNECTED, /**< socket is connected */
	};

	std::bitset<16> 		sockflags; /**< socket flags (see below) */

public:

	int 					sd; 				/**< the socket descriptor */
	caddress 				laddr; 				/**< local address socket is bound to */
	caddress 				raddr; 				/**< remote address of peer entity */
	int 					domain; 			/**< socket domain (PF_INET, PF_UNIX, ...) */
	int 					type; 				/**< socket type (SOCK_STREAM, SOCK_DGRAM, ...) */
	int 					protocol; 			/**< socket protocol (TCP, UDP, SCTP, ...) */
	int 					backlog; 			/**< backlog value for listen() system call */


public:

	/**
	 * Constructor for new sockets created by accept().
	 * @param sd new socket descriptor
	 * @param domain socket domain (default: PF_INET)
	 * @param type socket type (default: SOCK_STREAM)
	 * @param protocol socket protocol (default: 0)
	 * @param backlog listen backlog (default: 10)
	 */
	csocket(csocket_owner *owner,
			int sd,
			caddress const& ra,
			int domain 		= PF_INET,
			int type 		= SOCK_STREAM,
			int protocol 	= IPPROTO_TCP,
			int backlog 	= 10);


	/**
	 * Constructor for a new socket (for listening or connecting mode).
	 * @param domain socket domain (default: PF_INET)
	 * @param type socket type (default: SOCK_STREAM)
	 * @param protocol socket protocol (default: 0)
	 * @param backlog listen backlog (default: 10)
	 */
	csocket(csocket_owner *owner,
			int domain 	= PF_INET,
			int type 		= SOCK_STREAM,
			int protocol 	= IPPROTO_TCP,
			int backlog 	= 10);


	/**
	 * Destructor.
	 */
	virtual
	~csocket();

	/**
	 * Open socket in listening mode (server side).
	 *
	 * This opens a socket in listening mode bound to address 'la'
	 * with the specified socket parameters.
	 * @param la the local address for binding this socket
	 * @param domain the socket domain (default: PF_INET)
	 * @param type the socket type (default: SOCK_STREAM)	 * @param protocol protocol associated with this socket (default: 0 ="use the default protocol for the above socket type")
	 * @param backlog backlog value used for listen() system call (default: 10)
	 * @throw eSocketListenFailed failure in listen() system call
	 * @throw eSocketAddressInUse bind error while calling bind()
	 * @throw eSocketError thrown for all other socket related errors
	 */
	void 
	cpopen(
		caddress la,
		int domain = PF_INET, 
		int type = SOCK_STREAM, 
		int protocol = 0, 
		int backlog = 10,
		std::string devname = std::string("")) throw (eSocketError, eSocketListenFailed, eSocketAddressInUse);

	/**
	 * Open socket and connect to peer entity (client side).
	 *
	 * This opens a socket and connects to a peer entity.
	 * @param ra the remote address to connect to
	 * @param domain the socket domain (default: PF_INET)
	 * @param type the socket type (default: SOCK_STREAM)
	 * @param protocol protocol associated with this socket (default: 0 ="use the default protocol for the above socket type")
	 * @throw eSocketConnectFailed thrown if the connect() operation failed finally
	 * @throw eSocketError thrown for all other socket related errors
	 */
	void caopen(
		caddress ra, caddress la,
		int domain = PF_INET, 
		int type = SOCK_STREAM, 
		int protocol = 0) throw (eSocketError, eSocketConnectFailed);

	/**
	 * Closes this socket.
	 *
	 * Calls the shutdown() system call and closes the socket.
	 * Also deletes all packets queued in pout_squeue for transmission.
	 * After calling cclose() it is safe to call caopen() or cpopen() again.
	 */
	void cclose();

	/**
	 * Store a packet for transmission.
	 *
	 * This method stored the packet in the pout_squeue for transmission.
	 * If the socket is not connected and not a raw socket, the packet
	 * will be deleted and thus dropped.
	 * After pushing the packet pointer onto pout_squeue, the method registers
	 * the socket descriptor for a write operation and returns, giving the
	 * calling entity back control.
	 * @param pack the packet to store for transmission
	 */
	virtual void send_packet(cmemory *mem);

protected:


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
	virtual void
	handle_connected()
	{
		WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_connected()", this);
		if (socket_owner)
		{
			socket_owner->handle_connected(this, sd);
		}
	};

	/**
	 * Connect on socket failed (client mode).
	 *
	 * This notification method is called if the connect() operation fails
	 * on the socket. It should be overwritten by a derived class
	 * if the derived class wants to act upon this condition.
	 */
	virtual void
	handle_conn_refused()
	{
		WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_conn_refused()", this);
		if (socket_owner)
		{
			socket_owner->handle_connect_refused(this, sd);
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
	virtual void
	handle_accepted(int newsd, caddress const& ra)
	{
		WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_accepted()", this);
		if (socket_owner)
		{
			socket_owner->handle_accepted(this, newsd, ra);
		}
	};

	/**
	 * Socket was closed.
	 *
	 * This notification method is called when the socket is closed.
	 * @param sd the socket descriptor
	 */
	virtual void
	handle_closed(int sd)
	{
		deregister_filedesc_r(sd);
		WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_closed()", this);
		if (socket_owner)
		{
			socket_owner->handle_closed(this, sd);
		}
	};

	/**
	 * Read data from socket.
	 *
	 * This notification method is called from within csocket::handle_revent().
	 * A derived class should read a packet from the socket. This method
	 * must be overwritten by a derived class.
	 * @param fd the socket descriptor
	 */
	virtual void
	handle_read(int fd)
	{
		WRITELOG(CSOCKET, DBG, "csocket(%p)::handle_read()", this);
		if (socket_owner)
		{
			socket_owner->handle_read(this, sd);
		}
	};

	//
	// inherited from ciosrv
	//

	/**
	 * Handle read events on socket descriptor.
	 *
	 * Implemented by csocket, it either handles accept() in listening mode
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
	void dequeue_packet() throw (eSocketSendFailed, eSocketShortSend);
};

#endif
