#include "rofl/common/csocket.h"
#include "rofl/common/csocket_plain.h"
#ifdef HAVE_OPENSSL
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
std::string const	csocket::PARAM_SSL_CA_PATH("capath");
std::string const	csocket::PARAM_SSL_CA_FILE("cafile");
std::string const	csocket::PARAM_SSL_CERT("cert");
std::string const	csocket::PARAM_SSL_KEY("key");

//Values (non-numeric)
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
		return new csocket_plain(owner);
	} break;
#ifdef HAVE_OPENSSL
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
		return csocket_plain::get_params();
	} break;
#ifdef HAVE_OPENSSL
	case SOCKET_TYPE_OPENSSL: {
		return csocket_openssl::get_params();
	} break;
#endif
	default:
		throw eSocketTypeNotFound();
	}

}

bool
csocket::supports_socket_type(enum socket_type_t socket_type) const
{
	switch (socket_type) {

		case SOCKET_TYPE_PLAIN: {
			return true;
		} break;

#ifdef HAVE_OPENSSL
		case SOCKET_TYPE_OPENSSL: {
			return true; 
		} break;
#endif

		default:
			return false;
	}

}


