/*
 * ssl_connection.cc
 *
 *  Created on: Jan 23, 2014
 *      Author: tobi
 */

#include "ssl_connection.h"
#include "logging.h"

#include <openssl/err.h>

namespace rofl {

ssl_connection::ssl_connection(SSL *ssl) : ssl(ssl) {
}

ssl_connection::~ssl_connection() {
	// TODO disconnect ssl
}

} /* namespace rofl */

ssize_t rofl::ssl_connection::read(void* buffer, size_t size) {
	logging::error << __PRETTY_FUNCTION__ << std::endl;

	ssize_t err = SSL_read(ssl, buffer, size);
	if (0 >= err) {
		logging::error << " ssl_error=" << ERR_get_error() << ERR_error_string(ERR_get_error(), NULL) << std::endl;
	}
	return err;
}

ssize_t rofl::ssl_connection::write(const void* buffer, size_t size) {
	logging::error << __PRETTY_FUNCTION__ << std::endl;
	ssize_t err = SSL_write(ssl, buffer, size);

	if (0 >= err) {
		logging::error << " ssl_error=" << ERR_get_error() << ERR_error_string(ERR_get_error(), NULL) << std::endl;
	}

	return err;
}

int rofl::ssl_connection::handle_accept() {
	int sslerr = SSL_accept(ssl);

	if (1 == sslerr) {
		logging::debug << __PRETTY_FUNCTION__ << " accepted" << std::endl;
		return 0;
	}
	logging::error << " ssl_error=" << ERR_get_error() << ERR_error_string(ERR_get_error(), NULL) << std::endl;

	return ERR_get_error();
}

int rofl::ssl_connection::handle_connect() {
	int sslerr = SSL_connect(ssl);

	if (1 == sslerr) {
		return 0;
	}

	logging::error << " ssl_error=" << ERR_get_error() << ERR_error_string(ERR_get_error(), NULL) << std::endl;

	// todo handle err

	return ERR_get_error();
}
