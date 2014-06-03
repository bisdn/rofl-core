#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgmetermod_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmsgmetermod_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmsgmetermod_test::setUp()
{
}



void
cofmsgmetermod_test::tearDown()
{
}



void
cofmsgmetermod_test::testDefaultConstructor()
{
	rofl::openflow::cofmsg_meter_mod msg(rofl::openflow13::OFP_VERSION);
	CPPUNIT_ASSERT(rofl::openflow13::OFP_VERSION == msg.get_version());
}



void
cofmsgmetermod_test::testCopyConstructor()
{
	uint8_t of_version = rofl::openflow13::OFP_VERSION;
	uint32_t xid = 0xd1d2d3d4;
	uint16_t command = 0xa1a2;
	uint16_t flags = 0xb1b2;
	uint32_t meter_id = 0xc1c2c3c4;

	rofl::openflow::cofmsg_meter_mod msg(of_version);
	msg.set_xid(xid);
	msg.set_command(command);
	msg.set_flags(flags);
	msg.set_meter_id(meter_id);

	msg.set_meter_bands().set_meter_band_drop(0);
	msg.set_meter_bands().set_meter_band_drop(1);
	msg.set_meter_bands().set_meter_band_dscp_remark(0).set_prec_level(0xf1);
	msg.set_meter_bands().set_meter_band_dscp_remark(1).set_prec_level(0xf2);
#ifdef DEBUG
	std::cerr << "msg:" << std::endl << msg;
#endif

	rofl::openflow::cofmsg_meter_mod clone(msg);

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
#endif

	CPPUNIT_ASSERT(xid == clone.get_xid());
	CPPUNIT_ASSERT(command == clone.get_command());
	CPPUNIT_ASSERT(flags == clone.get_flags());
	CPPUNIT_ASSERT(meter_id == clone.get_meter_id());
	CPPUNIT_ASSERT(4 == clone.get_meter_bands().get_num_of_mbs());
}





