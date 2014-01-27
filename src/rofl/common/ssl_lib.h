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

class ssl_lib {
public:

	enum ssl_type {
		SSL_client,
		SSL_server
	};

	ssl_lib(enum ssl_type type);
	~ssl_lib();

	ssl_connection *
	create_ssl_connection(int fd);


private:

	enum ssl_type type;
	static bool is_lib_initialized; /*!< library initialized indicator */
	SSL_CTX *ssl_context; /*!< ssl context for  */

	/**
	 * Initialize OpenSSL library
	 */
	void
	init_lib();

	void
	create_context();

};

} /* namespace rofl */

#endif /* SSL_LIB_H_ */
