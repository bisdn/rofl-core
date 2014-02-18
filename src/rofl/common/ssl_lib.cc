/*
 * ssl_lib.cc
 *
 *  Created on: Jan 22, 2014
 *      Author: tobi
 */

#include "ssl_lib.h"
#include "ssl_connection.h"
#include "logging.h"

#include <assert.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

rofl::ssl_context::ssl_context(enum ssl_type type, const std::string &cert_and_key_file) :
		type(type) {

	ssl_ctx = SSL_CTX_new(TLSv1_2_method());

	if (NULL == ssl_ctx) {
		logging::error << " ssl_error=" << ERR_get_error() << ERR_error_string(ERR_get_error(), NULL);
	}
	assert(NULL != ssl_ctx);

	if (ssl_context::SSL_server == type) {

		if (cert_and_key_file.empty()) {
			throw RoflException("no cert_and_key_file");
		}

		logging::debug << __PRETTY_FUNCTION__ << " create SSL_server" << std::endl;
		/* Load server certificate into the SSL context */
		if (SSL_CTX_use_certificate_file(ssl_ctx, cert_and_key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
			logging::error << " ssl_error=" << ERR_get_error() << ERR_error_string(ERR_get_error(), NULL);
			exit(1);
		}

		/* Load the server private-key into the SSL context */
		if (SSL_CTX_use_PrivateKey_file(ssl_ctx, cert_and_key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
			logging::error << " ssl_error=" << ERR_get_error() << ERR_error_string(ERR_get_error(), NULL);
			exit(1);
		}

		/* no client certificate */
		SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL);

#if 0 /* client cert disabled currtently */
		/* Load trusted CA. */
		if (!SSL_CTX_load_verify_locations(ssl_ctx, "", NULL)) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}

		/* Set to require peer (client) certificate verification */
		SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, verify_function);
		/* Set the verification depth to 1 */
		SSL_CTX_set_verify_depth(ssl_ctx, 1);
#endif
	} else {
		logging::debug << __PRETTY_FUNCTION__ << " create SSL_client" << std::endl;
		// client
		assert(SSL_client == type);
#if 0 /* client cert disabled currtently */
		/*----- Load a client certificate into the SSL_CTX structure -----*/
		if (SSL_CTX_use_certificate_file(ssl_ctx, "", SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}

		/*----- Load a private-key into the SSL_CTX structure -----*/
		if (SSL_CTX_use_PrivateKey_file(ssl_ctx, "", SSL_FILETYPE_PEM) <= 0) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}
#endif

#if 0 /* todo check if we need addtional certs for verification */
		/* Load trusted CA. */
		if (!SSL_CTX_load_verify_locations(ssl_ctx, "", NULL)) {
			ERR_print_errors_fp(stderr);
			exit(1);
		}
#endif

		/* Set to require peer (client) certificate verification */
		SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE /* todo verfiy peer */, NULL /* built in default */);
		/* Set the verification depth to 1 */
		SSL_CTX_set_verify_depth(ssl_ctx, 1);
	}
}

rofl::ssl_context::~ssl_context() {
	SSL_CTX_free(ssl_ctx);
}

rofl::ssl_connection* rofl::ssl_context::create_ssl_connection(int fd) {

	assert(NULL != ssl_ctx);

	SSL *ssl = SSL_new(ssl_ctx);

	if (NULL == ssl) {
		logging::error << " ssl_error=" << ERR_get_error() << ERR_error_string(ERR_get_error(), NULL);

		// todo close fd?
		return NULL;
	}

	SSL_set_fd(ssl, fd);

	return new ssl_connection(ssl);
}

/* static */rofl::ssl_lib&
rofl::ssl_lib::get_instance() {
	static ssl_lib instance;
	return instance;
}

rofl::ssl_context* rofl::ssl_lib::create_ssl_context(enum ssl_context::ssl_type type, const std::string &cert_and_key_file) {
	return new ssl_context(type, cert_and_key_file);
}

rofl::ssl_lib::ssl_lib() {
	SSL_library_init(); /* load encryption & hash algorithms for SSL */
	SSL_load_error_strings(); /* load the error strings for good error reporting */

}

rofl::ssl_lib::~ssl_lib() {
}

