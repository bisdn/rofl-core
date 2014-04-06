/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/csocket.h"
#include "rofl/common/csocket_impl.h"
#ifdef ROFL_HAVE_OPENSSL
#include "rofl/common/csocket_openssl.h"
#endif

using namespace rofl;

//Keys
std::string const 	csocket::PARAM_KEY_DO_RECONNECT("do-reconnect");
std::string const 	csocket::PARAM_KEY_REMOTE_HOSTNAME("remote-hostname");
std::string const 	csocket::PARAM_KEY_REMOTE_PORT("remote-port");			// "6653"
std::string const 	csocket::PARAM_KEY_LOCAL_HOSTNAME("local-hostname");
std::string const 	csocket::PARAM_KEY_LOCAL_PORT("local-port");			// "0"
std::string const	csocket::PARAM_KEY_DOMAIN("domain"); 					// "inet", "inet6"
std::string const	csocket::PARAM_KEY_TYPE("type");						// "stream", "dgram"
std::string const	csocket::PARAM_KEY_PROTOCOL("protocol");				// "tcp", "udp"

//Socket type specific keys
std::string const	csocket::PARAM_SSL_KEY_CA_PATH("ca-path");
std::string const	csocket::PARAM_SSL_KEY_CA_FILE("ca-file");
std::string const	csocket::PARAM_SSL_KEY_CERT("cert");
std::string const	csocket::PARAM_SSL_KEY_PRIVATE_KEY("key");
std::string const	csocket::PARAM_SSL_KEY_PRIVATE_KEY_PASSWORD("password");

//Values (non-numeric)
std::string const 	csocket::PARAM_DOMAIN_VALUE_INET_ANY("inet-any");
std::string const 	csocket::PARAM_DOMAIN_VALUE_INET("inet");
std::string const 	csocket::PARAM_DOMAIN_VALUE_INET6("inet6");
std::string const 	csocket::PARAM_TYPE_VALUE_STREAM("stream");
std::string const 	csocket::PARAM_TYPE_VALUE_DGRAM("dgram");
std::string const 	csocket::PARAM_PROTOCOL_VALUE_TCP("tcp");
std::string const 	csocket::PARAM_PROTOCOL_VALUE_UDP("udp");



csocket*
csocket::csocket_factory(
		enum csocket::socket_type_t socket_type, csocket_owner *owner)
{
	switch (socket_type) {
	case SOCKET_TYPE_PLAIN: {
		return new csocket_impl(owner);
	} break;
#ifdef ROFL_HAVE_OPENSSL
	case SOCKET_TYPE_OPENSSL: {
		return new csocket_openssl(owner);
	} break;
#endif
	default:
		throw eSocketTypeNotFound();
	}
}



cparams
csocket::get_params(
		enum socket_type_t socket_type)
{
	switch (socket_type) {
	case SOCKET_TYPE_PLAIN: {
		return csocket_impl::get_params();
	} break;
#ifdef ROFL_HAVE_OPENSSL
	case SOCKET_TYPE_OPENSSL: {
		return csocket_openssl::get_params();
	} break;
#endif
	default:
		throw eSocketTypeNotFound();
	}

}

bool
csocket::supports_socket_type(enum socket_type_t socket_type)
{
	switch (socket_type) {

		case SOCKET_TYPE_PLAIN: {
			return true;
		} break;

#ifdef ROFL_HAVE_OPENSSL
		case SOCKET_TYPE_OPENSSL: {
			return true; 
		} break;
#endif

		default:
			return false;
	}

}


