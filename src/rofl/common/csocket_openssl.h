/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CSOCKET_OPENSSL_H
#define CSOCKET_OPENSSL_H

#include <set>
#include <list>
#include <bitset>
#include <stdio.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "rofl/common/csocket.h"
#include "rofl/common/csocket_impl.h"
#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class eOpenSSL 		: public	RoflException {
	std::string error;
public:
	eOpenSSL(std::string const& error) : error(error) {};
	friend std::ostream& operator<< (std::ostream& os, eOpenSSL const& e) {
		os << "EXCEPTION: <eOpenSSL error: " << e.error << " >" << std::endl;
		return os;
	};
};


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
	public csocket_impl
{
	//Defaults
	static std::string const	PARAM_DEFAULT_VALUE_SSL_KEY_CA_PATH;
	static std::string const	PARAM_DEFAULT_VALUE_SSL_KEY_CA_FILE;
	static std::string const	PARAM_DEFAULT_VALUE_SSL_KEY_CERT;
	static std::string const	PARAM_DEFAULT_VALUE_SSL_KEY_PRIVATE_KEY;
	static std::string const	PARAM_DEFAULT_VALUE_SSL_KEY_PRIVATE_KEY_PASSWORD;


	static bool ssl_initialized;

	/**
	 * @brief	Initialize SSL library internals.
	 */
	static void
	openssl_init();

	/**
	 * @brief	OpenSSL password callback.
	 */
	static int
	openssl_password_callback(char *buf, int size, int rwflag, void *userdata);

	static std::set<csocket_openssl*> openssl_sockets;

	/*
	 * OpenSSL related structures
	 */
	SSL_CTX						*ctx;
	SSL_METHOD					*method;
	SSL							*ssl;
	BIO							*bio;

	/*
	 * socket parameters
	 */
	std::string					capath;
	std::string					cafile;
	std::string					certfile;
	std::string 				keyfile;
	std::string					password;

	enum openssl_flag_t {
		FLAG_SSL_IDLE			= 0,
		FLAG_SSL_CONNECTING		= 1,
		FLAG_SSL_ESTABLISHED	= 2,
		FLAG_SSL_CLOSING		= 3,
	};

	std::bitset<64>				socket_flags;

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
	 * @param params parameters for this socket
	 */
	virtual void
	listen(
		cparams const& params);


	/**
	 * @brief 	Handle accepted socket descriptor obtained from external listening socket
	 */
	virtual void
	accept(int sd, cparams const& socket_params);



	/**
	 * @brief	Open socket and connect to peer entity (client side).
	 *
	 * This opens a socket and connects to a peer entity.
	 *
	 * @param params parameters for this socket
	 */
	virtual void
	connect(
		cparams const& params);


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
	 *
	 */
	static cparams
	get_default_params();


public:

	/**
	 *
	 */
	void
	set_capath(std::string const& capath) { this->capath = capath; };

	/**
	 *
	 */
	std::string const&
	get_capath() const { return capath; };

	/**
	 *
	 */
	void
	set_cafile(std::string const& cafile) { this->cafile = cafile; };

	/**
	 *
	 */
	std::string const&
	get_cafile() const { return cafile; };

	/**
	 *
	 */
	void
	set_certfile(std::string const& certfile) { this->certfile = certfile; };

	/**
	 *
	 */
	std::string const&
	get_certfile() const { return certfile; };

	/**
	 *
	 */
	void
	set_keyfile(std::string const& keyfile) { this->keyfile = keyfile; };

	/**
	 *
	 */
	std::string const&
	get_keyfile() const { return keyfile; };

	/**
	 *
	 */
	void
	set_password(std::string const& password) { this->password = password; };

	/**
	 *
	 */
	std::string const&
	get_password() const { return password; };





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
	handle_connected();

	/**
	 * Connect on socket failed (client mode).
	 *
	 * This notification method is called if the connect() operation fails
	 * on the socket. It should be overwritten by a derived class
	 * if the derived class wants to act upon this condition.
	 */
	virtual void
	handle_conn_refused();

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
	handle_accepted(
			int newsd,
			caddress const& ra);

	/**
	 * Socket was closed.
	 *
	 * This notification method is called when the socket is closed.
	 * @param sd the socket descriptor
	 */
	virtual void
	handle_closed();

	/**
	 * Read data from socket.
	 *
	 * This notification method is called from within csocket_impl::handle_revent().
	 * A derived class should read data from the socket. This method
	 * must be overwritten by a derived class.
	 * @param fd the socket descriptor
	 */
	virtual void
	handle_read();

	/**
	 * Write data to socket.
	 *
	 * This notification method is called from within csocket_impl::handle_revent().
	 * A derived class should write data to the socket. This method
	 * must be overwritten by a derived class.
	 * @param fd the socket descriptor
	 */
	virtual void
	handle_write();



private:

	/**
	 *
	 */
	void
	openssl_init_ctx();

	/**
	 *
	 */
	void
	openssl_destroy_ctx();

	/**
	 *
	 */
	void
	openssl_init_ssl();

	/**
	 *
	 */
	void
	openssl_destroy_ssl();


protected:


	/**
	 * Send packets in outgoing queue pout_squeue.
	 *
	 * This method transmits all pending packets from the transmission
	 * queue pout_squeue.
	 */
	virtual void
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
