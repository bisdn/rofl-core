/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "csocket_openssl.h"

using namespace rofl;

/*static*/std::set<csocket_openssl*> csocket_openssl::openssl_sockets;

//Defaults
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CA_PATH(".");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CA_FILE("ca.pem");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_CERT("cert.pem");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_PRIVATE_KEY("key.pem");
std::string const	csocket_openssl::PARAM_DEFAULT_VALUE_SSL_KEY_PRIVATE_KEY_PASSWORD("");

/*static*/cparams
csocket_openssl::get_default_params()
{
	cparams p;
	p.add_param(csocket::PARAM_KEY_DO_RECONNECT);
	p.add_param(csocket::PARAM_KEY_REMOTE_HOSTNAME);
	p.add_param(csocket::PARAM_KEY_LOCAL_HOSTNAME);
	p.add_param(csocket::PARAM_KEY_DOMAIN);
	p.add_param(csocket::PARAM_KEY_TYPE);
	p.add_param(csocket::PARAM_KEY_PROTOCOL);
	p.add_param(csocket::PARAM_SSL_KEY_CA_PATH);
	p.add_param(csocket::PARAM_SSL_KEY_CA_FILE);
	p.add_param(csocket::PARAM_SSL_KEY_CERT);
	p.add_param(csocket::PARAM_SSL_KEY_PRIVATE_KEY);
	p.add_param(csocket::PARAM_SSL_KEY_PRIVATE_KEY_PASSWORD);
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
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	BIO_new_fp(stderr, BIO_NOCLOSE);

	ssl_initialized = true;
}


csocket_openssl::csocket_openssl(
		csocket_owner *owner) :
				csocket_impl(owner, rofl::csocket::SOCKET_TYPE_OPENSSL),
				ctx(NULL),
				method(NULL),
				ssl(NULL),
				bio(NULL)
{
	openssl_sockets.insert(this);

	logging::debug << "[rofl][csocket][openssl] constructor:" << std::endl << *this;

	socket_flags.set(FLAG_SSL_IDLE);

	csocket_openssl::openssl_init();

	openssl_init_ctx();

	pthread_rwlock_init(&pout_squeue_lock, 0);
}



csocket_openssl::~csocket_openssl()
{
	logging::debug << "[rofl][csocket][openssl] destructor:" << std::endl << *this;
	close();

	openssl_destroy_ssl();

	openssl_destroy_ctx();

	pthread_rwlock_destroy(&pout_squeue_lock);

	openssl_sockets.erase(this);
}



void
csocket_openssl::openssl_init_ctx()
{
	ctx = SSL_CTX_new(SSLv23_client_method());

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

	// capath/cafile
	if (!SSL_CTX_load_verify_locations(ctx,
			cafile.empty() ? NULL : cafile.c_str(),
			capath.empty() ? NULL : capath.c_str())) {
		throw eOpenSSL("[rofl][csocket][openssl][init-ctx] unable to load ca locations");
	}


	SSL_CTX_set_verify_depth(ctx, 1);

	// TODO: get random numbers
}



void
csocket_openssl::openssl_destroy_ctx()
{
	SSL_CTX_free(ctx);
}



void
csocket_openssl::openssl_init_ssl()
{
	if ((ssl = SSL_new(ctx)) == NULL) {
		throw eOpenSSL("[rofl][csocket][openssl][init-ssl] unable to create new SSL object");
	}

	if ((bio = BIO_new(BIO_s_socket())) == NULL) {
		throw eOpenSSL("[rofl][csocket][openssl][init-ssl] unable to create new BIO object");
	}

	BIO_set_fd(bio, sd, BIO_NOCLOSE);

	SSL_set_bio(ssl, /*rbio*/bio, /*wbio*/bio);
}



void
csocket_openssl::openssl_destroy_ssl()
{
	if (bio) {
		BIO_free(bio); bio = NULL;
	}

	if (ssl) {
		SSL_free(ssl); ssl = NULL;
	}
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

	csocket_impl::listen(socket_params);
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

	this->sd = sd;

	csocket_openssl::handle_accepted(sd);
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

	csocket_impl::connect(socket_params);
}



void
csocket_openssl::handle_connected()
{
	int rc = 0;

	socket_flags.reset(FLAG_SSL_ESTABLISHED);

	openssl_init_ssl();

	socket_flags.set(FLAG_SSL_CONNECTING);

	if ((rc = SSL_connect(ssl)) <= 0) {
		switch (SSL_get_error(ssl, rc)) {
		case SSL_ERROR_WANT_READ: {
			// wait for next data from peer
		} return;
		case SSL_ERROR_WANT_WRITE: {
			register_filedesc_w(sd);
		} return;
		default:
			openssl_destroy_ssl();
			throw eOpenSSL("[rofl][csocket][openssl][handle-connected] SSL_connect() failed");
		}
	} else
	if (rc == 0) {

		openssl_destroy_ssl();
		throw eOpenSSL("[rofl][csocket][openssl][handle-connected] SSL_connect() failed");

	} else
	if (rc == 1) {

		socket_flags.reset(FLAG_SSL_CONNECTING);
		socket_flags.set(FLAG_SSL_ESTABLISHED);
		csocket_impl::handle_connected();
	}
}



void
csocket_openssl::handle_conn_refused()
{
	csocket_impl::handle_conn_refused();
}



void
csocket_openssl::handle_accepted(int newsd)
{
	int rc = 0;

	this->sd = newsd;

	socket_flags.reset(FLAG_SSL_ESTABLISHED);

	openssl_init_ssl();

	socket_flags.set(FLAG_SSL_ACCEPTING);

	if ((rc = SSL_accept(ssl)) < 0) {
		switch (SSL_get_error(ssl, rc)) {
		case SSL_ERROR_WANT_READ: {
			// wait for next data from peer
		} return;
		case SSL_ERROR_WANT_WRITE: {
			register_filedesc_w(sd);
		} return;
		default:
			openssl_destroy_ssl();
			throw eOpenSSL("[rofl][csocket][openssl][handle-accepted] SSL_accept() failed");
		}
	} else
	if (rc == 0) {

		openssl_destroy_ssl();
		throw eOpenSSL("[rofl][csocket][openssl][handle-accepted] SSL_accept() failed");

	} else
	if (rc == 1) {

		socket_flags.reset(FLAG_SSL_ACCEPTING);
		socket_flags.set(FLAG_SSL_ESTABLISHED);
		csocket_impl::handle_accepted(newsd);
	}
}



void
csocket_openssl::handle_closed()
{
	csocket_impl::handle_closed();
}



void
csocket_openssl::handle_read()
{
	int rc = 0;

	if (socket_flags.test(FLAG_SSL_IDLE)) {
		return; // do nothing

	} else
	if (socket_flags.test(FLAG_SSL_CONNECTING)) {

		if ((rc = SSL_connect(ssl)) <= 0) {
			switch (SSL_get_error(ssl, rc)) {
			case SSL_ERROR_WANT_READ: {
				// wait for next data from peer
			} return;
			case SSL_ERROR_WANT_WRITE: {
				register_filedesc_w(sd);
			} return;
			default:
				throw eOpenSSL("[rofl][csocket][openssl][handle-read] SSL_connect() failed");
			}
		} else
		if (rc == 0) {

			openssl_destroy_ssl();
			throw eOpenSSL("[rofl][csocket][openssl][handle-read] SSL_connect() failed");

		} else
		if (rc == 1) {

			socket_flags.reset(FLAG_SSL_CONNECTING);
			socket_flags.set(FLAG_SSL_ESTABLISHED);
			csocket_impl::handle_connected();
		}

	} else
	if (socket_flags.test(FLAG_SSL_ACCEPTING)) {

		if ((rc = SSL_accept(ssl)) < 0) {
			switch (SSL_get_error(ssl, rc)) {
			case SSL_ERROR_WANT_READ: {
				// wait for next data from peer
			} return;
			case SSL_ERROR_WANT_WRITE: {
				register_filedesc_w(sd);
			} return;
			default:
				openssl_destroy_ssl();
				throw eOpenSSL("[rofl][csocket][openssl][handle-read] SSL_accept() failed");
			}
		} else
		if (rc == 0) {

			openssl_destroy_ssl();
			throw eOpenSSL("[rofl][csocket][openssl][handle-read] SSL_accept() failed");

		} else
		if (rc == 1) {

			socket_flags.reset(FLAG_SSL_ACCEPTING);
			socket_flags.set(FLAG_SSL_ESTABLISHED);
			csocket_impl::handle_accepted(sd);
		}

	} else
	if (socket_flags.test(FLAG_SSL_ESTABLISHED)) {

		csocket_impl::handle_read(); // call socket owner => results in a call to this->recv()

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
			// wait for next data from peer
		} return rc;
		case SSL_ERROR_WANT_WRITE: {
			register_filedesc_w(sd);
		} return rc;
		default:
			openssl_destroy_ssl();
			throw eOpenSSL("[rofl][csocket][openssl][handle-read] SSL_read() failed");
		}
	}

	return rc;
}



void
csocket_openssl::handle_write()
{
	int rc = 0;

	if (socket_flags.test(FLAG_SSL_IDLE)) {
		return; // do nothing

	} else
	if (socket_flags.test(FLAG_SSL_CONNECTING)) {

		if ((rc = SSL_connect(ssl)) <= 0) {
			switch (SSL_get_error(ssl, rc)) {
			case SSL_ERROR_WANT_READ: {
				// wait for next data from peer
			} return;
			case SSL_ERROR_WANT_WRITE: {
				register_filedesc_w(sd);
			} return;
			default:
				openssl_destroy_ssl();
				throw eOpenSSL("[rofl][csocket][openssl][handle-write] SSL_connect() failed");
			}
		}

		socket_flags.reset(FLAG_SSL_CONNECTING);

		socket_flags.set(FLAG_SSL_ESTABLISHED);

		csocket_impl::handle_connected();

	} else
	if (socket_flags.test(FLAG_SSL_ESTABLISHED)) {

		dequeue_packet();
	}
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
				rofl::logging::warn << "[rofl][csocket][openssl] resending due to short write: " << std::endl << entry;
				had_short_write = false;
			}

			if ((rc = SSL_write(ssl,
					entry.mem->somem() + entry.msg_bytes_sent,
					entry.mem->memlen() - entry.msg_bytes_sent)) <= 0) {

				switch (SSL_get_error(ssl, rc)) {
				case SSL_ERROR_WANT_READ: {
					// wait for next data from peer
				} return;
				case SSL_ERROR_WANT_WRITE: {
					register_filedesc_w(sd);
				} return;
				default:
					rofl::logging::error << "[rofl][csocket][openssl][handle-write] SSL_connect() failed" << std::endl;
					goto out;
				}

			} else
			if ((((unsigned int)(rc + entry.msg_bytes_sent)) < entry.mem->memlen())) {

				if (SOCK_STREAM == type) {
					had_short_write = true;
					entry.msg_bytes_sent += rc;
					rofl::logging::warn << "[rofl][csocket][openssl] short write on socket descriptor:" << sd << ", retrying..." << std::endl << entry;
				} else {
					rofl::logging::warn << "[rofl][csocket][openssl] short write on socket descriptor:" << sd << ", dropping packet." << std::endl;
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



void
csocket_openssl::reconnect()
{
	if (not sockflags.test(FLAG_ACTIVE_SOCKET)) {
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

	socket_flags.reset(FLAG_SSL_ESTABLISHED);
	socket_flags.set(FLAG_SSL_CLOSING);

	if ((rc = SSL_shutdown(ssl)) < 0) {

		switch (SSL_get_error(ssl, rc)) {
		case SSL_ERROR_WANT_READ: {
			// wait for next data from peer
		} return;
		case SSL_ERROR_WANT_WRITE: {
			register_filedesc_w(sd);
		} return;
		default:
			openssl_destroy_ssl();
			csocket_impl::close();
			throw eOpenSSL("[rofl][csocket][openssl][close] SSL_shutdown() failed");
		}

	} else
	if (rc == 0) {

		if ((rc = SSL_shutdown(ssl)) < 0) {
			switch (SSL_get_error(ssl, rc)) {
			case SSL_ERROR_WANT_READ: {
				// wait for next data from peer
			} return;
			case SSL_ERROR_WANT_WRITE: {
				register_filedesc_w(sd);
			} return;
			default:
				openssl_destroy_ssl();
				csocket_impl::close();
				throw eOpenSSL("[rofl][csocket][openssl][close] SSL_shutdown() failed");
			}
		}

		openssl_destroy_ssl();
		socket_flags.reset(FLAG_SSL_CLOSING);
		socket_flags.set(FLAG_SSL_IDLE);
		csocket_impl::close();

	} else
	if (rc == 1) {

		openssl_destroy_ssl();
		socket_flags.reset(FLAG_SSL_CLOSING);
		socket_flags.set(FLAG_SSL_IDLE);
		csocket_impl::close();
	}
}



