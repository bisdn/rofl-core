/*
 * ssl_lib.cc
 *
 *  Created on: Jan 22, 2014
 *      Author: tobi
 */

#include "ssl_lib.h"
#include "ssl_connection.h"

#include <assert.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

/* static*/bool rofl::ssl_lib::is_lib_initialized = false;

rofl::ssl_lib::ssl_lib(enum ssl_type type) :
		type(type), ssl_context(NULL) {
	init_lib();

	create_context();
}

rofl::ssl_lib::~ssl_lib() {
	// todo cleanup context
}

rofl::ssl_connection* rofl::ssl_lib::create_ssl_connection(int fd) {

	assert(NULL != ssl_context);

	SSL *ssl = SSL_new(ssl_context);

	if (NULL == ssl) {
		unsigned long ssl_error = ERR_get_error();
		ERR_error_string(ssl_error, NULL); // todo log
		// todo close fs
		return NULL;
	}

	SSL_set_fd(ssl, fd);

	return new ssl_connection(ssl);
}

void rofl::ssl_lib::init_lib() {
	if (not is_lib_initialized) {
		SSL_library_init(); /* load encryption & hash algorithms for SSL */
		SSL_load_error_strings(); /* load the error strings for good error reporting */

		is_lib_initialized = true;
	}
}

void rofl::ssl_lib::create_context() {
	ssl_context = SSL_CTX_new(TLSv1_2_method());

	if (NULL == ssl_context) {
		unsigned long ssl_error = ERR_get_error();
		ERR_error_string(ssl_error, NULL); // todo log
	}

	if (SSL_server == type) {
		/* Load server certificate into the SSL context */
		if (SSL_CTX_use_certificate_file(ssl_context, "dummy.pem", SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}

		/* Load the server private-key into the SSL context */
		if (SSL_CTX_use_PrivateKey_file(ssl_context, "", SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}

		/* no client certificate */
		SSL_CTX_set_verify(ssl_context, SSL_VERIFY_NONE, NULL);

#if 0 /* client cert disabled currtently */
		/* Load trusted CA. */
		if (!SSL_CTX_load_verify_locations(ssl_context, "", NULL)) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}

		/* Set to require peer (client) certificate verification */
		SSL_CTX_set_verify(ssl_context, SSL_VERIFY_PEER, verify_function);
		/* Set the verification depth to 1 */
		SSL_CTX_set_verify_depth(ssl_context, 1);
#endif
	} else {
		// client
		assert(SSL_client == type);
#if 0 /* client cert disabled currtently */
		/*----- Load a client certificate into the SSL_CTX structure -----*/
		if (SSL_CTX_use_certificate_file(ssl_context, "", SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}

		/*----- Load a private-key into the SSL_CTX structure -----*/
		if (SSL_CTX_use_PrivateKey_file(ssl_context, "", SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}
#endif

#if 0 /* todo check if we need addtional certs for verification */
		/* Load trusted CA. */
		if (!SSL_CTX_load_verify_locations(ssl_context, "", NULL)) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}
#endif

		/* Set to require peer (client) certificate verification */
		SSL_CTX_set_verify(ssl_context, SSL_VERIFY_NONE /* todo verfiy peer */, NULL /* built in default */);
		/* Set the verification depth to 1 */
		SSL_CTX_set_verify_depth(ssl_context, 1);
	}
}
