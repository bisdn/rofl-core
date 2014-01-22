#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "crofconntest.h"
#include <stdlib.h>

CPPUNIT_TEST_SUITE_REGISTRATION( crofconnTest );

void
crofconnTest::setUp()
{
	srv_port = 6653;
	lsock = NULL;
	rsock = NULL;
	lconn = NULL;
	rconn = NULL;
}



void
crofconnTest::tearDown()
{

}



void
crofconnTest::testConnect()
{
	register_timer(TIMER_STOP, 10);

	rsock = new rofl::csocket(this, PF_INET, SOCK_STREAM, IPPROTO_TCP, 10);
	rsock->listen(rofl::caddress(AF_INET, "0.0.0.0", srv_port), PF_INET, SOCK_STREAM, IPPROTO_TCP);
	lsock = new rofl::csocket(this, PF_INET, SOCK_STREAM, IPPROTO_TCP, 10);
	lsock->connect(rofl::caddress(AF_INET, "127.0.0.1", srv_port), rofl::caddress(AF_INET, "0.0.0.0", 0), PF_INET, SOCK_STREAM, IPPROTO_TCP);

	rofl::ciosrv::run();

	delete rconn;
	delete lconn;
	delete lsock;
	delete rsock;
}



void
crofconnTest::handle_timeout(int opaque)
{
	switch (opaque) {
	case TIMER_STOP: {
		rofl::ciosrv::stop();
	} break;
	}
}



void
crofconnTest::handle_accepted(rofl::csocket *socket, int newsd, rofl::caddress const& ra)
{
	std::cerr << "accepted." << std::endl;
	rofl::openflow::cofhello_elem_versionbitmap versionbitmap;
	versionbitmap.add_ofp_version(rofl::openflow10::OFP_VERSION);
	versionbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
	versionbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
	rconn = new rofl::openflow::crofconn(this, 0, newsd, versionbitmap);
}

void
crofconnTest::handle_connected(rofl::csocket *socket, int sd)
{
	std::cerr << "connected." << std::endl;
	rofl::openflow::cofhello_elem_versionbitmap versionbitmap;
	versionbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
	versionbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
	lconn = new rofl::openflow::crofconn(this, 0, lsock->sd, versionbitmap);
}

void
crofconnTest::handle_connect_refused(rofl::csocket *socket, int sd)
{

}

void
crofconnTest::handle_read(rofl::csocket *socket, int sd)
{

}

void
crofconnTest::handle_closed(rofl::csocket *socket, int sd)
{

}

void
crofconnTest::handle_connected(rofl::openflow::crofconn *conn, uint8_t aux_id)
{

}

void
crofconnTest::handle_connect_refused(rofl::openflow::crofconn *conn)
{

}

void
crofconnTest::handle_closed(rofl::openflow::crofconn *conn)
{
	std::cerr << "handle_close() " << std::endl;
	rofl::ciosrv::stop();
}

void
crofconnTest::recv_message(rofl::openflow::crofconn *conn, rofl::cofmsg *msg)
{

}

uint32_t
crofconnTest::get_async_xid(rofl::openflow::crofconn *conn)
{
	return 10;
}

uint32_t
crofconnTest::get_sync_xid(rofl::openflow::crofconn *conn)
{
	return 20;
}

void
crofconnTest::release_sync_xid(rofl::openflow::crofconn *conn, uint32_t xid)
{

}


