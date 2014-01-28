/*
 * ssl_lib.h
 *
 *  Created on: Jan 22, 2014
 *      Author: tobi
 */

#ifndef SSL_LIB_H_
#define SSL_LIB_H_

#include <openssl/ssl.h>

namespace rofl {

class ssl_connection;

class ssl_context {
public:
	enum ssl_type {
		SSL_client, SSL_server
	};

	ssl_context(enum ssl_type type);

	~ssl_context();

	ssl_connection *
	create_ssl_connection(int fd);

private:
	enum ssl_type type;
	SSL_CTX *ssl_ctx; /*!< ssl context for  */
};

class ssl_lib {
public:

	static ssl_lib&
	get_instance();

	ssl_context *
	create_ssl_context(enum ssl_context::ssl_type type);

private:

	ssl_lib();
	~ssl_lib();
};

} /* namespace rofl */

#endif /* SSL_LIB_H_ */
