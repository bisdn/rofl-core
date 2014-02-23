/*
 * ssl_connection.cc
 *
 *  Created on: Jan 23, 2014
 *      Author: tobi
 */

#include "ssl_connection.h"
#include "logging.h"

#include <assert.h>
#include <openssl/err.h>

namespace rofl {

ssl_connection::ssl_connection(SSL *ssl) :
		ssl(ssl) {
}

ssl_connection::~ssl_connection() {
	SSL_free(ssl);
}

ssize_t ssl_connection::read(void* buffer, size_t size) {
	logging::error << __PRETTY_FUNCTION__ << std::endl;
	ssize_t err = SSL_read(ssl, buffer, size);
	return handle_error(err);
}

ssize_t ssl_connection::write(const void* buffer, size_t size) {
	logging::error << __PRETTY_FUNCTION__ << std::endl;
	ssize_t err = SSL_write(ssl, buffer, size);
	return handle_error(err);
}

int ssl_connection::handle_accept() {
	int sslerr = SSL_accept(ssl);

	if (1 == sslerr) {
		logging::info << __PRETTY_FUNCTION__ << " accepted" << std::endl;
		return 0;
	}
	return handle_error(sslerr);
}

int ssl_connection::handle_connect() {
	int sslerr = SSL_connect(ssl);

	if (1 == sslerr) {
		logging::info << __PRETTY_FUNCTION__ << " connected" << std::endl;
		return 0;
	}
	return handle_error(sslerr);
}

int ssl_connection::handle_error(int err) {
	if (0 >= err) {
		if (NULL != ssl) {
			err = SSL_get_error(ssl, err);
			logging::error << __PRETTY_FUNCTION__ << " SSL_get_error=" << err << std::endl;

			switch (err) {
			case SSL_ERROR_WANT_READ:
				logging::info << __PRETTY_FUNCTION__ << " SSL_ERROR_WANT_READ" << std::endl;
				return -SSL_ERROR_WANT_READ;
				break;

			case SSL_ERROR_WANT_WRITE:
				logging::info << __PRETTY_FUNCTION__ << " SSL_ERROR_WANT_WRITE" << std::endl;
				return -SSL_ERROR_WANT_WRITE;
				break;

			case SSL_ERROR_WANT_X509_LOOKUP:
			case SSL_ERROR_SYSCALL:/* look at error stack/return value/errno */
			case SSL_ERROR_ZERO_RETURN:
			case SSL_ERROR_WANT_CONNECT:
			case SSL_ERROR_WANT_ACCEPT:
			case SSL_ERROR_SSL:
				logging::error << __PRETTY_FUNCTION__ << " unhandled error" << std::endl;
				break;
			case SSL_ERROR_NONE:
				/* no error handling necessary */
				break;
			default:
				logging::error << __PRETTY_FUNCTION__ << " unknown error" << std::endl;
				break;
			}


		} else {
			logging::error << " ERR_get_error=" << ERR_get_error() << " " << ERR_error_string(ERR_get_error(), NULL)
					<< std::endl;
		}
	}
	return err;
}

} /* namespace rofl */
