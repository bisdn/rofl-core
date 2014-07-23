/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/ciosrv.h"
#include "rofl/common/crofconn.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/csocket.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class crofconnTest : public CppUnit::TestFixture, public rofl::csocket_owner, public rofl::openflow::crofconn_env, public rofl::ciosrv {

	CPPUNIT_TEST_SUITE( crofconnTest );
	CPPUNIT_TEST( testConnect );
	CPPUNIT_TEST_SUITE_END();

private:

	rofl::openflow::crofconn 	*lconn;
	rofl::openflow::crofconn	*rconn;
	rofl::csocket 				*lsock;			// left-side socket
	rofl::csocket 				*rsock;			// right-side socket
	uint16_t 					srv_port;

public:
	void setUp();
	void tearDown();

	void testConnect();

private:

	enum crofconnTest_timer_t {
		TIMER_STOP = 1,
	};

	virtual void
	handle_timeout(int opaque);

private:

	virtual void
	handle_accepted(rofl::csocket *socket, int newsd, rofl::caddress const& ra);

	virtual void
	handle_connected(rofl::csocket *socket, int sd);

	virtual void
	handle_connect_refused(rofl::csocket *socket, int sd);

	virtual void
	handle_read(rofl::csocket *socket, int sd);

	virtual void
	handle_closed(rofl::csocket *socket, int sd);

private:

	virtual void
	handle_connected(rofl::openflow::crofconn *conn, uint8_t aux_id);

	virtual void
	handle_connect_refused(rofl::openflow::crofconn *conn);

	virtual void
	handle_closed(rofl::openflow::crofconn *conn);

	virtual void
	recv_message(rofl::openflow::crofconn *conn, rofl::cofmsg *msg);

	virtual uint32_t
	get_async_xid(rofl::openflow::crofconn *conn);

	virtual uint32_t
	get_sync_xid(rofl::openflow::crofconn *conn);

	virtual void
	release_sync_xid(rofl::openflow::crofconn *conn, uint32_t xid);
};

