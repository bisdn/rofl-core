/*
 * ssl_connection.h
 *
 *  Created on: Jan 23, 2014
 *      Author: tobi
 */

#ifndef SSL_CONNECTION_H_
#define SSL_CONNECTION_H_

#include <openssl/ssl.h>

namespace rofl {

class ssl_connection {
public:
	ssl_connection(SSL *ssl);
	~ssl_connection();

	int
	handle_accept();

	int
	handle_connect();

	ssize_t
	read(void *buffer, size_t size);

	ssize_t
	write(const void *buffer, size_t size);

private:
	SSL *ssl;

	int
	handle_error(int err);
};

} /* namespace rofl */

#endif /* SSL_CONNECTION_H_ */
