#include "rofl/common/csocket.h"
#include "rofl/common/csocket_plain.h"
#include "rofl/common/csocket_openssl.h"

using namespace rofl;


csocket*
csocket::csocket_factory(
		enum csocket::socket_type_t socket_type, csocket_owner *owner)
{
	switch (socket_type) {
	case SOCKET_TYPE_PLAIN: {
		return new csocket_plain(owner);
	} break;
	case SOCKET_TYPE_OPENSSL: {
//		return new csocket_openssl(owner);
		return NULL;
	} break;
	default:
		throw eInval();
	}
}


