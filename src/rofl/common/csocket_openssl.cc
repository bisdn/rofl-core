/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "csocket_openssl.h"

using namespace rofl;

/*static*/std::set<csocket_openssl*> csocket_openssl::openssl_sockets;

//Defaults
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CA_PATH("");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CA_FILE("ca.pem");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CERT("cert.pem");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_PRIVATE_KEY("key.pem");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_PRIVATE_KEY_PASSWORD("");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_VERIFY_MODE("PEER"); // NONE|PEER
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_VERIFY_DEPTH("1");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CIPHERS("EECDH+ECDSA+AESGCM EECDH+aRSA+AESGCM EECDH+ECDSA+SHA256 EECDH+aRSA+RC4 EDH+aRSA EECDH RC4 !aNULL !eNULL !LOW !3DES !MD5 !EXP !PSK !SRP !DSS");

/*static*/cparams
csocket_openssl::get_default_params()
{
	cparams p = rofl::csocket_impl::get_default_params();
	p.add_param(csocket::PARAM_SSL_KEY_CA_PATH).set_string(csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CA_PATH);
	p.add_param(csocket::PARAM_SSL_KEY_CA_FILE).set_string(csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CA_FILE);
	p.add_param(csocket::PARAM_SSL_KEY_CERT).set_string(csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CERT);
	p.add_param(csocket::PARAM_SSL_KEY_PRIVATE_KEY).set_string(csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_PRIVATE_KEY);
	p.add_param(csocket::PARAM_SSL_KEY_PRIVATE_KEY_PASSWORD).set_string(csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_PRIVATE_KEY_PASSWORD);
	p.add_param(csocket::PARAM_SSL_KEY_VERIFY_MODE).set_string(csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_VERIFY_MODE);
	p.add_param(csocket::PARAM_SSL_KEY_VERIFY_DEPTH).set_string(csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_VERIFY_DEPTH);
	p.add_param(csocket::PARAM_SSL_KEY_CIPHERS).set_string(csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CIPHERS);
	return p;
}

bool csocket_openssl::ssl_initialized = false;

void
csocket_openssl::openssl_init()
{
	if (ssl_initialized)
		return;

	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_ERR_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	BIO_new_fp(stderr, BIO_NOCLOSE);

	ssl_initialized = true;
}


csocket_openssl::csocket_openssl(
		csocket_owner *owner) :
				csocket(owner, rofl::csocket::SOCKET_TYPE_OPENSSL),
				socket(this),
				ctx(NULL),
				method(NULL),
				ssl(NULL),
				bio(NULL)
{
	openssl_sockets.insert(this);

	logging::debug << "[rofl][csocket][openssl] constructor:" << std::endl << *this;

	socket_flags.set(FLAG_SSL_IDLE);

	csocket_openssl::openssl_init();

	pthread_rwlock_init(&ssl_lock, 0);
}



csocket_openssl::~csocket_openssl()
{
	logging::debug << "[rofl][csocket][openssl] destructor:" << std::endl << *this;
	close();

	openssl_destroy_ssl();

	pthread_rwlock_destroy(&ssl_lock);

	openssl_sockets.erase(this);
}



void
csocket_openssl::openssl_init_ctx()
{
	ctx = SSL_CTX_new(SSLv23_method());

	// certificate
	if (!SSL_CTX_use_certificate_file(ctx, certfile.c_str(), SSL_FILETYPE_PEM)) {
		throw eOpenSSL("[rofl][csocket][openssl][init-ctx] unable to read certfile:"+certfile);
	}

	// private key
	SSL_CTX_set_default_passwd_cb(ctx, &csocket_openssl::openssl_password_callback);
	SSL_CTX_set_default_passwd_cb_userdata(ctx, (void*)this);

	if (!SSL_CTX_use_PrivateKey_file(ctx, keyfile.c_str(), SSL_FILETYPE_PEM)) {
		throw eOpenSSL("[rofl][csocket][openssl][init-ctx] unable to read keyfile:"+keyfile);
	}

	// ciphers
	if ((not ciphers.empty()) && (0 == SSL_CTX_set_cipher_list(ctx, ciphers.c_str()))) {
		throw eOpenSSL("[rofl][csocket][openssl][init-ctx] unable to set ciphers:"+ciphers);
	}

	// capath/cafile
	if (!SSL_CTX_load_verify_locations(ctx,
			cafile.empty() ? NULL : cafile.c_str(),
			capath.empty() ? NULL : capath.c_str())) {
		throw eOpenSSL("[rofl][csocket][openssl][init-ctx] unable to load ca locations");
	}

	int mode = SSL_VERIFY_NONE;
	if (verify_mode == "NONE") {
		mode = SSL_VERIFY_NONE;
	} else
	if (verify_mode == "PEER") {
		mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
	}

	SSL_CTX_set_verify(ctx, mode, NULL); // TODO: verify callback


	int depth; std::istringstream( verify_depth ) >> depth;

	SSL_CTX_set_verify_depth(ctx, depth);

	// TODO: get random numbers
}



void
csocket_openssl::openssl_destroy_ctx()
{
	if (ctx) {
		SSL_CTX_free(ctx); ctx = NULL;
	}
}



void
csocket_openssl::openssl_init_ssl()
{
	openssl_init_ctx();

	if ((ssl = SSL_new(ctx)) == NULL) {
		throw eOpenSSL("[rofl][csocket][openssl][init-ssl] unable to create new SSL object");
	}

	if ((bio = BIO_new(BIO_s_socket())) == NULL) {
		throw eOpenSSL("[rofl][csocket][openssl][init-ssl] unable to create new BIO object");
	}

	BIO_set_fd(bio, socket.get_sd(), BIO_NOCLOSE);

	SSL_set_bio(ssl, /*rbio*/bio, /*wbio*/bio);

	if (socket_flags.test(FLAG_SSL_CONNECTING)) {
		SSL_set_connect_state(ssl);
	} else
	if (socket_flags.test(FLAG_SSL_ACCEPTING)) {
		SSL_set_accept_state(ssl);
	}
}



void
csocket_openssl::openssl_destroy_ssl()
{
	if (ssl) {
		SSL_free(ssl); ssl = NULL; bio = NULL;
	}

	openssl_destroy_ctx();
}



int
csocket_openssl::openssl_password_callback(char *buf, int size, int rwflag, void *userdata)
{
	csocket_openssl *socket = static_cast<csocket_openssl*>(userdata);
	if ((NULL == socket) ||
			(csocket_openssl::openssl_sockets.find(socket) ==
					csocket_openssl::openssl_sockets.end())) {
		throw eOpenSSL("[rofl][csocket][openssl][password-callback] unable to find socket object");
	}

	// use rwflag?

	if (socket->get_password().empty()) {
		return 0;
	}

	strncpy(buf, socket->get_password().c_str(), size);

	return strlen(buf);
}



void
csocket_openssl::listen(
		cparams const& socket_params)
{
	this->socket_params = socket_params;

	capath 		= socket_params.get_param(PARAM_SSL_KEY_CA_PATH).get_string();
	cafile 		= socket_params.get_param(PARAM_SSL_KEY_CA_FILE).get_string();
	certfile	= socket_params.get_param(PARAM_SSL_KEY_CERT).get_string();
	keyfile		= socket_params.get_param(PARAM_SSL_KEY_PRIVATE_KEY).get_string();
	password	= socket_params.get_param(PARAM_SSL_KEY_PRIVATE_KEY_PASSWORD).get_string();
	verify_mode	= socket_params.get_param(PARAM_SSL_KEY_VERIFY_MODE).get_string();
	verify_depth= socket_params.get_param(PARAM_SSL_KEY_VERIFY_DEPTH).get_string();
	ciphers		= socket_params.get_param(PARAM_SSL_KEY_CIPHERS).get_string();

	socket.listen(socket_params);
}



void
csocket_openssl::accept(
		cparams const& socket_params, int sd)
{
	this->socket_params = socket_params;

	capath 		= socket_params.get_param(PARAM_SSL_KEY_CA_PATH).get_string();
	cafile 		= socket_params.get_param(PARAM_SSL_KEY_CA_FILE).get_string();
	certfile	= socket_params.get_param(PARAM_SSL_KEY_CERT).get_string();
	keyfile		= socket_params.get_param(PARAM_SSL_KEY_PRIVATE_KEY).get_string();
	password	= socket_params.get_param(PARAM_SSL_KEY_PRIVATE_KEY_PASSWORD).get_string();
	verify_mode	= socket_params.get_param(PARAM_SSL_KEY_VERIFY_MODE).get_string();
	verify_depth= socket_params.get_param(PARAM_SSL_KEY_VERIFY_DEPTH).get_string();
	ciphers		= socket_params.get_param(PARAM_SSL_KEY_CIPHERS).get_string();

	socket.accept(socket_params, sd);
}



void
csocket_openssl::handle_accepted(rofl::csocket& socket)
{
	socket_flags.reset(FLAG_SSL_IDLE);
	socket_flags.reset(FLAG_SSL_ESTABLISHED);
	socket_flags.set(FLAG_SSL_ACCEPTING);

	openssl_init_ssl();

	openssl_accept();
}



void
csocket_openssl::handle_accept_refused(rofl::csocket& socket)
{
	socket.close();

	if (socket_owner) socket_owner->handle_accept_refused(*this);
}



void
csocket_openssl::connect(
		cparams const& socket_params)
{
	this->socket_params = socket_params;

	capath 		= socket_params.get_param(PARAM_SSL_KEY_CA_PATH).get_string();
	cafile 		= socket_params.get_param(PARAM_SSL_KEY_CA_FILE).get_string();
	certfile	= socket_params.get_param(PARAM_SSL_KEY_CERT).get_string();
	keyfile		= socket_params.get_param(PARAM_SSL_KEY_PRIVATE_KEY).get_string();
	password	= socket_params.get_param(PARAM_SSL_KEY_PRIVATE_KEY_PASSWORD).get_string();
	verify_mode	= socket_params.get_param(PARAM_SSL_KEY_VERIFY_MODE).get_string();
	verify_depth= socket_params.get_param(PARAM_SSL_KEY_VERIFY_DEPTH).get_string();
	ciphers		= socket_params.get_param(PARAM_SSL_KEY_CIPHERS).get_string();

	socket_flags.set(FLAG_ACTIVE_SOCKET);

	socket.connect(socket_params);
}



void
csocket_openssl::handle_connected(rofl::csocket& socket)
{
	socket_flags.reset(FLAG_SSL_IDLE);
	socket_flags.reset(FLAG_SSL_ESTABLISHED);
	socket_flags.set(FLAG_SSL_CONNECTING);

	openssl_init_ssl();

	openssl_connect();
}



void
csocket_openssl::handle_connect_refused(rofl::csocket& socket)
{
	socket.close();

	if (socket_owner) socket_owner->handle_connect_refused(*this);
}



void
csocket_openssl::handle_listen(rofl::csocket& socket, int newsd)
{
	if (socket_owner) socket_owner->handle_listen(*this, newsd);
}



void
csocket_openssl::handle_closed(rofl::csocket& socket)
{
	if (socket_owner) socket_owner->handle_closed(*this);
}



void
csocket_openssl::handle_read(rofl::csocket& socket)
{
	if (socket_flags.test(FLAG_SSL_IDLE)) {
		return; // do nothing

	} else
	if (socket_flags.test(FLAG_SSL_CONNECTING)) {

		openssl_connect();

	} else
	if (socket_flags.test(FLAG_SSL_ACCEPTING)) {

		openssl_accept();

	} else
	if (socket_flags.test(FLAG_SSL_ESTABLISHED)) {

		if (socket_owner) socket_owner->handle_read(*this); // call socket owner => results in a call to this->recv()

	} else
	if (socket_flags.test(FLAG_SSL_CLOSING)) {

		csocket_openssl::close();
	}
}



ssize_t
csocket_openssl::recv(void* buf, size_t count)
{
	int rc = 0;

	if ((rc = SSL_read(ssl, buf, count)) <= 0) {
		switch (SSL_get_error(ssl, rc)) {
		case SSL_ERROR_WANT_READ: {
			rofl::logging::debug << "[rofl][csocket][openssl][recv] receiving => SSL_ERROR_WANT_READ" << std::endl;
		} return rc;
		case SSL_ERROR_WANT_WRITE: {
			rofl::logging::debug << "[rofl][csocket][openssl][recv] receiving => SSL_ERROR_WANT_WRITE" << std::endl;
		} return rc;
		default:
			openssl_destroy_ssl();
			throw eOpenSSL("[rofl][csocket][openssl][handle-read] SSL_read() failed");
		}
	}

	// if there is more data to read, wake-up socket-owner again
	if (SSL_pending(ssl) > 0) {
		notify(EVENT_RECV_RXQUEUE);
	}

	return rc;
}



void
csocket_openssl::handle_write(rofl::csocket& socket)
{
	if (socket_flags.test(FLAG_SSL_IDLE)) {
		return; // do nothing

	} else
	if (socket_flags.test(FLAG_SSL_ACCEPTING)) {

		openssl_accept();

	} else
	if (socket_flags.test(FLAG_SSL_CONNECTING)) {

		openssl_connect();

	} else
	if (socket_flags.test(FLAG_SSL_ESTABLISHED)) {

		dequeue_packet();
	}
}



void
csocket_openssl::send(cmemory *mem, caddress const& dest)
{
	RwLock lock(&ssl_lock, RwLock::RWLOCK_WRITE);

	txqueue.push_back(mem);

	notify(cevent(EVENT_SEND_TXQUEUE));
}



void
csocket_openssl::handle_event(cevent const& e)
{
	switch (e.get_cmd()) {
	case EVENT_SEND_TXQUEUE: {
		dequeue_packet();
	} break;
	case EVENT_RECV_RXQUEUE: {
		if (socket_owner) socket_owner->handle_read(*this); // call socket owner => results in a call to this->recv()
	} break;
	default:
		csocket::handle_event(e);
	}
}


void
csocket_openssl::dequeue_packet()
{
	RwLock lock(&ssl_lock, RwLock::RWLOCK_WRITE);

	while (not txqueue.empty()) {

		int rc = 0, err_code = 0;

		rofl::cmemory *mem = txqueue.front();

		if ((rc = SSL_write(ssl, mem->somem(), mem->memlen())) < 0) {

			switch (err_code = SSL_get_error(ssl, rc)) {
			case SSL_ERROR_WANT_READ: {
				rofl::logging::debug << "[rofl][csocket][openssl][dequeue] sending => SSL_ERROR_WANT_READ" << std::endl;
				notify(EVENT_SEND_TXQUEUE);
			} return;
			case SSL_ERROR_WANT_WRITE: {
				rofl::logging::debug << "[rofl][csocket][openssl][dequeue] sending => SSL_ERROR_WANT_WRITE" << std::endl;
				notify(EVENT_SEND_TXQUEUE);
			} return;
			default: {
			};
			}

		}

		delete mem; txqueue.pop_front();
	}
}



void
csocket_openssl::reconnect()
{
	if (not socket_flags.test(FLAG_ACTIVE_SOCKET)) {
		throw eInval();
	}
	close();
	connect(socket_params);
}



void
csocket_openssl::close()
{
	int rc = 0;

	logging::info << "[rofl][csocket][openssl] close()" << std::endl;

	if (socket_flags.test(FLAG_SSL_IDLE))
		return;

	socket_flags.reset(FLAG_SSL_ESTABLISHED);
	socket_flags.set(FLAG_SSL_CLOSING);

	if (NULL == ssl)
		return;

	if ((rc = SSL_shutdown(ssl)) < 0) {

		switch (SSL_get_error(ssl, rc)) {
		case SSL_ERROR_WANT_READ: {
			rofl::logging::debug << "[rofl][csocket][openssl][close] closing => SSL_ERROR_WANT_READ" << std::endl;
		} return;
		case SSL_ERROR_WANT_WRITE: {
			rofl::logging::debug << "[rofl][csocket][openssl][close] closing => SSL_ERROR_WANT_WRITE" << std::endl;
		} return;
		default:
			openssl_destroy_ssl();
			socket.close();
			throw eOpenSSL("[rofl][csocket][openssl][close] SSL_shutdown() failed");
		}

	} else
	if (rc == 0) {

		if ((rc = SSL_shutdown(ssl)) < 0) {
			switch (SSL_get_error(ssl, rc)) {
			case SSL_ERROR_WANT_READ: {
				rofl::logging::debug << "[rofl][csocket][openssl][close] closing => SSL_ERROR_WANT_READ" << std::endl;
			} return;
			case SSL_ERROR_WANT_WRITE: {
				rofl::logging::debug << "[rofl][csocket][openssl][close] closing => SSL_ERROR_WANT_WRITE" << std::endl;
			} return;
			default:
				openssl_destroy_ssl();
				socket.close();
				throw eOpenSSL("[rofl][csocket][openssl][close] SSL_shutdown() failed");
			}
		}

		openssl_destroy_ssl();
		socket_flags.reset(FLAG_SSL_CLOSING);
		socket_flags.set(FLAG_SSL_IDLE);
		socket.close();

	} else
	if (rc == 1) {


		openssl_destroy_ssl();
		socket_flags.reset(FLAG_SSL_CLOSING);
		socket_flags.set(FLAG_SSL_IDLE);
		socket.close();
	}
}



void
csocket_openssl::openssl_accept()
{
	int rc = 0, err_code = 0;

	rofl::logging::debug << "[rofl][csocket][openssl][accept] SSL accepting..." << std::endl;

	if ((rc = SSL_accept(ssl)) < 0) {
		switch (err_code = SSL_get_error(ssl, rc)) {
		case SSL_ERROR_WANT_READ: {
			// wait for next data from peer
			rofl::logging::debug << "[rofl][csocket][openssl][accept] accepting => SSL_ERROR_WANT_READ" << std::endl;
		} return;
		case SSL_ERROR_WANT_WRITE: {
			rofl::logging::debug << "[rofl][csocket][openssl][accept] accepting => SSL_ERROR_WANT_WRITE" << std::endl;
		} return;
		case SSL_ERROR_WANT_ACCEPT: {
			rofl::logging::debug << "[rofl][csocket][openssl][accept] accepting => SSL_ERROR_WANT_ACCEPT" << std::endl;
		} return;
		case SSL_ERROR_WANT_CONNECT: {
			rofl::logging::debug << "[rofl][csocket][openssl][accept] accepting => SSL_ERROR_WANT_CONNECT" << std::endl;
		} return;


		case SSL_ERROR_NONE: {
			rofl::logging::debug << "[rofl][csocket][openssl][accept] SSL_accept() failed SSL_ERROR_NONE" << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		case SSL_ERROR_SSL: {
			rofl::logging::debug << "[rofl][csocket][openssl][accept] SSL_accept() failed SSL_ERROR_SSL" << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		case SSL_ERROR_SYSCALL: {
			rofl::logging::debug << "[rofl][csocket][openssl][accept] SSL_accept() failed SSL_ERROR_SYSCALL" << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		case SSL_ERROR_ZERO_RETURN: {
			rofl::logging::debug << "[rofl][csocket][openssl][accept] SSL_accept() failed SSL_ERROR_ZERO_RETURN" << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		default: {
			rofl::logging::debug << "[rofl][csocket][openssl][accept] SSL_accept() failed " << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		}

	} else
	if (rc == 0) {

		rofl::logging::debug << "[rofl][csocket][openssl][accept] SSL_accept() failed " << std::endl;

		ERR_print_errors_fp(stderr);

		openssl_destroy_ssl();
		socket.close();
		socket_flags.reset(FLAG_SSL_ACCEPTING);
		socket_flags.set(FLAG_SSL_IDLE);

		if (socket_owner) socket_owner->handle_accept_refused(*this);

	} else
	if (rc == 1) {

		if (not openssl_verify_ok()) {
			rofl::logging::debug << "[rofl][csocket][openssl][accept] SSL_accept() peer verification failed " << std::endl;

			ERR_print_errors_fp(stderr);

			openssl_destroy_ssl();
			socket.close();
			socket_flags.reset(FLAG_SSL_ACCEPTING);
			socket_flags.set(FLAG_SSL_IDLE);

			if (socket_owner) socket_owner->handle_accept_refused(*this);

			return;
		}

		rofl::logging::debug << "[rofl][csocket][openssl][accept] SSL_accept() succeeded " << std::endl;

		socket_flags.reset(FLAG_SSL_ACCEPTING);
		socket_flags.set(FLAG_SSL_ESTABLISHED);

		if (socket_owner) socket_owner->handle_accepted(*this);
	}
}



void
csocket_openssl::openssl_connect()
{
	int rc = 0, err_code = 0;

	rofl::logging::debug << "[rofl][csocket][openssl][connect] SSL connecting..." << std::endl;

	if ((rc = SSL_connect(ssl)) <= 0) {
		switch (err_code = SSL_get_error(ssl, rc)) {
		case SSL_ERROR_WANT_READ: {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] connecting => SSL_ERROR_WANT_READ" << std::endl;
		} return;
		case SSL_ERROR_WANT_WRITE: {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] connecting => SSL_ERROR_WANT_WRITE" << std::endl;
		} return;
		case SSL_ERROR_WANT_ACCEPT: {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] connecting => SSL_ERROR_WANT_ACCEPT" << std::endl;
		} return;
		case SSL_ERROR_WANT_CONNECT: {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] connecting => SSL_ERROR_WANT_CONNECT" << std::endl;
		} return;


		case SSL_ERROR_NONE: {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] SSL_connect() failed SSL_ERROR_NONE" << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		case SSL_ERROR_SSL: {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] SSL_connect() failed SSL_ERROR_SSL" << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		case SSL_ERROR_SYSCALL: {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] SSL_connect() failed SSL_ERROR_SYSCALL" << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		case SSL_ERROR_ZERO_RETURN: {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] SSL_connect() failed SSL_ERROR_ZERO_RETURN" << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		default: {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] SSL_connect() failed " << std::endl;
			ERR_print_errors_fp(stderr);
		} return;
		}

	} else
	if (rc == 0) {

		rofl::logging::debug << "[rofl][csocket][openssl][connect] SSL_connect() failed " << std::endl;
		ERR_print_errors_fp(stderr);

		openssl_destroy_ssl();
		socket.close();
		socket_flags.reset(FLAG_SSL_CONNECTING);
		socket_flags.set(FLAG_SSL_IDLE);

		if (socket_owner) socket_owner->handle_connect_refused(*this);

	} else
	if (rc == 1) {

		if (not openssl_verify_ok()) {
			rofl::logging::debug << "[rofl][csocket][openssl][connect] SSL_connect() peer verification failed " << std::endl;

			ERR_print_errors_fp(stderr);

			openssl_destroy_ssl();
			socket.close();
			socket_flags.reset(FLAG_SSL_CONNECTING);
			socket_flags.set(FLAG_SSL_IDLE);

			if (socket_owner) socket_owner->handle_connect_refused(*this);

			return;
		}

		rofl::logging::debug << "[rofl][csocket][openssl][connect] SSL_connect() succeeded " << std::endl;

		socket_flags.reset(FLAG_SSL_CONNECTING);
		socket_flags.set(FLAG_SSL_ESTABLISHED);

		if (socket_owner) socket_owner->handle_connected(*this);
	}
}



bool
csocket_openssl::openssl_verify_ok()
{
	/* strategy:
	 * - always check peer certificate in client mode
	 * - check peer certificate in server mode when explicitly enabled (mode == SSL_VERIFY_PEER)
	 */
	if ((verify_mode == "PEER") || socket_flags.test(FLAG_SSL_CONNECTING)) {

		/*
		 * there must be a certificate presented by the peer in mode SSL_VERIFY_PEER
		 */
		X509* cert = (X509*)NULL;
		if ((cert = SSL_get_peer_certificate(ssl)) == NULL) {

			rofl::logging::debug << "[rofl][csocket][openssl][verify] peer verification failed " << std::endl;

			openssl_destroy_ssl();
			socket.close();
			socket_flags.reset(FLAG_SSL_ACCEPTING);
			socket_flags.set(FLAG_SSL_IDLE);

			if (socket_owner) socket_owner->handle_accept_refused(*this);

			return false;
		}
		/*
		 * check verification result
		 */
		long result = 0;
		if ((result = SSL_get_verify_result(ssl)) != X509_V_OK) {

			switch (result) {
			case X509_V_OK: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: ok" << std::endl;
			} break;
			case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: unable to get issuer certificate" << std::endl;
			} break;
			case X509_V_ERR_UNABLE_TO_GET_CRL: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: unable to get certificate CRL" << std::endl;
			} break;
			case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: unable to decrypt certificate's signature" << std::endl;
			} break;
			case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: unable to decrypt CRL's signature" << std::endl;
			} break;
			case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: unable to decode issuer public key" << std::endl;
			} break;
			case X509_V_ERR_CERT_SIGNATURE_FAILURE: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: certificate signature failure" << std::endl;
			} break;
			case X509_V_ERR_CRL_SIGNATURE_FAILURE: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: CRL signature failure" << std::endl;
			} break;
			case X509_V_ERR_CERT_NOT_YET_VALID: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: certificate is not yet valid" << std::endl;
			} break;
			case X509_V_ERR_CERT_HAS_EXPIRED: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: certificate has expired" << std::endl;
			} break;
			case X509_V_ERR_CRL_NOT_YET_VALID: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: CRL is not yet valid" << std::endl;
			} break;
			case X509_V_ERR_CRL_HAS_EXPIRED: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: CRL has expired" << std::endl;
			} break;
			case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: format error in certificate's notBefore field" << std::endl;
			} break;
			case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: format error in certificate's notAfter field" << std::endl;
			} break;
			case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: format error in CRL's lastUpdate field" << std::endl;
			} break;
			case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: format error in CRL's nextUpdate field" << std::endl;
			} break;
			case X509_V_ERR_OUT_OF_MEM: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: out of memory" << std::endl;
			} break;
			case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: self signed certificate" << std::endl;
			} break;
			case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: self signed certificate in certificate chain" << std::endl;
			} break;
			case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: unable to get local issuer certificate" << std::endl;
			} break;
			case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: unable to verify the first certificate" << std::endl;
			} break;
			case X509_V_ERR_CERT_CHAIN_TOO_LONG: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: certificate chain too long" << std::endl;
			} break;
			case X509_V_ERR_CERT_REVOKED: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: certificate revoked" << std::endl;
			} break;
			case X509_V_ERR_INVALID_CA: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: invalid CA certificate" << std::endl;
			} break;
			case X509_V_ERR_PATH_LENGTH_EXCEEDED: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: path length constraint exceeded" << std::endl;
			} break;
			case X509_V_ERR_INVALID_PURPOSE: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: unsupported certificate purpose" << std::endl;
			} break;
			case X509_V_ERR_CERT_UNTRUSTED: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: certificate not trusted" << std::endl;
			} break;
			case X509_V_ERR_CERT_REJECTED: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: certificate rejected" << std::endl;
			} break;
			case X509_V_ERR_SUBJECT_ISSUER_MISMATCH: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: subject issuer mismatch" << std::endl;
			} break;
			case X509_V_ERR_AKID_SKID_MISMATCH: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: authority and subject key identifier mismatch" << std::endl;
			} break;
			case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: authority and issuer serial number mismatch" << std::endl;
			} break;
			case X509_V_ERR_KEYUSAGE_NO_CERTSIGN: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: key usage does not include certificate signing" << std::endl;
			} break;
			case X509_V_ERR_APPLICATION_VERIFICATION: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: application verification failure" << std::endl;
			} break;
			default: {
				rofl::logging::info << "[rofl][csocket][openssl][verify] SSL certificate verification: unknown error" << std::endl;
			};
			}

			return false;
		}
	}

	return true;
}
