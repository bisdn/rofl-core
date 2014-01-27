/*
 * ssl_connection.cc
 *
 *  Created on: Jan 23, 2014
 *      Author: tobi
 */

#include "ssl_connection.h"

namespace rofl {

ssl_connection::ssl_connection(SSL *ssl) : ssl(ssl) {
}

ssl_connection::~ssl_connection() {
	// TODO disconnect ssl
}

} /* namespace rofl */

ssize_t rofl::ssl_connection::read(void* buffer, size_t size) {
	ssize_t err = SSL_read(ssl, buffer, size);
	return err;
}

ssize_t rofl::ssl_connection::write(const void* buffer, size_t size) {
	ssize_t err = SSL_write(ssl, buffer, size);
	return err;
}

int rofl::ssl_connection::handle_accept() {
	int sslerr = SSL_accept(ssl);

	if (1 == sslerr) {
		return 0;
	}

	sslerr = SSL_get_error(ssl, sslerr);

	// todo handle err

	return sslerr;
}

int rofl::ssl_connection::handle_connect() {
	int sslerr = SSL_connect(ssl);

	if (1 == sslerr) {
		return 0;
	}

	sslerr = SSL_get_error(ssl, sslerr);

	// todo handle err

	return sslerr;
}
