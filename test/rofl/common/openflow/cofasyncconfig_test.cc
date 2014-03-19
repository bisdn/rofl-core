#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofasyncconfig_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofasyncconfig_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofasyncconfig_test::setUp()
{
}



void
cofasyncconfig_test::tearDown()
{
}



void
cofasyncconfig_test::testDefaultConstructor()
{
	rofl::openflow::cofasync_config ac;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == ac.get_version());
}



void
cofasyncconfig_test::testCopyConstructor()
{
	rofl::openflow::cofasync_config ac(rofl::openflow13::OFP_VERSION);

	ac.set_packet_in_mask_master(0x11121314);
	ac.set_packet_in_mask_slave(0x21222324);
	ac.set_port_status_mask_master(0x31323334);
	ac.set_port_status_mask_slave(0x41424344);
	ac.set_flow_removed_mask_master(0x51525354);
	ac.set_flow_removed_mask_slave(0x61626364);


	rofl::cmemory mac(ac.length());
	ac.pack(mac.somem(), mac.memlen());

#ifdef DEBUG
	std::cerr << "ac:" << std::endl << ac;
	std::cerr << "mac:" << std::endl << mac;
#endif

	rofl::openflow::cofasync_config clone(ac);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(mac == mclone);
}




void
cofasyncconfig_test::testPackUnpack()
{
	/*
	 * pack
	 */
	rofl::openflow::cofasync_config ac(rofl::openflow13::OFP_VERSION);

	ac.set_packet_in_mask_master(0x11121314);
	ac.set_packet_in_mask_slave(0x21222324);
	ac.set_port_status_mask_master(0x31323334);
	ac.set_port_status_mask_slave(0x41424344);
	ac.set_flow_removed_mask_master(0x51525354);
	ac.set_flow_removed_mask_slave(0x61626364);

	rofl::cmemory mac(ac.length());
	ac.pack(mac.somem(), mac.memlen());

	struct rofl::openflow::cofasync_config::async_config_t *async = (struct rofl::openflow::cofasync_config::async_config_t*)mac.somem();

#ifdef DEBUG
	std::cerr << "ac:" << std::endl << ac;
	std::cerr << "mac:" << std::endl << mac;
#endif

	CPPUNIT_ASSERT(mac.memlen() == sizeof(struct rofl::openflow::cofasync_config::async_config_t));
	CPPUNIT_ASSERT(be32toh(async->packet_in_mask[0]) == 0x11121314);
	CPPUNIT_ASSERT(be32toh(async->packet_in_mask[1]) == 0x21222324);
	CPPUNIT_ASSERT(be32toh(async->port_status_mask[0]) == 0x31323334);
	CPPUNIT_ASSERT(be32toh(async->port_status_mask[1]) == 0x41424344);
	CPPUNIT_ASSERT(be32toh(async->flow_removed_mask[0]) == 0x51525354);
	CPPUNIT_ASSERT(be32toh(async->flow_removed_mask[1]) == 0x61626364);

	/*
	 * unpack
	 */

	rofl::openflow::cofasync_config bc(rofl::openflow13::OFP_VERSION);

	bc.unpack(mac.somem(), mac.memlen());

#ifdef DEBUG
	std::cerr << "mac:" << std::endl << mac;
	std::cerr << "bc:" << std::endl << bc;
#endif

	CPPUNIT_ASSERT(bc.get_packet_in_mask_master() == 0x11121314);
	CPPUNIT_ASSERT(bc.get_packet_in_mask_slave() == 0x21222324);
	CPPUNIT_ASSERT(bc.get_port_status_mask_master() == 0x31323334);
	CPPUNIT_ASSERT(bc.get_port_status_mask_slave() == 0x41424344);
	CPPUNIT_ASSERT(bc.get_flow_removed_mask_master() == 0x51525354);
	CPPUNIT_ASSERT(bc.get_flow_removed_mask_slave() == 0x61626364);
}

